#include "stdafx.h"
#include "Stage_Title_UI.h"

#include "Export_Function.h"

CStage_Title_UI::CStage_Title_UI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{
}

CStage_Title_UI::~CStage_Title_UI()
{
}


HRESULT CStage_Title_UI::Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	FAILED_CHECK_RETURN(CUI::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fX = fX;
	m_fY = fY;

	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	return S_OK;
}

HRESULT CStage_Title_UI::LateReady_Object()
{
	FAILED_CHECK_RETURN(CUI::LateReady_Object(), E_FAIL);

	m_eSceneID = Engine::Get_SceneID();
	m_fValueRatio = 0.f;

	return S_OK;
}

_int CStage_Title_UI::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CUI::Update_Object(fTimeDelta);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	Alpha_Interpolation(fTimeDelta);

	Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CStage_Title_UI::Render_Object()
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
	pEffect->BeginPass(1);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);
}

CStage_Title_UI* CStage_Title_UI::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CStage_Title_UI*	pInstance = new CStage_Title_UI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(fX, fY, fSizeX, fSizeY)))
		Safe_Release(pInstance);

	return pInstance;
}

void CStage_Title_UI::Free()
{
	CUI::Free();
}

HRESULT CStage_Title_UI::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Prototype(L"Proto_Buffer_RcTex"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Stage_Title_UI"));
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

HRESULT CStage_Title_UI::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

	switch (m_eSceneID)
	{
	case Engine::SCENE_STAGE:
		m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", 0);
		break;

	case Engine::SCENE_STAGE_1:
		m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", 1);
		break;
	}

	return S_OK;
}

void CStage_Title_UI::Alpha_Interpolation(const _float& fTimeDelta)
{
	if (m_dwSurviveTime + m_dwSurviveDelay < GetTickCount())
	{
		if (!m_bAlphaMax)
		{
			m_fValueRatio += m_fAlphaInterpol * fTimeDelta;

			if (1.f <= m_fValueRatio)
			{
				m_bAlphaMax = true;
				m_dwAlphaMaxTime = GetTickCount();
			}
		}
		else
		{
			if (m_dwAlphaMaxTime + m_dwAlphaMaxDelay < GetTickCount())
			{
				m_fValueRatio -= m_fAlphaInterpol * fTimeDelta;

				if (0.f >= m_fValueRatio)
				{
					m_bDead = true;
				}
			}
		}
	}
}

