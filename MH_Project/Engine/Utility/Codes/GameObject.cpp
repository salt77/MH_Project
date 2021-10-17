#include "GameObject.h"

USING(Engine)

Engine::CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CGameObject::CGameObject(const CGameObject& rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CGameObject::~CGameObject(void)
{

}


HRESULT Engine::CGameObject::Ready_Object(void)
{
	return S_OK;
}

Engine::_int Engine::CGameObject::Update_Object(const _float& fTimeDelta)
{
	_int iResult = 0;

	for (auto& iter : m_mapComponent[ID_DYNAMIC])
	{
		iResult = iter.second->Update_Component(fTimeDelta);

		if (iResult & 0x80000000)
			return -1;
	}

	return iResult;
}

CComponent * CGameObject::Find_Component(const wstring pComponentTag, COMPONENTID eID)
{
	auto	iter = find_if(m_mapComponent[eID].begin(), m_mapComponent[eID].end(), CTag_Finder(pComponentTag));

	if (iter == m_mapComponent[eID].end())
		return nullptr;

	return iter->second;
}

Engine::CComponent* Engine::CGameObject::Get_Component(const wstring pComponentTag, COMPONENTID eID)
{
	CComponent*		pComponent = Find_Component(pComponentTag, eID);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}


void Engine::CGameObject::Free(void)
{
	for (_uint i = 0; i < ID_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
		m_mapComponent[i].clear();
	}

	Safe_Release(m_pGraphicDev);
}

