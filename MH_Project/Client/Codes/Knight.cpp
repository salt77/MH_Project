#include "stdafx.h"
#include "Knight.h"

#include "Player.h"
#include "Trail_Sword.h"

#include "Export_Function.h"
#include "Export_Utility.h"

USING(Engine)

CKnight::CKnight(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CKnight::~CKnight()
{
}


HRESULT CKnight::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(SCALE_MANKIND * 1.65f, SCALE_MANKIND * 1.6f, SCALE_MANKIND * 1.65f);

	m_pTransformCom->Update_Component(0.f);

	m_tInfo.iHp = 50000;
	m_tInfo.iMaxHp = m_tInfo.iHp;

	m_bBoss = false;
	m_bArmorOn = true;

	return S_OK;
}

HRESULT CKnight::LateReady_Object()
{
	CGameObject::LateReady_Object();

	// 디버그 용
	//CTransform*	vPlayerPos = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
	//dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Knight", L"Com_Transform", ID_DYNAMIC))->Set_Pos(vPlayerPos->Get_Info(INFO_POS));

	Load_ColInfo();
	Add_NaviMesh();

	m_pPlayer = static_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	//m_pTrailSword = static_cast<CTrail_Sword*>(Engine::Get_GameObject(L"GameLogic", L"Knight_Axe_Trail"));

	return S_OK;
}

_int CKnight::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	if (m_bDead)
		return iExit;

	m_fTimeDelta = fTimeDelta;

	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		Movement();
		Animation_Control();
		Collision_Control();
		MoveOn_Skill();
		Dissolve(fTimeDelta);

		m_pMeshCom->Set_AnimationIndex(m_iAniIndex);

		Engine::Add_RenderGroup(RENDER_ALPHA, this);
	}

	return iExit;
}

_int CKnight::LateUpdate_Object(const _float & fTimeDelta)
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

void CKnight::Render_Object(void)
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		m_pMeshCom->Play_Animation(m_fTimeDelta);

		//if (!m_mapColliderCom.empty())
		//{
		//	map<const wstring, CCollider*>::iterator	iter = m_mapColliderCom.begin();

		//	for (; iter != m_mapColliderCom.end(); ++iter)
		//	{
		//		iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
		//	}
		//}
		//if (!m_mapBoxColliderCom.empty())
		//{
		//	map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		//	for (; iter != m_mapBoxColliderCom.end(); ++iter)
		//	{
		//		iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
		//	}
		//}

		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

		LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
		pEffect->AddRef();

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		_uint iMaxPass = 0;

		pEffect->Begin(&iMaxPass, NULL);		// 1인자 : 현재 쉐이더 파일이 반환하는 pass의 최대 개수
												// 2인자 : 시작하는 방식을 묻는 FLAG
		pEffect->BeginPass(1);

		m_pMeshCom->Render_Meshes(pEffect);

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}
}

void CKnight::Set_Damage(_int iDamage)
{
	if (iDamage < m_tInfo.iHp)
	{
		m_tInfo.iHp -= iDamage;
	}
	else
	{
		m_tInfo.iHp = 0;

		m_iAniIndex = KNIGHT_DYING;
		m_pMeshCom->Set_TrackSpeed(1.f);
	}
}

void CKnight::Set_Enable(_vec3 vPos, _vec3 vRotate)
{
	m_bDissolveOn = false;
	m_fDissolveValue = 0.f;

	m_pTransformCom->Set_Pos(&vPos);
	m_pTransformCom->RotationFromOriginAngle(ROT_X, vRotate.x);
	m_pTransformCom->RotationFromOriginAngle(ROT_Y, vRotate.y);
	m_pTransformCom->RotationFromOriginAngle(ROT_Z, vRotate.z);

	m_pNaviMeshCom->Set_CellIndex(Compute_InCell());
	m_pTransformCom->Rotation(ROT_Y, -90.f);

	m_tInfo.iHp = m_tInfo.iMaxHp;

	m_iAniIndex = KNIGHT_SPAWN;

	m_pMeshCom->Set_TrackSpeed(2.f);

	Animation_Control();
}

HRESULT CKnight::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CDynamicMesh*>(Engine::Clone_Prototype(L"Proto_Mesh_Knight"));
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
	m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_Ahglan"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CKnight::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

