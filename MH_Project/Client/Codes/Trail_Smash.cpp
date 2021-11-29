#include "stdafx.h"
#include "Trail_Smash.h"

#include "Export_Function.h"

CTrail_Smash::CTrail_Smash(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CTrail_Smash::~CTrail_Smash()
{
}


HRESULT CTrail_Smash::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CTrail_Smash::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CTrail_Smash::Render_Object()
{
	m_pTrailCom->Set_InfoForTrail(m_fDeltaTime, m_vWeaponLower, m_vWeaponUpper, m_pMatWeapon);

	if (m_bRender)
	{
		LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
		pEffect->AddRef();

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		_uint iMaxPass = 0;

		pEffect->Begin(&iMaxPass, NULL);		// 1���� : ���� ���̴� ������ ��ȯ�ϴ� pass�� �ִ� ����
												// 2���� : �����ϴ� ����� ���� FLAG
		pEffect->BeginPass(0);

		m_pTrailCom->Render_Buffer();

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}
}

CTrail_Smash* CTrail_Smash::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTrail_Smash*	pInstance = new CTrail_Smash(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CTrail_Smash::Free()
{
	CGameObject::Free();
}

HRESULT CTrail_Smash::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pTrailCom = dynamic_cast<CSmashTrail*>(Clone_Prototype(L"Proto_Buffer_SmashTrail"));
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

HRESULT CTrail_Smash::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlphaValue);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	return S_OK;
}

