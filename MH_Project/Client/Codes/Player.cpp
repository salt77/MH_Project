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

	m_vDir = *m_pTransformCom->Get_Info(INFO_LOOK);

	return S_OK;
}

_int CPlayer::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	m_pMainCam = dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));

	//SetUp_OnTerrain();
	Compute_CanAction();
	Key_Input(fTimeDelta);

	Animation_Control();
	WeaponCollision_Control();
	MoveOn_Skill(fTimeDelta);

	/*_int iExit = CGameObject::Update_Object(fTimeDelta);*/
	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	m_pMeshCom->Play_Animation(fTimeDelta);

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

	if (m_pNaviMeshCom)
		m_pNaviMeshCom->Render_NaviMesh();
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

HRESULT CPlayer::Add_NaviMesh(_uint iCellCount, vector<_matrix> vecPoint)
{
	//map<const wstring, CComponent*>::iterator	iter = m_mapComponent[ID_STATIC].begin();

	//for (; iter != m_mapComponent[ID_STATIC].end(); ++iter)
	//{
	//	if (L"Com_NaviMesh" == iter->first)
	//	{
	//		Safe_Release(iter->second);
	//		m_mapComponent[ID_STATIC].erase(iter);

	//		break;
	//	}
	//}

	// NaviMesh
	CComponent*		pComponent = nullptr;

	pComponent = m_pNaviMeshCom = dynamic_cast<CNaviMesh*>(Clone_Prototype(L"Proto_NaviMesh"));
	NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	//m_pTransformCom->Get_INFO(INFO_LOOK, &m_vDir);
	//m_pTransformCom->Get_INFO(INFO_RIGHT, &m_vRightDir);

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
	if (m_eCurAction == PL_ATK ||
		m_eCurAction == PL_SMASH ||
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
	_vec3 vUp = _vec3(0.f, 1.f, 0.f);
	_vec3 vPlayerRight = *m_pTransformCom->Get_Info(INFO_RIGHT);
	_vec3 vTemp;

	D3DXVec3Normalize(&TargetLookVector, &TargetLookVector);
	D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);

	_float fAngle = D3DXToDegree(acos(D3DXVec3Dot(&TargetLookVector, &-vPlayerRight)));

	if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) > 0.f)
	{
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(fAngle));
	}
	else if (D3DXVec3Dot(&vUp, D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) < 0.f)
	{
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-fAngle));
	}

	// 디버그용
	_matrix vInfo = *m_pTransformCom->Get_WorldMatrix();
	if (vInfo._11 >= -360.f &&
		vInfo._11 <= 360.f)
	{
		int a = 0;
	}
	else
	{
		int a = 0;
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
			//qm_pTransformCom->Move_Pos(&(-*m_pTransformCom->Get_Info(INFO_RIGHT)), m_fSkillMoveSpeed, fTimeDelta);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &(-*m_pTransformCom->Get_Info(INFO_RIGHT)), m_fSkillMoveSpeed, fTimeDelta));
		}
		else if (m_dwSkillMoveReady + m_dwSkillMoveReadyTime < GetTickCount())
		{
			m_bSkillMove = false;
		}
	}
}

void CPlayer::Animation_Control()
{
	m_fAniTime = m_pMeshCom->Get_AniFrameTime();
	m_lfAniEnd = m_pMeshCom->Get_AniFrameEndTime();

	// Speed 조절
	if (m_eCurAction == PL_ATK)
	{
		m_pMeshCom->Set_TrackSpeed(3.2f);
	}
	else if (m_eCurAction == PL_SMASH || m_eCurAction == PL_SKILL || m_eCurAction == PL_DASH)
	{
		m_pMeshCom->Set_TrackSpeed(2.5f);
	}
	else
	{
		m_pMeshCom->Set_TrackSpeed(2.f);
	}

	// 각 Animation 별 디테일
	m_eCurState = (PL_STATE)m_iAniIndex;
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		//case STATE_SPRINT_STOP:
		//	break;
		case STATE_REVIVE:
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_SMASH1;
			break;

		case STATE_DEAD:
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_DAMAGE_RESIST:
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_DOWNTOIDLE_FRONT:
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_DOWNTOIDLE_BACK:
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
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_DAMAGEFROM_BACK:
			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
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

		case STATE_FURY2:
			break;

		case STATE_FURY:
			break;

		//case STATE_DASH_W:
		//	break;
		//case STATE_DASH_S:
		//	break;
		case STATE_DASH_N:
			SKILL_MOVE(135, 1200.f, 100);

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
			SKILL_MOVE(450, 600.f, 60);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_SMASH4_B;
			break;

		case STATE_SMASH2_B:
			SKILL_MOVE(300, 1100.f, 185);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_SMASH2:
			SKILL_MOVE(250, 600.f, 80);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_SMASH2_B;
			break;

		case STATE_SMASH1:
			SKILL_MOVE(150, 600.f, 80);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;
			break;

		case STATE_ATK4:
			SKILL_MOVE(135, 650.f, 80);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_SMASH4;
			break;

		case STATE_ATK3:
			SKILL_MOVE(150, 650.f, 80);

			m_eNextAtk = STATE_ATK4;
			m_eNextSmash = STATE_SMASH3;
			break;

		case STATE_ATK2:
			SKILL_MOVE(150, 650.f, 80);

			m_eNextAtk = STATE_ATK3;
			m_eNextSmash = STATE_SMASH2;
			break;

		case STATE_ATK1:
			SKILL_MOVE(150, 650.f, 50);

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
		m_ePreState = m_eCurState;
	}

	// State 자동 변경
	if (m_fAniTime >= m_lfAniEnd)
	{
		m_eCurAction = PL_IDLE;

		m_eNextAtk = STATE_ATK1;
		m_eNextSmash = STATE_SMASH1;
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
			HITBOX_CONTROLL(0.25f, 0.7f);
			break;

		case STATE_DASHATK:
			HITBOX_CONTROLL(0.1f, 0.65f);
			break;

		case STATE_FURY2:
			HITBOX_CONTROLL(0.45f, 0.9f);
			break;

		case STATE_FURY:
			HITBOX_CONTROLL(0.45f, 0.9f);
			break;

		case STATE_SMASH3:
			HITBOX_CONTROLL(0.05f, 0.8f);
			break;

		case STATE_SMASH4:
			HITBOX_CONTROLL(0.55f, 0.85f);
			break;

		case STATE_SMASH2_B:
			HITBOX_CONTROLL(0.45f, 0.7f);
			break;

		case STATE_SMASH1:
			HITBOX_CONTROLL(0.35f, 0.6f);
			break;

		case STATE_ATK4:
			HITBOX_CONTROLL(0.3f, 0.7f);
			break;

		case STATE_ATK3:
			HITBOX_CONTROLL(0.3f, 0.6f);
			break;

		case STATE_ATK2:
			HITBOX_CONTROLL(0.25f, 0.6f);
			break;

		case STATE_ATK1:
			HITBOX_CONTROLL(0.25f, 0.5f);
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
