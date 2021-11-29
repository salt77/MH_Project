#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CBalistaTrail : public CVIBuffer
{
public:
	explicit CBalistaTrail(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBalistaTrail(const CBalistaTrail& rhs);
	virtual ~CBalistaTrail();

public:
	virtual HRESULT Ready_Buffer() override;
	virtual void	Render_Buffer() override;

public:
	void		Management_Point();

public:
	void		Set_InfoForTrail(const _float& fDeltaTime, const _vec3& vWeaponLower, const _vec3& vWeaponUpper, const _matrix* pMatWeapon)
	{
		m_fDeltaTime = fDeltaTime;
		m_vWeaponLower = vWeaponLower;
		m_vWeaponUpper = vWeaponUpper;
		m_pMatWeapon = pMatWeapon;

		Management_Point();
	}

private:
	_float		m_fInTime = 0.f;
	_float		m_fDeltaTime = 0.f;

	_vec3		m_vWeaponLower;
	_vec3		m_vWeaponUpper;

	const _matrix*	m_pMatWeapon = nullptr;

	VTXTEX*		m_pVertex = nullptr;

	list<_vec3>	m_listPoint;

public:
	static	CComponent*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent *	Clone() override;
	virtual void			Free() override;
};

END