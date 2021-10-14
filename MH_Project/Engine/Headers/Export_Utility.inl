#include "Export_Utility.h"
inline HRESULT AddGameObjectInManager(const _tchar * LayerTag, CLayer * pLayer)
{
	CManagement::GetInstance()->AddGameObjectInManager(LayerTag, pLayer);

	return S_OK;
}
inline HRESULT DeleteGameObjectInManager(const _tchar * LayerTag)
{
	CManagement::GetInstance()->DeleteGameObjectInManager(LayerTag);

	return S_OK;
}
inline CGameObject * Get_MFCGameObject(const _tchar * pLayerTag, const _tchar * pObjTag)
{
	return CManagement::GetInstance()->Get_MFCGameObject(pLayerTag, pObjTag);
}
inline CGameObject * Get_GameObject(const _tchar * pLayerTag, const _tchar * pObjTag)
{
	return CManagement::GetInstance()->Get_GameObject(pLayerTag, pObjTag);
}
CComponent*		Get_Component(const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID)
{
	return CManagement::GetInstance()->Get_Component(pLayerTag, pObjTag, pComponentTag, eID);
}

HRESULT		Create_Management(CManagement** ppManagement)
{
	CManagement*		pManagement = CManagement::GetInstance();
	NULL_CHECK_RETURN(pManagement, E_FAIL);

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

HRESULT		Ready_Prototype(const _tchar* pProtoTag, CComponent* pInstance)
{
	return CProtoMgr::GetInstance()->Ready_Prototype(pProtoTag, pInstance);
}
CComponent* Clone_Prototype(const _tchar* pProtoTag)
{
	return CProtoMgr::GetInstance()->Clone_Prototype(pProtoTag);
}

inline HRESULT Delete_Prototype(const _tchar * pProtoTag)
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


void			Release_Utility(void)
{
	CRenderer::GetInstance()->DestroyInstance();
	CProtoMgr::GetInstance()->DestroyInstance();
	CManagement::GetInstance()->DestroyInstance();
}