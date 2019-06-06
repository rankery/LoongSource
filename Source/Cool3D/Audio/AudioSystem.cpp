#include "stdafx.h"
#include "AudioSystem.h"
#include "FModEx\fmod.hpp"
#include "FModEx\fmod_errors.h"
#include "AudioSystemMember.h"

#include <process.h>	// for _beginthreadex
#include <mmsystem.h>

namespace Cool3D
{
	// 定义线程函数类型
	typedef UINT (WINAPI* THREADPROC)(LPVOID);

	#define ERROR_CHECK(result) if((result) != FMOD_OK) { LogFMODError(result); }
	const int SOUND_INVALID = -1;

	IFS* f_pVFS = NULL;
	FMOD_RESULT F_CALLBACK FileOpenCallback(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata);
	FMOD_RESULT F_CALLBACK FileCloseCallback(void *handle, void *userdata);
	FMOD_RESULT F_CALLBACK FileReadCallback(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata);
	FMOD_RESULT F_CALLBACK FileSeekCallback(void *handle, unsigned int pos, void *userdata);
	UINT WINAPI ThreadUpdate(AudioSystem* pThis);

	CRITICAL_SECTION f_Lock;
	CRITICAL_SECTION io_Lock;


	enum EMusicPlayState
	{
		EMPS_None		= -1,	// 没有播放
		EMPS_FadeIn		= 0,	// 淡入
		EMPS_Play,				// 正常播放
		EMPS_FadeOut			// 淡出
	};

	EMusicPlayState	f_musicPlayState;
	AudioSystem::tagCacheMusic	f_musicCache;
	DWORD			f_musicFadeTime;
	DWORD			f_musicEscapeTime;


	AudioSystem::AudioSystem(void):m_hThreadUpdate(0), m_bTerminateThreadUpdate(0)
	{
		f_musicPlayState = EMPS_None;
		f_musicFadeTime = 0;
		f_musicEscapeTime = 0;

		InitializeCriticalSection(&f_Lock);
		InitializeCriticalSection(&io_Lock);
		m_p = new Member;
	}

	AudioSystem::~AudioSystem(void)
	{
		delete m_p;
		DeleteCriticalSection(&f_Lock);
		DeleteCriticalSection(&io_Lock);
	}

	AudioSystem* AudioSystem::Inst()
	{
		static AudioSystem s_soundManager;
		return &s_soundManager;
	}

