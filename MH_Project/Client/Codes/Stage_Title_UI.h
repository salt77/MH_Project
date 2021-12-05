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

class CStage_Title_UI final : public CUI
{
private:
	explicit CStage_Title_UI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage_Title_UI();

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
	_ulong				m_dwSurviveTime = GetTickCount();
	_ulong				m_dwSurviveDelay = 1000;

	_float				m_fAlphaInterpol = 0.5f;

	SCENE_ID			m_eSceneID = SCENE_END;

public:
	static CStage_Title_UI*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void			Free();
};
