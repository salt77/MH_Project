#pragma once

#ifndef __KNIGHT_H__
#define __KNIGHT_H__

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

class CKnight : public CGameObject
{
public:
	enum KNIGHT_STATE
	{
		KNIGHT_ATTACK, KNIGHT_DAMAGED, KNIGHT_DAMAGED2, KNIGHT_DOWN_BEGIN, KNIGHT_DOWN_IDLE, KNIGHT_DOWN_END,
		KNIGHT_DYING, KNIGHT_IDLE, KNIGHT_STAPBACK, KNIGHT_STAPLEFT, KNIGHT_STAPRIGHT, KNIGHT_STAPFRONT,
		KNIGHT_KNOCKBACK, 

		KNIGHT_END
	};

public:
	explicit CKnight(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKnight();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	// �⺻ �Լ���
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void			Animation_Control();
	void			Collision_Control();

	// ��ü �Լ���
	void			Contact();
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
	_bool			m_bSkillMove2 = false;
	//_bool			m_bSkillRotation = false;
	_bool			m_bAnimation = true;

	_uint			m_iAniIndex = KNIGHT_IDLE;

	_float			m_fTimeDelta = 0.f;
	_float			m_fSpeed = 1.5f;
	_float			m_fAngle = 0.f;
	_float			m_fAniTime = 0.f;
	_float			m_fDistance = INFINITY_FLOAT;
	_float			m_fRand = Engine::Random(0.f, 100.f);

	// Skill Move
	_float			m_fSkillMoveSpeed = 0.f;
	_float			m_fAniEndDelay = 1.f;
	_float			m_fSkillMoveStartTime = 0.f;
	_float			m_fSkillMoveEndTime = 0.f;
	_float			m_fSkillMoveStartTime2 = 0.f;
	_float			m_fSkillMoveEndTime2 = 0.f;

	_double			m_lfAniEnd = 0.f;

	_vec3			m_vDir;
	_vec3			m_vMyPos;
	_vec3			m_vPlayerPos;

	KNIGHT_STATE		m_eCurState = KNIGHT_IDLE;
	KNIGHT_STATE		m_ePreState = KNIGHT_END;

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
	static CKnight*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);


private:
	// Sound ������
	_bool	m_bSound = false;
};

#define	ENEMY_SKILL_MOVE(StartTime, Speed, EndTime)	m_bSkillMove = TRUE;  m_fSkillMoveSpeed = Speed; m_fSkillMoveStartTime = StartTime; m_fSkillMoveEndTime = EndTime;
#define	ENEMY_SKILL_MOVE2(StartTime, Speed, EndTime)m_bSkillMove2 = TRUE;  m_fSkillMoveSpeed = Speed; m_fSkillMoveStartTime2 = StartTime; m_fSkillMoveEndTime2 = EndTime;
#define ENEMY_SKILL_MOVE_END						m_bSkillMove = FALSE; m_fSkillMoveSpeed = 0.f;   m_fSkillMoveStartTime = 0.f;       m_fSkillMoveEndTime = 0.f;

#endif //__KNIGHT_H__