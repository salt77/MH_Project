#ifndef Scene_h__
#define Scene_h__

#include "Layer.h"

BEGIN(Engine)

class ENGINE_DLL CScene : public CBase
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene(void);

public:
	CGameObject*		Get_GameObject(const wstring pLayerTag, const wstring pObjTag);
	CComponent*			Get_Component(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID);
	const SCENE_ID&		Get_SceneID() { return m_eSceneID; }

public:
	virtual HRESULT		Ready_Scene(void);
	virtual HRESULT		LateReady_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual _int		LateUpdate_Scene(const _float& fTimeDelta);
	virtual void		Render_Scene(void) PURE;

public:
	virtual	void		Emplace_Layer(const wstring wstrLayerTag, CLayer* pLayer) { m_mapLayer.emplace(wstrLayerTag, pLayer); }

protected:
	LPDIRECT3DDEVICE9				m_pGraphicDev;
	map<const wstring, CLayer*>		m_mapLayer;
	SCENE_ID						m_eSceneID = SCENE_END;

public:
	virtual void Free(void);
};

END
#endif // Scene_h__
