#pragma once

#ifndef __DOG_H__
#define __DOG_H__

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

class CDog : public CGameObject
{
public:
	enum DOG_ACTION
	{
		DOG_IDLE,
		DOG_ATK,
		DOG_DAMAGED,
		DOG_DEAD,
		DOG_ENTRY,

		DOG_END
	};

	enum DOG_STATE
	{
		DOGSTATE_ATTACK, DOGSTATE_DAMAGED, DOGSTATE_DAMAGED2, DOGSTATE_DOWN_BEGIN, DOGSTATE_DOWN_IDLE, DOGSTATE_5,
		DOGSTATE_6, DOGSTATE_IDLE, DOGSTATE_TURNLEFT, DOGSTATE_TURNRIGHT, DOGSTATE_DOWNDMG,
		DOGSTATE_DEAD_BEGIN, DOGSTATE_DEAD_DURING, DOGSTATE_DEAD_END, 
		DOGSTATE_KNOCKBACK, DOGSTATE_BRR, 
		DOGSTATE_16, 
		DOGSTATE_RUN, 

		DOGSTATE_END
	};

public:
	explicit CDog(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDog();

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

	// 객체 함수들
	void			Contact();
	void			Movement();
	void			MoveOn_Skill();

public:
	HRESULT			Add_NaviMesh();
	HRESULT			Load_ColInfo();

private:
	_bool			m_bTargetIsRight = false;
	_bool			m_bCanAction = true;
	_bool			m_bSkillMove = false;
	//_bool			m_bSkillRotation = false;
	_bool			m_bAnimation = true;

	_uint			m_iAniIndex = DOGSTATE_IDLE;

	_ulong			m_dwBreakTime = GetTickCount();
	_ulong			m_dwBreakDelay = 1000;

	_float			m_fTimeDelta = 0.f;
	_float			m_fSpeed = 3.5f;
	_float			m_fAngle = 0.f;
	_float			m_fAniTime = 0.f;
	_float			m_fDistance = INFINITY_FLOAT;
	_float			m_fRand = Engine::Random(0.f, 100.f);

	// Skill Move
	_float			m_fSkillMoveSpeed = 0.f;
	_float			m_fAniEndDelay = 1.f;
	_float			m_fSkillMoveStartTime = 0.f;
	_float			m_fSkillMoveEndTime = 0.f;

	_double			m_lfAniEnd = 0.f;

	_vec3			m_vDir;
	_vec3			m_vMyPos;
	_vec3			m_vPlayerPos;

	DOG_ACTION		m_eDogAction = DOG_END;

	DOG_STATE		m_eCurState = DOGSTATE_IDLE;
	DOG_STATE		m_ePreState = DOGSTATE_END;

	map<const wstring, _bool>	m_mapActiveParts;

	CPlayer*		m_pPlayer = nullptr;
	CTransform*		m_pPlayerTrans = nullptr;
	
	// Component
	CDynamicMesh*	m_pMeshCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;
	CNaviMesh*		m_pNaviMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

public:
	static CDog*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);


private:
	// Sound 변수들
	_bool	m_bSound = false;
};

#define	ENEMY_SKILL_MOVE(StartTime, Speed, EndTime)	m_bSkillMove = TRUE;  m_fSkillMoveSpeed = Speed; m_fSkillMoveStartTime = StartTime; m_fSkillMoveEndTime = EndTime;
#define ENEMY_SKILL_MOVE_END						m_bSkillMove = FALSE; m_fSkillMoveSpeed = 0.f;   m_fSkillMoveStartTime = 0.f;       m_fSkillMoveEndTime = 0.f;

#endif //__DOG_H__