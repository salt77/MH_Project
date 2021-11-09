#include "stdafx.h"
#include "Logo.h"
#include "Stage.h"

#include "Export_Function.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{

}

CLogo::~CLogo(void)
{

}

HRESULT CLogo::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);

	m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);
	NULL_CHECK_RETURN(m_pLoading, E_FAIL);

	return S_OK;
}

HRESULT CLogo::LateReady_Scene()
{
	m_eSceneID = SCENE_LOGO;

	return S_OK;
}

Engine::_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int		iExit = CScene::Update_Scene(fTimeDelta);

	Update_ProgressUI();

	if (true == m_pLoading->Get_Finish() && 
		1.f <= m_fFadeOut)
	{
		CScene*		pScene = nullptr;

		pScene = CStage::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pScene, E_FAIL);

		FAILED_CHECK_RETURN(Set_Scene(pScene), E_FAIL);

		return iExit;
	}

	return iExit;
}

void CLogo::Render_Scene(void)
{
	Render_Font(L"Font_Nanumgothic_Bold", m_pLoading->Get_String(), &_vec2(SCREEN_CENTER_X - 390.f, LOADINGBAR_Y - 60.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

HRESULT CLogo::Ready_Layer_Environment(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*		pGameObject = nullptr;

	// BackGround
	pGameObject = CBackGround::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CLogo::Ready_Layer_UI(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*		pGameObject = nullptr;

	pGameObject = CLoadingBar_BackEffect::Create(m_pGraphicDev, SCREEN_CENTER_X, LOADINGBAR_Y, WINCX * 0.7f, 15.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Loading_BackEffect_UI", pGameObject), E_FAIL);

	pGameObject = CLoadingBar_Progress::Create(m_pGraphicDev, SCREEN_CENTER_X, LOADINGBAR_Y, WINCX * 0.7f, 15.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Loading_Progress_UI", pGameObject), E_FAIL);

	pGameObject = CFadeInOut::Create(m_pGraphicDev, 0.f, 0.f, WINCX * 2.f, WINCY * 2.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"FadeInOut_UI", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CLogo::Ready_Prototype(void)
{
	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Buffer_TriCol", CTriCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Buffer_RcTex", CRcTex::Create(m_pGraphicDev)), E_FAIL);

	// 로딩씬 텍스쳐
	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Texture_Logo", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Logo/Stage1.dds", TEX_NORMAL, 1)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Texture_Loading_BackEffect_UI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Loading_BackEffect.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Texture_Loading_Back_UI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Loading_Back.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Texture_Loading_Progress_UI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Loading_Progress.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Texture_Loading_Point_UI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Loading_Point.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Texture_FadeInOut_UI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/FadeInOut.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Transform", CTransform::Create(m_pGraphicDev)), E_FAIL);

	return S_OK;
}

void CLogo::Update_ProgressUI()
{
	CLoadingBar_Progress*	pProgress = static_cast<CLoadingBar_Progress*>(Engine::Get_GameObject(L"UI", L"Loading_Progress_UI"));
	CFadeInOut*		pFadeInOut = static_cast<CFadeInOut*>(Engine::Get_GameObject(L"UI", L"FadeInOut_UI"));

	if (pProgress && pFadeInOut)
	{
		_float	fTarget = 0.f;
		_float	fPreTarget = 0.f;
		_float	fSpeed = 0.f;

		if (L"Loading Buffer.." == m_pLoading->Get_String())
		{
			fTarget = 5.f;

			fSpeed = LERP(fTarget, fPreTarget, 0.9f);
		}
		else if (L"Loading Texture.." == m_pLoading->Get_String())
		{
			fTarget = 10.f;

			fSpeed = LERP(fTarget, fPreTarget, 0.9f);
		}
		else if (L"Loading Etc.." == m_pLoading->Get_String())
		{
			fTarget = 10.f;

			fSpeed = LERP(fTarget, fPreTarget, 0.9f);
		}
		else if (L"Loading Stage.." == m_pLoading->Get_String())
		{
			fTarget = 25.f;

			fSpeed = LERP(fTarget, fPreTarget, 0.9f);
		}
		else if (L"Loading Player.." == m_pLoading->Get_String())
		{
			fTarget = 45.f;

			fSpeed = LERP(fTarget, fPreTarget, 0.9f);
		}
		else if (L"Loading Object.." == m_pLoading->Get_String())
		{
			fTarget = 75.f;

			fSpeed = LERP(fTarget, fPreTarget, 0.9f);
		}
		else if (L"Loading Complete!" == m_pLoading->Get_String())
		{
			fTarget = 100.f;

			fSpeed = LERP(fTarget, fPreTarget, 0.9f);
		}

		fPreTarget = fTarget;

		if (m_fProgressBar < (fTarget * 0.01f))
		{
			m_fProgressBar += fSpeed * PROGRESSBARSPEED;			// 프로그래스바 속도 보정(너무 빠름)
		}

		if (1.f <= m_fProgressBar)
		{
			m_fFadeOut += FADEOUTSPEED * 0.85f;
			pFadeInOut->Set_ValueRatio(m_fFadeOut);
		}
		else
		{
			pProgress->Set_ValueRatio(m_fProgressBar);
			pFadeInOut->Set_ValueRatio(1.f - m_fProgressBar);
		}
	}
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo*	pInstance = new CLogo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Safe_Release(pInstance);

	return pInstance;
}

void CLogo::Free(void)
{
	Safe_Release(m_pLoading);

	CScene::Free();
}
