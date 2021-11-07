#include "stdafx.h"
#include "Ahglan.h"

#include "Player.h"
#include "DynamicCamera.h"

#include "Boss_Hpbar_BackUI.h"
#include "Boss_Hpbar_GreenUI.h"
#include "Boss_Hpbar_YellowUI.h"
#include "Boss_Hpbar_RedUI.h"
#include "Boss_Hpbar_FontUI.h"

#include "Export_Function.h"
#include "Export_Utility.h"

USING(Engine)

CAhglan::CAhglan(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CAhglan::~CAhglan()
{
}


HRESULT CAhglan::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(0.f, 0.f, -7.f);
	m_pTransformCom->Set_Scale(SCALE_AHGLAN, SCALE_AHGLAN, SCALE_AHGLAN);
	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180));

	m_pTransformCom->Update_Component(0.f);

	m_tInfo.iHp = 100000;
	m_tInfo.iMaxHp = m_tInfo.iHp;

	m_bBoss = true;
	
	m_iLineHp = _int(m_tInfo.iMaxHp * 0.1f);
	m_iMaxLineHp = m_iLineHp;

	return S_OK;
}

HRESULT CAhglan::LateReady_Object()
{
	CGameObject::LateReady_Object();

	m_pMainCamera = static_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));

	m_mapActiveParts.emplace(L"golem_ahglan_Chest.tga", TRUE);
	m_mapActiveParts.emplace(L"golem_ahglan_Head.tga", TRUE);
	m_mapActiveParts.emplace(L"golem_ahglan_RHand.tga", TRUE);
	m_mapActiveParts.emplace(L"golem_ahglan_ore.tga", TRUE);


	m_pUILayer = CLayer::Create();
	NULL_CHECK_RETURN(m_pUILayer, E_FAIL);

	return S_OK;
}

_int CAhglan::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	if (m_bDead)
		return iExit;

	//// 디버그용
	if (Key_Down('G'))
	{
		Set_Damage(5000);
	}
	else if (Key_Down('H'))
	{
		m_iAniIndex = SPAWN;
		Animation_Control();
	}
	//else if (Key_Down('J'))
	//{
	//	m_bDead = true;
	//}

	m_fTimeDelta = fTimeDelta;

	Movement();
	Animation_Control();
	Collision_Control();
	Update_UI();
	FootStep();
	MoveOn_Skill();
	RotationOn_Skill();

	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	if (m_bAnimation)
		m_pMeshCom->Play_Animation(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

_int CAhglan::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	if (!m_mapColliderCom.empty())
	{
		map<const wstring, CCollider*>::iterator	iter = m_mapColliderCom.begin();

		for (; iter != m_mapColliderCom.end(); ++iter)
		{
			iter->second->Set_ColliderMatrix(m_pTransformCom->Get_WorldMatrix());
		}
	}
	if (!m_mapBoxColliderCom.empty())
	{
		map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		for (; iter != m_mapBoxColliderCom.end(); ++iter)
		{
			iter->second->Set_ColliderMatrix(m_pTransformCom->Get_WorldMatrix());
		}
	}

	return iExit;
}

void CAhglan::Render_Object(void)
{
	if (!m_mapColliderCom.empty())
	{
		map<const wstring, CCollider*>::iterator	iter = m_mapColliderCom.begin();

		for (; iter != m_mapColliderCom.end(); ++iter)
		{
			if (iter->second->Get_CanCollision())
			{
				iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
			}
		}
	}
	if (!m_mapBoxColliderCom.empty())
	{
		map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		for (; iter != m_mapBoxColliderCom.end(); ++iter)
		{
			if (iter->second->Get_CanCollision())
			{
				iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
			}
		}
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	pEffect->AddRef();

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	_uint iMaxPass = 0;

	pEffect->Begin(&iMaxPass, NULL);		// 1인자 : 현재 쉐이더 파일이 반환하는 pass의 최대 개수
											// 2인자 : 시작하는 방식을 묻는 FLAG
	pEffect->BeginPass(0);

	m_pMeshCom->Render_Meshes(pEffect, m_mapActiveParts);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CAhglan::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CDynamicMesh*>(Engine::Clone_Prototype(L"Proto_Mesh_Ahglan"));
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

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Prototype(L"Proto_Calculator"));
	NULL_CHECK_RETURN(m_pCalculatorCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Calculator", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CAhglan::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 10.f;

	pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrl.Diffuse);
	pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrl.Specular);
	pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrl.Ambient);

	pEffect->SetFloat("g_fPower", tMtrl.Power);

	const D3DLIGHT9*	pLightInfo = Get_Light();
	NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));

	pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLightInfo->Diffuse);
	pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLightInfo->Specular);
	pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLightInfo->Ambient);

	D3DXMatrixInverse(&matView, NULL, &matView);
	pEffect->SetVector("g_vCamPos", (_vec4*)&matView._41);

	return S_OK;
}

