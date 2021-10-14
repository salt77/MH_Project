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
public:
	HRESULT			AddGameObjectInManager(const _tchar* LayerTag, CLayer* pLayer);
	HRESULT			DeleteGameObjectInManager(const _tchar* LayerTag);
	CGameObject*	Get_MFCGameObject(const _tchar* pLayerTag, const _tchar* pObjTag);
	CGameObject*	Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag);
	CComponent*		Get_Component(const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID);

public:
	HRESULT			Set_Scene(CScene* pScene);
	_int			Update_Scene(const _float& fTimeDelta);
	void			Render_Scene(LPDIRECT3DDEVICE9& pGraphicDev);

private:
	CScene*			m_pScene = nullptr;

public:
	virtual void Free(void);

};

END
#endif // Management_h__
