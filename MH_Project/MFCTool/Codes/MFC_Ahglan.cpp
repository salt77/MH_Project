#include "stdafx.h"
#include "MFC_Ahglan.h"
#include "DynamicMesh.h"

USING(Engine)

CMFC_Ahglan::CMFC_Ahglan(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMFC_Ahglan::~CMFC_Ahglan()
{
}


HRESULT CMFC_Ahglan::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(3.f, 0.f, 3.f);
	m_pTransformCom->Set_Scale(0.05f, 0.05f, 0.05f);
	
	m_pMeshCom->Set_AnimationIndex(0);

	return S_OK;
}

_int CMFC_Ahglan::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Key_Input(fTimeDelta);

	if (m_iAniIndex >= 0 && 
		m_iAniIndex <= 18)
	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	m_pMeshCom->Play_Animation(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CMFC_Ahglan::Render_Object(void)
{
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	
	/*if (m_pColliderCom)
		m_pColliderCom->Render_Collider(m_pTransformCom->Get_WorldMatrix());*/
	if (!m_mapColliderCom.empty())
	{
		map<const wstring, CCollider*>::iterator	iter = m_mapColliderCom.begin();

		for (; iter != m_mapColliderCom.end(); ++iter)
			iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
	}
	if (!m_mapBoxColliderCom.empty())
	{
		map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		for (; iter != m_mapBoxColliderCom.end(); ++iter)
			iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pMeshCom->Render_Meshes();
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMFC_Ahglan::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// DynamicMesh
	pComponent = m_pMeshCom = dynamic_cast<CDynamicMesh*>(Clone_Prototype(L"Proto_DynamicMesh_Ahglan"));
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

void CMFC_Ahglan::Key_Input(const _float & fTimeDelta)
{
	//m_pTransformCom->Get_INFO(INFO_LOOK, &m_vDir);

	//if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	//	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(90.f * fTimeDelta));

	//if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	//	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-90.f * fTimeDelta));
}

HRESULT CMFC_Ahglan::Add_Collider(_float fRadius, wstring cstrName, COLLIDERTYPE eColliderType)
{
	CComponent*		pComponent = CCollider::Create(m_pGraphicDev, fRadius, eColliderType);
	m_mapColliderCom.emplace(cstrName, dynamic_cast<CCollider*>(pComponent));
	if (m_mapColliderCom.empty())
		return E_FAIL;

	//wstring cstrCollider, cstrColNum;
	//cstrColNum.Format(_T("%d"), m_iColliderNum);
	//cstrCollider = "Com_Collider";
	//cstrCollider += cstrColNum;
	m_mapComponent[ID_STATIC].emplace(cstrName, pComponent);
	//++m_iColliderNum;

	return S_OK;
}

HRESULT CMFC_Ahglan::Add_Collider(_float vMinX, _float vMinY, _float vMinZ, _float vMaxX, _float vMaxY, _float vMaxZ, wstring wstrName, COLLIDERTYPE eColliderType)
{
	CComponent*		pComponent = CBoxCollider::Create(m_pGraphicDev, vMinX, vMinY, vMinZ, vMaxX, vMaxY, vMaxZ, eColliderType);
	m_mapBoxColliderCom.emplace(wstrName, dynamic_cast<CBoxCollider*>(pComponent));
	if (m_mapBoxColliderCom.empty())
		return E_FAIL;

	m_mapComponent[ID_STATIC].emplace(wstrName, pComponent);

	return S_OK;
}

HRESULT CMFC_Ahglan::Delete_Collider(wstring wstrName, COLLIDERTYPE eColliderType)
{
	map<const wstring, CComponent*>::iterator	iterCom;
	map<const wstring, CBoxCollider*>::iterator	iterBox;
	map<const wstring, CCollider*>::iterator	iterSphere;

	switch (eColliderType)
	{
	case Engine::COLTYPE_BOX_DAMAGED:
		iterBox = find_if(m_mapBoxColliderCom.begin(), m_mapBoxColliderCom.end(), CTag_Finder(wstrName));
		iterCom = find_if(m_mapComponent[ID_STATIC].begin(), m_mapComponent[ID_STATIC].end(), CTag_Finder(wstrName));

		if (iterBox == m_mapBoxColliderCom.end() ||
			iterCom == m_mapComponent[ID_STATIC].end())
			return E_FAIL;

		Safe_Release(iterBox->second);
		Safe_Release(iterCom->second);
		m_mapBoxColliderCom.erase(wstrName);
		m_mapComponent[ID_STATIC].erase(wstrName);

		break;
	case Engine::COLTYPE_BOX_HIT:
		iterBox = find_if(m_mapBoxColliderCom.begin(), m_mapBoxColliderCom.end(), CTag_Finder(wstrName));
		iterCom = find_if(m_mapComponent[ID_STATIC].begin(), m_mapComponent[ID_STATIC].end(), CTag_Finder(wstrName));

		if (iterBox == m_mapBoxColliderCom.end() ||
			iterCom == m_mapComponent[ID_STATIC].end())
			return E_FAIL;

		Safe_Release(iterBox->second);
		Safe_Release(iterCom->second);
		m_mapBoxColliderCom.erase(wstrName);
		m_mapComponent[ID_STATIC].erase(wstrName);

		break;
	case Engine::COLTYPE_SPHERE_DAMAGED:
		iterSphere = find_if(m_mapColliderCom.begin(), m_mapColliderCom.end(), CTag_Finder(wstrName));
		iterCom = find_if(m_mapComponent[ID_STATIC].begin(), m_mapComponent[ID_STATIC].end(), CTag_Finder(wstrName));

		if (iterSphere == m_mapColliderCom.end() ||
			iterCom == m_mapComponent[ID_STATIC].end())
			return E_FAIL;

		Safe_Release(iterSphere->second);
		Safe_Release(iterCom->second);
		m_mapColliderCom.erase(wstrName);
		m_mapComponent[ID_STATIC].erase(wstrName);

		break;
	case Engine::COLTYPE_SPHERE_HIT:
		iterSphere = find_if(m_mapColliderCom.begin(), m_mapColliderCom.end(), CTag_Finder(wstrName));
		iterCom = find_if(m_mapComponent[ID_STATIC].begin(), m_mapComponent[ID_STATIC].end(), CTag_Finder(wstrName));

		if (iterSphere == m_mapColliderCom.end() ||
			iterCom == m_mapComponent[ID_STATIC].end())
			return E_FAIL;

		Safe_Release(iterSphere->second);
		Safe_Release(iterCom->second);
		m_mapColliderCom.erase(wstrName);
		m_mapComponent[ID_STATIC].erase(wstrName);

		break;
	}

	return S_OK;
}

HRESULT CMFC_Ahglan::Change_ColliderScale(_float fRadius, wstring cstrName)
{
	//map<const wstring, CCollider*>::iterator	iter = find_if(m_mapColliderCom.begin(), m_mapColliderCom.end(), CTag_Finder(cstrName));

	//if (iter == m_mapColliderCom.end())

	//	return E_FAIL;

	//Safe_Release(iter->second);
	//m_mapColliderCom.erase(cstrName);

	//Add_Collider(fRadius, cstrName);

	return S_OK;
}

CMFC_Ahglan * CMFC_Ahglan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMFC_Ahglan* pInstance = new CMFC_Ahglan(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMFC_Ahglan::Free()
{
	CGameObject::Free();
}
