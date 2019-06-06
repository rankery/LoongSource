#include "StdAfx.h"

#include ".\soundmanager.h"

#include "FModEx\fmod.hpp"
#include "FModEx\fmod_errors.h"

#include "SoundManagerMember.h"

#define ERROR_CHECK(result) if((result) != FMOD_OK) { LogFMODError(result); return; }
#define ERROR_CHECK_R(result) if((result) != FMOD_OK) { LogFMODError(result); return SOUND_INVALID; }


const int SOUND_INVALID = -1;
SoundManager::SoundManager(void)
{
	m_p = new Member;
}

SoundManager::~SoundManager(void)
{
}

SoundManager* SoundManager::Inst()
{
	static SoundManager s_soundManager;
	return &s_soundManager;
}

void SoundManager::Create()
{
	Kernel::Inst()->SetSoundCallBack(this);

	//--
	FMOD::System *pSys = NULL;

	//--create system object and initialize
	FMOD_RESULT result;
	result = FMOD::System_Create(&pSys);
	ERROR_CHECK(result);

	result = pSys->setSoftwareChannels(20);
	ERROR_CHECK(result);

	result = pSys->init(256, FMOD_INIT_NORMAL, 0);
	ERROR_CHECK(result);

	//Set the distance units. (meters/feet etc).
	//result = pSys->set3DSettings(1.0f, 0.01f, 1.0f);
	ERROR_CHECK(result);

	//--´´½¨sound group
	result = pSys->createChannelGroup("_f3d_sound",&m_p->pSoundGroup);
	ERROR_CHECK(result);

	FMOD::ChannelGroup *masterGroup = NULL;
	result = pSys->getMasterChannelGroup(&masterGroup);
	ERROR_CHECK(result);

	result = masterGroup->addGroup(m_p->pSoundGroup);
	ERROR_CHECK(result);

	m_p->pSystem = pSys;
	LOG(_T("SoundManager create ok"));
}

bool SoundManager::IsCreated()
{
	return m_p->pSystem != NULL;
}

void SoundManager::Destroy()
{
	if(m_p->pSystem)
	{
		m_p->pSystem->close();
		m_p->pSystem->release();
	}
}

void SoundManager::Update()
{
	if(m_p->pSystem == NULL)
		return;

	FMOD_RESULT result = m_p->pSystem->update();
	ERROR_CHECK(result);
}

void SoundManager::GetSystemState(float& cpuUsage,int& mem)
{
	cpuUsage = 0;
	mem = 0;

	if(m_p->pSystem == NULL)
		return;

	float dsp, stream, update;
	m_p->pSystem->getCPUUsage(&dsp,&stream,&update,&cpuUsage);

	int maxAlloced = 0;
	FMOD::Memory_GetStats(&mem,&maxAlloced);
}

void SoundManager::SetMusicVolume(float vol)
{
	if(m_p->pSystem == NULL)
		return;

	m_p->musicVolume = vol;
	
	if(m_p->pMusicChannel)
		m_p->pMusicChannel->setVolume(vol);
}

void SoundManager::SetSoundVolume(float vol)
{
	if(m_p->pSystem == NULL)
		return;

	m_p->soundVolume = vol;

	if(m_p->pSoundGroup)
		m_p->pSoundGroup->setVolume(vol);
}

void SoundManager::PlayMusic(const char* szFileName)
{
	if(m_p->pSystem == NULL)
		return;

	//--release old
	StopMusic();
	
	BOOL workDirOK=::SetCurrentDirectory(Kernel::Inst()->GetAppPath());
	//--load new
	FMOD_RESULT result;
	result = m_p->pSystem->createStream(szFileName,FMOD_LOOP_NORMAL|FMOD_2D,NULL,&m_p->pMusicSound);
	ERROR_CHECK(result);

	//--play
	result = m_p->pSystem->playSound(FMOD_CHANNEL_FREE,m_p->pMusicSound,true,&m_p->pMusicChannel);
	ERROR_CHECK(result);

	result = m_p->pMusicChannel->setVolume(m_p->musicVolume);
	ERROR_CHECK(result);

	result = m_p->pMusicChannel->setPriority(Member::EP_Music);
	ERROR_CHECK(result);

	result = m_p->pMusicChannel->setPaused(false);
	ERROR_CHECK(result);
}

void SoundManager::StopMusic()
{
	if(m_p->pSystem == NULL)
		return;

	if(m_p->pMusicSound)
	{
		m_p->pMusicSound->release();
		m_p->pMusicSound = NULL;
		m_p->pMusicChannel = NULL;
	}
}

