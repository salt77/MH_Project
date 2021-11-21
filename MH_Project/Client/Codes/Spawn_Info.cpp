#include "stdafx.h"
#include "Spawn_Info.h"

#include "Export_Function.h"

CSpawn_Info::CSpawn_Info(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CSpawn_Info::CSpawn_Info(const CSpawn_Info& rhs)
	: CGameObject(rhs)
{
}

CSpawn_Info::~CSpawn_Info(void)
{
}


HRESULT CSpawn_Info::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);

	return S_OK;
}

HRESULT CSpawn_Info::LateReady_Object()
{
	FAILED_CHECK_RETURN(CGameObject::LateReady_Object(), E_FAIL);

	return S_OK;
}

_int CSpawn_Info::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	//Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

_int CSpawn_Info::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	return iExit;
}

void CSpawn_Info::Render_Object(void)
{
}

void CSpawn_Info::Free(void)
{
	CGameObject::Free();
}

