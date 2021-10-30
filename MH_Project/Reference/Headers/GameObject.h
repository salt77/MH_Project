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

	const map<const wstring, CCollider*>&		Get_MapCollider() { return m_mapColliderCom; }
	const map<const wstring, CBoxCollider*>&	Get_MapBoxCollider() { return m_mapBoxColliderCom; }

public:
	virtual		HRESULT		Ready_Object(void);
	virtual		_int		Update_Object(const _float& fTimeDelta);
	virtual		void		Render_Object(void) {};

public:
	HRESULT			Add_Collider(_float fRadius, wstring wstrName, COLLIDERTYPE eColliderType);
	HRESULT			Add_Collider(_float vMinX, _float vMinY, _float vMinZ,
								 _float vMaxX, _float vMaxY, _float vMaxZ,
								 wstring wstrName, COLLIDERTYPE eColliderType);

protected:
	LPDIRECT3DDEVICE9					m_pGraphicDev;
	map<const wstring, CComponent*>		m_mapComponent[ID_END];
	map<const wstring, CCollider*>		m_mapColliderCom;
	map<const wstring, CBoxCollider*>	m_mapBoxColliderCom;

private:
	CComponent*		Find_Component(const wstring pComponentTag, COMPONENTID eID);

public:
	virtual void	Free(void);

};

END
#endif // GameObject_h__
