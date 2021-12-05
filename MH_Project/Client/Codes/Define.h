#ifndef Define_h__
#define Define_h__

#define	WINCX	1400
#define	WINCY	900

extern	HWND		 g_hWnd;
extern	HINSTANCE	 g_hInst;


#pragma region HitBox_Control_Macro 

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

#pragma endregion


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
	if (m_tPlayerInfo.iSkillPoint + (rand() % 50 + 20) < m_tPlayerInfo.iMaxSkillPoint)	\
	{																					\
		m_tPlayerInfo.iSkillPoint += rand() % 50 + 20;									\
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
#define DIS_SMASH4B		3.75f
#define DIS_SHORT		5.f
#define DIS_MID			10.f
#define DIS_LONG		15.f

#define SCALE_PLAYER	0.01f
#define SCALE_AHGLAN	0.07f
#define SCALE_NORMAL	0.01f
#define SCALE_MANKIND	0.012f
#define SCALE_BALISTA	0.03f
#define SCALE_ITEM		29.5f
#define SCALE_ICON		30.f
#define SCALE_TOOLTIP	275.f

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

#define STAGE_BALISTA_POWER	40000

#define PLAYER_SP_FEVER		100
#define PLAYER_SP_STAND		300
#define PLAYER_SP_FURY_NO7	750

#define FADEOUTSPEED		0.0005f
#define PROGRESSBARSPEED	0.05f

#define LERP_PLAYERHPBAR	0.008f

#define	INFINITY_INT		999999
#define INFINITY_FLOAT		999999.f

#define SCREEN_CENTER_X		WINCX * 0.5f
#define SCREEN_CENTER_Y		WINCY * 0.5f

#define LOADINGBAR_Y		WINCY * 0.88f
#define BOSS_HPBAR_Y		WINCY * 0.15f
#define BOSS_HPBAR_SCALE_X	WINCX * 0.52f
#define BOSS_HPBAR_SCALE_Y	35.f


#pragma region Define_Postion 

#define POOLING_POS				_vec3(0.f, -999.f, 0.f)
#define STAGE_BALISTA_POS		_vec3(700.f, 5.f, 250.f)
#define STAGE_BALISTA_TARGET	_vec3(-0.98f, 5.f, 7.12f)
#define TRAP_POS				_vec3(71.672f, 2.6f, 39.88f)
#define	WALL_COLLIDER_POS		_vec3(71.55f, 2.6f, 7.89f)
#define WALL_SYMBOL_POS			_vec3(-0.42f, 5.5f, 68.93f)
#define LASTROOM_POS			_vec3(2.5f, 3.f, 61.28f)
#define BOX_POS					_vec3(68.f, 2.6f, 11.5f)
#define BOX_2_POS				_vec3(72.f, 2.6f, 15.f)
#define BALISTA_TRIGGER_START	_vec3(61.4f, 2.6f, 6.47f)
#define BALISTA_TRIGGER_END		_vec3(66.74f, 2.6f, 40.f)

#pragma endregion


#pragma region Object_Pooling_Count

#define DAMAGEFONT_COUNT	40
#define DAMAGEFONT_SKILL_COUNT	40
#define DAMAGEFONT_MONSTER_COUNT 12

#define DOG_COUNT		6
#define SOLDIER_COUNT	6
#define KNIGHT_COUNT	4

#define	BALISTA_COUNT		8
#define STAGE_BALISTA_COUNT	64

#define SLASHPOINT_COUNT	8

#pragma endregion




#pragma region Sound_Macro

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

#pragma endregion


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
