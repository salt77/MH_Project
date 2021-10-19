#pragma once

#include "GameObject.h"

class CMFC_CamInterpol : public CGameObject
{
public:
	explicit CMFC_CamInterpol(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMFC_CamInterpol();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

public:
	void			Set_NextPos(_vec3 vPos) { m_vNextPos = vPos; if (vPos != _vec3(0.f, 0.f, 0.f)) { m_vPrePos = vPos; } }

public:
	void			RefreshPos() { m_pTransformCom->Set_Pos(&m_vOriginPos); }

private:
	HRESULT			Add_Component(void);

public:
	static	CMFC_CamInterpol*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void				Free();

private:
	_float			m_fCamSpeed = 0.f;

	_vec3			m_vOriginPos = {};
	_vec3			m_vNextPos = {};
	_vec3			m_vPrePos = {};

	CTransform*		m_pTransformCom = nullptr;
};

