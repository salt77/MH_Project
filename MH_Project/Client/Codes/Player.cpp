#include "stdafx.h"
#include "Player.h"

#include "Export_Function.h"
#include "DynamicCamera.h"
#include "Ahglan.h"
#include "StickyBomb.h"
#include "Trail_Sword.h"

#include "Player_Hpbar_BackUI.h"
#include "Player_Hpbar_ValueUI.h"
#include "Player_Hpbar_LerpUI.h"
#include "Player_Steminabar_BackUI.h"
#include "Player_Steminabar_ValueUI.h"
#include "Player_Spbar_BackUI.h"
#include "Player_Spbar_ValueUI.h"
#include "Player_Buff_CriticalUI.h"
#include "DamageFont.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
	ZeroMemory(&m_tPlayerInfo, sizeof(PL_INFO));
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

CPlayer::~CPlayer(void)
{
}


HRESULT CPlayer::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(0.f, 0.f, 10.f);
	m_pTransformCom->Set_Scale(SCALE_PLAYER, SCALE_PLAYER, SCALE_PLAYER);
	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(270.f));

	//m_pNaviMeshCom->Set_CellIndex(0);

	m_vDir = *m_pTransformCom->Get_Info(INFO_LOOK);
	m_pTransformCom->Update_Component(0.f);

	m_tPlayerInfo.tagInfo.iHp = 10000;
	m_tPlayerInfo.tagInfo.iMaxHp = m_tPlayerInfo.tagInfo.iHp;
	m_tPlayerInfo.fStamina = 300.f;
	m_tPlayerInfo.fMaxStamina = m_tPlayerInfo.fStamina;
	m_tPlayerInfo.iSkillPoint = 0;
	m_tPlayerInfo.iMaxSkillPoint = 1000;

	return S_OK;
}

HRESULT CPlayer::LateReady_Object()
{
	m_pOtherLayer = CLayer::Create();
	NULL_CHECK_RETURN(m_pOtherLayer, E_FAIL);
	m_pUILayer = CLayer::Create();
	NULL_CHECK_RETURN(m_pUILayer, E_FAIL);

	CGameObject*			pGameObject = nullptr;

	// Hpbar
	pGameObject = CPlayer_Hpbar_BackUI::Create(m_pGraphicDev, 340.f, WINCY - 135.f, 650.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Hpbar_BackUI", pGameObject), E_FAIL);

	pGameObject = CPlayer_Hpbar_LerpUI::Create(m_pGraphicDev, 340.f, WINCY - 135.f, 650.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Hpbar_LerpUI", pGameObject), E_FAIL);

	pGameObject = CPlayer_Hpbar_ValueUI::Create(m_pGraphicDev, 340.f, WINCY - 135.f, 650.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Hpbar_ValueUI", pGameObject), E_FAIL);

	// Steminabar
	pGameObject = CPlayer_Steminabar_BackUI::Create(m_pGraphicDev, 340.f, WINCY - 115.f, 650.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Steminabar_BackUI", pGameObject), E_FAIL);

	pGameObject = CPlayer_Steminabar_ValueUI::Create(m_pGraphicDev, 340.f, WINCY - 115.f, 650.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Steminabar_ValueUI", pGameObject), E_FAIL);

	// Spbar
	pGameObject = CPlayer_Spbar_BackUI::Create(m_pGraphicDev, 210.f, WINCY - 100.f, 350.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Spbar_BackUI", pGameObject), E_FAIL);

	pGameObject = CPlayer_Spbar_ValueUI::Create(m_pGraphicDev, 210.f, WINCY - 100.f, 350.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Spbar_ValueUI", pGameObject), E_FAIL);

	Engine::Emplace_Layer(L"Player_UI", m_pUILayer);


	// Trail_Sword
	pGameObject = CTrail_Sword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pOtherLayer->Add_GameObject(L"Player_Sword_Trail", pGameObject), E_FAIL);

	Engine::Emplace_Layer(L"Player_Sword_Trail", m_pOtherLayer);


	m_mapActiveParts.emplace(L"dualsword_vanquisher.tga", TRUE);
	m_mapActiveParts.emplace(L"sticky_bomb.tga", FALSE);

	m_pAhglan = dynamic_cast<CAhglan*>(Engine::Get_GameObject(L"GameLogic", L"Ahglan"));

	m_pTrailSword = dynamic_cast<CTrail_Sword*>(Engine::Get_GameObject(L"Player_Sword_Trail", L"Player_Sword_Trail"));

	m_pHpbarValueUI = dynamic_cast<CPlayer_Hpbar_ValueUI*>(Engine::Get_GameObject(L"Player_UI", L"Player_Hpbar_ValueUI"));
	m_pHpbarLerpUI = dynamic_cast<CPlayer_Hpbar_LerpUI*>(Engine::Get_GameObject(L"Player_UI", L"Player_Hpbar_LerpUI"));
	m_pSteminabarValueUI = dynamic_cast<CPlayer_Steminabar_ValueUI*>(Engine::Get_GameObject(L"Player_UI", L"Player_Steminabar_ValueUI"));
	m_pSpbarValueUI = dynamic_cast<CPlayer_Spbar_ValueUI*>(Engine::Get_GameObject(L"Player_UI", L"Player_Spbar_ValueUI"));

	m_pNaviMeshCom->Set_CellIndex(0);

	return S_OK;
}

_int CPlayer::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	if (m_bDead)
		return iExit;

	// Debug 용
	if (Key_Down('J'))
	{
		m_tPlayerInfo.iSkillPoint += 250;
	}

	m_pMainCam = static_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));

	//SetUp_OnTerrain();
	Compute_CanAction();
	Compute_Buff();
	Key_Input(fTimeDelta);
	SecondaryMode_MouseMove();
	Weapon_Change();

	Animation_Control();
	Collision_Control();
	Update_State();
	Update_UI();
	FootStepSound();
	MoveOn_Skill(fTimeDelta);
	Make_TrailEffect(fTimeDelta);

	/*_int iExit = CGameObject::Update_Object(fTimeDelta);*/
	StopMotion();

	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	if (!m_bStopMotion && m_bAnimation)
		m_pMeshCom->Play_Animation(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

_int CPlayer::LateUpdate_Object(const _float & fTimeDelta)
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
			if (L"Other_Attack" == iter->first)
			{
				CTransform*	pHitBoxPosTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"HitBox_Pos", L"Com_Transform", ID_DYNAMIC));

				iter->second->Set_ColliderMatrix(pHitBoxPosTrans->Get_WorldMatrix());
			}
			else
			{
				iter->second->Set_ColliderMatrix(m_pTransformCom->Get_WorldMatrix());
			}
		}
	}

	return iExit;
}

void CPlayer::Render_Object(void)
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
				if (L"Other_Attack" == iter->first)
				{
					CTransform*	pHitBoxPosTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"HitBox_Pos", L"Com_Transform", ID_DYNAMIC));

					iter->second->Render_Collider(COL_FALSE, pHitBoxPosTrans->Get_WorldMatrix());
				}
				else
				{
					iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
				}
			}
		}
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	if (m_pNaviMeshCom)
		m_pNaviMeshCom->Render_NaviMesh();

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

