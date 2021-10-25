#include "stdafx.h"
#include "Player.h"

#include "Export_Function.h"
#include "DynamicCamera.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{

}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{

}

CPlayer::~CPlayer(void)
{

}

HRESULT CPlayer::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	//m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(90.f));

	//m_pNaviMeshCom->Set_CellIndex(0);

	return S_OK;
}

_int CPlayer::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	m_pMainCam = dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));

	//SetUp_OnTerrain();
	Key_Input(fTimeDelta);

	/*_int iExit = CGameObject::Update_Object(fTimeDelta);*/
	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	m_pMeshCom->Play_Animation(fTimeDelta);

	Animation_Control();
	WeaponCollision_Control();

	Add_RenderGroup(RENDER_NONALPHA, this);

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
				iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
			}
		}
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	//m_pNaviMeshCom->Render_NaviMesh();
	m_pMeshCom->Render_Meshes();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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

HRESULT CPlayer::Add_Collider(_float fRadius, wstring wstrName, COLLIDERTYPE eColliderType)
{
	CComponent*		pComponent = CCollider::Create(m_pGraphicDev, fRadius, eColliderType);
	m_mapColliderCom.emplace(wstrName, dynamic_cast<CCollider*>(pComponent));
	if (m_mapColliderCom.empty())
		return E_FAIL;

	m_mapComponent[ID_STATIC].emplace(wstrName, pComponent);

	return S_OK;
}

HRESULT CPlayer::Add_Collider(_float vMinX, _float vMinY, _float vMinZ, _float vMaxX, _float vMaxY, _float vMaxZ, wstring wstrName, COLLIDERTYPE eColliderType)
{
	CComponent*		pComponent = CBoxCollider::Create(m_pGraphicDev, vMinX, vMinY, vMinZ, vMaxX, vMaxY, vMaxZ, eColliderType);
	m_mapBoxColliderCom.emplace(wstrName, dynamic_cast<CBoxCollider*>(pComponent));
	if (m_mapBoxColliderCom.empty())
		return E_FAIL;

	m_mapComponent[ID_STATIC].emplace(wstrName, pComponent);

	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	//m_pTransformCom->Get_INFO(INFO_LOOK, &m_vDir);
	//m_pTransformCom->Get_INFO(INFO_RIGHT, &m_vRightDir);

	_vec3 vMoveDir = _vec3(0.f, 0.f, 0.f);
	_vec3 m_vDir = m_pMainCam->Get_CamDirVector(DIR_LOOK);
	_vec3 m_vRightDir = m_pMainCam->Get_CamDirVector(DIR_RIGHT);

	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		m_eCurAction = PL_SMASH;
		m_iAniIndex = m_eNextSmash;
	}
	else if (/*Get_DIMouseState(DIM_LB) & 0X80*/GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		m_eCurAction = PL_ATK;
		m_iAniIndex = m_eNextAtk;
	}

	if (m_eCurAction <= PL_MOVE)
	{
		// 두 가지 이상의 키 값을 누를 때
		if ((GetAsyncKeyState('W') & 0x8000) && (GetAsyncKeyState('A') & 0x8000))
		{
			vMoveDir = m_vDir - m_vRightDir;
		}
		else if ((GetAsyncKeyState('A') & 0x8000) && (GetAsyncKeyState('S') & 0x8000))
		{
			vMoveDir = -m_vDir - m_vRightDir;
		}
		else if ((GetAsyncKeyState('S') & 0x8000) && (GetAsyncKeyState('D') & 0x8000))
		{
			vMoveDir = -m_vDir + m_vRightDir;
		}
		else if ((GetAsyncKeyState('W') & 0x8000) && (GetAsyncKeyState('D') & 0x8000))
		{
			vMoveDir = m_vDir + m_vRightDir;
		}
		// 한 개의 키 값만 누를 때
		else
		{
			if (GetAsyncKeyState('W') & 0x8000)
			{
				vMoveDir = m_vDir;
			}
			else if (GetAsyncKeyState('A') & 0x8000)
			{
				vMoveDir = -m_vRightDir;
			}
			else if (GetAsyncKeyState('S') & 0x8000)
			{
				vMoveDir = -m_vDir;
			}
			else if (GetAsyncKeyState('D') & 0x8000)
			{
				vMoveDir = m_vRightDir;
			}
			else
			{
				m_eCurAction = PL_IDLE;
			}
		}

		if (vMoveDir != _vec3(0.f, 0.f, 0.f))
		{
			D3DXVec3Normalize(&vMoveDir, &vMoveDir);

			m_pTransformCom->Move_Pos(&vMoveDir, m_fSpeed, fTimeDelta);
			//m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &vMoveDir, m_fSpeed, fTimeDelta));
			m_pMainCam->Sync_PlayerPos(vMoveDir, m_fSpeed, fTimeDelta);

			m_eCurAction = PL_MOVE;
			m_iAniIndex = STATE_RUN;

			Rotate_PlayerLook(fTimeDelta, vMoveDir);
		}
	}

	if (m_eCurAction <= PL_IDLE)
	{
		m_iAniIndex = STATE_IDLE;
	}
}