void CAhglan::Contact()
{
	if (BS_ENTRY <= m_eBossAction &&
		m_fDistance <= DIS_LONG)
	{
		CDynamicCamera* pCamera = static_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));
		if (pCamera)
			pCamera->Set_CameraMode(CDynamicCamera::MODE_AHGLAN_START);

		m_iAniIndex = ENTRY_CONTACT;

		Animation_Control();
	}
}

void CAhglan::Movement()
{
	m_pPlayer = static_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	if (m_pPlayer)
	{
		if (0.f >= m_pPlayer->Get_TagPlayerInfo().tagInfo.iHp && 
			BS_IDLE == m_eBossAction)
		{
			m_iAniIndex = IDLE;
		}
		else
		{
			m_pPlayerTrans = static_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC));

			m_vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
			if (m_pPlayerTrans)
				m_vPlayerPos = *m_pPlayerTrans->Get_Info(INFO_POS);
			m_fDistance = D3DXVec3Length(&(m_vMyPos - m_vPlayerPos));

			_vec3	vPlayerDir = m_vPlayerPos - m_vMyPos;
			m_vDir = -(*m_pTransformCom->Get_Info(INFO_LOOK));
			D3DXVec3Normalize(&m_vDir, &m_vDir);
			D3DXVec3Normalize(&vPlayerDir, &vPlayerDir);

			m_fAngle = D3DXToDegree(acos(D3DXVec3Dot(&m_vDir, &vPlayerDir)));

			// 아글란 메쉬의 축이 반대로 뒤틀려있다. 그래서 계산도 반대로 함. Look == Back, Right == Left;
			if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&_vec3(), &vPlayerDir, &m_vDir)) > 0.f)
				m_bTargetIsRight = false;
			else if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&_vec3(), &vPlayerDir, &m_vDir)) < 0.f)
				m_bTargetIsRight = true;


			if (m_bCanAction && m_pPlayerTrans)
			{
				if (BS_DAMAGED <= m_eBossAction)
				{

				}
				else if (BS_ATK <= m_eBossAction)
				{
					if (DIS_SHORT > m_fDistance)
					{
						if (m_dwWindmillCoolDown + m_dwWindmillDelay < GetTickCount())
						{
							m_iAniIndex = ATK_WINDMILL;
						}
						else
						{
							if (20.f >= m_fRand)
							{
								m_iAniIndex = ATK_TWOHANDS;
							}
							else if (35.f >= m_fRand)
							{
								m_iAniIndex = ATK_ONEHAND;
							}
							else if (45.f >= m_fRand)
							{
								m_iAniIndex = TAUNT;
							}
							else if (60.f >= m_fRand)
							{
								m_iAniIndex = ATK_STAMP;
							}
							else if (75.f >= m_fRand)
							{
								m_iAniIndex = ATK_ONETIME_STAMP;
							}
							else if (90.f >= m_fRand)
							{
								m_iAniIndex = ATK_THREETIME_STAMP;
							}
							else
							{
								if (m_bTargetIsRight)
								{
									m_iAniIndex = ATK_TURNRIGHT;
								}
								else
								{
									m_iAniIndex = ATK_TURNLEFT;
								}
							}
						}
					}
				}
				else if (BS_IDLE <= m_eBossAction)
				{
					if (DIS_SHORTEST >= m_fDistance &&
						20.f >= m_fRand)
					{
						if (m_bTargetIsRight)
						{
							m_iAniIndex = ATK_TURNRIGHT;
						}
						else
						{
							m_iAniIndex = ATK_TURNLEFT;
						}
					}
					else if (50.f <= m_fAngle)
					{
						if (m_bTargetIsRight)
						{
							m_iAniIndex = TURN_RIGHT;
						}
						else
						{
							m_iAniIndex = TURN_LEFT;
						}
					}
					else if (DIS_SHORT < m_fDistance)
					{
						if (m_dwRollingAtkCoolDown + m_dwRollingAtkDelay < GetTickCount())
						{
							m_iAniIndex = ATK_ROLLING_ONETIME_BEGIN;
						}
						else
						{
							m_iAniIndex = WALK;
						}
					}
				}
			}
		}
	}
}

