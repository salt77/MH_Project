#include "stdafx.h"
#include "Player.h"

#include "Export_Function.h"
#include "DynamicCamera.h"
#include "Ahglan.h"

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

	m_pTransformCom->Set_Pos(0.f, 0.f, 10.f);
	m_pTransformCom->Set_Scale(SCALE_PLAYER, SCALE_PLAYER, SCALE_PLAYER);
	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(270.f));

	//m_pNaviMeshCom->Set_CellIndex(0);

	m_vDir = *m_pTransformCom->Get_Info(INFO_LOOK);

	return S_OK;
}

_int CPlayer::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	m_pMainCam = dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));

	_vec3	vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
	if (!(-100.f <= vMyPos.y && 100.f >= vMyPos.y))
	{
		_vec3 vPos = vMyPos;
	}

	//SetUp_OnTerrain();
	Compute_CanAction();
	Key_Input(fTimeDelta);

	Animation_Control();
	Collision_Control();
	MoveOn_Skill(fTimeDelta);

	/*_int iExit = CGameObject::Update_Object(fTimeDelta);*/
	StopMotion();

	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	if (!m_bStopMotion)
		m_pMeshCom->Play_Animation(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

_int CPlayer::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

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

	m_pMeshCom->Render_Meshes(pEffect);

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

	// Damaged
	if (PL_DAMAGED >= m_eCurAction)
	{
		if (Key_Down('V'))
		{
			if (m_bCanAction)
			{
				if (STATE_DOWNIDLE_FRONT == m_iAniIndex)
				{
					m_iAniIndex = STATE_DOWNTOIDLE_BACK;
				}
				else if (STATE_DOWNIDLE_BACK == m_iAniIndex)
				{
					m_iAniIndex = STATE_DOWNTOIDLE_BACK;
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
	_vec3 vPlayerRight = *m_pTransformCom->Get_Info(INFO_RIGHT);
	_vec3 vTemp;

	D3DXVec3Normalize(&TargetLookVector, &TargetLookVector);
	D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);

	if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) > 0.f)
	{
		m_pTransformCom->Rotation(ROT_Y, acos(D3DXVec3Dot(&TargetLookVector, &-vPlayerRight)));
	}
	else if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&vTemp, &TargetLookVector, &vPlayerRight)) < 0.f)
	{
		m_pTransformCom->Rotation(ROT_Y, -acos(D3DXVec3Dot(&TargetLookVector, &-vPlayerRight)));
	}

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
		m_pMeshCom->Set_TrackSpeed(1.75f);
	}
	else
	{
		m_pMeshCom->Set_TrackSpeed(2.f);
	}

	// 각 Animation 별 디테일
	m_eCurState = (PL_STATE)m_iAniIndex;
	if (m_eCurState != m_ePreState)
	{
		_uint	iRandSound;

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
			SKILL_MOVE(0, -350.f, 700);

			m_eNextAtk = STATE_ATK1;
			m_eNextSmash = STATE_DASHATK;

			iRandSound = rand() % 4;
			if (0 == iRandSound)
			{
				SoundMgrLowerVol(L"lethita_hurt1.wav", CSoundMgr::PLAYER, 0.2f);
			}
			else if (1 == iRandSound)
			{
				SoundMgrLowerVol(L"lethita_hurt2.wav", CSoundMgr::PLAYER, 0.2f);
			}
			else if (2 == iRandSound)
			{
				SoundMgrLowerVol(L"lethita_hurt3.wav", CSoundMgr::PLAYER, 0.2f);
			}
			else if (3 == iRandSound)
			{
				SoundMgrLowerVol(L"lethita_hurt4.wav", CSoundMgr::PLAYER, 0.2f);
			}
			break;

		case STATE_DOWNTOIDLE_FRONT:
			break;

		case STATE_DOWNTOIDLE_BACK:
			Rotate_PlayerLook(m_vDir);

			SKILL_MOVE(200, 350.f, 800);

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
			break;

		case STATE_DAMAGEFROM_BACK:
			SKILL_MOVE(0, 350.f, 700);

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

	// State 자동 변경
	if (m_fAniTime >= m_lfAniEnd)
	{
		switch (m_eCurState)
		{
		case Engine::STATE_DEAD:
			break;

		case Engine::STATE_DOWNIDLE_FRONT:
			break;
			
		case Engine::STATE_DOWNIDLE_BACK:
			break;

		case Engine::STATE_DAMAGEFROM_FRONT:
			m_iAniIndex = STATE_DOWNIDLE_FRONT;
			break;

		case Engine::STATE_DAMAGEFROM_BACK:
			m_iAniIndex = STATE_DOWNIDLE_BACK;
			break;

		default:
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

		switch (m_iAniIndex)
		{
		case STATE_DOUBLE_CRECSENT:
			HITBOX_CONTROLL(0.35f, 0.7f, TRUE);
			break;

		case STATE_DASHATK:
			HITBOX_CONTROLL(0.1f, 0.65f, FALSE);
			break;

		case STATE_FURY2:
			HITBOX_CONTROLL(0.45f, 0.8f, TRUE);
			break;

		case STATE_FURY:
			HITBOX_CONTROLL(0.45f, 0.8f, TRUE);
			break;

		case STATE_SMASH3:
			HITBOX_CONTROLL(0.25f, 0.55f, TRUE);
			break;

		case STATE_SMASH4:
			HITBOX_CONTROLL(0.55f, 0.8f, TRUE);
			break;

		case STATE_SMASH2_B:
			HITBOX_CONTROLL(0.45f, 0.7f, TRUE);
			break;

		case STATE_SMASH1:
			HITBOX_CONTROLL(0.4f, 0.6f, TRUE);
			break;

		case STATE_ATK4:
			HITBOX_CONTROLL(0.3f, 0.7f, FALSE);
			break;

		case STATE_ATK3:
			HITBOX_CONTROLL(0.3f, 0.6f, FALSE);
			break;

		case STATE_ATK2:
			HITBOX_CONTROLL(0.25f, 0.6f, FALSE);
			break;

		case STATE_ATK1:
			HITBOX_CONTROLL(0.25f, 0.5f, FALSE);
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

	switch (m_eSceneID)
	{
	case Engine::SCENE_STAGE:
		// Player 공격
		if (m_bCanHit)
		{
			mapBossBoxCol = static_cast<CAhglan*>(Engine::Get_GameObject(L"GameLogic", L"Ahglan"))->Get_MapBoxCollider();

			for (_uint i = 0; i < m_mapBoxColliderCom.size(); ++i)
			{
				iter_BossDamaged = mapBossBoxCol.begin();

				for (_uint j = 0; j < mapBossBoxCol.size(); ++j)
				{
					iter_PlayerHit->second->Set_RenderColType(COL_FALSE);
					iter_BossDamaged->second->Set_RenderColType(COL_FALSE);

					if (iter_PlayerHit->second->Get_CanCollision() &&
						iter_BossDamaged->second->Get_CanCollision())
					{
						if (m_pCalculatorCom->Collision_OBB(&iter_PlayerHit->second->Get_Min(), &iter_PlayerHit->second->Get_Max(), iter_PlayerHit->second->Get_ColliderWorld(),
															&iter_BossDamaged->second->Get_Min(), &iter_BossDamaged->second->Get_Max(), iter_BossDamaged->second->Get_ColliderWorld()))
						{
							switch (m_eCurState)
							{
							case Engine::STATE_DOUBLE_CRECSENT:
								STOP_MOTION(100);
								break;

							case Engine::STATE_SMASH4:
								STOP_MOTION(100);
								break;

							case Engine::STATE_SMASH1:
								STOP_MOTION(75);
								break;
							}

							iter_PlayerHit->second->Set_RenderColType(COL_TRUE);
							iter_BossDamaged->second->Set_RenderColType(COL_TRUE);

							m_bCanHit = false;

							break;
						}
					}

					++iter_BossDamaged;
				}

				++iter_PlayerHit;
			}
		}

		// Monster 공격

		mapBossSphereCol = static_cast<CAhglan*>(Engine::Get_GameObject(L"GameLogic", L"Ahglan"))->Get_MapCollider();

		for (_uint i = 0; i < m_mapColliderCom.size(); ++i)
		{
			iter_BossHit = mapBossSphereCol.begin();

			for (_uint j = 0; j < mapBossSphereCol.size(); ++j)
			{
				iter_PlayerDamaged->second->Set_RenderColType(COL_FALSE);
				iter_BossHit->second->Set_RenderColType(COL_FALSE);

				if (iter_PlayerDamaged->second->Get_CanCollision() &&
					iter_BossHit->second->Get_CanCollision())
				{
					if (m_pCalculatorCom->Collision_Sphere(iter_PlayerDamaged->second->Get_Center(), iter_PlayerDamaged->second->Get_Radius() * SCALE_PLAYER,
						iter_BossHit->second->Get_Center(), iter_BossHit->second->Get_Radius() * SCALE_AHGLAN))
					{	
						if (80.f <= Engine::Random(0.f, 100.f))
						{
							m_eCurAction = PL_DAMAGED;
							m_iAniIndex = STATE_DAMAGE_RESIST;
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
								}
								else
								{
									m_eCurAction = PL_DAMAGED;
									m_iAniIndex = STATE_DAMAGEFROM_BACK;
								}
							}
						}

						iter_PlayerDamaged->second->Set_RenderColType(COL_TRUE);
						iter_BossHit->second->Set_RenderColType(COL_TRUE);

						break;
					}
				}

				++iter_BossHit;
			}

			++iter_PlayerDamaged;
		}
		break;

	default:
		break;
	}
}
