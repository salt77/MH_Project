#include "stdafx.h"
#include "Tooltip_Stemina.h"
#include "Buff_Stemina.h"

#include "Export_Function.h"

CTooltip_Stemina::CTooltip_Stemina(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{
}

CTooltip_Stemina::~CTooltip_Stemina()
{
}


HRESULT CTooltip_Stemina::Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	FAILED_CHECK_RETURN(CUI::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fX = fX;
	m_fY = fY;

	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	return S_OK;
}

HRESULT CTooltip_Stemina::LateReady_Object()
{
	FAILED_CHECK_RETURN(CUI::LateReady_Object(), E_FAIL);

	m_fX = dynamic_cast<CBuff_Player*>(Engine::Get_GameObject(L"Player_UI", L"Buff_Stemina_Icon"))->Get_PosX();
	m_fY = dynamic_cast<CBuff_Player*>(Engine::Get_GameObject(L"Player_UI", L"Buff_Stemina_Icon"))->Get_PosY();

	m_fX += 60.f;
	m_fY += 60.f;

	return S_OK;
}

_int CTooltip_Stemina::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CUI::Update_Object(fTimeDelta);

	if (m_bTooltipOn)
	{
		D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

		Tooltip_Announce();

		Add_RenderGroup(RENDER_ALPHA, this);
	}

	return iExit;
}

void CTooltip_Stemina::Render_Object()
{
	if (m_bTooltipOn)
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
}

CTooltip_Stemina* CTooltip_Stemina::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CTooltip_Stemina*	pInstance = new CTooltip_Stemina(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(fX, fY, fSizeX, fSizeY)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTooltip_Stemina::Free()
{
	CUI::Free();
}

HRESULT CTooltip_Stemina::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Prototype(L"Proto_Buffer_RcTex"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Tooltip_Stemina"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);

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

HRESULT CTooltip_Stemina::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

	pEffect->SetFloat("g_fAlphaValue", 0.9f);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", 0);

	return S_OK;
}

void CTooltip_Stemina::Tooltip_Announce()
{
	if (m_bTooltipOn)
	{
		if (m_dwTooltipStart + m_dwTooltipDelay < GetTickCount())
		{
			m_bTooltipOn = false;
		}
	}
}