void SoundManager::SetListener(Vector3 pos,Vector3 vel,Vector3 forward,Vector3 up)
{
	if(m_p->pSystem == NULL)
		return;

	FMOD_VECTOR listener_pos = {pos.x,pos.y,pos.z};
	FMOD_VECTOR listener_forward = {forward.x,forward.y,forward.z};
	FMOD_VECTOR listener_up = {up.x,up.y,up.z};
	FMOD_VECTOR listener_vel = {vel.x,vel.y,vel.z};


	FMOD::System *pSys = m_p->pSystem;

	FMOD_RESULT result;
	result = pSys->set3DListenerAttributes(0, &listener_pos, &listener_vel,
		&listener_forward,&listener_up);

	ERROR_CHECK(result);
}

void SoundManager::ClearEnvironmentSound()
{
	if(m_p->pSystem == NULL)
		return;

	m_p->FreeAll3DEnvSound();

}

void SoundManager::AddEnvironmentSound(const char* szFileName,Vector3 pos,float minDist,float maxDist)
{
	if(m_p->pSystem == NULL)
		return;

	BOOL workDirOK=::SetCurrentDirectory(Kernel::Inst()->GetAppPath());
	//--
	FMOD::System *pSys = m_p->pSystem;
	FMOD::Sound		*pSound = NULL;
	FMOD::Channel	*pChannel = NULL;

	FMOD_RESULT result;

	result = pSys->createSound(szFileName,FMOD_SOFTWARE|FMOD_3D,NULL,&pSound);
	ERROR_CHECK(result);

	result = pSound->set3DMinMaxDistance(minDist,maxDist);
	ERROR_CHECK(result);

	result = pSound->setMode(FMOD_LOOP_NORMAL);
	ERROR_CHECK(result);

	result = pSys->playSound(FMOD_CHANNEL_FREE,pSound,true,&pChannel);
	ERROR_CHECK(result);
	
	FMOD_VECTOR fpos = {pos.x,pos.y,pos.z};
	result = pChannel->set3DAttributes(&fpos,NULL);
	ERROR_CHECK(result);

	result = pChannel->setChannelGroup(m_p->pSoundGroup);
	ERROR_CHECK(result);

	result = pChannel->setVolume(m_p->soundVolume);
	ERROR_CHECK(result);

	result = pChannel->setPriority(Member::EP_3DEnv);
	ERROR_CHECK(result);

	result = pChannel->setPaused(false);
	ERROR_CHECK(result);

	//--
	SoundInfo snd(pSound,pChannel);
	m_p->sound3dEnvList.push_back(snd);
}

int SoundManager::GetNum3DEnvSoundPlaying()
{
	if(m_p->pSystem == NULL)
		return 0;

	int ret = 0;
	for(std::list<SoundInfo>::iterator iter = m_p->sound3dEnvList.begin();
		iter != m_p->sound3dEnvList.end(); ++iter)
	{
		FMOD::Sound		*pSound = (iter->first);
		FMOD::Channel	*pChannel = (iter->second);

		FMOD::Sound *pTestSound = NULL;
		pChannel->getCurrentSound(&pTestSound);

		if(pTestSound == pSound)
		{
			bool bVirtual = true;
			pChannel->isVirtual(&bVirtual);
			
			if(!bVirtual)
				ret++;
		}
	}

	return ret;
}

int  SoundManager::Create3DEventSound(const char* szFileName,bool bLoop,float minDist,float maxDist)
{
	if(m_p->pSystem == NULL)
		return SOUND_INVALID;

	BOOL workDirOK=::SetCurrentDirectory(Kernel::Inst()->GetAppPath());

	FMOD::System *pSys = m_p->pSystem;
	FMOD::Sound		*pSound = NULL;
	FMOD::Channel	*pChannel = NULL;

	FMOD_RESULT result;

	result = pSys->createSound(szFileName,FMOD_SOFTWARE|FMOD_3D,NULL,&pSound);
	ERROR_CHECK_R(result);

	result = pSound->set3DMinMaxDistance(minDist,maxDist);
	ERROR_CHECK_R(result);

	if(bLoop)
	{
		result = pSound->setMode(FMOD_LOOP_NORMAL);
		ERROR_CHECK_R(result);
	}

	result = pSys->playSound(FMOD_CHANNEL_FREE,pSound,true,&pChannel);
	ERROR_CHECK_R(result);

	result = pChannel->setChannelGroup(m_p->pSoundGroup);
	ERROR_CHECK_R(result);

	result = pChannel->setVolume(m_p->soundVolume);
	ERROR_CHECK_R(result);

	result = pChannel->setPriority(Member::EP_3DEvent);
	ERROR_CHECK_R(result);

	//--
	SoundInfo snd(pSound,pChannel);
	int id = m_p->GenID();
	m_p->sound3DEventMap.insert(make_pair(id,snd));
	

	return id;
}

