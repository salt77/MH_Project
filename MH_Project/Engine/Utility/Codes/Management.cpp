#include "Export_Utility.h"
#include "MFC_ObjectManager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CManagement)

Engine::CManagement::CManagement(void)	
{
}

Engine::CManagement::~CManagement(void)
{
	Free();
}

HRESULT Engine::CManagement::Set_Scene(CScene* pScene)
{
	Safe_Release(m_pScene);	// 주의합시다!!!!!!!!!!!!

	Clear_RenderGroup();

	m_pScene = pScene;

	return S_OK;
}

Engine::_int Engine::CManagement::Update_Scene(const _float& fTimeDelta)
{
	if (nullptr == m_pScene)
		return -1;

	m_pScene->Update_Scene(fTimeDelta);

	return 0;
}

void Engine::CManagement::Render_Scene(LPDIRECT3DDEVICE9& pGraphicDev)
{
	Render_GameObject(pGraphicDev);

	if (nullptr == m_pScene)
		return;

	m_pScene->Render_Scene();
}

void Engine::CManagement::Free(void)
{
	CMFC_ObjectManager::DestroyInstance();

	Safe_Release(m_pScene);
}

HRESULT CManagement::AddGameObjectInManager(const _tchar * LayerTag, CLayer * pLayer)
{
	CMFC_ObjectManager::GetInstance()->AddGameObjectInManager(LayerTag, pLayer);

	return S_OK;
}

HRESULT CManagement::DeleteGameObjectInManager(const _tchar * LayerTag)
{
	CMFC_ObjectManager::GetInstance()->DeleteGameObjectInManager(LayerTag);

	return S_OK;
}

CGameObject * CManagement::Get_MFCGameObject(const _tchar * pLayerTag, const _tchar * pObjTag)
{
	return CMFC_ObjectManager::GetInstance()->Get_MFCGameObject(pLayerTag, pObjTag);
}

CGameObject * CManagement::Get_GameObject(const _tchar * pLayerTag, const _tchar * pObjTag)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_GameObject(pLayerTag, pObjTag);
}

Engine::CComponent* Engine::CManagement::Get_Component(const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_Component(pLayerTag, pObjTag, pComponentTag, eID);
}

