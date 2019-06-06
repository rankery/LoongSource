#pragma once
#include "FSM_LP.h"

class LocalPlayer;
struct tagPreState;
/** \class LowerState_LP
	\brief 本地玩家下身状态基类
*/
class LowerState_LP
{
public:
	LowerState_LP(void);
	virtual ~LowerState_LP(void);
	void SetFSM(FSM_LP* pFSM){ m_pFSM=pFSM;}			

	//--
	/** 激活一个状态
	*/
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL) =0;	
	/** 激活一个状态
	*/
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,LocalPlayer* pRole,tagPreState* pPreState=NULL){}	
	/** 退出一个状态
	*/
	virtual void OnExit(){};
	/** 重新播放动作
	*/
	virtual void ReplayAni(LocalPlayer* pRole){}
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
	/** 上身状态切换处理
	*/
	virtual void OnChangeUpperState(FSM_LP::EUpperState newState){}

protected:
	FSM_LP* m_pFSM; //所属状态机
};
