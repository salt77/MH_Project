#pragma once

#include "Engine_Define.h"
#include "Engine_Enum.h"
#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"

#include "Export_System.h"
#include "Export_Utility.h"


const int MAPTOOL_WINCX = 1200;
const int MAPTOOL_WINCY = 900;

const int TILECX = 130;
const int TILECY = 68;
const int TILEX = 20;
const int TILEY = 30;

extern HWND g_hWnd;

enum RENDERTYPE_MFC
{
	RENDERTYPE_MFC_SOLID, RENDERTYPE_MFC_WIREFRAME, RENDERTYPE_MFC_NONE, RENDERTYPE_MFC_END
};

enum OBJECTADD_MFC
{
	OBJECTADD_MFC_PLAYER, OBJECTADD_MFC_AHGLAN, OBJECTADD_MFC_END
};

#include "SyncManager.h"

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