void CAhglan::MoveOn_Skill()
{
	if (m_bSkillMove && 
		BS_DAMAGED > m_eBossAction)
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

void CAhglan::RotationOn_Skill()
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

void CAhglan::FootStep()
{
	if (STATE::WALK == m_eCurState &&
		m_dwFootStepStart + m_dwFootStepDelay <= GetTickCount())
	{
		m_dwFootStepStart = GetTickCount();

		SoundMgr(L"step_lv3.wav", CSoundMgr::MONSTER2);
		m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 0.6f);
	}
}

void CAhglan::Animation_Control()
{
	m_fAniTime = m_pMeshCom->Get_AniFrameTime();
	m_lfAniEnd = m_pMeshCom->Get_AniFrameEndTime();

	// 상태 변경 시 한번만 실행
	m_eCurState = (STATE)m_iAniIndex;
	if (m_eCurState != m_ePreState)
	{
		m_bAnimation = true;
		m_fAniTime = 0.f;

		// Sound들 전부 다시 false;
		m_bAtkSound = false;
		m_bEffectSound = false;
		for (_uint i = 0; i < 3; ++i)
			m_bStampAtkSound[i] = false;
		//////////////////////////

		_uint iRandSound = 0;
		CGameObject*	pGameObject = nullptr;

		switch (m_eCurState)
		{
		case WALK:
			m_eBossAction = BS_IDLE;

			m_pMeshCom->Set_TrackSpeed(2.5f);
			m_dwFootStepStart = GetTickCount();
			break;

		case TURN_RIGHT:
			m_eBossAction = BS_IDLE;
			break;

		case TURN_LEFT:
			m_eBossAction = BS_IDLE;
			break;

		case TAUNT:
			m_eBossAction = BS_ATK;
			break;

		case SPAWN:
			pGameObject = CBoss_Hpbar_BackUI::Create(m_pGraphicDev, 550.f, 100.f, 550.f, 30.f);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Boss_Hpbar_BackUI", pGameObject), );

			pGameObject = CBoss_Hpbar_RedUI::Create(m_pGraphicDev, 550.f, 100.f, 550.f, 30.f, 2);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Boss_Hpbar_RedUI", pGameObject), );

			pGameObject = CBoss_Hpbar_YellowUI::Create(m_pGraphicDev, 550.f, 100.f, 550.f, 30.f, 1);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Boss_Hpbar_YellowUI", pGameObject), );

			pGameObject = CBoss_Hpbar_GreenUI::Create(m_pGraphicDev, 550.f, 100.f, 550.f, 30.f, 0);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Boss_Hpbar_GreenUI", pGameObject), );
			
			// Font Image
			pGameObject = CBoss_Hpbar_FontUI::Create(m_pGraphicDev, 775.f, 100.f, 30.f, 30.f, true);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Boss_Hpbar_FontUI", pGameObject), );

			pGameObject = CBoss_Hpbar_FontUI::Create(m_pGraphicDev, 800.f, 100.f, 30.f, 30.f, false);
			NULL_CHECK_RETURN(pGameObject, );
			FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Boss_Hpbar_FontUI", pGameObject), );

			Engine::Emplace_Layer(L"Boss_Hpbar_UI", m_pUILayer);

			m_pGreenHpbar = dynamic_cast<CBoss_Hpbar_GreenUI*>(Engine::Get_GameObject(L"Boss_Hpbar_UI", L"Boss_Hpbar_GreenUI"));
			m_pYellowHpbar = dynamic_cast<CBoss_Hpbar_YellowUI*>(Engine::Get_GameObject(L"Boss_Hpbar_UI", L"Boss_Hpbar_YellowUI"));
			m_pRedHpbar = dynamic_cast<CBoss_Hpbar_RedUI*>(Engine::Get_GameObject(L"Boss_Hpbar_UI", L"Boss_Hpbar_RedUI"));
			m_pFontHpbar = dynamic_cast<CBoss_Hpbar_FontUI*>(Engine::Get_GameObject(L"Boss_Hpbar_UI", L"Boss_Hpbar_FontUI"));

			SoundGolemAtk;
			SoundMgrBGM(L"bgm_ep8_ahglan.wav", CSoundMgr::BGM);
			break;

		case LOW_HEALTH:
			m_eBossAction = BS_IDLE;
			break;

		case IDLE:
			m_eBossAction = BS_IDLE;
			break;

		case ATK_WINDMILL:
			m_eBossAction = BS_ATK;

			m_bCanAction = false;

			m_dwWindmillCoolDown = GetTickCount();
			m_dwCoolDownInterpol += 5000;
			m_dwWindmillDelay = m_dwCoolDownInterpol + rand() % 20000;

			m_fAniEndDelay = 1.06f;

			SoundGolemAtk;
			break;

		case ATK_TWOHANDS_COMBO:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(1.8f + m_fRandSpeed);
			BS_SKILL_MOVE(0.f, 3.5f, (_float)m_lfAniEnd * 0.4f);
			BS_SKILL_ROTATION(0.f, 180.f, (_float)m_lfAniEnd * 0.5f);

			m_fAniEndDelay = 1.f;
			m_bCanAction = false;

			SoundGolemAtk;
			break;

		case ATK_TWOHANDS:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(2.f + m_fRandSpeed);
			BS_SKILL_MOVE((_float)m_lfAniEnd * 0.05f, 4.f, (_float)m_lfAniEnd * 0.8f);
			BS_SKILL_ROTATION(0.f, 180.f, (_float)m_lfAniEnd * 0.8f);

			m_fAniEndDelay = 1.07f;
			m_bCanAction = false;

			SoundGolemAtk;
			break;

		case ATK_TURNRIGHT:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(2.3f + m_fRandSpeed);

			m_fAniEndDelay = 1.07f;
			m_bCanAction = false;

			SoundGolemAtk;
			break;

		case ATK_TURNLEFT:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(2.3f + m_fRandSpeed);

			m_fAniEndDelay = 1.07f;
			m_bCanAction = false;

			SoundGolemAtk;
			break;

		case ATK_STAMP:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(2.f + m_fRandSpeed);
			BS_SKILL_MOVE((_float)m_lfAniEnd * 0.05f, 4.f, (_float)m_lfAniEnd * 1.2f);
			BS_SKILL_ROTATION(0.f, 180.f, (_float)m_lfAniEnd * 0.9f);

			m_bCanAction = false;

			SoundGolemAtk;
			break;

		case ATK_ONETIME_STAMP:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(1.6f + m_fRandSpeed);
			BS_SKILL_MOVE((_float)m_lfAniEnd * 0.05f, 4.f, (_float)m_lfAniEnd * 0.25f);
			BS_SKILL_ROTATION(0.f, 180.f, (_float)m_lfAniEnd * 0.15f);

			m_fAniEndDelay = 1.08f;
			m_bCanAction = false;

			SoundGolemAtk;
			break;

		case ATK_THREETIME_STAMP:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(1.95f + m_fRandSpeed);
			BS_SKILL_MOVE((_float)m_lfAniEnd * 0.05f, 4.f, (_float)m_lfAniEnd * 1.2f);
			BS_SKILL_ROTATION(0.f, 180.f, (_float)m_lfAniEnd * 0.9f);

			m_fAniEndDelay = 1.05f;
			m_bCanAction = false;

			SoundGolemAtk;
			break;

		case ATK_ROLLING_TWICE:
			break;

		case ATK_ROLLING_ONETIME_END:
			m_pMeshCom->Set_TrackSpeed(2.1f);

			m_fAniEndDelay = 1.135f;
			m_bCanAction = false;
			break;

		case ATK_ROLLING_ONETIME_BEGIN:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(1.8f);
			m_dwRollingStartTime = GetTickCount();
			m_dwRollingAtkCoolDown = GetTickCount();
			m_dwRollingAtkDelay = m_dwCoolDownInterpol + rand() % 10000;

			m_fAniEndDelay = 0.95f;
			m_bCanAction = false;

			SoundGolemAtk;
			break;

		case ATK_ONEHAND:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(2.f + m_fRandSpeed);
			BS_SKILL_MOVE((_float)m_lfAniEnd * 0.45f, 2.f, (_float)m_lfAniEnd * 0.7f);
			BS_SKILL_ROTATION(0.f, 180.f, (_float)m_lfAniEnd * 0.5f);

			m_fAniEndDelay = 1.055f;
			m_bCanAction = false;

			SoundGolemAtk;
			break;

		case DAMAGE_FROM_FRONT:
			m_eBossAction = BS_DAMAGED;

			m_pMeshCom->Set_TrackSpeed(1.2f);
			BS_SKILL_MOVE((_float)m_lfAniEnd * 0.1f, -0.8f, (_float)m_lfAniEnd * 0.5f);

			m_fAniEndDelay = 0.86f;
			m_bCanAction = false;
			break;

		case DAMAGE_FROM_BACK:
			m_eBossAction = BS_DAMAGED;

			m_pMeshCom->Set_TrackSpeed(1.8f);
			BS_SKILL_MOVE((_float)m_lfAniEnd * 0.1f, 0.8f, (_float)m_lfAniEnd * 0.5f);

			m_fAniEndDelay = 0.92f;
			m_bCanAction = false;
			break;

		case ENTRY_IDLE:
			m_eBossAction = BS_ENTRY;
			m_lfAniEnd = 1.f;
			break;

		case ENTRY_CONTACT:
			m_pMeshCom->Set_TrackSpeed(1.85f);
			m_fAniEndDelay = 1.08f;
			break;
		}

		m_ePreState = m_eCurState;
	}


	// 상태 변경 시 매 프레임 실행
	switch (m_eCurState)
	{
	case WALK:
		m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(&m_vMyPos, &m_vDir, m_fSpeed, m_fTimeDelta));

		if (5.f <= m_fAngle)
		{
			if (m_bTargetIsRight)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * m_fTimeDelta));
			}
			else
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * -m_fTimeDelta));
			}
		}

		if (DIS_SHORT > m_fDistance)
		{
			// 강제로 다음 행동이 ATK을 수행하게 한다. 
			m_eBossAction = BS_ATK;
		}
		break;

	case TURN_RIGHT:
		if (5.f <= m_fAngle)
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * m_fTimeDelta));
		break;

	case TURN_LEFT:
		if (5.f <= m_fAngle)
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * -m_fTimeDelta));
		break;

	case TAUNT:
		if (!m_bAtkSound &&
			m_fAniTime >= m_lfAniEnd * 0.2f)
		{
			m_bAtkSound = true;
			SoundMgr(L"golem_attack02.wav", CSoundMgr::MONSTER);
		}
		break;

	case SPAWN:
		break;

	case LOW_HEALTH:
		break;

	case IDLE:
		if (DIS_SHORT > m_fDistance)
		{
			// 강제로 다음 행동이 ATK을 수행하게 한다. 
			m_eBossAction = BS_ATK;
		}
		break;

	case ATK_WINDMILL:
		if (!m_bEffectSound &&
			m_fAniTime <= m_lfAniEnd * 0.3f)
		{
			m_bEffectSound = true;
			SoundMgr(L"golem_windmill.wav", CSoundMgr::MONSTER);
		}
		if (m_fAniTime <= m_lfAniEnd * 0.3f)
		{
			m_pMainCamera->Set_CameraShake(true, CAMSHAKE_POWER * 0.5f, 4000, 3.f);
		}
		break;

	case ATK_TWOHANDS_COMBO:
		if (!m_bEffectSound &&
			m_fAniTime >= m_lfAniEnd * 0.36f)
		{
			m_bEffectSound = true;
			SoundMgr(L"step_lv6.wav", CSoundMgr::MONSTER3);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.5f);
		}
		break;

	case ATK_TWOHANDS:
		if (!m_bEffectSound &&
			m_fAniTime >= m_lfAniEnd * 0.42f)
		{
			m_bEffectSound = true;
			SoundMgr(L"step_lv6.wav", CSoundMgr::MONSTER3);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.f);
		}
		break;

	case ATK_TURNRIGHT:
		if (!m_bEffectSound &&
			m_fAniTime >= m_lfAniEnd * 0.4f)
		{
			m_bEffectSound = true;
			SoundMgr(L"swing_lv7.wav", CSoundMgr::MONSTER3);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.f);
		}
		break;

	case ATK_TURNLEFT:
		if (!m_bEffectSound &&
			m_fAniTime >= m_lfAniEnd * 0.4f)
		{
			m_bEffectSound = true;
			SoundMgr(L"swing_lv7.wav", CSoundMgr::MONSTER3);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.f);
		}
		break;

	case ATK_STAMP:
		if (!m_bStampAtkSound[0] &&
			m_fAniTime >= m_lfAniEnd * 0.15f)
		{
			m_bStampAtkSound[0] = true;
			SoundMgr(L"step_lv4.wav", CSoundMgr::MONSTER2);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.5f);
		}
		else if (!m_bStampAtkSound[1] &&
			m_fAniTime >= m_lfAniEnd * 0.37f)
		{
			m_bStampAtkSound[1] = true;
			SoundMgr(L"step_lv4.wav", CSoundMgr::MONSTER3);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.5f);
		}
		else if (!m_bStampAtkSound[2] &&
			m_fAniTime >= m_lfAniEnd * 0.63f)
		{
			m_bStampAtkSound[2] = true;
			SoundMgr(L"step_lv5.wav", CSoundMgr::MONSTER2);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.5f);
		}
		break;

	case ATK_ONETIME_STAMP:
		if (!m_bStampAtkSound[0] &&
			m_fAniTime >= m_lfAniEnd * 0.83f)
		{
			m_bStampAtkSound[0] = true;
			SoundMgr(L"step_lv4.wav", CSoundMgr::MONSTER2);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.5f);
		}
		break;

	case ATK_THREETIME_STAMP:
		if (!m_bStampAtkSound[0] &&
			m_fAniTime >= m_lfAniEnd * 0.29f)
		{
			m_bStampAtkSound[0] = true;
			SoundMgr(L"step_lv4.wav", CSoundMgr::MONSTER2);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.5f);
		}
		else if (!m_bStampAtkSound[1] &&
			m_fAniTime >= m_lfAniEnd * 0.6f)
		{
			m_bStampAtkSound[1] = true;
			SoundMgr(L"step_lv4.wav", CSoundMgr::MONSTER3);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.5f);
		}
		else if (!m_bStampAtkSound[2] &&
			m_fAniTime >= m_lfAniEnd * 1.f)
		{
			m_bStampAtkSound[2] = true;
			SoundMgr(L"step_lv5.wav", CSoundMgr::MONSTER2);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.5f);
		}
		break;

	case ATK_ROLLING_TWICE:
		break;

	case ATK_ROLLING_ONETIME_END:
		break;

	case ATK_ROLLING_ONETIME_BEGIN:
		if (m_dwRollingStartTime + 1500 > GetTickCount())
		{
			m_pMainCamera->Set_CameraShake(true, CAMSHAKE_POWER * 0.8f, 1200, 4.2f);
		}
		if (m_dwRollingStartTime + 1800 > GetTickCount())
		{
			if (10.f <= m_fAngle)
			{
				if (m_bTargetIsRight)
				{
					m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(270.f * m_fTimeDelta));
				}
				else
				{
					m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-270.f * m_fTimeDelta));
				}
			}
		}
		if (m_dwRollingStartTime + 1750 < GetTickCount() &&
			m_dwRollingStartTime + 2700 > GetTickCount())
		{
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(&m_vMyPos, &m_vDir, 22.5f, m_fTimeDelta));
		}
		break;

	case ATK_ONEHAND:
		if (!m_bEffectSound &&
			m_fAniTime >= m_lfAniEnd * 0.35f)
		{
			m_bEffectSound = true;
			SoundMgr(L"step_lv6.wav", CSoundMgr::MONSTER3);
			m_pMainCamera->Set_CameraShake(false, CAMSHAKE_POWER * 2.5f);
		}
		break;

	case DAMAGE_FROM_FRONT:
		break;

	case DAMAGE_FROM_BACK:
		break;

	case ENTRY_IDLE:
		Contact();
		break;

	case ENTRY_CONTACT:
		break;

	default:
		break;
	}

	if (m_fAniTime >= m_lfAniEnd * m_fAniEndDelay + m_fRandSpeed)
	{
		_uint	iRandomPattern = rand() % 2;

		if (m_iAniIndex == (_uint)WALK || 
			m_iAniIndex == (_uint)IDLE)
		{
			m_bAnimation = true;
		}
		else
		{
			m_bAnimation = false;
		}

		if (m_iAniIndex == (_uint)ATK_ROLLING_ONETIME_BEGIN)
		{
			m_iAniIndex = (_uint)ATK_ROLLING_ONETIME_END;
		}
		else if (m_iAniIndex == (_uint)ATK_ONETIME_STAMP &&
				 0 == iRandomPattern)
		{
			if (m_bTargetIsRight)
			{
				m_iAniIndex = (_uint)ATK_TURNRIGHT;
				Animation_Control();
			}
			else
			{
				m_iAniIndex = (_uint)ATK_TURNLEFT;
				Animation_Control();
			}
		}
		else if (m_iAniIndex == (_uint)ATK_ONETIME_STAMP &&
			1 == iRandomPattern)
		{
			m_iAniIndex = (_uint)ATK_TWOHANDS_COMBO;
			Animation_Control();
		}
		else if (m_iAniIndex == (_uint)ATK_THREETIME_STAMP &&
			0 == iRandomPattern)
		{
			if (m_bTargetIsRight)
			{
				m_iAniIndex = (_uint)ATK_TURNRIGHT;
				Animation_Control();
			}
			else
			{
				m_iAniIndex = (_uint)ATK_TURNLEFT;
				Animation_Control();
			}
		}
		else if (m_iAniIndex == (_uint)ATK_THREETIME_STAMP &&
			1 == iRandomPattern)
		{
			m_iAniIndex = (_uint)ATK_TWOHANDS;
			Animation_Control();
		}
		else if (m_iAniIndex == (_uint)TAUNT &&
			60.f <= m_fRand)
		{
			m_iAniIndex = ATK_TWOHANDS_COMBO;

			m_fRand = Engine::Random(0.f, 100.f);
			Animation_Control();
		}
		else if (m_iAniIndex != (_uint)ENTRY_IDLE &&
			m_iAniIndex != (_uint)ENTRY_CONTACT &&
			m_iAniIndex != (_uint)WALK)
		{
			if (m_iAniIndex == (_uint)ATK_TURNLEFT)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-90.f));
			}
			else if (m_iAniIndex == (_uint)ATK_TURNRIGHT)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(90.f));
			}

			m_fRand = Engine::Random(0.f, 100.f);

			m_iAniIndex = (_uint)IDLE;
			m_eBossAction = BS_IDLE;

			m_pMeshCom->Set_TrackSpeed(2.f);
			m_fRandSpeed = (rand() % 300) * 0.001f;

			m_bCanAction = true;
		}
		else if (m_iAniIndex == (_uint)ENTRY_CONTACT)
		{
			m_iAniIndex = (_uint)SPAWN;
		}
	}
}

