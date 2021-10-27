#include "stdafx.h"
#include "Stage.h"

#include "Export_Function.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
	m_vecSavePoint.reserve(32);
}

CStage::~CStage(void)
{
}


HRESULT CStage::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);


	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);

	return S_OK;
}

HRESULT CStage::LateReady_Scene()
{
	FAILED_CHECK_RETURN(Load_PlayerCol(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Navimesh(), E_FAIL);

	return S_OK;
}

Engine::_int CStage::Update_Scene(const _float& fTimeDelta)
{
	m_fTime += fTimeDelta;

	return CScene::Update_Scene(fTimeDelta);
}

void CStage::Render_Scene(void)
{
	// DEBUG 용
	m_dwRenderCnt++;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS : %d", m_dwRenderCnt);
		m_dwRenderCnt = 0;
		m_fTime = 0.f;
	}

	Render_Font(L"Font_Jinji", m_szFPS, &_vec2(400.f, 10.f), D3DXCOLOR(0.f, 1.f, 0.f, 1.f));
}

HRESULT CStage::Ready_Layer_Environment(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*			pGameObject = nullptr;

	// DynamicCamera
	pGameObject = CDynamicCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 1.5f, -2.5f),
		&_vec3(0.f, 0.f, 1.f),
		&_vec3(0.f, 1.f, 0.f),
		D3DXToRadian(60.f), (_float)WINCX / WINCY,
		0.1f, 1000.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

	// SkyBox
	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);


	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage::Ready_Layer_GameLogic(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*			pGameObject = nullptr;

	// Terrain 추가
	pGameObject = CTerrain::Create(m_pGraphicDev, 0);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	// Terrain_Grass 추가
	pGameObject = CTerrain::Create(m_pGraphicDev, 1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain2", pGameObject), E_FAIL);
	pGameObject = CTerrain::Create(m_pGraphicDev, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain3", pGameObject), E_FAIL);

	// StageMesh
	pGameObject = CStageMesh::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"StageMesh", pGameObject), E_FAIL);

	// Player
	pGameObject = CPlayer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage::Ready_Layer_UI(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*			pGameObject = nullptr;


	m_mapLayer.emplace(pLayerTag, pLayer);
	return S_OK;
}

HRESULT CStage::Ready_Prototype(void)
{
	return S_OK;
}

HRESULT CStage::Ready_LightInfo(void)
{
	D3DLIGHT9			tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Direction = _vec3(1.f, -1.f, 0.f);

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

	return S_OK;
}

HRESULT CStage::Load_PlayerCol()
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

		pPlayer->Add_Collider(fRadius, pNameBuff, eColliderType);

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

		pPlayer->Add_Collider(vMin.x, vMin.y, vMin.z, vMax.x, vMax.y, vMax.z, pNameBuff, eColliderType);

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

	return S_OK;
}

HRESULT CStage::Load_Navimesh()
{
	HANDLE hFile = CreateFile(L"../../Data/Navi.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

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
		pPlayer->Add_NaviMesh(m_vecSavePoint.size(), m_vecSavePoint);

	return S_OK;
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage*	pInstance = new CStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Safe_Release(pInstance);

	return pInstance;
}

void CStage::Free(void)
{
	CScene::Free();
}