void CPlayer::Set_SpPoint(_bool bIsSmash)
{
	if (bIsSmash)
	{
		if (m_tPlayerInfo.iSkillPoint + (rand() % 50 + 5) < m_tPlayerInfo.iMaxSkillPoint)
		{
			m_tPlayerInfo.iSkillPoint += rand() % 50 + 5;
		}
		else
		{
			m_tPlayerInfo.iSkillPoint = m_tPlayerInfo.iMaxSkillPoint;
		}
	}
	else
	{
		if (m_tPlayerInfo.iSkillPoint + (rand() % 5 + 1) < m_tPlayerInfo.iMaxSkillPoint)
		{																				
			m_tPlayerInfo.iSkillPoint += rand() % 5 + 1;								
		}																				
		else																			
		{																				
			m_tPlayerInfo.iSkillPoint = m_tPlayerInfo.iMaxSkillPoint;					
		}
	}
}

void CPlayer::Set_StopMotion(_bool bStopMotion, _ulong dwTime)
{
	if (bStopMotion)
	{
		STOP_MOTION(dwTime);
	}
	else
	{
		STOP_MOTION_END;
	}
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer*	pInstance = new CPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CPlayer::Free(void)
{
	for_each(m_listBuff.begin(), m_listBuff.end(), CDeleteList());

	Safe_Release(m_pUILayer);
	Safe_Release(m_pOtherLayer);

	CGameObject::Free();
}

void CPlayer::Set_Damage(_int iDamage, const _matrix* pMatDamageFontPos, _bool bFront)
{
	if (iDamage < m_tPlayerInfo.tagInfo.iHp)
	{
		m_tPlayerInfo.tagInfo.iHp -= iDamage;

		if (80.f <= Engine::Random(0.f, 100.f))
		{
			m_eCurAction = PL_DAMAGED;
			m_iAniIndex = STATE_DAMAGE_RESIST;

			if (!m_bLethitaSound[0])
			{
				m_bLethitaSound[0] = true;
				SoundMgr(L"Hit_Flesh_Blunt.wav", CSoundMgr::PLAYER);

				Pooling_DamageFont(iDamage, pMatDamageFontPos, DAMAGEFONT_MONSTER);
			}
		}
		else
		{
			if (bFront)
			{
				m_iAniIndex = STATE_DAMAGEFROM_FRONT;
			}
			else
			{
				m_iAniIndex = STATE_DAMAGEFROM_BACK;
			}

			m_eCurAction = PL_DAMAGED;

			if (!m_bLethitaSound[0])
			{
				m_bLethitaSound[0] = true;
				SoundMgr(L"Hit_Flesh_Blunt.wav", CSoundMgr::PLAYER);

				Pooling_DamageFont(iDamage, pMatDamageFontPos, DAMAGEFONT_MONSTER);
			}
		}
	}
	else
	{
		m_eCurAction = PL_DEAD;
		m_tPlayerInfo.tagInfo.iHp = 0;

		if (bFront)
		{
			m_iAniIndex = STATE_DAMAGEFROM_FRONT;
		}
		else
		{
			m_iAniIndex = STATE_DAMAGEFROM_BACK;
		}
	}

	if (CDynamicCamera::MODE_NORMAL != m_pMainCam->Get_CamMode() ||
		WEAPON_DUALSWORD != m_eCurWeaponMode)
	{
		m_pMainCam->Set_CameraMode(CDynamicCamera::MODE_NORMAL);
		m_eCurWeaponMode = WEAPON_DUALSWORD;
	}
}

HRESULT CPlayer::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CDynamicMesh*>(Clone_Prototype(L"Proto_Mesh_Player"));
	NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Clone_Prototype(L"Proto_Calculator"));
	NULL_CHECK_RETURN(m_pCalculatorCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Calculator", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Player"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

HRESULT CPlayer::Add_NaviMesh()
{
	CComponent*		pComponent = nullptr;

	pComponent = m_pNaviMeshCom = dynamic_cast<CNaviMesh*>(Clone_Prototype(L"Proto_NaviMesh"));
	NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	//m_pTransformCom->Get_INFO(INFO_LOOK, &m_vDir);
	//m_pTransformCom->Get_INFO(INFO_RIGHT, &m_vRightDir);\

	_vec3 vMoveDir = _vec3(0.f, 0.f, 0.f);
	_vec3 m_vLookDir = m_pMainCam->Get_CamDirVector(DIR_LOOK);
	_vec3 m_vRightDir = m_pMainCam->Get_CamDirVector(DIR_RIGHT);

	// 두 가지 이상의 키 값을 누를 때
	if (Key_Pressing('W') && Key_Pressing('A'))
	{
		vMoveDir = m_vDir = m_vLookDir - m_vRightDir;
	}
	else if (Key_Pressing('A') && Key_Pressing('S'))
	{
		vMoveDir = m_vDir = -m_vLookDir - m_vRightDir;
	}
	else if (Key_Pressing('S') && Key_Pressing('D'))
	{
		vMoveDir = m_vDir = -m_vLookDir + m_vRightDir;
	}
	else if (Key_Pressing('W') && Key_Pressing('D'))
	{
		vMoveDir = m_vDir = m_vLookDir + m_vRightDir;
	}
	// 한 개의 키 값만 누를 때
	else
	{
		if (Key_Pressing('W'))
		{
			vMoveDir = m_vDir = m_vLookDir;
		}
		else if (Key_Pressing('A'))
		{
			vMoveDir = m_vDir = -m_vRightDir;
		}
		else if (Key_Pressing('S'))
		{
			vMoveDir = m_vDir = -m_vLookDir;
		}
		else if (Key_Pressing('D'))
		{
			vMoveDir = m_vDir = m_vRightDir;
		}
		else if (PL_MOVE >= m_eCurAction)
		{
			m_eCurAction = PL_IDLE;
		}
	}

	// Damaged
	if (PL_DAMAGED >= m_eCurAction)
	{
		if (Key_Down('V'))
		{
			if (m_bCanAction)
			{
				if (STATE_DAMAGEFROM_FRONT == m_iAniIndex)
				{
					m_iAniIndex = STATE_DOWNTOIDLE_BACK;
				}
				else if (STATE_DAMAGEFROM_BACK == m_iAniIndex)
				{
					m_iAniIndex = STATE_DOWNTOIDLE_BACK;
				}
			}
		}
	}

	// 스테미나 x 
	// => Key_Input()보단 Animation_Control()이 더 맞을 것 같아서 이동함. 
	/*if (PL_TIRED >= m_eCurAction)
	{
		if (m_iAniIndex != STATE_TIRED_DURING)
		{
			if (0.f >= m_tPlayerInfo.fStamina)
			{
				m_iAniIndex = STATE_TIRED_BEGIN;
			}
		}
	}*/

	// 스킬 액션(sp스킬, 보조무기 액션)
	if (PL_SKILL >= m_eCurAction)
	{
		if (Key_Down('Z'))
		{
			if (m_bCanAction)
			{
				if (PLAYER_SP_FEVER <= m_tPlayerInfo.iSkillPoint)
				{
					m_eCurAction = PL_SKILL;
					m_iAniIndex = STATE_SP_FEVER;

					m_tPlayerInfo.iSkillPoint -= PLAYER_SP_FEVER;
				}
			}
		}

		else if (Key_Down('F'))
		{
			if (SCENE_STAGE == Engine::Get_SceneID())
			{
				CStickyBomb*	pBomb = static_cast<CStickyBomb*>(Engine::Get_GameObject(L"StickyBomb", L"StickyBomb"));

				if (m_bCanAction)
				{
					if (STATE_THROW_DURING == m_eCurState)
					{
						m_eCurAction = PL_IDLE;
						m_eCurWeaponMode = WEAPON_DUALSWORD;

						Rotate_PlayerLook(+*m_pTransformCom->Get_Info(INFO_LOOK));
						m_pMainCam->Set_CameraMode(CDynamicCamera::MODE_NORMAL);
					}
					else
					{
						if (!pBomb)
						{
							m_eCurAction = PL_SKILL;
							m_iAniIndex = (_uint)STATE_THROW_DURING;

							m_pMainCam->Set_CameraMode(CDynamicCamera::MODE_SECONDARY);
						}
					}
				}
			}
		}
		else if (Key_Pressing(VK_LBUTTON))
		{
			if (m_bCanAction)
			{
				if (STATE_THROW_DURING == m_eCurState)
				{
					m_iAniIndex = (_uint)STATE_THROW_END;

					m_dwThrowStart = GetTickCount();
				}
			}
		}
	}

	// 스매쉬
	if (PL_SMASH >= m_eCurAction)
	{
		if (Key_Down(VK_RBUTTON))
		{
			if (m_bCanAction)
			{
				m_eCurAction = PL_SMASH;
				m_iAniIndex = m_eNextSmash;

				D3DXVec3Normalize(&vMoveDir, &vMoveDir);
				Rotate_PlayerLook(vMoveDir);
			}
		}
	}

	// 슬립대쉬
	if (PL_DASH >= m_eCurAction)
	{
		if (Key_Down(VK_SPACE))
		{
			if (m_bCanAction)
			{
				switch (m_iDashCount)
				{
				case 0:
					m_eCurAction = PL_DASH;
					m_iAniIndex = STATE_DASH_N;

					++m_iDashCount;
					break;

				case 1:
					m_eCurAction = PL_DASH;
					m_iAniIndex = STATE_DASH_S;

					m_iDashCount = 0;
					break;

				default:
					break;
				}
			}
		}
	}

	// 평타
	if (PL_ATK >= m_eCurAction)
	{
		if (Key_Down(VK_LBUTTON))
		{
			if (m_bCanAction)
			{
				m_eCurAction = PL_ATK;
				m_iAniIndex = m_eNextAtk;

				D3DXVec3Normalize(&vMoveDir, &vMoveDir);
				Rotate_PlayerLook(vMoveDir);
			}
		}
	}

	// 움직임
	if (PL_MOVE >= m_eCurAction &&
		vMoveDir != _vec3(0.f, 0.f, 0.f))
	{
		D3DXVec3Normalize(&vMoveDir, &vMoveDir);

		if (Key_Pressing(VK_SHIFT))
		{
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &vMoveDir, m_fSpeed * 1.35f, fTimeDelta));
			m_pMainCam->Sync_PlayerPos(vMoveDir, m_fSpeed * 1.35f, fTimeDelta);

			m_iAniIndex = STATE_SPRINT;
		}
		else
		{
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &vMoveDir, m_fSpeed, fTimeDelta));
			m_pMainCam->Sync_PlayerPos(vMoveDir, m_fSpeed, fTimeDelta);

			m_iAniIndex = STATE_RUN;
		}

		m_eCurAction = PL_MOVE;

		Rotate_PlayerLook(fTimeDelta, vMoveDir);
	}

	// 대기
	if (PL_IDLE >= m_eCurAction)
	{
		m_iAniIndex = STATE_IDLE;
	}
}

