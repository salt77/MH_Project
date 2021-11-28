#include "stdafx.h"
#include "Dog.h"

#include "Player.h"

#include "Export_Function.h"
#include "Export_Utility.h"

USING(Engine)

CDog::CDog(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CDog::~CDog()
{
}


HRESULT CDog::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(SCALE_NORMAL, SCALE_NORMAL, SCALE_NORMAL);
	m_pTransformCom->Update_Component(0.f);

	m_tInfo.iHp = 13500;
	m_tInfo.iMaxHp = m_tInfo.iHp;

	m_bBoss = false;

	return S_OK;
}

HRESULT CDog::LateReady_Object()
{
	CGameObject::LateReady_Object();

	m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));

	if (m_pPlayer)
		m_pPlayerTrans = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC));

	Load_ColInfo();
	Add_NaviMesh();

	return S_OK;
}

_int CDog::Update_Object(const _float & fTimeDelta)
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

		Engine::Add_RenderGroup(RENDER_NONALPHA, this);
	}

	return iExit;
}

_int CDog::LateUpdate_Object(const _float & fTimeDelta)
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

void CDog::Render_Object(void)
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
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

void CDog::Set_Damage(_int iDamage)
{
	if (iDamage < m_tInfo.iHp)
	{
		m_tInfo.iHp -= iDamage;

		if (1000 <= iDamage)
		{
			m_iAniIndex = DOGSTATE_DOWN_BEGIN;
		}
		else if (DOGSTATE_DOWN_BEGIN != m_eCurState &&
				 DOGSTATE_DOWN_IDLE != m_eCurState)
		{
			_uint iRandom = rand() % 2;

			if (0 == iRandom)
			{
				m_iAniIndex = DOGSTATE_DAMAGED;
			}
			else
			{
				m_iAniIndex = DOGSTATE_DAMAGED2;
			}
		}
	}
	else
	{
		m_tInfo.iHp = 0;

		m_iAniIndex = DOGSTATE_DYING;
	}

	Animation_Control();
}

void CDog::Set_Enable(_vec3 vPos, _vec3 vRotate)
{
	m_bDissolveOn = false;
	m_fDissolveValue = 0.f;

	m_pTransformCom->Set_Pos(&vPos);
	m_pTransformCom->RotationFromOriginAngle(ROT_X, vRotate.x);
	m_pTransformCom->RotationFromOriginAngle(ROT_Y, vRotate.y);
	m_pTransformCom->RotationFromOriginAngle(ROT_Z, vRotate.z);

	m_pNaviMeshCom->Set_CellIndex(Compute_InCell());

	m_tInfo.iHp = m_tInfo.iMaxHp;

	m_iAniIndex = DOGSTATE_SPAWN;

	Animation_Control();
}

HRESULT CDog::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CDynamicMesh*>(Engine::Clone_Prototype(L"Proto_Mesh_Dog"));
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

HRESULT CDog::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

void CDog::Movement()
{
	if (DOGSTATE_DEAD_IDLE != m_eCurState)
	{
		if (m_pPlayer)
		{
			if (0.f >= m_pPlayer->Get_TagPlayerInfo().tagInfo.iHp)
			{
				m_iAniIndex = DOGSTATE_IDLE;
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


				if ((m_bCanAction && m_pPlayerTrans) || (!m_bAnimation)) // m_bAnimation은 디버깅용
				{
					if (15.f <= m_fAngle)
					{
						if (m_bTargetIsRight)
						{
							m_iAniIndex = DOGSTATE_TURNRIGHT;
						}
						else
						{
							m_iAniIndex = DOGSTATE_TURNLEFT;
						}
					}
					else if (DIS_SHORTEST > m_fDistance)
					{
						m_iAniIndex = DOGSTATE_THREAT;
					}
					else
					{
						if (DIS_SHORTEST < m_fDistance)
						{
							m_iAniIndex = DOGSTATE_RUN;
						}
						else
						{
							m_iAniIndex = DOGSTATE_IDLE;
						}
					}
				}
			}
		}
	}
}

