#include "stdafx.h"
#include "Cloyan.h"

#include "Player.h"
#include "Trail_Cloyan.h"
#include "Boss_Hpbar_BackUI.h"
#include "Boss_Hpbar_GreenUI.h"
#include "Boss_Hpbar_YellowUI.h"
#include "Boss_Hpbar_RedUI.h"
#include "Boss_NamingScene.h"
#include "Boss_Hpbar_FontUI.h"
#include "Ahglan_FontName.h"

#include "Export_Function.h"
#include "Export_Utility.h"

USING(Engine)

CCloyan::CCloyan(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CCloyan::~CCloyan()
{
}


HRESULT CCloyan::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(SCALE_MANKIND * 1.75f, SCALE_MANKIND * 1.75f, SCALE_MANKIND * 1.75f);

	m_pTransformCom->Update_Component(0.f);

	m_tInfo.iHp = 250000;
	m_tInfo.iMaxHp = m_tInfo.iHp;
	m_iLineHp = _int(m_tInfo.iMaxHp * 0.1f);
	m_iMaxLineHp = m_iLineHp;

	m_bBoss = true;

	return S_OK;
}

HRESULT CCloyan::LateReady_Object()
{
	CGameObject::LateReady_Object();

	Load_ColInfo();
	Add_NaviMesh();

	m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pTrail = dynamic_cast<CTrail_Cloyan*>(Engine::Get_GameObject(L"GameLogic", L"Cloyan_Trail"));
	NULL_CHECK_RETURN(m_pTrail, E_FAIL);

	m_pUILayer = CLayer::Create();
	NULL_CHECK_RETURN(m_pUILayer, E_FAIL);

	return S_OK;
}

_int CCloyan::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	if (m_bDead)
		return iExit;

	m_fTimeDelta = fTimeDelta;

	Make_TrailEffect(fTimeDelta);

	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		m_pNaviMeshCom->Set_CellIndex(Compute_InCell());
		Movement();
		Animation_Control();
		Collision_Control();
		Update_UI();
		Update_State();
		MoveOn_Skill();
		RotationOn_Skill();
		Dissolve(fTimeDelta);
		

		m_pMeshCom->Set_AnimationIndex(m_iAniIndex);

		Engine::Add_RenderGroup(RENDER_NONALPHA, this);
	}

	return iExit;
}

_int CCloyan::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	if (!m_mapColliderCom.empty())
	{
		map<const wstring, CCollider*>::iterator	iter = m_mapColliderCom.begin();

		for (; iter != m_mapColliderCom.end(); ++iter)
		{
			iter->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());
		}
	}
	if (!m_mapBoxColliderCom.empty())
	{
		map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		for (; iter != m_mapBoxColliderCom.end(); ++iter)
		{
			iter->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());
		}
	}

	return iExit;
}

void CCloyan::Render_Object()
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		if (m_bAnimation)
			m_pMeshCom->Play_Animation(m_fTimeDelta);

		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

		LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
		pEffect->AddRef();

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		_uint iMaxPass = 0;

		pEffect->Begin(&iMaxPass, NULL);		// 1인자 : 현재 쉐이더 파일이 반환하는 pass의 최대 개수
												// 2인자 : 시작하는 방식을 묻는 FLAG
		pEffect->BeginPass(0);

		m_pMeshCom->Render_Meshes(pEffect);

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}
}

void CCloyan::Set_Enable(_vec3 vPos, _vec3 vRotate)
{
	m_pTransformCom->Set_Pos(&vPos);
	m_pTransformCom->RotationFromOriginAngle(ROT_X, vRotate.x);
	m_pTransformCom->RotationFromOriginAngle(ROT_Y, vRotate.y);
	m_pTransformCom->RotationFromOriginAngle(ROT_Z, vRotate.z);

	m_pNaviMeshCom->Set_CellIndex(Compute_InCell());
	//m_pTransformCom->Rotation(ROT_Y, -90.f);

	m_tInfo.iHp = m_tInfo.iMaxHp;

	m_iAniIndex = CLOYAN_SPAWN;

	Animation_Control();
}

