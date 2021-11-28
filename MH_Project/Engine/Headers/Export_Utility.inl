#include "Export_Utility.h"

inline HRESULT AddGameObjectInManager(const wstring LayerTag, CLayer * pLayer)
{
	CManagement::GetInstance()->AddGameObjectInManager(LayerTag, pLayer);

	return S_OK;
}
inline HRESULT DeleteGameObjectInManager(const wstring LayerTag)
{
	CManagement::GetInstance()->DeleteGameObjectInManager(LayerTag);

	return S_OK;
}
inline CGameObject * Get_MFCGameObject(const wstring pLayerTag, const wstring pObjTag)
{
	return CManagement::GetInstance()->Get_MFCGameObject(pLayerTag, pObjTag);
}
inline CComponent * Get_MFCComponent(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID)
{
	return CManagement::GetInstance()->Get_MFCComponent(pLayerTag, pObjTag, pComponentTag, eID);
}
inline CGameObject * Get_GameObject(const wstring pLayerTag, const wstring pObjTag)
{
	return CManagement::GetInstance()->Get_GameObject(pLayerTag, pObjTag);
}
inline HRESULT Set_RenderColType(const wstring pColType, COLTYPE eColType, COLLIDERTYPE eColliderType)
{
	CManagement::GetInstance()->Set_RenderColType(pColType, eColType, eColliderType);

	return S_OK;
}
CComponent*		Get_Component(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID)
{
	return CManagement::GetInstance()->Get_Component(pLayerTag, pObjTag, pComponentTag, eID);
}

inline const SCENE_ID & Get_SceneID()
{
	return CManagement::GetInstance()->Get_SceneID();
}

inline const map<const wstring, CLayer*>& Get_MapLayer()
{
	return CManagement::GetInstance()->Get_MapLayer();
}

inline CLayer * Get_Layer(const wstring wstrLayerTag)
{
	return CManagement::GetInstance()->Get_Layer(wstrLayerTag);
}

inline const map<const wstring, CGameObject*>& Get_MapObject(const wstring wstrLayerTag)
{
	return CManagement::GetInstance()->Get_MapObject(wstrLayerTag);
}

HRESULT		Create_Management(LPDIRECT3DDEVICE9& pGraphicDev, CManagement** ppManagement)
{
	CManagement*		pManagement = CManagement::GetInstance();
	NULL_CHECK_RETURN(pManagement, E_FAIL);

	FAILED_CHECK_RETURN(pManagement->Ready_Shader(pGraphicDev), E_FAIL);

	*ppManagement = pManagement;

	return S_OK;
}

HRESULT		Set_Scene(CScene* pScene)
{
	return CManagement::GetInstance()->Set_Scene(pScene);
}
_int			Update_Scene(const _float& fTimeDelta)
{
	CManagement::GetInstance()->Update_Scene(fTimeDelta);
}
void			Render_Scene(LPDIRECT3DDEVICE9& pGraphicDev)
{
	CManagement::GetInstance()->Render_Scene(pGraphicDev);
}

inline _float Random(_float _fMin, _float _fMax)
{
	random_device rd;

	mt19937 engine(rd());						// MT19937 난수 엔진
	uniform_real_distribution<_float> distribution(_fMin, _fMax);    // 생성 범위
	auto generator = bind(distribution, engine);

	return generator();
}

inline void Emplace_Layer(const wstring wstrLayerTag, CLayer * pLayer)
{
	CManagement::GetInstance()->Emplace_Layer(wstrLayerTag, pLayer);
}

inline void Delete_Layer(const wstring wstrLayerTag, const wstring wstrObjTag)
{
	CManagement::GetInstance()->Delete_Layer(wstrLayerTag, wstrObjTag);
}

inline void Delete_AllInLayer(const wstring wstrLayerTag)
{
	CManagement::GetInstance()->Delete_AllInLayer(wstrLayerTag);
}

HRESULT		Ready_Prototype(const wstring pProtoTag, CComponent* pInstance)
{
	return CProtoMgr::GetInstance()->Ready_Prototype(pProtoTag, pInstance);
}
CComponent* Clone_Prototype(const wstring pProtoTag)
{
	return CProtoMgr::GetInstance()->Clone_Prototype(pProtoTag);
}

inline HRESULT Delete_Prototype(const wstring pProtoTag)
{
	CProtoMgr::GetInstance()->Delete_Prototype(pProtoTag);

	return S_OK;
}

CRenderer*		Get_Renderer(void)
{
	return CRenderer::GetInstance();
}

void		Add_RenderGroup(RENDERID eID, CGameObject* pGameObject)
{
	CRenderer::GetInstance()->Add_RenderGroup(eID, pGameObject);
}
void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	CRenderer::GetInstance()->Render_GameObject(pGraphicDev);
}
void		Clear_RenderGroup(void)
{
	CRenderer::GetInstance()->Clear_RenderGroup();
}


inline HRESULT Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9 * pLightInfo, const _uint & iIndex)
{
	return CLightMgr::GetInstance()->Ready_Light(pGraphicDev, pLightInfo, iIndex);
}

inline void Render_Light(LPD3DXEFFECT & pEffect)
{
	CLightMgr::GetInstance()->Render_Light(pEffect);
}

inline const D3DLIGHT9 * Get_Light(const _uint & iIndex)
{
	return CLightMgr::GetInstance()->Get_Light(iIndex);
}

inline HRESULT Ready_RenderTarget(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pTargetTag, const _uint & iWidth, const _uint & iHeight, D3DFORMAT Format, D3DXCOLOR Color)
{
	return CRenderTargetMgr::GetInstance()->Ready_RenderTarget(pGraphicDev, pTargetTag, iWidth, iHeight, Format, Color);
}

inline HRESULT Ready_MRT(const wstring pMRTTag, const wstring pTargetTag)
{
	return CRenderTargetMgr::GetInstance()->Ready_MRT(pMRTTag, pTargetTag);
}

inline HRESULT Begin_MRT(const wstring pMRTTag)
{
	return CRenderTargetMgr::GetInstance()->Begin_MRT(pMRTTag);
}

inline HRESULT End_MRT(const wstring pMRTTag)
{
	return CRenderTargetMgr::GetInstance()->End_MRT(pMRTTag);
}

inline HRESULT Ready_DebugBuffer(const wstring pTargetTag, const _float & fX, const _float & fY, const _float & fSizeX, const _float & fSizeY)
{
	return CRenderTargetMgr::GetInstance()->Ready_DebugBuffer(pTargetTag, fX, fY, fSizeX, fSizeY);
}

inline void Render_DebugBuffer(const wstring pMRTTag)
{
	CRenderTargetMgr::GetInstance()->Render_DebugBuffer(pMRTTag);
}

inline void Get_RenderTargetTexture(LPD3DXEFFECT & pEffect, const wstring pTargetTag, const char * pConstantTable)
{
	CRenderTargetMgr::GetInstance()->Get_RenderTargetTexture(pEffect, pTargetTag, pConstantTable);
}


void	Release_Utility(void)
{
	CRenderTargetMgr::GetInstance()->DestroyInstance();
	CLightMgr::GetInstance()->DestroyInstance();
	CRenderer::GetInstance()->DestroyInstance();
	CProtoMgr::GetInstance()->DestroyInstance();
	CManagement::GetInstance()->DestroyInstance();
}