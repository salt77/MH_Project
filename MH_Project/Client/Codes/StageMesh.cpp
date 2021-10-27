#include "stdafx.h"
#include "StageMesh.h"

#include "Export_Function.h"

CStageMesh::CStageMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CStageMesh::CStageMesh(const CStageMesh& rhs)
	: CGameObject(rhs)
{
}

CStageMesh::~CStageMesh(void)
{
}


HRESULT CStageMesh::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);
	m_pTransformCom->Set_Scale(0.025f, 0.02f, 0.02f);

	return S_OK;
}

_int CStageMesh::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	_vec3	vPos;
	m_pTransformCom->Get_INFO(INFO_POS, &vPos);

	//m_bDraw = m_pOptimizationCom->Isin_Frustum_ForObject(&vPos, 0.f);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CStageMesh::Render_Object(void)
{
	//if (false == m_bDraw)
	//	return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	FAILED_CHECK_RETURN(SetUp_Material(), );

	m_pMeshCom->Render_Meshes();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CStageMesh* CStageMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStageMesh*	pInstance = new CStageMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CStageMesh::Free(void)
{
	CGameObject::Free();
}

HRESULT CStageMesh::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CStaticMesh*>(Clone_Prototype(L"Proto_Mesh_Stage"));
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

	//// Optimization
	//pComponent = m_pOptimizationCom = dynamic_cast<COptimization*>(Clone_Prototype(L"Proto_Optimization"));
	//NULL_CHECK_RETURN(m_pOptimizationCom, E_FAIL);
	//m_mapComponent[ID_STATIC].emplace(L"Com_Optimization", pComponent);

	return S_OK;
}

HRESULT CStageMesh::SetUp_Material()
{
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.45f, 0.45f, 0.45f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

