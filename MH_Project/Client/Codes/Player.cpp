#include "stdafx.h"
#include "Player.h"

#include "Export_Function.h"
#include "DynamicCamera.h"
#include "Ahglan.h"
#include "StickyBomb.h"
#include "Player_Hpbar_ValueUI.h"
#include "Player_Hpbar_LerpUI.h"

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
	m_tPlayerInfo.iStamina = 300;
	m_tPlayerInfo.iMaxStamina = m_tPlayerInfo.iStamina;
	m_tPlayerInfo.iSkillPoint = 100;
	m_tPlayerInfo.iMaxSkillPoint = m_tPlayerInfo.iSkillPoint;

	return S_OK;
}

HRESULT CPlayer::LateReady_Object()
{
	m_pStickyLayer = CLayer::Create();

	if (!m_pStickyLayer)
		return E_FAIL;

	m_mapActiveParts.emplace(L"dualsword_vanquisher.tga", TRUE);
	m_mapActiveParts.emplace(L"sticky_bomb.tga", FALSE);

	m_pHpbarValueUI = dynamic_cast<CPlayer_Hpbar_ValueUI*>(Engine::Get_GameObject(L"UI", L"Player_Hpbar_ValueUI"));
	m_pHpbarLerpUI = dynamic_cast<CPlayer_Hpbar_LerpUI*>(Engine::Get_GameObject(L"UI", L"Player_Hpbar_LerpUI"));

	return S_OK;
}

_int CPlayer::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	if (m_bDead)
		return iExit;

	m_pMainCam = dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));

	//SetUp_OnTerrain();
	Compute_CanAction();
	Key_Input(fTimeDelta);
	SecondaryMode_MouseMove();
	Weapon_Change();

	Animation_Control();
	Collision_Control();
	Update_State();
	Update_UI();
	FootStepSound();
	MoveOn_Skill(fTimeDelta);

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

				iter->second->Set_ColliderMatrix(m_pTransformCom->Get_WorldMatrix());
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
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

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

	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
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
	Safe_Release(m_pStickyLayer);

	CGameObject::Free();
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
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	//// NaviMesh
	//pComponent = m_pNaviMeshCom = dynamic_cast<CNaviMesh*>(Clone_Prototype(L"Proto_NaviMesh"));
	//NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	//m_mapComponent[ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	//// Collider
	//pComponent = m_pColliderCom = CCollider::Create(m_pGraphicDev, m_pMeshCom->Get_VtxPos(), m_pMeshCom->Get_VtxCnt(), m_pMeshCom->Get_Stride());
	//NULL_CHECK_RETURN(m_pColliderCom, E_FAIL);
	//m_mapComponent[ID_STATIC].emplace(L"Com_Collider", pComponent);

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

	// 스킬 액션(퓨리, sp스킬, 보조무기 액션)
	if (PL_SKILL >= m_eCurAction)
	{
		if (Key_Down('F'))
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
				m_eCurAction = PL_DASH;
				m_iAniIndex = STATE_DASH_N;
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
			//m_pTransformCom->Move_Pos(&vMoveDir, m_fSpeed * 1.35f, fTimeDelta);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &vMoveDir, m_fSpeed * 1.35f, fTimeDelta));
			m_pMainCam->Sync_PlayerPos(vMoveDir, m_fSpeed * 1.35f, fTimeDelta);

			m_iAniIndex = STATE_SPRINT;
		}
		else
		{
			//m_pTransformCom->Move_Pos(&vMoveDir, m_fSpeed, fTimeDelta);
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

void CPlayer::SetUp_OnTerrain(void)
{
	_vec3		vPos;
	m_pTransformCom->Get_INFO(INFO_POS, &vPos);

	CTerrainTex*	pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Buffer", ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ);
	m_pTransformCom->Set_Pos(vPos.x, fHeight, vPos.z);
}

Engine::_vec3 CPlayer::PickUp_OnTerrain(void)
{
	CTerrainTex*	pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Buffer", ID_STATIC));
	NULL_CHECK_RETURN(pTerrainBufferCom, _vec3(0.f, 0.f, 0.f));

	CTransform*	pTerrainTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Transform", ID_DYNAMIC));
	NULL_CHECK_RETURN(pTerrainTransCom, _vec3(0.f, 0.f, 0.f));

	return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransCom);
}

