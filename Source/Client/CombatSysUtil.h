#pragma once
#include "..\WorldDefine\move_define.h"
#include "..\WorldDefine\ItemDefine.h"
#include "AvatarAniPlayer.h"

class ChatFrame;
class SceneCenterInfoFrame;
class Role;
class NPC;
class AudioSys;
class Player;
/** \class CombatSysUtil
	\brief 战斗系统工具类
*/
class CombatSysUtil
{
public:
	/** \初始化，战斗系统初始化时调用
	*/
	void Init();
	/** \释放数据
	*/
	void Destroy();
	/** \计算键盘移动方向向量
	*/
	Vector3 CalcKeyMoveDir(EWalkDir dir,float cameraYaw);
	/** \在聊天系统战斗频道显示战斗信息
	*/
	void ShowCombatMsg(const TCHAR* szFormat,...);
	/** \在聊天频道的系统频道显示信息
	*/
	void ShowSystemMsg(const TCHAR* szFormat,...);
	/** \在屏幕中上显示信息
	*/
	void ShowScreenCenterMsg(const TCHAR* szFormat,...);
	/** \在屏幕中上显示信息(任务)
	*/
	void ShowScreenCenterMsgEx(const TCHAR* szFormat,...);
	/** \显示PK相关错误信息
	*/
	void ShowPKErrorMsg(DWORD dwErrorCode);
	/** \显示行为检查相关信息
	*/
	void ShowActionCheckMsg(DWORD dwErrorCode);
	/** \显示使用技能时服务器返回的错误信息
	*/
	void ShowSkillSvrErrMsg(DWORD dwErrorCode);
	/** \显示使用物品时服务器返回的错误信息
	*/
	void ShowItemSvrErrMsg(DWORD dwErrorCode);
	/** \显示播放个性动作时的错误信息
	*/
	void ShowPlayStyleActionErrMsg(DWORD dwErrorCode);
	/** \根据玩家武器类型，返回备战动作
	*/
	const TCHAR* GetReadyTrackName(EItemTypeEx weapon);
	/** \根据玩家武器类型等，返回走跑动作
	*/
	const TCHAR* GetMoveTrackName(EItemTypeEx weapon,bool bInCombat,float speed);
	/** \根据玩家武器类型，返回行走动作
	*/
	const TCHAR* GetWalkTrackName(EItemTypeEx weapon,bool bInCombat);
	/** \根据玩家武器类型，返回奔跑动作
	*/
	const TCHAR* GetRunTrackName(EItemTypeEx weapon,bool bInCombat);
	/** \根据玩家武器类型，返回冲刺动作
	*/
	const TCHAR* GetRushTrackName(EItemTypeEx weapon,bool bInCombat);
	/** \根据玩家武器类型，返回格档动作
	*/
	const TCHAR* GetBlockTrackName(EItemTypeEx weapon);
	/** \根据玩家武器类型，返回站立动作
	*/
	const TCHAR* GetStandTrackName(EItemTypeEx RWeapon);
	/** \根据玩家武器类型，返回进入备战动作
	*/
	const TCHAR* GetEnterCombatTrackName(EItemTypeEx RWeapon);
	/** \根据玩家武器类型，返回离开备战动作
	*/
	const TCHAR* GetLeaveCombatTrackName(EItemTypeEx RWeapon);
	/** \生成被攻击动作
	*/
	const TCHAR* GetBeattackTrackName();
	/** \根据NPC移动速度，返回走跑等动作
	*/
	const TCHAR* GetNPCMoveTrackName(float moveSpeed);
	/**	\获得坐骑动作
	*/
	const TCHAR* GetMountTrackName(AvatarAniPlayer::EAniType aniType,float moveSpeed);
	/** \生成技能和使用物品消息序列号
	*/
	static DWORD MakeSkillMsgSerial();
	/** \播放脚步音效
	*/
	void PlayFootSound(const Vector3& rolePos);
	void PlayNPCFootSound(NPC* pNPC);
	/** \播放摔倒音效
	*/
	void PlayFallSound(const Vector3& rolePos);
	/** \播放角色死亡音效
	*/
	void PlayRoleDeadSound(Role* pRole);
	/** \播放怪物进入战斗音效
	*/
	void PlayMonsterCombatSound(NPC* pMonster);
	/** \播放武器音效
	*/
	void PlayWeaponSound(Player* pPlayer);
	/** \播放玩家进攻喊叫音效
	*/
	void PlayPlayerRoarSound(Player* pPlayer);
	/** \播放玩家挨打喊叫音效
	*/
	void PlayPlayerHowlSound(Player* pPlayer);
	/** \播放怪物进攻喊叫音效
	*/
	void PlayMonsterRoarSound(NPC* pRole);
	/** \播放怪物挨打喊叫音效
	*/
	void PlayMonsterHowlSound(NPC* pRole);
	/** \本地玩家瞬间使用物品
	*/
	void SelfUseItemImmediate(INT64 n64ItemID,DWORD dwItemTypeID,DWORD dwSrcRoleID,DWORD dwTargetRoleID,const Vector3& srcRolePos);
	/** \将本地玩家从空中拖拽下来
	*/
	void PullLocalPlayer();
	/** \普通攻击是否可移动
	*/
	bool NormalAttackCanMove();

	CombatSysUtil(void);
	~CombatSysUtil(void);
	static CombatSysUtil* Inst();
private:
	TObjRef<GameFrameMgr>	m_pGameFrameMgr;
	TObjRef<AudioSys>		m_pAudioSys;
	ChatFrame*				m_pChatFrame;
	SceneCenterInfoFrame*	m_pSceneCenterInfoFrame;
};
