#include "stdafx.h"
#include "Wall_Collision.h"

#include "Export_Function.h"


CWall_Collision::CWall_Collision(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CWall_Collision::CWall_Collision(const CWall_Collision & rhs)
	: CGameObject(rhs)
{
}

CWall_Collision::~CWall_Collision()
{
}


HRESULT CWall_Collision::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Collider(), E_FAIL);

	m_pTransformCom->Set_Pos(&WALL_COLLIDER_POS);

	return S_OK;
}

HRESULT CWall_Collision::LateReady_Object()
{
	FAILED_CHECK_RETURN(CGameObject::LateReady_Object(), E_FAIL);

	return S_OK;
}

_int CWall_Collision::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

_int CWall_Collision::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	m_mapBoxColliderCom.begin()->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());

	return iExit;
}

void CWall_Collision::Render_Object()
{
	m_mapBoxColliderCom.begin()->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CWall_Collision::Add_Component()
{
	CComponent*		pComponent = nullptr;

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

HRESULT CWall_Collision::Ready_Collider()
{
	FAILED_CHECK_RETURN(Add_Collider(-3.f, -2.f, -0.5f, 3.f, 2.f, 0.5f, L"Wall_Collider", m_pTransformCom->Get_WorldMatrix(), COLTYPE_BOX_DAMAGED), E_FAIL);

	return S_OK;
}


CWall_Collision * CWall_Collision::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWall_Collision*	pInstance = new CWall_Collision(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CWall_Collision::Free()
{
	CGameObject::Free();
}
