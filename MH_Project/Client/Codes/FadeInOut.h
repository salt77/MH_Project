#pragma once

#include "UI.h"

class CFadeInOut : public CUI
{
private:
	explicit CFadeInOut(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFadeInOut();

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY, _float fStartAlpha = 1.f);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

protected:
	HRESULT				Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	void		AhglanFadeOut(const _float& fTimeDelta);
	void		Set_AhglanFadeOut() { m_bAhglanFadeOut = true; }

private:
	_bool		m_bAhglanFadeOut = false;
	
	_uint		m_iFadeCount = 0;

public:
	static CFadeInOut*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY, _float fStartAlpha = 1.f);
	virtual void		Free(void) override;
};