void CKnight::Movement()
{
	if (KNIGHT_DYING != m_eCurState)
	{
		if (m_pPlayer)
		{
			if (0.f >= m_pPlayer->Get_TagPlayerInfo().tagInfo.iHp)
			{
				m_iAniIndex = KNIGHT_IDLE;
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

				// 이것도 축이 반대인가? . Look == Back, Right == Left;
				if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&_vec3(), &vPlayerDir, &m_vDir)) > 0.f)
					m_bTargetIsRight = false;
				else if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&_vec3(), &vPlayerDir, &m_vDir)) < 0.f)
					m_bTargetIsRight = true;


				if ((m_bCanAction && m_pPlayerTrans) || !m_bAnimation) // m_bAnimation은 디버깅용
				{
					if (10.f <= m_fAngle)
					{
						if (m_bTargetIsRight)
						{
							m_iAniIndex = KNIGHT_TURNRIGHT;
						}
						else
						{
							m_iAniIndex = KNIGHT_TURNLEFT;
						}
					}
					else if (DIS_FACETOFACE >= m_fDistance)
					{
						m_iAniIndex = KNIGHT_ATTACK;
					}
					else
					{
						if (DIS_FACETOFACE < m_fDistance)
						{
							m_iAniIndex = KNIGHT_STAPFRONT;
						}
						else
						{
							m_iAniIndex = KNIGHT_IDLE;
						}
					}
				}
			}
		}
	}
}

void CKnight::MoveOn_Skill()
{
	if (KNIGHT_ATTACK == m_eCurState)
	{
		if (m_bSkillMove)
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

		if (m_bSkillMove2)
		{
			if (m_fSkillMoveStartTime2 <= m_fAniTime &&
				m_fSkillMoveEndTime2 >= m_fAniTime)
			{
				m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(&m_vMyPos, &m_vDir, m_fSkillMoveSpeed, m_fTimeDelta));
			}
			else if (m_fSkillMoveEndTime2 < m_fAniTime)
			{
				m_bSkillMove2 = false;
			}
		}
	}
}

void CKnight::RotateLookVector()
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

