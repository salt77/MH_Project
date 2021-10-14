#pragma once

#include "Engine_Define.h"
#include "Engine_Macro.h"

#include "Base.h"

BEGIN(Engine)

class CGameObject;
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
	CGameObject*	Get_MFCGameObject(const _tchar* pLayerTag, const _tchar* pObjTag);

public:
	HRESULT	AddGameObjectInManager(const _tchar* LayerTag, CLayer* pLayer);
	HRESULT	DeleteGameObjectInManager(const _tchar* LayerTag);

public:
	virtual void Free();

private:
	map<const _tchar*, CLayer*>		m_mapMFCLayer;
};

END