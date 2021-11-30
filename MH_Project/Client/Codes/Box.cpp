#include "stdafx.h"
#include "Box.h"


CBox::CBox(LPDIRECT3DDEVICE9 pGraghicDev)
	: CGameObject(pGraghicDev)
{
}

CBox::CBox(const CBox & rhs)
	: CGameObject(rhs)
{
}

CBox::~CBox()
{
}


HRESULT CBox::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Collider(), E_FAIL);

	//m_pTransformCom->Set_Pos(&BOX_POS);
	m_pTransformCom->Set_Scale(SCALE_NORMAL * 1.6f, SCALE_NORMAL * 2.f, SCALE_NORMAL * 1.6f);

	return S_OK;
}

HRESULT CBox::LateReady_Object()
{
	FAILED_CHECK_RETURN(CGameObject::LateReady_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_NaviMesh(), E_FAIL);

	m_pNaviMeshCom->Set_CellIndex(Compute_InCell());

	return S_OK;
}

_int CBox::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

_int CBox::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	if (!m_mapColliderCom.empty())
	{
		_matrix		matWorld = *m_pTransformCom->Get_WorldMatrix();
		matWorld._42 += 0.3f;
		matWorld._43 -= 0.25f;

		m_mapColliderCom.begin()->second->LateUpdate_Collider(&matWorld);
	}
	if (!m_mapBoxColliderCom.empty())
		m_mapBoxColliderCom.begin()->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());

	return iExit;
}

void CBox::Render_Object()
{
	//_matrix		matWorld = *m_pTransformCom->Get_WorldMatrix();
	//matWorld._42 += 0.3f;
	//matWorld._43 -= 0.25f;
	//m_mapColliderCom.begin()->second->Render_Collider(COL_FALSE, &matWorld);
	//m_mapBoxColliderCom.begin()->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());

	//
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

HRESULT CBox::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CStaticMesh*>(Clone_Prototype(L"Proto_Mesh_Box"));
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

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CBox::Add_NaviMesh()
{
	CComponent*		pComponent = nullptr;

	// NaviMesh
	pComponent = m_pNaviMeshCom = dynamic_cast<CNaviMesh*>(Engine::Clone_Prototype(L"Proto_NaviMesh"));
	NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	return S_OK;
}

HRESULT CBox::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

HRESULT CBox::Ready_Collider()
{
	FAILED_CHECK_RETURN(Add_Collider(25.f, L"Box_Push_Collider", m_pTransformCom->Get_WorldMatrix(), COLTYPE_SPHERE_DAMAGED), E_FAIL);
	FAILED_CHECK_RETURN(Add_Collider(-32.f, -35.f, -35.f, 32.f, 50.f, 170.f, L"Gimmick_Box_Collider", m_pTransformCom->Get_WorldMatrix(), COLTYPE_BOX_OTHER), E_FAIL);

	return S_OK;
}

const _ulong & CBox::Compute_InCell()
{
	vector<CCell*>	vecCell = m_pNaviMeshCom->Get_CellVector();
	vector<CCell*>::iterator	iter = vecCell.begin();

	for (; iter != vecCell.end(); ++iter)
	{
		for (_uint i = 0; i < CCell::POINT_END; ++i)
		{
			_vec3	vTemp;
			_vec3	vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
			vMyPos.y = 0.f;

			_vec3	vPointA = *(*iter)->Get_Point(CCell::POINT_A);
			_vec3	vPointB = *(*iter)->Get_Point(CCell::POINT_B);
			_vec3	vPointC = *(*iter)->Get_Point(CCell::POINT_C);
			vPointA.y = 0.f;
			vPointB.y = 0.f;
			vPointC.y = 0.f;

			_vec3	vDirAB = vPointB - vPointA;
			_vec3	vDirAC = vPointC - vPointA;
			_vec3	vDirAP = vMyPos - vPointA;
			D3DXVec3Normalize(&vDirAB, &vDirAB);
			D3DXVec3Normalize(&vDirAC, &vDirAC);
			D3DXVec3Normalize(&vDirAP, &vDirAP);

			_vec3	vDirBA = vPointA - vPointB;
			_vec3	vDirBC = vPointC - vPointB;
			_vec3	vDirBP = vMyPos - vPointB;
			D3DXVec3Normalize(&vDirBA, &vDirBA);
			D3DXVec3Normalize(&vDirBC, &vDirBC);
			D3DXVec3Normalize(&vDirBP, &vDirBP);

			_vec3	vDirCA = vPointA - vPointC;
			_vec3	vDirCB = vPointB - vPointC;
			_vec3	vDirCP = vMyPos - vPointC;
			D3DXVec3Normalize(&vDirCA, &vDirCA);
			D3DXVec3Normalize(&vDirCB, &vDirCB);
			D3DXVec3Normalize(&vDirCP, &vDirCP);

			if (0.f < D3DXVec3Cross(&vTemp, &vDirCP, &vDirCB)->y &&
				0.f < D3DXVec3Cross(&vTemp, &vDirBP, &vDirBA)->y &&
				0.f < D3DXVec3Cross(&vTemp, &vDirAP, &vDirAC)->y)
			{
				return *(*iter)->Get_CellIndex();
			}
		}
	}

	return 0;
}

void CBox::MoveOn_NaviMesh(_vec3 vDir, _float fSpeed, const _float& fTimeDelta)
{
	m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(m_pTransformCom->Get_Info(INFO_POS), &vDir, fSpeed, fTimeDelta));
}


CBox * CBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBox*	pInstance = new CBox(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CBox::Free()
{
	CGameObject::Free();
}
