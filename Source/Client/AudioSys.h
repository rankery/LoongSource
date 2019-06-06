/********************************************************************
	created:	2008/12/03
	created:	3:12:2008   11:09
	filename: 	d:\loong_client\Client\AudioSys.h
	file path:	d:\loong_client\Client
	file base:	AudioSys
	file ext:	h
	author:		leili
	
	purpose:	声音系统声明，用于在游戏中播放背景音乐及各种声音特效
*********************************************************************/
#pragma once

#include "MapEvent.h"
#include "CombatEvent.h"

class LocalPlayer;
class ClientCamera;


#define SOUND_LOUD		(1.0f)	// 音量大
#define SOUND_NORMAL	(0.7f)	// 音量中
#define SOUND_WISPER	(0.4f)	// 音量小

class AudioSys
{
public:
	AudioSys(void);
	~AudioSys(void);

	BOOL				Init(void);
	VOID				Destroy(void);

	VOID				Update(DWORD delta);
	VOID				SetCamera(ClientCamera& camera);

	/**	播放2D音效
	*/
	VOID				Play2DSound(LPCTSTR szSound, float volume=1.0f);

	/**	停止播放所有2D音效
	*/
	VOID				StopAll2DSound(void);

	// 播放音乐
	VOID				PlayMusic(LPCTSTR szMusic, BOOL bLoop, FLOAT fVolume);
	// 停止音乐
	VOID				StopMusic();
	BOOL				MusicIsPlaying(void);

	/**	创建3D音效
	@remarks
		音效的生命周期由用户管理
		音效在minDist和maxDist之间进行线性变化
		小于minDist的一直是最大音量
		大于maxDist的一直是最小音量
	*/
	INT					Create3DSound(LPCTSTR szSound,bool bLoop,float minDist,float maxDist,float volume=1.0f);

	/**	销毁3D音效
	*/
	VOID				Destroy3DSound(INT id);

	/**	播放3D音效
	*/
	VOID				Play3DSound(INT id);

	/**	停止播放3D音效
	*/
	VOID				Stop3DSound(INT id);

	/**	设置3D音效参数
	@param pos 音效的位置
	@param vel 音效移动的速度
	*/
	VOID				Set3DSoundAtt(INT id,const Vector3& pos,const Vector3& vel);

	/**	停止播放所有3D音效
	*/
	VOID				StopAll3DSound(void);

	/** 创建3D音效并播放
	@remarks
		音效的生命周期由AudioSys自己管理
	*/
	VOID				Play3DSound(LPCTSTR szSound,float minDist,float maxDist,const Vector3& pos,float volume=1.0f);

	VOID				EnableCombatMusic(BOOL bEnable) { m_enableCombatMusic = bEnable; }

private:
	DWORD				EnterMapArea(tagEnterMapAreaEvent* pEvent);	// 进入某区域
	DWORD				LeaveMapArea(tagLeaveMapAreaEvent* pEvent);	// 离开某区域
	DWORD				EnterCombat(tagEnterCombatEvent* pEvent);	// 进入战斗
	DWORD				LeaveCombat(tagLeaveCombatEvent* pEvent);	// 离开战斗

	DWORD				OnOpenMap(tagGameEvent* pEvent);	// 加载3D环境音效
	DWORD				OnCloseMap(tagGameEvent* pEvent);	// 结束播放背景音效
	DWORD				OnCloseMapMusic(tagGameEvent* pEvent);	// 结束播放背景音乐

	VOID				GarbageCollection(void);
	VOID				Release3DSound(void);

private:
	TSFPTrunk<AudioSys>		m_Trunk;
	TObjRef<VarContainer>	m_pVarContainer;
	Util					*m_pUtil;

	static const int		s_fadeinTime	= 0;	// 淡入时间
	static const int		s_fadeoutTime	= 2000;	// 淡出时间
	static const int		s_musicCount	= 3;

	struct tagSound 
	{
		DWORD	name;	// 音效CRC
		INT		id;		// 音效ID
	};
	typedef std::list<tagSound> SoundList;
	SoundList			m_playSounds;		// 记录当前正在播放的音效
	SoundList			m_deadSounds;		// 记录已经播放完毕的音效

	const tagMapAreaEx*	m_playArea;
	int					m_playMusicIndex;
	DWORD				m_musicIntervalEscaped;

	enum EEnvType
	{
		ENV_Normal	= 0,
		ENV_Combat
	};
	EEnvType			m_envType;			//当前环境类型

	typedef std::vector<tstring> CombatEnvMusics;
	CombatEnvMusics		m_combatEnvMusic;

	bool				m_bFirstCombat;
	BOOL				m_enableCombatMusic;
};