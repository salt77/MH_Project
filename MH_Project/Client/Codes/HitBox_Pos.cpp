#include "stdafx.h"
#include "HitBox_Pos.h"

#include "Export_Function.h"

CHitBox_Pos::CHitBox_Pos(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CHitBox_Pos::CHitBox_Pos(const CHitBox_Pos& rhs)
	: CGameObject(rhs)
{
}

CHitBox_Pos::~CHitBox_Pos(void)
{
}


HRESULT CHitBox_Pos::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);

	m_pTransformCom->Update_Component(0.f);

	return S_OK;
}

HRESULT CHitBox_Pos::LateReady_Object()
{
	CTransform*	pPlayerTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));

	if (pPlayerTrans)
	{
		_vec3	vDir = -*pPlayerTrans->Get_Info(INFO_RIGHT);
		_vec3	vMyPos = *pPlayerTrans->Get_Info(INFO_POS);

		vMyPos.y = 0.6f;
		vMyPos += vDir * 10.f;
		m_pTransformCom->Set_Pos(&vMyPos);
	}

	return S_OK;
}

_int CHitBox_Pos::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	CTransform*	pPlayerTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));

	if (pPlayerTrans)
	{
		_vec3	vDir = -*pPlayerTrans->Get_Info(INFO_RIGHT);
		_vec3	vMyPos = *pPlayerTrans->Get_Info(INFO_POS);
		
		vMyPos.y = 0.6f;
		vMyPos += vDir * 10.f;
		m_pTransformCom->Set_Pos(&vMyPos);
	}

	//Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

_int CHitBox_Pos::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	if (!m_mapColliderCom.empty())
	{
		map<const wstring, CCollider*>::iterator	iter = m_mapColliderCom.begin();

		for (; iter != m_mapColliderCom.end(); ++iter)
		{
			iter->second->Set_ColliderMatrix(m_pTransformCom->Get_WorldMatrix());
		}
	}
	if (!m_mapBoxColliderCom.empty())
	{
		map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		for (; iter != m_mapBoxColliderCom.end(); ++iter)
		{
			iter->second->Set_ColliderMatrix(m_pTransformCom->Get_WorldMatrix());
		}
	}

	return iExit;
}

void CHitBox_Pos::Render_Object(void)
{
}

CHitBox_Pos* CHitBox_Pos::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHitBox_Pos*	pInstance = new CHitBox_Pos(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CHitBox_Pos::Free(void)
{
	CGameObject::Free();
}

HRESULT CHitBox_Pos::Add_Component(void)
{
	CComponent*		pComponent = nullptr;
	
	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	return S_OK;
}

