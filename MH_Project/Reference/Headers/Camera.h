#ifndef Camera_h__
#define Camera_h__

#include "GameObject.h"

BEGIN(Engine)

class CTransform;

class ENGINE_DLL CCamera : public CGameObject
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;

public:
	const _float&	Get_Speed() { return m_fSpeed; }
	const _bool&	Get_ActionCam() { return m_bActionCam; }
	const _vec3&	Get_EyePos() { return m_vEye; }
	const _vec3&	Get_AtPos() { return m_vAt; }

public:
	void			Set_ActionCam(_bool bValue) { m_bActionCam = bValue; }
	void			Set_ActionEye(_vec3 vEye) { m_vActionEye = vEye; }
	void			Set_ActionAt(_vec3 vAt) { m_vActionAt = vAt; }
	void			Set_StalkTarget(_vec3 vTargetPos) { m_vStalkTarget = vTargetPos; }
	void			Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }

public:
	_vec3&			Get_CamLook();

protected:
	_vec3			m_vEye, m_vAt, m_vUp;
	_float			m_fFov, m_fAspect, m_fNear, m_fFar;
	_matrix			m_matView, m_matProj;

	// Action Camera Settings
	_bool		m_bActionCam = false;

	_float		m_fSpeed = 0.005f;

	_vec3		m_vStalkTarget = {};
	_vec3		m_vActionEye = {};
	_vec3		m_vActionAt = {};

public:
	virtual void Free(void) override;
};

END
#endif // Camera_h__