HRESULT CCloyan::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CDynamicMesh*>(Engine::Clone_Prototype(L"Proto_Mesh_Cloyan"));
	NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Texture_Dissolve
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Prototype(L"Proto_Texture_Dissolve"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Texture_Dissolve", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_Ahglan"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CCloyan::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fDissolveValue", m_fDissolveValue);

	m_pTextureCom->Set_Texture(pEffect, "g_DissolveTexture", 0);

	const D3DLIGHT9*	pLightInfo = Get_Light();
	NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));

	return S_OK;
}

void CCloyan::Movement()
{
	if (m_pPlayer)
	{
		if (0.f >= m_pPlayer->Get_TagPlayerInfo().tagInfo.iHp)
		{
			m_iAniIndex = CLOYAN_IDLE;
		}
		else
		{
			m_pPlayerTrans = static_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC));

			m_vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
			if (m_pPlayerTrans)
				m_vPlayerPos = *m_pPlayerTrans->Get_Info(INFO_POS);
			m_fDistance = D3DXVec3Length(&(m_vMyPos - m_vPlayerPos));

			_vec3	vPlayerDir = m_vPlayerPos - m_vMyPos;
			m_vDir = -(*m_pTransformCom->Get_Info(INFO_RIGHT));
			D3DXVec3Normalize(&m_vDir, &m_vDir);
			D3DXVec3Normalize(&vPlayerDir, &vPlayerDir);

			m_fAngle = D3DXToDegree(acos(D3DXVec3Dot(&m_vDir, &vPlayerDir)));

			// 이것도 축이 반대인가? . Look == Back, Right == Left;
			if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&_vec3(), &vPlayerDir, &m_vDir)) > 0.f)
				m_bTargetIsRight = false;
			else if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&_vec3(), &vPlayerDir, &m_vDir)) < 0.f)
				m_bTargetIsRight = true;


			if ((m_bCanAction && m_pPlayerTrans) || !m_bAnimation) // m_bAnimation은 디버깅용
			{
				if (15.f <= m_fAngle)
				{
					if (m_bTargetIsRight)
					{
						m_iAniIndex = CLOYAN_TURNRIGHT;
					}
					else
					{
						m_iAniIndex = CLOYAN_TURNLEFT;
					}
				}
				else if (DIS_FACETOFACE >= m_fDistance)
				{
					if (m_dwSlipDashCoolDown + m_dwSlipDashDelay < GetTickCount())
					{
						m_iAniIndex = CLOYAN_SLIPDASH;
					}
					else if (20.f >= m_fRand)
					{
						m_iAniIndex = CLOYAN_ATTACK;
					}
					else if (45.f >= m_fRand)
					{
						m_iAniIndex = CLOYAN_SLASHPIERCE;
					}
					else if (75.f >= m_fRand)
					{
						m_iAniIndex = CLOYAN_STEPPIERCE;
					}
					else
					{
						m_iAniIndex = CLOYAN_TWINBOLT;
					}
				}
				else
				{
					m_iAniIndex = CLOYAN_RUN;
				}
			}
		}
	}
}

void CCloyan::MoveOn_Skill()
{
	if (m_bSkillMove && 
		CLOYAN_DYING != m_iAniIndex)
	{
		if (m_fSkillMoveStartTime <= m_fAniTime &&
			m_fSkillMoveEndTime >= m_fAniTime)
		{
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(&m_vMyPos, &m_vDir, m_fSkillMoveSpeed, m_fTimeDelta));
		}
		else if (m_fSkillMoveEndTime < m_fAniTime)
		{
			m_bSkillMove = false;
		}
	}
}

void CCloyan::RotationOn_Skill()
{
	if (m_bSkillRotation)
	{
		if (m_fSkillRotStartTime <= m_fAniTime &&
			m_fSkillRotEndTime >= m_fAniTime)
		{
			if (10.f <= m_fAngle)
			{
				if (m_bTargetIsRight)
				{
					m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(m_fSkillRotSpeed * m_fTimeDelta));
				}
				else
				{
					m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-m_fSkillRotSpeed * m_fTimeDelta));
				}
			}
		}
		else if (m_fSkillRotEndTime < m_fAniTime)
		{
			m_bSkillRotation = false;
		}
	}
}

