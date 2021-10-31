#include "stdafx.h"
#include "MainApp.h"
#include "Logo.h"


CMainApp::CMainApp(void)
{

}

CMainApp::~CMainApp(void)
{

}

HRESULT CMainApp::Ready_MainApp(void)
{
	FAILED_CHECK_RETURN(SetUp_DefaultSetting(&m_pGraphicDev), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Scene(m_pGraphicDev, &m_pManagementClass), E_FAIL);

	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

_int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	NULL_CHECK_RETURN(m_pManagementClass, -1);

	Update_InputDev();

	m_pManagementClass->Update_Scene(fTimeDelta);

	return 0;
}

_int CMainApp::LateUpdate_MainApp(const _float & fTimeDelta)
{
	NULL_CHECK_RETURN(m_pManagementClass, -1);

	m_pManagementClass->LateUpdate_Scene(fTimeDelta);

	return 0;
}

void CMainApp::Render_MainApp(void)
{
	NULL_CHECK(m_pManagementClass);

	Render_Begin(D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f));

	m_pManagementClass->Render_Scene(m_pGraphicDev);

	Render_End();
	
	// m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);	// Z버퍼에 깊이 값을 무조건 기록은 하되, Z버퍼에 의한 자동정렬을 수행할지 말지  결정
	// m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE); // Z버퍼에 쓸지 말지 결정
}

HRESULT CMainApp::SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	FAILED_CHECK_RETURN(Ready_GraphicDev(g_hWnd, MODE_WIN, WINCX, WINCY, &m_pDeviceClass), E_FAIL);
	m_pDeviceClass->AddRef();

	*ppGraphicDev = m_pDeviceClass->GetDevice();
	(*ppGraphicDev)->AddRef();

	(*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Font 추가
	FAILED_CHECK_RETURN(Ready_Font(*ppGraphicDev, L"Font_Default", L"바탕", 15, 15, FW_NORMAL), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Font(*ppGraphicDev, L"Font_Jinji", L"궁서", 30, 30, FW_HEAVY), E_FAIL);

	// Input 추가
	FAILED_CHECK_RETURN(Ready_InputDev(g_hInst, g_hWnd), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9& pGraphicDev, CManagement** ppManagement)
{
	CScene*	pScene = nullptr;

	FAILED_CHECK_RETURN(Create_Management(ppManagement), E_FAIL);
	(*ppManagement)->AddRef();
	
	pScene = CLogo::Create(pGraphicDev);
	NULL_CHECK_RETURN(pScene, E_FAIL);

	FAILED_CHECK_RETURN((*ppManagement)->Set_Scene(pScene), E_FAIL);
	
	return S_OK;
}

CMainApp* CMainApp::Create(void)
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Ready_MainApp()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMainApp::Free(void)
{
	Safe_Release(m_pGraphicDev);

	Safe_Release(m_pManagementClass);
	Safe_Release(m_pDeviceClass);

	Release_Utility();
	Release_System();
}

