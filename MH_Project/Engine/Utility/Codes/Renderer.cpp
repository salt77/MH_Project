#include "Renderer.h"
#include "Export_Utility.h"

USING(Engine)
IMPLEMENT_SINGLETON(CRenderer)

Engine::CRenderer::CRenderer(void)
{
}

Engine::CRenderer::~CRenderer(void)
{
	Free();
}


void Engine::CRenderer::Add_RenderGroup(RENDERID eID, CGameObject* pGameObject)
{
	if (RENDER_END <= eID)
		return;

	m_RenderGroup[eID].push_back(pGameObject);
	pGameObject->AddRef();
}

void Engine::CRenderer::Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& iter : m_RenderGroup[RENDER_PRIORITY])
		iter->Render_Object();
}

void Engine::CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& iter : m_RenderGroup[RENDER_NONALPHA])
		iter->Render_Object();
}

_bool		Compare_Z(CGameObject* pDest, CGameObject* pSrc)
{
	return pDest->Get_ViewZ() > pSrc->Get_ViewZ();
}

void Engine::CRenderer::Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_RenderGroup[RENDER_ALPHA].sort(Compare_Z);

	for (auto& iter : m_RenderGroup[RENDER_ALPHA])
		iter->Render_Object();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void Engine::CRenderer::Render_UI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& iter : m_RenderGroup[RENDER_UI])
		iter->Render_Object();
}

void CRenderer::Render_Deferred(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	Begin_MRT(L"MRT_Deferred");

	Render_NonAlpha(pGraphicDev);

	End_MRT(L"MRT_Deferred");
}

void CRenderer::Render_LightAcc(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	Begin_MRT(L"MRT_LightAcc");

	CShader*	pShader = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Shade"));
	NULL_CHECK(pShader);

	LPD3DXEFFECT	pEffect = pShader->Get_EffectHandle();
	pEffect->AddRef();

	Get_RenderTargetTexture(pEffect, L"Target_Normal", "g_NormalTexture");
	Get_RenderTargetTexture(pEffect, L"Target_Depth", "g_DepthTexture");

	pEffect->Begin(NULL, 0);

	Render_Light(pEffect);

	pEffect->End();

	End_MRT(L"MRT_LightAcc");

	Safe_Release(pEffect);
	Safe_Release(pShader);
}

void CRenderer::Render_Blend(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	CShader*	pShader = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Blend"));
	NULL_CHECK(pShader);

	LPD3DXEFFECT	pEffect = pShader->Get_EffectHandle();
	pEffect->AddRef();

	Engine::Get_RenderTargetTexture(pEffect, L"Target_Albedo", "g_AlbedoTexture");
	Engine::Get_RenderTargetTexture(pEffect, L"Target_Shade", "g_ShadeTexture");
	Engine::Get_RenderTargetTexture(pEffect, L"Target_Specular", "g_SpecularTexture");

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	pGraphicDev->SetFVF(FVF_SCREEN);
	pGraphicDev->SetIndices(m_pIB);
	pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
	Safe_Release(pShader);
}

HRESULT CRenderer::Ready_Renderer(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	if (FAILED(pGraphicDev->CreateVertexBuffer(sizeof(VTXSCREEN) * 4,
		0, // 정적 버퍼(사이즈를 정해준 만큼 할당해서 사용한다는 뜻) - D3DUSAGE_DYNAMIC(동적 버퍼 : 파티클)
		FVF_SCREEN,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL)))
		return E_FAIL;

	if (FAILED(pGraphicDev->CreateIndexBuffer(sizeof(INDEX16) * 2,
		0, // 정적 버퍼(사이즈를 정해준 만큼 할당해서 사용한다는 뜻) - D3DUSAGE_DYNAMIC(동적 버퍼 : 파티클)
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL)))
		return E_FAIL;

	D3DVIEWPORT9		ViewPort;
	pGraphicDev->GetViewport(&ViewPort);

	VTXSCREEN*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = _vec4(0.f, 0.f, 0.f, 1.f);
	pVertex[0].vTexUV = _vec2(0.f, 0.f);

	pVertex[1].vPosition = _vec4((_float)ViewPort.Width, 0.f, 0.f, 1.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);

	pVertex[2].vPosition = _vec4((_float)ViewPort.Width, (_float)ViewPort.Height, 0.f, 1.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);

	pVertex[3].vPosition = _vec4(0.f, (_float)ViewPort.Height, 0.f, 1.f);
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


	return S_OK;
}

void Engine::CRenderer::Clear_RenderGroup(void)
{
	for (_ulong i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}
}

void Engine::CRenderer::Free(void)
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	Clear_RenderGroup();
}

void Engine::CRenderer::Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	Render_Priority(pGraphicDev);

	// 수업이기 때문에 논알파에 해당하는 오브젝트들만 디퍼드를 적용하여 그리고 있음
	Render_Deferred(pGraphicDev);
	Render_LightAcc(pGraphicDev);

	Render_Blend(pGraphicDev);

	Render_Alpha(pGraphicDev);
	Render_UI(pGraphicDev);

	Engine::Render_DebugBuffer(L"MRT_Deferred");
	Engine::Render_DebugBuffer(L"MRT_LightAcc");

	Clear_RenderGroup();
}

