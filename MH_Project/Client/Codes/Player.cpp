#include "stdafx.h"
#include "Player.h"

#include "Export_Function.h"
#include "DynamicCamera.h"
#include "Ahglan.h"
#include "StickyBomb.h"
#include "Balista.h"
#include "Trail_Sword.h"
#include "Trail_Smash.h"
#include "CriticalEfx.h"
#include "RadialBlur.h"

#include "Player_Hpbar_BackUI.h"
#include "Player_Hpbar_ValueUI.h"
#include "Player_Hpbar_LerpUI.h"
#include "Player_Steminabar_BackUI.h"
#include "Player_Steminabar_ValueUI.h"
#include "Player_Spbar_BackUI.h"
#include "Player_Spbar_ValueUI.h"
#include "Player_Buff_CriticalUI.h"
#include "Player_SlotUI.h"
#include "Potion_Hp.h"
#include "Potion_Stemina.h"
#include "Potion_Sp.h"
#include "Item_StickyBomb.h"
#include "Skill_Balista.h"

#include "Skill_Sp_Stand.h"
#include "Skill_Sp_Fever.h"
#include "Skill_Fury_No7.h"

#include "Buff_Stemina.h"
#include "Buff_Reinforce.h"
#include "Tooltip_Stemina.h"
#include "Tooltip_Reinforce.h"

#include "DamageFont.h"

#include "Announce_Balista.h"

#include "DataMgr.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
	ZeroMemory(&m_tPlayerInfo, sizeof(PL_INFO));
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

CPlayer::~CPlayer()
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

	Ready_PlayerStructInfo();

	return S_OK;
}

HRESULT CPlayer::LateReady_Object()
{
	Ready_Layer_PlayerUI();

	m_eSceneID = Engine::Get_SceneID();

	if (SCENE_STAGE == m_eSceneID)
	{
		m_tPlayerInfo = CDataMgr::GetInstance()->Get_PlayerInfo();
	}

	m_mapActiveParts.emplace(L"dualsword_vanquisher.tga", TRUE);
	m_mapActiveParts.emplace(L"sticky_bomb.tga", FALSE);

	m_pAhglan = dynamic_cast<CAhglan*>(Engine::Get_GameObject(L"GameLogic", L"Ahglan"));

	m_pTrailSwordL = dynamic_cast<CTrail_Sword*>(Engine::Get_GameObject(L"GameLogic", L"Player_Sword_Trail"));
	m_pTrailSwordR = dynamic_cast<CTrail_Sword*>(Engine::Get_GameObject(L"GameLogic", L"Player_Sword_Trail2"));
	m_pTrailSmashL = dynamic_cast<CTrail_Smash*>(Engine::Get_GameObject(L"GameLogic", L"Player_Smash_Trail"));
	m_pTrailSmashR = dynamic_cast<CTrail_Smash*>(Engine::Get_GameObject(L"GameLogic", L"Player_Smash_Trail2"));

	m_pRadialBlur = dynamic_cast<CRadialBlur*>(Engine::Get_GameObject(L"Effect", L"Efx_RadiulBlur"));

	m_pHpbarValueUI = dynamic_cast<CPlayer_Hpbar_ValueUI*>(Engine::Get_GameObject(L"Player_UI", L"Player_Hpbar_ValueUI"));
	m_pHpbarLerpUI = dynamic_cast<CPlayer_Hpbar_LerpUI*>(Engine::Get_GameObject(L"Player_UI", L"Player_Hpbar_LerpUI"));
	m_pSteminabarValueUI = dynamic_cast<CPlayer_Steminabar_ValueUI*>(Engine::Get_GameObject(L"Player_UI", L"Player_Steminabar_ValueUI"));
	m_pSpbarValueUI = dynamic_cast<CPlayer_Spbar_ValueUI*>(Engine::Get_GameObject(L"Player_UI", L"Player_Spbar_ValueUI"));

	m_pNaviMeshCom->Set_CellIndex(Compute_InCell());

	return S_OK;
}

_int CPlayer::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	if (m_bDead)
		return iExit;

	if (Key_Down('Y'))
	{
		m_bColRender = !m_bColRender;
		m_bNaviMeshRender = !m_bNaviMeshRender;
	}

	//_vec3 vPos;
	//vPos = *m_pTransformCom->Get_Info(INFO_POS);
	//m_fSpeed = 9.f;

	m_pMainCam = static_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));

	m_pNaviMeshCom->Set_CellIndex(Compute_InCell());
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
	SupportFire_Balista();

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
			//if (iter->second->Get_CanCollision())
			//{
			iter->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());
			//}
		}
	}
	if (!m_mapBoxColliderCom.empty())
	{
		map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		for (; iter != m_mapBoxColliderCom.end(); ++iter)
		{
			//if (iter->second->Get_CanCollision())
			//{
			if (L"Other_Attack" == iter->first)
			{
				CTransform*	pHitBoxPosTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"HitBox_Pos", L"Com_Transform", ID_DYNAMIC));

				iter->second->LateUpdate_Collider(pHitBoxPosTrans->Get_WorldMatrix());
			}
			else
			{
				iter->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());
			}
			//}
		}
	}

	return iExit;
}

