#include "TrailBuffer.h"


CTrailBuffer::CTrailBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CTrailBuffer::CTrailBuffer(const CTrailBuffer & rhs)
	: CVIBuffer(rhs)
{
}

CTrailBuffer::~CTrailBuffer()
{
}


CTrailBuffer * CTrailBuffer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTrailBuffer*	pInstance = new CTrailBuffer(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CTrailBuffer::Clone(void)
{
	return new CTrailBuffer(*this);
}

void CTrailBuffer::Free()
{
	CVIBuffer::Free();
}
