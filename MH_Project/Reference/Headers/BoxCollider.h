#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CBoxCollider : public CComponent
{
private:
	explicit CBoxCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBoxCollider(const CBoxCollider& rhs);
	virtual ~CBoxCollider(void);

public:
	const _vec3&		Get_Min(void) { return m_vMin; }
	const _vec3&		Get_Max(void) { return m_vMax; }
	const _matrix*		Get_ColliderWorld(void) { return &m_matColMatrix; }
	const wstring		Get_BoneName() { return m_cstrBoneName; }
	const COLLIDERTYPE&	Get_ColldierType() { return m_eColliderType; }

public:
	void	Set_RenderColType(COLTYPE _eColType) { m_eColType = _eColType; }
	void	Set_Matrix(_matrix* matInfo) { m_matColParts = matInfo; *m_matColParts *= 0.01f; }
	void	Set_BoneName(wstring eBoneName) { m_cstrBoneName = eBoneName; }

public:
	HRESULT			Ready_Collider(_float vMinX, _float vMinY, _float vMinZ,
									_float vMaxX, _float vMaxY, _float vMaxZ, COLLIDERTYPE eColliderType);
	void			Render_Collider(COLTYPE eType, const _matrix* pColliderMatrix);

private:
	_vec3			m_vMin, m_vMax;
	_matrix			m_matColMatrix;
	_matrix*		m_matColParts = nullptr;

	wstring			m_cstrBoneName;

	COLLIDERTYPE	m_eColliderType = COLLIDERTYPE::COLTYPE_END;

#ifdef _DEBUG
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;
	LPDIRECT3DTEXTURE9			m_pTexture[COL_END];

	COLTYPE						m_eColType = COL_FALSE;

#endif

public:
	static CBoxCollider*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float vMinX, _float vMinY, _float vMinZ,
									_float vMaxX, _float vMaxY, _float vMaxZ, COLLIDERTYPE eColliderType);
	virtual CComponent*		Clone(void);
	virtual void			Free(void);
};

END