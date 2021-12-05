#include "stdafx.h"
#include "Start_Scene.h"
#include "Logo.h"

#include "Export_Function.h"

CStart_Scene::CStart_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CStart_Scene::~CStart_Scene()
{
}


HRESULT CStart_Scene::Ready_Scene()
{
	FAILED_CHECK_RETURN(CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);

	return S_OK;
}

HRESULT CStart_Scene::LateReady_Scene()
{
	m_eSceneID = SCENE_LOGO;

	return S_OK;
}

Engine::_int CStart_Scene::Update_Scene(const _float& fTimeDelta)
{
	_int		iExit = CScene::Update_Scene(fTimeDelta);

	Ready_to_Logo(fTimeDelta);

	if (1.f <= m_fFadeOut)
	{
		Engine::Clear_Prototype_ForNextStage();

		CScene*		pScene = nullptr;

		pScene = CLogo::Create(m_pGraphicDev, 1);
		NULL_CHECK_RETURN(pScene, E_FAIL);

		FAILED_CHECK_RETURN(Set_Scene(pScene), E_FAIL);

		return iExit;
	}

	return iExit;
}

void CStart_Scene::Render_Scene()
{
}

HRESULT CStart_Scene::Ready_Layer_Environment(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*		pGameObject = nullptr;

	// BackGround
	pGameObject = CBackGround::Create(m_pGraphicDev, 98);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStart_Scene::Ready_Layer_UI(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*		pGameObject = nullptr;

	pGameObject = CFadeInOut::Create(m_pGraphicDev, 0.f, 0.f, WINCX * 2.f, WINCY * 2.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"FadeInOut_UI", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStart_Scene::Ready_Prototype()
{
	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Buffer_TriCol", CTriCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Buffer_RcTex", CRcTex::Create(m_pGraphicDev)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Texture_Start_Scene", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Logo.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Texture_FadeInOut_UI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/FadeInOut.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_Transform", CTransform::Create(m_pGraphicDev)), E_FAIL);

	return S_OK;
}

void CStart_Scene::Ready_to_Logo(const _float& fTimeDelta)
{
	if (Key_Down(VK_RETURN))
	{
		m_bReadyFadeOut = true;

		SoundMgrLowerVol(L"ui_horn_sound.wav", CSoundMgr::BATTLE, 0.15f);
	}

	if (m_bReadyFadeOut)
	{
		m_fFadeOut += FADEOUTSPEED * 1000.f * fTimeDelta;
	}

	static_cast<CFadeInOut*>(Engine::Get_GameObject(L"UI", L"FadeInOut_UI"))->Set_ValueRatio(m_fFadeOut);
}


CStart_Scene* CStart_Scene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStart_Scene*	pInstance = new CStart_Scene(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Safe_Release(pInstance);

	return pInstance;
}

void CStart_Scene::Free()
{
	CScene::Free();
}
