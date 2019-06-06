#pragma once
#include "FSM_RP.h"

class Player;
/** \class ActionState_RP
	\brief 远程玩家动作状态基类
*/
class ActionState_RP
{
public:
	ActionState_RP(void);
	virtual ~ActionState_RP(void);
	void SetFSM(FSM_RP* pFSM){ m_pFSM=pFSM;}			

	/** 激活一个状态
	*/
	virtual void Active(NavMap* pNav,Player* pRole) =0;	
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
	/** 是否可以切换上身状态
	*/
	virtual bool IfCanChange2UpperState(FSM_RP::EUpperState state) =0;

protected:
	FSM_RP* m_pFSM; //所属状态机
};
