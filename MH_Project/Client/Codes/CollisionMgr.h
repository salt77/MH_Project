#pragma once

#include "Base.h"
#include "Export_Function.h"

USING(Engine)

class CPlayer;
class CAhglan;
class CStickyBomb;
class CWall_Collision;

class CCollisionMgr : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)

private:
	CCollisionMgr();
	virtual ~CCollisionMgr();

private:
	typedef	struct tagOBB
	{
		_vec3		vPoint[8];
		_vec3		vCenter;

		_vec3		vProjAxis[3];	// 중점에서 각 면을 향해 뻗어나가는 방향 벡터
		_vec3		vAxis[3];		// 면체와 평행한 축 벡터

	} OBB;

public:
	HRESULT			Ready_CollisionMgr();
	_uint			Update_CollisionMgr(const _float& fTimeDelta);

private:
	void			Collision_PlayerAttack();
	void			Collision_MonsterAttack();
	void			Collision_Balista_Stage();
	void			Collision_Balista_Stage_1();
	void			Collision_Trigger();
	void			Collision_Box_Player_Stage_1(const _float& fTimeDelta);

	void			Update_MultipleCollision();

	void			CollisionMgr_FrameStart();
	
private:
	_bool			Collision_Sphere(const _vec3& vDstCenter, const _float& fDstRadius, const _vec3& vSrcCenter, const _float& fSrcRadius);
	_bool			Collision_AABB(const _vec3* pDestMin, const _vec3* pDestMax, const _matrix* pDestWorld,
								   const _vec3* pSourMin, const _vec3* pSourMax, const _matrix* pSourWorld);
	_bool			Collision_OBB(const _vec3* pDestMin, const _vec3* pDestMax, const _matrix* pDestWorld,
								  const _vec3* pSourMin, const _vec3* pSourMax, const _matrix* pSourWorld);

private:
	void			Set_Point(OBB* pObb, const _vec3* pMin, const _vec3* pMax);
	void			Set_Axis(OBB* pObb);

	void			Pooling_DamageFont(_uint iDamage, const _matrix* pMatrix, DAMAGEFONT_ID eID);
	void			Pooling_SlashPoint(const _matrix* pMatrix, _bool bIsSmash = false);

private:
	_bool				m_bCollisionFrame = false;
	_bool				m_bCanCollision = false;
	_bool				m_bNextStageLoad = false;
	_bool				m_bMainBossEnd = false;

	SCENE_ID			m_eSceneID = SCENE_END;

	CPlayer*			m_pPlayer = nullptr;
	CAhglan*			m_pAhglan = nullptr;
	CStickyBomb*		m_pStickyBomb = nullptr;
	CWall_Collision*	m_pWall = nullptr;

	CTransform*			m_pPlayerTrans = nullptr;

	PL_STATE			m_ePlayerCurState = STATE_END;
	PL_STATE			m_ePlayerPreState = STATE_END;

	list<CBoxCollider*>		m_listEnemyDamagedCol;

public:
	virtual void Free() override;
};

