#include "stdafx.h"
#include "Stage_1.h"
#include "DamageFont.h"
#include "CollisionMgr.h"

#include "Export_Function.h"

CStage_1::CStage_1(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
	m_vecSavePoint.reserve(256);
}

CStage_1::~CStage_1(void)
{
}


HRESULT CStage_1::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);

	return S_OK;
}

HRESULT CStage_1::LateReady_Scene()
{
	m_eSceneID = SCENE_STAGE_1;

	FAILED_CHECK_RETURN(Load_ObjectInfo(), E_FAIL);
	FAILED_CHECK_RETURN(Load_PlayerCol(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Navimesh(), E_FAIL);

	CCollisionMgr::GetInstance()->Ready_CollisionMgr();

	return S_OK;
}

Engine::_int CStage_1::Update_Scene(const _float& fTimeDelta)
{
	m_fTime += fTimeDelta;

	_int iExit = CScene::Update_Scene(fTimeDelta);

	CCollisionMgr::GetInstance()->Update_CollisionMgr();

	return iExit;
}

_int CStage_1::LateUpdate_Scene(const _float & fTimeDelta)
{
	_int iExit = CScene::LateUpdate_Scene(fTimeDelta);

	return iExit;
}

void CStage_1::Render_Scene(void)
{
	// DEBUG 용
	m_dwRenderCnt++;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS : %d", m_dwRenderCnt);
		m_dwRenderCnt = 0;
		m_fTime = 0.f;
	}

	Render_Font(L"Font_DFP", m_szFPS, &_vec2(850.f, 10.f), D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
}

HRESULT CStage_1::Ready_Layer_Environment(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*			pGameObject = nullptr;

	// DynamicCamera
	pGameObject = CDynamicCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 1.5f, 13.f),
		&_vec3(0.f, 0.f, 1.f),
		&_vec3(0.f, 1.f, 0.f),
		D3DXToRadian(60.f), (_float)WINCX / WINCY,
		0.1f, 1000.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

	//// SkyBox
	//pGameObject = CSkyBox::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_Layer_GameLogic(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*			pGameObject = nullptr;

	// Terrain 추가
	pGameObject = CTerrain::Create(m_pGraphicDev, 0);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	// StageMesh
	pGameObject = CStageMesh_1::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"StageMesh", pGameObject), E_FAIL);

	// Player
	pGameObject = CPlayer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pGameObject), E_FAIL);

	// Dummy
	pGameObject = CHitBox_Pos::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"HitBox_Pos", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_Layer_UI(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*			pGameObject = nullptr;

	// FadeInOut
	pGameObject = CFadeInOut::Create(m_pGraphicDev, 0.f, 0.f, WINCX * 2.f, WINCY * 2.f, 0.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"FadeInOut_UI", pGameObject), E_FAIL);

	// Damage Font UI (풀링 사용 => 미리 생성 (종류별로 30개씩 생성 -> 최대 4자릿수 데미지))
	for (_uint i = 0; i < DAMAGEFONT_COUNT; ++i)
	{
		wstring wstrNormalFont = L"DamageFont_Normal_UI_";
		
		wstrNormalFont += to_wstring(i);
		
		pGameObject = CDamageFont::Create(m_pGraphicDev, DAMAGEFONT_NORMAL);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(wstrNormalFont, pGameObject), E_FAIL);
	}
	for (_uint i = 0; i < DAMAGEFONT_SKILL_COUNT; ++i)
	{
		wstring wstrSkillFont = L"DamageFont_Skill_UI_";

		wstrSkillFont += to_wstring(i);

		pGameObject = CDamageFont::Create(m_pGraphicDev, DAMAGEFONT_SKILL);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(wstrSkillFont, pGameObject), E_FAIL);
	}
	for (_uint i = 0; i < DAMAGEFONT_MONSTER_COUNT; ++i)
	{
		wstring wstrMonsterFont = L"DamageFont_Monster_UI_";

		wstrMonsterFont += to_wstring(i);

		pGameObject = CDamageFont::Create(m_pGraphicDev, DAMAGEFONT_MONSTER);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(wstrMonsterFont, pGameObject), E_FAIL);
	}

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_Prototype(void)
{
	return S_OK;
}

HRESULT CStage_1::Ready_LightInfo(void)
{
	D3DLIGHT9			tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	tLightInfo.Direction = _vec3(-1.f, -1.f, -1.f);

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

	return S_OK;
}