void CPlayer::SecondaryMode_MouseMove()
{
	if (STATE_THROW_DURING == m_eCurState)
	{
		_long	dwMouse = 0;

		if (dwMouse = Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(dwMouse * 0.1f));
		}
	}
}

//void CPlayer::SetUp_OnTerrain(void)
//{
//	_vec3		vPos;
//	m_pTransformCom->Get_INFO(INFO_POS, &vPos);
//
//	CTerrainTex*	pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Buffer", ID_STATIC));
//	NULL_CHECK(pTerrainBufferCom);
//
//	_float fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ);
//	m_pTransformCom->Set_Pos(vPos.x, fHeight, vPos.z);
//}
//
//Engine::_vec3 CPlayer::PickUp_OnTerrain(void)
//{
//	CTerrainTex*	pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Buffer", ID_STATIC));
//	NULL_CHECK_RETURN(pTerrainBufferCom, _vec3(0.f, 0.f, 0.f));
//
//	CTransform*	pTerrainTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Transform", ID_DYNAMIC));
//	NULL_CHECK_RETURN(pTerrainTransCom, _vec3(0.f, 0.f, 0.f));
//
//	return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransCom);
//}

void CPlayer::Compute_CanAction()
{
	if (m_eCurAction == PL_ATK)
	{
		if (STATE_ATK4 != m_iAniIndex)
		{
			if (m_fAniTime < (m_lfAniEnd * 0.32f))
			{
				m_bCanAction = false;
			}
			else
			{
				m_bCanAction = true;
			}
		}
		else if (STATE_ATK4 == m_iAniIndex)
		{
			if (m_fAniTime < (m_lfAniEnd * 0.5f))
			{
				m_bCanAction = false;
			}
			else
			{
				m_bCanAction = true;
			}
		}
	}
	else if (m_eCurAction == PL_DASH)
	{
		if (m_fAniTime < (m_lfAniEnd * 0.45f))
		{
			m_bCanAction = false;
		}
		else
		{
			m_bCanAction = true;
		}
	}
	else if (m_eCurAction == PL_SMASH ||
		m_eCurAction == PL_SKILL)
	{
		if (m_fAniTime < (m_lfAniEnd * 0.25f))
		{
			m_bCanAction = false;
		}
		else
		{
			m_bCanAction = true;
		}
	}
	else
	{
		m_bCanAction = true;
	}
}

void CPlayer::Compute_Buff()
{
	if (!m_listBuff.empty())
	{
		list<tag_BuffDeBuff*>::iterator		iter = m_listBuff.begin();

		for (; iter != m_listBuff.end(); )
		{
			if ((*iter)->dwBuffStartTime + (*iter)->dwBuffDuration < GetTickCount())
			{
				Safe_Delete(*iter);
				iter = m_listBuff.erase(iter);
			}

			switch ((*iter)->eBuffID)
			{
			case BUFF_CRITICAL:
				if (STATE_FURY != m_ePreState)
				{
					SetNextSmash(STATE_FURY, (*iter)->dwBuffDuration);
				}
				else
				{
					SetNextSmash(STATE_FURY2, (*iter)->dwBuffDuration);
				}

				Safe_Delete(*iter);
				iter = m_listBuff.erase(iter);
				break;

			default:
				break;
			}

			if (iter != m_listBuff.end())
			{
				++iter;
			}
		}
	}
}

