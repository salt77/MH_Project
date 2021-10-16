#include "Light.h"

USING(Engine)

Engine::CLight::CLight(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CLight::~CLight(void)
{

}

HRESULT Engine::CLight::Ready_Light(const D3DLIGHT9* pLightInfo, const _uint& iIndex)
{
	m_iIndex = iIndex;

	memcpy(&m_tLightInfo, pLightInfo, sizeof(D3DLIGHT9));

	m_pGraphicDev->SetLight(iIndex, pLightInfo);
	m_pGraphicDev->LightEnable(iIndex, TRUE);

	return S_OK;
}

Engine::CLight* Engine::CLight::Create(LPDIRECT3DDEVICE9 pGraphicDev,
										const D3DLIGHT9* pLightInfo,
										const _uint& iIndex)
{
	CLight*		pInstance = new CLight(pGraphicDev);

	if (FAILED(pInstance->Ready_Light(pLightInfo, iIndex)))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CLight::Free(void)
{
	m_pGraphicDev->LightEnable(m_iIndex, FALSE);

	Safe_Release(m_pGraphicDev);
}

