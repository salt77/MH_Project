#pragma once

#include "Base.h"
#include "Export_Function.h"

USING(Engine)

class CEffectMgr : public CBase
{
	DECLARE_SINGLETON(CEffectMgr)

private:
	CEffectMgr();
	virtual ~CEffectMgr();

public:
	HRESULT		Ready_EffectMgr();
	_uint		Update_EffectMgr();

public:
	void		Enable_SlashPoint();

public:
	virtual void Free() override;
};

