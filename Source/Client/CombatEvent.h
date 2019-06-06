#pragma once
#include "..\WorldDefine\move_define.h"
#include "..\WorldDefine\msg_combat.h"

/** 鼠标移动/游泳事件
*/
struct tagMouseMoveEvent : public tagGameEvent
{
	Vector3					start;			//起点
	Vector3					end;			//终点
	float					validDist;		//有效距离
	bool					bUserCtrl;		//是否用户操作
	bool					bNeedSendStopCmd;	//是否需要发送停止消息
	
	tagMouseMoveEvent():tagGameEvent(_T("tagMouseMoveEvent"),NULL),bUserCtrl(true),validDist(0.0f),bNeedSendStopCmd(false)
	{}
	tagMouseMoveEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) 
		: tagGameEvent(szEventName,pSenderFrame),bUserCtrl(true),validDist(0.0f),bNeedSendStopCmd(false)
	{}
};

/** 鼠标移动/游泳结束事件
*/
struct tagMouseMoveFinishEvent : public tagGameEvent
{
	bool					bSuccessed;			//是否成功

	tagMouseMoveFinishEvent():tagGameEvent(_T("tagMouseMoveFinishEvent"),NULL),bSuccessed(true)
	{}
	tagMouseMoveFinishEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame),bSuccessed(true)
	{}
};

/** 键盘移动/游泳事件
*/
struct tagKeyboardMoveEvent : public tagGameEvent
{
	Vector3					start;			//起点
	EWalkDir				dir;			//方向

	tagKeyboardMoveEvent():tagGameEvent(_T("tagKeyboardMoveEvent"),NULL)
	{}
	tagKeyboardMoveEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 停止键盘移动/游泳事件
*/
struct tagStopKeyboardMoveEvent : public tagGameEvent
{
	tagStopKeyboardMoveEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 跳跃事件
*/
struct tagJumpEvent : public tagGameEvent
{
	Vector3					start;			//起点
	Vector3					dir;			//方向

	tagJumpEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{
		dir=MathConst::Zero3;
	}
};

/** 掉落事件
*/
struct tagDropEvent : public tagGameEvent
{
	Vector3					start;			//起点
	Vector3					dir;			//方向

	tagDropEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 垂直掉落事件
*/
struct tagVDropEvent : public tagGameEvent
{
	Vector3					start;			//起点

	tagVDropEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 滑落事件
*/
struct tagSlideEvent : public tagGameEvent
{
	Vector3					start;			//起点

	tagSlideEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};


/** 使用技能游戏事件
*/
struct tagUseSkillEvent : public tagGameEvent
{
	DWORD					dwTargetRoleID;
	DWORD					dwSkillID;

	tagUseSkillEvent():tagGameEvent(_T("tagUseSkillEvent"),NULL)
	{}
	tagUseSkillEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 与NPC对话游戏事件
*/
struct tagTalkNPCEvent : public tagGameEvent
{
	DWORD					dwNPCID;
	DWORD					dwTypeID;

	tagTalkNPCEvent():tagGameEvent(_T("tagTalkNPCEvent"),NULL)
	{}
	tagTalkNPCEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 显示角色头顶HP变化信息游戏事件
*/
struct tagShowHPChangeEvent : public tagGameEvent
{
	DWORD					dwRoleID;		//角色ID
	ERoleHPChangeCause		eCause;			//变化原因
	int						nHPChange;		//HP变化值
	DWORD					dwSrcRoleID;	//发起者角色ID
	bool					bMiss;			//是否Miss
	bool					bCrit;			//是否暴击
	bool					bBlocked;		//是否被格挡

	tagShowHPChangeEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 命中目标游戏事件
*/
struct tagHitTargetEvent : public tagGameEvent
{
	DWORD					dwSrcRoleID;	//攻击者ID
	DWORD					dwTarRoleID;	//目标ID
	bool					bSingle;		//是否单体攻击		
	bool					bLast;			//是否是本次攻击的最后一个事件
	EHitTargetCause			eCause;			//命中目标原因
	DWORD					dwMisc;			//如果是技能，为技能ID;如果是物品，物品ID
	DWORD					dwMisc2;		//管道序号
	DWORD					dwSerial;		//技能或使用物品序列号

	tagHitTargetEvent():tagGameEvent(_T("tagHitTargetEvent"),NULL)
	{}
	tagHitTargetEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 被攻击游戏事件
*/
struct tagBeAttackEvent : public tagGameEvent
{
	bool					bMiss;			//是否MISS
	bool					bBlock;			//是否格挡
	tagBeAttackEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{
		bMiss=false;
		bBlock=false;
	}
};

/** 角色进入空闲状态通知游戏事件
*/
struct tagEnterStandStateEvent : public tagGameEvent
{
	tagEnterStandStateEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 角色进入战斗状态游戏事件
*/
struct tagEnterCombatEvent : public tagGameEvent
{
	tagEnterCombatEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 角色离开战斗状态游戏事件
*/
struct tagLeaveCombatEvent : public tagGameEvent
{
	tagLeaveCombatEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 跟随角色游戏事件
*/
struct tagFollowRoleEvent : public tagGameEvent
{
	DWORD					dwTargetRoleID;	//被跟随的角色ID

