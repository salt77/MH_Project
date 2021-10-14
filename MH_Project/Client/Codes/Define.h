#ifndef Define_h__
#define Define_h__

#define	WINCX	800
#define	WINCY	600

extern	HWND		 g_hWnd;
extern	HINSTANCE	 g_hInst;

//#ifdef _DEBUG
//
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifndef DBG_NEW 
//
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 
//
//#endif
//#endif

// 외부에서 사용가능한 정적변수 : 전역변수
// 내부에서 사용가능한 정적변수 : static 변수

#endif // Define_h__