void CCloyan::RotateLookVector()
{
	_vec3 vPlayerPos = *m_pPlayerTrans->Get_Info(INFO_POS);
	_vec3 vMyLook = -*m_pTransformCom->Get_Info(INFO_LOOK);
	_vec3 vToPlayerDir = vPlayerPos - *m_pTransformCom->Get_Info(INFO_POS);
	_vec3 vTemp;

	D3DXVec3Normalize(&vToPlayerDir, &vToPlayerDir);
	D3DXVec3Normalize(&vMyLook, &vMyLook);

	if (D3DXToDegree(acos(D3DXVec3Dot(&vToPlayerDir, &vMyLook))) > 5.f)
	{
		if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&vTemp, &vToPlayerDir, &vMyLook)) > 0.f)
		{
			m_pTransformCom->Rotation(ROT_Y, acos(D3DXVec3Dot(&vToPlayerDir, &vMyLook)));
		}
		else if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&vTemp, &vToPlayerDir, &vMyLook)) < 0.f)
		{
			m_pTransformCom->Rotation(ROT_Y, -acos(D3DXVec3Dot(&vToPlayerDir, &vMyLook)));
		}
	}
}

void CCloyan::Animation_Control()
{
	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	m_fAniTime = m_pMeshCom->Get_AniFrameTime();

	// 상태 변경 시 한번만 실행
	m_eCurState = (CLOYAN_STATE)m_iAniIndex;

	if (m_eCurState != m_ePreState)
	{
		m_bAnimation = true;

		m_fAniTime = 0.f;
		m_lfAniEnd = m_pMeshCom->Get_AniFrameEndTime();

		// Sound들 전부 다시 false;
		m_bSound = false;
		//////////////////////////

		_uint iRandSound = 0;
		CGameObject*	pGameObject = nullptr;

		switch (m_eCurState)
		{
		case CCloyan::CLOYAN_ATTACK:
			m_bCanAction = false;
			m_lfAniEnd = 2.6f;
			m_eCurAction = CL_ATK;
			ENEMY_SKILL_MOVE((float)m_lfAniEnd * 0.38f, 3.5f, (_float)m_lfAniEnd * 0.55f);
			ENEMY_SKILL_ROTATION((float)m_lfAniEnd * 0.1f, 180.f, (float)m_lfAniEnd * 0.5f);
			m_pMeshCom->Set_TrackSpeed(2.1f);
			break;

		case CCloyan::CLOYAN_DAMAGEFROMBACK:
			m_bCanAction = false;
			m_lfAniEnd = 0.85f;
			m_eCurAction = CL_DAMAGED;
			break;

		case CCloyan::CLOYAN_DAMAGEFROMFRONT:
			m_bCanAction = false;
			m_lfAniEnd = 0.85f;
			m_eCurAction = CL_DAMAGED;
			break;

		case CCloyan::CLOYAN_DYING:
			m_bDissolveOn = true;
			m_bCanAction = false;
			m_pMeshCom->Set_TrackSpeed(1.f);
			m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
			m_lfAniEnd = 0.7f;
			m_eCurAction = CL_DEAD;
			break;

		case CCloyan::CLOYAN_IDLE:
			m_bCanAction = true;
			m_eCurAction = CL_IDLE;
			break;

		case CCloyan::CLOYAN_LOW_HEALTH:
			m_eCurAction = CL_DAMAGED;
			break;

		case CCloyan::CLOYAN_RUN:
			m_bCanAction = true;
			m_pMeshCom->Set_TrackSpeed(1.9f);
			break;

		case CCloyan::CLOYAN_SLASHPIERCE:
			m_bCanAction = false;
			m_lfAniEnd = 2.8f;
			ENEMY_SKILL_MOVE((float)m_lfAniEnd * 0.6f, 6.f, (_float)m_lfAniEnd * 0.7f);
			ENEMY_SKILL_ROTATION((float)m_lfAniEnd * 0.1f, 180.f, (float)m_lfAniEnd * 0.65f);
			m_eCurAction = CL_ATK;
			m_pMeshCom->Set_TrackSpeed(2.2f);
			break;

		case CCloyan::CLOYAN_SLIPDASH:
			m_bCanAction = false;
			m_lfAniEnd = 1.f;
			ENEMY_SKILL_MOVE((float)m_lfAniEnd * 0.1f, -8.f, (_float)m_lfAniEnd * 0.3f);
			m_pMeshCom->Set_TrackSpeed(1.9f);
			break;

		case CCloyan::CLOYAN_SPAWN:
			m_bCanAction = false;
			m_lfAniEnd = 2.5f;
			m_eCurAction = CL_SPAWN;

			pGameObject = CBoss_Hpbar_BackUI::Create(m_pGraphicDev, SCREEN_CENTER_X, BOSS_HPBAR_Y, BOSS_HPBAR_SCALE_X, BOSS_HPBAR_SCALE_Y);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"0.Boss_Hpbar_BackUI", pGameObject), );

			pGameObject = CBoss_Hpbar_RedUI::Create(m_pGraphicDev, SCREEN_CENTER_X, BOSS_HPBAR_Y, BOSS_HPBAR_SCALE_X, BOSS_HPBAR_SCALE_Y, 2);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"1.Boss_Hpbar_RedUI", pGameObject), );

			pGameObject = CBoss_Hpbar_YellowUI::Create(m_pGraphicDev, SCREEN_CENTER_X, BOSS_HPBAR_Y, BOSS_HPBAR_SCALE_X, BOSS_HPBAR_SCALE_Y, 1);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"2.Boss_Hpbar_YellowUI", pGameObject), );

			pGameObject = CBoss_Hpbar_GreenUI::Create(m_pGraphicDev, SCREEN_CENTER_X, BOSS_HPBAR_Y, BOSS_HPBAR_SCALE_X, BOSS_HPBAR_SCALE_Y, 0);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"3.Boss_Hpbar_GreenUI", pGameObject), );

			// Font Image
			pGameObject = CBoss_Hpbar_FontUI::Create(m_pGraphicDev, WINCX * 0.72f, BOSS_HPBAR_Y, 23.f, 23.f, true, false);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"4.Boss_Hpbar_FontXUI", pGameObject), );

			pGameObject = CBoss_Hpbar_FontUI::Create(m_pGraphicDev, WINCX * 0.735f, BOSS_HPBAR_Y, 30.f, 30.f, false, false);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"5.Boss_Hpbar_FontNumUI", pGameObject), );

			pGameObject = CBoss_Hpbar_FontUI::Create(m_pGraphicDev, WINCX * 0.745f, BOSS_HPBAR_Y, 30.f, 30.f, false, true);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"6.Boss_Hpbar_FontNumTenUI", pGameObject), );

			pGameObject = CAhglan_FontName::Create(m_pGraphicDev, SCREEN_CENTER_X, BOSS_HPBAR_Y, 100.f, 50.f, BOSS_CLOYAN);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"7.Ahglan_FontName", pGameObject), );

			pGameObject = CBoss_NamingScene::Create(m_pGraphicDev, SCREEN_CENTER_X, WINCY - 185.f, 1500.f, 550.f, NAMING_BACKGROUND, BOSS_CLOYAN);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"9.Cloyan_NamingScene_Back", pGameObject), );

			pGameObject = CBoss_NamingScene::Create(m_pGraphicDev, SCREEN_CENTER_X + 5.f, WINCY - 130.f, 205.f, 95.f, NAMING_BOSSNAME, BOSS_CLOYAN);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"10.Cloyan_NamingScene_BossName", pGameObject), );

			pGameObject = CBoss_NamingScene::Create(m_pGraphicDev, SCREEN_CENTER_X, WINCY - 75.f, 153.f, 80.f, NAMING_STAGENAME, BOSS_CLOYAN);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"11.Cloyan_NamingScene_StageName", pGameObject), );

			Engine::Emplace_Layer(L"Boss_UI", m_pUILayer);

			m_pGreenHpbar = dynamic_cast<CBoss_Hpbar_GreenUI*>(Engine::Get_GameObject(L"Boss_UI", L"3.Boss_Hpbar_GreenUI"));
			m_pYellowHpbar = dynamic_cast<CBoss_Hpbar_YellowUI*>(Engine::Get_GameObject(L"Boss_UI", L"2.Boss_Hpbar_YellowUI"));
			m_pRedHpbar = dynamic_cast<CBoss_Hpbar_RedUI*>(Engine::Get_GameObject(L"Boss_UI", L"1.Boss_Hpbar_RedUI"));
			m_pFontHpbar = dynamic_cast<CBoss_Hpbar_FontUI*>(Engine::Get_GameObject(L"Boss_UI", L"5.Boss_Hpbar_FontNumUI"));
			m_pFontHpbarTen = dynamic_cast<CBoss_Hpbar_FontUI*>(Engine::Get_GameObject(L"Boss_UI", L"6.Boss_Hpbar_FontNumTenUI"));

			SoundMgrBGM(L"bgm_ep8_mankind.wav", CSoundMgr::BGM);
			break;

		case CCloyan::CLOYAN_STEPBACK:
			break;

		case CCloyan::CLOYAN_STEPFRONT:
			break;

		case CCloyan::CLOYAN_STEPPIERCE:
			m_bCanAction = false;
			m_lfAniEnd = 3.5f;
			m_eCurAction = CL_ATK;
			ENEMY_SKILL_MOVE((float)m_lfAniEnd * 0.75f, 17.f, (_float)m_lfAniEnd * 0.85f);
			ENEMY_SKILL_ROTATION((float)m_lfAniEnd * 0.1f, 180.f, (float)m_lfAniEnd * 0.8f);
			m_pMeshCom->Set_TrackSpeed(2.2f);
			break;

		case CCloyan::CLOYAN_TWINBOLT:
			m_bCanAction = false;
			m_lfAniEnd = 2.8f;
			m_eCurAction = CL_ATK;
			m_pMeshCom->Set_TrackSpeed(2.1f);
			ENEMY_SKILL_ROTATION((float)m_lfAniEnd * 0.1f, 180.f, (float)m_lfAniEnd * 0.6f);
			break;

		case CCloyan::CLOYAN_TURNLEFT:
			m_bCanAction = false;
			m_lfAniEnd = 1.5f;
			break;

		case CCloyan::CLOYAN_TURNRIGHT:
			m_bCanAction = false;
			m_lfAniEnd = 1.5f;
			break;
		}

		m_ePreState = m_eCurState;
	}


	// 상태 변경 시 매 프레임 실행
	_vec3	vDir = -*m_pTransformCom->Get_Info(INFO_RIGHT);
	_vec3	vRightDir = *m_pTransformCom->Get_Info(INFO_LOOK);

	switch (m_eCurState)
	{
	case CCloyan::CLOYAN_ATTACK:
		break;

	case CCloyan::CLOYAN_DAMAGEFROMBACK:
		break;

	case CCloyan::CLOYAN_DAMAGEFROMFRONT:
		break;

	case CCloyan::CLOYAN_DYING:
		break;

	case CCloyan::CLOYAN_IDLE:
		break;

	case CCloyan::CLOYAN_LOW_HEALTH:
		break;

	case CCloyan::CLOYAN_RUN:
		m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(&m_vMyPos, D3DXVec3Normalize(&vDir, &vDir), m_fSpeed, m_fTimeDelta, true));

		if (10.f <= m_fAngle)
		{
			if (m_bTargetIsRight)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(360.f * m_fTimeDelta));
			}
			else
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(360.f * -m_fTimeDelta));
			}
		}
		break;

	case CCloyan::CLOYAN_SLASHPIERCE:
		break;

	case CCloyan::CLOYAN_SLIPDASH:
		break;

	case CCloyan::CLOYAN_SPAWN:
		break;

	case CCloyan::CLOYAN_STEPBACK:
		m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(&m_vMyPos, D3DXVec3Normalize(&vDir, &vDir), -m_fStepSpeed, m_fTimeDelta, true));

		if (10.f <= m_fAngle)
		{
			if (m_bTargetIsRight)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(360.f * m_fTimeDelta));
			}
			else
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(360.f * -m_fTimeDelta));
			}
		}
		break;

	case CCloyan::CLOYAN_STEPPIERCE:
		break;

	case CCloyan::CLOYAN_TWINBOLT:
		break;

	case CCloyan::CLOYAN_TURNLEFT:
		if (10.f <= m_fAngle)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * -m_fTimeDelta));
		}
		else
		{
			m_iAniIndex = CLOYAN_RUN;
			Animation_Control();
		}
		break;

	case CCloyan::CLOYAN_TURNRIGHT:
		if (10.f <= m_fAngle)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * m_fTimeDelta));
		}
		else
		{
			m_iAniIndex = CLOYAN_RUN;
			Animation_Control();
		}
		break;
	}

	if (m_fAniTime >= m_lfAniEnd * m_fAniEndDelay)
	{
		m_bCanAction = true;

		if (CLOYAN_RUN == m_iAniIndex ||
			CLOYAN_IDLE == m_iAniIndex)
		{
			m_bAnimation = true;
		}
		else
		{
			m_bAnimation = false;
		}

		if (CLOYAN_DYING == m_eCurState)
		{
			m_bDead = true;
		}
		else if (CLOYAN_SLIPDASH == m_eCurState)
		{
			m_iAniIndex = CLOYAN_STEPPIERCE;
			m_dwSlipDashCoolDown = GetTickCount();
			m_dwSlipDashDelay = (_ulong)Engine::Random(10000.f, 20000.f);

			Animation_Control();
		}
		else if (0 >= m_pPlayer->Get_TagPlayerInfo().tagInfo.iHp)
		{
			m_iAniIndex = CLOYAN_IDLE;
		}
		else if (CLOYAN_IDLE != m_eCurState &&
				 CLOYAN_RUN != m_eCurState)
		{
			m_fRand = Engine::Random(0.f, 100.f);

			m_eCurAction = CL_IDLE;
			m_iAniIndex = CLOYAN_IDLE;
			m_pMeshCom->Set_TrackSpeed(2.f);

			Animation_Control();
		}
	}
}

