#ifndef ProtoMgr_h__
#define ProtoMgr_h__

#include "Engine_Define.h"
#include "Base.h"

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CProtoMgr : public CBase
{
	DECLARE_SINGLETON(CProtoMgr)

private:
	explicit CProtoMgr(void);
	virtual ~CProtoMgr(void);

public:
	HRESULT		Ready_Prototype(const wstring pProtoTag, CComponent* pInstance);
	CComponent* Clone_Prototype(const wstring pProtoTag);
	HRESULT		Delete_Prototype(const wstring pProtoTag);

private:
	CComponent*	Find_Prototype(const wstring pProtoTag);

private:
	map<const wstring, CComponent*>		m_mapPrototype;

public:
	virtual void	Free(void);
};

END
#endif // ProtoMgr_h__
