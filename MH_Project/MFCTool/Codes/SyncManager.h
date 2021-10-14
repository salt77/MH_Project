#pragma once

#include "Base.h"

class CSyncManager final : public CBase
{
	DECLARE_SINGLETON(CSyncManager);

private:
	explicit CSyncManager();
	virtual ~CSyncManager();

public:
	RENDERTYPE_MFC	Get_TerrainRenderType() { return m_eRenderType; }
	_int*			Get_TerrainInfo() { return m_iTerrainInfo; }

public:
	void	Set_TerrainRenderSync(RENDERTYPE_MFC _eRenderType) { m_eRenderType = _eRenderType; }
	void	Set_WireFrameSync(_int iRowX, _int iColZ, _int iInterval, _int iDetail);

private:
	// TerrainTool
	RENDERTYPE_MFC m_eRenderType = RENDERTYPE_MFC_END;
	_int m_iTerrainInfo[4] = {};

public:
	virtual void Free(void) override;
};