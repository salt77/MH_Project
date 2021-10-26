#ifndef DynamicCamera_h__
#define DynamicCamera_h__

#include "Camera.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;

END

class CPlayer;

class CDynamicCamera : public CCamera
{
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicCamera(const CDynamicCamera& rhs);
	virtual ~CDynamicCamera(void);

public:
	HRESULT Ready_Object(const _vec3* pEye,
						const _vec3* pAt,
						const _vec3* pUp,
						const _float& fFov,
						const _float& fAspect,
						const _float& fNear,
						const _float& fFar);
	virtual _int Update_Object(const _float& fTimeDelta) override;

public:
	const _vec3&	Get_CamDirVector(DIR eDir);

public:
	void		Sync_PlayerPos(_vec3 vDir, _float fSpeed, const _float& fTimeDelta) { m_vEye += vDir * fSpeed * fTimeDelta; }

private:
	void		Movement(const _float& fTimeDelta);
	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move(void);
	void		Mouse_Fix(void);

private:
	_bool		m_bClick = false;
	_bool		m_bFix   = true;

	_float		m_fSpeed = 20.f;
	_float		m_fCamAngle = 0.f;
	_float		m_fDistanceFromTarget = 3.f;
	_float		m_fInterpolY = 0.85f;

	//CTransform*	m_pTransformCom = nullptr;
	CPlayer*	m_pPlayer = nullptr;
	CTransform*	m_pPlayerTrans = nullptr;


public:
	static CDynamicCamera*	Create(LPDIRECT3DDEVICE9 pGraphicDev,
									const _vec3* pEye,
									const _vec3* pAt,
									const _vec3* pUp,
									const _float& fFov,
									const _float& fAspect,
									const _float& fNear,
									const _float& fFar);
	
	virtual void			Free(void);

};

#endif // DynamicCamera_h__
