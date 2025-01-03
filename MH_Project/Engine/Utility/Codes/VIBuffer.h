#ifndef VIBuffer_h__
#define VIBuffer_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer :public CComponent
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer(void);

public:
	const _ulong&			Get_TriCnt(void) { return m_dwTriCnt; }

public:
	virtual HRESULT			Ready_Buffer(void);
	virtual void			Render_Buffer(void);

protected:
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;

	_ulong						m_dwVtxCnt;
	_ulong						m_dwVtxCntX;
	_ulong						m_dwVtxCntZ;
	_ulong						m_dwVtxSize;
	_ulong						m_dwTriCnt;
	_ulong						m_dwFVF;

	_ulong						m_dwIdxSize;
	D3DFORMAT					m_IdxFmt;

public:
	virtual void	Free(void);
};


END
#endif // VIBuffer_h__
