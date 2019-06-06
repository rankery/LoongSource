#pragma once
#include "AvatarAniMixer.h"

class Player;
/** \class AvatarAniPlayer
	\brief 玩家角色动画播放器
	\remarks 根据动作状态播放动作，如骑乘状态下跑动，不是播放跑，而是播放骑乘动作
*/
class AvatarAniPlayer
{
public:
	enum EAniType			//动作类型
	{
		EAT_Stand,			//站立
		EAT_Move,			//移动
		EAT_StartJump,		//起跳
		EAT_Jumpping,		//跳跃中
		EAT_Land,			//落地
		EAT_Drop,			//掉落
		EAT_VDrop,			//垂直掉落
		EAT_Slide,			//滑动
		EAT_Ready,			//备战
		EAT_Ease,			//悠闲
		EAT_Other,			//其它
	};

public:
	AvatarAniPlayer(void);
	~AvatarAniPlayer(void);
	void SetPlayer(Player* pPlayer);

	/** 播放一个动作
	*/
	void PlayTrack(EAniType aniType,AvatarAniMixer::EAniPart aniPart,const TCHAR* szTrackName,bool bLoop,bool bForceReplay=false,float curTime=0.0f,float fadeInTime=0.2f,AvatarAniMixer::EPriority pri=AvatarAniMixer::EPri_Normal);
	/** 上身动作同步到下身动作
	*/
	void Sync2Lower();
	/** 动作是否播放完毕？
	*/
	bool IsTrackEnd(EAniType aniType,const TCHAR* szTrackName);
	/** 每帧调用
	*/
	void Update();

private:
	AvatarAniMixer	m_aniMixer;
};
