#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	//explicit CCollider(const CCollider& rhs);
	virtual ~CCollider();

public:
	//const _vec3*		Get_Min() { return &m_vMin; }
	//const _vec3*		Get_Max() { return &m_vMax; }
	const _matrix*		Get_ColliderWorld() { return &m_matColMatrix; }

public:
	void	Set_RenderColType(COLTYPE _eColType) { m_eColType = _eColType; }
	void	Set_Matrix(_matrix* matInfo) { m_matColParts = matInfo; *m_matColParts *= 0.01f; }
	void	Set_MatrixInterpolX(_float fX) { m_fInterpolX = fX; }
	void	Set_MatrixInterpolY(_float fY) { m_fInterpolY = fY; }
	void	Set_MatrixInterpolZ(_float fZ) { m_fInterpolZ = fZ; }

public:
	HRESULT			Ready_Collider(/*const _vec3* pPos, const _ulong& dwVtxCnt, const _ulong& dwStride, */const _float& fRadius);
	void			Render_Collider(COLTYPE eType, const _matrix* pColliderMatrix);

private:
	_matrix			m_matColMatrix;
	_matrix*		m_matColParts = nullptr;

	_float			m_fInterpolX = 0.f;
	_float			m_fInterpolY = 0.f;
	_float			m_fInterpolZ = 0.f;

	LPD3DXMESH		m_pSphere;

#ifdef _DEBUG
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;
	LPDIRECT3DTEXTURE9			m_pTexture[COLTYPE::COL_END];

	COLTYPE						m_eColType = COL_FALSE;

#endif

public:
	static CCollider*		Create(LPDIRECT3DDEVICE9 pGraphicDev/*, const _vec3* pPos, const _ulong& dwVtxCnt, const _ulong& dwStride*/, const _float& fRadius);
	virtual CComponent*		Clone();
	virtual void			Free();
};

END