	tagFollowRoleEvent():tagGameEvent(_T("tagFollowRoleEvent"),NULL)
	{}
	tagFollowRoleEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 开启PK保护游戏事件
*/
struct tagOpenSafeguardEvent : public tagGameEvent
{
	bool					bOpen;			
	tagOpenSafeguardEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 开启行凶状态游戏事件
*/
struct tagOpenPKEvent : public tagGameEvent
{
	bool bPK;
	tagOpenPKEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 显示死亡菜单游戏事件
*/
struct tagShowDeadMenuEvent : public tagGameEvent
{
	tagShowDeadMenuEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};


struct tagPersonAct;
/** 播放个性动作事件
*/
struct tagPlayPersonalityActEvent : public tagGameEvent
{
	DWORD dwTargetID;
	bool  bDestRole;   // 是否被邀请角色（交互动作）
	const tagPersonAct* pData;
	tagPlayPersonalityActEvent(LPCTSTR szEventName, GameFrame* pSenderFrame,const tagPersonAct* pAct) 
		: tagGameEvent(szEventName,pSenderFrame),dwTargetID(GT_INVALID),pData(pAct),bDestRole(false){}
};

/** 交互个性动作退出事件
*/
struct tagPersonalityActExitEvent : public tagGameEvent
{
	DWORD dwRoleID;   // 播放个性动作的角色ID
	DWORD dwTargetID; // 交互个性动作的目标角色ID（非交互则为GT_INVALID）
	DWORD dwActID;    // 个性动作ID
	tagPersonalityActExitEvent() : tagGameEvent(_T("tagPersonalityActExitEvent"),NULL),dwRoleID(GT_INVALID),dwTargetID(GT_INVALID),dwActID(GT_INVALID){}
	tagPersonalityActExitEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame),dwRoleID(GT_INVALID),dwTargetID(GT_INVALID),dwActID(GT_INVALID){}
};

/** 使用物品游戏事件
*/
struct tagUseItemEvent : public tagGameEvent
{
	DWORD					dwTargetRoleID;
	INT64					n64ItemID;
	DWORD					dwItemTypeID;
	bool					bSpell;		//是否带起手，战斗系统内部使用

	tagUseItemEvent():tagGameEvent(_T("tagUseItemEvent"),NULL),bSpell(false)
	{}
	tagUseItemEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};


/** 拾取物品游戏事件
*/
struct tagPickupItemEvent : public tagGameEvent
{
	INT64					n64ItemID;

	tagPickupItemEvent():tagGameEvent(_T("tagPickupItemEvent"),NULL)
	{}
	tagPickupItemEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 处理缓存的奖励相关消息游戏事件
	\remark 加经验、升级
*/
struct tagProcessRewardMsgEvent : public tagGameEvent
{
	DWORD					dwRoleID;	//被奖励者
	tagProcessRewardMsgEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};


/** 处理缓存的惩罚相关消息游戏事件
	\remark 掉落
*/
struct tagProcessCostMsgEvent : public tagGameEvent
{
	DWORD					dwRoleID;	//被惩罚者
	tagProcessCostMsgEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** NPC播放悠闲动作时说话游戏事件
*/
struct tagNPCEaseSpeechEvent : public tagGameEvent
{
	DWORD					dwNPCID;
	DWORD					dwTypeID;
	tagNPCEaseSpeechEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) 
		: tagGameEvent(szEventName,pSenderFrame),dwNPCID(GT_INVALID),dwTypeID(GT_INVALID)
	{}
};

/** 追踪事件
*/
struct tagPursueEvent : public tagGameEvent
{
	enum ETargetType
	{
		ETargetType_Role,
		ETargetType_Pos,
	};

	ETargetType				targetType;
	Vector3					start;				//起点
	DWORD					targetRoleID;		//目标角色ID
	Vector3					targetPos;			//目标位置
	float					validDist;			//有效距离
	bool					bNeedSendStopCmd;	//是否需要发送停止消息

	tagPursueEvent():tagGameEvent(_T("tagPursueEvent"),NULL)
	{}
	tagPursueEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 停止追踪事件
*/
struct tagStopPursueEvent : public tagGameEvent
{
	tagStopPursueEvent():tagGameEvent(_T("tagStopPursueEvent"),NULL)
	{}
	tagStopPursueEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 追踪完成事件
*/
struct tagPursueFinishEvent : public tagGameEvent
{
	bool					bSuccessed;			//是否成功

	tagPursueFinishEvent():tagGameEvent(_T("tagPursueFinishEvent"),NULL)
	{}
	tagPursueFinishEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 角色开始吟唱事件
*/
struct tagRoleSpellStartEvent : public tagGameEvent
{
	DWORD		dwRoleID;
	bool		bSkill;          // 是否是使用技能吟唱（否则为使用物品）
	DWORD		dwTypeID;        // 技能或物品ID
	DWORD		dwCurTime;       // 当前已经经过的时间
	DWORD		dwTimeCount;     // 总时间

	tagRoleSpellStartEvent():tagGameEvent(_T("tagRoleSpellStartEvent"),NULL),dwRoleID(GT_INVALID),bSkill(false),dwTypeID(GT_INVALID),dwCurTime(0),dwTimeCount(0)
	{}
	tagRoleSpellStartEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 角色结束吟唱事件
*/
struct tagRoleSpellStopEvent : public tagGameEvent
{
	DWORD		dwRoleID;
	bool		bInterrupt;      // 是否被打断

	tagRoleSpellStopEvent():tagGameEvent(_T("tagRoleSpellStopEvent"),NULL),dwRoleID(GT_INVALID),bInterrupt(false)
	{}
	tagRoleSpellStopEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

/** 选中目标事件(只发给被选中的角色)
*/
struct tagTargetSelectEvent : public tagGameEvent
{
	tagTargetSelectEvent():tagGameEvent(_T("tagTargetSelectEvent"),NULL)
	{}
	tagTargetSelectEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};


/** ESC取消普通攻击和起手
*/
struct tagESCCancelEvent : public tagGameEvent
{
	BOOL		bCanceled;

	tagESCCancelEvent():tagGameEvent(_T("tagESCCancelEvent"),NULL),bCanceled(FALSE)
	{}
	tagESCCancelEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};