void CCloyan::Collision_Control()
{
	map<const wstring, CBoxCollider*>::iterator	iter_Damaged = m_mapBoxColliderCom.begin();
	map<const wstring, CCollider*>::iterator	iter_Hit = m_mapColliderCom.begin();

	// HitBox
	for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
	{
		switch (m_eCurState)
		{
		case CCloyan::CLOYAN_ATTACK:
			HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.4f, m_lfAniEnd * 0.55f);
			break;

		case CCloyan::CLOYAN_SLASHPIERCE:
			HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.25f, m_lfAniEnd * 0.9f);
			break;

		case CCloyan::CLOYAN_STEPPIERCE:
			HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.7f, m_lfAniEnd * 0.85f);
			break;

		case CCloyan::CLOYAN_TWINBOLT:
			HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.5f, m_lfAniEnd * 0.8f);
			break;

		default:
			iter_Hit->second->Set_CanCollision(false);
			break;
		}
	}
}

const _ulong & CCloyan::Compute_InCell()
{
	vector<CCell*>	vecCell = m_pNaviMeshCom->Get_CellVector();
	vector<CCell*>::iterator	iter = vecCell.begin();

	for (; iter != vecCell.end(); ++iter)
	{
		for (_uint i = 0; i < CCell::POINT_END; ++i)
		{
			_vec3	vTemp;
			_vec3	vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
			vMyPos.y = 0.f;

			_vec3	vPointA = *(*iter)->Get_Point(CCell::POINT_A);
			_vec3	vPointB = *(*iter)->Get_Point(CCell::POINT_B);
			_vec3	vPointC = *(*iter)->Get_Point(CCell::POINT_C);
			vPointA.y = 0.f;
			vPointB.y = 0.f;
			vPointC.y = 0.f;

			_vec3	vDirAB = vPointB - vPointA;
			_vec3	vDirAC = vPointC - vPointA;
			_vec3	vDirAP = vMyPos - vPointA;
			D3DXVec3Normalize(&vDirAB, &vDirAB);
			D3DXVec3Normalize(&vDirAC, &vDirAC);
			D3DXVec3Normalize(&vDirAP, &vDirAP);

			_vec3	vDirBA = vPointA - vPointB;
			_vec3	vDirBC = vPointC - vPointB;
			_vec3	vDirBP = vMyPos - vPointB;
			D3DXVec3Normalize(&vDirBA, &vDirBA);
			D3DXVec3Normalize(&vDirBC, &vDirBC);
			D3DXVec3Normalize(&vDirBP, &vDirBP);

			_vec3	vDirCA = vPointA - vPointC;
			_vec3	vDirCB = vPointB - vPointC;
			_vec3	vDirCP = vMyPos - vPointC;
			D3DXVec3Normalize(&vDirCA, &vDirCA);
			D3DXVec3Normalize(&vDirCB, &vDirCB);
			D3DXVec3Normalize(&vDirCP, &vDirCP);

			if (0.f < D3DXVec3Cross(&vTemp, &vDirCP, &vDirCB)->y &&
				0.f < D3DXVec3Cross(&vTemp, &vDirBP, &vDirBA)->y &&
				0.f < D3DXVec3Cross(&vTemp, &vDirAP, &vDirAC)->y)
			{
				return *(*iter)->Get_CellIndex();
			}
		}
	}

	return 0;
}

