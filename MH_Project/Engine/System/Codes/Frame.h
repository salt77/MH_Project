#ifndef Frame_h__
#define Frame_h__

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CFrame : public CBase
{
private:
	explicit CFrame(void);
	virtual ~CFrame(void);

public:
	_bool				IsPermit_Call(const _float& fTimeDelta);

public:
	HRESULT				Ready_Frame(const _float& fCallLimit);

private:
	_float				m_fCallLimit;			// fps 제한을 하긴 위한 조건변수
	_float				m_fAccTimeDelta;		// 누적된 시간 값

public:
	static CFrame*		Create(const _float& fCallLimit);
	virtual void		Free(void);

};

END
#endif // Frame_h__
