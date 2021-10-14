#include "stdafx.h"
#include "MFC_Player.h"
#include "StaticMesh.h"


USING(Engine)

CMFC_Player::CMFC_Player(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
	m_vecColliderCom.reserve(8);
}

CMFC_Player::~CMFC_Player()
{
}

HRESULT CMFC_Player::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);
	m_pTransformCom->Set_Scale(0.05f, 0.05f, 0.05f);

	return S_OK;
}

_int CMFC_Player::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CMFC_Player::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	
	/*if (m_pColliderCom)
		m_pColliderCom->Render_Collider(m_pTransformCom->Get_WorldMatrix());*/
	if (!m_vecColliderCom.empty())
	{
		for (CCollider* pCol : m_vecColliderCom)
			pCol->Render_Collider(m_pTransformCom->Get_WorldMatrix());
	}

	m_pMeshCom->Render_Meshes();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMFC_Player::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	pComponent = m_pMeshCom = dynamic_cast<CStaticMesh*>(Clone_Prototype(L"Proto_StaticMesh_Player"));
	NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_StaticMesh", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	//// Calculator
	//pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Clone_Prototype(L"Proto_Calculator"));
	//NULL_CHECK_RETURN(m_pCalculatorCom, E_FAIL);
	//m_mapComponent[ID_STATIC].emplace(L"Com_Calculator", pComponent);

	//// Collider
	//pComponent = m_pColliderCom = CCollider::Create(m_pGraphicDev, m_pMeshCom->Get_VtxPos(), m_pMeshCom->Get_VtxCnt(), m_pMeshCom->Get_Stride());
	//NULL_CHECK_RETURN(m_pColliderCom, E_FAIL);
	//m_mapComponent[ID_STATIC].emplace(L"Com_Collider", pComponent);

	return S_OK;
}

HRESULT CMFC_Player::Add_Collider(_float fRadius)
{
	CComponent*		pComponent = nullptr;

	pComponent = CCollider::Create(m_pGraphicDev, fRadius);
	m_vecColliderCom.emplace_back(dynamic_cast<CCollider*>(pComponent));
	if (m_vecColliderCom.empty())
		return E_FAIL;

	CString cstrCollider, cstrColNum;
	cstrColNum.Format(_T("%d"), m_iColliderNum);
	cstrCollider = "Com_Collider";
	cstrCollider += cstrColNum;
	m_mapComponent[ID_STATIC].emplace(cstrCollider, pComponent);
	++m_iColliderNum;

	return S_OK;
}

CMFC_Player * CMFC_Player::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMFC_Player* pInstance = new CMFC_Player(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMFC_Player::Free()
{
	CGameObject::Free();
}
