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

class CTrail_Cloyan final : public CGameObject
{
private:
	explicit CTrail_Cloyan(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTrail_Cloyan();

public:
	virtual HRESULT Ready_Object();
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

protected:
	HRESULT				Add_Component();
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	void		Set_InfoForTrail(const _float& fDeltaTime, const _vec3& vWeaponLower, const _vec3& vWeaponUpper, const _matrix* pMatWeapon)
	{
		m_fDeltaTime = fDeltaTime;
		m_vWeaponLower = vWeaponLower;
		m_vWeaponUpper = vWeaponUpper;
		m_pMatWeapon = pMatWeapon;
	}
	void		Set_Render(_bool bValue) { m_bRender = bValue; }
	void		Set_RenderAlpha(_float fAlpha) { m_fAlphaValue = fAlpha; }

private:
	_bool		m_bRender = false;

	_float		m_fDeltaTime = 0.f;
	_float		m_fAlphaValue = 1.f;

	_vec3		m_vWeaponLower;
	_vec3		m_vWeaponUpper;

	const _matrix*	m_pMatWeapon = nullptr;

	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	CTrail*			m_pTrailCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;

public:
	static CTrail_Cloyan*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free();
};
