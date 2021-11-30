#pragma once

#include "Slot_ItemSkill.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CRenderer;
class CTransform;
class CShader;

END

class CPlayer_SlotUI;

class CPotion_Sp final : public CSlot_ItemSkill
{
private:
	explicit CPotion_Sp(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPotion_Sp();

public:
	virtual HRESULT		Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual HRESULT		LateReady_Object() override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual _int		LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void		Render_Object() override;

protected:
	HRESULT					Add_Component();
	virtual HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	static CPotion_Sp*		Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void			Free();
};
