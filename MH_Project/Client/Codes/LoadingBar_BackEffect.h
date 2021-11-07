#pragma once

#include "UI.h"

class CLoadingBar_BackEffect : public CUI
{
private:
	explicit CLoadingBar_BackEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoadingBar_BackEffect();

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

protected:
	HRESULT				Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	static CLoadingBar_BackEffect*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void Free(void) override;
};

