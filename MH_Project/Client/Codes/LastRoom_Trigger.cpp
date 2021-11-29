#include "stdafx.h"
#include "LastRoom_Trigger.h"

#include "Export_Function.h"


CLastRoom_Trigger::CLastRoom_Trigger(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CLastRoom_Trigger::CLastRoom_Trigger(const CLastRoom_Trigger & rhs)
	: CGameObject(rhs)
{
}

CLastRoom_Trigger::~CLastRoom_Trigger()
{
}


HRESULT CLastRoom_Trigger::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Collider(), E_FAIL);

	m_pTransformCom->Set_Pos(&LASTROOM_POS);

	return S_OK;
}

HRESULT CLastRoom_Trigger::LateReady_Object()
{
	FAILED_CHECK_RETURN(CGameObject::LateReady_Object(), E_FAIL);

	return S_OK;
}

_int CLastRoom_Trigger::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

_int CLastRoom_Trigger::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	if (!m_mapColliderCom.empty())
		m_mapColliderCom.begin()->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());

	return iExit;
}

void CLastRoom_Trigger::Render_Object()
{
	m_mapColliderCom.begin()->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CLastRoom_Trigger::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	return S_OK;
}

HRESULT CLastRoom_Trigger::Ready_Collider()
{
	FAILED_CHECK_RETURN(Add_Collider(3.f, L"Trigger_LastRoom_Collider", m_pTransformCom->Get_WorldMatrix(), COLTYPE_SPHERE_DAMAGED), E_FAIL);

	return S_OK;
}


CLastRoom_Trigger * CLastRoom_Trigger::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLastRoom_Trigger*	pInstance = new CLastRoom_Trigger(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CLastRoom_Trigger::Free()
{
	CGameObject::Free();
}