	void AudioSystem::Init()
	{
		FMOD::System *pSys = NULL;

		//--create system object and initialize
		FMOD_RESULT result;
		result = FMOD::System_Create(&pSys);
		ERROR_CHECK(result);

		result = pSys->setSoftwareChannels(100);
		ERROR_CHECK(result);

		result = pSys->init(256, FMOD_INIT_NORMAL, 0);
		ERROR_CHECK(result);

		result = pSys->setHardwareChannels(32, 64, 32, 64);
		ERROR_CHECK(result);

		//Set the distance units. (meters/feet etc).
		//result = pSys->set3DSettings(1.0f, 100.0f, 1.0f);
		//ERROR_CHECK(result);

		//--创建sound group
		result = pSys->createChannelGroup("_f3d_sound",&m_p->pSoundGroup);
		ERROR_CHECK(result);

		FMOD::ChannelGroup *masterGroup = NULL;
		result = pSys->getMasterChannelGroup(&masterGroup);
		ERROR_CHECK(result);

		result = masterGroup->addGroup(m_p->pSoundGroup);
		ERROR_CHECK(result);

		// 开启音乐播放线程
		m_hThreadUpdate = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)&ThreadUpdate, this, 0, NULL);

		m_p->pSystem = pSys;
		LOG( _T("AudioSystem create ok") );
	}

	void AudioSystem::SetFS(IFS* pVFS)
	{
		f_pVFS = pVFS;
		if(m_p->pSystem)
		{
			EnterCriticalSection(&f_Lock);
			m_p->pSystem->setFileSystem(&FileOpenCallback, &FileCloseCallback, &FileReadCallback, &FileSeekCallback, -1);
			LeaveCriticalSection(&f_Lock);
		}
	}

	bool AudioSystem::IsHWSurpport()
	{
		return m_p->pSystem != NULL;
	}

	void AudioSystem::Destroy()
	{
		// 等待线程关闭
		m_bTerminateThreadUpdate = TRUE;
		WaitForSingleObject(m_hThreadUpdate, 2000);

		if(m_p->pSystem)
		{
			StopMusic();
			m_p->pSystem->close();
			m_p->pSystem->release();
			m_p->pSystem->setFileSystem(0, 0, 0, 0, -1);
		}
	}

	void AudioSystem::Update(DWORD delta)
	{
		if( m_p->pSystem )
		{
			EnterCriticalSection(&f_Lock);
			FMOD_RESULT result = m_p->pSystem->update();
			ERROR_CHECK(result);

			// 环境音效
			Member::Sound3DEnvList::iterator iter;
			for( iter=m_p->m_sound3dEnvList.begin(); iter!=m_p->m_sound3dEnvList.end(); ++iter )
			{
				tagEnvSoundInfo& snd = *iter;

				if( snd.interval == 0 && snd.current > snd.interval )
					continue;

				if( snd.current == 0 )//开始重新播放
				{
					result = m_p->pSystem->playSound(FMOD_CHANNEL_FREE,snd.pSound,true,&snd.pChannel);
					if( result != FMOD_OK )
					{
						LogFMODError(result);
						continue;
					}

					snd.pChannel->setChannelGroup( m_p->pSoundGroup );
					snd.pChannel->setVolume( m_p->sysSoundVolume * snd.volume );
					snd.pChannel->setPriority( Member::EP_3DEnv );
					snd.pChannel->set3DAttributes( &snd.pos, NULL );
					snd.pChannel->setPosition( 0, FMOD_TIMEUNIT_MS );
					snd.pChannel->setPaused( false );

					snd.current = 1;
				}
				else
				{
					snd.current += delta;
					if( snd.current > snd.interval )
					{
						snd.current = 0;
					}
				}
			}
			LeaveCriticalSection(&f_Lock);
		}
	}

	UINT WINAPI ThreadUpdate(AudioSystem* pThis)
	{
		DWORD dwTime = timeGetTime();

		while( !pThis->m_bTerminateThreadUpdate || f_musicPlayState != EMPS_None )
		{
			DWORD delta = timeGetTime() - dwTime;
			dwTime = timeGetTime();

			EnterCriticalSection(&f_Lock);
			switch( f_musicPlayState )
			{
			case EMPS_FadeIn:	// 淡入
				{
					f_musicEscapeTime += delta;
					if( f_musicFadeTime <= f_musicEscapeTime )
					{
						f_musicPlayState = EMPS_Play;
						pThis->GetMember()->pMusicChannel->setVolume(pThis->GetMember()->sysMusicVolume*pThis->GetMember()->playMusicVolume);
					}
					else
					{
						pThis->GetMember()->pMusicChannel->setVolume((float)f_musicEscapeTime/(float)f_musicFadeTime*pThis->GetMember()->sysMusicVolume*pThis->GetMember()->playMusicVolume);
					}
				}
				break;
			case EMPS_Play:		// 播放
				{
					bool isPlaying = false;
					FMOD_RESULT result = pThis->GetMember()->pMusicChannel->isPlaying(&isPlaying);
					ERROR_CHECK(result);

					if( !isPlaying )
					{
						f_musicPlayState = EMPS_None;
					}
				}
				break;
			case EMPS_FadeOut:	// 淡出
				{
					f_musicEscapeTime += delta;
					if( f_musicFadeTime <= f_musicEscapeTime )
					{
						pThis->StopMusic();
					}
					else
					{
						pThis->GetMember()->pMusicChannel->setVolume((1.0f-(float)f_musicEscapeTime/(float)f_musicFadeTime)*pThis->GetMember()->sysMusicVolume*pThis->GetMember()->playMusicVolume);
					}
				}
				break;
			case EMPS_None:		// 空闲
				{
					if( f_musicCache.valid )
					{
						pThis->ExecuteMusic( f_musicCache );
						f_musicCache.valid = false;
					}
				}
				break;

			default:
				break;
			}
			LeaveCriticalSection(&f_Lock);

			Sleep(100);
		}

		_endthreadex(0);
		return 0;
	}

	void AudioSystem::GetSystemState(float& cpuUsage,int& mem)
	{
		cpuUsage = 0;
		mem = 0;

		if(m_p->pSystem == NULL)
			return;

		float dsp, stream, update;
		int maxAlloced = 0;

		EnterCriticalSection(&f_Lock);
		m_p->pSystem->getCPUUsage(&dsp,&stream,&update,&cpuUsage);
		FMOD::Memory_GetStats(&mem,&maxAlloced);
		LeaveCriticalSection(&f_Lock);
	}

	void AudioSystem::SetSystemMusicVolume(float vol)
	{
		m_p->sysMusicVolume = vol;
		EnterCriticalSection(&f_Lock);
		m_p->SetMusicVolume();
		LeaveCriticalSection(&f_Lock);
	}

	void AudioSystem::SetSystemSoundVolume(float vol)
	{
		m_p->sysSoundVolume = vol;
		EnterCriticalSection(&f_Lock);
		m_p->SetSoundVolume();
		LeaveCriticalSection(&f_Lock);
	}

	void AudioSystem::PlayMusic( const TCHAR* szFileName, bool bLoop, float volume, DWORD fadeinTime )
	{
		if(m_p->pSystem == NULL)
			return;

		EnterCriticalSection(&f_Lock);
		f_musicCache.valid = true;
		f_musicCache.strFileName = szFileName;
		f_musicCache.loop = bLoop;
		f_musicCache.fadeinTime = fadeinTime;
		f_musicCache.volume = volume;

		if( f_musicPlayState == EMPS_None || f_musicPlayState == EMPS_FadeIn )
		{
			ExecuteMusic(f_musicCache);
			f_musicCache.valid = false;
		}
		LeaveCriticalSection(&f_Lock);
	}

	void AudioSystem::ExecuteMusic( const tagCacheMusic& cache )
	{
		StopMusic();

		FMOD_MODE mode = FMOD_2D;
		if( cache.loop )
		{
			mode |= FMOD_LOOP_NORMAL;
		}

		//--load new
		FMOD_RESULT result;
		EnterCriticalSection(&f_Lock);
		result = m_p->pSystem->createStream((const char *)cache.strFileName.c_str(),mode,NULL,&m_p->pMusicSound);
		ERROR_CHECK(result);

		//--play
		result = m_p->pSystem->playSound(FMOD_CHANNEL_FREE,m_p->pMusicSound,true,&m_p->pMusicChannel);
		ERROR_CHECK(result);

		m_p->playMusicVolume = cache.volume;
		float volume = m_p->sysMusicVolume * m_p->playMusicVolume;
		f_musicPlayState = EMPS_Play;
		f_musicFadeTime = cache.fadeinTime;
		f_musicEscapeTime = 0;
		if( cache.fadeinTime > 0 )
		{
			volume = 0.0f;
			f_musicPlayState = EMPS_FadeIn;
		}

		result = m_p->pMusicChannel->setVolume(volume);
		ERROR_CHECK(result);

		result = m_p->pMusicChannel->setPriority(Member::EP_Music);
		ERROR_CHECK(result);

		result = m_p->pMusicChannel->setPaused(false);
		ERROR_CHECK(result);
		LeaveCriticalSection(&f_Lock);
	}

	void AudioSystem::StopMusic( DWORD fadeoutTime )
	{
		if( m_p->pSystem == NULL )
			return;

		if( m_p->pMusicSound )
		{
			EnterCriticalSection(&f_Lock);
			switch( f_musicPlayState )
			{
			case EMPS_FadeIn:
			case EMPS_Play:
				{
					if( fadeoutTime > 0 )
					{
						f_musicPlayState = EMPS_FadeOut;
						f_musicFadeTime = fadeoutTime;
						f_musicEscapeTime = 0;
					}
					else
					{
						m_p->pMusicSound->release();
						m_p->pMusicSound = NULL;
						m_p->pMusicChannel = NULL;
						f_musicPlayState = EMPS_None;
					}
				}
				break;
			default:
				{
					m_p->pMusicSound->release();
					m_p->pMusicSound = NULL;
					m_p->pMusicChannel = NULL;
					f_musicPlayState = EMPS_None;
				}
				break;
			}
			LeaveCriticalSection(&f_Lock);
		}
	}

	bool AudioSystem::MusicIsPlaying( void )
	{
		return f_musicPlayState != EMPS_None;
	}

	void AudioSystem::SetListener(const Vector3& pos,const Vector3& vel,const Vector3& forward,const Vector3& up)
	{
		if(m_p->pSystem == NULL)
			return;

		FMOD_VECTOR listener_pos = {pos.x,pos.y,pos.z};
		FMOD_VECTOR listener_forward = {forward.x,forward.y,forward.z};
		FMOD_VECTOR listener_up = {up.x,up.y,up.z};
		FMOD_VECTOR listener_vel = {vel.x,vel.y,vel.z};

		FMOD::System *pSys = m_p->pSystem;

		FMOD_RESULT result;
		EnterCriticalSection(&f_Lock);
		result = pSys->set3DListenerAttributes(0, &listener_pos, &listener_vel,&listener_forward,&listener_up);
		ERROR_CHECK(result);
		LeaveCriticalSection(&f_Lock);
	}

	void AudioSystem::ClearEnvironmentSound()
	{
		if(m_p->pSystem == NULL)
			return;

		EnterCriticalSection(&f_Lock);
		m_p->FreeAll3DEnvSound();
		LeaveCriticalSection(&f_Lock);
	}

	void AudioSystem::AddEnvironmentSound(const TCHAR* szFileName,Vector3 pos,float minDist,float maxDist,float volume,DWORD interval)
	{
		if(m_p->pSystem == NULL)
			return;

		//--
		FMOD::System	*pSys = m_p->pSystem;
		FMOD::Sound		*pSound = NULL;

		FMOD_RESULT result;

		EnterCriticalSection(&f_Lock);
		result = pSys->createSound((const char *)szFileName,FMOD_SOFTWARE|FMOD_3D,NULL,&pSound);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			_WriteLogString(_T("Env sound filename:%s\r\n"), szFileName);
			LeaveCriticalSection(&f_Lock);
			return;
		}

		if( minDist >= maxDist )//max必须大于min
			maxDist = minDist + 1.0f;
		result = pSound->set3DMinMaxDistance(minDist,maxDist);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return;
		}

		FMOD_MODE mode = FMOD_3D_LINEARROLLOFF;
		if( interval==0 )
		{
			mode |= FMOD_LOOP_NORMAL;
		}
		result = pSound->setMode(mode);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return;
		}

		FMOD_VECTOR fpos = {pos.x,pos.y,pos.z};
		LeaveCriticalSection(&f_Lock);

		//--
		tagEnvSoundInfo snd(interval,pSound,NULL,fpos,volume);
		m_p->m_sound3dEnvList.push_back(snd);
	}

	int AudioSystem::GetNum3DEnvSoundPlaying()
	{
		if(m_p->pSystem == NULL)
			return 0;

		int ret = 0;
		for(std::list<tagEnvSoundInfo>::iterator iter = m_p->m_sound3dEnvList.begin();
			iter != m_p->m_sound3dEnvList.end(); ++iter)
		{
			tagEnvSoundInfo& snd = *iter;
			FMOD::Sound		*pSound = (snd.pSound);
			FMOD::Channel	*pChannel = (snd.pChannel);

			FMOD::Sound *pTestSound = NULL;
			EnterCriticalSection(&f_Lock);
			pChannel->getCurrentSound(&pTestSound);
			if(pTestSound == pSound)
			{
				bool bVirtual = true;
				pChannel->isVirtual(&bVirtual);

				if(!bVirtual)
					ret++;
			}
			LeaveCriticalSection(&f_Lock);
		}

		return ret;
	}

	int  AudioSystem::Create3DEventSound(const TCHAR* szFileName,bool bLoop,float minDist,float maxDist,float volume)
	{
		if(m_p->pSystem == NULL)
			return SOUND_INVALID;

		FMOD::System *pSys = m_p->pSystem;
		FMOD::Sound		*pSound = NULL;
		FMOD::Channel	*pChannel = NULL;

		FMOD_RESULT result;

		EnterCriticalSection(&f_Lock);
		result = pSys->createSound((const char *)szFileName,FMOD_SOFTWARE|FMOD_3D,NULL,&pSound);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			_WriteLogString(_T("3D sound filename:%s\r\n"), szFileName);
			LeaveCriticalSection(&f_Lock);
			return SOUND_INVALID;
		}

		result = pSound->set3DMinMaxDistance(minDist,maxDist);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return SOUND_INVALID;
		}

		FMOD_MODE mode = FMOD_3D_LINEARROLLOFF;
		if(bLoop)
		{
			mode |= FMOD_LOOP_NORMAL;
		}
		result = pSound->setMode(mode);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return SOUND_INVALID;
		}

		result = pSys->playSound(FMOD_CHANNEL_FREE,pSound,true,&pChannel);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return SOUND_INVALID;
		}

		result = pChannel->setChannelGroup(m_p->pSoundGroup);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return SOUND_INVALID;
		}

		result = pChannel->setVolume(m_p->sysSoundVolume * volume);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return SOUND_INVALID;
		}

		result = pChannel->setPriority(Member::EP_3DEvent);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return SOUND_INVALID;
		}
		LeaveCriticalSection(&f_Lock);

		//--
		tagSoundInfo snd(pSound,pChannel,volume);
		int id = m_p->GenID();
		m_p->m_sound3DEventMap.insert(make_pair(id,snd));

		return id;
	}

	void AudioSystem::Destroy3DEventSound(int id)
	{
		if(m_p->pSystem == NULL)
			return;
		if(id == SOUND_INVALID)
			return;

		map<int,tagSoundInfo>::iterator iter = m_p->m_sound3DEventMap.find(id);
		if(iter != m_p->m_sound3DEventMap.end())
		{
			tagSoundInfo& snd = iter->second;

			EnterCriticalSection(&f_Lock);
			snd.pSound->release();
			LeaveCriticalSection(&f_Lock);

			m_p->m_sound3DEventMap.erase(iter);
		}
	}

	bool AudioSystem::SoundIsStoped( int id )
	{
		bool bRet = true;

		if(m_p->pSystem == NULL)
			return bRet;
		if(id == SOUND_INVALID)
			return bRet;

		map<int,tagSoundInfo>::iterator iter = m_p->m_sound3DEventMap.find(id);
		if(iter != m_p->m_sound3DEventMap.end())
		{
			tagSoundInfo& snd = iter->second;

			EnterCriticalSection(&f_Lock);
			bRet = m_p->IsSoundStoped( snd );
			LeaveCriticalSection(&f_Lock);
		}

		return bRet;
	}

	void AudioSystem::Play3DEventSound(int id)
	{
		if(m_p->pSystem == NULL)
			return;
		if(id == SOUND_INVALID)
			return;

		map<int,tagSoundInfo>::iterator iter = m_p->m_sound3DEventMap.find(id);
		if(iter != m_p->m_sound3DEventMap.end())
		{
			tagSoundInfo& snd = iter->second;

			EnterCriticalSection(&f_Lock);
			snd.pChannel->setPaused(false);
			snd.pChannel->setPosition(0,FMOD_TIMEUNIT_MS);
			LeaveCriticalSection(&f_Lock);
		}
	}

	void AudioSystem::Active3DEventSound(int id,float volume)
	{
		if(m_p->pSystem == NULL)
			return;
		if(id == SOUND_INVALID)
			return;

		map<int,tagSoundInfo>::iterator iter = m_p->m_sound3DEventMap.find(id);
		if(iter != m_p->m_sound3DEventMap.end())
		{
			tagSoundInfo& snd = iter->second;

			EnterCriticalSection(&f_Lock);
			m_p->pSystem->playSound(FMOD_CHANNEL_FREE,snd.pSound,true,&snd.pChannel);
			snd.volume=volume;
			snd.pChannel->setVolume(m_p->sysSoundVolume*volume);
			snd.pChannel->set3DAttributes(&snd.pos,NULL);
			snd.pChannel->setPosition(0,FMOD_TIMEUNIT_MS);
			snd.pChannel->setPaused(false);
			LeaveCriticalSection(&f_Lock);
		}
	}

	void AudioSystem::Stop3DEventSound(int id)
	{
		if(m_p->pSystem == NULL)
			return;
		if(id == SOUND_INVALID)
			return;

		map<int,tagSoundInfo>::iterator iter = m_p->m_sound3DEventMap.find(id);
		if(iter != m_p->m_sound3DEventMap.end())
		{
			tagSoundInfo& snd = iter->second;

			EnterCriticalSection(&f_Lock);
			snd.pChannel->setPaused(true);
			LeaveCriticalSection(&f_Lock);
		}
	}

	void AudioSystem::Set3DEventSoundAtt(int id,const Vector3& pos,const Vector3& vel)
	{
		if(m_p->pSystem == NULL)
			return;
		if(id == SOUND_INVALID)
			return;

		map<int,tagSoundInfo>::iterator iter = m_p->m_sound3DEventMap.find(id);
		if(iter != m_p->m_sound3DEventMap.end())
		{
			tagSoundInfo& snd = iter->second;

			snd.pos.x = pos.x;
			snd.pos.y = pos.y;
			snd.pos.z = pos.z;
			FMOD_VECTOR fvel = {vel.x,vel.y,vel.z};

			EnterCriticalSection(&f_Lock);
			snd.pChannel->set3DAttributes(&snd.pos,&fvel);
			LeaveCriticalSection(&f_Lock);
		}
	}

	void AudioSystem::StopAll3DEventSound()
	{
		if(m_p->pSystem == NULL)
			return;

		EnterCriticalSection(&f_Lock);
		m_p->FreeAll3DEventSound();
		LeaveCriticalSection(&f_Lock);
	}

	void AudioSystem::Play2DEventSound(const TCHAR* szFileName,float volume)
	{
		if(m_p->pSystem == NULL)
			return;

		//--garbage collection
		EnterCriticalSection(&f_Lock);
		m_p->FreeStoped2DSound();

		//--
		FMOD::System *pSys = m_p->pSystem;
		FMOD::Sound		*pSound = NULL;
		FMOD::Channel	*pChannel = NULL;

		FMOD_RESULT result;
		result = pSys->createSound((const char *)szFileName,FMOD_DEFAULT,NULL,&pSound);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			_WriteLogString(_T("2D sound filename:%s\r\n"), szFileName);
			LeaveCriticalSection(&f_Lock);
			return;
		}

		result = pSys->playSound(FMOD_CHANNEL_FREE,pSound,true,&pChannel);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return;
		}

		result = pChannel->setChannelGroup(m_p->pSoundGroup);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return;
		}

		pChannel->setVolume(m_p->sysSoundVolume*volume);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return;
		}

		result = pChannel->setPriority(Member::EP_2DEvent);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return;
		}

		result = pChannel->setPaused(false);
		if( result != FMOD_OK )
		{
			LogFMODError(result);
			LeaveCriticalSection(&f_Lock);
			return;
		}
		LeaveCriticalSection(&f_Lock);

		//--add to list
		tagSoundInfo snd(pSound,pChannel,volume);
		m_p->m_sound2dList.push_back(snd);
	}

	void AudioSystem::StopAll2DSound()
	{
		if(m_p->pSystem == NULL)
			return;

		for(std::list<tagSoundInfo>::iterator iter=m_p->m_sound2dList.begin();
			iter != m_p->m_sound2dList.end();++iter)
		{
			tagSoundInfo& snd = *iter;

			EnterCriticalSection(&f_Lock);
			snd.pSound->release();
			LeaveCriticalSection(&f_Lock);
		}

		m_p->m_sound2dList.clear();
	}

	int AudioSystem::GetNum2DSoundPlaying()
	{
		return (int)m_p->m_sound2dList.size();
	}

	FMOD_RESULT F_CALLBACK FileOpenCallback(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata)
	{
		if( !f_pVFS )
			return FMOD_ERR_FILE_NOTFOUND;

		EnterCriticalSection(&io_Lock);

		DWORD dwSize = f_pVFS->GetSize((LPCTSTR)name);
		if( -1 == dwSize )
		{
			LeaveCriticalSection(&io_Lock);
			return FMOD_ERR_FILE_NOTFOUND;
		}
		else
			*filesize = dwSize;

		DWORD dwHandle = f_pVFS->Open((LPCTSTR)name);
		if( !dwHandle )
		{
			LeaveCriticalSection(&io_Lock);
			return FMOD_ERR_FILE_NOTFOUND;
		}
		else
			*handle = (LPVOID)dwHandle;

		*userdata = 0;

		LeaveCriticalSection(&io_Lock);

		return FMOD_OK;
	}

	FMOD_RESULT F_CALLBACK FileCloseCallback(void *handle, void *userdata)
	{
		if( !handle )
			return FMOD_ERR_INVALID_PARAM;

		if( !f_pVFS )
			return FMOD_ERR_FILE_NOTFOUND;

		EnterCriticalSection(&io_Lock);

		BOOL ret = f_pVFS->Close((DWORD)handle);

		LeaveCriticalSection(&io_Lock);

		if( ret == TRUE )
			return FMOD_OK;
		else
			return FMOD_ERR_FILE_NOTFOUND;
	}

	FMOD_RESULT F_CALLBACK FileReadCallback(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
	{
		if( !handle )
			return FMOD_ERR_INVALID_PARAM;

		if( !f_pVFS )
			return FMOD_ERR_FILE_NOTFOUND;

		EnterCriticalSection(&io_Lock);

		DWORD dwRead = f_pVFS->Read(buffer, sizebytes, (DWORD)handle);
		*bytesread = dwRead;

		LeaveCriticalSection(&io_Lock);

		if( dwRead < sizebytes )
			return FMOD_ERR_FILE_EOF;
		else
			return FMOD_OK;
	}


	FMOD_RESULT F_CALLBACK FileSeekCallback(void *handle, unsigned int pos, void *userdata)
	{
		if( !handle )
			return FMOD_ERR_INVALID_PARAM;

		if( !f_pVFS )
			return FMOD_ERR_FILE_NOTFOUND;

		EnterCriticalSection(&io_Lock);

		f_pVFS->Seek((DWORD)handle, pos, 0);

		LeaveCriticalSection(&io_Lock);

		return FMOD_OK;
	}
}