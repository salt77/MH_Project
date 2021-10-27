#include "stdafx.h"
#include "Terrain.h"

#include "Export_Function.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

CTerrain::~CTerrain(void)
{
}


HRESULT CTerrain::Ready_Object(_uint iGrass)
{
	m_iGrass = iGrass;

	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CTerrain::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CTerrain::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	FAILED_CHECK_RETURN(SetUp_Material(), );

	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iGrass)
{
	CTerrain*	pInstance = new CTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iGrass)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTerrain::Free(void)
{
	CGameObject::Free();
}

HRESULT CTerrain::Add_Component(void)
{
	CComponent*		pComponent = nullptr;
	
	// buffer
	if (0 == m_iGrass)
	{
		pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(Clone_Prototype(L"Proto_Buffer_TerrainTex"));
		NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);
	}
	else if (1 == m_iGrass)
	{
		pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(Clone_Prototype(L"Proto_Buffer_TerrainTex2"));
		NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);
	}
	else if (2 == m_iGrass)
	{
		pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(Clone_Prototype(L"Proto_Buffer_TerrainTex3"));
		NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);
	}

	// texture
	if (0 == m_iGrass)
	{
		pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Terrain"));
		NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
	}
	else if (1 == m_iGrass || 2 == m_iGrass)
	{
		pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Terrain2"));
		NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
	}

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}

HRESULT CTerrain::SetUp_Material(void)
{
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.65f, 0.65f, 0.65f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

