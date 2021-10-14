#pragma once

#include "Engine_Define.h"
#include "Engine_Macro.h"

#include "Base.h"

BEGIN(Engine)

class CLayer;

class ENGINE_DLL CGameObjectManager final : public CBase
{
	DECLARE_SINGLETON(CGameObjectManager);

private:
	explicit CGameObjectManager();
	virtual ~CGameObjectManager();

public:
	HRESULT		Ready_Manager();
	_int		Update_Objects(const _float& fTimeDelta);


public:
	HRESULT	AddGameObjectInManager(const _tchar* LayerTag, CLayer* pLayer);

public:
	virtual void Free();

private:
	map<const _tchar*, CLayer*>		m_mapLayer;
};

END