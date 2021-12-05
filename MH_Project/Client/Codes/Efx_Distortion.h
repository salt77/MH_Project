#pragma once

#include "Effect.h"
#include "Define.h"

class CEfx_Distortion : public CEffect
{
private:
	explicit CEfx_Distortion(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEfx_Distortion(const CEfx_Distortion& rhs);
	virtual ~CEfx_Distortion();

public:
	virtual HRESULT Ready_Object() override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

private:
	HRESULT			Add_Component();
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	void			Set_EnableDistortion(_vec3 vPos);

private:
	void			Scale_Interpolation(const _float& fTimeDelta);

private:
	_float				g_fRatio = 1.f;
	_float				m_fInterpol = 15.f;

	_vec3				m_vPos;
	_vec3				m_vScale;
	_vec3				m_vOriginScale;

	//CRcTex*			m_pBufferCom = nullptr;
	//CTexture*			m_pTextureCom = nullptr;
	//CRenderer*		m_pRendererCom = nullptr;
	//CTransform*		m_pTransformCom = nullptr;
	//CShader*			m_pShaderCom = nullptr;

public:
	static	CEfx_Distortion*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free() override;
};

