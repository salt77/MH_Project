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

class CDamage_FontUI final : public CUI
{
private:
	explicit CDamage_FontUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDamage_FontUI(void);

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY, DAMAGEFONT_ID eID, _uint iDamage);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

protected:
	HRESULT				Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	_uint				m_iDamage = 0;

	DAMAGEFONT_ID		m_eFontID = DAMAGEFONT_END;

public:
	static CDamage_FontUI*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY, DAMAGEFONT_ID eID, _uint iDamage);
	virtual void		Free(void);
};
