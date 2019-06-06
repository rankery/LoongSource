#pragma once
#include "FSM_LP.h"

class LocalPlayer;
struct tagPreState;

/** \class UpperState_LP
	\brief 本地玩家上身状态基类
*/
class UpperState_LP
{
public:
	UpperState_LP(void);
	virtual ~UpperState_LP(void);
	void SetFSM(FSM_LP* pFSM){ m_pFSM=pFSM;}			

	//--
	/** 激活一个状态
	*/
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole) =0;	
	/** 激活一个状态
	*/
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole){}
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
	/** 下身状态切换处理
	*/
	virtual void OnChangeLowerState(FSM_LP::ELowerState newState,tagGameEvent* pEvent){}
	/** 是否允许下身播放动作？
	*/
	virtual bool IfCanPlayLowerAni() =0;
	/** 是否允许下身设置朝向？
	*/
	virtual bool IfLowerCanSetYaw() =0;

protected:
	FSM_LP* m_pFSM; //所属状态机
};
