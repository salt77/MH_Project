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
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);
	void			Key_Input(const _float& fTimeDelta);
	void			Action_Change();
	void			SetUp_OnTerrain(void);
	_vec3			PickUp_OnTerrain(void);
	void			Rotate_PlayerLook(const _float& fTimeDelta, _vec3& TargetLookVector);

private:
	_bool			m_bIsPlayer = true;

	_uint			m_iAniIndex = 31;		// Idle

	_float			m_fSpeed = 3.5f;

	_vec3			m_vDir;
	_vec3			m_vRightDir;

	PL_ACTION		m_eCurAction = PL_IDLE;

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

#endif // Player_h__
