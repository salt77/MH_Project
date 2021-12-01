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

class CPlayer;
class CPlayer_SlotUI;

class CSlot_ItemSkill abstract : public CUI
{
protected:
	explicit CSlot_ItemSkill(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSlot_ItemSkill();

public:
	virtual HRESULT			Ready_Object();
	virtual HRESULT			LateReady_Object() override;
	virtual _int			Update_Object(const _float& fTimeDelta) override;
	virtual _int			LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void			Render_Object() override;

protected:
	//HRESULT				Add_Component();
	virtual HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	const	_float&			Get_PosX() { return m_fX; }
	const	_float&			Get_PosY() { return m_fY; }
	const	_bool&			Get_CanUseItemSkill() { return m_bCanUse; }

public:
	void					Set_UseItemSkill() { m_bCanUse = false; m_fCoolDownValue = 0.f; }

protected:
	virtual	void			Cool_Down(const _float& fTimeDelta);

protected:
	_bool					m_bCanUse = true;

	_float					m_fCoolDownDelay = 0.01f;
	_float					m_fCoolDownValue = 1.f;

	CPlayer*				m_pPlayer = nullptr;
	CPlayer_SlotUI*			m_pSlotUI = nullptr;

public:
	virtual void			Free();
};
