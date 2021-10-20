#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class CNavmesh : public CVIBuffer
{
public:
	explicit CNavmesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CNavmesh(const CNavmesh& rhs);
	virtual ~CNavmesh();

public:
	const _vec3*		Get_VtxPos(void) const { return m_pPos; }

public:
	HRESULT		 Ready_Buffer(const _vec3& fFirstIndex, const _vec3& fSecondIndex, const _vec3& fThirdIndex);
	virtual void Render_Buffer(void) override;

public:
	static	CVIBuffer*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& fFirstIndex, const _vec3& fSecondIndex, const _vec3& fThirdIndex);
	virtual CComponent* Clone(void) override;
	virtual void		Free() override;

private:
	_vec3*		m_pPos = nullptr;
};

END