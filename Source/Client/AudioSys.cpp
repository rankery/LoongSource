/********************************************************************
	created:	2008/12/03
	created:	3:12:2008   11:11
	filename: 	d:\loong_client\Client\AudioSys.cpp
	file path:	d:\loong_client\Client
	file base:	AudioSys
	file ext:	cpp
	author:		leili
	
	purpose:	声音系统实现
*********************************************************************/
#include "stdafx.h"
#include "AudioSys.h"
#include "MapMgr.h"
#include "GameSet.h"
#include "ClientCamera.h"

AudioSys::AudioSys( void )
: m_Trunk(this)
, m_playArea(NULL)
, m_playMusicIndex(0)
, m_musicIntervalEscaped(0)
, m_envType(ENV_Normal)
, m_bFirstCombat(false)
{
}

AudioSys::~AudioSys( void )
{
}


BOOL AudioSys::Init( void )
{
	m_pUtil = TObjRef<Util>();

	m_pVarContainer = CreateObj("SoundTable", "VarContainer");
	if( !m_pVarContainer )
		return FALSE;

	if( FALSE == m_pVarContainer->Load("VFS_System", _T("data\\sound\\sound_table.xml")) )
		return FALSE;

	// 载入战斗环境音乐
	TCHAR szMusic[20];
	LPCTSTR szValue = NULL;

	for( INT i=0; i<3; i++ )
	{
		_stprintf( szMusic, _T("envmusic_combat%d"), i );

		szValue = m_pVarContainer->GetString(szMusic);
		if( P_VALID(szValue) )
		{
			m_combatEnvMusic.push_back(szValue);
		}
		else
			break;
	}

	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("LoadMapIsOK"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::OnOpenMap));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("CloseMap"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::OnCloseMap));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("CloseMapMusic"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::OnCloseMapMusic));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("tagEnterMapAreaEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::EnterMapArea));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("tagLeaveMapAreaEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::LeaveMapArea));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("tagEnterCombatEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::EnterCombat));
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("tagLeaveCombatEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::LeaveCombat));

	return TRUE;
}

VOID AudioSys::Destroy( void )
{
	Cool3D::AudioSystem::Inst()->StopMusic(s_fadeoutTime);
	Cool3D::AudioSystem::Inst()->StopAll2DSound();
	Cool3D::AudioSystem::Inst()->StopAll3DEventSound();

	Release3DSound();

	KillObj("SoundTable");

	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("LoadMapIsOK"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::OnOpenMap));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("CloseMap"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::OnCloseMap));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("CloseMapMusic"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::OnCloseMapMusic));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("tagEnterMapAreaEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::EnterMapArea));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("tagLeaveMapAreaEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::LeaveMapArea));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("tagEnterCombatEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::EnterCombat));
	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("tagLeaveCombatEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&AudioSys::LeaveCombat));
}

VOID AudioSys::Update( DWORD delta )
{
	Cool3D::AudioSystem::Inst()->Update(delta);

	// 判断音效是否播放完毕，播放完毕的放入deadSounds
	SoundList::iterator iter;
	for( iter=m_playSounds.begin(); iter!=m_playSounds.end(); )
	{
		tagSound& snd = *iter;
		if( Cool3D::AudioSystem::Inst()->SoundIsStoped(snd.id) )//已经播放完毕
		{
			m_deadSounds.push_back(snd);
			iter = m_playSounds.erase(iter);
		}
		else
			 ++iter;
	}

	if( m_envType == ENV_Normal )
	{
		if( P_VALID(m_playArea) && !Cool3D::AudioSystem::Inst()->MusicIsPlaying() )
		{
			m_musicIntervalEscaped += delta;
			if( m_musicIntervalEscaped > m_playArea->wInterval )
			{
				m_playMusicIndex++;
				m_playMusicIndex %= s_musicCount;

				if( m_playArea->strMusic[m_playMusicIndex].empty() )
				{
					m_playMusicIndex = 0;
				}

				Cool3D::AudioSystem::Inst()->PlayMusic( m_playArea->strMusic[m_playMusicIndex].c_str(), false, (float)m_playArea->byVol/100.0f, s_fadeinTime );
				m_musicIntervalEscaped = 0;
			}
		}
	}
	else//Combat
	{
		if( !m_combatEnvMusic.empty() && !Cool3D::AudioSystem::Inst()->MusicIsPlaying() )
		{
			INT max = (INT)m_combatEnvMusic.size()-1;
			DWORD fadeinTime = 0;
			if( m_bFirstCombat )//第一次进入战斗时渐起播放音乐
			{
				fadeinTime = s_fadeinTime;
				m_bFirstCombat = false;
			}
			Cool3D::AudioSystem::Inst()->PlayMusic( m_combatEnvMusic[m_pUtil->RandomInRange(0,max)].c_str(), false, SOUND_NORMAL, fadeinTime );
		}
	}
}

// 播放音乐
VOID AudioSys::PlayMusic(LPCTSTR szMusic, BOOL bLoop, FLOAT fVolume)
{
	LPCTSTR szValue = m_pVarContainer->GetString(szMusic);
	if( P_VALID(szValue) )
	{
		Cool3D::AudioSystem::Inst()->PlayMusic(szValue, bLoop==TRUE, fVolume, s_fadeinTime);
	}
}

// 停止音乐
VOID AudioSys::StopMusic()
{
	Cool3D::AudioSystem::Inst()->StopMusic(s_fadeoutTime);
}

BOOL AudioSys::MusicIsPlaying( void )
{
	return ( Cool3D::AudioSystem::Inst()->MusicIsPlaying() ? TRUE : FALSE );
}

VOID AudioSys::SetCamera( ClientCamera& camera )
{
	Vector3 pos = camera.GetLookat();

	// 判断pos是否有效，有时pos变成无穷大时会导致蓝屏
	if( pos.x > 10000000.0f || pos.x < -10000000.0f
		|| pos.y > 10000000.0f || pos.y < -10000000.0f
		|| pos.z > 10000000.0f || pos.z < -10000000.0f )
	{
		return;
	}

	Vector3 vel = MathConst::Zero3;
	Cool3D::AudioSystem::Inst()->SetListener( pos, vel, camera.GetForward(), camera.GetUp() );
}

DWORD AudioSys::OnOpenMap( tagGameEvent* pEvent )
{
	if( MapMgr::Inst()->GetCurMapID() != GT_INVALID )
	{
		GameMap& curMap = MapMgr::Inst()->GetGameMap();
		const vector<tagMapSound>& mapSounds = curMap.GetSounds();

		vector<tagMapSound>::const_iterator iter = mapSounds.begin();

		for( ; iter != mapSounds.end(); ++iter )
		{
			const tagMapSound& sound = *iter;

			Vector3 pos;
			pos.x = sound.fPos[0];
			pos.y = sound.fPos[1];
			pos.z = sound.fPos[2];
			Cool3D::AudioSystem::Inst()->AddEnvironmentSound( sound.szFileName, pos, sound.fNoAtteRange, sound.fRange, sound.fVolume, sound.dwInterval );
		}
	}

	return 0;
}

DWORD AudioSys::OnCloseMap( tagGameEvent* pEvent )
{
	Cool3D::AudioSystem::Inst()->ClearEnvironmentSound();

	return 0;
}

DWORD AudioSys::OnCloseMapMusic( tagGameEvent* pEvent )
{
	Cool3D::AudioSystem::Inst()->StopMusic(s_fadeoutTime);
	m_playArea = NULL;
	m_playMusicIndex = 0;
	m_musicIntervalEscaped = 0;

	return 0;
}

DWORD AudioSys::EnterMapArea( tagEnterMapAreaEvent* pEvent )
{
	if( m_envType == ENV_Normal )
	{
		if( P_VALID(m_playArea) )	// 淡出旧区域的背景音乐
		{
			Cool3D::AudioSystem::Inst()->StopMusic(s_fadeoutTime);
		}
	}

	GameMap& curMap = MapMgr::Inst()->GetGameMap();
	m_playArea = curMap.FindMapAreaEx( pEvent->mapAreaID );

	if( P_VALID(m_playArea) )
	{
		if( m_playArea->strMusic[0].empty() )
		{
			m_playArea = NULL;
			return 0;
		}

		if( m_envType == ENV_Normal )
		{
			Cool3D::AudioSystem::Inst()->PlayMusic( m_playArea->strMusic[0].c_str(), false, (float)m_playArea->byVol/100.0f, s_fadeinTime );
		}
		m_playMusicIndex=0;
		m_musicIntervalEscaped=0;
	}

	return 0;
}

DWORD AudioSys::LeaveMapArea( tagLeaveMapAreaEvent* pEvent )
{
	if( m_envType == ENV_Normal )
	{
		if( P_VALID(m_playArea) )
		{
			Cool3D::AudioSystem::Inst()->StopMusic(s_fadeoutTime);	// 淡出离开区域的背景音乐
			m_playArea = NULL;
		}
	}
	else//Combat
	{
		m_playArea = NULL;
	}

	return 0;
}

DWORD AudioSys::EnterCombat( tagEnterCombatEvent* pEvent )
{
	if( m_enableCombatMusic && (m_envType != ENV_Combat) )
	{
		m_envType = ENV_Combat;
		m_bFirstCombat = true;
		StopMusic();
	}

	return 0;
}

DWORD AudioSys::LeaveCombat( tagLeaveCombatEvent* pEvent )
{
	if( m_envType != ENV_Normal )
	{
		m_envType = ENV_Normal;
		StopMusic();

		//立即播放下一首背景音乐
		if( P_VALID( m_playArea ) )
			m_musicIntervalEscaped = m_playArea->wInterval + 1;
	}

	return 0;
}

VOID AudioSys::Play2DSound( LPCTSTR szSound, float volume/*=1.0f*/ )
{
	LPCTSTR szValue = m_pVarContainer->GetString(szSound);
	if( P_VALID(szValue) )
	{
		Cool3D::AudioSystem::Inst()->Play2DEventSound( szValue, volume );
	}
}

VOID AudioSys::StopAll2DSound( void )
{
	Cool3D::AudioSystem::Inst()->StopAll2DSound();
}

INT AudioSys::Create3DSound( LPCTSTR szSound, bool bLoop,float minDist,float maxDist,float volume/*=1.0f*/ )
{
	LPCTSTR szValue = m_pVarContainer->GetString(szSound);
	if( P_VALID(szValue) )
		return Cool3D::AudioSystem::Inst()->Create3DEventSound( szValue, bLoop, minDist, maxDist, volume );

	return GT_INVALID;
}

VOID AudioSys::Destroy3DSound( INT id )
{
	Cool3D::AudioSystem::Inst()->Destroy3DEventSound(id);
}

VOID AudioSys::Play3DSound( INT id )
{
	Cool3D::AudioSystem::Inst()->Play3DEventSound(id);
}

VOID AudioSys::Stop3DSound( INT id )
{
	Cool3D::AudioSystem::Inst()->Stop3DEventSound(id);
}

VOID AudioSys::Set3DSoundAtt( INT id,const Vector3& pos,const Vector3& vel )
{
	Cool3D::AudioSystem::Inst()->Set3DEventSoundAtt(id,pos,vel);
}

VOID AudioSys::StopAll3DSound( void )
{
	Cool3D::AudioSystem::Inst()->StopAll3DEventSound();
}

VOID AudioSys::Play3DSound( LPCTSTR szSound,float minDist,float maxDist,const Vector3& pos,float volume/*=1.0f*/ )
{
	DWORD name = m_pUtil->Crc32(szSound);
	INT id = -1;

	//从deadSounds搜索是否有相同名称的音效
	bool bExist = false;
	SoundList::iterator iter;
	for( iter=m_deadSounds.begin(); iter!=m_deadSounds.end(); ++iter )
	{
		tagSound& snd = *iter;
		if( name == snd.name )
		{
			id = snd.id;
			m_playSounds.push_back(snd);
			m_deadSounds.erase(iter);
			bExist = true;
			break;
		}
	}

	if( !bExist )
	{
		GarbageCollection();
		id = Create3DSound(szSound, false, minDist, maxDist, volume);
		tagSound snd;
		snd.name = name;
		snd.id = id;
		m_playSounds.push_back(snd);
	}

	Vector3 vel=Vector3(0.0f,0.0f,0.0f);
	Set3DSoundAtt(id,pos,vel);

	if( bExist )
		Cool3D::AudioSystem::Inst()->Active3DEventSound(id,volume);
	else
		Play3DSound(id);
}

VOID AudioSys::GarbageCollection( void )
{
	// 如果音效死亡列表>20，释放前10个音效
	if( m_deadSounds.size() > 20 )
	{
		while( m_deadSounds.size()>10 )
		{
			SoundList::iterator iter = m_deadSounds.begin();
			tagSound& snd = *iter;

			Destroy3DSound(snd.id);
			m_deadSounds.erase(iter);
		}
	}
}

VOID AudioSys::Release3DSound( void )
{
	SoundList::iterator iter;

	for( iter=m_playSounds.begin(); iter!=m_playSounds.end(); ++iter )
	{
		tagSound& snd = *iter;
		Destroy3DSound(snd.id);
	}
	m_playSounds.clear();

	for( iter=m_deadSounds.begin(); iter!=m_deadSounds.end(); ++iter )
	{
		tagSound& snd = *iter;
		Destroy3DSound(snd.id);
	}
	m_deadSounds.clear();
}