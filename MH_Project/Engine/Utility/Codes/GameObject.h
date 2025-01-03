#ifndef GameObject_h__
#define GameObject_h__

#include "Component.h"
#include "Collider.h"
#include "BoxCollider.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject(void);

public:
	CComponent*		Get_Component(const wstring pComponentTag, COMPONENTID eID);
	_float			Get_ViewZ(void) { return m_fViewZ; }

	const TAGINFO&	Get_TagInfo() { return m_tInfo; }
	const _bool&	Get_ArmorOn() { return m_bArmorOn; }

	const map<const wstring, CCollider*>&		Get_MapCollider() { return m_mapColliderCom; }
	const map<const wstring, CBoxCollider*>&	Get_MapBoxCollider() { return m_mapBoxColliderCom; }

public:
	void					Set_Dead() { m_bDead = true; }
	virtual		void		Set_Damage(_int iDamage);

public:
	virtual		HRESULT		Ready_Object();
	virtual		HRESULT		LateReady_Object();
	virtual		_int		Update_Object(const _float& fTimeDelta);
	virtual		_int		LateUpdate_Object(const _float& fTimeDelta);
	virtual		void		Render_Object(void) {};

public:
	void			Compute_ViewZ(const _vec3* pPos);
	HRESULT			Add_Collider(_float fRadius, wstring wstrName, const _matrix* pColliderMatrix, COLLIDERTYPE eColliderType);
	HRESULT			Add_Collider(_float vMinX, _float vMinY, _float vMinZ,
								 _float vMaxX, _float vMaxY, _float vMaxZ,
								 wstring wstrName, const _matrix* pColliderMatrix, COLLIDERTYPE eColliderType);

protected:
	LPDIRECT3DDEVICE9					m_pGraphicDev;

	map<const wstring, CComponent*>		m_mapComponent[ID_END];
	map<const wstring, CCollider*>		m_mapColliderCom;
	map<const wstring, CBoxCollider*>	m_mapBoxColliderCom;

	_bool								m_bDead = false;
	_bool								m_bLateReady = false;
	_bool								m_bSoundCheck = false;
	_bool								m_bBoss = false;
	_bool								m_bArmorOn = false;
	_bool								m_bDissolveOn = false;

	// 보스 몬스터 라인 Hp 체크용
	//_int								m_iPreHp;
	_int								m_iLineHp;
	_int								m_iMaxLineHp;

	_float								m_fViewZ;
	_float								m_fDissolveValue = 0.f;

	SCENE_ID							m_eCurSceneID = SCENE_END;

	TAGINFO								m_tInfo;

private:
	CComponent*		Find_Component(const wstring pComponentTag, COMPONENTID eID);

public:
	virtual void	Free();
};

END
#endif // GameObject_h__
