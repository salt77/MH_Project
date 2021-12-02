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

class CStart_Dissolve final : public CUI
{
private:
	explicit CStart_Dissolve(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStart_Dissolve();

public:
	virtual HRESULT		Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual HRESULT		LateReady_Object() override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		Render_Object() override;

protected:
	HRESULT				Add_Component();
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	void				Alpha_Interpolation(const _float& fTimeDelta);

private:
	_bool				m_bAlphaMax = false;

	_ulong				m_dwAlphaMaxTime = GetTickCount();
	_ulong				m_dwAlphaMaxDelay = 5000;

	_float				m_fAlphaInterpol = 0.5f;

	SCENE_ID			m_eSceneID = SCENE_END;

public:
	static CStart_Dissolve*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void			Free();
};
