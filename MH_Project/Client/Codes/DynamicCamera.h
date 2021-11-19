#ifndef DynamicCamera_h__
#define DynamicCamera_h__

#include "Camera.h"
#include "Define.h"

#include "FadeInOut.h"

BEGIN(Engine)

class CTransform;
class CStaticMesh;

END

class CPlayer;
class CStageMesh;
class CStageMesh_1;

class CDynamicCamera : public CCamera
{
public:
	enum MODE
	{
		MODE_NORMAL, MODE_SECONDARY,
		MODE_AHGLAN_START, MODE_AHGLAN_RISE, MODE_AHGLAN_STAND, MODE_AHGLAN_COMPLETE,
		MODE_END
	};

private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicCamera(const CDynamicCamera& rhs);
	virtual ~CDynamicCamera(void);

public:
	HRESULT			Ready_Object(const _vec3* pEye,
								 const _vec3* pAt,
								 const _vec3* pUp,
								 const _float& fFov,
								 const _float& fAspect,
								 const _float& fNear,
								 const _float& fFar);
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;

public:
	_vec3		Get_CamDirVector(DIR eDir);
	const MODE&	Get_CamMode() { return m_eCurMode; }

public:
	void		Set_CameraMode(MODE eMode) { m_eCurMode = eMode; if (MODE_AHGLAN_START == eMode)	m_dwStartTime = GetTickCount(); }
	void		Set_CameraShake(_bool bShakeType, _float fPower, _ulong dwEndTime = 1000, _float fWaveInterpol = 0.5f);
	void		Set_HighlightSkillShot(_float fDistance, _ulong dwHighlightTime) 
	{ 
		m_fHighlightDistance = fDistance;
		m_dwHighlightStart = GetTickCount();
		m_dwHighlightDelay = dwHighlightTime;
	}
	void		Set_PrePlayerPos(_vec3 vPlayerPos) { m_vPrePlayerPos = vPlayerPos; }

public:
	void		Sync_PlayerPos(_vec3 vDir, _float fSpeed, const _float& fTimeDelta) { if (MODE_NORMAL == m_eCurMode)	m_vEye += vDir * fSpeed * fTimeDelta; }
	void		Sync_PlayerPos(_vec3 vDir);
	void		Highlight_SkillShot();

private:
	void		Camera_Shake(const _float& fTimeDelta);
	void		Mode_Change(const _float& fTimeDelta);
	void		At_Update(const _float& fTimeDelta);
	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move(void);
	void		Mouse_Fix(void);
	void		CutScene_Eye(const _float& fTimeDelta);
	void		Collision_StageMesh();

private:
	_bool		m_bClick = false;
	_bool		m_bFix = true;
	_bool		m_bShake = false;
	_bool		m_bLongShake = false;
	_bool		m_bSoundGolemEntry = false;
	_bool		m_bCollisionCam = false;

	_ulong		m_dwHighlightStart = GetTickCount();
	_ulong		m_dwHighlightDelay = 0;
	_ulong		m_dwShakeTime = GetTickCount();
	_ulong		m_dwShakeDelay = 1000;

	_float		m_fLongWaveInterpol = 0.5f;
	_float		m_fFXProgress = 0.f;
	_float		m_fShakeWaveX = 0.f;
	_float		m_fFYProgress = 0.f;
	_float		m_fShakeWaveY = 0.f;
	_float		m_fShakePower = 0.025f;
	_float		m_fSpeed = 20.f;
	_float		m_fCamAngle = 0.f;
	_float		m_fDistanceFromTarget = 2.15f;
	_float		m_fOriginDistanceFromTarget = 2.15f;
	_float		m_fHighlightDistance = 2.15f;
	_float		m_fInterpolY = 0.85f;

	// Ahglan Cutscene
	_float		m_fRiseUp = 0.05f;
	_float		m_fFarAway = 0.f;

	_ulong		m_dwStartTime = GetTickCount();
	_ulong		m_dwRiseTime = GetTickCount();
	_ulong		m_dwStandTime = GetTickCount();
	_ulong		m_dwCompleteTime = GetTickCount();
	///////////////////////

	_vec3		m_vPrePlayerPos = { 0.f, 0.f, 0.f };
	_vec3		m_vShakeInterpol = { 0.f, 0.f, 0.f };
	_vec3		m_vPreShakeInterpol = { 0.f, 0.f, 0.f };
	//_vec3		m_vFollowDir = _vec3(0.f, 0.f, 0.f);

	MODE		m_eCurMode = MODE_NORMAL;
	MODE		m_ePreMode = MODE_END;

	CPlayer*	m_pPlayer = nullptr;
	CTransform*	m_pPlayerTrans = nullptr;
	CTransform*	m_pTransformCom = nullptr;

	CFadeInOut*	m_pFadeInOut = nullptr;
	CStageMesh*		m_pStage = nullptr;
	CStageMesh_1*	m_pStage_1 = nullptr;
	CStaticMesh*	m_pStageMesh = nullptr;

#define		WaveFxProgressive	1.5f
#define		WaveFyProgressive	0.9f

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