void CPlayer::Action_Change()
{
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
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(360.f * fTimeDelta * 2.f));
		}
		else if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) < 0.f)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(360.f * -fTimeDelta * 2.f));
		}
	}
}

void CPlayer::Animation_Control()
{
	_float fAniTime = m_pMeshCom->Get_AniFrameTime();
	_double lfAniEnd = m_pMeshCom->Get_AniFrameEndTime();

	// Speed 조절
	if (m_eCurAction == (PL_ATK || PL_SMASH || PL_SKILL))
	{
		m_pMeshCom->Set_TrackSpeed(2.5f);
	}
	else
	{
		m_pMeshCom->Set_TrackSpeed(2.f);
	}

	// State 자동 변경
	if (fAniTime >= lfAniEnd)
	{
		m_eCurAction = PL_IDLE;

		m_eNextAtk = STATE_ATK1;
		m_eNextSmash = STATE_SMASH1;
	}

	// 각 Animation 별 디테일
	switch (m_iAniIndex)
	{
	case STATE_SPRINT_STOP:
		break;

	case STATE_REVIVE:
		break;

	case STATE_DEAD:
		break;

	case STATE_DAMAGE_RESIST:
		break;

	case STATE_DOWNTOIDLE_FRONT:
		break;

	case STATE_DOWNTOIDLE_BACK:
		break;

	case STATE_DOWNIDLE_FRONT:
		break;

	case STATE_DOWNIDLE_BACK:
		break;

	case STATE_DAMAGEFROM_FRONT:
		break;

	case STATE_DAMAGEFROM_BACK:
		break;

	case STATE_WINDMILL:
		break;

	case STATE_DOUBLE_CRECSENT:
		break;

	case STATE_DASHATK:
		break;

	case STATE_FURY2:
		break;

	case STATE_FURY:
		break;

	case STATE_DASH_W:
		break;

	case STATE_DASH_S:
		break;

	case STATE_DASH_N:
		break;

	case STATE_DASH_E:
		break;

	case STATE_SMASH3:
		m_eNextAtk = STATE_ATK1;
		m_eNextSmash = STATE_SMASH1;
		break;

	case STATE_SMASH4_B:
		m_eNextAtk = STATE_ATK1;
		m_eNextSmash = STATE_SMASH1;
		break;

	case STATE_SMASH4:
		m_eNextAtk = STATE_ATK1;
		m_eNextSmash = STATE_SMASH4_B;
		break;

	case STATE_SMASH2_B:
		m_eNextAtk = STATE_ATK1;
		m_eNextSmash = STATE_SMASH1;
		break;

	case STATE_SMASH2:
		m_eNextAtk = STATE_ATK1;
		m_eNextSmash = STATE_SMASH2_B;
		break;

	case STATE_SMASH1:
		m_eNextAtk = STATE_ATK1;
		m_eNextSmash = STATE_SMASH1;
		break;

	case STATE_ATK4:
		m_eNextAtk = STATE_ATK1;
		m_eNextSmash = STATE_SMASH4;
		break;

	case STATE_ATK3:
		m_eNextAtk = STATE_ATK4;
		m_eNextSmash = STATE_SMASH3;
		break;

	case STATE_ATK2:
		m_eNextAtk = STATE_ATK3;
		m_eNextSmash = STATE_SMASH2;
		break;

	case STATE_ATK1:
		m_eNextAtk = STATE_ATK2;
		m_eNextSmash = STATE_SMASH1;
		break;

	default:
		break;
	}
}

void CPlayer::WeaponCollision_Control()
{
	if (!m_mapBoxColliderCom.empty())
	{
		_float	fAniTime = m_pMeshCom->Get_AniFrameTime();
		map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		switch (m_iAniIndex)
		{
		case STATE_DOUBLE_CRECSENT:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.25f <= fAniTime &&
					0.7f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			break;

		case STATE_DASHATK:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.1f <= fAniTime &&
					0.65f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			break;

		case STATE_FURY2:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.45f <= fAniTime &&
					0.9f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			break;

		case STATE_FURY:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.45f <= fAniTime &&
					0.9f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			break;

		case STATE_SMASH3:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.05f <= fAniTime &&
					0.8f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			break;

		case STATE_SMASH4:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.6f <= fAniTime &&
					0.85f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			break;

		case STATE_SMASH2_B:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.45f <= fAniTime &&
					0.7f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			break;

		case STATE_SMASH1:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.35f <= fAniTime &&
					0.6f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			break;

		case STATE_ATK4:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.3f <= fAniTime &&
					0.7f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			break;

		case STATE_ATK3:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.3f <= fAniTime &&
					0.6f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			break;

		case STATE_ATK2:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.25f <= fAniTime &&
					0.6f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
			}
			break;

		case STATE_ATK1:
			for (; iter != m_mapBoxColliderCom.end(); ++iter)
			{
				if (0.25f <= fAniTime &&
					0.5f >= fAniTime)
				{
					iter->second->Set_CanCollision(true);
				}
				else
				{
					iter->second->Set_CanCollision(false);
				}
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
}
