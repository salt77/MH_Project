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

class CAhglan_FontName final : public CUI
{
private:
	explicit CAhglan_FontName(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAhglan_FontName(void);

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY, BOSS_ID eBossType);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

protected:
	HRESULT				Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	BOSS_ID				m_eBossId = BOSS_AHGLAN;

public:
	static CAhglan_FontName*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY, BOSS_ID eBossType = BOSS_AHGLAN);
	virtual void		Free(void);
};
