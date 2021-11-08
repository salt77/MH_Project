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

#define	SoundMgr(SoundName, ChannelID)		CSoundMgr::GetInstance()->PlaySound(SoundName, ChannelID);
#define SoundMgrBGM(SoundName, ChannelID)	CSoundMgr::GetInstance()->StopSound(ChannelID);				\
											CSoundMgr::GetInstance()->PlayBGM(SoundName, ChannelID);
#define SoundMgrStop					    CSoundMgr::GetInstance()->StopAll();
#define SoundMgrStopChannel(ChannelID)		CSoundMgr::GetInstance()->StopSound(ChannelID);

#define SoundMgrHalfVol(SoundName, ChannelID)			CSoundMgr::GetInstance()->PlaySoundLowerVol(SoundName, ChannelID);
#define SoundMgrLowerVol(SoundName, ChannelID, Volume)	CSoundMgr::GetInstance()->PlaySoundLowerVol(SoundName, ChannelID, Volume);
#define SoundMgrHigherVol(SoundName, ChannelID, Volume) CSoundMgr::GetInstance()->PlaySoundHigherVol(SoundName, ChannelID, Volume);


#define DIS_SHORTEST	3.25f
#define DIS_SHORT		5.f
#define DIS_MID			10.f
#define DIS_LONG		15.f

#define SCALE_PLAYER	0.01f
#define SCALE_AHGLAN	0.07f

#define CAMSHAKE_POWER	0.025f

#define AHGLAN_ATKPOWER	1200

#define PLAYER_ATKPOWER 450
#define PLAYER_SMASH1POWER	3200
#define PLAYER_SMASH2POWER	2800
#define PLAYER_SMASH2BPOWER	3200
#define PLAYER_SMASH3POWER	4800
#define PLAYER_SMASH4POWER	6500
#define PLAYER_SMASHPOWER2	9000

#define FADEOUTSPEED		0.0005f
#define PROGRESSBARSPEED	0.00005f;

#define LERP_PLAYERHPBAR	2.f

#define	INFINITY_INT		999999
#define INFINITY_FLOAT		999999.f






// ���� ��ũ�� => �ڵ差�� �ʹ� ������� ���� �Ⱦ ����� ���� ��ũ�� ��Ƴ���
#define SoundPlayerHurt										\
iRandSound = rand() % 4;									\
if (0 == iRandSound)										\
{															\
	SoundMgr(L"lethita_hurt1.wav", CSoundMgr::PLAYER);		\
}															\
else if (1 == iRandSound)									\
{															\
	SoundMgr(L"lethita_hurt2.wav", CSoundMgr::PLAYER);		\
}															\
else if (2 == iRandSound)									\
{															\
	SoundMgr(L"lethita_hurt3.wav", CSoundMgr::PLAYER);		\
}															\
else if (3 == iRandSound)									\
{															\
	SoundMgr(L"lethita_hurt4.wav", CSoundMgr::PLAYER);		\
}

#define	SoundPlayerStrongAtk										\
iRandSound = rand() % 5;											\
if (0 == iRandSound)												\
{																	\
	SoundMgr(L"lethita_strongattack1.wav", CSoundMgr::PLAYER);		\
}																	\
else if (1 == iRandSound)											\
{																	\
	SoundMgr(L"lethita_strongattack2.wav", CSoundMgr::PLAYER);		\
}																	\
else if (2 == iRandSound)											\
{																	\
	SoundMgr(L"lethita_strongattack3.wav", CSoundMgr::PLAYER);		\
}																	\
else if (3 == iRandSound)											\
{																	\
	SoundMgr(L"lethita_strongattack4.wav", CSoundMgr::PLAYER);		\
}																	\
else if (4 == iRandSound)											\
{																	\
	SoundMgr(L"lethita_strongattack5.wav", CSoundMgr::PLAYER);		\
}

#define SoundPlayerAtk												\
iRandSound = rand() % 8;											\
if (0 == iRandSound)												\
{																	\
	SoundMgr(L"lethita_attack1.wav", CSoundMgr::PLAYER);			\
}																	\
else if (1 == iRandSound)											\
{																	\
	SoundMgr(L"lethita_attack2.wav", CSoundMgr::PLAYER);			\
}																	\
else if (2 == iRandSound)											\
{																	\
	SoundMgr(L"lethita_attack3.wav", CSoundMgr::PLAYER);			\
}																	\
else if (3 == iRandSound)											\
{																	\
	SoundMgr(L"lethita_attack4.wav", CSoundMgr::PLAYER);			\
}																	\
else if (4 == iRandSound)											\
{																	\
	SoundMgr(L"lethita_attack5.wav", CSoundMgr::PLAYER);			\
}																	\
else if (5 == iRandSound)											\
{																	\
	SoundMgr(L"lethita_attack6.wav", CSoundMgr::PLAYER);			\
}																	\
else if (6 == iRandSound)											\
{																	\
	SoundMgr(L"lethita_attack7.wav", CSoundMgr::PLAYER);			\
}																	\
else if (7 == iRandSound)											\
{																	\
	SoundMgr(L"lethita_attack8.wav", CSoundMgr::PLAYER);			\
}																	


#define SoundGolemAtk											\
iRandSound = rand() % 2;										\
if (0 == iRandSound)											\
{																\
	SoundMgr(L"golem_attack01.wav", CSoundMgr::MONSTER);		\
}																\
else                                                            \
{																\
	SoundMgr(L"golem_attack02.wav", CSoundMgr::MONSTER);		\
}
	


																
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

// �ܺο��� ��밡���� �������� : ��������
// ���ο��� ��밡���� �������� : static ����

#endif // Define_h__
