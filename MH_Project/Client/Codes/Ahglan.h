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

END

enum STATE
{
	WALK, TURN_RIGHT, TURN_LEFT, TAUNT, SPAWN, LOW_HEALTH, IDLE,
	ATK_WINDMILL, ATK_TWOHANDS_COMBO, ATK_TWOHANDS, ATK_TURNRIGHT, ATK_TURNLEFT, ATK_STAMP, ATK_ROLLING_TWICE,
	ATK_ROLLING_ONETIME_END, ATK_ROLLING_ONETIME_BEGIN, ATK_ONEHAND,
	ENTRY_IDLE, ENTRY_CONTACT,
	STATE_END
};

class CAhglan : public CGameObject
{
public:
	explicit CAhglan(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAhglan();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	// 기본 함수들
	HRESULT			Add_Component(void);

public:
	HRESULT			Add_Collider(_float fRadius, wstring cstrName, COLLIDERTYPE eColliderType = COLLIDERTYPE::COLTYPE_SPHERE_DAMAGED);
	HRESULT			Add_Collider(_float vMinX, _float vMinY, _float vMinZ,
								 _float vMaxX, _float vMaxY, _float vMaxZ,
								 wstring wstrName, COLLIDERTYPE eColliderType);
	HRESULT			Add_NaviMesh(_uint iCellCount, vector<_matrix> vecPoint);

private:
	_uint			m_iAniIndex = 17;		// Entry Idle

	_float			m_fSpeed = 3.f;
	_float			m_fAniTime = 0.f;

	_vec3			m_vDir;

	// Component
	CDynamicMesh*	m_pMeshCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;
	CNaviMesh*		m_pNaviMeshCom = nullptr;

public:
	static CAhglan*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);
};

#endif //__AHGLAN_H__