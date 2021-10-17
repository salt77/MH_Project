#ifndef GameObject_h__
#define GameObject_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject(void);


public:
	CComponent*		Get_Component(const wstring pComponentTag, COMPONENTID eID);

public:
	virtual		HRESULT		Ready_Object(void);
	virtual		_int		Update_Object(const _float& fTimeDelta);
	virtual		void		Render_Object(void) {};

protected:
	LPDIRECT3DDEVICE9					m_pGraphicDev;
	map<const wstring, CComponent*>		m_mapComponent[ID_END];

private:
	CComponent*		Find_Component(const wstring pComponentTag, COMPONENTID eID);

public:
	virtual void	Free(void);

};

END
#endif // GameObject_h__
