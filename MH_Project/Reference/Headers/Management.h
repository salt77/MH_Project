#ifndef Management_h__
#define Management_h__

#include "Scene.h"

BEGIN(Engine)

class ENGINE_DLL CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit CManagement(void);
	virtual ~CManagement(void);

	// MFC
public:
	HRESULT			AddGameObjectInManager(const wstring LayerTag, CLayer* pLayer);
	HRESULT			DeleteGameObjectInManager(const wstring LayerTag);
	CGameObject*	Get_MFCGameObject(const wstring pLayerTag, const wstring pObjTag);
	CComponent*		Get_MFCComponent(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID);
	HRESULT			Set_RenderColType(const wstring pColName, COLTYPE eColType, COLLIDERTYPE eColliderType = COLLIDERTYPE::COLTYPE_SPHERE_DAMAGED);

	// Client
public:
	HRESULT			Ready_Shader(LPDIRECT3DDEVICE9& pGraphicDev);

public:
	CGameObject*	Get_GameObject(const wstring pLayerTag, const wstring pObjTag);
	CComponent*		Get_Component(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID);
	const SCENE_ID&	Get_SceneID();
	const map<const wstring, CLayer*>&	Get_MapLayer();
	CLayer*	Get_Layer(const wstring wstrLayerTag);
	const map<const wstring, CGameObject*>&	Get_MapObject(const wstring wstrLayerTag);

public:
	HRESULT			Set_Scene(CScene* pScene);
	void			Emplace_Layer(const wstring wstrLayerTag, CLayer* pLayer);
	void			Delete_Layer(const wstring wstrLayerTag, const wstring wstrObjTag);
	void			Delete_AllInLayer(const wstring wstrLayerTag);

public:
	_int			Update_Scene(const _float& fTimeDelta);
	_int			LateUpdate_Scene(const _float& fTimeDelta);
	void			Render_Scene(LPDIRECT3DDEVICE9& pGraphicDev);

private:
	CScene*			m_pScene = nullptr;

public:
	virtual void Free(void);

};

END
#endif // Management_h__
