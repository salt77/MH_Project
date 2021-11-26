#include "stdafx.h"
#include "RadialBlur.h"


CRadialBlur::CRadialBlur(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CRadialBlur::CRadialBlur(const CRadialBlur & rhs)
	: CEffect(rhs)
{
}

CRadialBlur::~CRadialBlur()
{
}


HRESULT CRadialBlur::Ready_Object()
{
	FAILED_CHECK_RETURN(CEffect::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_vPos = POOLING_POS;

	m_pTransformCom->Set_Pos(&m_vPos);
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_pTransformCom->Rotation(ROT_X, D3DXToRadian(90));

	m_vOriginScale = *m_pTransformCom->Get_ScaleInfo();

	return S_OK;
}

HRESULT CRadialBlur::LateReady_Object()
{
	FAILED_CHECK_RETURN(CEffect::LateReady_Object(), E_FAIL);

	return S_OK;
}

_int CRadialBlur::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CEffect::Update_Object(fTimeDelta);

	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		_vec3 vPos;
		m_pTransformCom->Get_INFO(INFO_POS, &vPos);

		Compute_ViewZ(&vPos);

		Position_Interpolation(fTimeDelta);
		Scale_Interpolation(fTimeDelta);

		Add_RenderGroup(RENDER_ALPHA, this);
	}

	return iExit;
}

_int CRadialBlur::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CEffect::LateUpdate_Object(fTimeDelta);

	return iExit;
}

void CRadialBlur::Render_Object()
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

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
}

HRESULT CRadialBlur::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Prototype(L"Proto_Buffer_RcTex"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Ground_Hit"));
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
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_Normal"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CRadialBlur::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlphaValue", 0.6f);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	return S_OK;
}

void CRadialBlur::Set_EnableRadialBlur(_vec3 vPos)
{
	m_vScale = m_vOriginScale;
	m_vPos = vPos;

	m_pTransformCom->Set_Pos(&vPos);
	m_pTransformCom->Set_Scale(m_vOriginScale.x, m_vOriginScale.y, m_vOriginScale.z);
}

void CRadialBlur::Position_Interpolation(const _float & fTimeDelta)
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		if (10.f <= m_vScale.x)
		{
			m_pTransformCom->Set_Pos(&POOLING_POS);
		}
	}
}

void CRadialBlur::Scale_Interpolation(const _float & fTimeDelta)
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		if (10.f > m_vScale.x)
		{
			m_vScale.x += m_fScaleInterpol * fTimeDelta;
			m_vScale.y += m_fScaleInterpol * fTimeDelta;
		}

		m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);
	}
	else
	{
		m_vScale = m_vOriginScale;
		
		m_pTransformCom->Set_Scale(m_vOriginScale.x, m_vOriginScale.y, m_vOriginScale.z);
	}
}


CRadialBlur * CRadialBlur::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRadialBlur*	pInstance = new CRadialBlur(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CRadialBlur::Free()
{
	CEffect::Free();
}
