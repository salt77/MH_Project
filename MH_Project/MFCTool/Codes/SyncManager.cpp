#include "stdafx.h"
#include "SyncManager.h"

IMPLEMENT_SINGLETON(CSyncManager);

CSyncManager::CSyncManager()
{
}

CSyncManager::~CSyncManager()
{
	Free();
}


void CSyncManager::Set_WireFrameSync(_int iRowX, _int iColZ, _int iInterval, _int iDetail)
{
	m_iTerrainInfo[0] = iRowX;
	m_iTerrainInfo[1] = iColZ;
	m_iTerrainInfo[2] = iInterval;
	m_iTerrainInfo[3] = iDetail;
}

void CSyncManager::Free(void)
{
}
