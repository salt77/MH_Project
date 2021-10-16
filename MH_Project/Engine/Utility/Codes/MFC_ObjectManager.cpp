#include "MFC_ObjectManager.h"
#include "Layer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CMFC_ObjectManager)

CMFC_ObjectManager::CMFC_ObjectManager()
{
}

CMFC_ObjectManager::~CMFC_ObjectManager()
{
	Free();
}


//HRESULT CMFC_ObjectManager::Ready_Manager()
//{
//	return S_OK;
//}

//_int CMFC_ObjectManager::Update_Objects(const _float & fTimeDelta)
//{
//	_int iResult = 0;
//
//	for (auto& iter : m_mapLayer)
//	{
//		iResult = iter.second->Update_Layer(fTimeDelta);
//
//		if (iResult & 0x80000000)
//			return iResult;
//	}
//
//	return iResult;
//}

CGameObject * CMFC_ObjectManager::Get_MFCGameObject(const _tchar * pLayerTag, const _tchar * pObjTag)
{
	map<const _tchar*, CLayer*>::iterator	iter = find_if(m_mapMFCLayer.begin(), m_mapMFCLayer.end(), CTag_Finder(pLayerTag));

	if (iter == m_mapMFCLayer.end())
		return nullptr;

	return iter->second->Get_GameObject(pLayerTag, pObjTag);
}

CComponent * CMFC_ObjectManager::Get_MFCComponent(const _tchar * pLayerTag, const _tchar * pObjTag, const _tchar * pComponentTag, COMPONENTID eID)
{
	map<const _tchar*, CLayer*>::iterator	iter = find_if(m_mapMFCLayer.begin(), m_mapMFCLayer.end(), CTag_Finder(pLayerTag));

	if (iter == m_mapMFCLayer.end())
		return nullptr;

	return iter->second->Get_Component(pObjTag, pComponentTag, eID);
}

HRESULT CMFC_ObjectManager::AddGameObjectInManager(const _tchar * LayerTag, CLayer * pLayer)
{
	m_mapMFCLayer.emplace(LayerTag, pLayer);

	return S_OK;
}

HRESULT CMFC_ObjectManager::DeleteGameObjectInManager(const _tchar * LayerTag)
{
	map<const _tchar*, CLayer*>::iterator	iter = m_mapMFCLayer.begin();

	for (; iter != m_mapMFCLayer.end(); )
	{
		if (iter->first == LayerTag)
		{
			Safe_Release(iter->second);
			m_mapMFCLayer.erase(LayerTag);
		}
		else
			++iter;
	}

	return S_OK;
}

void CMFC_ObjectManager::Free()
{
	for_each(m_mapMFCLayer.begin(), m_mapMFCLayer.end(), CDeleteMap());
	m_mapMFCLayer.clear();
}