void CPlayer::Compute_CanAction()
{
	if (m_eCurAction == PL_ATK)
	{
		if (STATE_ATK1 != m_iAniIndex &&
			STATE_ATK4 != m_iAniIndex)
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
		else if (STATE_ATK1 == m_iAniIndex)
		{
			if (m_fAniTime < (m_lfAniEnd * 0.27f))
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
	else if (m_eCurAction == PL_SMASH ||
			 m_eCurAction == PL_SKILL ||
			 m_eCurAction == PL_DASH)
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

void CPlayer::Rotate_PlayerLook(const _float& fTimeDelta, _vec3& TargetLookVector)
{
	_vec3 vUp = _vec3(0.f, 1.f, 0.f);
	_vec3 vPlayerRight = *m_pTransformCom->Get_Info(INFO_RIGHT);
	_vec3 vTemp;

	D3DXVec3Normalize(&TargetLookVector, &TargetLookVector);
	D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);

	if (D3DXToDegree(acos(D3DXVec3Dot(&TargetLookVector, &-vPlayerRight))) > 5.f)
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

	// Debug 용
	_vec3	vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
	if (!(-100.f <= vMyPos.y && 100.f >= vMyPos.y))
	{
		_vec3 vPos = vMyPos;
	}
}

void CPlayer::MoveOn_Skill(const _float& fTimeDelta)
{
	if (m_bSkillMove)
	{
		if (m_dwSkillMoveReady + m_dwSkillMoveReadyTime < GetTickCount())
		{
			m_dwSkillMoveReady = GetTickCount();
			m_dwSkillMoveReadyTime = 9999999;
			m_dwSkillMoveStart = GetTickCount();
		}

		if (m_dwSkillMoveStart + m_dwSkillMoveTime >= GetTickCount())
		{
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &(-*m_pTransformCom->Get_Info(INFO_RIGHT)), m_fSkillMoveSpeed, fTimeDelta));
			m_pMainCam->Sync_PlayerPos(-*m_pTransformCom->Get_Info(INFO_RIGHT), m_fSkillMoveSpeed, fTimeDelta);
		}
		else if (m_dwSkillMoveReady + m_dwSkillMoveReadyTime < GetTickCount())
		{
			m_bSkillMove = false;
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

void CPlayer::Animation_Control()
{
	m_fAniTime = m_pMeshCom->Get_AniFrameTime();
	m_lfAniEnd = m_pMeshCom->Get_AniFrameEndTime();

	// Speed 조절
	if (PL_ATK == m_eCurAction)
	{
		m_pMeshCom->Set_TrackSpeed(3.2f);
	}
	else if (PL_SMASH == m_eCurAction || PL_SKILL == m_eCurAction || PL_DASH == m_eCurAction)
	{
		m_pMeshCom->Set_TrackSpeed(2.5f);
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

	// 각 Animation 별 디테일
	m_eCurState = (PL_STATE)m_iAniIndex;
	if (m_eCurState != m_ePreState)
	{
		m_bAnimation = true;

		// Sound 변수 초기화
		m_bAtkSound = false;
		m_bLethitaSound = false;
		/////////////////////

		_uint	iRandSound = 0;
		switch (m_eCurState)
		{
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
			NULL_CHECK(m_pStickyLayer);

			{
				CGameObject*	pGameObject = nullptr;

				pGameObject = CStickyBomb::Create(m_pGraphicDev);
				NULL_CHECK(pGameObject);

				//wstring	wstrName = L"StickyBomb" + to_wstring(m_iSecondaryCount);
				FAILED_CHECK_RETURN(m_pStickyLayer->Add_GameObject(L"StickyBomb", pGameObject), );

				Engine::Emplace_Layer(L"StickyBomb", m_pStickyLayer);

				//++m_iSecondaryCount;
			}

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_SP_FEVER:
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
			SKILL_MOVE(100, -400.f, 750);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			m_lfAniEnd *= 0.65f;

			SoundPlayerHurt;
			SoundMgr(L"Skill_DownResist.wav", CSoundMgr::PLAYER_EFFECT);
			break;

		case STATE_DOWNTOIDLE_FRONT:
			break;

		case STATE_DOWNTOIDLE_BACK:
			Rotate_PlayerLook(m_vDir);

			SKILL_MOVE(200, 350.f, 750);

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
			SKILL_MOVE(0, -350.f, 700);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			m_lfAniEnd *= 0.1f;

			SoundPlayerHurt;
			break;

		case STATE_DAMAGEFROM_BACK:
			SKILL_MOVE(0, 350.f, 700);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			m_lfAniEnd *= 0.05f;

			SoundPlayerHurt;
			break;

		case STATE_WINDMILL:
			break;

		case STATE_DOUBLE_CRECSENT:
			SKILL_MOVE(135, 300.f, 250);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_DASHATK:
			SKILL_MOVE(10, 1100.f, 150);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_FURY2:
			break;

		case STATE_FURY:
			break;

			//case STATE_DASH_W:
			//	break;
		case STATE_DASH_S:
			break;

		case STATE_DASH_N:
			SKILL_MOVE(125, 1200.f, 125);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DOUBLE_CRECSENT;
			break;
			//case STATE_DASH_E:
			//	break;

		case STATE_SMASH3:
			SKILL_MOVE(180, 750.f, 275);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_SMASH1;
			break;

		case STATE_SMASH4_B:
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_SMASH4:
			SKILL_MOVE(450, 600.f, 80);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_SMASH4_B;
			break;

		case STATE_SMASH2_B:
			SKILL_MOVE(300, 1100.f, 200);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_SMASH2:
			SKILL_MOVE(250, 600.f, 95);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_SMASH2_B;
			break;

		case STATE_SMASH1:
			SKILL_MOVE(150, 600.f, 95);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_ATK4:
			SKILL_MOVE(135, 650.f, 95);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_SMASH4;
			break;

		case STATE_ATK3:
			SKILL_MOVE(150, 650.f, 95);

			m_eNextAtk = STATE_ATK4;
			m_eNextSmash = STATE_SMASH3;
			break;

		case STATE_ATK2:
			SKILL_MOVE(150, 650.f, 95);

			m_eNextAtk = STATE_ATK3;
			m_eNextSmash = STATE_SMASH2;
			break;

		case STATE_ATK1:
			SKILL_MOVE(150, 650.f, 70);

			m_eNextAtk = STATE_ATK2;
			m_eNextSmash = STATE_SMASH1;
			break;

		default:
			SKILL_MOVE_END;

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;
		}

		m_fAniTime = 0.f;
		m_bCanHit = true;
		m_ePreState = m_eCurState;
	}

	// 매 프레임 실행
	switch (m_eCurState)
	{
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
			Engine::STATE_SPRINT == m_eCurState)
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

		default:
			if (STATE_THROW_END == m_ePreState)
			{
				m_eCurWeaponMode = WEAPON_DUALSWORD;

				m_pMainCam->Set_CameraMode(CDynamicCamera::MODE_NORMAL);
				Rotate_PlayerLook(+*m_pTransformCom->Get_Info(INFO_LOOK));
			}

			m_eCurAction = PL_IDLE;

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;
		}
	}
}

void CPlayer::Collision_Control()
{
	_float	fAniTime = m_pMeshCom->Get_AniFrameTime();

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

		_uint iRandSound = 0;
		switch (m_iAniIndex)
		{
		case STATE_DOUBLE_CRECSENT:
			/*for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (L"Other_Attack" == iter->first)
				{
					if (0.35f <= fAniTime &&
						0.7f >= fAniTime)
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
			}*/
			HITBOX_CONTROLL(0.35f, 0.7f, TRUE);
			if (!m_bAtkSound &&
				m_fAniTime >= 0.3f)
			{
				m_bAtkSound = true;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_DASHATK:
			HITBOX_CONTROLL(0.1f, 0.65f, FALSE);
			if (!m_bAtkSound &&
				m_fAniTime >= 0.1f)
			{
				m_bAtkSound = true;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_FURY2:
			HITBOX_CONTROLL(0.45f, 0.8f, TRUE);
			if (!m_bLethitaSound &&
				m_fAniTime >= 0.45f)
			{
				m_bAtkSound = true;
				SoundMgr(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_FURY:
			HITBOX_CONTROLL(0.45f, 0.8f, TRUE);
			if (!m_bLethitaSound &&
				m_fAniTime >= 0.45f)
			{
				m_bAtkSound = true;
				SoundMgr(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_SMASH3:
			HITBOX_CONTROLL(0.35f, 0.5f, TRUE);
			if (!m_bLethitaSound &&
				m_fAniTime >= 0.25f)
			{
				m_bLethitaSound = true;
				SoundPlayerStrongAtk;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);
			}
			else if (!m_bAtkSound &&
				m_fAniTime >= 0.45f)
			{
				m_bAtkSound = true;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_SMASH4:
			HITBOX_CONTROLL(0.55f, 0.8f, TRUE);
			if (!m_bLethitaSound &&
				m_fAniTime >= 0.55f)
			{
				m_bLethitaSound = true;
				SoundPlayerStrongAtk;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_SMASH2:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (L"Other_Attack" == iter->first)
				{
					if (0.5f <= fAniTime &&
						0.7f >= fAniTime)
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
			if (!m_bLethitaSound &&
				m_fAniTime >= 0.5f)
			{
				m_bLethitaSound = true;
				SoundPlayerStrongAtk;
				SoundMgr(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_SMASH2_B:
			HITBOX_CONTROLL(0.45f, 0.7f, TRUE);
			if (!m_bLethitaSound &&
				m_fAniTime >= 0.45f)
			{
				m_bLethitaSound = true;
				SoundPlayerStrongAtk;
				SoundMgr(L"Swing_Metal.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_SMASH1:
			HITBOX_CONTROLL(0.4f, 0.6f, TRUE);
			if (!m_bLethitaSound &&
				m_fAniTime >= 0.3f)
			{
				m_bLethitaSound = true;
				SoundPlayerStrongAtk;
				SoundMgr(L"Swing_MetalStrong.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_ATK4:
			HITBOX_CONTROLL(0.3f, 0.7f, FALSE);
			if (!m_bLethitaSound &&
				m_fAniTime >= 0.3f)
			{
				m_bLethitaSound = true;
				SoundPlayerAtk;
				SoundMgr(L"Swing_Metal.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_ATK3:
			HITBOX_CONTROLL(0.3f, 0.6f, FALSE);
			if (!m_bLethitaSound &&
				m_fAniTime >= 0.3f)
			{
				m_bLethitaSound = true;
				SoundPlayerAtk;
				SoundMgr(L"Swing_Metal.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_ATK2:
			HITBOX_CONTROLL(0.25f, 0.6f, FALSE);
			if (!m_bLethitaSound &&
				m_fAniTime >= 0.25f)
			{
				m_bLethitaSound = true;
				SoundPlayerAtk;
				SoundMgr(L"Swing_Metal.wav", CSoundMgr::PLAYER_EFFECT);
			}
			break;

		case STATE_ATK1:
			HITBOX_CONTROLL(0.25f, 0.5f, FALSE);
			if (!m_bLethitaSound &&
				m_fAniTime >= 0.25f)
			{
				m_bLethitaSound = true;
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

	// 실제 충돌 처리 
	m_eSceneID = Engine::Get_SceneID();

	map<const wstring, CCollider*>::iterator		iter_PlayerDamaged = m_mapColliderCom.begin();
	map<const wstring, CCollider*>::iterator		iter_BossHit;
	map<const wstring, CCollider*>					mapBossSphereCol;

	map<const wstring, CBoxCollider*>::iterator		iter_PlayerHit = m_mapBoxColliderCom.begin();
	map<const wstring, CBoxCollider*>::iterator		iter_BossDamaged;
	map<const wstring, CBoxCollider*>				mapBossBoxCol;

	CAhglan*	pAhglan = static_cast<CAhglan*>(Engine::Get_GameObject(L"GameLogic", L"Ahglan"));

	switch (m_eSceneID)
	{
	case Engine::SCENE_STAGE:
		// Player 공격
		if (m_bCanHit)
		{
			if (pAhglan)
			{
				mapBossBoxCol = pAhglan->Get_MapBoxCollider();

				for (; iter_PlayerHit != m_mapBoxColliderCom.end(); ++iter_PlayerHit)
				{
					iter_BossDamaged = mapBossBoxCol.begin();

					for (; iter_BossDamaged != mapBossBoxCol.end(); ++iter_BossDamaged)
					{
						//iter_PlayerHit->second->Set_RenderColType(COL_FALSE);
						//iter_BossDamaged->second->Set_RenderColType(COL_FALSE);

						if (iter_PlayerHit->second->Get_CanCollision() &&
							iter_BossDamaged->second->Get_CanCollision())
						{
							const _matrix*	pMatrix = iter_PlayerHit->second->Get_ColliderWorld();
							_matrix matTemp = *pMatrix;
							if (m_pCalculatorCom->Collision_OBB(&iter_PlayerHit->second->Get_Min(), &iter_PlayerHit->second->Get_Max(), iter_PlayerHit->second->Get_ColliderWorld(),
																&iter_BossDamaged->second->Get_Min(), &iter_BossDamaged->second->Get_Max(), iter_BossDamaged->second->Get_ColliderWorld()))
							{
								switch (m_eCurState)
								{
								case Engine::STATE_DOUBLE_CRECSENT:
									SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.08f);
									STOP_MOTION(100);
									break;

								case Engine::STATE_SMASH4:
									STOP_MOTION(100);
									SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.08f);
									break;

								case Engine::STATE_SMASH3:
									//STOP_MOTION(100);
									SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.08f);
									break;

								case Engine::STATE_SMASH2_B:
									SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
									break;

								case Engine::STATE_SMASH2:
									STOP_MOTION(100);
									SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.08f);
									break;

								case Engine::STATE_SMASH1:
									STOP_MOTION(75);
									SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.08f);
									break;

								case Engine::STATE_ATK1:
									STOP_MOTION(35);
									SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
									break;

								case Engine::STATE_ATK2:
									STOP_MOTION(35);
									SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
									break;

								case Engine::STATE_ATK3:
									STOP_MOTION(35);
									SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
									break;

								case Engine::STATE_ATK4:
									STOP_MOTION(35);
									SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
									break;
								}

								//iter_PlayerHit->second->Set_RenderColType(COL_TRUE);
								//iter_BossDamaged->second->Set_RenderColType(COL_TRUE);

								m_bCanHit = false;

								break;
							}
						}
					}
				}
			}
		}

		// Monster 공격
		if (pAhglan)
		{
			mapBossSphereCol = pAhglan->Get_MapCollider();

			for (_uint i = 0; i < m_mapColliderCom.size(); ++i)
			{
				iter_BossHit = mapBossSphereCol.begin();

				for (_uint j = 0; j < mapBossSphereCol.size(); ++j)
				{
					//iter_PlayerDamaged->second->Set_RenderColType(COL_FALSE);
					//iter_BossHit->second->Set_RenderColType(COL_FALSE);

					if (iter_PlayerDamaged->second->Get_CanCollision() &&
						iter_BossHit->second->Get_CanCollision())
					{
						if (m_pCalculatorCom->Collision_Sphere(iter_PlayerDamaged->second->Get_Center(), iter_PlayerDamaged->second->Get_Radius() * SCALE_PLAYER,
							iter_BossHit->second->Get_Center(), iter_BossHit->second->Get_Radius() * SCALE_AHGLAN))
						{
							if (80.f <= Engine::Random(0.f, 100.f) && 
								AHGLAN_ATKPOWER < m_tPlayerInfo.tagInfo.iHp)
							{
								m_eCurAction = PL_DAMAGED;
								m_iAniIndex = STATE_DAMAGE_RESIST;

								if (!m_bLethitaSound)
								{
									m_bLethitaSound = true;
									SoundMgr(L"Hit_Flesh_Blunt.wav", CSoundMgr::PLAYER);

									m_tPlayerInfo.tagInfo.iHp -= AHGLAN_ATKPOWER;
								}
							}
							else
							{
								CTransform*	pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

								if (pBossTrans)
								{
									_vec3 vLookDir = *m_pTransformCom->Get_Info(INFO_RIGHT);
									_vec3 vToBossDir = iter_BossHit->second->Get_Center() - *m_pTransformCom->Get_Info(INFO_POS);
									D3DXVec3Normalize(&vLookDir, &vLookDir);
									D3DXVec3Normalize(&vToBossDir, &vToBossDir);

									if (D3DXVec3Dot(&vToBossDir, &vLookDir) > 0.f)
									{
										m_eCurAction = PL_DAMAGED;
										m_iAniIndex = STATE_DAMAGEFROM_FRONT;

										if (!m_bLethitaSound)
										{
											m_bLethitaSound = true;
											SoundMgr(L"Hit_Flesh_Blunt.wav", CSoundMgr::PLAYER);

											m_tPlayerInfo.tagInfo.iHp -= AHGLAN_ATKPOWER;
										}
									}
									else
									{
										m_eCurAction = PL_DAMAGED;
										m_iAniIndex = STATE_DAMAGEFROM_BACK;

										if (!m_bLethitaSound)
										{
											m_bLethitaSound = true;
											SoundMgr(L"Hit_Flesh_Blunt.wav", CSoundMgr::PLAYER);

											m_tPlayerInfo.tagInfo.iHp -= AHGLAN_ATKPOWER;
										}
									}
								}
							}

							//iter_PlayerDamaged->second->Set_RenderColType(COL_TRUE);
							//iter_BossHit->second->Set_RenderColType(COL_TRUE);

							break;
						}
					}

					++iter_BossHit;
				}

				++iter_PlayerDamaged;
			}
		}
		break;

	default:
		break;
	}
}

void CPlayer::Update_UI()
{
	// Hpbar
	if (m_pHpbarValueUI)
	{
		m_pHpbarValueUI->Set_ValueRatio(m_tPlayerInfo.tagInfo.iHp * 0.0001f);
	}
	if (m_pHpbarLerpUI)
	{
		m_pHpbarLerpUI->Set_ValueRatio(m_tPlayerInfo.tagInfo.iHp * 0.0001f);
	}
}

void CPlayer::Update_State()
{
	if (0 >= m_tPlayerInfo.tagInfo.iHp)
	{
		m_tPlayerInfo.tagInfo.iHp = 0;

		m_eCurAction = PL_DEAD;
	}
	else if (m_tPlayerInfo.tagInfo.iMaxHp <= m_tPlayerInfo.tagInfo.iHp)
	{
		m_tPlayerInfo.tagInfo.iHp = m_tPlayerInfo.tagInfo.iMaxHp;
	}

	if (0 >= m_tPlayerInfo.iStamina)
	{
		m_tPlayerInfo.iStamina = 0;
	}
	else if (m_tPlayerInfo.iMaxStamina <= m_tPlayerInfo.iStamina)
	{
		m_tPlayerInfo.iStamina = m_tPlayerInfo.iMaxStamina;
	}

	if (0 >= m_tPlayerInfo.iSkillPoint)
	{
		m_tPlayerInfo.iSkillPoint = 0;
	}
	else if (m_tPlayerInfo.iMaxSkillPoint <= m_tPlayerInfo.iSkillPoint)
	{
		m_tPlayerInfo.iSkillPoint = m_tPlayerInfo.iMaxSkillPoint;
	}
}
