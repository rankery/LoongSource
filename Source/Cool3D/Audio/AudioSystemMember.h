#include "..\Util\DebugMisc.h"

namespace Cool3D
{
	struct tagSoundInfo
	{
		FMOD::Sound		*pSound;
		FMOD::Channel	*pChannel;
		FMOD_VECTOR		pos;
		float			volume;

		tagSoundInfo(FMOD::Sound *sound,FMOD::Channel *channel,FMOD_VECTOR soundPos,float vol=1.0f)
			: pSound(sound)
			, pChannel(channel)
			, volume(vol)
			, pos(soundPos)
		{}
		tagSoundInfo(FMOD::Sound *sound,FMOD::Channel *channel,float vol=1.0f)
			: pSound(sound)
			, pChannel(channel)
			, volume(vol)
		{
			pos.x = 0.0f;
			pos.y = 0.0f;
			pos.z = 0.0f;
		}
	};

	struct tagEnvSoundInfo : public tagSoundInfo
	{
		DWORD	interval;		// 间隔时间：单位毫秒
		DWORD	current;		// 播放时间：单位毫秒

		tagEnvSoundInfo(DWORD intervalTime,FMOD::Sound *sound,FMOD::Channel *channel,FMOD_VECTOR soundPos,float vol=1.0f)
			: interval(intervalTime)
			, current(0)
			, tagSoundInfo(sound,channel,soundPos,vol)
		{}
	};

	class AudioSystem::Member
	{
	public:
		Member(void)
		{
			pSystem = NULL;
			pMusicChannel = NULL;
			pSoundGroup = NULL;

			pMusicSound = NULL;
			pMusicChannel = NULL;

			sysMusicVolume = 1.0f;
			sysSoundVolume = 1.0f;
			playMusicVolume = 1.0f;

			m_nextID = 1;
		}
		~Member(void){}

		enum EPriority
		{
			EP_Music = 0,
			EP_3DEvent = 32,
			EP_2DEvent = 64,
			EP_3DEnv = 128,
		};

	public:
		FMOD::System		*pSystem;

		FMOD::ChannelGroup	*pSoundGroup;//用来方便设置音量

		//--music
		FMOD::Sound		*pMusicSound;
		FMOD::Channel	*pMusicChannel;

		//--2d sound
		typedef std::list<tagSoundInfo>	Sound2DList;
		Sound2DList		m_sound2dList;

		//--3d environment sound
		typedef std::list<tagEnvSoundInfo> Sound3DEnvList;
		Sound3DEnvList	m_sound3dEnvList;

		//--3d event sound
		int	m_nextID;
		typedef std::map<int,tagSoundInfo> Sound3DEventMap;
		Sound3DEventMap	m_sound3DEventMap;

		//--
		float sysMusicVolume;	// 系统音乐音量
		float sysSoundVolume;	// 系统音效音量
		float playMusicVolume;	// 当前播放音乐音量

	public:
		int GenID()
		{
			m_nextID++;
			return m_nextID;
		}

		void FreeAll3DEventSound()
		{
			Sound3DEventMap::iterator iter;
			for( iter=m_sound3DEventMap.begin(); iter!=m_sound3DEventMap.end(); ++iter )
			{
				tagSoundInfo& snd = iter->second;

				FMOD::Sound *pSound = snd.pSound;
				pSound->release();
			}

			m_sound3DEventMap.clear();
		}


		void FreeAll3DEnvSound()
		{
			Sound3DEnvList::iterator iter;
			for( iter=m_sound3dEnvList.begin(); iter!=m_sound3dEnvList.end(); ++iter )
			{
				tagSoundInfo& snd = *iter;
				FMOD::Sound *pSound = snd.pSound;
				pSound->release();
			}

			m_sound3dEnvList.clear();
		}

		bool IsSoundStoped(const tagSoundInfo& snd)
		{
			FMOD::Sound *pTestSound = NULL;
			snd.pChannel->getCurrentSound(&pTestSound);

			if(pTestSound != snd.pSound)
				return true;

			bool bPlaying = false;
			snd.pChannel->isPlaying(&bPlaying);

			bool bPaused = false;
			snd.pChannel->getPaused(&bPaused);

			return !bPlaying || bPaused;
		}

		void FreeStoped2DSound()
		{
			Sound2DList::iterator iter;
			for( iter=m_sound2dList.begin(); iter!=m_sound2dList.end(); )
			{
				tagSoundInfo& snd = *iter;

				if(IsSoundStoped(snd))
				{
					snd.pSound->release();
					iter = m_sound2dList.erase(iter);
				}
				else
					++iter;
			}
		}

		void SetMusicVolume(void)
		{
			if( pMusicChannel != NULL )
				pMusicChannel->setVolume( sysMusicVolume * playMusicVolume );
		}

		void SetSoundVolume(void)
		{
			Sound2DList::iterator iter2D;
			for( iter2D=m_sound2dList.begin(); iter2D!=m_sound2dList.end(); ++iter2D )
			{
				tagSoundInfo& snd = *iter2D;
				snd.pChannel->setVolume( sysSoundVolume * snd.volume );
			}

			Sound3DEnvList::iterator iter3DEnv;
			for( iter3DEnv=m_sound3dEnvList.begin(); iter3DEnv!=m_sound3dEnvList.end(); ++iter3DEnv )
			{
				tagEnvSoundInfo& snd = *iter3DEnv;
				snd.pChannel->setVolume( sysSoundVolume * snd.volume );
			}

			Sound3DEventMap::iterator iter3DEvent;
			for( iter3DEvent=m_sound3DEventMap.begin(); iter3DEvent!=m_sound3DEventMap.end(); ++iter3DEvent )
			{
				tagSoundInfo& snd = iter3DEvent->second;
				snd.pChannel->setVolume( sysSoundVolume * snd.volume );
			}
		}
	};

	void LogFMODError(FMOD_RESULT result)
	{
		_WriteLogStringA( ("FMOD error! (%d) %s\r\n"), result, FMOD_ErrorString(result) );
	}
}
