#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CRenderer;

END

class CMFC_CamEye : public CGameObject
{
public:
	explicit CMFC_CamEye(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMFC_CamEye();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

public:
	void			Set_Selected(COLTYPE eColType) { m_eColType = eColType; }
	void			Set_PreEye(_vec3 vPreEyePos) { m_vPreEye = vPreEyePos; }
	void			Set_NextEye(_vec3 vNextEyePos) { m_vNextEye = vNextEyePos; }
	void			Set_CamSpeed(_float fCamSpeed) { m_fCamSpeed = fCamSpeed; }

private:
	HRESULT			Add_Component(void);
	void			Interpolation_Traffic();

public:
	static	CMFC_CamEye*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free();

private:
	_float			m_fCamSpeed = 0.f;

	_vec3			m_vPreEye = {};
	_vec3			m_vNextEye = {};
	_vec3			m_vTargetEye = {};

	COLTYPE			m_eColType = COL_FALSE;

	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;

	LPD3DXMESH		m_pSphere;
	LPD3DXLINE		m_pLine;
#ifdef _DEBUG
	LPDIRECT3DTEXTURE9			m_pTexture[COLTYPE::COL_END];
#endif
};

