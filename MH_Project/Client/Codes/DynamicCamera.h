#ifndef DynamicCamera_h__
#define DynamicCamera_h__

#include "Camera.h"
#include "Define.h"

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

private:
	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move(void);
	void		Mouse_Fix(void);

private:
	_float		m_fSpeed = 20.f;
	_bool		m_bClick = false;
	_bool		m_bFix   = true;


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
