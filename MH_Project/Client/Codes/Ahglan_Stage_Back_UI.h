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

class CAhglan_Stage_Back_UI final : public CUI
{
private:
	explicit CAhglan_Stage_Back_UI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAhglan_Stage_Back_UI(void);

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

protected:
	HRESULT				Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	static CAhglan_Stage_Back_UI*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void		Free(void);
};
