#pragma once

#ifndef __SOLDIER_H__
#define __SOLDIER_H__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CDynamicMesh;
class CTexture;
class CTransform;
class CRenderer;
class CCollider;
class CNaviMesh;
class CShader;

END

class CPlayer;

class CSoldier : public CGameObject
{
public:
	enum SOL_STATE
	{
		SOLSTATE_ATTACK, SOLSTATE_IDLE, SOLSTATE_RUN, SOLSTATE_SPAWN, SOLSTATE_TURNLEFT, SOLSTATE_TURNRIGHT,
		SOLSTATE_DYING, SOLSTATE_DAMAGED, SOLSTATE_DAMAGED2, SOLSTATE_DOWN_BEGIN, SOLSTATE_DOWN_IDLE, SOLSTATE_DOWN_END,
		SOLSTATE_DEAD_IDLE, 
		SOLSTATE_END
	};

private:
	explicit CSoldier(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSoldier();

public:
	virtual HRESULT Ready_Object() override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

public:
	virtual	void	Set_Damage(_int iDamage);
	void			Set_Enable(_vec3 vPos, _vec3 vRotate);

private:
	// 기본 함수들
	HRESULT			Add_Component();
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void			Animation_Control();
	void			Collision_Control();
	void			Dissolve(const _float& fTimeDelta);
	void			Update_State();
	const _ulong&	Compute_InCell();

	// 객체 함수들
	void			Movement();
	void			MoveOn_Skill();
	void			RotateLookVector();

public:
	HRESULT			Add_NaviMesh();
	HRESULT			Load_ColInfo();

private:
	_bool			m_bTargetIsRight = false;
	_bool			m_bCanAction = true;
	_bool			m_bSkillMove = false;
	//_bool			m_bSkillRotation = false;
	_bool			m_bAnimation = true;

	_uint			m_iAniIndex = SOLSTATE_SPAWN;

	_float			m_fTimeDelta = 0.f;
	_float			m_fSpeed = 2.8f;
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

	SOL_STATE		m_eCurState = SOLSTATE_IDLE;
	SOL_STATE		m_ePreState = SOLSTATE_END;

	CPlayer*		m_pPlayer = nullptr;
	CTransform*		m_pPlayerTrans = nullptr;
	
	// Component
	CDynamicMesh*	m_pMeshCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;
	CNaviMesh*		m_pNaviMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

public:
	static CSoldier*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);


private:
	// Sound 변수들
	_bool	m_bSound = false;
};

#define	ENEMY_SKILL_MOVE(StartTime, Speed, EndTime)	m_bSkillMove = TRUE;  m_fSkillMoveSpeed = Speed; m_fSkillMoveStartTime = StartTime; m_fSkillMoveEndTime = EndTime;
#define ENEMY_SKILL_MOVE_END						m_bSkillMove = FALSE; m_fSkillMoveSpeed = 0.f;   m_fSkillMoveStartTime = 0.f;       m_fSkillMoveEndTime = 0.f;

#endif //__SOLDIER_H__