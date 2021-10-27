#include "stdafx.h"
#include "Ahglan.h"

#include "Export_Utility.h"

USING(Engine)

CAhglan::CAhglan(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CAhglan::~CAhglan()
{
}


HRESULT CAhglan::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.07f, 0.07f, 0.07f);

	return S_OK;

	return S_OK;
}

_int CAhglan::Update_Object(const _float & fTimeDelta)
{
	return _int();
}

void CAhglan::Render_Object(void)
{
	if (!m_mapColliderCom.empty())
	{
		map<const wstring, CCollider*>::iterator	iter = m_mapColliderCom.begin();

		for (; iter != m_mapColliderCom.end(); ++iter)
		{
			if (iter->second->Get_CanCollision())
			{
				iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
			}
		}
	}
	if (!m_mapBoxColliderCom.empty())
	{
		map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		for (; iter != m_mapBoxColliderCom.end(); ++iter)
		{
			if (iter->second->Get_CanCollision())
			{
				iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
			}
		}
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pMeshCom->Render_Meshes();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CAhglan::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CDynamicMesh*>(Clone_Prototype(L"Proto_Mesh_Player"));
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

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Clone_Prototype(L"Proto_Calculator"));
	NULL_CHECK_RETURN(m_pCalculatorCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Calculator", pComponent);

	return S_OK;
}

HRESULT CAhglan::Add_Collider(_float fRadius, wstring cstrName, COLLIDERTYPE eColliderType)
{
	return S_OK;
}

HRESULT CAhglan::Add_Collider(_float vMinX, _float vMinY, _float vMinZ, _float vMaxX, _float vMaxY, _float vMaxZ, wstring wstrName, COLLIDERTYPE eColliderType)
{
	return S_OK;
}

HRESULT CAhglan::Add_NaviMesh(_uint iCellCount, vector<_matrix> vecPoint)
{
	return S_OK;
}


CAhglan * CAhglan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAhglan*	pInstance = new CAhglan(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CAhglan::Free(void)
{
	CGameObject::Free();
}
