#include "stdafx.h"
#include "Damage_FontUI.h"

#include "Export_Function.h"

CDamage_FontUI::CDamage_FontUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{
}

CDamage_FontUI::~CDamage_FontUI(void)
{
}


HRESULT CDamage_FontUI::Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY, DAMAGEFONT_ID eID, _uint iDamage)
{
	m_eFontID = eID;

	FAILED_CHECK_RETURN(CUI::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fX = fX;
	m_fY = fY;

	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	m_iDamage = iDamage;

	return S_OK;
}

_int CDamage_FontUI::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CUI::Update_Object(fTimeDelta);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CDamage_FontUI::Render_Object(void)
{
	_matrix		matOldView, matOldProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOldProj);

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

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);
}

CDamage_FontUI* CDamage_FontUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY, DAMAGEFONT_ID eID, _uint iDamage)
{
	CDamage_FontUI*	pInstance = new CDamage_FontUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(fX, fY, fSizeX, fSizeY, eID, iDamage)))
		Safe_Release(pInstance);

	return pInstance;
}

void CDamage_FontUI::Free(void)
{
	CUI::Free();
}

HRESULT CDamage_FontUI::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Prototype(L"Proto_Buffer_RcTex"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	switch (m_eFontID)
	{
	case Engine::DAMAGEFONT_NORMAL:
		pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Damage_Font_NormalUI"));
		NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
		break;

	case Engine::DAMAGEFONT_SKILL:
		pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Damage_Font_SkillUI"));
		NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
		break;

	case Engine::DAMAGEFONT_MONSTER:
		pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Damage_Font_MonsterUI"));
		NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
		break;
	}

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_UI"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CDamage_FontUI::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);

	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;
	matWorld._33 = 1.f;
	matWorld._41 = m_fX - WINCX * 0.5f;
	matWorld._42 = -m_fY + WINCY * 0.5f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &m_matProj);

	//m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", );

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

	return S_OK;
}

