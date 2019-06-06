

typedef std::pair<FMOD::Sound*,FMOD::Channel*> SoundInfo;

class SoundManager::Member
{
public:
	Member(void)
	{
		pSystem = NULL;
		pMusicChannel = NULL;
		pSoundGroup = NULL;

		pMusicSound = NULL;
		pMusicChannel = NULL;

		musicVolume = 1.0f;
		soundVolume = 1.0f;

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
	std::list<SoundInfo>	sound2dList;

	//--3d environment sound
	std::list<SoundInfo>	sound3dEnvList;

	//--3d event sound
	int	m_nextID;
	std::map<int,SoundInfo>	sound3DEventMap;

	//--
	float musicVolume;
	float soundVolume;

public:
	int GenID()
	{
		m_nextID++;
		return m_nextID;
	}

	void FreeAll3DEventSound()
	{

		for(std::map<int,SoundInfo>::iterator iter = sound3DEventMap.begin();
			iter != sound3DEventMap.end(); ++iter)
		{
			const SoundInfo& snd = (iter->second);

			FMOD::Sound *pSound = (snd.first);
			pSound->release();
		}

		sound3DEventMap.clear();
	}
	

	void FreeAll3DEnvSound()
	{
		for(std::list<SoundInfo>::iterator iter = sound3dEnvList.begin();
			iter != sound3dEnvList.end(); ++iter)
		{
			FMOD::Sound *pSound = (iter->first);
			pSound->release();
		}

		sound3dEnvList.clear();
	}

	bool IsSoundStoped(const SoundInfo& snd)
	{
		FMOD::Sound *pTestSound = NULL;
		snd.second->getCurrentSound(&pTestSound);

		if(pTestSound != snd.first)
			return true;

		bool bPlaying = false;
		snd.second->isPlaying(&bPlaying);

		return !bPlaying;
	}

	void FreeStoped2DSound()
	{
		for(std::list<SoundInfo>::iterator iter=sound2dList.begin();
			iter != sound2dList.end();)
		{
			const SoundInfo& snd = (*iter);
		
			if(IsSoundStoped(snd))
			{
				snd.first->release();
				iter = sound2dList.erase(iter);
			}
			else
				++iter;
		}
	}
};

void LogFMODError(FMOD_RESULT result)
{
	_WriteLogString(_T("FMOD error! (%d) %s\r\n"), result, FMOD_ErrorString(result));
}
