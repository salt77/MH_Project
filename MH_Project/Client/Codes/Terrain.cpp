#include "stdafx.h"
#include "Terrain.h"

#include "Export_Function.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

CTerrain::~CTerrain(void)
{
}


HRESULT CTerrain::Ready_Object(_uint iGrass)
{
	m_iGrass = iGrass;

	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pIndex = new INDEX32[m_pBufferCom->Get_TriCnt()];

	return S_OK;
}

HRESULT CTerrain::LateReady_Object()
{
	SCENE_ID	eID = Engine::Get_SceneID();

	if (SCENE_STAGE_1 == eID)
	{
		_vec3	vPos = *m_pTransformCom->Get_Info(INFO_POS);
		vPos.y += 2.6f;
		vPos.z -= 50.f;
		m_pTransformCom->Set_Pos(&vPos);
	}

	return S_OK;
}

_int CTerrain::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CTerrain::Render_Object(void)
{
	//	m_pBufferCom->Copy_Indices(m_pIndex, m_dwTriCnt);

	LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
	pEffect->AddRef();

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	_uint iMaxPass = 0;

	pEffect->Begin(&iMaxPass, NULL);		// 1인자 : 현재 쉐이더 파일이 반환하는 pass의 최대 개수
											// 2인자 : 시작하는 방식을 묻는 FLAG
	pEffect->BeginPass(0);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iGrass)
{
	CTerrain*	pInstance = new CTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iGrass)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTerrain::Free(void)
{
	Safe_Delete_Array(m_pIndex);

	CGameObject::Free();
}

HRESULT CTerrain::Add_Component(void)
{
	CComponent*		pComponent = nullptr;
	
	// buffer
	if (0 == m_iGrass)
	{
		pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(Clone_Prototype(L"Proto_Buffer_TerrainTex"));
		NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);
	}
	else if (1 == m_iGrass)
	{
		pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(Clone_Prototype(L"Proto_Buffer_TerrainTex2"));
		NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);
	}
	else if (2 == m_iGrass)
	{
		pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(Clone_Prototype(L"Proto_Buffer_TerrainTex3"));
		NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);
	}

	// texture
	if (0 == m_iGrass)
	{
		pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Terrain"));
		NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
	}
	else if (1 == m_iGrass || 2 == m_iGrass)
	{
		pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Terrain2"));
		NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
	}

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Terrain"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CTerrain::SetUp_Material(void)
{
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.65f, 0.65f, 0.65f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

HRESULT CTerrain::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", 0);
	m_pTextureCom->Set_Texture(pEffect, "g_NormalTexture", 1);

	//_float	fDetail = 1.f;

	//pEffect->SetFloat("g_fDetail", fDetail);

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