void CPlayer::Render_Object()
{
	if (m_bColRender)
	{
		if (!m_mapColliderCom.empty())
		{
			map<const wstring, CCollider*>::iterator	iter = m_mapColliderCom.begin();

			for (; iter != m_mapColliderCom.end(); ++iter)
			{
				if (iter->second->Get_CanCollision())
				{
					iter->second->Render_Collider(COL_TRUE, m_pTransformCom->Get_WorldMatrix());
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
					iter->second->Render_Collider(COL_TRUE, m_pTransformCom->Get_WorldMatrix());
				}
			}
		}
	}

	if (m_bNaviMeshRender)
	{
		if (m_pNaviMeshCom)
			m_pNaviMeshCom->Render_NaviMesh();
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
				m_iAniIndex = STATE_DAMAGEFROM_BACK;
			}
			else
			{
				m_iAniIndex = STATE_DAMAGEFROM_FRONT;
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

HRESULT CPlayer::Add_Component()
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

	switch (m_eSceneID)
	{
	case Engine::SCENE_STAGE:
		pEffect->SetInt("g_iStageNum", 0);
		break;
	case Engine::SCENE_STAGE_1:
		pEffect->SetInt("g_iStageNum", 1);
		break;
	}

	const D3DLIGHT9*	pLightInfo = Get_Light();
	NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));

	return S_OK;
}

