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

	if (FAILED(m_pGraphicDev->CreateVertexBuffer(sizeof(VTXSCREEN) * 4,
		0, // 정적 버퍼(사이즈를 정해준 만큼 할당해서 사용한다는 뜻) - D3DUSAGE_DYNAMIC(동적 버퍼 : 파티클)
		FVF_SCREEN,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL)))
		return E_FAIL;

	if (FAILED(m_pGraphicDev->CreateIndexBuffer(sizeof(INDEX16) * 2,
		0, // 정적 버퍼(사이즈를 정해준 만큼 할당해서 사용한다는 뜻) - D3DUSAGE_DYNAMIC(동적 버퍼 : 파티클)
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL)))
		return E_FAIL;

	D3DVIEWPORT9		ViewPort;
	m_pGraphicDev->GetViewport(&ViewPort);

	VTXSCREEN*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// 조명에 의해서 외곽선의 픽셀이 약간 강조되어보이기 때문에 0.5f를 빼줌. (림라이트에 써먹을 수 있을지도)
	pVertex[0].vPosition = _vec4(0.f - 0.5f, 0.f - 0.5f, 0.f, 1.f);
	pVertex[0].vTexUV = _vec2(0.f, 0.f);

	pVertex[1].vPosition = _vec4((_float)ViewPort.Width - 0.5f, 0.f - 0.5f, 0.f, 1.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);

	pVertex[2].vPosition = _vec4((_float)ViewPort.Width - 0.5f, (_float)ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);

	pVertex[3].vPosition = _vec4(0.f - 0.5f, (_float)ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);

	m_pVB->Unlock();

	INDEX16*	pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();


	/*m_pGraphicDev->SetLight(iIndex, pLightInfo);
	m_pGraphicDev->LightEnable(iIndex, TRUE);*/

	return S_OK;
}

void CLight::Render_Light(LPD3DXEFFECT & pEffect)
{
	pEffect->SetVector("g_vLightDir", &_vec4(m_tLightInfo.Direction, 0.f));

	pEffect->SetVector("g_vLightDiffuse", (_vec4*)&m_tLightInfo.Diffuse);
	pEffect->SetVector("g_vLightAmbient", (_vec4*)&m_tLightInfo.Ambient);

	_matrix	matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);

	pEffect->SetMatrix("g_matInvView", &matView);
	pEffect->SetMatrix("g_matInvProj", &matProj);

	pEffect->CommitChanges();

	pEffect->BeginPass(0);

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pGraphicDev->SetFVF(FVF_SCREEN);
	m_pGraphicDev->SetIndices(m_pIB);
	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
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
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	m_pGraphicDev->LightEnable(m_iIndex, FALSE);

	Safe_Release(m_pGraphicDev);
}

