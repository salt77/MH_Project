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
		ENEMY_EFFECT,
		BATTLE,
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

	void SetVolume(CHANNELID eID, float fVolume);



	void PlaySoundHigherVol(TCHAR* pSoundKey, CHANNELID eID, _float fVol = 2.5f);
	void PlaySoundLowerVol(TCHAR* pSoundKey, CHANNELID eID, _float fVol = 0.5f);

private:
	void LoadSoundFile();

private:
	// ���� ���ҽ� ������ ���� ��ü 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	int				m_iChannel = 10; //�ϳ��� ä���� ��� ������ �����ϴ���, 1�� �⺻��
	FMOD_CHANNEL*	m_pChannelArr[MAXCHANNEL * 10]; // iChannel�̶� ������ �����ش�
	int				m_iChannelCount[MAXCHANNEL] = { 1 };

	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM*	m_pSystem;

	bool			m_bEnable;

public:
	virtual void Free(void) override;
};

#endif //__SOUNDMGR_H__