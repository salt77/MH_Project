#ifndef Player_h__
#define Player_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CDynamicMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class CNaviMesh;

END

class CDynamicCamera;

class CPlayer : public CGameObject
{
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	// 기본 함수들
	HRESULT			Add_Component(void);
	void			Key_Input(const _float& fTimeDelta);
	void			SetUp_OnTerrain(void);
	_vec3			PickUp_OnTerrain(void);
	void			Animation_Control();
	void			Collision_Control();

	// 객체 함수들
	void			Compute_CanAction();
	void			Rotate_PlayerLook(const _float& fTimeDelta, _vec3& TargetLookVector);
	void			Rotate_PlayerLook(_vec3& TargetLookVector);
	void			MoveOn_Skill(const _float& fTimeDelta);
	void			StopMotion();

public:
	HRESULT			Add_NaviMesh();

private:
	_bool			m_bIsPlayer = true;
	_bool			m_bCanAction = true;
	_bool			m_bSkillMove = false;
	_bool			m_bCanHit = true;
	_bool			m_bStopMotion = false;

	_uint			m_iAniIndex = (_uint)STATE_IDLE;

	_float			m_fSpeed = 3.f;
	_float			m_fSkillMoveSpeed = 0.f;
	_float			m_fAniTime = 0.f;

	_double			m_lfAniEnd = 0.f;	

	_ulong			m_dwSkillMoveReady = GetTickCount();
	_ulong			m_dwSkillMoveReadyTime = 0;
	_ulong			m_dwSkillMoveStart = GetTickCount();
	_ulong			m_dwSkillMoveTime = 0;
	_ulong			m_dwStopMotionStart = GetTickCount();
	_ulong			m_dwStopMotionTime = 0;

	_vec3			m_vDir;
	_vec3			m_vLookDir;
	_vec3			m_vRightDir;

	SCENE_ID		m_eSceneID = SCENE_END;

	PL_ACTION		m_eCurAction = PL_IDLE;

	PL_STATE		m_eCurState = (PL_STATE)m_iAniIndex;
	PL_STATE		m_ePreState = (PL_STATE)m_iAniIndex;

	PL_STATE		m_eNextAtk = STATE_ATK1;
	PL_STATE		m_eNextSmash = STATE_DASHATK;

	// Component
	CDynamicMesh*	m_pMeshCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;
	CNaviMesh*		m_pNaviMeshCom = nullptr;

	// Instance
	CDynamicCamera*	m_pMainCam = nullptr;

public:
	static CPlayer*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);
};

#define	SKILL_MOVE(ReadyTime, Speed, Time)		m_bSkillMove = TRUE; m_dwSkillMoveReady = GetTickCount();		\
												m_dwSkillMoveReadyTime = ReadyTime;	m_fSkillMoveSpeed = Speed; m_dwSkillMoveTime = Time;
#define SKILL_MOVE_END							m_bSkillMove = FALSE; m_fSkillMoveSpeed = 0.f; m_dwSkillMoveTime = 0;
#define STOP_MOTION(Time)			m_bStopMotion = TRUE; m_dwStopMotionStart = GetTickCount();	m_dwStopMotionTime = Time;
#define STOP_MOTION_END							m_bStopMotion = FALSE; m_dwStopMotionTime = 0;

#endif // Player_h__