void CAhglan::Collision_Control()
{
	_float	fAniTime = m_pMeshCom->Get_AniFrameTime();

	map<const wstring, CBoxCollider*>::iterator	iter_Damaged = m_mapBoxColliderCom.begin();
	map<const wstring, CCollider*>::iterator	iter_Hit = m_mapColliderCom.begin();

	// 부위파괴되면 충돌체 삭제 
	//map<const wstring, _bool>::iterator			iter_ActiveParts = m_mapActiveParts.begin();

	//for (; iter_ActiveParts != m_mapActiveParts.end(); ++iter_ActiveParts)
	//{
	//	if (!iter_ActiveParts->second)
	//	{
	//		for (; iter_Damaged != m_mapBoxColliderCom.end(); ++iter_Damaged)
	//		{
	//			if (iter_Damaged->first == iter)
	//		}
	//	}
	//}

	// DamagedBox
	if (!m_mapBoxColliderCom.empty())
	{
		for (; iter_Damaged != m_mapBoxColliderCom.end(); ++iter_Damaged)
		{
			if (ATK_WINDMILL == m_iAniIndex)
			{
				iter_Damaged->second->Set_CanCollision(false);
			}
			else
			{
				iter_Damaged->second->Set_CanCollision(true);
			}
		}
	}

	// HitBox
	switch (m_eCurState)
	{
	case CAhglan::ATK_WINDMILL:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.13f, m_lfAniEnd * 0.7f);
		}
		break;

	case CAhglan::ATK_TWOHANDS_COMBO:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_BodyAtk" == iter_Hit->first ||
				L"Hit_LHand" == iter_Hit->first ||
				L"Hit_RHand" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.3f, m_lfAniEnd * 0.4f);
			}
		}
		break;

	case CAhglan::ATK_TWOHANDS:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_BodyAtk" == iter_Hit->first ||
				L"Hit_LHand" == iter_Hit->first ||
				L"Hit_RHand" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.4f, m_lfAniEnd * 0.5f);
			}
		}
		break;

	case CAhglan::ATK_TURNRIGHT:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_LHand" == iter_Hit->first ||
				L"Hit_RHand" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.35f, m_lfAniEnd * 0.55f);
			}
		}
		break;

	case CAhglan::ATK_TURNLEFT:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_LHand" == iter_Hit->first ||
				L"Hit_RHand" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.4f, m_lfAniEnd * 0.65f);
			}
		}
		break;

	case CAhglan::ATK_STAMP:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_LFoot" == iter_Hit->first)
			{
				if (m_lfAniEnd * 0.1f <= fAniTime &&
					m_lfAniEnd * 0.21f >= fAniTime)
				{
					iter_Hit->second->Set_CanCollision(true);
				}
				else if (m_lfAniEnd * 0.53f <= fAniTime &&
					m_lfAniEnd * 0.65f >= fAniTime)
				{
					iter_Hit->second->Set_CanCollision(true);
				}
				else
				{
					iter_Hit->second->Set_CanCollision(false);
				}
			}
			else if (L"Hit_RFoot" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.35f, m_lfAniEnd * 0.42f);
			}
		}
		break;

	case CAhglan::ATK_ONETIME_STAMP:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_LFoot" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.6f, m_lfAniEnd * 0.83f);
			}
		}
		break;

	case CAhglan::ATK_THREETIME_STAMP:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_LFoot" == iter_Hit->first)
			{
				if (m_lfAniEnd * 0.12f <= fAniTime &&
					m_lfAniEnd * 0.29f >= fAniTime)
				{
					iter_Hit->second->Set_CanCollision(true);
				}
				else if (m_lfAniEnd * 0.85f <= fAniTime &&
					m_lfAniEnd * 1.f >= fAniTime)
				{
					iter_Hit->second->Set_CanCollision(true);
				}
				else
				{
					iter_Hit->second->Set_CanCollision(false);
				}
			}
			else if (L"Hit_RFoot" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.5f, m_lfAniEnd * 0.6f);
			}
		}
		break;

	case CAhglan::ATK_ROLLING_TWICE:
		break;

	case CAhglan::ATK_ROLLING_ONETIME_END:
		break;

	case CAhglan::ATK_ROLLING_ONETIME_BEGIN:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.55f, m_lfAniEnd * 0.95f);
		}
		break;

	case CAhglan::ATK_ONEHAND:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_RHand" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.3f, m_lfAniEnd * 0.42f);
			}
		}
		break;

	default:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			iter_Hit->second->Set_CanCollision(false);
		}
		break;
	}
}