void CCloyan::Update_UI()
{
	if (m_pGreenHpbar)
	{
		m_pGreenHpbar->Set_ValueRatio((_float)m_tInfo.iHp);
		m_pGreenHpbar->Set_MaxValueRatio((_float)m_tInfo.iMaxHp);
		m_pGreenHpbar->Set_LineHpRatio((_float)m_iLineHp);
		m_pGreenHpbar->Set_MaxLineHpRatio((_float)m_iMaxLineHp);
	}
	if (m_pYellowHpbar)
	{
		m_pYellowHpbar->Set_ValueRatio((_float)m_tInfo.iHp);
		m_pYellowHpbar->Set_MaxValueRatio((_float)m_tInfo.iMaxHp);
		m_pYellowHpbar->Set_LineHpRatio((_float)m_iLineHp);
		m_pYellowHpbar->Set_MaxLineHpRatio((_float)m_iMaxLineHp);
	}
	if (m_pRedHpbar)
	{
		m_pRedHpbar->Set_ValueRatio((_float)m_tInfo.iHp);
		m_pRedHpbar->Set_MaxValueRatio((_float)m_tInfo.iMaxHp);
		m_pRedHpbar->Set_LineHpRatio((_float)m_iLineHp);
		m_pRedHpbar->Set_MaxLineHpRatio((_float)m_iMaxLineHp);
	}
	if (m_pFontHpbar)
	{
		m_pFontHpbar->Set_ValueRatio((_float)m_tInfo.iHp);
		m_pFontHpbar->Set_MaxValueRatio((_float)m_tInfo.iMaxHp);
	}
	if (m_pFontHpbarTen)
	{
		m_pFontHpbarTen->Set_ValueRatio((_float)m_tInfo.iHp);
		m_pFontHpbarTen->Set_MaxValueRatio((_float)m_tInfo.iMaxHp);
	}
}

