#include "stdafx.h"
#include "DataMgr.h"

IMPLEMENT_SINGLETON(CDataMgr)

CDataMgr::CDataMgr()
{
	ZeroMemory(&m_tPlayerInfo, sizeof(PL_INFO));
}

CDataMgr::~CDataMgr()
{
	Free();
}


HRESULT CDataMgr::Ready_EffectMgr()
{
	return S_OK;
}

void CDataMgr::Set_PlayerData(PL_INFO tPlayerInfo)
{
	m_tPlayerInfo = tPlayerInfo;
}

void CDataMgr::Free()
{
}
