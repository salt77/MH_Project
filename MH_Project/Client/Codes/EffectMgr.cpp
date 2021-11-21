#include "stdafx.h"
#include "EffectMgr.h"

IMPLEMENT_SINGLETON(CEffectMgr)

CEffectMgr::CEffectMgr()
{
}

CEffectMgr::~CEffectMgr()
{
}


HRESULT CEffectMgr::Ready_EffectMgr()
{
	return S_OK;
}

_uint CEffectMgr::Update_EffectMgr()
{
	return _uint();
}

void CEffectMgr::Enable_SlashPoint()
{

}

void CEffectMgr::Free(void)
{
}