void CCloyan::Update_State()
{
	if (0 >= m_tInfo.iHp)
	{
		m_iAniIndex = CLOYAN_DYING;

		Animation_Control();
	}
}

void CCloyan::Dissolve(const _float & fTimeDelta)
{
	if (m_bDissolveOn)
	{
		m_fDissolveValue += 0.25f * fTimeDelta;
	}
}

void CCloyan::Make_TrailEffect(const _float & fTimeDelta)
{
	map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

	for (; iter != m_mapBoxColliderCom.end(); ++iter)
	{
		if (L"Trail_Info" == iter->first)
		{
			m_pTrail->Set_InfoForTrail(fTimeDelta, iter->second->Get_Min(), iter->second->Get_Max(), iter->second->Get_ColliderWorld());
		}
	}

	if (CL_ATK == m_eCurAction)
	{
		m_pTrail->Set_Render(true);
	}
	else
	{
		m_pTrail->Set_Render(false);
	}
}

HRESULT CCloyan::Add_NaviMesh()
{
	CComponent*		pComponent = nullptr;

	pComponent = m_pNaviMeshCom = dynamic_cast<CNaviMesh*>(Engine::Clone_Prototype(L"Proto_NaviMesh"));
	NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	return S_OK;
}

HRESULT CCloyan::Load_ColInfo()
{
	HANDLE hFile = CreateFile(L"../../Data/Cloyan_Col.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwbyte = 0;
	DWORD dwStringSize = 0;
	DWORD dwStringSize2 = 0;
	_float fRadius = 0.f;
	TCHAR*	pNameBuff = nullptr;
	TCHAR*	pNameBuff2 = nullptr;
	wstring wstrColName, wstrBoneName;
	COLLIDERTYPE	eColliderType;
	_vec3	vMin = {};
	_vec3	vMax = {};

	_uint iSize = 0;
	_uint iBoxSize = 0;

	ReadFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iBoxSize, sizeof(_uint), &dwbyte, nullptr);

	for (_uint i = 0; i < iSize; ++i)
	{
		if (0 == dwbyte)
			break;

		ReadFile(hFile, &dwStringSize, sizeof(DWORD), &dwbyte, nullptr);
		ReadFile(hFile, &dwStringSize2, sizeof(DWORD), &dwbyte, nullptr);

		pNameBuff = new TCHAR[dwStringSize];
		pNameBuff2 = new TCHAR[dwStringSize2];

		ReadFile(hFile, pNameBuff, dwStringSize, &dwbyte, nullptr);
		ReadFile(hFile, pNameBuff2, dwStringSize2, &dwbyte, nullptr);
		ReadFile(hFile, &fRadius, sizeof(_float), &dwbyte, nullptr);
		ReadFile(hFile, &eColliderType, sizeof(COLLIDERTYPE), &dwbyte, nullptr);

		wstrColName = pNameBuff;
		wstrBoneName = pNameBuff2;

		this->Add_Collider(fRadius, pNameBuff, m_pTransformCom->Get_WorldMatrix(), eColliderType);

		list<D3DXMESHCONTAINER_DERIVED*>			listTemp = m_pMeshCom->Get_MeshContainerList();
		list<D3DXMESHCONTAINER_DERIVED*>::iterator	iterList = listTemp.begin();

		for (; iterList != listTemp.end(); ++iterList)
		{
			for (_uint i = 0; i < (*iterList)->dwNumBones; ++i)
			{
				_tchar	pTemp[64] = L"";
				MultiByteToWideChar(CP_ACP, 0, (*iterList)->pSkinInfo->GetBoneName(i), strlen((*iterList)->pSkinInfo->GetBoneName(i)), pTemp, 64);

				if (/*(*iterList)->pSkinInfo->GetBoneName(i)*/!wcscmp(pTemp, pNameBuff2))
				{
					dynamic_cast<CCollider*>(this->Get_Component(wstrColName, ID_STATIC))->Set_BoneName(wstrBoneName);
					dynamic_cast<CCollider*>(this->Get_Component((wstring)wstrColName, ID_STATIC))->Set_Matrix((*iterList)->ppCombinedTransformMatrix[i]);

					break;
				}
			}
		}

		Safe_Delete(pNameBuff);
		Safe_Delete(pNameBuff2);
	}

	for (_uint i = 0; i < iBoxSize; ++i)
	{
		if (0 == dwbyte)
			break;

		ReadFile(hFile, &dwStringSize, sizeof(DWORD), &dwbyte, nullptr);
		ReadFile(hFile, &dwStringSize2, sizeof(DWORD), &dwbyte, nullptr);

		pNameBuff = new TCHAR[dwStringSize];
		pNameBuff2 = new TCHAR[dwStringSize2];

		ReadFile(hFile, pNameBuff, dwStringSize, &dwbyte, nullptr);
		ReadFile(hFile, pNameBuff2, dwStringSize2, &dwbyte, nullptr);
		ReadFile(hFile, &vMin, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vMax, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &eColliderType, sizeof(COLLIDERTYPE), &dwbyte, nullptr);

		wstrColName = pNameBuff;
		wstrBoneName = pNameBuff2;

		this->Add_Collider(vMin.x, vMin.y, vMin.z, vMax.x, vMax.y, vMax.z, pNameBuff,
			dynamic_cast<CTransform*>(this->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_WorldMatrix(), eColliderType);

		// Bone 찾기
		list<D3DXMESHCONTAINER_DERIVED*>			listTemp = m_pMeshCom->Get_MeshContainerList();
		list<D3DXMESHCONTAINER_DERIVED*>::iterator	iterList = listTemp.begin();

		for (; iterList != listTemp.end(); ++iterList)
		{
			for (_uint i = 0; i < (*iterList)->dwNumBones; ++i)
			{
				_tchar	pTemp[64] = L"";
				MultiByteToWideChar(CP_ACP, 0, (*iterList)->pSkinInfo->GetBoneName(i), strlen((*iterList)->pSkinInfo->GetBoneName(i)), pTemp, 64);

				if (!wcscmp(pTemp, pNameBuff2))
				{
					dynamic_cast<CBoxCollider*>(this->Get_Component(wstrColName, ID_STATIC))->Set_BoneName(wstrBoneName);
					dynamic_cast<CBoxCollider*>(this->Get_Component((wstring)wstrColName, ID_STATIC))->Set_Matrix((*iterList)->ppCombinedTransformMatrix[i]);

					break;
				}
			}
		}

		Safe_Delete(pNameBuff);
		Safe_Delete(pNameBuff2);
	}

	CloseHandle(hFile);

	return S_OK;
}


CCloyan * CCloyan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCloyan*	pInstance = new CCloyan(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CCloyan::Free(void)
{
	Engine::Delete_AllInLayer(L"Boss_UI");
	Safe_Release(m_pUILayer);

	CGameObject::Free();
}