void CPlayer::Compute_Critical()
{
	if (m_fCriticalPotential >= Engine::Random(0.f, 10.f))
	{
		Add_Buff(BUFF_CRITICAL, 1500);

		SoundMgrLowerVol(L"hit_common_critical.wav", CSoundMgr::BATTLE, 0.3f);
	}
}

void CPlayer::Add_Buff(BUFF_ID eID, _ulong dwBuffDuration)
{
	list<tag_BuffDeBuff*>::iterator		iter = m_listBuff.begin();

	for (; iter != m_listBuff.end(); )
	{
		if (eID == (*iter)->eBuffID)
		{
			Safe_Delete(*iter);
			iter = m_listBuff.erase(iter);

			m_listBuff.emplace_back(new tag_BuffDeBuff(eID, GetTickCount(), dwBuffDuration));
			break;
		}
		else
		{
			++iter;
		}
	}

	if (iter == m_listBuff.end())
	{
		m_listBuff.emplace_back(new tag_BuffDeBuff(eID, GetTickCount(), dwBuffDuration));
	}
}

void CPlayer::Rotate_PlayerLook(const _float& fTimeDelta, _vec3& TargetLookVector)
{
	_vec3 vUp = _vec3(0.f, 1.f, 0.f);
	_vec3 vPlayerRight = *m_pTransformCom->Get_Info(INFO_RIGHT);
	_vec3 vTemp;

	D3DXVec3Normalize(&TargetLookVector, &TargetLookVector);
	D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);

	_float	fAngle = D3DXToDegree(acos(D3DXVec3Dot(&TargetLookVector, &-vPlayerRight)));

	if (fAngle > 5.f)
	{
		if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) > 0.f)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(360.f * fTimeDelta * 2.25f));
		}
		else if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) < 0.f)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(360.f * -fTimeDelta * 2.25f));
		}
	}
	else if (fAngle <= 5.f)
	{
		if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) > 0.f)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(45.f * fTimeDelta));
		}
		else if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) < 0.f)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(45.f * -fTimeDelta));
		}
	}
}

void CPlayer::Rotate_PlayerLook(_vec3 & TargetLookVector)
{
	_vec3 vPlayerRight = *m_pTransformCom->Get_Info(INFO_RIGHT);
	_vec3 vTemp;

	D3DXVec3Normalize(&TargetLookVector, &TargetLookVector);
	D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);

	if (D3DXToDegree(acos(D3DXVec3Dot(&TargetLookVector, &-vPlayerRight))) > 5.f)
	{
		if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) > 0.f)
		{
			m_pTransformCom->Rotation(ROT_Y, acos(D3DXVec3Dot(&TargetLookVector, &-vPlayerRight)));
		}
		else if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) < 0.f)
		{
			m_pTransformCom->Rotation(ROT_Y, -acos(D3DXVec3Dot(&TargetLookVector, &-vPlayerRight)));
		}
	}
}

//void CPlayer::MoveOn_Skill(const _float& fTimeDelta)
//{
//	if (m_bSkillMove)
//	{
//		if (m_dwSkillMoveReady + m_dwSkillMoveReadyTime < GetTickCount())
//		{
//			m_dwSkillMoveReady = GetTickCount();
//			m_dwSkillMoveReadyTime = INFINITY_INT;
//			m_dwSkillMoveStart = GetTickCount();
//		}
//
//		if (m_dwSkillMoveStart + m_dwSkillMoveTime >= GetTickCount())
//		{
//			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &(-*m_pTransformCom->Get_Info(INFO_RIGHT)), m_fSkillMoveSpeed, fTimeDelta));
//			m_pMainCam->Sync_PlayerPos(-*m_pTransformCom->Get_Info(INFO_RIGHT), m_fSkillMoveSpeed, fTimeDelta);
//		}
//		else if (m_dwSkillMoveReady + m_dwSkillMoveReadyTime < GetTickCount())
//		{
//			m_bSkillMove = false;
//		}
//	}
//}

void CPlayer::MoveOn_Skill(const _float & fTimeDelta)
{
	if (m_bSkillMove && !m_bStopMotion)
	{
		if (PL_TIRED != m_eCurAction)
		{
			if (m_fSkillMoveStartTime <= m_fAniTime &&
				m_fSkillMoveEndTime >= m_fAniTime)
			{
				m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &(-*m_pTransformCom->Get_Info(INFO_RIGHT)), m_fSkillMoveSpeed, fTimeDelta));
				m_pMainCam->Sync_PlayerPos(-*m_pTransformCom->Get_Info(INFO_RIGHT), m_fSkillMoveSpeed, fTimeDelta);
			}
			else if (m_fSkillMoveEndTime < m_fAniTime)
			{
				m_bSkillMove = false;
			}
		}
	}
}

void CPlayer::StopMotion()
{
	if (m_bStopMotion)
	{
		if (m_dwStopMotionStart + m_dwStopMotionTime < GetTickCount())
		{
			STOP_MOTION_END;
		}
	}
}

void CPlayer::FootStepSound()
{
	if (STATE_SPRINT == m_eCurState &&
		m_dwFootStepStart + m_dwFootStepDelay <= GetTickCount())
	{
		m_dwFootStepStart = GetTickCount();

		_uint	iRand = rand() % 4;

		switch (iRand)
		{
		case 0:
			SoundMgr(L"material_rock_step1.wav", CSoundMgr::PLAYER_EFFECT);
			break;

		case 1:
			SoundMgr(L"material_rock_step2.wav", CSoundMgr::PLAYER_EFFECT);
			break;

		case 2:
			SoundMgr(L"material_rock_step3.wav", CSoundMgr::PLAYER_EFFECT);
			break;

		case 3:
			SoundMgr(L"material_rock_step4.wav", CSoundMgr::PLAYER_EFFECT);
			break;
		}
	}
}

void CPlayer::Weapon_Change()
{
	if (m_eCurWeaponMode != m_ePreWeaponMode)
	{
		map<const wstring, _bool>::iterator		iter = m_mapActiveParts.begin();

		switch (m_eCurWeaponMode)
		{
		case Engine::WEAPON_DUALSWORD:
			for (; iter != m_mapActiveParts.end(); ++iter)
			{
				if (L"dualsword_vanquisher.tga" == iter->first)
				{
					iter->second = TRUE;
				}
				else
				{
					iter->second = FALSE;
				}
			}
			break;

		case Engine::WEAPON_SECONDARY:
			for (; iter != m_mapActiveParts.end(); ++iter)
			{
				if (L"sticky_bomb.tga" == iter->first)
				{
					iter->second = TRUE;
				}
				else
				{
					iter->second = FALSE;
				}
			}
			break;

		case Engine::WEAPON_END:
			for (; iter != m_mapActiveParts.end(); ++iter)
			{
				iter->second = FALSE;
			}
			break;
		}

		m_ePreWeaponMode = m_eCurWeaponMode;
	}
}

