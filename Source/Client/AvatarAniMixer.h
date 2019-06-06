#pragma once

class Player;
/** \class AvatarAniMixer
	\brief 玩家角色动画混合器
*/
class AvatarAniMixer
{
public:
	enum EAniPart			//动作部位
	{
		EAP_Upper,			//上身动作
		EAP_Lower,			//下身动作
		EAP_All,			//全身动作
	};

	enum EPriority			//动作优先级
	{
		EPri_Lower,			//低
		EPri_Normal,		//普通
		EPri_High,			//高
	};

private:
	enum EPlayState			//动作播放状态
	{
		EPS_None,			//未播放任何动作
		EPS_All,			//全身动作
		EPS_Upper,			//上身动作
		EPS_Lower,			//下身动作
		EPS_Blend,			//上下身混合
		EPS_UpperAndStand,	//上身动作和站立动作，这时只会播放上身动作
	};

	struct tagTrackInfo		//缓存的动画信息
	{
		EAniPart	aniPart;
		bool		bIsStand;
		tstring		trackName;
		bool		bLoop;
		float		curTime;
		float		fadeInTime;
		EPriority	pri;
	};

public:
	AvatarAniMixer(void);
	~AvatarAniMixer(void);
	void SetPlayer(Player* pPlayer) {m_pPlayer=pPlayer;}
	Player* GetPlayer(void) const	{return m_pPlayer;}

	/** 播放一个动作
	*/
	void PlayTrack(EAniPart aniPart,bool bIsStand,const TCHAR* szTrackName,bool bLoop,bool bForceReplay=false,float curTime=0.0f,float fadeInTime=0.2f,EPriority pri=EPri_Normal);
	/** 上身动作同步到下身动作
	*/
	void Sync2Lower();
	/** 动作是否播放完毕？
		\return 动作正在缓存或播放时返回false，其它返回true
	*/
	bool IsTrackEnd(const TCHAR* szTrackName);
	/** 每帧调用
	*/
	void Update();

private:
	Player*					m_pPlayer;
	EPlayState				m_state;

	tstring					m_szAllTrack;
	tstring					m_szUpperTrack;
	tstring					m_szLowerTrack;

	EPriority				m_priAll;
	EPriority				m_priUpper;
	EPriority				m_priLower;

	bool					m_bLowerIsStand;

	pair<bool,tagTrackInfo> m_cacheTrack;
};
