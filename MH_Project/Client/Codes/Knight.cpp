#include "stdafx.h"
#include "Knight.h"

#include "Player.h"

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

	m_pTransformCom->Set_Scale(SCALE_MANKIND * 1.5f, SCALE_MANKIND * 1.5f, SCALE_MANKIND * 1.5f);

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

	// ����� ��
	//CTransform*	vPlayerPos = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
	//dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Knight", L"Com_Transform", ID_DYNAMIC))->Set_Pos(vPlayerPos->Get_Info(INFO_POS));

	Load_ColInfo();
	Add_NaviMesh();

	return S_OK;
}

_int CKnight::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	if (m_bDead)
		return iExit;

	//// ����׿�
	//if (Key_Down('G'))
	//{
	//	Set_Damage(15000);
	//}
	//else if (Key_Down('H'))
	//{
	//	m_iAniIndex = SPAWN;

	//	Animation_Control();
	//}

	m_fTimeDelta = fTimeDelta;

	Movement();
	Animation_Control();
	Collision_Control();
	MoveOn_Skill();

	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);

	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

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

void CKnight::Render_Object(void)
{
	if (m_vPlayerPos)
	{
		if (10.f > D3DXVec3Length(&(m_vPlayerPos - *m_pTransformCom->Get_Info(INFO_POS))))
		{
			if (m_bAnimation)
				m_pMeshCom->Play_Animation(m_fTimeDelta);

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

			pEffect->Begin(&iMaxPass, NULL);		// 1���� : ���� ���̴� ������ ��ȯ�ϴ� pass�� �ִ� ����
													// 2���� : �����ϴ� ����� ���� FLAG
			pEffect->BeginPass(0);

			m_pMeshCom->Render_Meshes(pEffect, m_mapActiveParts);

			pEffect->EndPass();
			pEffect->End();

			Safe_Release(pEffect);
		}
	}
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

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Prototype(L"Proto_Calculator"));
	NULL_CHECK_RETURN(m_pCalculatorCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Calculator", pComponent);

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

void CKnight::Contact()
{
}

void CKnight::Movement()
{
	m_pPlayer = static_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));

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

			// �̰͵� ���� �ݴ��ΰ�? . Look == Back, Right == Left;
			if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&_vec3(), &vPlayerDir, &m_vDir)) > 0.f)
				m_bTargetIsRight = false;
			else if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&_vec3(), &vPlayerDir, &m_vDir)) < 0.f)
				m_bTargetIsRight = true;


			if ((m_bCanAction && m_pPlayerTrans) || !m_bAnimation) // m_bAnimation�� ������
			{
				if (DIS_FACETOFACE >= m_fDistance)
				{
					m_iAniIndex = KNIGHT_ATTACK;
				}
				else
				{
					/*if (50.f <= m_fAngle)
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
					else */
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

void CKnight::MoveOn_Skill()
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
	m_fAniTime = m_pMeshCom->Get_AniFrameTime();
	//m_lfAniEnd = m_pMeshCom->Get_AniFrameEndTime();

	// ���� ���� �� �ѹ��� ����
	m_eCurState = (KNIGHT_STATE)m_iAniIndex;
	if (m_eCurState != m_ePreState)
	{
		m_bAnimation = true;

		m_fAniTime = 0.f;
		m_lfAniEnd = m_pMeshCom->Get_AniFrameEndTime();

		// Sound�� ���� �ٽ� false;
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

		case KNIGHT_ATTACK:
			m_bCanAction = false;

			m_lfAniEnd = 4.3f;
			ENEMY_SKILL_MOVE((float)m_lfAniEnd * 0.38f, 4.f, (_float)m_lfAniEnd * 0.48f);
			ENEMY_SKILL_MOVE2((float)m_lfAniEnd * 0.58f, 4.f, (_float)m_lfAniEnd * 0.66f);
			break;

		case KNIGHT_DAMAGED:
			m_bCanAction = false;
			break;

		case KNIGHT_DAMAGED2:
			m_bCanAction = false;
			break;

			//case KNIGHT_DEAD_BEGIN:
			//	m_eSolAction = SOL_DEAD;
			//	break;

			//case KNIGHT_DEAD_DURING:
			//	m_eSolAction = SOL_DEAD;
			//	break;

			//case KNIGHT_DEAD_END:
			//	m_eSolAction = SOL_DEAD;
			//	break;
		}

		m_ePreState = m_eCurState;
	}


	// ���� ���� �� �� ������ ����
	switch (m_eCurState)
	{
	case KNIGHT_STAPFRONT:
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

		if (DIS_FACETOFACE >= m_fDistance)
		{
			// ������ ���� �ൿ�� ATK�� �����ϰ� �Ѵ�. 
			m_iAniIndex = KNIGHT_ATTACK;
		}
		break;

		//case KNIGHT_TURNRIGHT:
		//	if (5.f <= m_fAngle)
		//	{
		//		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * m_fTimeDelta));
		//	}
		//	else
		//	{
		//		m_iAniIndex = KNIGHT_RUN;
		//		Animation_Control();
		//	}
		//	break;

		//case KNIGHT_TURNLEFT:
		//	if (5.f <= m_fAngle)
		//	{
		//		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * -m_fTimeDelta));
		//	}
		//	else
		//	{
		//		m_iAniIndex = KNIGHT_RUN;
		//		Animation_Control();
		//	}

	case KNIGHT_IDLE:
		//if (DIS_FACETOFACE > m_fDistance &&
		//	0.f < m_pPlayer->Get_TagPlayerInfo().tagInfo.iHp)
		//{
		//	// ������ ���� �ൿ�� ATK�� �����ϰ� �Ѵ�. 
		//	m_eSolAction = SOL_ATK;
		//}
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

		if (0 >= m_pPlayer->Get_TagPlayerInfo().tagInfo.iHp)
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
			HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.3f, m_lfAniEnd * 0.68f);
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

		// Bone ã��
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