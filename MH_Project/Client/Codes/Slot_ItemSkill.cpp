#include "stdafx.h"
#include "Slot_ItemSkill.h"
#include "Player_SlotUI.h"

#include "Export_Function.h"

CSlot_ItemSkill::CSlot_ItemSkill(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{
}

CSlot_ItemSkill::~CSlot_ItemSkill()
{
}


HRESULT CSlot_ItemSkill::Ready_Object()
{
	FAILED_CHECK_RETURN(CUI::Ready_Object(), E_FAIL);

	return S_OK;
}

HRESULT CSlot_ItemSkill::LateReady_Object()
{
	FAILED_CHECK_RETURN(CUI::LateReady_Object(), E_FAIL);

	return S_OK;
}

_int CSlot_ItemSkill::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CUI::Update_Object(fTimeDelta);

	return iExit;
}

_int CSlot_ItemSkill::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CUI::LateUpdate_Object(fTimeDelta);

	return _int();
}

void CSlot_ItemSkill::Render_Object()
{
}

void CSlot_ItemSkill::Free()
{
	CUI::Free();
}

HRESULT CSlot_ItemSkill::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

	pEffect->SetFloat("g_fAlphaValue", m_fValueRatio);
	pEffect->SetFloat("g_fCoolDownValue", m_fCoolDownValue);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", 0);

	return S_OK;
}

void CSlot_ItemSkill::Cool_Down(const _float & fTimeDelta)
{
	if (!m_bCanUse)
	{
		m_fCoolDownValue += m_fCoolDownDelay * fTimeDelta;

		if (1.f <= m_fCoolDownValue)
		{
			m_bCanUse = true;
		}
	}
}