HRESULT CPlayer::Ready_Layer_PlayerUI()
{
	m_pOtherLayer = CLayer::Create();
	NULL_CHECK_RETURN(m_pOtherLayer, E_FAIL);
	m_pUILayer = CLayer::Create();
	NULL_CHECK_RETURN(m_pUILayer, E_FAIL);

	CGameObject*			pGameObject = nullptr;

	// Hpbar
	pGameObject = CPlayer_Hpbar_BackUI::Create(m_pGraphicDev, 340.f, WINCY - 185.f, 650.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Hpbar_BackUI", pGameObject), E_FAIL);

	pGameObject = CPlayer_Hpbar_LerpUI::Create(m_pGraphicDev, 340.f, WINCY - 185.f, 650.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Hpbar_LerpUI", pGameObject), E_FAIL);

	pGameObject = CPlayer_Hpbar_ValueUI::Create(m_pGraphicDev, 340.f, WINCY - 185.f, 650.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Hpbar_ValueUI", pGameObject), E_FAIL);

	// Steminabar
	pGameObject = CPlayer_Steminabar_BackUI::Create(m_pGraphicDev, 340.f, WINCY - 165.f, 650.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Steminabar_BackUI", pGameObject), E_FAIL);

	pGameObject = CPlayer_Steminabar_ValueUI::Create(m_pGraphicDev, 340.f, WINCY - 165.f, 650.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Steminabar_ValueUI", pGameObject), E_FAIL);

	// Spbar
	pGameObject = CPlayer_Spbar_BackUI::Create(m_pGraphicDev, 210.f, WINCY - 150.f, 350.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Spbar_BackUI", pGameObject), E_FAIL);

	pGameObject = CPlayer_Spbar_ValueUI::Create(m_pGraphicDev, 210.f, WINCY - 150.f, 350.f, 25.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_Spbar_ValueUI", pGameObject), E_FAIL);

	// QuickSlot
	pGameObject = CPlayer_SlotUI::Create(m_pGraphicDev, SCREEN_CENTER_X, 50.f, 620.f, 85.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Player_SlotUI", pGameObject), E_FAIL);

	// QuickSlot Item 
	// 퀵슬롯 아이템들은 퀵슬롯 UI를 따라다닐 것이기 때문에 아무 위치에나 배치해도 상관없음.  
	pGameObject = CPotion_Hp::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_ITEM, SCALE_ITEM);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Potion_Hp", pGameObject), E_FAIL);

	pGameObject = CPotion_Stemina::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_ITEM, SCALE_ITEM);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Potion_Stemina", pGameObject), E_FAIL);

	pGameObject = CPotion_Sp::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_ITEM, SCALE_ITEM);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Potion_Sp", pGameObject), E_FAIL);

	pGameObject = CSkill_Sp_Stand::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_ITEM, SCALE_ITEM);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Skill_Sp_Stand", pGameObject), E_FAIL);

	if (SCENE_STAGE == Engine::Get_SceneID())
	{
		pGameObject = CItem_StickyBomb::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_ITEM, SCALE_ITEM);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Skill_StickyBomb", pGameObject), E_FAIL);

		pGameObject = CSkill_Balista::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_ITEM, SCALE_ITEM);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Skill_Balista", pGameObject), E_FAIL);
	}

	// QuickSlot Skill Icon 
	pGameObject = CSkill_Sp_Fever::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_ITEM, SCALE_ITEM);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Skill_Sp_Fever", pGameObject), E_FAIL);

	pGameObject = CSkill_Fury_No7::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_ITEM, SCALE_ITEM);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Skill_Fury_No7", pGameObject), E_FAIL);

	// QuickSlot Buff Icon 
	pGameObject = CBuff_Stemina::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_ICON, SCALE_ICON);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Buff_Stemina_Icon", pGameObject), E_FAIL);

	pGameObject = CBuff_Reinforce::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_ICON, SCALE_ICON);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Buff_Reinforce_Icon", pGameObject), E_FAIL);

	// Buff_Tooltip
	pGameObject = CTooltip_Stemina::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_TOOLTIP, SCALE_TOOLTIP - 150.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Tooltip_Stemina", pGameObject), E_FAIL);

	pGameObject = CTooltip_Reinforce::Create(m_pGraphicDev, SCREEN_CENTER_X, 70.f, SCALE_TOOLTIP, SCALE_TOOLTIP - 150.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pUILayer->Add_GameObject(L"Tooltip_Reinforce", pGameObject), E_FAIL);


	Engine::Emplace_Layer(L"Player_UI", m_pUILayer);

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

	// Balista Support Fire s
	if (Key_Down('B'))
	{
		if (SCENE_STAGE == Engine::Get_SceneID())
		{
			if (static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Skill_Balista"))->Get_CanUseItemSkill())
			{
				m_bBalistaFire = true;
				m_iBalistaFireCount = 0;

				CAnnounce_Balista*	pAnnounce = static_cast<CAnnounce_Balista*>(Engine::Get_GameObject(L"UI", L"Announce_BalistaAttack_UI"));
				pAnnounce->Set_EnableAnnounce();

				m_pMainCam->Set_CameraMode(CDynamicCamera::MODE_BALISTA_HIGHLIGHT);
				static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Skill_Balista"))->Set_UseItemSkill();
			}
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
		if (Key_Down('G'))
		{
			if (m_bCanAction)
			{
				if (PLAYER_SP_FURY_NO7 <= m_tPlayerInfo.iSkillPoint)
				{
					if (static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Skill_Fury_No7"))->Get_CanUseItemSkill())
					{
						m_eCurAction = PL_SKILL;
						m_iAniIndex = STATE_FURY_NO1;

						m_tPlayerInfo.iSkillPoint -= PLAYER_SP_FURY_NO7;

						static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Skill_Fury_No7"))->Set_UseItemSkill();
					}
				}
			}
		}
		else if (Key_Down('Z'))
		{
			if (m_bCanAction)
			{
				if (PLAYER_SP_FEVER <= m_tPlayerInfo.iSkillPoint)
				{
					if (static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Skill_Sp_Fever"))->Get_CanUseItemSkill())
					{
						m_eCurAction = PL_SKILL;
						m_iAniIndex = STATE_SP_FEVER;

						m_tPlayerInfo.iSkillPoint -= PLAYER_SP_FEVER;

						static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Skill_Sp_Fever"))->Set_UseItemSkill();
					}
				}
			}
		}
		else if (Key_Down('4'))
		{
			if (m_bCanAction)
			{
				if (PLAYER_SP_STAND <= m_tPlayerInfo.iSkillPoint)
				{
					if (static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Skill_Sp_Stand"))->Get_CanUseItemSkill())
					{
						m_eCurAction = PL_SKILL;
						m_iAniIndex = STATE_SP_STAND;

						m_tPlayerInfo.iSkillPoint -= PLAYER_SP_STAND;

						static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Skill_Sp_Stand"))->Set_UseItemSkill();

						Add_Buff(BUFF_REINFORCE, 30000);
					}
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
							if (static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Skill_StickyBomb"))->Get_CanUseItemSkill())
							{
								m_eCurAction = PL_SKILL;
								m_iAniIndex = (_uint)STATE_THROW_DURING;

								m_pMainCam->Set_CameraMode(CDynamicCamera::MODE_SECONDARY);
							}
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

					static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Skill_StickyBomb"))->Set_UseItemSkill();
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
		if (m_bCanAction)
		{
			D3DXVec3Normalize(&vMoveDir, &vMoveDir);

			if (Key_Pressing(VK_SHIFT))
			{
				m_pMainCam->Set_PrePlayerPos(*m_pTransformCom->Get_Info(INFO_POS));
				m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &vMoveDir, m_fSpeed * 1.35f, fTimeDelta, true));
				//m_pTransformCom->Move_Pos(&vMoveDir, m_fSpeed * 1.35f, fTimeDelta);
				m_pMainCam->Sync_PlayerPos(vMoveDir);

				m_iAniIndex = STATE_SPRINT;
			}
			else
			{
				m_pMainCam->Set_PrePlayerPos(*m_pTransformCom->Get_Info(INFO_POS));
				m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &vMoveDir, m_fSpeed, fTimeDelta, true));
				//m_pTransformCom->Move_Pos(&vMoveDir, m_fSpeed, fTimeDelta);
				m_pMainCam->Sync_PlayerPos(vMoveDir);

				m_iAniIndex = STATE_RUN;

				if (m_bPush)
				{
					m_iAniIndex = STATE_PUSH;
				}
			}

			m_eCurAction = PL_MOVE;

			Rotate_PlayerLook(fTimeDelta, vMoveDir);
		}
	}

	// 대기
	if (PL_IDLE >= m_eCurAction)
	{
		if (Key_Down('1'))
		{
			if (m_bCanAction)
			{
				if (static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Potion_Hp"))->Get_CanUseItemSkill())
				{
					m_ePotionType = POTION_HP;

					m_iAniIndex = STATE_POTION_BEGIN;
					Animation_Control();
				}
			}
		}
		else if (Key_Down('2'))
		{
			if (m_bCanAction)
			{
				if (static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Potion_Stemina"))->Get_CanUseItemSkill())
				{
					m_ePotionType = POTION_STEMINA;

					m_iAniIndex = STATE_POTION_BEGIN;
					Animation_Control();
				}
			}
		}
		else if (Key_Down('3'))
		{
			if (m_bCanAction)
			{
				if (static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Potion_Sp"))->Get_CanUseItemSkill())
				{
					m_ePotionType = POTION_SP;

					m_iAniIndex = STATE_POTION_BEGIN;
					Animation_Control();
				}
			}
		}

		if (STATE_POTION_BEGIN != m_eCurState &&
			STATE_POTION_DURING != m_eCurState)
		{
			m_iAniIndex = STATE_IDLE;
		}
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

void CPlayer::Ready_PlayerStructInfo()
{
	m_tPlayerInfo.tagInfo.iHp = 10000;
	m_tPlayerInfo.tagInfo.iMaxHp = m_tPlayerInfo.tagInfo.iHp;

	m_tPlayerInfo.fStamina = 300.f;
	m_tPlayerInfo.fMaxStamina = m_tPlayerInfo.fStamina;

	m_tPlayerInfo.fStaminaRecovery = 0.35f;
	m_tPlayerInfo.fStaminaOriginRecovery = m_tPlayerInfo.fStaminaRecovery;

	m_tPlayerInfo.fAttackSpeedInterpol = 0.f;

	m_tPlayerInfo.iSkillPoint = 0;
	m_tPlayerInfo.iMaxSkillPoint = 1000;
}

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
		if (STATE_SP_FEVER == m_eCurState)
		{
			if (m_fAniTime < (m_lfAniEnd * 0.15f))
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
			if (m_fAniTime < (m_lfAniEnd * 0.25f))
			{
				m_bCanAction = false;
			}
			else
			{
				m_bCanAction = true;
			}
		}
	}
	else if (STATE_POTION_BEGIN == m_eCurState ||
		STATE_POTION_DURING == m_eCurState)
	{
		m_bCanAction = false;
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
			// Buff 효과 
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

				//Safe_Delete(*iter);
				//iter = m_listBuff.erase(iter);
				break;

			case BUFF_STAMINA:
				m_tPlayerInfo.fStaminaRecovery = m_tPlayerInfo.fStaminaOriginRecovery * 2.5f;
				break;

			case BUFF_REINFORCE:
				m_fSpeed = 3.5f;
				m_tPlayerInfo.fAttackSpeedInterpol = 0.25f;
				m_fCriticalPotential = 5.5f;
				break;
			}

			// Buff 시간 종료 시 삭제 
			if ((*iter)->dwBuffStartTime + (*iter)->dwBuffDuration < GetTickCount())
			{
				switch ((*iter)->eBuffID)
				{
				case BUFF_CRITICAL:
					break;

				case BUFF_STAMINA:
					m_tPlayerInfo.fStaminaRecovery = m_tPlayerInfo.fStaminaOriginRecovery;

					static_cast<CBuff_Player*>(Engine::Get_GameObject(L"Player_UI", L"Buff_Stemina_Icon"))->Set_BuffIcon(false);
					static_cast<CTooltip_Stemina*>(Engine::Get_GameObject(L"Player_UI", L"Tooltip_Stemina"))->Set_TooltipOn(false);
					break;

				case BUFF_REINFORCE:
					m_fSpeed = m_fOriginSpeed;
					m_tPlayerInfo.fAttackSpeedInterpol = 0.f;
					m_fCriticalPotential = m_fOriginCriticalPotectial;

					static_cast<CBuff_Player*>(Engine::Get_GameObject(L"Player_UI", L"Buff_Reinforce_Icon"))->Set_BuffIcon(false);
					static_cast<CTooltip_Reinforce*>(Engine::Get_GameObject(L"Player_UI", L"Tooltip_Reinforce"))->Set_TooltipOn(false);
					break;
				}

				Safe_Delete(*iter);
				iter = m_listBuff.erase(iter);
			}

			if (iter != m_listBuff.end())
			{
				++iter;
			}
		}
	}
}

void CPlayer::Compute_Critical(const _matrix* matWorld)
{
	if (STATE_SMASH4_B == m_iAniIndex)
	{
		if (8.f >= Engine::Random(0.f, 10.f))
		{
			Add_Buff(BUFF_CRITICAL, 1500);

			CCriticalEfx*	pEfx = static_cast<CCriticalEfx*>(Engine::Get_GameObject(L"Effect", L"Efx_Critical"));
			if (pEfx)
			{
				_vec3 vPos;
				memcpy(&vPos, &matWorld->_41, sizeof(_vec3));
				pEfx->Set_EnableCriticalEfx(vPos);
			}

			SoundMgrLowerVol(L"hit_common_critical.wav", CSoundMgr::BATTLE, 0.3f);
		}
	}
	else
	{
		if (m_fCriticalPotential >= Engine::Random(0.f, 10.f))
		{
			Add_Buff(BUFF_CRITICAL, 1500);

			CCriticalEfx*	pEfx = static_cast<CCriticalEfx*>(Engine::Get_GameObject(L"Effect", L"Efx_Critical"));
			if (pEfx)
			{
				_vec3 vPos;
				memcpy(&vPos, &matWorld->_41, sizeof(_vec3));
				pEfx->Set_EnableCriticalEfx(vPos);
			}

			SoundMgrLowerVol(L"hit_common_critical.wav", CSoundMgr::BATTLE, 0.3f);
		}
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
		switch (eID)
		{
		case Engine::BUFF_CRITICAL:
			break;

		case Engine::BUFF_STAMINA:
			static_cast<CBuff_Player*>(Engine::Get_GameObject(L"Player_UI", L"Buff_Stemina_Icon"))->Set_BuffIcon(true);
			static_cast<CTooltip_Stemina*>(Engine::Get_GameObject(L"Player_UI", L"Tooltip_Stemina"))->Set_TooltipOn(true);
			break;

		case Engine::BUFF_REINFORCE:
			static_cast<CBuff_Player*>(Engine::Get_GameObject(L"Player_UI", L"Buff_Reinforce_Icon"))->Set_BuffIcon(true);
			static_cast<CTooltip_Reinforce*>(Engine::Get_GameObject(L"Player_UI", L"Tooltip_Reinforce"))->Set_TooltipOn(true);
			break;
		}

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

	if (5.f <= fAngle)
	{
		if (90.f <= fAngle)
		{
			if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) > 0.f)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(fAngle * fTimeDelta * 10.f));
			}
			else if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) < 0.f)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(fAngle * -fTimeDelta * 10.f));
			}
		}
		else
		{
			if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) > 0.f)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(fAngle * fTimeDelta * 25.f));
			}
			else if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) < 0.f)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(fAngle * -fTimeDelta * 25.f));
			}
		}
	}

	// 이전에 쓰던 회전 방식(큰 회전 때는 괜찮은데 작은 회전 때는 뚝뚝 끊기는 느낌이 남)
	// 그래서 윗 방식대로 회전각 비율에 따라서 회전 속도를 조정하되, 어느정도 보정만 해주는 방향으로 변경. 
	/*if (fAngle > 10.f)
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
	else if (fAngle <= 10.f)
	{
		if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) > 0.f)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(45.f * fTimeDelta));
		}
		else if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) < 0.f)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(45.f * -fTimeDelta));
		}
	}*/
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

