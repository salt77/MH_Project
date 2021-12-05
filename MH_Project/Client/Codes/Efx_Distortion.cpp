#include "stdafx.h"
#include "Efx_Distortion.h"


CEfx_Distortion::CEfx_Distortion(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CEfx_Distortion::CEfx_Distortion(const CEfx_Distortion & rhs)
	: CEffect(rhs)
{
}

CEfx_Distortion::~CEfx_Distortion()
{
}


HRESULT CEfx_Distortion::Ready_Object()
{
	FAILED_CHECK_RETURN(CEffect::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_vPos = POOLING_POS;

	m_pTransformCom->Set_Pos(&m_vPos);
	m_pTransformCom->Set_Scale(8.f, 8.f, 8.f);

	m_vOriginScale = *m_pTransformCom->Get_ScaleInfo();

	return S_OK;
}

HRESULT CEfx_Distortion::LateReady_Object()
{
	FAILED_CHECK_RETURN(CEffect::LateReady_Object(), E_FAIL);

	return S_OK;
}

_int CEfx_Distortion::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CEffect::Update_Object(fTimeDelta);

	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		_matrix		matWorld, matView, matBill;

		m_pTransformCom->Get_WorldMatrix(&matWorld);
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

		D3DXMatrixIdentity(&matBill);

		//matBill._11 = matView._11;
		//matBill._12 = matView._12;
		//matBill._21 = matView._21;
		//matBill._22 = matView._22;

		matBill._11 = matView._11;
		matBill._13 = matView._13;
		matBill._31 = matView._31;
		matBill._33 = matView._33;

		//matBill._22 = matView._22;
		//matBill._23 = matView._23;
		//matBill._32 = matView._32;

		D3DXMatrixInverse(&matBill, NULL, &matBill);

		m_pTransformCom->Set_WorldMatrix(&(matBill * matWorld));

		_vec3 vPos;
		m_pTransformCom->Get_INFO(INFO_POS, &vPos);

		Compute_ViewZ(&vPos);

		Scale_Interpolation(fTimeDelta);

		Add_RenderGroup(RENDER_ALPHA, this);
	}

	return iExit;
}

_int CEfx_Distortion::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CEffect::LateUpdate_Object(fTimeDelta);

	return iExit;
}

void CEfx_Distortion::Render_Object()
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

HRESULT CEfx_Distortion::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Prototype(L"Proto_Buffer_RcTex"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Distortion"));
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
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_Distortion"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CEfx_Distortion::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

	pEffect->SetFloat("g_fRatio", g_fRatio);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	return S_OK;
}

void CEfx_Distortion::Set_EnableDistortion(_vec3 vPos)
{
	m_vScale = m_vOriginScale;
	m_vPos = vPos;

	g_fRatio = 1.f;

	m_pTransformCom->Set_Pos(&vPos);
	m_pTransformCom->Set_Scale(m_vOriginScale.x, m_vOriginScale.y, m_vOriginScale.z);
}

void CEfx_Distortion::Scale_Interpolation(const _float & fTimeDelta)
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		g_fRatio -= 0.1f * fTimeDelta;

		//if (0.f < m_vScale.x)
		//	m_vScale.x -= m_fInterpol * 5.f * fTimeDelta;
		//if (0.f < m_vScale.y)
		//	m_vScale.y -= m_fInterpol * 5.f * fTimeDelta;
		//if (0.f < m_vScale.z)
		//	m_vScale.z -= m_fInterpol * 5.f * fTimeDelta;

		//m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);
	}
	else
	{
		m_vScale = m_vOriginScale;

		m_pTransformCom->Set_Pos(&POOLING_POS);
		m_pTransformCom->Set_Scale(m_vOriginScale.x, m_vOriginScale.y, m_vOriginScale.z);
	}
}


CEfx_Distortion * CEfx_Distortion::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEfx_Distortion*	pInstance = new CEfx_Distortion(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CEfx_Distortion::Free()
{
	CEffect::Free();
}
