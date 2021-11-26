#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CShader;

END

class CDamageFont : public CGameObject
{
private:
	explicit CDamageFont(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDamageFont();

public:
	HRESULT			Ready_Object(DAMAGEFONT_ID eID);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

private:
	HRESULT			Add_Component();
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	void			Set_EnableDamageFont(_vec3 vPos, _uint iFontNum, _uint iDigit);

private:
	void			Position_Interpolation(const _float& fTimeDelta);
	void			Scale_Interpolation(const _float& fTimeDelta);
	void			Alpha_Interpolation(const _float& fTimeDelta);

private:
	_uint			m_iDigit = 0;
	_uint			m_iFontNum = 0;

	_ulong			m_dwFontStart = GetTickCount();
	_ulong			m_dwFontDelay = 1000;

	_float			m_fAlpha = 0.01f;
	const _float	m_fOriginAlpha = 0.01f;
	_float			m_fScaleDown = 0.1f;
	const _float	m_fOriginScaleDown = 0.1f;

	_vec3			m_vPos;
	_vec3			m_vOriginPos;
	_vec3			m_vOriginScale;

	DAMAGEFONT_ID	m_eFontID = DAMAGEFONT_END;

	CRcTex*			m_pBufferCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

public:
	static CDamageFont*		Create(LPDIRECT3DDEVICE9 pGraphicDev, DAMAGEFONT_ID eID);
	virtual void			Free(void);
};

