#pragma once

#include "Engine_Define.h"
#include "Engine_Macro.h"

#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CComponent;
class CLayer;

class ENGINE_DLL CMFC_ObjectManager final : public CBase
{
	DECLARE_SINGLETON(CMFC_ObjectManager);

private:
	explicit CMFC_ObjectManager();
	virtual ~CMFC_ObjectManager();

//public:
//	HRESULT		Ready_Manager();
//	_int		Update_Objects(const _float& fTimeDelta);

public:
	CGameObject*	Get_MFCGameObject(const wstring pLayerTag, const wstring pObjTag);
	CComponent*		Get_MFCComponent(const wstring pLayerTag, const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID);

public:
	HRESULT	AddGameObjectInManager(const wstring LayerTag, CLayer* pLayer);
	HRESULT	DeleteGameObjectInManager(const wstring LayerTag);

public:
	virtual void Free();

private:
	map<const wstring, CLayer*>		m_mapMFCLayer;
};

END