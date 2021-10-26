#include "KeyMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CKeyMgr)

CKeyMgr::CKeyMgr()
{
}

CKeyMgr::~CKeyMgr()
{
	Free();
}


_bool CKeyMgr::Key_Pressing(int _key)
{
	if (GetAsyncKeyState(_key) & 0x8000)
		return true;

	return false;
}

_bool CKeyMgr::Key_Down(int _key)
{
	if (!m_bKeyState[_key] && (GetAsyncKeyState(_key) & 0x8000))
	{
		m_bKeyState[_key] = !m_bKeyState[_key];
		return true;
	}

	if (m_bKeyState[_key] && !(GetAsyncKeyState(_key) & 0x8000))
		m_bKeyState[_key] = !m_bKeyState[_key];

	return false;
}

_bool CKeyMgr::Key_Up(int _key)
{
	if (m_bKeyState[_key] && !(GetAsyncKeyState(_key) & 0x8000))
	{
		m_bKeyState[_key] = !m_bKeyState[_key];
		return true;
	}

	if (!m_bKeyState[_key] && (GetAsyncKeyState(_key) & 0x8000))
		m_bKeyState[_key] = !m_bKeyState[_key];

	return false;
}


void CKeyMgr::Free(void)
{
}
