#include "stdafx.h"
#include "SoundMgr.h"


IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr()
{
	m_pSystem = nullptr;
}

CSoundMgr::~CSoundMgr()
{
	Free();
}


HRESULT CSoundMgr::Ready_Sound_Manager()
{
	FMOD_System_Create(&m_pSystem);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	//FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);
	FMOD_System_Init(m_pSystem, MAXCHANNEL * m_iChannel, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile();

	return S_OK;
}

void CSoundMgr::Release_Sound_Manager()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();
	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

void CSoundMgr::PlaySound(TCHAR * pSoundKey, CHANNELID eID)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	if (!lstrcmp(pSoundKey, L"Hit_HardFlesh_StrongSlash.wav"))
	{
		int a = 0;
	}

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID * m_iChannel + m_iChannelCount[eID]], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID * m_iChannel + m_iChannelCount[eID]]);
		FMOD_Channel_SetVolume(m_pChannelArr[eID * m_iChannel + m_iChannelCount[eID]], 0.1f);
	}
	FMOD_System_Update(m_pSystem);

	m_iChannelCount[eID]++;
	if (m_iChannelCount[eID] >= m_iChannel) m_iChannelCount[eID] = 1;

	//map<TCHAR*, FMOD_SOUND*>::iterator iter;

	//iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	//{
	//	return !lstrcmp(pSoundKey, iter.first);
	//});

	//if (iter == m_mapSound.end())
	//	return;

	//FMOD_BOOL bPlay = FALSE;
	//if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	//{
	//	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID]);
	//}
	//FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayBGM(TCHAR * pSoundKey, CHANNELID eID)
{
	//m_iChannelCount[MAXCHANNEL]

	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID * m_iChannel + m_iChannelCount[eID]]);
	FMOD_Channel_SetVolume(m_pChannelArr[eID * m_iChannel + m_iChannelCount[eID]], 0.085f);
	FMOD_Channel_SetMode(m_pChannelArr[eID * m_iChannel + m_iChannelCount[eID]], FMOD_LOOP_NORMAL);
	FMOD_System_Update(m_pSystem);

	m_iChannelCount[eID]++;
	if (m_iChannelCount[eID] >= m_iChannel) m_iChannelCount[eID] = 0;

	//map<TCHAR*, FMOD_SOUND*>::iterator iter;

	//iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	//{
	//	return !lstrcmp(pSoundKey, iter.first);
	//});

	//if (iter == m_mapSound.end())
	//	return;

	//FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID]);
	//FMOD_Channel_SetMode(m_pChannelArr[eID], FMOD_LOOP_NORMAL);
	//FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	for (int i = 0; i < m_iChannel; i++)
	{
		FMOD_Channel_Stop(m_pChannelArr[eID + i]);
	}
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL * m_iChannel; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::SetVolume(float fVolume)
{
	////////////////////////////////
	//기본 사운드 설정

	for (int i = 0; i < CHANNELID::MAXCHANNEL/* * m_iChannel*/; i++)
	{
		FMOD_Channel_SetVolume(m_pChannelArr[i * m_iChannel + m_iChannelCount[i]], fVolume);
	}
	
	////////////////////////////////

	//FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	////////////////////////////////
}

void CSoundMgr::PlaySoundHigherVol(TCHAR * pSoundKey, CHANNELID eID, _float fVol)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID * m_iChannel + m_iChannelCount[eID]], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID * m_iChannel + m_iChannelCount[eID]]);
		FMOD_Channel_SetVolume(m_pChannelArr[eID], fVol);
	}
	FMOD_System_Update(m_pSystem);

	//m_iChannelCount[eID]++;
	//if (m_iChannelCount[eID] >= m_iChannel) m_iChannelCount[eID] = 1;
}

void CSoundMgr::PlaySoundLowerVol(TCHAR * pSoundKey, CHANNELID eID, _float fVol)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	if (!lstrcmp(pSoundKey, L"Hit_HardFlesh_StrongSlash.wav"))
	{
		int a = 0;
	}

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID * m_iChannel + m_iChannelCount[eID]], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID * m_iChannel + m_iChannelCount[eID]]);
		FMOD_Channel_SetVolume(m_pChannelArr[eID * m_iChannel + m_iChannelCount[eID]], fVol);
	}
	FMOD_System_Update(m_pSystem);

	//m_iChannelCount[eID]++;
	//if (m_iChannelCount[eID] >= m_iChannel) m_iChannelCount[eID] = 1;
}

void CSoundMgr::LoadSoundFile()
{
	_finddata_t fd;

	long handle = _findfirst("../Bin/Resource/Sound/*.*", &fd);

	if (0 == handle)
		return;

	int iResult = 0;

	char szCurPath[128] = "../Bin/Resource/Sound/";
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}

		iResult = _findnext(handle, &fd);
	}

	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}


void CSoundMgr::Free(void)
{
	Release_Sound_Manager();
}
