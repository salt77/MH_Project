#include "stdafx.h"
#include "Boss_NamingScene.h"

#include "Export_Function.h"

CBoss_NamingScene::CBoss_NamingScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{
}

CBoss_NamingScene::~CBoss_NamingScene(void)
{
}


HRESULT CBoss_NamingScene::Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY, NAMINGSCENE_ID eID, BOSS_ID eBossId)
{
	m_eNamingId = eID;
	m_eBossId = eBossId;

	FAILED_CHECK_RETURN(CUI::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fX = fX;
	m_fY = fY;

	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	m_fValueRatio = 0.f;

	return S_OK;
}

_int CBoss_NamingScene::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CUI::Update_Object(fTimeDelta);

	if (m_bDead)
		return OBJ_DEAD;

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	FadeInOut(fTimeDelta);

	Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CBoss_NamingScene::Render_Object(void)
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

CBoss_NamingScene* CBoss_NamingScene::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY, NAMINGSCENE_ID eID, BOSS_ID eBossId)
{
	CBoss_NamingScene*	pInstance = new CBoss_NamingScene(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(fX, fY, fSizeX, fSizeY, eID, eBossId)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBoss_NamingScene::Free(void)
{
	CUI::Free();
}

HRESULT CBoss_NamingScene::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Prototype(L"Proto_Buffer_RcTex"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	switch (m_eBossId)
	{
	case Engine::BOSS_AHGLAN:
		if (NAMING_BACKGROUND == m_eNamingId)
		{
			pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Boss_NamingSceneUI"));
			NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
			m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
		}
		else if (NAMING_BOSSNAME == m_eNamingId)
		{
			pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Ahglan_Bottom_FontUI"));
			NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
			m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
		}
		else if (NAMING_STAGENAME == m_eNamingId)
		{
			pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Ahglan_Bottom_FontUI2"));
			NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
			m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
		}
		break;

	case Engine::BOSS_CLOYAN:
		if (NAMING_BACKGROUND == m_eNamingId)
		{
			pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Boss_NamingSceneUI"));
			NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
			m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
		}
		else if (NAMING_BOSSNAME == m_eNamingId)
		{
			pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Cloyan_FontUI"));
			NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
			m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
		}
		else if (NAMING_STAGENAME == m_eNamingId)
		{
			pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Cloyan_Bottom_FontUI2"));
			NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
			m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);
		}
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
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_FadeInOut_UI"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CBoss_NamingScene::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", 0);

	pEffect->SetFloat("g_fAlphaValue", m_fValueRatio);

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

void CBoss_NamingScene::FadeInOut(const _float& fTimeDelta)
{
	if (1.f > m_fValueRatio &&
		0 == m_iFadeCount)
	{
		switch (m_eNamingId)
		{
		case Engine::NAMING_BOSSNAME:
			if (m_dwBossNameInStart + m_dwBossNameInDelay <= GetTickCount())
			{
				m_fValueRatio += FADEOUTSPEED * 900.f * fTimeDelta;
			}
			break;

		case Engine::NAMING_STAGENAME:
			m_fValueRatio += FADEOUTSPEED * 600.f * fTimeDelta;
			break;

		case Engine::NAMING_BACKGROUND:
			m_fValueRatio += FADEOUTSPEED * 600.f * fTimeDelta;
			break;
		}
	}

	if (1.f <= m_fValueRatio)
	{
		m_iFadeCount = 1;

		if (!m_bOneTimeCheck)
		{
			m_bOneTimeCheck = true;

			m_dwBackOutTime = GetTickCount();
			m_dwBossNameOutTime = GetTickCount();
		}
	}

	if (1 == m_iFadeCount &&
		0.f <= m_fValueRatio)
	{
		switch (m_eNamingId)
		{
		case Engine::NAMING_BOSSNAME:
			if (m_dwBossNameOutTime + m_dwBossNameOutDelay <= GetTickCount())
			{
				m_fValueRatio -= FADEOUTSPEED * 600.f * fTimeDelta;
			}
			break;

		case Engine::NAMING_STAGENAME:
			m_fValueRatio -= FADEOUTSPEED * 400.f * fTimeDelta;
			break;

		case Engine::NAMING_BACKGROUND:
			if (m_dwBackOutTime + m_dwBackOutDelay <= GetTickCount())
			{
				m_fValueRatio -= FADEOUTSPEED * 400.f * fTimeDelta;
			}
			break;
		}
	}

	// 페이드아웃 종료 후 레이어에서 삭제
	if (1 == m_iFadeCount &&
		0.f >= m_fValueRatio)
	{
		m_bDead = true;
	}
}