void CPlayer::Pooling_DamageFont(_uint iDamage, const _matrix* pMatrix, DAMAGEFONT_ID eID)
{
	_uint	iDigitThousands = _uint((iDamage % 10000) * 0.001f);
	_uint	iDigitHundreds = _uint((iDamage % 1000) * 0.01f);
	_uint	iDigitTen = _uint((iDamage % 100) * 0.1f);
	_uint	iDigitOne = _uint(iDamage % 10);

	CDamageFont*	pFont0 = nullptr;
	CDamageFont*	pFont1 = nullptr;
	CDamageFont*	pFont2 = nullptr;
	CDamageFont*	pFont3 = nullptr;

	CTransform*		pTransform = nullptr;

	_vec3	vCamRight = m_pMainCam->Get_CamDirVector(DIR_RIGHT);

	_vec3	vPos;
	vPos.x = pMatrix->_41 + Engine::Random(-0.5f, 0.5f);
	if (0.5f > pMatrix->_42)
	{
		vPos.y = 0.5f;
	}
	else
	{
		vPos.y = pMatrix->_42;
	}
	vPos.z = pMatrix->_43 + Engine::Random(-0.5f, 0.5f);

	wstring		wstrNormalFont = L"DamageFont_Normal_UI_";
	wstring		wstrSkillFont = L"DamageFont_Skill_UI_";
	wstring		wstrMonsterFont = L"DamageFont_Monster_UI_";

	switch (eID)
	{
	case Engine::DAMAGEFONT_NORMAL:
		for (_uint i = 0; i < DAMAGEFONT_COUNT; ++i)
		{
			wstrNormalFont = L"DamageFont_Normal_UI_";
			wstrNormalFont += to_wstring(i);

			pTransform = static_cast<CTransform*>(Engine::Get_Component(L"UI", wstrNormalFont, L"Com_Transform", ID_DYNAMIC));

			if (POOLING_POS == *pTransform->Get_Info(INFO_POS))
			{
				if (!pFont0)
				{
					pFont0 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont1)
				{
					pFont1 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont2)
				{
					pFont2 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont3)
				{
					pFont3 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (pFont0 && pFont1 && pFont2 && pFont3)
				{
					break;
				}

				continue;
			}
		}
		break;

	case Engine::DAMAGEFONT_SKILL:
		for (_uint i = 0; i < DAMAGEFONT_SKILL_COUNT; ++i)
		{
			wstrSkillFont = L"DamageFont_Skill_UI_";
			wstrSkillFont += to_wstring(i);

			pTransform = static_cast<CTransform*>(Engine::Get_Component(L"UI", wstrSkillFont, L"Com_Transform", ID_DYNAMIC));

			if (POOLING_POS == *pTransform->Get_Info(INFO_POS))
			{
				if (!pFont0)
				{
					pFont0 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont1)
				{
					pFont1 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont2)
				{
					pFont2 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont3)
				{
					pFont3 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (pFont0 && pFont1 && pFont2 && pFont3)
				{
					break;
				}

				continue;
			}
		}
		break;

	case Engine::DAMAGEFONT_MONSTER:
		for (_uint i = 0; i < DAMAGEFONT_MONSTER_COUNT; ++i)
		{
			wstrMonsterFont = L"DamageFont_Monster_UI_";
			wstrMonsterFont += to_wstring(i);

			pTransform = static_cast<CTransform*>(Engine::Get_Component(L"UI", wstrMonsterFont, L"Com_Transform", ID_DYNAMIC));

			if (POOLING_POS == *pTransform->Get_Info(INFO_POS))
			{
				if (!pFont0)
				{
					pFont0 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrMonsterFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont1)
				{
					pFont1 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrMonsterFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont2)
				{
					pFont2 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrMonsterFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont3)
				{
					pFont3 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrMonsterFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (pFont0 && pFont1 && pFont2 && pFont3)
				{
					break;
				}

				continue;
			}
		}
		break;
	}

	if (1000 <= iDamage)
	{
		pFont0->Set_EnableDamageFont(vPos, iDigitThousands, 1000);
		pFont1->Set_EnableDamageFont(vPos, iDigitHundreds, 100);
		pFont2->Set_EnableDamageFont(vPos, iDigitTen, 10);
		pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
	}
	else if (100 <= iDamage)
	{
		pFont1->Set_EnableDamageFont(vPos, iDigitHundreds, 100);
		pFont2->Set_EnableDamageFont(vPos, iDigitTen, 10);
		pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
	}
	else if (10 <= iDamage)
	{
		pFont2->Set_EnableDamageFont(vPos, iDigitTen, 10);
		pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
	}
	else if (1 <= iDamage)
	{
		pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
	}
}

void CPlayer::Make_TrailEffect(const _float& fDeltaTime)
{
	map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

	for (; iter != m_mapBoxColliderCom.end(); )
	{
		if (L"Hit_LHandLong" == iter->first)
		{
			m_pTrailSword->Set_InfoForTrail(fDeltaTime, iter->second->Get_Min(), iter->second->Get_Max(), iter->second->Get_ColliderWorld());
		}
		else if (L"Hit_RHandLong" == iter->first)
		{
			m_pTrailSword->Set_InfoForTrail(fDeltaTime, iter->second->Get_Min(), iter->second->Get_Max(), iter->second->Get_ColliderWorld());
		}

		++iter;
	}
}

void CPlayer::Animation_Control()
{
	m_fAniTime = m_pMeshCom->Get_AniFrameTime();

	// Speed 조절
	if (PL_ATK == m_eCurAction)
	{
		m_pMeshCom->Set_TrackSpeed(3.2f);
	}
	else if (PL_SMASH == m_eCurAction || PL_SKILL == m_eCurAction || PL_DASH == m_eCurAction)
	{
		if (STATE_FURY == m_iAniIndex ||
			STATE_FURY2 == m_iAniIndex)
		{
			m_pMeshCom->Set_TrackSpeed(2.15f);
		}
		else if (STATE_SP_FEVER == m_iAniIndex)
		{
			m_pMeshCom->Set_TrackSpeed(2.65f);
		}
		else
		{
			m_pMeshCom->Set_TrackSpeed(2.4f);
		}
	}
	else if (STATE_DAMAGE_RESIST == m_eCurState)
	{
		m_pMeshCom->Set_TrackSpeed(1.8f);
	}
	else if (STATE_RUN == m_eCurState ||
			 STATE_SPRINT == m_eCurState)
	{
		m_pMeshCom->Set_TrackSpeed(2.f);
	}
	else
	{
		m_pMeshCom->Set_TrackSpeed(1.9f);
	}


	// 스테미나 오링 
	if (PL_TIRED >= m_eCurAction)
	{
		if (m_iAniIndex != STATE_TIRED_DURING)
		{
			if (0.f >= m_tPlayerInfo.fStamina)
			{
				m_iAniIndex = STATE_TIRED_BEGIN;
			}
		}
	}

	// 각 Animation 별 디테일
	m_eCurState = (PL_STATE)m_iAniIndex;

	if (m_eCurState != m_ePreState)
	{
		m_bAnimation = true;

		m_lfAniEnd = m_pMeshCom->Get_AniFrameEndTime();

		// Sound 변수 초기화
		for (_uint i = 0; i < 3; ++i)
		{
			m_bAtkSound[i] = false;
			m_bLethitaSound[i] = false;
			m_bSkillSound[i] = false;
		}
		/////////////////////

		_uint	iRandSound = 0;
		switch (m_eCurState)
		{
		case STATE_TIRED_BEGIN:
			m_lfAniEnd *= 0.35f;
			break;

		case STATE_TIRED_DURING:
			break;

		case STATE_TIRED_END:
			m_lfAniEnd *= 0.8f;
			break;

		case STATE_THROW_BEGIN:
			break;

		case STATE_THROW_DURING:
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;

			m_eCurWeaponMode = WEAPON_SECONDARY;

			Rotate_PlayerLook(-*m_pTransformCom->Get_Info(INFO_LOOK));
			break;

		case STATE_THROW_END:
			// StickyBomb
			NULL_CHECK(m_pOtherLayer);

			{
				CGameObject*	pGameObject = nullptr;

				pGameObject = CStickyBomb::Create(m_pGraphicDev);
				NULL_CHECK(pGameObject);

				//wstring	wstrName = L"StickyBomb" + to_wstring(m_iSecondaryCount);
				FAILED_CHECK_RETURN(m_pOtherLayer->Add_GameObject(L"StickyBomb", pGameObject), );

				Engine::Emplace_Layer(L"StickyBomb", m_pOtherLayer);

				//++m_iSecondaryCount;
			}

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_SP_FEVER:
			SKILL_MOVE_BYANI(0.6f, 2500.f, 0.73f);
			m_pMainCam->Set_HighlightSkillShot(1.85f, 750);

			m_lfAniEnd *= 2.f;
			break;

			//case STATE_SPRINT_STOP:
			//	break;
		case STATE_REVIVE:
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_DEAD:
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_DAMAGE_RESIST:
			SKILL_MOVE_BYANI(0.1f, -400.f, 0.78f);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;

			SoundPlayerHurt;
			SoundMgr(L"Skill_DownResist.wav", CSoundMgr::PLAYER_EFFECT);
			break;

		case STATE_DOWNTOIDLE_FRONT:
			break;

		case STATE_DOWNTOIDLE_BACK:
			Rotate_PlayerLook(m_vDir);

			SKILL_MOVE_BYANI(0.2f, 350.f, 0.85f);

			m_lfAniEnd *= 0.7f;
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_DOWNIDLE_FRONT:
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_DOWNIDLE_BACK:
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_DAMAGEFROM_FRONT:
			SKILL_MOVE_BYANI(0.f, -350.f, 0.7f);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;

			//m_lfAniEnd *= 0.85f
			m_lfAniEnd = 1.23f;
			SoundPlayerHurt;
			break;

		case STATE_DAMAGEFROM_BACK:
			SKILL_MOVE_BYANI(0.f, 350.f, 0.7f);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;

			//m_lfAniEnd *= 0.85f;
			m_lfAniEnd = 1.23f;
			SoundPlayerHurt;
			break;

		case STATE_WINDMILL:
			break;

		case STATE_DOUBLE_CRECSENT:
			SKILL_MOVE_BYANI(0.135f, 300.f, 0.5f);

			m_pMainCam->Set_HighlightSkillShot(1.95f, 750);
			Set_PlayerStemina(-20.f);

			m_eNextAtk = STATE_ATK1;
			break;

		case STATE_DASHATK:
			SKILL_MOVE_BYANI(0.01f, 1100.f, 0.15f);

			m_pMainCam->Set_HighlightSkillShot(1.85f, 750);
			Set_PlayerStemina(-10.f);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_FURY2:
			SKILL_MOVE_BYANI(0.45f, 4000.f, 0.6f);
			m_lfAniEnd = 1.9f;

			Set_PlayerStemina(-30.f);
			break;

		case STATE_FURY:
			SKILL_MOVE_BYANI(0.45f, 3250.f, 0.6f);
			m_lfAniEnd = 1.7f;

			Set_PlayerStemina(-30.f);
			break;

			//case STATE_DASH_W:
			//	break;

		case STATE_DASH_S:
			SKILL_MOVE_BYANI(0.125f, 700.f, 0.3f);

			Set_PlayerStemina(-9.f);

			m_eNextAtk = STATE_ATK1;
			SetNextSmash(STATE_DOUBLE_CRECSENT, 750);
			break;

		case STATE_DASH_N:
			SKILL_MOVE_BYANI(0.125f, 900.f, 0.3f);

			Set_PlayerStemina(-15.f);

			m_eNextAtk = STATE_ATK1;
			SetNextSmash(STATE_DOUBLE_CRECSENT, 750);
			break;

			//case STATE_DASH_E:
			//	break;

		case STATE_SMASH3:
			SKILL_MOVE_BYANI(0.18f, 700.f, 0.53f);

			Set_PlayerStemina(-23.f);

			m_eNextAtk = STATE_ATK1;
			SetNextSmash(STATE_SMASH1, 500);
			break;

		case STATE_SMASH4_B:
			Set_PlayerStemina(-15.f);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_SMASH4:
			SKILL_MOVE_BYANI(0.45f, 650.f, 0.53f);

			Set_PlayerStemina(-25.f);

			m_eNextAtk = STATE_ATK1;
			//m_eNextSmash = STATE_DASHATK;		// 4타 스매쉬 추가타 구현 전까지는 잠깐 대쉬어택으로 대체함. 
			//m_eNextSmash = STATE_SMASH4_B;
			break;

		case STATE_SMASH2_B:
			SKILL_MOVE_BYANI(0.3f, 1100.f, 0.55f);

			Set_PlayerStemina(-20.f);

			m_eNextAtk = STATE_ATK1;
			break;

		case STATE_SMASH2:
			SKILL_MOVE_BYANI(0.25f, 600.f, 0.38f);

			Set_PlayerStemina(-15.f);

			m_eNextAtk = STATE_ATK1;
			SetNextSmash(STATE_SMASH2_B, 750);
			break;

		case STATE_SMASH1:
			SKILL_MOVE_BYANI(0.15f, 600.f, 0.28f);

			Set_PlayerStemina(-15.f);

			m_eNextAtk = STATE_ATK1;
			break;

		case STATE_ATK4:
			SKILL_MOVE_BYANI(0.135f, 650.f, 0.26f);

			Set_PlayerStemina(-0.f);

			m_eNextAtk = STATE_ATK1;
			SetNextSmash(STATE_SMASH4, 750);
			break;

		case STATE_ATK3:
			SKILL_MOVE_BYANI(0.15f, 650.f, 0.28f);

			Set_PlayerStemina(-0.f);

			m_eNextAtk = STATE_ATK4;
			SetNextSmash(STATE_SMASH3, 750);
			break;

		case STATE_ATK2:
			SKILL_MOVE_BYANI(0.15f, 650.f, 0.28f);

			Set_PlayerStemina(-0.f);

			m_eNextAtk = STATE_ATK3;
			SetNextSmash(STATE_SMASH2, 750);
			break;

		case STATE_ATK1:
			SKILL_MOVE_BYANI(0.15f, 650.f, 0.26f);

			Set_PlayerStemina(-0.f);

			m_eNextAtk = STATE_ATK2;
			SetNextSmash(STATE_SMASH1, 750);
			break;

		default:
			SKILL_MOVE_END;

			m_eNextAtk = STATE_ATK1;
			//m_eNextSmash = STATE_DASHATK;
			break;
		}

		m_fAniTime = 0.f;
		m_bCanHit = true;
		m_ePreState = m_eCurState;
	}

	// 매 프레임 실행
	switch (m_eCurState)
	{
	case STATE_SPRINT:
		Set_PlayerStemina(-0.05f);

		m_eNextSmash = STATE_DASHATK;
		break;

	case STATE_THROW_END:
		if (m_dwThrowStart + m_dwThrowDelay <= GetTickCount())
		{
			m_eCurWeaponMode = WEAPON_END;
		}
		break;
	}

	// State 자동 변경
	if (m_fAniTime >= m_lfAniEnd)
	{
		if (Engine::STATE_IDLE == m_eCurState ||
			Engine::STATE_RUN == m_eCurState ||
			Engine::STATE_SPRINT == m_eCurState ||
			Engine::STATE_THROW_DURING == m_eCurState || 
			Engine::STATE_TIRED_DURING == m_eCurState)
		{
			m_bAnimation = true;
		}
		else
		{
			m_bAnimation = false;
		}

		switch (m_eCurState)
		{
		case Engine::STATE_DEAD:
			break;

		case Engine::STATE_DOWNIDLE_FRONT:		// 누운 상태에서 지 혼자 못 일어나게 막음
			break;

		case Engine::STATE_DOWNIDLE_BACK:
			break;

		case Engine::STATE_DAMAGEFROM_FRONT:
			//m_iAniIndex = (_uint)STATE_DOWNIDLE_BACK;
			break;

		case Engine::STATE_DAMAGEFROM_BACK:
			//m_iAniIndex = (_uint)STATE_DOWNIDLE_FRONT;
			break;

		case Engine::STATE_THROW_BEGIN:
			//m_iAniIndex = (_uint)STATE_THROW_DURING;
			break;

		case Engine::STATE_THROW_DURING:
			break;

		case Engine::STATE_TIRED_BEGIN:
			m_iAniIndex = (_uint)STATE_TIRED_DURING;
			break;

		case Engine::STATE_TIRED_DURING:
			break;

		default:
			if (STATE_THROW_END == m_ePreState)
			{
				m_eCurWeaponMode = WEAPON_DUALSWORD;

				m_pMainCam->Set_CameraMode(CDynamicCamera::MODE_NORMAL);
				Rotate_PlayerLook(+*m_pTransformCom->Get_Info(INFO_LOOK));
			}

			m_iDashCount = 0;

			m_eCurAction = PL_IDLE;

			m_eNextAtk = STATE_ATK1;
			if (m_dwNextSmashCheckTime + m_dwNextSmashDelay < GetTickCount())
			{
				m_eNextSmash = STATE_DASHATK;
			}
			break;
		}
	}
}

void CPlayer::Collision_Control()
{
	if (!m_mapColliderCom.empty())
	{
		map<const wstring, CCollider*>::iterator	iter = m_mapColliderCom.begin();

		for (; iter != m_mapColliderCom.end(); ++iter)
		{
			if (STATE_DASH_N == m_iAniIndex ||
				STATE_DASH_S == m_iAniIndex ||
				STATE_DAMAGE_RESIST == m_iAniIndex ||
				STATE_DAMAGEFROM_BACK == m_iAniIndex ||
				STATE_DAMAGEFROM_FRONT == m_iAniIndex ||
				STATE_DOWNIDLE_BACK == m_iAniIndex ||
				STATE_DOWNIDLE_FRONT == m_iAniIndex ||
				STATE_DOWNTOIDLE_BACK == m_iAniIndex ||
				STATE_DOWNTOIDLE_FRONT == m_iAniIndex ||
				STATE_REVIVE == m_iAniIndex ||
				STATE_DEAD == m_iAniIndex)
			{
				iter->second->Set_CanCollision(false);
			}
			else
			{
				iter->second->Set_CanCollision(true);
			}
		}
	}

	if (!m_mapBoxColliderCom.empty())
	{
		map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		//for (; iter != m_mapBoxColliderCom.end(); ++iter)
		//{
		//	if (iter->second->Get_CanCollision())
		//	{
		//		iter->second->Set_RenderColType(COL_TRUE);
		//	}
		//	else
		//	{
		//		iter->second->Set_RenderColType(COL_FALSE);
		//	}
		//}

		_uint iRandSound = 0;
		switch (m_iAniIndex)
		{
		case STATE_SP_FEVER:
			HITBOX_CONTROLL(0.6f, 0.75f, TRUE);
			if (!m_bAtkSound[0] &&
				m_fAniTime >= 0.6f)
			{
				m_bAtkSound[0] = true;
				SoundMgr(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT3);
			}
			break;

		case STATE_DOUBLE_CRECSENT:
			HITBOX_CONTROLL(0.35f, 0.6f, TRUE);
			if (!m_bAtkSound[2] &&
				m_fAniTime >= 0.3f)
			{
				m_bAtkSound[2] = true;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_DASHATK:
			HITBOX_CONTROLL(0.1f, 0.65f, FALSE);
			if (!m_bAtkSound[0] &&
				m_fAniTime >= 0.1f)
			{
				m_bAtkSound[0] = true;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_FURY2:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (L"Hit_LHandLongLong" == iter->first || 
					L"Hit_RHandLongLong" == iter->first)
				{
					if (0.37f <= m_fAniTime &&
						0.6f >= m_fAniTime)
					{
						iter->second->Set_CanCollision(true);
					}
					else
					{
						iter->second->Set_CanCollision(false);
					}
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			//HITBOX_CONTROLL(0.4f, 0.6f, TRUE);
			if (!m_bSkillSound[0] &&
				m_fAniTime >= 0.37f)
			{
				m_bSkillSound[0] = true;
				SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT3, 0.23f);

				m_pMainCam->Set_HighlightSkillShot(1.85f, 750);
			}
			break;

		case STATE_FURY:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (L"Hit_LHandLongLong" == iter->first ||
					L"Hit_RHandLongLong" == iter->first)
				{
					if (0.37f <= m_fAniTime &&
						0.6f >= m_fAniTime)
					{
						iter->second->Set_CanCollision(true);
					}
					else
					{
						iter->second->Set_CanCollision(false);
					}
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			//HITBOX_CONTROLL(0.4f, 0.6f, TRUE);
			if (!m_bSkillSound[0] &&
				m_fAniTime >= 0.37f)
			{
				m_bSkillSound[0] = true;
				SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT2, 0.25f);

				m_pMainCam->Set_HighlightSkillShot(1.85f, 750);
			}
			break;

		case STATE_SMASH3:
			HITBOX_CONTROLL(0.35f, 0.5f, TRUE);
			if (!m_bLethitaSound[1] &&
				m_fAniTime >= 0.25f)
			{
				m_bLethitaSound[1] = true;
				SoundPlayerStrongAtk;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);

				m_pMainCam->Set_HighlightSkillShot(1.85f, 750);
			}
			else if (!m_bAtkSound[1] &&
				m_fAniTime >= 0.45f)
			{
				m_bAtkSound[1] = true;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_SMASH4:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (L"Hit_LHandLongLong" == iter->first ||
					L"Hit_RHandLongLong" == iter->first)
				{
					if (0.55f <= m_fAniTime &&
						0.8f >= m_fAniTime)
					{
						iter->second->Set_CanCollision(true);
					}
					else
					{
						iter->second->Set_CanCollision(false);
					}
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			//HITBOX_CONTROLL(0.55f, 0.8f, TRUE);
			if (!m_bLethitaSound[0] &&
				m_fAniTime >= 0.55f)
			{
				m_bLethitaSound[0] = true;
				SoundPlayerStrongAtk;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);

				m_pMainCam->Set_HighlightSkillShot(1.85f, 750);
			}
			break;

		case STATE_SMASH2:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (L"Other_Attack" == iter->first)
				{
					if (0.5f <= m_fAniTime &&
						0.7f >= m_fAniTime)
					{
						iter->second->Set_CanCollision(true);
					}
					else
					{
						iter->second->Set_CanCollision(false);
					}
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			if (!m_bAtkSound[0] &&
				m_fAniTime >= 0.5f)
			{
				m_bAtkSound[0] = true;
				SoundPlayerStrongAtk;
				SoundMgr(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT);

				m_pMainCam->Set_HighlightSkillShot(1.85f, 750);
			}
			break;

		case STATE_SMASH2_B:
			HITBOX_CONTROLL(0.45f, 0.7f, TRUE);
			if (!m_bAtkSound[1] &&
				m_fAniTime >= 0.45f)
			{
				m_bAtkSound[1] = true;
				SoundPlayerStrongAtk;
				SoundMgr(L"Swing_Metal.wav", CSoundMgr::PLAYER_EFFECT);

				m_pMainCam->Set_HighlightSkillShot(1.85f, 750);
			}
			break;

		case STATE_SMASH1:
			HITBOX_CONTROLL(0.4f, 0.6f, TRUE);
			if (!m_bAtkSound[1] &&
				m_fAniTime >= 0.3f)
			{
				m_bAtkSound[1] = true;
				SoundPlayerStrongAtk;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);

				m_pMainCam->Set_HighlightSkillShot(1.85f, 750);
			}
			break;

		case STATE_ATK4:
			HITBOX_CONTROLL(0.3f, 0.7f, FALSE);
			if (!m_bLethitaSound[2] &&
				m_fAniTime >= 0.3f)
			{
				m_bLethitaSound[2] = true;
				SoundPlayerAtk;
				SoundMgr(L"Swing_Metal.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_ATK3:
			HITBOX_CONTROLL(0.3f, 0.35f, FALSE);
			if (!m_bLethitaSound[1] &&
				m_fAniTime >= 0.3f)
			{
				m_bLethitaSound[1] = true;
				SoundPlayerAtk;
				SoundMgr(L"Swing_Metal.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_ATK2:
			HITBOX_CONTROLL(0.25f, 0.4f, FALSE);
			if (!m_bLethitaSound[0] &&
				m_fAniTime >= 0.25f)
			{
				m_bLethitaSound[0] = true;
				SoundPlayerAtk;
				SoundMgr(L"Swing_Metal.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_ATK1:
			HITBOX_CONTROLL(0.25f, 0.4f, FALSE);
			if (!m_bLethitaSound[1] &&
				m_fAniTime >= 0.25f)
			{
				m_bLethitaSound[1] = true;
				SoundPlayerAtk;
				SoundMgr(L"Swing_Metal.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		default:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				iter->second->Set_CanCollision(false);
			}
			break;
		}
	}

	// 실제 충돌 처리 => 는 콜리젼 매니저로 기능 분리함.
}

void CPlayer::Update_UI()
{
	// Hpbar
	if (m_pHpbarValueUI)
	{
		m_pHpbarValueUI->Set_ValueRatio((_float)m_tPlayerInfo.tagInfo.iHp);
		m_pHpbarValueUI->Set_MaxValueRatio((_float)m_tPlayerInfo.tagInfo.iMaxHp);
	}
	if (m_pHpbarLerpUI)
	{
		m_pHpbarLerpUI->Set_ValueRatio((_float)m_tPlayerInfo.tagInfo.iHp);
		m_pHpbarLerpUI->Set_MaxValueRatio((_float)m_tPlayerInfo.tagInfo.iMaxHp);
	}
	// Steminabar
	if (m_pSteminabarValueUI)
	{
		m_pSteminabarValueUI->Set_ValueRatio(m_tPlayerInfo.fStamina);
		m_pSteminabarValueUI->Set_MaxValueRatio(m_tPlayerInfo.fMaxStamina);
	}
	// Spbar
	if (m_pSpbarValueUI)
	{
		m_pSpbarValueUI->Set_ValueRatio((_float)m_tPlayerInfo.iSkillPoint);
		m_pSpbarValueUI->Set_MaxValueRatio((_float)m_tPlayerInfo.iMaxSkillPoint);
	}
}

void CPlayer::Update_State()
{
	// 체력 관련
	if (0 >= m_tPlayerInfo.tagInfo.iHp)
	{
		m_tPlayerInfo.tagInfo.iHp = 0;

		m_eCurAction = PL_DEAD;
	}
	else if (m_tPlayerInfo.tagInfo.iMaxHp < m_tPlayerInfo.tagInfo.iHp)
	{
		m_tPlayerInfo.tagInfo.iHp = m_tPlayerInfo.tagInfo.iMaxHp;
	}

	// 스테미나 관련
	if (STATE_TIRED_DURING == m_eCurState)
	{
		if (30.f <= m_tPlayerInfo.fStamina)
		{
			m_iAniIndex = STATE_TIRED_END;
		}
	}

	if (0 > m_tPlayerInfo.fStamina)
	{
		m_tPlayerInfo.fStamina = 0;

		m_eCurAction = PL_TIRED;
		Animation_Control();
	}
	else if (m_tPlayerInfo.fMaxStamina < m_tPlayerInfo.fStamina)
	{
		m_tPlayerInfo.fStamina = m_tPlayerInfo.fMaxStamina;
	}

	if (m_tPlayerInfo.fStamina < m_tPlayerInfo.fMaxStamina &&
		m_dwSteminaRecoveryTime + m_dwSteminaRecoveryDelay < GetTickCount())
	{
		m_tPlayerInfo.fStamina += 0.3f;
	}

	// sp 관련
	if (0 > m_tPlayerInfo.iSkillPoint)
	{
		m_tPlayerInfo.iSkillPoint = 0;
	}
	else if (m_tPlayerInfo.iMaxSkillPoint < m_tPlayerInfo.iSkillPoint)
	{
		m_tPlayerInfo.iSkillPoint = m_tPlayerInfo.iMaxSkillPoint;
	}
}
