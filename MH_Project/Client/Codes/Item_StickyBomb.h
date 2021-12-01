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

class CItem_StickyBomb final : public CSlot_ItemSkill
{
private:
	explicit CItem_StickyBomb(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CItem_StickyBomb();

public:
	virtual HRESULT			Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual HRESULT			LateReady_Object() override;
	virtual _int			Update_Object(const _float& fTimeDelta) override;
	virtual _int			LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void			Render_Object() override;

protected:
	HRESULT					Add_Component();
	virtual HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	static CItem_StickyBomb*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void				Free();
};
