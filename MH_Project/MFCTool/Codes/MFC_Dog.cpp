#include "stdafx.h"
#include "MFC_Dog.h"
#include "DynamicMesh.h"

USING(Engine)

CMFC_Dog::CMFC_Dog(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMFC_Dog::~CMFC_Dog()
{
}


HRESULT CMFC_Dog::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(3.f, 0.f, 3.f);
	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	
	m_pMeshCom->Set_AnimationIndex(0);

	return S_OK;
}

_int CMFC_Dog::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Key_Input(fTimeDelta);

	if (m_iAniIndex >= 0 && 
		m_iAniIndex <= 17)
	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	m_pMeshCom->Play_Animation(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CMFC_Dog::Render_Object(void)
{
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

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
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

HRESULT CMFC_Dog::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// DynamicMesh
	pComponent = m_pMeshCom = dynamic_cast<CDynamicMesh*>(Clone_Prototype(L"Proto_DynamicMesh_Dog"));
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
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CMFC_Dog::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 10.f;

	pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrl.Diffuse);
	pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrl.Specular);
	pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrl.Ambient);

	pEffect->SetFloat("g_fPower", tMtrl.Power);

	const D3DLIGHT9*	pLightInfo = Get_Light();
	NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));

	pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLightInfo->Diffuse);
	pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLightInfo->Specular);
	pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLightInfo->Ambient);

	D3DXMatrixInverse(&matView, NULL, &matView);
	pEffect->SetVector("g_vCamPos", (_vec4*)&matView._41);

	return S_OK;
}

void CMFC_Dog::Key_Input(const _float & fTimeDelta)
{
	//m_pTransformCom->Get_INFO(INFO_LOOK, &m_vDir);

	//if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	//	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(90.f * fTimeDelta));

	//if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	//	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-90.f * fTimeDelta));
}

HRESULT CMFC_Dog::Add_Collider(_float fRadius, wstring cstrName, const _matrix* pColliderMatrix, COLLIDERTYPE eColliderType)
{
	CComponent*		pComponent = CCollider::Create(m_pGraphicDev, fRadius, pColliderMatrix, eColliderType);
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

HRESULT CMFC_Dog::Add_Collider(_float vMinX, _float vMinY, _float vMinZ, _float vMaxX, _float vMaxY, _float vMaxZ, wstring wstrName, const _matrix* pColliderMatrix, COLLIDERTYPE eColliderType)
{
	CComponent*		pComponent = CBoxCollider::Create(m_pGraphicDev, vMinX, vMinY, vMinZ, vMaxX, vMaxY, vMaxZ, pColliderMatrix, eColliderType);
	m_mapBoxColliderCom.emplace(wstrName, dynamic_cast<CBoxCollider*>(pComponent));
	if (m_mapBoxColliderCom.empty())
		return E_FAIL;

	m_mapComponent[ID_STATIC].emplace(wstrName, pComponent);

	return S_OK;
}

HRESULT CMFC_Dog::Delete_Collider(wstring wstrName, COLLIDERTYPE eColliderType)
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

HRESULT CMFC_Dog::Change_ColliderScale(_float fRadius, wstring cstrName)
{
	//map<const wstring, CCollider*>::iterator	iter = find_if(m_mapColliderCom.begin(), m_mapColliderCom.end(), CTag_Finder(cstrName));

	//if (iter == m_mapColliderCom.end())

	//	return E_FAIL;

	//Safe_Release(iter->second);
	//m_mapColliderCom.erase(cstrName);

	//Add_Collider(fRadius, cstrName);

	return S_OK;
}

CMFC_Dog * CMFC_Dog::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMFC_Dog* pInstance = new CMFC_Dog(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMFC_Dog::Free()
{
	CGameObject::Free();
}
