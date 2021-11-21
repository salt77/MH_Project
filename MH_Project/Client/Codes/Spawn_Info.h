#ifndef Spawn_Info_h__
#define Spawn_Info_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;

END

class CSpawn_Info abstract : public CGameObject
{
protected:
	explicit CSpawn_Info(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSpawn_Info(const CSpawn_Info& rhs);
	virtual ~CSpawn_Info(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

public:
	const _vec3*	Get_Position() { return m_pTransformCom->Get_Info(INFO_POS); }

public:
	void			Set_Contact() { m_bContact = true; }

protected:
	virtual	void	Contact(const _float& fTimeDelta) PURE;

protected:
	_bool			m_bContact = false;

	_float			m_fAreaLimit = DIS_SHORT;
	_float			m_fContactPlayerDis = DIS_MID;

	_vec3			m_vSpawnPos = _vec3(0.f, 0.f, 0.f);
	_vec3			m_vRotateInfo = _vec3(0.f, 0.f, 0.f);

	CTransform*		m_pTransformCom = nullptr;
	CTransform*		m_pPlayerTrans = nullptr;

public:
	virtual void	Free(void);
};

#endif // Spawn_Info_h__
