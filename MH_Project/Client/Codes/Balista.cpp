#include "stdafx.h"
#include "Balista.h"


CBalista::CBalista(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CBalista::CBalista(const CBalista & rhs)
	: CGameObject(rhs)
{
}

CBalista::~CBalista()
{
}


HRESULT CBalista::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Collider(), E_FAIL);

	m_pTransformCom->Set_Pos(&POOLING_POS);
	m_pTransformCom->Set_Scale(SCALE_BALISTA, SCALE_BALISTA, SCALE_BALISTA);

	return S_OK;
}

HRESULT CBalista::LateReady_Object()
{
	FAILED_CHECK_RETURN(CGameObject::LateReady_Object(), E_FAIL);

	return S_OK;
}

_int CBalista::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		Movement(fTimeDelta);

		Add_RenderGroup(RENDER_NONALPHA, this);
	}

	return iExit;
}

_int CBalista::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	m_mapColliderCom.begin()->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());
	m_mapBoxColliderCom.begin()->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());

	return iExit;
}

void CBalista::Render_Object()
{
	map<const wstring, CCollider*>::iterator	iter_Sphere = m_mapColliderCom.begin();
	map<const wstring, CBoxCollider*>::iterator	iter_Box = m_mapBoxColliderCom.begin();

	for (; iter_Sphere != m_mapColliderCom.end(); ++iter_Sphere)
	{
		iter_Sphere->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
	}
	for (; iter_Box != m_mapBoxColliderCom.end(); ++iter_Box)
	{
		iter_Box->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
	}

	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

		LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
		pEffect->AddRef();

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		_uint iMaxPass = 0;

		pEffect->Begin(&iMaxPass, NULL);		// 1인자 : 현재 쉐이더 파일이 반환하는 pass의 최대 개수
												// 2인자 : 시작하는 방식을 묻는 FLAG
		pEffect->BeginPass(0);

		m_pMeshCom->Render_Meshes(pEffect);

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}
}

HRESULT CBalista::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CStaticMesh*>(Clone_Prototype(L"Proto_Mesh_Balista"));
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

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CBalista::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	return S_OK;
}

HRESULT CBalista::Ready_Collider()
{
	FAILED_CHECK_RETURN(Add_Collider(20.f, L"Collider_Sphere", m_pTransformCom->Get_WorldMatrix(), COLTYPE_SPHERE_HIT), E_FAIL);
	FAILED_CHECK_RETURN(Add_Collider(-5.f, -5.f, -5.f, 5.f, 5.f, 5.f, L"Collider_Box", m_pTransformCom->Get_WorldMatrix(), COLTYPE_BOX_HIT), E_FAIL);

	return S_OK;
}

void CBalista::Movement(const _float& fTimeDelta)
{
	if (SCENE_STAGE_1 == Engine::Get_SceneID())
	{
		if (!m_bCollision)
		{
			m_pTransformCom->Move_Pos(&m_vDir, m_fSpeed, fTimeDelta);
		}
		else
		{
			if (m_dwSurviveTime + m_dwSurviveDelay < GetTickCount())
			{
				m_bCollision = false;
	
				m_pTransformCom->Set_Pos(&POOLING_POS);
			}
		}
	}
	else if (SCENE_STAGE == Engine::Get_SceneID())
	{

	}
}

void CBalista::Set_EnableBalista(_vec3 vPos, _vec3 vDir)
{
	m_bCollision = false;

	m_pTransformCom->Set_Pos(&vPos);
	m_vDir = *D3DXVec3Normalize(&vDir, &vDir);
}


CBalista * CBalista::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBalista*	pInstance = new CBalista(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CBalista::Free()
{
	CGameObject::Free();
}