void CPlayer::MoveOn_Skill(const _float & fTimeDelta)
{
	if (m_bSkillMove && !m_bStopMotion)
	{
		if (CDynamicCamera::MODE_GAME_END != m_pMainCam->Get_CamMode())
		{
			if (PL_TIRED != m_eCurAction)
			{
				if (m_fSkillMoveStartTime <= m_fAniTime &&
					m_fSkillMoveEndTime >= m_fAniTime)
				{
					//m_pMainCam->Set_PrePlayerPos(*m_pTransformCom->Get_Info(INFO_POS));
					m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &(-*m_pTransformCom->Get_Info(INFO_RIGHT)), m_fSkillMoveSpeed, fTimeDelta));
					m_pMainCam->Sync_PlayerPos(-*m_pTransformCom->Get_Info(INFO_RIGHT), m_fSkillMoveSpeed, fTimeDelta);
					//m_pMainCam->Sync_PlayerPos(-*m_pTransformCom->Get_Info(INFO_RIGHT));
				}
				else if (m_fSkillMoveEndTime < m_fAniTime)
				{
					m_bSkillMove = false;
				}
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
		if (pFont0 && pFont1 && pFont2 && pFont3)
		{
			pFont0->Set_EnableDamageFont(vPos, iDigitThousands, 1000);
			pFont1->Set_EnableDamageFont(vPos, iDigitHundreds, 100);
			pFont2->Set_EnableDamageFont(vPos, iDigitTen, 10);
			pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
		}
	}
	else if (100 <= iDamage)
	{
		if (pFont1 && pFont2 && pFont3)
		{
			pFont1->Set_EnableDamageFont(vPos, iDigitHundreds, 100);
			pFont2->Set_EnableDamageFont(vPos, iDigitTen, 10);
			pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
		}
	}
	else if (10 <= iDamage)
	{
		if (pFont2 && pFont3)
		{
			pFont2->Set_EnableDamageFont(vPos, iDigitTen, 10);
			pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
		}
	}
	else if (1 <= iDamage)
	{
		if (pFont3)
		{
			pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
		}
	}
}

