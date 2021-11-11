#pragma once

#ifndef __AHGLAN_H__
#define __AHGLAN_H__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CDynamicMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class CNaviMesh;
class CShader;

END

class CPlayer;
class CDynamicCamera;
class CBoss_Hpbar_GreenUI;
class CBoss_Hpbar_YellowUI;
class CBoss_Hpbar_RedUI;
class CBoss_Hpbar_FontUI;

class CAhglan : public CGameObject
{
public:
	enum BOSS_ACTION
	{
		BS_IDLE,
		//BS_MIDDLE,
		BS_ATK,
		BS_DAMAGED,
		BS_DEAD,
		BS_ENTRY,

		BS_END
	};

	enum STATE
	{
		WALK, TURN_RIGHT, TURN_LEFT, TAUNT, SPAWN, LOW_HEALTH, IDLE,
		ATK_WINDMILL, ATK_TWOHANDS_COMBO, ATK_TWOHANDS, ATK_TURNRIGHT, ATK_TURNLEFT, ATK_STAMP, ATK_ROLLING_TWICE,
		ATK_ROLLING_ONETIME_END, ATK_ROLLING_ONETIME_BEGIN, ATK_ONEHAND,
		DAMAGE_FROM_FRONT, DAMAGE_FROM_BACK,
		ENTRY_IDLE, ENTRY_CONTACT, ATK_THREETIME_STAMP, ATK_ONETIME_STAMP,

		A_STATE_END
	};

public:
	explicit CAhglan(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAhglan();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	// 기본 함수들
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void			Animation_Control();
	void			Collision_Control();
	void			Update_UI();

	// 객체 함수들
	void			Contact();
	void			Movement();
	void			MoveOn_Skill();
	void			RotationOn_Skill();
	void			FootStep();

public:
	HRESULT			Add_NaviMesh();
	void			BombAttacked(const _vec3& vBombPos, const wstring& wstrPartsName = L"");

private:
	_bool			m_bTargetIsRight = false;
	_bool			m_bCanAction = true;
	_bool			m_bSkillMove = false;
	_bool			m_bSkillRotation = false;
	_bool			m_bAnimation = true;

	_uint			m_iAniIndex = (_uint)ENTRY_IDLE;

	// Windmill
	_ulong			m_dwWindmillCoolDown = GetTickCount();
	_ulong			m_dwCoolDownInterpol = 30000;
	_ulong			m_dwWindmillDelay = m_dwCoolDownInterpol + rand() % 20000;

	// Rolling
	_ulong			m_dwRollingAtkCoolDown = GetTickCount();
	_ulong			m_dwRollingAtkDelay = m_dwCoolDownInterpol + rand() % 10000;
	_ulong			m_dwRollingStartTime = GetTickCount();
	_ulong			m_dwFootStepStart = GetTickCount();
	_ulong			m_dwFootStepDelay = 850;

	_float			m_fTimeDelta = 0.f;
	_float			m_fSpeed = 4.5f;
	_float			m_fAngle = 0.f;
	_float			m_fAniTime = 0.f;
	_float			m_fDistance = INFINITY_FLOAT;
	_float			m_fRandSpeed = (rand() % 300) * 0.001f;
	_float			m_fRand;
	// Skill Move
	_float			m_fSkillMoveSpeed = 0.f;
	_float			m_fAniEndDelay = 1.f;
	_float			m_fSkillMoveStartTime = 0.f;
	_float			m_fSkillMoveEndTime = 0.f;
	// Skill Rotation
	_float			m_fSkillRotSpeed = 0.f;
	_float			m_fSkillRotStartTime = 0.f;
	_float			m_fSkillRotEndTime = 0.f;

	_double			m_lfAniEnd = 0.f;

	_vec3			m_vDir;
	_vec3			m_vMyPos;
	_vec3			m_vPlayerPos;

	BOSS_ACTION		m_eBossAction = BS_END;

	STATE			m_eCurState = ENTRY_IDLE;
	STATE			m_ePreState = A_STATE_END;

	map<const wstring, _bool>	m_mapActiveParts;

	CPlayer*		m_pPlayer = nullptr;
	CTransform*		m_pPlayerTrans = nullptr;
	CDynamicCamera*	m_pMainCamera = nullptr;
	
	// Component
	CDynamicMesh*	m_pMeshCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;
	CNaviMesh*		m_pNaviMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

	// Instance
	CLayer*					m_pUILayer = nullptr;
	CBoss_Hpbar_GreenUI*	m_pGreenHpbar = nullptr;
	CBoss_Hpbar_YellowUI*	m_pYellowHpbar = nullptr;
	CBoss_Hpbar_RedUI*		m_pRedHpbar = nullptr;
	CBoss_Hpbar_FontUI*		m_pFontHpbar = nullptr;
	CBoss_Hpbar_FontUI*		m_pFontHpbarTen = nullptr;

public:
	static CAhglan*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);


private:
	// Sound 변수들
	_bool	m_bAtkSound = false;
	_bool	m_bEffectSound = false;
	_bool	m_bStampAtkSound[3] = { false, false, false };
};

#define BS_SKILL_ROTATION(StartTime, RotSpeed, EndTime) m_bSkillRotation = TRUE;  m_fSkillRotStartTime = StartTime;	m_fSkillRotSpeed = RotSpeed;  m_fSkillRotEndTime = EndTime;
#define BS_SKILL_ROTATION_END							m_bSkillRotation = FALSE; m_fSkillRotStartTime = 0.f;		m_fSkillRotSpeed = 0.f		  m_fSkillRotEndTime = 0.f;
#define	BS_SKILL_MOVE(StartTime, Speed, EndTime)	m_bSkillMove = TRUE;  m_fSkillMoveSpeed = Speed; m_fSkillMoveStartTime = StartTime; m_fSkillMoveEndTime = EndTime;
#define BS_SKILL_MOVE_END							m_bSkillMove = FALSE; m_fSkillMoveSpeed = 0.f;   m_fSkillMoveStartTime = 0.f;       m_fSkillMoveEndTime = 0.f;

#endif //__AHGLAN_H__