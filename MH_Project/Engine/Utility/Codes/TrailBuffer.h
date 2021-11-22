#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTrailBuffer : public CVIBuffer
{
public:
	explicit CTrailBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTrailBuffer(const CTrailBuffer& rhs);
	virtual ~CTrailBuffer();

public:
	HRESULT			Create_Buffer();

private:
	_ulong			m_dwCreateTime = GetTickCount();
	_ulong			m_dwCreateDelay = 100;

public:
	static	CTrailBuffer*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent *	Clone() override;
	virtual void			Free() override;
};

END