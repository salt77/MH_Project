#ifndef Player_h__
#define Player_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CDynamicMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CNaviMesh;
class CShader;
class CTexture;

END

class CDynamicCamera;
class CAhglan;
class CTrail_Sword;
class CTrail_Smash;
class CRadialBlur;
class CPlayer_Hpbar_ValueUI;
class CPlayer_Hpbar_LerpUI;
class CPlayer_Steminabar_ValueUI;
class CPlayer_Spbar_ValueUI;

class CPlayer : public CGameObject
{
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

public:
	virtual HRESULT Ready_Object() override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

public:
	const PL_INFO&	Get_TagPlayerInfo() { return m_tPlayerInfo; }
	const _bool&	Get_CanHit() { return m_bCanHit; }
	const PL_STATE&	Get_CurState() { return m_eCurState; }
	const PL_ACTION&Get_CurAction() { return m_eCurAction; }
	const _float&	Get_PlayerSpeed() { return m_fSpeed; }

public:
	void			Set_CanHit(_bool bValue) { m_bCanHit = bValue; }
	void			Set_SpPoint(_bool bIsSmash);
	void			Set_StopMotion(_bool bStopMotion, _ulong dwTime);
	void			Set_PlayerStemina(_float fStemina) 
	{ 
		if (0 < m_tPlayerInfo.fStamina)
		{
			m_tPlayerInfo.fStamina += fStemina;
			m_dwSteminaRecoveryTime = GetTickCount();
		}
	}
	void			Set_PushState(_bool bValue) { m_bPush = bValue; }
	virtual	void	Set_Damage(_int iDamage, const _matrix* pMatDamageFontPos, _bool bFront = false);
	void			Set_ColRender() { m_bColRender = !m_bColRender; }
	void			Set_NaviMeshRender() { m_bNaviMeshRender = !m_bNaviMeshRender; }

private:
	// 기본 함수들
	HRESULT			Add_Component();
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	HRESULT			Ready_Layer_PlayerUI();
	void			Key_Input(const _float& fTimeDelta);
	void			SecondaryMode_MouseMove();
	//void			SetUp_OnTerrain();
	//_vec3			PickUp_OnTerrain();
	void			Animation_Control();
	void			Collision_Control();
	void			Update_UI();
	void			Update_State();
	const _ulong&	Compute_InCell();

	// 객체 함수들
	void			Ready_PlayerStructInfo();
	void			Compute_CanAction();
	void			Compute_Buff();
	void			Rotate_PlayerLook(const _float& fTimeDelta, _vec3& TargetLookVector);
	void			Rotate_PlayerLook(_vec3& TargetLookVector);
	void			MoveOn_Skill(const _float& fTimeDelta);
	void			StopMotion();
	void			FootStepSound();
	void			Weapon_Change();
	void			SetNextSmash(PL_STATE eState, _ulong dwDelay) { m_eNextSmash = eState; m_dwNextSmashCheckTime = GetTickCount();  m_dwNextSmashDelay = dwDelay; }
	void			Pooling_DamageFont(_uint iDamage, const _matrix* pMatrix, DAMAGEFONT_ID eID);
	void			Make_TrailEffect(const _float& fDeltaTime);
	void			SupportFire_Balista();

public:
	void			Compute_Critical(const _matrix* matWorld);
	void			Add_Buff(BUFF_ID eID, _ulong dwBuffDuration);
	HRESULT			Add_NaviMesh();

private:
	_bool			m_bColRender = false;
	_bool			m_bNaviMeshRender = false;

	_bool			m_bIsPlayer = true;
	_bool			m_bCanAction = true;
	_bool			m_bSkillMove = false;
	_bool			m_bCanHit = true;
	_bool			m_bStopMotion = false;
	_bool			m_bAnimation = true;
	_bool			m_bBalistaFire = false;
	_bool			m_bPush = false;

	_uint			m_iAniIndex = (_uint)STATE_IDLE;
	_uint			m_iSecondaryCount = 0;
	_uint			m_iDashCount = 0;
	_uint			m_iFuryNo7Count = 0;
	_uint			m_iBalistaFireCount = 0;

