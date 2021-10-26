#pragma once

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

#define VK_MAX 0xFF

class ENGINE_DLL CKeyMgr : public CBase
{
	DECLARE_SINGLETON(CKeyMgr)

private:
	explicit CKeyMgr();
	virtual ~CKeyMgr();

public:
	_bool	Key_Pressing(int _key);
	_bool	Key_Down(int _key);
	_bool	Key_Up(int _key);

private:
	_bool	m_bKeyState[VK_MAX];

public:
	virtual void Free(void) override;
};

END