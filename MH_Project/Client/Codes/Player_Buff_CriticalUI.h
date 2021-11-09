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

class CPlayer_Buff_CriticalUI final : public CUI
{
private:
	explicit CPlayer_Buff_CriticalUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer_Buff_CriticalUI(void);

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

protected:
	HRESULT				Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	
private:
	void				FadeInOut();

private:
	_bool				m_bOneTimeCheck = false;
	_bool				m_bFadeInOutCheck = false;

public:
	static CPlayer_Buff_CriticalUI*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void		Free(void);
};
