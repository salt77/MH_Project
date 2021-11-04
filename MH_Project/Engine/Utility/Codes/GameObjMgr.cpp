#include "GameObjMgr.h"

#include "Component.h"
#include "GameObject.h"
#include "Layer.h"
#include "Scene.h"

USING(Engine)

IMPLEMENT_SINGLETON(CGameObjMgr)

CGameObjMgr::CGameObjMgr()
{
}

CGameObjMgr::~CGameObjMgr()
{
	Free();
}


CGameObject * CGameObjMgr::Get_ObjInManager(const wstring pLayerTag, const wstring pObjTag)
{
	map<const wstring, CLayer*>::iterator	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second->Get_GameObject(pLayerTag, pObjTag);
}

CComponent * CGameObjMgr::Get_ComponentInManager(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID)
{
	auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second->Get_Component(pObjTag, pComponentTag, eID);
}


HRESULT CGameObjMgr::Ready_Manager(void)
{
	return S_OK;
}

HRESULT CGameObjMgr::LateReady_Manager()
{
	return S_OK;
}

_int CGameObjMgr::Update_Manager(const _float & fTimeDelta)
{
	_int iResult = 0;

	for (auto& iter : m_mapLayer)
	{
		iResult = iter.second->Update_Layer(fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}

	return iResult;
}

_int CGameObjMgr::LateUpdate_Manager(const _float & fTimeDelta)
{
	_int iResult = 0;

	for (auto& iter : m_mapLayer)
	{
		iResult = iter.second->LateUpdate_Layer(fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}

	return iResult;
}

HRESULT CGameObjMgr::DeleteObjInManager(const wstring LayerTag)
{
	map<const wstring, CLayer*>::iterator	iter = m_mapLayer.begin();

	for (; iter != m_mapLayer.end(); )
	{
		if (iter->first == LayerTag)
		{
			Safe_Release(iter->second);
			m_mapLayer.erase(LayerTag);
		}
		else
			++iter;
	}

	return S_OK;
}


void CGameObjMgr::Free()
{
	for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
	m_mapLayer.clear();
}
