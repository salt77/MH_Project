#include "Component.h"

USING(Engine)

Engine::CComponent::CComponent(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bClone(false)
{
	m_pGraphicDev->AddRef();
}

Engine::CComponent::CComponent(const CComponent& rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
	, m_bClone(true)
{
	m_pGraphicDev->AddRef();
}

Engine::CComponent::CComponent(void)
	: m_pGraphicDev(nullptr)
	, m_bClone(true)
{

}

Engine::CComponent::~CComponent(void)
{

}


void Engine::CComponent::Free(void)
{
	Safe_Release(m_pGraphicDev);
}

