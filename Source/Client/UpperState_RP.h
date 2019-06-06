#pragma once
#include "FSM_RP.h"

class Player;

/** \class UpperState_RP
	\brief 远程玩家上身状态基类
*/
class UpperState_RP
{
public:
	UpperState_RP(void);
	virtual ~UpperState_RP(void);
	void SetFSM(FSM_RP* pFSM){ m_pFSM=pFSM;}			

	//--
	/** 激活一个状态
	*/
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole) =0;	
	/** 激活一个状态
	*/
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,Player* pRole){}	
	/** 退出一个状态
	*/
	virtual void OnExit(){};
	/** 更新状态逻辑
	*/
	virtual void Update(NavMap* pNav,Player* pRole) =0;							
	/** 游戏事件处理
	*/
	virtual void OnGameEvent(tagGameEvent* pEvent) =0;	
	/** 网络消息处理
	*/
	virtual void OnNetCmd(tagNetCmd* pNetCmd) =0;
	/** 是否允许下身播放动作？
	*/
	virtual bool IfCanPlayLowerAni() =0;
	/** 是否允许下身设置朝向？
	*/
	virtual bool IfLowerCanSetYaw() =0;
	/** 下身状态改变
	*/
	virtual void OnChangeLowerState(FSM_RP::ELowerState state){}

protected:
	FSM_RP* m_pFSM; //所属状态机
};
