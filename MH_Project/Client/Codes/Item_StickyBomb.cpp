#include "stdafx.h"
#include "Item_StickyBomb.h"
#include "Player_SlotUI.h"

#include "Export_Function.h"

CItem_StickyBomb::CItem_StickyBomb(LPDIRECT3DDEVICE9 pGraphicDev)
	: CSlot_ItemSkill(pGraphicDev)
{
}

CItem_StickyBomb::~CItem_StickyBomb()
{
}


HRESULT CItem_StickyBomb::Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	FAILED_CHECK_RETURN(CSlot_ItemSkill::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fX = fX;
	m_fY = fY;

	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	m_fValueRatio = 0.8f;
	m_fCoolDownDelay = 0.08f;

	return S_OK;
}

HRESULT CItem_StickyBomb::LateReady_Object()
{
	FAILED_CHECK_RETURN(CSlot_ItemSkill::LateReady_Object(), E_FAIL);

	m_pSlotUI = dynamic_cast<CPlayer_SlotUI*>(Engine::Get_GameObject(L"Player_UI", L"Player_SlotUI"));
	NULL_CHECK_RETURN(m_pSlotUI, E_FAIL);

	m_fX = m_pSlotUI->Get_PosX();
	m_fY = m_pSlotUI->Get_PosY();

	m_fX += 250.f;

	return S_OK;
}

_int CItem_StickyBomb::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CSlot_ItemSkill::Update_Object(fTimeDelta);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	Cool_Down(fTimeDelta);

	Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

_int CItem_StickyBomb::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CSlot_ItemSkill::LateUpdate_Object(fTimeDelta);

	return iExit;
}

void CItem_StickyBomb::Render_Object()
{
	_matrix		matOldView, matOldProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOldProj);

	LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
	pEffect->AddRef();

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	_uint iMaxPass = 0;

	pEffect->Begin(&iMaxPass, NULL);		// 1���� : ���� ���̴� ������ ��ȯ�ϴ� pass�� �ִ� ����
											// 2���� : �����ϴ� ����� ���� FLAG
	pEffect->BeginPass(0);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);
}

CItem_StickyBomb* CItem_StickyBomb::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CItem_StickyBomb*	pInstance = new CItem_StickyBomb(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(fX, fY, fSizeX, fSizeY)))
		Safe_Release(pInstance);

	return pInstance;
}

void CItem_StickyBomb::Free()
{
	CSlot_ItemSkill::Free();
}

HRESULT CItem_StickyBomb::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Prototype(L"Proto_Buffer_RcTex"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Item_StickBomb"));
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
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_Skill_UI"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CItem_StickyBomb::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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