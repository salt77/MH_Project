#pragma once

#ifndef __CLOYAN_H__
#define __CLOYAN_H__

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
class CBoss_Hpbar_GreenUI;
class CBoss_Hpbar_YellowUI;
class CBoss_Hpbar_RedUI;
class CBoss_Hpbar_FontUI;

class CCloyan : public CGameObject
{
public:
	enum CLOYAN_ACTION
	{
		CL_IDLE, CL_ATK, CL_DAMAGED, CL_DEAD, CL_SPAWN, 
		CL_END
	};
	enum CLOYAN_STATE
	{
		CLOYAN_ATTACK, CLOYAN_DAMAGEFROMBACK, CLOYAN_DAMAGEFROMFRONT, CLOYAN_DYING, CLOYAN_IDLE, CLOYAN_LOW_HEALTH,
		CLOYAN_RUN, CLOYAN_SLASHPIERCE, CLOYAN_SLIPDASH, CLOYAN_SPAWN, CLOYAN_STEPBACK, CLOYAN_STEPFRONT, CLOYAN_STEPPIERCE, CLOYAN_TWINBOLT, 
		CLOYAN_TURNLEFT, CLOYAN_TURNRIGHT, 
		CLOYAN_END
	};

private:
	explicit CCloyan(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCloyan();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

public:
	void			Set_Enable(_vec3 vPos, _vec3 vRotate);

private:
	// 기본 함수들
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void			Animation_Control();
	void			Collision_Control();
	const _ulong&	Compute_InCell();
	void			Update_UI();
	void			Update_State();

	// 객체 함수들
	void			Movement();
	void			MoveOn_Skill();
	void			RotationOn_Skill();
	void			RotateLookVector();

public:
	HRESULT			Add_NaviMesh();
	HRESULT			Load_ColInfo();

private:
	_bool			m_bTargetIsRight = false;
	_bool			m_bCanAction = true;
	_bool			m_bSkillMove = false;
	_bool			m_bSkillRotation = false;
	_bool			m_bAnimation = true;

	_uint			m_iAniIndex = CLOYAN_SPAWN;

	_ulong			m_dwSlipDashCoolDown = GetTickCount();
	_ulong			m_dwSlipDashDelay = (_ulong)Engine::Random(10000.f, 20000.f);

	_float			m_fTimeDelta = 0.f;
	_float			m_fSpeed = 4.5f;
	_float			m_fStepSpeed = 1.5f;
	_float			m_fAngle = 0.f;
	_float			m_fAniTime = 0.f;
	_float			m_fDistance = INFINITY_FLOAT;
	_float			m_fRand = Engine::Random(0.f, 100.f);

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

	CLOYAN_ACTION	m_eCurAction = CL_IDLE;

	CLOYAN_STATE	m_eCurState = CLOYAN_IDLE;
	CLOYAN_STATE	m_ePreState = CLOYAN_END;

	CPlayer*		m_pPlayer = nullptr;
	CTransform*		m_pPlayerTrans = nullptr;
	CLayer*			m_pUILayer = nullptr;
	
	// Component
	CDynamicMesh*	m_pMeshCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;
	CNaviMesh*		m_pNaviMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

	// Instance 
	CBoss_Hpbar_GreenUI*	m_pGreenHpbar = nullptr;
	CBoss_Hpbar_YellowUI*	m_pYellowHpbar = nullptr;
	CBoss_Hpbar_RedUI*		m_pRedHpbar = nullptr;
	CBoss_Hpbar_FontUI*		m_pFontHpbar = nullptr;
	CBoss_Hpbar_FontUI*		m_pFontHpbarTen = nullptr;

public:
	static CCloyan*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);


private:
	// Sound 변수들
	_bool	m_bSound = false;
};

#define	ENEMY_SKILL_MOVE(StartTime, Speed, EndTime)	m_bSkillMove = TRUE;  m_fSkillMoveSpeed = Speed; m_fSkillMoveStartTime = StartTime; m_fSkillMoveEndTime = EndTime;
#define ENEMY_SKILL_MOVE_END						m_bSkillMove = FALSE; m_fSkillMoveSpeed = 0.f;   m_fSkillMoveStartTime = 0.f;       m_fSkillMoveEndTime = 0.f;
#define ENEMY_SKILL_ROTATION(StartTime, RotSpeed, EndTime) m_bSkillRotation = TRUE;  m_fSkillRotStartTime = StartTime;	m_fSkillRotSpeed = RotSpeed;  m_fSkillRotEndTime = EndTime;
#define ENEMY_SKILL_ROTATION_END						   m_bSkillRotation = FALSE; m_fSkillRotStartTime = 0.f;		m_fSkillRotSpeed = 0.f		  m_fSkillRotEndTime = 0.f;

#endif //__CLOYAN_H__