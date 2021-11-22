#pragma once

#include "UI.h"

#include "Define.h"

BEGIN(Engine)

class CTrail;
class CTexture;
class CRenderer;
class CTransform;
class CShader;

END

class CTrail_Sword final : public CGameObject
{
private:
	explicit CTrail_Sword(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTrail_Sword(void);

public:
	virtual HRESULT Ready_Object();
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

protected:
	HRESULT				Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	void		Set_InfoForTrail(const _float& fDeltaTime, const _vec3& vWeaponLower, const _vec3& vWeaponUpper, const _matrix* pMatWeapon/*, _ulong dwMaxVtxCnt = 30*/)
	{
		m_fDeltaTime = fDeltaTime;
		m_vWeaponLower = vWeaponLower;
		m_vWeaponUpper = vWeaponUpper;
		m_pMatWeapon = pMatWeapon;
		//m_dwMaxVtxCnt = dwMaxVtxCnt;
	}
	void		Set_Render(_bool bValue) { m_bRender = bValue; }

private:
	_bool		m_bRender = false;

	_ulong		m_dwMaxVtxCnt = 15;

	_float		m_fDeltaTime = 0.f;

	_vec3		m_vWeaponLower;
	_vec3		m_vWeaponUpper;

	const _matrix*	m_pMatWeapon = nullptr;

	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	CTrail*			m_pTrailCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;

public:
	static CTrail_Sword*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void	Free(void);
};
