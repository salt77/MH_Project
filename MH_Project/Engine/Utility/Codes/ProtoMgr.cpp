#include "ProtoMgr.h"
#include "Management.h"

USING(Engine)
IMPLEMENT_SINGLETON(CProtoMgr)

Engine::CProtoMgr::CProtoMgr(void)
{

}

Engine::CProtoMgr::~CProtoMgr(void)
{
	Free();
}

HRESULT Engine::CProtoMgr::Ready_Prototype(const wstring pProtoTag, CComponent* pInstance)
{
	CComponent*		pComponent = Find_Prototype(pProtoTag);

	if (nullptr != pComponent)
		return E_FAIL;

	m_mapPrototype.emplace(pProtoTag, pInstance);

	return S_OK;
}

Engine::CComponent* Engine::CProtoMgr::Clone_Prototype(const wstring pProtoTag)
{
	CComponent*		pComponent = Find_Prototype(pProtoTag);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent->Clone();
}

HRESULT CProtoMgr::Delete_Prototype(const wstring pProtoTag)
{
	CComponent*		pComponent = Find_Prototype(pProtoTag);

	if (nullptr == pComponent)
	{
		return E_FAIL;
	}
	else
	{
		Safe_Release(pComponent);
		m_mapPrototype.erase(pProtoTag);
	}

	return S_OK;
}

HRESULT CProtoMgr::Clear_Prototype_ForNextStage()
{
	map<const wstring, CComponent*>::iterator	iter = m_mapPrototype.begin();

	for (; iter != m_mapPrototype.end(); )
	{
		Safe_Release(iter->second);
		m_mapPrototype.erase(iter->first);

		iter = m_mapPrototype.begin();
	}

	CManagement::GetInstance()->Ready_Prototype_Shader(CManagement::GetInstance()->Get_CurSceneDevice());

	return S_OK;
}

Engine::CComponent* Engine::CProtoMgr::Find_Prototype(const wstring pProtoTag)
{
	auto	iter = find_if(m_mapPrototype.begin(), m_mapPrototype.end(), CTag_Finder(pProtoTag));

	if (iter == m_mapPrototype.end())
		return nullptr;

	return iter->second;
}

void Engine::CProtoMgr::Free(void)
{
	for_each(m_mapPrototype.begin(), m_mapPrototype.end(), CDeleteMap());
	m_mapPrototype.clear();
}

