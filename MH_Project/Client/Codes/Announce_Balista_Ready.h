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

class CAnnounce_Balista_Ready final : public CUI
{
private:
	explicit CAnnounce_Balista_Ready(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAnnounce_Balista_Ready(void);

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

protected:
	HRESULT				Add_Component();
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	void				Set_EnableAnnounce() { m_bAnnounced = true; m_bJustOneTime = false; }
	
private:
	void				FadeInOut(const _float& fTimeDelta);

private:
	_bool				m_bAnnounced = false;
	_bool				m_bJustOneTime = false;

	_ulong				m_dwAnnounceStart = GetTickCount();
	_ulong				m_dwAnnounceDelay = 3000;

public:
	static CAnnounce_Balista_Ready*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void				Free();
};
