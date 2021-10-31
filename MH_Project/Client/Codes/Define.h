#ifndef Define_h__
#define Define_h__

#define	WINCX	1100
#define	WINCY	800

extern	HWND		 g_hWnd;
extern	HINSTANCE	 g_hInst;


#define HITBOX_CONTROLL(StartTime, EndTime, Smash)			\
for (; iter != m_mapBoxColliderCom.end(); ++iter)			\
{															\
	if (Smash)												\
	{														\
		if (L"Hit_LHandLong" == iter->first ||				\
			L"Hit_RHandLong" == iter->first)				\
		{													\
			if (StartTime <= fAniTime &&					\
				EndTime >= fAniTime)						\
			{												\
				iter->second->Set_CanCollision(true);		\
			}												\
			else                                            \
			{												\
				iter->second->Set_CanCollision(false);		\
			}												\
		}													\
		else                                                \
		{													\
			iter->second->Set_CanCollision(false);			\
		}													\
	}														\
	else                                                    \
	{														\
		if (L"Hit_LHand" == iter->first ||					\
			L"Hit_RHand" == iter->first)					\
		{													\
			if (StartTime <= fAniTime &&					\
				EndTime >= fAniTime)						\
			{												\
				iter->second->Set_CanCollision(true);		\
			}												\
			else											\
			{												\
				iter->second->Set_CanCollision(false);		\
			}												\
		}													\
		else                                                \
		{													\
			iter->second->Set_CanCollision(false);			\
		}													\
	}														\
}

#define HITBOX_CONTROLL_SPHERE(StartTime, EndTime)			\
	if (StartTime <= fAniTime &&							\
		EndTime >= fAniTime)								\
	{														\
		iter_Hit->second->Set_CanCollision(true);			\
	}														\
	else													\
	{														\
		iter_Hit->second->Set_CanCollision(false);			\
	}


#define DIS_SHORTEST	3.5f
#define DIS_SHORT		5.5f
#define DIS_MID			10.f
#define DIS_LONG		15.f

#define SCALE_PLAYER	0.01f
#define SCALE_AHGLAN	0.07f

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
