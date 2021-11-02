#include "stdafx.h"
#include "Loading.h"

#include "Export_Function.h"
#include "DynamicMesh.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
{
	m_pGraphicDev->AddRef();
	ZeroMemory(m_szLoading, sizeof(_tchar) * 256);
}

CLoading::~CLoading(void)
{
}


unsigned int CALLBACK CLoading::Thread_Main(void* pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;
	
	_uint		iFlag = 0;

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;

	case LOADING_BOSS:
		break;
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	//_endthreadex(0);

	return iFlag;
}

HRESULT CLoading::Ready_Loading(LOADINGID eLoading)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

	m_eLoading = eLoading;
	
	return 0;
}

Engine::_uint CLoading::Loading_ForStage(void)
{
	lstrcpy(m_szLoading, L"Loading Buffer...............");
	
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_TerrainTex", CTerrainTex::Create(m_pGraphicDev, 64, 48)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_TerrainTex2", CTerrainTex::Create(m_pGraphicDev, 96, 48, 1, 1)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_TerrainTex3", CTerrainTex::Create(m_pGraphicDev, 96, 48, 1, 2)), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_CubeTex", CCubeTex::Create(m_pGraphicDev)), E_FAIL);
		
	lstrcpy(m_szLoading, L"Loading Texture...............");

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Terrain", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Ortel_%d.tga", TEX_NORMAL, 2)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Terrain2", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Ortel2_%d.tga", TEX_NORMAL, 2)), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_SkyBox", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SkyBox/SkyBox%d.dds", TEX_CUBE, 2)), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Component...............");
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Calculator", CCalculator::Create(m_pGraphicDev)), E_FAIL);

	//FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_NaviMesh", CNaviMesh::Create(m_pGraphicDev)), E_FAIL);
	// FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Collider", CCollider::Create(m_pGraphicDev, nullptr, 0, 0)), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Mesh...............");

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Stage", CStaticMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/StaticMesh/Stage/", L"Stage.X")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Player", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Lethita/", L"Lethita.X")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Ahglan", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Ahglan/", L"Ahglan.X")), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Complete!!!!");

	m_bFinish = true;

	return 0;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
	CLoading*	pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eID)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLoading::Free(void)
{
	Safe_Release(m_pGraphicDev);
	
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);
}

