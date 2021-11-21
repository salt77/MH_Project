#include "stdafx.h"
#include "Effect.h"


CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{
}

CEffect::~CEffect()
{
}


HRESULT CEffect::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);

	return S_OK;
}

HRESULT CEffect::LateReady_Object()
{
	FAILED_CHECK_RETURN(CGameObject::LateReady_Object(), E_FAIL);

	return S_OK;
}

_int CEffect::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	return iExit;
}

_int CEffect::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	return iExit;
}

void CEffect::Render_Object()
{
}


void CEffect::Free()
{
	CGameObject::Free();
}
