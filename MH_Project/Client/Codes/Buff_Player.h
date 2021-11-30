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

class CPlayer_SlotUI;

class CBuff_Player abstract : public CUI
{
protected:
	explicit CBuff_Player(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBuff_Player();

public:
	virtual HRESULT			Ready_Object();
	virtual HRESULT			LateReady_Object() override;
	virtual _int			Update_Object(const _float& fTimeDelta) override;
	virtual _int			LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void			Render_Object() override;

public:
	const	_float&			Get_PosX() { return m_fX; }
	const	_float&			Get_PosY() { return m_fY; }

public:
	void					Set_BuffIcon(_bool bValue) { m_bBuffOn = bValue; m_fValueRatio = 1.f; }

protected:
	void					Blink_Icon(const _float& fTimeDelta);

protected:
	_bool					m_bBuffOn = false;

	_uint					m_iBlinkOneCycle = 0;

	_float					m_fAlphaInterpol = 0.95f;

	CPlayer_SlotUI*			m_pSlotUI = nullptr;

public:
	virtual void			Free() override;
};
