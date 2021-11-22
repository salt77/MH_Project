#include "stdafx.h"
#include "Trail_Sword.h"

#include "Export_Function.h"

CTrail_Sword::CTrail_Sword(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CTrail_Sword::~CTrail_Sword(void)
{
}


HRESULT CTrail_Sword::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CTrail_Sword::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	//if (!m_bRender)
	//{
	//	m_pTrailCom->Clear_TrailBuffer();
	//}

	Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CTrail_Sword::Render_Object(void)
{
	m_pTrailCom->Set_InfoForTrail(m_fDeltaTime, m_vWeaponLower, m_vWeaponUpper, m_pMatWeapon/*, m_dwMaxVtxCnt*/);

	if (m_bRender)
	{
		LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
		pEffect->AddRef();

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		_uint iMaxPass = 0;

		pEffect->Begin(&iMaxPass, NULL);		// 1인자 : 현재 쉐이더 파일이 반환하는 pass의 최대 개수
												// 2인자 : 시작하는 방식을 묻는 FLAG
		pEffect->BeginPass(0);

		m_pTrailCom->Render_Buffer();

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}
}

CTrail_Sword* CTrail_Sword::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTrail_Sword*	pInstance = new CTrail_Sword(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CTrail_Sword::Free(void)
{
	CGameObject::Free();
}

HRESULT CTrail_Sword::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pTrailCom = dynamic_cast<CTrail*>(Clone_Prototype(L"Proto_Buffer_Trail"));
	NULL_CHECK_RETURN(m_pTrailCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_SwordTrail"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_Trail"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CTrail_Sword::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//pEffect->SetTexture("g_BaseTexture", 0);
	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	//_matrix		matWorld, matView, matProj;

	//m_pGraphicDev->GetTransform(D3DTS_WORLD, &matWorld);
	//m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	//m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	//m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	//m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	//pEffect->SetMatrix("g_matWorld", &matWorld);
	//pEffect->SetMatrix("g_matView", &matView);
	//pEffect->SetMatrix("g_matProj", &matProj);

	//m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	return S_OK;
}

