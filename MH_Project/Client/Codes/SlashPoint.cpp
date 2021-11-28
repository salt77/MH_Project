#include "stdafx.h"
#include "SlashPoint.h"


CSlashPoint::CSlashPoint(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CSlashPoint::CSlashPoint(const CSlashPoint & rhs)
	: CEffect(rhs)
{
}

CSlashPoint::~CSlashPoint()
{
}


HRESULT CSlashPoint::Ready_Object()
{
	FAILED_CHECK_RETURN(CEffect::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_vPos = POOLING_POS;

	m_pTransformCom->Set_Pos(&m_vPos);
	m_pTransformCom->Set_Scale(1.75f, 1.75f, 1.75f);
	// Radian이 아닌 Degree로 회전(약간 어긋나보이게끔)
	m_pTransformCom->Rotation(ROT_Z, 90.f);

	m_vOriginScale = *m_pTransformCom->Get_ScaleInfo();

	return S_OK;
}

HRESULT CSlashPoint::LateReady_Object()
{
	FAILED_CHECK_RETURN(CEffect::LateReady_Object(), E_FAIL);

	return S_OK;
}

_int CSlashPoint::Update_Object(const _float & fTimeDelta)
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

		//matBill._11 = matView._11;
		//matBill._13 = matView._13;
		//matBill._31 = matView._31;
		//matBill._33 = matView._33;

		matBill._22 = matView._22;
		matBill._23 = matView._23;
		matBill._32 = matView._32;

		D3DXMatrixInverse(&matBill, NULL, &matBill);

		m_pTransformCom->Set_WorldMatrix(&(matBill * matWorld));

		_vec3 vPos;
		m_pTransformCom->Get_INFO(INFO_POS, &vPos);

		Compute_ViewZ(&vPos);

		Position_Interpolation(fTimeDelta);
		Scale_Interpolation(fTimeDelta);

		Add_RenderGroup(RENDER_ALPHA, this);
	}

	return iExit;
}

_int CSlashPoint::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CEffect::LateUpdate_Object(fTimeDelta);

	return iExit;
}

void CSlashPoint::Render_Object()
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
		pEffect->BeginPass(1);

		m_pBufferCom->Render_Buffer();

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}
}

HRESULT CSlashPoint::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Prototype(L"Proto_Buffer_RcTex"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_SlashPoint"));
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

HRESULT CSlashPoint::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

	pEffect->SetFloat("g_fAlphaValue", 1.f);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	return S_OK;
}

void CSlashPoint::Set_EnableSlashPoint(_vec3 vPos, _bool bIsSmash)
{
	m_vScale = m_vOriginScale;
	m_vPos = vPos;

	m_pTransformCom->Set_Pos(&vPos);

	_vec3	vScale;

	if (!bIsSmash)
	{
		vScale = m_vOriginScale;
	}
	else
	{
		vScale = m_vOriginScale * 1.5f;
	}

	_float	fRand = Engine::Random(1.f, 1.5f);
	m_pTransformCom->Set_Scale(vScale.x * fRand, vScale.y * fRand, vScale.z * fRand);
}

void CSlashPoint::Position_Interpolation(const _float & fTimeDelta)
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		if (0.35f >= m_vScale.x)
		{
			m_pTransformCom->Set_Pos(&POOLING_POS);
		}
	}
}

void CSlashPoint::Scale_Interpolation(const _float & fTimeDelta)
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		if (0.35f < m_vScale.x)
			m_vScale.x -= m_fScaleXInterpol * fTimeDelta;
		if (0.35f < m_vScale.y)
			m_vScale.y -= m_fScaleYInterpol * fTimeDelta;

		m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);
	}
	else
	{
		m_vScale = m_vOriginScale;

		m_pTransformCom->Set_Scale(m_vOriginScale.x, m_vOriginScale.y, m_vOriginScale.z);
	}
}


CSlashPoint * CSlashPoint::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSlashPoint*	pInstance = new CSlashPoint(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CSlashPoint::Free()
{
	CEffect::Free();
}
