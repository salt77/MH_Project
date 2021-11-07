#pragma once

#include "UI.h"

#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CRenderer;
class CTransform;
class CShader;

END

class CBoss_Hpbar_FontUI final : public CUI
{
private:
	explicit CBoss_Hpbar_FontUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBoss_Hpbar_FontUI(void);

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY, _bool	bFontX);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

protected:
	HRESULT				Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	_bool				m_bIsFontX = false;

public:
	static CBoss_Hpbar_FontUI*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY, _bool	bFontX);
	virtual void		Free(void);
};
