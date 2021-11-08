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

class CBoss_Hpbar_YellowUI final : public CUI
{
private:
	explicit CBoss_Hpbar_YellowUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBoss_Hpbar_YellowUI(void);

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY, _int iHpType);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

protected:
	HRESULT				Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	void				Lerp();

public:
	void				Set_LineHpRatio(_float fLineHpRatio) { m_fLineHpRatio = fLineHpRatio; }
	void				Set_MaxLineHpRatio(_float fMaxLineHpRatio) { m_fFullLineHpRatio = fMaxLineHpRatio; }

private:
	_int				m_iHpType;

	_float				m_fLineHpRatio = 1.f;
	_float				m_fFullLineHpRatio = 1.f;

	_float				m_fLerpValue = 0.f;

public:
	static CBoss_Hpbar_YellowUI*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY, _int iHpType);
	virtual void	Free(void);
};
