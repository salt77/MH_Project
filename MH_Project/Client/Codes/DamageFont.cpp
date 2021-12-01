#include "stdafx.h"
#include "DamageFont.h"
#include "DynamicCamera.h"

#include "Export_Function.h"

CDamageFont::CDamageFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CDamageFont::~CDamageFont()
{
}


HRESULT CDamageFont::Ready_Object(DAMAGEFONT_ID eID)
{
	m_eFontID = eID;

	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_vPos = POOLING_POS;

	m_pTransformCom->Set_Pos(&m_vPos);
	m_pTransformCom->Set_Scale(1.75f, 1.75f, 1.75f);

	m_vOriginScale = *m_pTransformCom->Get_ScaleInfo();

	return S_OK;
}

_int CDamageFont::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		_matrix		matWorld, matView, matBill;

		m_pTransformCom->Get_WorldMatrix(&matWorld);
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

		D3DXMatrixIdentity(&matBill);

		matBill._11 = matView._11;
		matBill._13 = matView._13;
		matBill._31 = matView._31;
		matBill._33 = matView._33;

		D3DXMatrixInverse(&matBill, NULL, &matBill);

		m_pTransformCom->Set_WorldMatrix(&(matBill * matWorld));

		_vec3 vPos;
		m_pTransformCom->Get_INFO(INFO_POS, &vPos);

		Compute_ViewZ(&vPos);

		Position_Interpolation(fTimeDelta);
		Scale_Interpolation(fTimeDelta);
		Alpha_Interpolation(fTimeDelta);

		Add_RenderGroup(RENDER_ALPHA, this);
	}

	return iExit;
}

void CDamageFont::Render_Object()
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


HRESULT CDamageFont::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Prototype(L"Proto_Buffer_RcTex"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
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
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_DamageFont"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CDamageFont::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", m_iFontNum);

	return S_OK;
}

void CDamageFont::Position_Interpolation(const _float& fTimeDelta)
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		if (m_dwFontStart + m_dwFontDelay >= GetTickCount())
		{
			CDynamicCamera*		pCamera = static_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));

			_vec3	vRightDir = pCamera->Get_CamDirVector(DIR_RIGHT);

			m_vOriginPos.y += 0.5f * fTimeDelta;
			_float	fInterval = m_pTransformCom->Get_ScaleInfo()->x * 1.3f;

			switch (m_iDigit)
			{
			case 1:
				m_vPos = m_vOriginPos + (vRightDir * fInterval);
				break;

			case 10:
				m_vPos = m_vOriginPos + (vRightDir * fInterval * 0.5f);
				break;

			case 100:
				m_vPos = m_vOriginPos - (vRightDir * 0.f);
				break;

			case 1000:
				m_vPos = m_vOriginPos - (vRightDir * fInterval * 0.5f);
				break;

			case 10000:
				m_vPos = m_vOriginPos - (vRightDir * fInterval * 1.f);
				break;
			}

			m_pTransformCom->Set_Pos(&m_vPos);
		}
		else
		{
			m_pTransformCom->Set_Pos(&POOLING_POS);
		}
	}
}

void CDamageFont::Scale_Interpolation(const _float& fTimeDelta)
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		CDynamicCamera*		pCamera = static_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));
		NULL_CHECK(pCamera);

		_vec3	vScale = *m_pTransformCom->Get_ScaleInfo();
		vScale.x = D3DXVec3Length(&(pCamera->Get_EyePos() - *m_pTransformCom->Get_Info(INFO_POS))) * 0.08f - m_fScaleDown * fTimeDelta;
		vScale.y = D3DXVec3Length(&(pCamera->Get_EyePos() - *m_pTransformCom->Get_Info(INFO_POS))) * 0.08f - m_fScaleDown * fTimeDelta;
		vScale.z = D3DXVec3Length(&(pCamera->Get_EyePos() - *m_pTransformCom->Get_Info(INFO_POS))) * 0.08f - m_fScaleDown * fTimeDelta;

		if (0.f <= vScale.x)
		{
			m_fScaleDown += m_fOriginScaleDown * 2.f;

			m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);
		}
		else
		{
			m_pTransformCom->Set_Scale(0.f, 0.f, 0.f);
		}
	}
	else
	{
		m_fScaleDown = m_fOriginScaleDown;

		m_pTransformCom->Set_Scale(m_vOriginScale.x, m_vOriginScale.y, m_vOriginScale.z);
	}
}

void CDamageFont::Alpha_Interpolation(const _float & fTimeDelta)
{
	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		m_fAlpha += fTimeDelta * 0.7f;
	}
}

void CDamageFont::Set_EnableDamageFont(_vec3 vPos, _uint iFontNum, _uint iDigit)
{
	m_vOriginPos = vPos;
	m_iFontNum = iFontNum;
	m_iDigit = iDigit;

	m_dwFontStart = GetTickCount();

	m_vPos = POOLING_POS + POOLING_POS;

	m_fScaleDown = m_fOriginScaleDown;
	m_fAlpha = m_fOriginAlpha;

	m_pTransformCom->Set_Pos(&m_vPos);
	m_pTransformCom->Set_Scale(m_vOriginScale.x, m_vOriginScale.y, m_vOriginScale.z);
}


CDamageFont * CDamageFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, DAMAGEFONT_ID eID)
{
	CDamageFont*	pInstance = new CDamageFont(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(eID)))
		Safe_Release(pInstance);

	return pInstance;
}

void CDamageFont::Free()
{
	CGameObject::Free();
}
