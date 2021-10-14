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

#include "Camera.h"

BEGIN(Engine)


// Management
inline HRESULT		AddGameObjectInManager(const _tchar* LayerTag, CLayer* pLayer);
inline CGameObject*	Get_MFCGameObject(const _tchar* pLayerTag, const _tchar* pObjTag);
inline CGameObject*	Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag);
inline CComponent*	Get_Component(const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID);
inline HRESULT		Create_Management(CManagement** ppManagement);
inline HRESULT		Set_Scene(CScene* pScene);
inline _int			Update_Scene(const _float& fTimeDelta);
inline void			Render_Scene(LPDIRECT3DDEVICE9& pGraphicDev);

// ProtoMgr
inline HRESULT		Ready_Prototype(const _tchar* pProtoTag, CComponent* pInstance);
inline CComponent*	Clone_Prototype(const _tchar* pProtoTag);
inline HRESULT		Delete_Prototype(const _tchar* pProtoTag);

// Renderer
inline CRenderer*	Get_Renderer(void);
inline void			Add_RenderGroup(RENDERID eID, CGameObject* pGameObject);
inline void			Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
inline void			Clear_RenderGroup(void);

inline void			Release_Utility(void);

#include "Export_Utility.inl"

END
#endif // Export_Utility_h__
