#pragma once

#include "UI.h"

class CLoadingBar_Progress : public CUI
{
private:
	explicit CLoadingBar_Progress(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoadingBar_Progress();

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

protected:
	HRESULT				Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	static CLoadingBar_Progress*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void		Free(void) override;
};

