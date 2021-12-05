#pragma once

#include "Base.h"
#include "Export_Function.h"

USING(Engine)

class CDataMgr : public CBase
{
	DECLARE_SINGLETON(CDataMgr)

private:
	CDataMgr();
	virtual ~CDataMgr();

public:
	HRESULT		Ready_EffectMgr();

public:
	PL_INFO		Get_PlayerInfo() { return m_tPlayerInfo; }

public:
	void		Set_PlayerData(PL_INFO tPlayerInfo);

private:
	PL_INFO		m_tPlayerInfo;

public:
	virtual void Free() override;
};

