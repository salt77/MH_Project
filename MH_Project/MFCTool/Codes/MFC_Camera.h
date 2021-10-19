#pragma once

#include "Camera.h"

BEGIN(Engine)

class CTransform;

END

class CMFC_Camera : public CCamera
{
public:
	explicit CMFC_Camera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMFC_Camera(const CMFC_Camera& rhs);
	virtual ~CMFC_Camera();

public:
	HRESULT	Ready_Object(const _vec3* pEye,
						const _vec3* pAt,
						const _vec3* pUp,
						const _float& fFov,
						const _float& fAspect,
						const _float& fNear,
						const _float& fFar);

	virtual _int Update_Object(const _float& fTimeDelta) override;

private:
	HRESULT		Add_Component();
	void		Key_Input(const _float& fTimeDelta);
	//void		Mouse_Move(void);

public:
	static CMFC_Camera*	Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov,
		const _float& fAspect,
		const _float& fNear,
		const _float& fFar);

	virtual void Free(void) override;

private:
	_float		m_fSpeed = 15.f;

	CTransform*	m_pTransformCom = nullptr;
};