void CPlayer::Make_TrailEffect(const _float& fDeltaTime)
{
	map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

	for (; iter != m_mapBoxColliderCom.end(); ++iter)
	{
		if (L"Hit_LHand" == iter->first)
		{
			// 축을 맞춰주기 위해서 반대로 매개변수를 줬다. 
			m_pTrailSmashL->Set_InfoForTrail(fDeltaTime, iter->second->Get_Max(), iter->second->Get_Min(), iter->second->Get_ColliderWorld());
		}
		else if (L"Hit_RHand" == iter->first)
		{
			m_pTrailSmashR->Set_InfoForTrail(fDeltaTime, iter->second->Get_Min(), iter->second->Get_Max(), iter->second->Get_ColliderWorld());
		}
		if (STATE_SMASH4 == m_iAniIndex)
		{
			if (L"Hit_LHandLong" == iter->first)
			{
				// 축을 맞춰주기 위해서 반대로 매개변수를 줬다. 
				m_pTrailSwordL->Set_InfoForTrail(fDeltaTime, iter->second->Get_Max(), iter->second->Get_Min(), iter->second->Get_ColliderWorld());
			}
			else if (L"Hit_RHandLong" == iter->first)
			{
				m_pTrailSwordR->Set_InfoForTrail(fDeltaTime, iter->second->Get_Min(), iter->second->Get_Max(), iter->second->Get_ColliderWorld());
			}
		}
		else
		{
			// Trail용 충돌체를 따로 만들었지만 UV값 변경해주고 나니까 이게 더 어울림..
			if (L"Hit_LHand" == iter->first)
			{
				// 축을 맞춰주기 위해서 반대로 매개변수를 줬다. 
				m_pTrailSwordL->Set_InfoForTrail(fDeltaTime, iter->second->Get_Max(), iter->second->Get_Min(), iter->second->Get_ColliderWorld());
			}
			else if (L"Hit_RHand" == iter->first)
			{
				m_pTrailSwordR->Set_InfoForTrail(fDeltaTime, iter->second->Get_Min(), iter->second->Get_Max(), iter->second->Get_ColliderWorld());
			}
		}
	}

	if (PL_ATK == m_eCurAction ||
		PL_SMASH == m_eCurAction ||
		PL_SKILL == m_eCurAction)
	{
		if (WEAPON_MODE::WEAPON_SECONDARY != m_eCurWeaponMode &&
			STATE_SP_STAND != m_eCurState &&
			STATE_THROW_DURING != m_eCurState &&
			STATE_THROW_END != m_eCurState)
		{
			if (PL_SKILL == m_eCurAction)
			{
				m_pTrailSmashL->Set_Render(true);
				m_pTrailSmashR->Set_Render(true);
			}
			else
			{
				m_pTrailSwordL->Set_Render(true);
				m_pTrailSwordR->Set_Render(true);
			}
		}
	}
	else
	{
		m_pTrailSwordL->Set_Render(false);
		m_pTrailSwordR->Set_Render(false);
		m_pTrailSmashL->Set_Render(false);
		m_pTrailSmashR->Set_Render(false);
	}
}

