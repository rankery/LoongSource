#pragma once
#include "FSM_LP.h"

class LocalPlayer;
/** \class ActionState_LP
	\brief 本地玩家动作状态基类
*/
class ActionState_LP
{
public:
	ActionState_LP(void);
	virtual ~ActionState_LP(void);
	void SetFSM(FSM_LP* pFSM){ m_pFSM=pFSM;}			

	/** 激活一个状态
	*/
	virtual void Active(NavMap* pNav,LocalPlayer* pRole) =0;	
	/** 退出一个状态
	*/
	virtual void OnExit(){};
	/** 更新状态逻辑
	*/
	virtual void Update(NavMap* pNav,LocalPlayer* pRole) =0;							
	/** 游戏事件处理
	*/
	virtual void OnGameEvent(tagGameEvent* pEvent) =0;	
	/** 网络消息处理
	*/
	virtual void OnNetCmd(tagNetCmd* pNetCmd) =0;
	/** 是否可以切换上身状态
	*/
	virtual bool IfCanChange2UpperState(FSM_LP::EUpperState state,tagGameEvent* pEvent) =0;
	/** 是否可以切换下身状态
	*/
	virtual bool IfCanChange2LowerState(FSM_LP::ELowerState state,tagGameEvent* pEvent) =0;

protected:
	FSM_LP* m_pFSM; //所属状态机
};
