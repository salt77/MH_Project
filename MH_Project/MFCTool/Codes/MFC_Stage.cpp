#include "stdafx.h"
#include "MFC_Stage.h"

#include "Export_Function.h"

CMFC_Stage::CMFC_Stage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMFC_Stage::CMFC_Stage(const CMFC_Stage& rhs)
	: CGameObject(rhs)
{
}

CMFC_Stage::~CMFC_Stage(void)
{
}


HRESULT CMFC_Stage::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);
	m_pTransformCom->Set_Scale(0.025f, 0.02f, 0.02f);

	return S_OK;
}

_int CMFC_Stage::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	_vec3	vPos;
	m_pTransformCom->Get_INFO(INFO_POS, &vPos);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CMFC_Stage::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	FAILED_CHECK_RETURN(SetUp_Material(), );

	m_pMeshCom->Render_Meshes();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CMFC_Stage* CMFC_Stage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMFC_Stage*	pInstance = new CMFC_Stage(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMFC_Stage::Free(void)
{
	CGameObject::Free();
}

HRESULT CMFC_Stage::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CStaticMesh*>(Clone_Prototype(L"Proto_StaticMesh_Stage"));
	NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}

HRESULT CMFC_Stage::SetUp_Material()
{
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