void CPlayer::SupportFire_Balista()
{
	if (m_bBalistaFire)
	{
		if (STAGE_BALISTA_COUNT <= m_iBalistaFireCount)
		{
			m_bBalistaFire = false;
			m_iBalistaFireCount = 0;
		}

		if (m_dwBalistaFire + m_dwBalistaDelay < GetTickCount())
		{
			map<const wstring, CGameObject*>	mapBalistaObject = Engine::Get_MapObject(L"Balista");
			map<const wstring, CGameObject*>::iterator		iter_Balista = mapBalistaObject.begin();

			for (; iter_Balista != mapBalistaObject.end(); ++iter_Balista)
			{
				CBalista*	pBalista = static_cast<CBalista*>(iter_Balista->second);
				CTransform*	pBaliTrans = static_cast<CTransform*>(pBalista->Get_Component(L"Com_Transform", ID_DYNAMIC));

				if (POOLING_POS == *pBaliTrans->Get_Info(INFO_POS))
				{
					m_dwBalistaFire = GetTickCount();
					++m_iBalistaFireCount;
					m_dwBalistaDelay = (_ulong)Engine::Random(50.f, 150.f);

					_float	fRand = Engine::Random(-10.f, 10.f);

					_vec3	vDir = *D3DXVec3Normalize(&(STAGE_BALISTA_TARGET - STAGE_BALISTA_POS), &(STAGE_BALISTA_TARGET - STAGE_BALISTA_POS));
					_vec3	vPos = STAGE_BALISTA_POS;
					vPos = _vec3(vPos.x + Engine::Random(-20.f, 5.f), vPos.y, vPos.z + Engine::Random(-25.f, 10.f));

					pBalista->Set_EnableBalista(vPos, vDir);

					break;
				}
			}
		}
	}
}

