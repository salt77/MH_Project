#ifndef Export_Utility_h__
#define Export_Utility_h__

#include <random>

#include "Management.h"
#include "ProtoMgr.h"

#include "TriCol.h"
#include "RcTex.h"
#include "TerrainTex.h"
#include "CubeTex.h"

#include "Texture.h"
#include "Transform.h"
#include "Renderer.h"
#include "Calculator.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "LightMgr.h"
#include "Navimesh.h"
#include "Shader.h"
#include "RenderTargetMgr.h"
#include "Trail.h"
#include "SmashTrail.h"
#include "BalistaTrail.h"

#include "Camera.h"

BEGIN(Engine)


// Management
inline HRESULT		AddGameObjectInManager(const wstring LayerTag, CLayer* pLayer);
inline HRESULT		DeleteGameObjectInManager(const wstring LayerTag);
inline CGameObject*	Get_MFCGameObject(const wstring pLayerTag, const wstring pObjTag);
inline CComponent*	Get_MFCComponent(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID);
inline HRESULT		Set_RenderColType(const wstring pColType, COLTYPE eColType, COLLIDERTYPE eColliderType = COLLIDERTYPE::COLTYPE_SPHERE_DAMAGED);

inline CScene*		Get_CurrentScenePointer();
inline LPDIRECT3DDEVICE9&	Get_CurSceneDevice();
inline CGameObject*	Get_GameObject(const wstring pLayerTag, const wstring pObjTag);
inline CComponent*	Get_Component(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID);
inline const SCENE_ID& Get_SceneID();
inline const map<const wstring, CLayer*>&	Get_MapLayer();
inline CLayer*		Get_Layer(const wstring wstrLayerTag);
inline const map<const wstring, CGameObject*>&	Get_MapObject(const wstring wstrLayerTag);
inline HRESULT		Create_Management(LPDIRECT3DDEVICE9& pGraphicDev, CManagement** ppManagement);
inline HRESULT		Get_Management(CManagement** ppManagement);
inline HRESULT		Set_Scene(CScene* pScene);
inline _int			Update_Scene(const _float& fTimeDelta);
inline void			Render_Scene(LPDIRECT3DDEVICE9& pGraphicDev);
inline void			Release_CurrentScene();
inline HRESULT		Ready_Prototype_Shader(LPDIRECT3DDEVICE9& pGraphicDev);

inline _float		Random(_float _fMin, _float _fMax);

inline void			Emplace_Layer(const wstring wstrLayerTag, CLayer* pLayer);
inline void			Delete_Layer(const wstring wstrLayerTag, const wstring wstrObjTag);
inline void			Delete_AllInLayer(const wstring wstrLayerTag);

// ProtoMgr
inline HRESULT		Ready_Prototype(const wstring pProtoTag, CComponent* pInstance);
inline CComponent*	Clone_Prototype(const wstring pProtoTag);
inline HRESULT		Delete_Prototype(const wstring pProtoTag);
inline HRESULT		Clear_Prototype_ForNextStage();

// Renderer
inline CRenderer*	Get_Renderer(void);
inline void			Add_RenderGroup(RENDERID eID, CGameObject* pGameObject);
inline void			Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
inline void			Clear_RenderGroup(void);

// LightMgr
inline HRESULT			Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,
									const D3DLIGHT9* pLightInfo,
									const _uint& iIndex);
inline void				Render_Light(LPD3DXEFFECT& pEffect);
inline const D3DLIGHT9*	Get_Light(const _uint& iIndex = 0);

// RenderTarget
inline HRESULT		Ready_RenderTarget(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pTargetTag, const _uint& iWidth, const _uint& iHeight, D3DFORMAT Format, D3DXCOLOR Color);
inline HRESULT		Ready_MRT(const wstring pMRTTag, const wstring pTargetTag);
inline HRESULT		Begin_MRT(const wstring pMRTTag);
inline HRESULT		End_MRT(const wstring pMRTTag);

inline HRESULT		Ready_DebugBuffer(const wstring pTargetTag, const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);
inline void			Render_DebugBuffer(const wstring pMRTTag);
inline void			Get_RenderTargetTexture(LPD3DXEFFECT& pEffect, const wstring pTargetTag, const char* pConstantTable);

inline void			Release_Utility(void);

#include "Export_Utility.inl"

END
#endif // Export_Utility_h__
