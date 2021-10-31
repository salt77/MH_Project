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
public:
	enum MODE
	{
		MODE_NORMAL, 
		MODE_AHGLAN_START, MODE_AHGLAN_RISE, MODE_AHGLAN_STAND, MODE_AHGLAN_COMPLETE,
		MODE_END
	};

private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicCamera(const CDynamicCamera& rhs);
	virtual ~CDynamicCamera(void);

public:
	HRESULT		Ready_Object(const _vec3* pEye,
							 const _vec3* pAt,
							 const _vec3* pUp,
							 const _float& fFov,
							 const _float& fAspect,
							 const _float& fNear,
							 const _float& fFar);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual _int LateUpdate_Object(const _float& fTimeDelta) override;

public:
	const _vec3&	Get_CamDirVector(DIR eDir);

public:
	void			Set_CameraMode(MODE eMode) { m_eCurMode = eMode; }

public:
	void		Sync_PlayerPos(_vec3 vDir, _float fSpeed, const _float& fTimeDelta) { if (MODE_NORMAL == m_eCurMode)	m_vEye += vDir * fSpeed * fTimeDelta; }

private:
	void		Camera_Shake();
	void		Mode_Change();
	void		At_Update();
	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move(void);
	void		Mouse_Fix(void);

private:
	_bool		m_bClick = false;
	_bool		m_bFix   = true;
	_bool		m_bShake = false;

	_ulong		m_dwShakeStartTime = GetTickCount();
	_ulong		m_dwShakeDelay = 0;

	_float		m_fShakePower = 0.f;
	_float		m_fSpeed = 20.f;
	_float		m_fCamAngle = 0.f;
	_float		m_fDistanceFromTarget = 2.15f;
	_float		m_fInterpolY = 0.85f;

	// Ahglan Cutscene
	_float		m_fRiseUp = 0.05f;
	_float		m_fFarAway = 0.f;

	_ulong		m_dwStartTime = GetTickCount();
	_ulong		m_dwRiseTime = GetTickCount();
	_ulong		m_dwStandTime = GetTickCount();
	_ulong		m_dwCompleteTime = GetTickCount();
	///////////////////////

	_vec3		m_vFollowDir = _vec3(0.f, 0.f, 0.f);

	MODE		m_eCurMode = MODE_NORMAL;
	MODE		m_ePreMode = MODE_END;

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
