#ifndef NaviMesh_h__
#define NaviMesh_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNaviMesh : public CComponent
{
private:
	explicit CNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CNaviMesh(const CNaviMesh& rhs);
	virtual ~CNaviMesh(void);

public:
	HRESULT			Ready_NaviMesh(void);

private:
	HRESULT			Link_Cell(void);

public:
	static CNaviMesh*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent*		Clone(void);
	virtual void			Free(void);
};

END
#endif // NaviMesh_h__
