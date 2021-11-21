#ifndef Soldier_Info_h__
#define Soldier_Info_h__

#include "Spawn_Info.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;

END

class CSoldier_Info final : public CSpawn_Info
{
private:
	explicit CSoldier_Info(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSoldier_Info(const CSoldier_Info& rhs);
	virtual ~CSoldier_Info(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);
	virtual void	Contact(const _float& fTimeDelta);

//private:
//	_bool			m_bContact = false;
//
//	_float			m_fAreaLimit = DIS_SHORT;
//	_float			m_fContactPlayerDis = DIS_MID;
//
//	_vec3			m_vSpawnPos = _vec3(0.f, 0.f, 0.f);
//	_vec3			m_vRotateInfo = _vec3(0.f, 0.f, 0.f);
//
//	CTransform*		m_pTransformCom = nullptr;
//	CTransform*		m_pPlayerTrans = nullptr;

public:
	static CSoldier_Info*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free(void);
};

#endif // Soldier_Info_h__
