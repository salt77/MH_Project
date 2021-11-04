#pragma once

#include "Engine_Define.h"
#include "Engine_Macro.h"

#include "Base.h"

BEGIN(Engine)

class CComponent;
class CGameObject;
class CLayer;
class CScene;

class ENGINE_DLL CGameObjMgr final : public CBase
{
	DECLARE_SINGLETON(CGameObjMgr);

private:
	explicit CGameObjMgr();
	virtual ~CGameObjMgr();

public:
	CGameObject*		Get_ObjInManager(const wstring pLayerTag, const wstring pObjTag);
	CComponent*			Get_ComponentInManager(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID);

public:
	HRESULT		Ready_Manager(void);
	HRESULT		LateReady_Manager();
	_int		Update_Manager(const _float& fTimeDelta);
	_int		LateUpdate_Manager(const _float& fTimeDelta);

public:
	void		Add_ObjInManager(const wstring wstrLayerTag, CLayer* pLayer) { m_mapLayer.emplace(wstrLayerTag, pLayer); }
	HRESULT		DeleteObjInManager(const wstring LayerTag);

private:
	map<const wstring, CLayer*>		m_mapLayer;

public:
	virtual void Free();
};

END