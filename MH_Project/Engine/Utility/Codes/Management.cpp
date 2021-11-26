#include "Export_Utility.h"
#include "MFC_ObjectManager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CManagement)

Engine::CManagement::CManagement(void)	
{
}

Engine::CManagement::~CManagement(void)
{
	Free();
}


HRESULT CManagement::Ready_Shader(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	CShader*		pShader = nullptr;

	D3DVIEWPORT9	ViewPort;
	pGraphicDev->GetViewport(&ViewPort);

	// 렌더 타겟 생성
	FAILED_CHECK_RETURN(Ready_RenderTarget(pGraphicDev, L"Target_Albedo", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_DebugBuffer(L"Target_Albedo", 0.f, 0.f, 200.f, 200.f), E_FAIL);

	FAILED_CHECK_RETURN(Ready_RenderTarget(pGraphicDev, L"Target_Normal", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_DebugBuffer(L"Target_Normal", 0.f, 200.f, 200.f, 200.f), E_FAIL);

	FAILED_CHECK_RETURN(Ready_RenderTarget(pGraphicDev, L"Target_Depth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(1.f, 1.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_DebugBuffer(L"Target_Depth", 0.f, 400.f, 200.f, 200.f), E_FAIL);

	FAILED_CHECK_RETURN(Ready_RenderTarget(pGraphicDev, L"Target_Shade", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_DebugBuffer(L"Target_Shade", 200.f, 0.f, 200.f, 200.f), E_FAIL);

	FAILED_CHECK_RETURN(Ready_RenderTarget(pGraphicDev, L"Target_Specular", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_DebugBuffer(L"Target_Specular", 200.f, 200.f, 200.f, 200.f), E_FAIL);

	// 멀티 렌더 그룹 편성
	FAILED_CHECK_RETURN(Ready_MRT(L"MRT_Deferred", L"Target_Albedo"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_MRT(L"MRT_Deferred", L"Target_Normal"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_MRT(L"MRT_Deferred", L"Target_Depth"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_MRT(L"MRT_LightAcc", L"Target_Shade"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_MRT(L"MRT_LightAcc", L"Target_Specular"), E_FAIL);


	// shader_sample
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Sample.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_Sample", pShader), E_FAIL);

	//shader_terrain
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Terrain.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_Terrain", pShader), E_FAIL);

	// shader_mesh
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Mesh.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_Mesh", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Player.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_Player", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Ahglan.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_Ahglan", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_StageMesh.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_StageMesh", pShader), E_FAIL);

	// shader_shade
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Shade.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_Shade", pShader), E_FAIL);

	// shader_shade
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Blend.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_Blend", pShader), E_FAIL);

	// shader_Normal_UI
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_UI.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_UI", pShader), E_FAIL);

	// shader_Player_Hpbar_Value_UI
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Player_Hpbar.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_Player_Hpbar_Value_UI", pShader), E_FAIL);

	// shader_Loading_Progress_UI
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Loading_Progress.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_Loading_Progress_UI", pShader), E_FAIL);

	// shader_FadeInOut_UI
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_FadeInOut.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_FadeInOut_UI", pShader), E_FAIL);

	// shader_BossHpbar_UI
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Boss_Hpbar.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_BossHpbar_UI", pShader), E_FAIL);

	// shader_DamageFont
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_DamageFont.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_DamageFont", pShader), E_FAIL);

	// shader_Normal
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Normal.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_Normal", pShader), E_FAIL);

	// shader_Normal
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Headers/Shader_Trail.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Shader_Trail", pShader), E_FAIL);

	return S_OK;
}

HRESULT Engine::CManagement::Set_Scene(CScene* pScene)
{
	Safe_Release(m_pScene);	// 주의합시다!!!!!!!!!!!!

	Clear_RenderGroup();

	m_pScene = pScene;

	m_pScene->LateReady_Scene();

	return S_OK;
}

void CManagement::Emplace_Layer(const wstring wstrLayerTag, CLayer * pLayer)
{
	if (!m_pScene)
		return;

	m_pScene->Emplace_Layer(wstrLayerTag, pLayer);
}

void CManagement::Delete_Layer(const wstring wstrLayerTag, const wstring wstrObjTag)
{
	if (!m_pScene)
		return;

	m_pScene->Delete_Layer(wstrLayerTag, wstrObjTag);
}

void CManagement::Delete_AllInLayer(const wstring wstrLayerTag)
{
	if (!m_pScene)
		return;

	m_pScene->Delete_AllInLayer(wstrLayerTag);
}

Engine::_int Engine::CManagement::Update_Scene(const _float& fTimeDelta)
{
	if (nullptr == m_pScene)
		return -1;

	m_pScene->Update_Scene(fTimeDelta);

	return 0;
}

_int CManagement::LateUpdate_Scene(const _float & fTimeDelta)
{
	if (nullptr == m_pScene)
		return -1;

	m_pScene->LateUpdate_Scene(fTimeDelta);

	return 0;
}

void Engine::CManagement::Render_Scene(LPDIRECT3DDEVICE9& pGraphicDev)
{
	Render_GameObject(pGraphicDev);

	if (nullptr == m_pScene)
		return;

	m_pScene->Render_Scene();
}

void Engine::CManagement::Free(void)
{
	CMFC_ObjectManager::DestroyInstance();

	Safe_Release(m_pScene);
}

HRESULT CManagement::AddGameObjectInManager(const wstring LayerTag, CLayer * pLayer)
{
	CMFC_ObjectManager::GetInstance()->AddGameObjectInManager(LayerTag, pLayer);

	return S_OK;
}

HRESULT CManagement::DeleteGameObjectInManager(const wstring LayerTag)
{
	CMFC_ObjectManager::GetInstance()->DeleteGameObjectInManager(LayerTag);

	return S_OK;
}

CGameObject * CManagement::Get_MFCGameObject(const wstring pLayerTag, const wstring pObjTag)
{
	return CMFC_ObjectManager::GetInstance()->Get_MFCGameObject(pLayerTag, pObjTag);
}

CComponent * CManagement::Get_MFCComponent(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID)
{
	return CMFC_ObjectManager::GetInstance()->Get_MFCComponent(pLayerTag, pObjTag, pComponentTag, eID);
}

CGameObject * CManagement::Get_GameObject(const wstring pLayerTag, const wstring pObjTag)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_GameObject(pLayerTag, pObjTag);
}

Engine::CComponent* Engine::CManagement::Get_Component(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_Component(pLayerTag, pObjTag, pComponentTag, eID);
}

const SCENE_ID & CManagement::Get_SceneID()
{
	if (nullptr == m_pScene)
		return SCENE_END;

	return m_pScene->Get_SceneID();
}

const map<const wstring, CLayer*>& CManagement::Get_MapLayer()
{
	if (nullptr == m_pScene)
		return map<const wstring, CLayer*>();

	return m_pScene->Get_MapLayer();
}

CLayer * CManagement::Get_Layer(const wstring wstrLayerTag)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_Layer(wstrLayerTag);
}

const map<const wstring, CGameObject*>& CManagement::Get_MapObject(const wstring wstrLayerTag)
{
	if (nullptr == m_pScene)
		return map<const wstring, CGameObject*>();
	
	if (nullptr == Get_Layer(wstrLayerTag))
	{
		return map<const wstring, CGameObject*>();
	}
		

	return Get_Layer(wstrLayerTag)->Get_MapObject();
}

HRESULT CManagement::Set_RenderColType(const wstring pColName, COLTYPE eColType, COLLIDERTYPE eColliderType)
{
	CCollider*	pCollider = nullptr;
	CBoxCollider*	pBoxCollider = nullptr;

	switch (eColliderType)
	{
	case Engine::COLTYPE_BOX_DAMAGED:
		pBoxCollider = dynamic_cast<CBoxCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", pColName, ID_STATIC));

		if (!pBoxCollider)
			return E_FAIL;

		pBoxCollider->Set_RenderColType(eColType);
		break;

	case Engine::COLTYPE_BOX_HIT:
		pBoxCollider = dynamic_cast<CBoxCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", pColName, ID_STATIC));

		if (!pBoxCollider)
			return E_FAIL;

		pBoxCollider->Set_RenderColType(eColType);

		break;

	case Engine::COLTYPE_SPHERE_DAMAGED:
		pCollider = dynamic_cast<CCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", pColName, ID_STATIC));

		if (!pCollider)
			return E_FAIL;

		pCollider->Set_RenderColType(eColType);
		break;

	case Engine::COLTYPE_SPHERE_HIT:
		pCollider = dynamic_cast<CCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", pColName, ID_STATIC));

		if (!pCollider)
			return E_FAIL;

		pCollider->Set_RenderColType(eColType);

		break;

	default:
		pCollider = dynamic_cast<CCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", pColName, ID_STATIC));

		if (!pCollider)
			return E_FAIL;

		pCollider->Set_RenderColType(eColType);
		break;
	}

	return S_OK;
}

