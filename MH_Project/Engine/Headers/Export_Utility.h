#ifndef Export_Utility_h__
#define Export_Utility_h__

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
#include "LightMgr.h"

#include "Camera.h"

BEGIN(Engine)


// Management
inline HRESULT		AddGameObjectInManager(const wstring LayerTag, CLayer* pLayer);
inline HRESULT		DeleteGameObjectInManager(const wstring LayerTag);
inline CGameObject*	Get_MFCGameObject(const wstring pLayerTag, const wstring pObjTag);
inline CComponent*	Get_MFCComponent(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID);
inline HRESULT		Set_RenderColType(const wstring pColType, COLTYPE eColType);

inline CGameObject*	Get_GameObject(const wstring pLayerTag, const wstring pObjTag);
inline CComponent*	Get_Component(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID);
inline HRESULT		Create_Management(CManagement** ppManagement);
inline HRESULT		Set_Scene(CScene* pScene);
inline _int			Update_Scene(const _float& fTimeDelta);
inline void			Render_Scene(LPDIRECT3DDEVICE9& pGraphicDev);

// ProtoMgr
inline HRESULT		Ready_Prototype(const wstring pProtoTag, CComponent* pInstance);
inline CComponent*	Clone_Prototype(const wstring pProtoTag);
inline HRESULT		Delete_Prototype(const wstring pProtoTag);

// Renderer
inline CRenderer*	Get_Renderer(void);
inline void			Add_RenderGroup(RENDERID eID, CGameObject* pGameObject);
inline void			Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
inline void			Clear_RenderGroup(void);

// LightMgr
inline HRESULT		Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,
								const D3DLIGHT9* pLightInfo,
								const _uint& iIndex);

inline void			Release_Utility(void);

#include "Export_Utility.inl"

END
#endif // Export_Utility_h__