void CAhglan::Update_UI()
{
	//_int	iDamaged = m_tInfo.iMaxHp - m_tInfo.iHp;
	//iDamaged %= _int(m_tInfo.iMaxHp * 0.1f);

	//if (m_iPreHp != m_tInfo.iHp)
	//{
	//	iDamaged = m_iPreHp - m_tInfo.iHp;

	//	if (iDamaged <= m_iLineHp)
	//	{
	//		m_iLineHp -= iDamaged;
	//	}
	//	else
	//	{
	//		iDamaged -= m_iLineHp;
	//		m_iLineHp = m_iMaxLineHp;
	//		m_iLineHp -= iDamaged;
	//	}

	//	m_iPreHp = m_tInfo.iHp;
	//}

	_float	fHpRatio = (_float)m_tInfo.iHp / (_float)m_tInfo.iMaxHp;

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
}

HRESULT CAhglan::Add_NaviMesh()
{
	CComponent*		pComponent = nullptr;

	pComponent = m_pNaviMeshCom = dynamic_cast<CNaviMesh*>(Engine::Clone_Prototype(L"Proto_NaviMesh"));
	NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	return S_OK;
}

void CAhglan::BombAttacked(const _vec3& vBombPos, const wstring& wstrPartsName)
{
	_bool	bFront = false;
	_vec3	vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
	_vec3	vDir = vBombPos - vMyPos;

	D3DXVec3Normalize(&vDir, &vDir);

	map<const wstring, _bool>::iterator		iter = m_mapActiveParts.begin();

	if (0.f < D3DXVec3Dot(&vDir, &(-(*m_pTransformCom->Get_Info(INFO_LOOK)))))
	{
		bFront = true;
		m_iAniIndex = DAMAGE_FROM_FRONT;
	}
	else
	{
		bFront = false;
		m_iAniIndex = DAMAGE_FROM_BACK;
	}

	if (L"golem_ahglan_Chest.tga" == wstrPartsName)
	{
		for (; iter != m_mapActiveParts.end(); ++iter)
		{
			if (bFront &&
				iter->first == wstrPartsName)
			{
				if (iter->second)
				{
					iter->second = FALSE;
					break;
				}
				else
				{
					map<const wstring, _bool>::iterator		iter_Begin = m_mapActiveParts.begin();

					for (; iter_Begin != m_mapActiveParts.end(); ++iter_Begin)
					{
						if (L"golem_ahglan_ore_tga" == iter_Begin->first)
						{
							iter_Begin->second = FALSE;
							break;
						}
					}
				}
			}
		}
	}
	else if (L"golem_ahglan_Head.tga" == wstrPartsName)
	{
		for (; iter != m_mapActiveParts.end(); ++iter)
		{
			if (iter->first == wstrPartsName)
			{
				iter->second = FALSE;
				break;
			}
		}
	}
	else if (L"golem_ahglan_RHand.tga" == wstrPartsName)
	{
		for (; iter != m_mapActiveParts.end(); ++iter)
		{
			if (iter->first == wstrPartsName)
			{
				iter->second = FALSE;
				break;
			}
		}
	}

	Animation_Control();
}


CAhglan * CAhglan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAhglan*	pInstance = new CAhglan(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CAhglan::Free(void)
{
	Safe_Release(m_pUILayer);

	CGameObject::Free();
}
