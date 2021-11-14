#include "stdafx.h"
#include "MFC_Terrain.h"


CMFC_Terrain::CMFC_Terrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMFC_Terrain::CMFC_Terrain(const CMFC_Terrain& rhs)
	: CGameObject(rhs)
{
}

CMFC_Terrain::~CMFC_Terrain()
{
}


HRESULT CMFC_Terrain::Ready_Object(_uint iGrass)
{
	m_iGrass = iGrass;

	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_vec3	vPos = *m_pTransformCom->Get_Info(INFO_POS);
	vPos.y += 2.6f;
	vPos.z -= 50.f;
	m_pTransformCom->Set_Pos(&vPos);

	return S_OK;
}

_int CMFC_Terrain::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CMFC_Terrain::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture(0);

	switch (m_eRenderType)
	{
	case RENDERTYPE_MFC_SOLID:
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		break;
	case RENDERTYPE_MFC_WIREFRAME:
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		break;
	default:
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		break;
	}

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

HRESULT CMFC_Terrain::Add_Component()
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
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Clone_Prototype(L"Proto_Calculator"));
	NULL_CHECK_RETURN(m_pCalculatorCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Calculator", pComponent);

	return S_OK;
}

HRESULT CMFC_Terrain::Setup_Material()
{
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

CMFC_Terrain * CMFC_Terrain::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iGrass)
{
	CMFC_Terrain*	pInstance = new CMFC_Terrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iGrass)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMFC_Terrain::Free()
{
	CGameObject::Free();
}
