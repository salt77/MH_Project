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
	HRESULT			Ready_Collider(/*const _vec3* pPos, const _ulong& dwVtxCnt, const _ulong& dwStride, */const _float& fRadius);
	void			Render_Collider(const _matrix* pColliderMatrix);

private:
	_matrix			m_matColMatrix;
	LPD3DXMESH		m_pSphere;

#ifdef _DEBUG
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;
	LPDIRECT3DTEXTURE9			m_pTexture;

#endif

public:
	static CCollider*		Create(LPDIRECT3DDEVICE9 pGraphicDev/*, const _vec3* pPos, const _ulong& dwVtxCnt, const _ulong& dwStride*/, const _float& fRadius);
	virtual CComponent*		Clone();
	virtual void			Free();
};

END