HRESULT CStage_1::Load_ObjectInfo()
{
	HANDLE hFile = CreateFile(L"../../Data/Stage_1_Player.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwbyte = 0;

	_uint iSceneType = -1;
	_uint iObjCount = -1;

	_vec3	vPos, vScale, vRotate;

	ReadFile(hFile, &iSceneType, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iObjCount, sizeof(_uint), &dwbyte, nullptr);

	ReadFile(hFile, &vPos, sizeof(_vec3), &dwbyte, nullptr);
	ReadFile(hFile, &vScale, sizeof(_vec3), &dwbyte, nullptr);
	ReadFile(hFile, &vRotate, sizeof(_vec3), &dwbyte, nullptr);

	CTransform* pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));

	pPlayerTrans->Set_Pos(&vPos);
	//pPlayerTrans->Set_Scale(vScale.x, vScale.y, vScale.z);
	pPlayerTrans->RotationFromOriginAngle(ROT_X, vRotate.x);
	pPlayerTrans->RotationFromOriginAngle(ROT_Y, vRotate.y);
	pPlayerTrans->RotationFromOriginAngle(ROT_Z, vRotate.z);

	for (_uint i = 0; i < iObjCount; ++i)
	{
		if (0 == dwbyte)
			return E_FAIL;

		ReadFile(hFile, &vPos, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vScale, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vRotate, sizeof(_vec3), &dwbyte, nullptr);

		CTransform* pTrans = nullptr;

		switch (iSceneType)
		{
		case 0:
			break;

		case 1:
			break;

		case 2:
			break;
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CStage_1::Load_PlayerCol()
{
	HANDLE hFile = CreateFile(L"../../Data/Lethita_HitBox.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

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

		// Bone 찾기
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
		CDynamicMesh* pMeshCom = dynamic_cast<CDynamicMesh*>(pPlayer->Get_Component(L"Com_Mesh", ID_STATIC));

		pPlayer->Add_Collider(fRadius, pNameBuff, dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_WorldMatrix(), eColliderType);

		list<D3DXMESHCONTAINER_DERIVED*>			listTemp = pMeshCom->Get_MeshContainerList();
		list<D3DXMESHCONTAINER_DERIVED*>::iterator	iterList = listTemp.begin();

		for (; iterList != listTemp.end(); ++iterList)
		{
			for (_uint i = 0; i < (*iterList)->dwNumBones; ++i)
			{
				_tchar	pTemp[64] = L"";
				MultiByteToWideChar(CP_ACP, 0, (*iterList)->pSkinInfo->GetBoneName(i), strlen((*iterList)->pSkinInfo->GetBoneName(i)), pTemp, 64);

				if (/*(*iterList)->pSkinInfo->GetBoneName(i)*/!wcscmp(pTemp, pNameBuff2))
				{
					dynamic_cast<CCollider*>(Engine::Get_Component(L"GameLogic", L"Player", wstrColName, ID_STATIC))->Set_BoneName(wstrBoneName);
					dynamic_cast<CCollider*>(pPlayer->Get_Component((wstring)wstrColName, ID_STATIC))->Set_Matrix((*iterList)->ppCombinedTransformMatrix[i]);

					break;
				}
			}
		}

		Safe_Delete(pNameBuff);
		Safe_Delete(pNameBuff2);

	}

	CloseHandle(hFile);

	hFile = CreateFile(L"../../Data/Lethita_WeaponHitBox.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	ReadFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iBoxSize, sizeof(_uint), &dwbyte, nullptr);

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

		// Bone 찾기
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
		CDynamicMesh* pMeshCom = dynamic_cast<CDynamicMesh*>(pPlayer->Get_Component(L"Com_Mesh", ID_STATIC));

		pPlayer->Add_Collider(vMin.x, vMin.y, vMin.z, vMax.x, vMax.y, vMax.z, pNameBuff, 
							  dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_WorldMatrix(), eColliderType);

		// Bone 찾기
		list<D3DXMESHCONTAINER_DERIVED*>			listTemp = pMeshCom->Get_MeshContainerList();
		list<D3DXMESHCONTAINER_DERIVED*>::iterator	iterList = listTemp.begin();

		for (; iterList != listTemp.end(); ++iterList)
		{
			for (_uint i = 0; i < (*iterList)->dwNumBones; ++i)
			{
				_tchar	pTemp[64] = L"";
				MultiByteToWideChar(CP_ACP, 0, (*iterList)->pSkinInfo->GetBoneName(i), strlen((*iterList)->pSkinInfo->GetBoneName(i)), pTemp, 64);

				if (!wcscmp(pTemp, pNameBuff2))
				{
					dynamic_cast<CBoxCollider*>(Engine::Get_Component(L"GameLogic", L"Player", wstrColName, ID_STATIC))->Set_BoneName(wstrBoneName);
					dynamic_cast<CBoxCollider*>(pPlayer->Get_Component((wstring)wstrColName, ID_STATIC))->Set_Matrix((*iterList)->ppCombinedTransformMatrix[i]);

					break;
				}
			}
		}

		Safe_Delete(pNameBuff);
		Safe_Delete(pNameBuff2);
	}

	CloseHandle(hFile);

	// 검에 붙어있지 않은 히트박스 임의 생성
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	CTransform*	pHitBox_Pos = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"HitBox_Pos", L"Com_Transform", ID_DYNAMIC));

	pPlayer->Add_Collider(-30.f, -60.f, -30.f, 30.f, 60.f, 30.f, L"Other_Attack",
						  pHitBox_Pos->Get_WorldMatrix(), COLTYPE_BOX_OTHER);

	return S_OK;
}

HRESULT CStage_1::Load_Navimesh()
{
	HANDLE hFile = CreateFile(L"../../Data/Stage_1_Navi20.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwbyte = 0;
	_uint iSize = 0;
	_matrix matPoint;
	ZeroMemory(matPoint, sizeof(_matrix));

	ReadFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);

	for (_uint i = 0; i < iSize; ++i)
	{
		if (0 == dwbyte)
			break;

		ReadFile(hFile, &matPoint, sizeof(_matrix), &dwbyte, nullptr);

		m_vecSavePoint.push_back(matPoint);
	}

	CloseHandle(hFile);

	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_NaviMesh", CNaviMesh::Create(m_pGraphicDev, m_vecSavePoint.size(), m_vecSavePoint)), E_FAIL);

	CPlayer*	pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	if (pPlayer)
		pPlayer->Add_NaviMesh();

	return S_OK;
}

CStage_1* CStage_1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage_1*	pInstance = new CStage_1(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Safe_Release(pInstance);

	return pInstance;
}

void CStage_1::Free(void)
{
	CCollisionMgr::DestroyInstance();

	CScene::Free();
}
