#include "Scene.h"

USING(Engine)

Engine::CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CScene::~CScene(void)
{

}

HRESULT Engine::CScene::Ready_Scene(void)
{
	return S_OK;
}

HRESULT CScene::LateReady_Scene()
{
	return S_OK;
}

Engine::_int Engine::CScene::Update_Scene(const _float& fTimeDelta)
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

_int CScene::LateUpdate_Scene(const _float & fTimeDelta)
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


void Engine::CScene::Free(void)
{
	for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
	m_mapLayer.clear();

	Safe_Release(m_pGraphicDev);
}

CGameObject * CScene::Get_GameObject(const wstring pLayerTag, const wstring pObjTag)
{
	map<const wstring, CLayer*>::iterator	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second->Get_GameObject(pLayerTag, pObjTag);
}

Engine::CComponent* Engine::CScene::Get_Component(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID)
{
	auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second->Get_Component(pObjTag, pComponentTag, eID);
}

