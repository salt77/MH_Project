#pragma once

#include "Buff_Player.h"

class CBuff_Stemina final : public CBuff_Player
{
private:
	explicit CBuff_Stemina(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBuff_Stemina();

public:
	virtual HRESULT			Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual HRESULT			LateReady_Object() override;
	virtual _int			Update_Object(const _float& fTimeDelta) override;
	virtual _int			LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void			Render_Object() override;

private:
	HRESULT					Add_Component();
	virtual HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	static CBuff_Stemina*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void			Free() override;
};