	_float			m_fSpeed = 3.f;
	_float			m_fOriginSpeed = 3.f;
	_float			m_fSkillMoveSpeed = 0.f;
	_float			m_fAniTime = 0.f;
	_float			m_fSkillMoveStartTime = 0.f;
	_float			m_fSkillMoveEndTime = 0.f;
	_float			m_fCriticalPotential = 3.f;
	_float			m_fOriginCriticalPotectial = 3.f;

	_double			m_lfAniEnd = 0.f;

	_ulong			m_dwNextSmashCheckTime = 0;
	_ulong			m_dwNextSmashDelay = 0;
	_ulong			m_dwSteminaRecoveryTime = GetTickCount();
	_ulong			m_dwSteminaRecoveryDelay = 2000;
	_ulong			m_dwStopMotionStart = GetTickCount();
	_ulong			m_dwStopMotionTime = 0;
	_ulong			m_dwFootStepStart = GetTickCount();
	_ulong			m_dwFootStepDelay = 300;
	_ulong			m_dwThrowStart = GetTickCount();
	_ulong			m_dwThrowDelay = 150;
	_ulong			m_dwBalistaFire = GetTickCount();
	_ulong			m_dwBalistaDelay = 200;

	_vec3			m_vDir;
	_vec3			m_vLookDir;
	_vec3			m_vRightDir;

	PL_INFO			m_tPlayerInfo;

	SCENE_ID		m_eSceneID = SCENE_END;

	PL_ACTION		m_eCurAction = PL_IDLE;

	PL_STATE		m_eCurState = (PL_STATE)m_iAniIndex;
	PL_STATE		m_ePreState = (PL_STATE)m_iAniIndex;

	PL_STATE		m_eNextAtk = STATE_ATK1;
	PL_STATE		m_eNextSmash = STATE_DASHATK;

	WEAPON_MODE		m_eCurWeaponMode = WEAPON_DUALSWORD;
	WEAPON_MODE		m_ePreWeaponMode = WEAPON_DUALSWORD;

	POTION_TYPE		m_ePotionType = POTION_END;

	list<tag_BuffDeBuff*>		m_listBuff;
	map<const wstring, _bool>	m_mapActiveParts;

	// Component
	CDynamicMesh*	m_pMeshCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CNaviMesh*		m_pNaviMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

	// Instance
	CDynamicCamera*	m_pMainCam = nullptr;
	CAhglan*		m_pAhglan = nullptr;
	CLayer*			m_pUILayer = nullptr;
	CLayer*			m_pOtherLayer = nullptr;
	CTrail_Sword*	m_pTrailSwordL = nullptr;
	CTrail_Sword*	m_pTrailSwordR = nullptr;
	CTrail_Smash*	m_pTrailSmashL = nullptr;
	CTrail_Smash*	m_pTrailSmashR = nullptr;
	CRadialBlur*	m_pRadialBlur = nullptr;
	CPlayer_Hpbar_ValueUI*	m_pHpbarValueUI = nullptr;
	CPlayer_Hpbar_LerpUI*	m_pHpbarLerpUI = nullptr;
	CPlayer_Steminabar_ValueUI*	m_pSteminabarValueUI = nullptr;
	CPlayer_Spbar_ValueUI*		m_pSpbarValueUI = nullptr;

public:
	static CPlayer*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);


	// Sound 변수들 
private:
	_bool		m_bAtkSound[3] = { false, false, false };
	_bool		m_bLethitaSound[3] = { false, false, false };
	_bool		m_bSkillSound[3] = { false, false, false };
	_uint		m_iFuryNo7Sound = 0;
};

#define SKILL_MOVE_BYANI(StartTime, Speed, EndTime)		m_bSkillMove = TRUE; m_fSkillMoveStartTime = StartTime;	m_fSkillMoveEndTime = EndTime;	m_fSkillMoveSpeed = Speed;
#define SKILL_MOVE_END									m_bSkillMove = FALSE; m_fSkillMoveStartTime = 0.f; m_fSkillMoveEndTime = (_float)m_lfAniEnd; m_fSkillMoveSpeed = 0.f;
#define STOP_MOTION(Time)								m_bStopMotion = TRUE; m_dwStopMotionStart = GetTickCount();	m_dwStopMotionTime = Time;
#define STOP_MOTION_END									m_bStopMotion = FALSE; m_dwStopMotionTime = 0;

#endif // Player_h__