void CDog::MoveOn_Skill()
{
	if (m_bSkillMove && 
		DOGSTATE_ATTACK == m_eCurState)
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

void CDog::Animation_Control()
{
	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	m_fAniTime = m_pMeshCom->Get_AniFrameTime();

	// 상태 변경 시 한번만 실행
	m_eCurState = (DOG_STATE)m_iAniIndex;

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
		case DOGSTATE_RUN:
			m_bCanAction = true;

			m_pMeshCom->Set_TrackSpeed(2.f);
			break;

		case DOGSTATE_TURNRIGHT:
			m_bCanAction = true;
			break;

		case DOGSTATE_TURNLEFT:
			m_bCanAction = true;
			break;

		case DOGSTATE_IDLE:
			m_bCanAction = true;
			break;

		case DOGSTATE_ATTACK:
			m_bCanAction = false;

			m_lfAniEnd = 1.2424f;
			ENEMY_SKILL_MOVE((float)m_lfAniEnd * 0.2f, 4.f, (_float)m_lfAniEnd * 0.6f);
			break;

		case DOGSTATE_THREAT:
			m_bCanAction = false;
			break;

		case DOGSTATE_DAMAGED:
			m_lfAniEnd = 0.8f;
			m_bCanAction = false;
			break;

		case DOGSTATE_DAMAGED2:
			m_lfAniEnd = 0.8f;
			m_bCanAction = false;
			break;

		case DOGSTATE_DOWN_BEGIN:
			m_bCanAction = false;
			m_lfAniEnd = 0.58f;
			break;

		case DOGSTATE_DOWN_IDLE:
			m_bCanAction = false;
			break;

		case DOGSTATE_DOWN_END:
			m_bCanAction = false;
			m_lfAniEnd = 1.2f;
			break;

		case DOGSTATE_DYING:
			m_bCanAction = false; 
			m_lfAniEnd = 0.6f;
			break;

		case DOGSTATE_DEAD_IDLE:
			m_bDissolveOn = true;
			m_bCanAction = false;
			break;

		case DOGSTATE_SPAWN:
			m_bCanAction = false;
			m_lfAniEnd = 2.f;
			break;
		}

		m_ePreState = m_eCurState;
	}


	// 상태 변경 시 매 프레임 실행
	_vec3	vDir = -*m_pTransformCom->Get_Info(INFO_LOOK);

	switch (m_eCurState)
	{
	case DOGSTATE_RUN:
		m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(&m_vMyPos, D3DXVec3Normalize(&vDir, &vDir), m_fSpeed, m_fTimeDelta));

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

		if (DIS_SHORTEST > m_fDistance)
		{
			// 강제로 다음 행동이 ATK을 수행하게 한다. 
			m_iAniIndex = DOGSTATE_THREAT;
		}
		break;

	case DOGSTATE_TURNRIGHT:
		if (10.f <= m_fAngle)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * m_fTimeDelta));
		}
		else
		{
			m_iAniIndex = DOGSTATE_RUN;
			Animation_Control();
		}
		break;

	case DOGSTATE_TURNLEFT:
		if (10.f <= m_fAngle)
		{
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * -m_fTimeDelta));
		}
		else
		{
			m_iAniIndex = DOGSTATE_RUN;
			Animation_Control();
		}

	case DOGSTATE_ATTACK:
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

		if (DOGSTATE_RUN == m_iAniIndex ||
			DOGSTATE_IDLE == m_iAniIndex)
		{
			m_bAnimation = true;
		}
		else
		{
			m_bAnimation = false;
		}

		if (DOGSTATE_DYING == m_eCurState)
		{
			m_iAniIndex = DOGSTATE_DEAD_IDLE;

			Animation_Control();
		}
		else if (DOGSTATE_DOWN_BEGIN == m_eCurState)
		{
			m_iAniIndex = DOGSTATE_DOWN_IDLE;

			Animation_Control();
		}
		else if (DOGSTATE_DOWN_IDLE == m_eCurState)
		{
			m_iAniIndex = DOGSTATE_DOWN_END;

			Animation_Control();
		}
		else if (DOGSTATE_THREAT == m_eCurState)
		{
			m_iAniIndex = DOGSTATE_ATTACK;

			Animation_Control();
		}
		else if (0 >= m_pPlayer->Get_TagPlayerInfo().tagInfo.iHp)
		{
			m_iAniIndex = DOGSTATE_IDLE;
		}
		else if (DOGSTATE_IDLE != m_eCurState &&
				 DOGSTATE_RUN != m_eCurState &&
				 DOGSTATE_DEAD_IDLE != m_eCurState)
		{
			m_fRand = Engine::Random(0.f, 100.f);
			m_iAniIndex = DOGSTATE_IDLE;

			m_pMeshCom->Set_TrackSpeed(2.f);

			Animation_Control();
		}
	}
}

void CDog::Collision_Control()
{
	map<const wstring, CBoxCollider*>::iterator	iter_Damaged = m_mapBoxColliderCom.begin();
	map<const wstring, CCollider*>::iterator	iter_Hit = m_mapColliderCom.begin();

	// HitBox
	switch (m_eCurState)
	{
	case CDog::DOGSTATE_ATTACK:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.1f, m_lfAniEnd * 0.6f);
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

void CDog::Dissolve(const _float & fTimeDelta)
{
	if (m_bDissolveOn)
	{
		m_fDissolveValue += 0.25f * fTimeDelta;

		if (1.f <= m_fDissolveValue)
		{
			m_pTransformCom->Set_Pos(&POOLING_POS);
			//m_pTransformCom->Update_Component(m_fTimeDelta);
		}
	}
}

const _ulong & CDog::Compute_InCell()
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

HRESULT CDog::Add_NaviMesh()
{
	CComponent*		pComponent = nullptr;

	pComponent = m_pNaviMeshCom = dynamic_cast<CNaviMesh*>(Engine::Clone_Prototype(L"Proto_NaviMesh"));
	NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	return S_OK;
}

HRESULT CDog::Load_ColInfo()
{
	HANDLE hFile = CreateFile(L"../../Data/Dog_Col.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

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


CDog * CDog::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDog*	pInstance = new CDog(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CDog::Free(void)
{
	CGameObject::Free();
}
