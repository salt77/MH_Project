#pragma once

#include "Effect.h"
#include "Define.h"

class CSlashPoint : public CEffect
{
public:
	explicit CSlashPoint(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSlashPoint(const CSlashPoint& rhs);
	virtual ~CSlashPoint();

public:
	virtual HRESULT Ready_Object() override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

private:
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	void			Scale_Interpolation(const _float& fTimeDelta);

private:
	_ulong				m_dwEffectStart = GetTickCount();
	_ulong				m_dwEffectTime = 1000;

	_vec3				m_vPos;
	_vec3				m_vScale;
	_vec3				m_vOriginScale;

	//CRcTex*			m_pBufferCom = nullptr;
	//CTexture*			m_pTextureCom = nullptr;
	//CRenderer*		m_pRendererCom = nullptr;
	//CTransform*		m_pTransformCom = nullptr;
	//CShader*			m_pShaderCom = nullptr;

public:
	static	CSlashPoint*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free() override;
};

