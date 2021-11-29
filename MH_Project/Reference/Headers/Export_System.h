#ifndef Export_System_h__
#define Export_System_h__

#include "GraphicDev.h"
#include "TimerMgr.h"
#include "FrameMgr.h"
#include "FontMgr.h"
#include "InputDev.h"
#include "KeyMgr.h"

BEGIN(Engine)

// GraphicDev
inline HRESULT	Ready_GraphicDev(HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, CGraphicDev** ppGraphicDev);
inline void		Render_Begin(D3DXCOLOR Color);
inline void		Render_End(void);

// TimerMgr
inline _float		Get_TimeDelta(const wstring pTimerTag);
inline void			Set_TimeDelta(const wstring pTimerTag);
inline HRESULT		Ready_Timer(const wstring pTimerTag);

// FrameMgr
inline	_bool		IsPermit_Call(const wstring pFrameTag, const _float& fTimeDelta);
inline 	HRESULT		Ready_Frame(const wstring pFrameTag, const _float& fCallLimit);

// FontMgr
inline HRESULT	Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
						   const wstring pFontTag,
						   const wstring pFontType,
						   const _uint& iWidth,
						   const _uint& iHeight,
						   const _uint& iWeight);

inline void	Render_Font(const wstring pFontTag,
						const wstring pString,
						const _vec2* pPos,
						D3DXCOLOR Color);


// InputDev
inline _byte	Get_DIKeyState(_ubyte byKeyID);
inline _byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
inline _long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
inline HRESULT	Ready_InputDev(HINSTANCE hInst, HWND hWnd);
inline void		Update_InputDev(void);

// KeyMgr
inline _bool	Key_Pressing(int _key);
inline _bool	Key_Down(int _key);
inline _bool	Key_Up(int _key);


inline void		Release_System(void);

#include "Export_System.inl"

END
#endif // Export_System_h__
