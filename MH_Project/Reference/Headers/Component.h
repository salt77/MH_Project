#ifndef Component_h__
#define Component_h__

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CComponent : public CBase
{
protected:
	explicit CComponent(void);
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent(void);

public:
	virtual _int			Update_Component(const _float& fTimeDelta) { return 0; }

protected:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	_bool					m_bClone;

public:
	virtual CComponent*		Clone(void) PURE;
	virtual void			Free(void);
};


END
#endif // Component_h__