void CPlayer::Animation_Control()
{
	m_fAniTime = m_pMeshCom->Get_AniFrameTime();

	if (CDynamicCamera::MODE_GAME_END == m_pMainCam->Get_CamMode())
	{
		m_bAnimation = false;
	}
	else
	{
		m_bAnimation = true;
	}

	// Speed 조절
	if (PL_ATK == m_eCurAction)
	{
		m_pMeshCom->Set_TrackSpeed(3.25f + (m_tPlayerInfo.fAttackSpeedInterpol + 0.5f));
	}
	else if (PL_SMASH == m_eCurAction || PL_SKILL == m_eCurAction || PL_DASH == m_eCurAction)
	{
		if (STATE_FURY == m_iAniIndex ||
			STATE_FURY2 == m_iAniIndex)
		{
			m_pMeshCom->Set_TrackSpeed(2.15f + (m_tPlayerInfo.fAttackSpeedInterpol + 0.25f));
		}
		else if (STATE_SP_FEVER == m_iAniIndex)
		{
			m_pMeshCom->Set_TrackSpeed(2.65f + (m_tPlayerInfo.fAttackSpeedInterpol + 0.3f));
		}
		else if (STATE_SP_STAND == m_iAniIndex)
		{
			m_pMeshCom->Set_TrackSpeed(3.5f);
		}
		else
		{
			m_pMeshCom->Set_TrackSpeed(2.3f + m_tPlayerInfo.fAttackSpeedInterpol);
		}
	}
	else if (STATE_DAMAGE_RESIST == m_eCurState)
	{
		m_pMeshCom->Set_TrackSpeed(1.8f);
	}
	else if (STATE_RUN == m_eCurState ||
		STATE_SPRINT == m_eCurState)
	{
		m_pMeshCom->Set_TrackSpeed(2.f + m_tPlayerInfo.fAttackSpeedInterpol);
	}
	else
	{
		m_pMeshCom->Set_TrackSpeed(1.9f);
	}

	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);


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
		case STATE_SP_STAND:
			SoundMgrLowerVol(L"lethita_liberalize.wav", CSoundMgr::PLAYER, 0.1f);
			SoundMgrLowerVol(L"effect_lethita_dualspear_liberalize.wav", CSoundMgr::PLAYER_EFFECT2, 0.1f);

			m_pMainCam->Set_HighlightSkillShot(1.38f, 1000);
			break;

		case STATE_POTION_DURING:
			SoundMgr(L"action_drink_potion.wav", CSoundMgr::PLAYER);
			break;

		case STATE_PUSH:
			m_fSpeed = 0.75f;
			break;

		case STATE_FURY_NO2:
			SKILL_MOVE_BYANI(0.1f, 3250.f, 0.2f);
			++m_iFuryNo7Count;
			m_lfAniEnd = 0.6f;

			Rotate_PlayerLook(m_vDir);
			break;

		case STATE_FURY_NO1:
			SKILL_MOVE_BYANI(0.1f, 3250.f, 0.2f);
			++m_iFuryNo7Count;
			m_lfAniEnd = 0.6f;
			if (7 == m_iFuryNo7Count)
				m_lfAniEnd = 1.6f;

			Rotate_PlayerLook(m_vDir);
			break;

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
			m_pMainCam->Set_HighlightSkillShot(1.3f, 500);
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

			//m_lfAniEnd *= 0.7f;
			m_lfAniEnd = 0.9f;
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
			m_lfAniEnd = 1.3f;
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

			m_pMainCam->Set_HighlightSkillShot(1.7f, 700);
			Set_PlayerStemina(-20.f);

			m_eNextAtk = STATE_ATK1;
			break;

		case STATE_DASHATK:
			SKILL_MOVE_BYANI(0.01f, 1100.f, 0.15f);

			m_pMainCam->Set_HighlightSkillShot(1.6f, 700);
			Set_PlayerStemina(-10.f);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_FURY2:
			SKILL_MOVE_BYANI(0.45f, 3250.f, 0.6f);
			m_lfAniEnd = 1.9f;

			Set_PlayerStemina(-30.f);
			break;

		case STATE_FURY:
			SKILL_MOVE_BYANI(0.45f, 2750.f, 0.6f);
			m_lfAniEnd = 1.7f;

			Set_PlayerStemina(-30.f);
			break;

			//case STATE_DASH_W:
			//	break;

		case STATE_DASH_S:
			SKILL_MOVE_BYANI(0.125f, 900.f, 0.3f);

			Set_PlayerStemina(-9.f);

			m_lfAniEnd = 0.568f;
			m_eNextAtk = STATE_ATK1;
			SetNextSmash(STATE_DOUBLE_CRECSENT, 750);
			break;

		case STATE_DASH_N:
			SKILL_MOVE_BYANI(0.125f, 900.f, 0.3f);

			Set_PlayerStemina(-15.f);

			m_lfAniEnd = 0.568f;
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
			SetNextSmash(STATE_SMASH4_B, 750);
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

			m_fSpeed = m_fOriginSpeed;
			m_eNextAtk = STATE_ATK1;
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
		if (STATE_IDLE == m_eCurState ||
			STATE_RUN == m_eCurState ||
			STATE_SPRINT == m_eCurState ||
			STATE_THROW_DURING == m_eCurState ||
			STATE_TIRED_DURING == m_eCurState ||
			STATE_PUSH == m_eCurState)
		{
			m_bAnimation = true;
		}
		else
		{
			m_bAnimation = false;
		}

		switch (m_eCurState)
		{
		case STATE_DEAD:
			break;

		case STATE_DOWNIDLE_FRONT:		// 누운 상태에서 지 혼자 못 일어나게 막음
			break;

		case STATE_DOWNIDLE_BACK:
			break;

		case STATE_PUSH:
			break;

		case STATE_DAMAGEFROM_FRONT:
			//m_iAniIndex = (_uint)STATE_DOWNIDLE_BACK;
			break;

		case STATE_DAMAGEFROM_BACK:
			//m_iAniIndex = (_uint)STATE_DOWNIDLE_FRONT;
			break;

		case STATE_THROW_BEGIN:
			//m_iAniIndex = (_uint)STATE_THROW_DURING;
			break;

		case STATE_THROW_DURING:
			break;

		case STATE_TIRED_BEGIN:
			m_iAniIndex = (_uint)STATE_TIRED_DURING;
			break;

		case STATE_TIRED_DURING:
			break;

		case STATE_POTION_BEGIN:
			m_iAniIndex = STATE_POTION_DURING;
			Animation_Control();
			break;

		default:
			if (STATE_THROW_END == m_ePreState)
			{
				m_eCurWeaponMode = WEAPON_DUALSWORD;

				m_pMainCam->Set_CameraMode(CDynamicCamera::MODE_NORMAL);
				Rotate_PlayerLook(+*m_pTransformCom->Get_Info(INFO_LOOK));
			}
			else if (STATE_FURY_NO1 == m_eCurState ||
				STATE_FURY_NO2 == m_eCurState)
			{
				if (7 > m_iFuryNo7Count)
				{
					if (STATE_FURY_NO1 == m_eCurState)
					{
						m_iAniIndex = STATE_FURY_NO2;
						Animation_Control();
					}
					else
					{
						m_iAniIndex = STATE_FURY_NO1;
						Animation_Control();
					}

					break;
				}
				else
				{
					m_iFuryNo7Count = 0;
				}
			}
			else if (STATE_POTION_DURING == m_eCurState)
			{
				m_iAniIndex = STATE_IDLE;

				switch (m_ePotionType)
				{
				case Engine::POTION_HP:
					m_tPlayerInfo.tagInfo.iHp += 3000;

					static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Potion_Hp"))->Set_UseItemSkill();
					break;

				case Engine::POTION_STEMINA:
					static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Potion_Stemina"))->Set_UseItemSkill();
					Add_Buff(BUFF_STAMINA, 50000);
					break;

				case Engine::POTION_SP:
					m_tPlayerInfo.iSkillPoint += 500;

					static_cast<CSlot_ItemSkill*>(Engine::Get_GameObject(L"Player_UI", L"Potion_Sp"))->Set_UseItemSkill();
					break;
				}

				SoundMgr(L"effect_HP_recover.wav", CSoundMgr::PLAYER_EFFECT);
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
				STATE_FURY_NO1 == m_iAniIndex ||
				STATE_FURY_NO2 == m_iAniIndex ||
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
		case STATE_SMASH4_B:
			HITBOX_CONTROLL(0.8f, 0.9f, TRUE);
			if (!m_bLethitaSound[1] &&
				m_fAniTime >= 0.33f)
			{
				m_bLethitaSound[1] = true;
				SoundMgrLowerVol(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT, 0.15f);
			}
			else if (!m_bAtkSound[1] &&
				m_fAniTime >= 0.45f)
			{
				m_bAtkSound[1] = true;
				SoundMgrLowerVol(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT2, 0.15f);
			}

			if (!m_bAtkSound[0] &&
				m_fAniTime >= 0.85f)
			{
				m_bAtkSound[0] = true;
				_vec3 vPos = *m_pTransformCom->Get_Info(INFO_POS);
				vPos.y += 0.1f;
				m_pRadialBlur->Set_EnableRadialBlur(vPos);

				SoundPlayerStrongAtk;
				SoundMgrLowerVol(L"hit_common_ground_hit.wav", CSoundMgr::PLAYER_EFFECT3, 0.15f);
			}
			m_pMainCam->Set_HighlightSkillShot(1.3f, 350);
			break;

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

		case STATE_FURY_NO1:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (L"Hit_LHandLongLong" == iter->first ||
					L"Hit_RHandLongLong" == iter->first)
				{
					if (0.1f <= m_fAniTime &&
						0.5f >= m_fAniTime)
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
				m_fAniTime >= 0.1f)
			{
				m_bSkillSound[0] = true;
				SoundMgr(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT2);

				//m_pMainCam->Set_HighlightSkillShot(1.65f, 750);
			}
			break;

		case STATE_FURY_NO2:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (L"Hit_LHandLongLong" == iter->first ||
					L"Hit_RHandLongLong" == iter->first)
				{
					if (0.1f <= m_fAniTime &&
						0.5f >= m_fAniTime)
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
			if (!m_bSkillSound[1] &&
				m_fAniTime >= 0.1f)
			{
				m_bSkillSound[1] = true;
				SoundMgr(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT2);

				//m_pMainCam->Set_HighlightSkillShot(1.65f, 750);
			}
			break;

		case STATE_FURY2:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (L"Hit_LHandLongLong" == iter->first ||
					L"Hit_RHandLongLong" == iter->first)
				{
					if (0.37f <= m_fAniTime &&
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
			//HITBOX_CONTROLL(0.4f, 0.6f, TRUE);
			if (!m_bSkillSound[0] &&
				m_fAniTime >= 0.37f)
			{
				m_bSkillSound[0] = true;
				SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT3, 0.18f);
			}

			m_pMainCam->Set_HighlightSkillShot(1.35f, 50);
			break;

		case STATE_FURY:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (L"Hit_LHandLongLong" == iter->first ||
					L"Hit_RHandLongLong" == iter->first)
				{
					if (0.37f <= m_fAniTime &&
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
			//HITBOX_CONTROLL(0.4f, 0.6f, TRUE);
			if (!m_bSkillSound[0] &&
				m_fAniTime >= 0.37f)
			{
				m_bSkillSound[0] = true;
				SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT2, 0.15f);
			}

			m_pMainCam->Set_HighlightSkillShot(1.35f, 50);
			break;

		case STATE_SMASH3:
			HITBOX_CONTROLL(0.35f, 0.5f, TRUE);
			if (!m_bLethitaSound[1] &&
				m_fAniTime >= 0.25f)
			{
				m_bLethitaSound[1] = true;
				SoundPlayerStrongAtk;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);

				m_pMainCam->Set_HighlightSkillShot(1.65f, 750);
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

				m_pMainCam->Set_HighlightSkillShot(1.65f, 750);
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

				m_pMainCam->Set_HighlightSkillShot(1.65f, 750);
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

				m_pMainCam->Set_HighlightSkillShot(1.65f, 750);
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

				m_pMainCam->Set_HighlightSkillShot(1.65f, 750);
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
		m_tPlayerInfo.fStamina += m_tPlayerInfo.fStaminaRecovery;
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

const _ulong & CPlayer::Compute_InCell()
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
