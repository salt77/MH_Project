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
	CGameObject*	Get_GameObject(const wstring pLayerTag, const wstring pObjTag);
	CComponent*		Get_Component(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID);

public:
	virtual HRESULT		Ready_Scene(void);
	virtual HRESULT		LateReady_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		Render_Scene(void)PURE;

protected:
	LPDIRECT3DDEVICE9				m_pGraphicDev;
	map<const wstring, CLayer*>		m_mapLayer;

public:
	virtual void Free(void);
};

END
#endif // Scene_h__
