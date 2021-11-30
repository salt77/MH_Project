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

class CTooltip_Stemina final : public CUI
{
private:
	explicit CTooltip_Stemina(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTooltip_Stemina();

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

private:
	HRESULT				Add_Component();
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	void				Set_TooltipOn(_bool bValue) { m_bTooltipOn = bValue; m_dwTooltipStart = GetTickCount(); }

private:
	void				Tooltip_Announce();

private:
	_bool				m_bTooltipOn = false;

	_ulong				m_dwTooltipStart = GetTickCount();
	_ulong				m_dwTooltipDelay = 3000;

public:
	static CTooltip_Stemina*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void				Free();
};
