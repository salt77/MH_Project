#include "stdafx.h"
#include "StageMesh_1.h"

#include "Export_Function.h"

CStageMesh_1::CStageMesh_1(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CStageMesh_1::CStageMesh_1(const CStageMesh_1& rhs)
	: CGameObject(rhs)
{
}

CStageMesh_1::~CStageMesh_1(void)
{
}


HRESULT CStageMesh_1::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);
	m_pTransformCom->Set_Scale(0.02f, 0.02f, 0.02f);

	return S_OK;
}

_int CStageMesh_1::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	_vec3	vPos;
	m_pTransformCom->Get_INFO(INFO_POS, &vPos);

	//m_bDraw = m_pOptimizationCom->Isin_Frustum_ForObject(&vPos, 0.f);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CStageMesh_1::Render_Object(void)
{
	//if (false == m_bDraw)
	//	return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
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

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CStageMesh_1* CStageMesh_1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStageMesh_1*	pInstance = new CStageMesh_1(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CStageMesh_1::Free(void)
{
	CGameObject::Free();
}

HRESULT CStageMesh_1::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CStaticMesh*>(Clone_Prototype(L"Proto_Mesh_Stage_1"));
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
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_StageMesh"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CStageMesh_1::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//D3DMATERIAL9		tMtrl;
	//ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	//tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	//tMtrl.Power = 10.f;

	//pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrl.Diffuse);
	//pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrl.Specular);
	//pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrl.Ambient);

	//pEffect->SetFloat("g_fPower", tMtrl.Power);

	const D3DLIGHT9*		pLightInfo = Get_Light();
	NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));

	//_vec3	vLocalLightDir = pLightInfo->Direction;
	//_matrix	matStageWorld = *m_pTransformCom->Get_WorldMatrix();
	//D3DXMatrixInverse(&matStageWorld, nullptr, &matStageWorld);
	//D3DXVec3TransformNormal(&vLocalLightDir, &vLocalLightDir, &matStageWorld);
	//pEffect->SetVector("g_vLocalLightDir", &_vec4(vLocalLightDir, 0.f));

	//pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLightInfo->Diffuse);
	//pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLightInfo->Specular);
	//pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLightInfo->Ambient);

	D3DXMatrixInverse(&matView, NULL, &matView);
	pEffect->SetVector("g_vCamPos", (_vec4*)&matView._41);

	return S_OK;
}