void SoundManager::Destroy3DEventSound(int id)
{
	if(m_p->pSystem == NULL)
		return;
	if(id == SOUND_INVALID)
		return;

	map<int,SoundInfo>::iterator iter = m_p->sound3DEventMap.find(id);
	if(iter != m_p->sound3DEventMap.end())
	{
		SoundInfo& snd = iter->second;

		snd.first->release();

		m_p->sound3DEventMap.erase(iter);
	}
}

void SoundManager::Play3DEventSound(int id)
{
	if(m_p->pSystem == NULL)
		return;
	if(id == SOUND_INVALID)
		return;
	
	map<int,SoundInfo>::iterator iter = m_p->sound3DEventMap.find(id);
	if(iter != m_p->sound3DEventMap.end())
	{
		SoundInfo& snd = iter->second;
		
		snd.second->setPaused(false);
		snd.second->setPosition(0,FMOD_TIMEUNIT_MS);
	}
}

void SoundManager::Stop3DEventSound(int id)
{
	if(m_p->pSystem == NULL)
		return;
	if(id == SOUND_INVALID)
		return;

	map<int,SoundInfo>::iterator iter = m_p->sound3DEventMap.find(id);
	if(iter != m_p->sound3DEventMap.end())
	{
		SoundInfo& snd = iter->second;

		snd.second->setPaused(true);
	}
}

void SoundManager::Set3DEventSoundAtt(int id,Vector3 pos,Vector3 vel)
{
	if(m_p->pSystem == NULL)
		return;
	if(id == SOUND_INVALID)
		return;

	map<int,SoundInfo>::iterator iter = m_p->sound3DEventMap.find(id);
	if(iter != m_p->sound3DEventMap.end())
	{
		SoundInfo& snd = iter->second;

		FMOD_VECTOR fpos = {pos.x,pos.y,pos.z};
		FMOD_VECTOR fvel = {vel.x,vel.y,vel.z};

		snd.second->set3DAttributes(&fpos,&fvel);
	}
}

void SoundManager::StopAll3DEventSound()
{
	if(m_p->pSystem == NULL)
		return;
	
	m_p->FreeAll3DEventSound();
}

void SoundManager::Play2DEventSound(const char* szFileName)
{
	if(m_p->pSystem == NULL)
		return;

	BOOL workDirOK=::SetCurrentDirectory(Kernel::Inst()->GetAppPath());

	//--garbage collection
	m_p->FreeStoped2DSound();

	//--
	FMOD::System *pSys = m_p->pSystem;
	FMOD::Sound		*pSound = NULL;
	FMOD::Channel	*pChannel = NULL;

	FMOD_RESULT result;

	result = pSys->createSound(szFileName,FMOD_DEFAULT,NULL,&pSound);
	ERROR_CHECK(result);

	result = pSys->playSound(FMOD_CHANNEL_FREE,pSound,true,&pChannel);
	ERROR_CHECK(result);

	result = pChannel->setChannelGroup(m_p->pSoundGroup);
	ERROR_CHECK(result);

	pChannel->setVolume(m_p->soundVolume);
	ERROR_CHECK(result);

	result = pChannel->setPriority(Member::EP_2DEvent);
	ERROR_CHECK(result);

	result = pChannel->setPaused(false);
	ERROR_CHECK(result);

	//--add to list
	SoundInfo snd(pSound,pChannel);
	m_p->sound2dList.push_back(snd);
}

void SoundManager::StopAll2DSound()
{
	if(m_p->pSystem == NULL)
		return;

	for(std::list<SoundInfo>::iterator iter=m_p->sound2dList.begin();
		iter != m_p->sound2dList.end();++iter)
	{
		const SoundInfo& snd = (*iter);

		snd.first->release();
	}

	m_p->sound2dList.clear();
}

int SoundManager::GetNum2DSoundPlaying()
{
	return (int)m_p->sound2dList.size();
}
