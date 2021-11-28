#pragma once

#ifndef __SOUNDMGR_H__
#define __SOUNDMGR_H__

#include "Base.h"
#include "Export_Utility.h"

USING(Engine)

class CSoundMgr : public CBase
{
	DECLARE_SINGLETON(CSoundMgr)

public:
	enum CHANNELID
	{
		BGM,
		PLAYER,
		MONSTER,
		MONSTER2,
		MONSTER3,
		PLAYER_EFFECT,
		PLAYER_EFFECT2,
		PLAYER_EFFECT3,
		ENEMY_EFFECT,
		BATTLE, 
		BATTLE2, 
		UI,
		OBJ,

		MAXCHANNEL
	};

private:
	explicit CSoundMgr();
	virtual ~CSoundMgr();

public:
	HRESULT Ready_Sound_Manager();
	void Release_Sound_Manager();

public:
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID);
	void PlayBGM(TCHAR * pSoundKey, CHANNELID eID);
	void StopSound(CHANNELID eID);
	void StopAll();

	void GetVolume(_float* pVol) { FMOD_Channel_GetVolume(m_pChannelArr[PLAYER], pVol); }
	void SetVolume(float fVolume);



	void PlaySoundHigherVol(TCHAR* pSoundKey, CHANNELID eID, _float fVol = 2.5f);
	void PlaySoundLowerVol(TCHAR* pSoundKey, CHANNELID eID, _float fVol = 0.075f);

private:
	void LoadSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	int				m_iChannel = 13; //하나의 채널을 몇개로 나누어 관리하는지, 1이 기본값
	FMOD_CHANNEL*	m_pChannelArr[MAXCHANNEL * 13]; // iChannel이랑 개수를 맞춰준다
	int				m_iChannelCount[MAXCHANNEL] = { 1 };

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM*	m_pSystem;

	bool			m_bEnable;

public:
	virtual void Free() override;
};

#endif //__SOUNDMGR_H__