void CKnight::Animation_Control()
{
	Update_State();

	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	m_fAniTime = m_pMeshCom->Get_AniFrameTime();

	// 상태 변경 시 한번만 실행
	m_eCurState = (KNIGHT_STATE)m_iAniIndex;

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
		case KNIGHT_STAPFRONT:
			m_bCanAction = true;

			m_pMeshCom->Set_TrackSpeed(1.9f);
			break;

			//case KNIGHT_TURNRIGHT:
			//	m_eSolAction = SOL_IDLE;
			//	break;

			//case KNIGHT_TURNLEFT:
			//	m_eSolAction = SOL_IDLE;
			//	break;

		case KNIGHT_IDLE:
			m_bCanAction = true;
			break;

		case KNIGHT_SPAWN:
			m_bCanAction = false;
			m_lfAniEnd = 2.5f;

			iRandSound = rand() % 2;
			if (0 == iRandSound)
			{
				SoundMgrLowerVol(L"mankind_spawn_01.wav", CSoundMgr::MONSTER3, 0.035f);
			}
			else
			{
				SoundMgrLowerVol(L"mankind_spawn_02.wav", CSoundMgr::MONSTER3, 0.035f);
			}

			SoundMgrLowerVol(L"effect_monster_spawn.wav", CSoundMgr::ENEMY_EFFECT, 0.015f);
			break;

		case KNIGHT_ATTACK:
			m_bCanAction = false;

			m_lfAniEnd = 4.3f;
			ENEMY_SKILL_MOVE((float)m_lfAniEnd * 0.38f, 4.f, (_float)m_lfAniEnd * 0.48f);
			ENEMY_SKILL_MOVE2((float)m_lfAniEnd * 0.58f, 4.f, (_float)m_lfAniEnd * 0.66f);

			iRandSound = rand() % 3;
			if (0 == iRandSound)
			{
				SoundMgrLowerVol(L"mankind_hurt_01.wav", CSoundMgr::MONSTER3, 0.05f);
			}
			else if (1 == iRandSound)
			{
				SoundMgrLowerVol(L"mankind_hurt_02.wav", CSoundMgr::MONSTER3, 0.05f);
			}
			else
			{
				SoundMgrLowerVol(L"mankind_hurt_03.wav", CSoundMgr::MONSTER3, 0.05f);
			}
			break;

		case KNIGHT_DAMAGED:
			m_bCanAction = false;
			break;

		case KNIGHT_DAMAGED2:
			m_bCanAction = false;
			break;

		case KNIGHT_DYING:
			m_bDissolveOn = true;
			m_bCanAction = false;

			m_pMeshCom->Set_TrackSpeed(1.5f);

			iRandSound = rand() % 2;
			if (0 == iRandSound)
			{
				SoundMgrLowerVol(L"mankind_dying_01.wav", CSoundMgr::MONSTER3, 0.05f);
			}
			else
			{
				SoundMgrLowerVol(L"mankind_dying_02.wav", CSoundMgr::MONSTER3, 0.05f);
			}
			break;
		}

		m_ePreState = m_eCurState;
	}


	// 상태 변경 시 매 프레임 실행
	_vec3	vDir = -*m_pTransformCom->Get_Info(INFO_LOOK);

	switch (m_eCurState)
	{
	case KNIGHT_STAPFRONT:
		m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(&m_vMyPos, D3DXVec3Normalize(&vDir, &vDir), m_fSpeed, m_fTimeDelta));

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

		if (DIS_FACETOFACE >= m_fDistance)
		{
			// 강제로 다음 행동이 ATK을 수행하게 한다. 
			m_iAniIndex = KNIGHT_ATTACK;
		}
		break;

	case KNIGHT_TURNRIGHT:
		if (10.f <= m_fAngle)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * m_fTimeDelta));
		}
		else
		{
			m_iAniIndex = KNIGHT_STAPFRONT;
			Animation_Control();
		}
		break;

	case KNIGHT_TURNLEFT:
		if (10.f <= m_fAngle)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * -m_fTimeDelta));
		}
		else
		{
			m_iAniIndex = KNIGHT_STAPFRONT;
			Animation_Control();
		}

	case KNIGHT_IDLE:
		break;

	case KNIGHT_ATTACK:
		if (!m_bSound)
		{
			m_bSound = true;
			//SoundMgr(L"step_lv6.wav", CSoundMgr::MONSTER3);
		}
		break;
	}

	if (m_fAniTime >= m_lfAniEnd * m_fAniEndDelay)
	{
		m_bCanAction = true;

		if (m_iAniIndex == KNIGHT_STAPFRONT ||
			m_iAniIndex == KNIGHT_IDLE)
		{
			m_bAnimation = true;
		}
		else
		{
			m_bAnimation = false;
		}

		if (KNIGHT_DYING == m_eCurState)
		{
			m_bCanAction = false;
		}
		else if (0 >= m_pPlayer->Get_TagPlayerInfo().tagInfo.iHp)
		{
			m_iAniIndex = KNIGHT_IDLE;
		}
		else if (m_eCurState != KNIGHT_IDLE &&
				 m_eCurState != KNIGHT_STAPFRONT)
		{
			//RotateLookVector();

			m_fRand = Engine::Random(0.f, 100.f);

			m_iAniIndex = KNIGHT_IDLE;
			m_pMeshCom->Set_TrackSpeed(2.f);

			Animation_Control();
		}
	}
}

void CKnight::Collision_Control()
{
	map<const wstring, CBoxCollider*>::iterator	iter_Damaged = m_mapBoxColliderCom.begin();
	map<const wstring, CCollider*>::iterator	iter_Hit = m_mapColliderCom.begin();

	// HitBox
	switch (m_eCurState)
	{
	case CKnight::KNIGHT_ATTACK:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.4f, m_lfAniEnd * 0.68f);
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

void CKnight::Dissolve(const _float & fTimeDelta)
{
	if (m_bDissolveOn)
	{
		m_fDissolveValue += 0.5f * fTimeDelta;

		if (1.f <= m_fDissolveValue)
		{
			m_pTransformCom->Set_Pos(&POOLING_POS);
		}
	}
}

const _ulong & CKnight::Compute_InCell()
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

void CKnight::Update_State()
{
	if (0 >= m_tInfo.iHp)
	{
		m_iAniIndex = KNIGHT_DYING;
		m_pMeshCom->Set_TrackSpeed(1.f);
	}
}

HRESULT CKnight::Add_NaviMesh()
{
	CComponent*		pComponent = nullptr;

	pComponent = m_pNaviMeshCom = dynamic_cast<CNaviMesh*>(Engine::Clone_Prototype(L"Proto_NaviMesh"));
	NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	return S_OK;
}

HRESULT CKnight::Load_ColInfo()
{
	HANDLE hFile = CreateFile(L"../../Data/Knight_Col.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

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


CKnight * CKnight::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKnight*	pInstance = new CKnight(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CKnight::Free(void)
{
	CGameObject::Free();
}
