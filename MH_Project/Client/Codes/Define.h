#ifndef Define_h__
#define Define_h__

#define	WINCX	1300
#define	WINCY	900

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
			if (StartTime <= m_fAniTime &&					\
				m_fAniTime <= EndTime)						\
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
			if (StartTime <= m_fAniTime &&					\
				EndTime >= m_fAniTime)						\
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
	if (StartTime <= m_fAniTime &&							\
		EndTime >= m_fAniTime)								\
	{														\
		iter_Hit->second->Set_CanCollision(true);			\
	}														\
	else													\
	{														\
		iter_Hit->second->Set_CanCollision(false);			\
	}

#define GET_SPPOINT_ATK																	\
	if (m_tPlayerInfo.iSkillPoint + (rand() % 5 + 1) < m_tPlayerInfo.iMaxSkillPoint)	\
	{																					\
		m_tPlayerInfo.iSkillPoint += rand() % 5 + 1;									\
	}																					\
	else																				\
	{																					\
		m_tPlayerInfo.iSkillPoint = m_tPlayerInfo.iMaxSkillPoint;						\
	}

#define GET_SPPOINT_SMASH																\
	if (m_tPlayerInfo.iSkillPoint + (rand() % 50 + 5) < m_tPlayerInfo.iMaxSkillPoint)	\
	{																					\
		m_tPlayerInfo.iSkillPoint += rand() % 50 + 5;									\
	}																					\
	else																				\
	{																					\
		m_tPlayerInfo.iSkillPoint = m_tPlayerInfo.iMaxSkillPoint;						\
	}


#define	SoundMgr(SoundName, ChannelID)		CSoundMgr::GetInstance()->PlaySound(SoundName, ChannelID);
#define SoundMgrBGM(SoundName, ChannelID)	CSoundMgr::GetInstance()->StopSound(ChannelID);				\
											CSoundMgr::GetInstance()->PlayBGM(SoundName, ChannelID);
#define SoundMgrStop					    CSoundMgr::GetInstance()->StopAll();
#define SoundMgrStopChannel(ChannelID)		CSoundMgr::GetInstance()->StopSound(ChannelID);

#define SoundMgrHalfVol(SoundName, ChannelID)			CSoundMgr::GetInstance()->PlaySoundLowerVol(SoundName, ChannelID);
#define SoundMgrLowerVol(SoundName, ChannelID, Volume)	CSoundMgr::GetInstance()->PlaySoundLowerVol(SoundName, ChannelID, Volume);
#define SoundMgrHigherVol(SoundName, ChannelID, Volume) CSoundMgr::GetInstance()->PlaySoundHigherVol(SoundName, ChannelID, Volume);


#define DIS_FACETOFACE	1.5f
#define DIS_VERY_SHORTEST	2.f
#define DIS_SHORTEST	3.25f
#define DIS_SHORT		5.f
#define DIS_MID			10.f
#define DIS_LONG		15.f

#define SCALE_PLAYER	0.01f
#define SCALE_AHGLAN	0.07f
#define SCALE_NORMAL	0.01f
#define SCALE_MANKIND	0.012f

#define CAMSHAKE_POWER	0.025f

#define AHGLAN_ATKPOWER	1200

#define MANKIND_ATKPOWER 600

#define PLAYER_ATKPOWER 450
#define PLAYER_SMASH1POWER	3200
#define PLAYER_SMASH2POWER	2800
#define PLAYER_SMASH2BPOWER	3200
#define PLAYER_SMASH3POWER	4800
#define PLAYER_SMASH4POWER	6500
#define PLAYER_SMASHPOWER2	9000

#define PLAYER_SP_FEVER		75

#define FADEOUTSPEED		0.0005f
#define PROGRESSBARSPEED	0.05f

#define LERP_PLAYERHPBAR	0.008f

#define	INFINITY_INT		999999
#define INFINITY_FLOAT		999999.f

#define SCREEN_CENTER_X		WINCX * 0.5f

#define LOADINGBAR_Y		WINCY * 0.88f
#define BOSS_HPBAR_Y		WINCY * 0.15f
#define BOSS_HPBAR_SCALE_X	WINCX * 0.52f
#define BOSS_HPBAR_SCALE_Y	35.f

#define POOLING_POS			_vec3(0.f, -999.f, 0.f)

#define DAMAGEFONT_COUNT	40
#define DAMAGEFONT_SKILL_COUNT	40
#define DAMAGEFONT_MONSTER_COUNT 12

#define DOG_COUNT		6
#define SOLDIER_COUNT	6
#define KNIGHT_COUNT	4

#define SLASHPOINT_COUNT	8






// 사운드 매크로 => 코드량이 너무 길어지고 보기 싫어서 여기로 따로 매크로 잡아놓음
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

// 외부에서 사용가능한 정적변수 : 전역변수
// 내부에서 사용가능한 정적변수 : static 변수

#endif // Define_h__
