#pragma once

class FSM_NPC;
class NPC;
/** \class State_NPC
	\brief NPC状态基类
*/
class State_NPC
{
public:
	State_NPC(void);
	virtual ~State_NPC(void);
	void SetFSM(FSM_NPC* pFSM){ m_pFSM=pFSM;}			

	//--
	/** 激活一个状态
	*/
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole) =0;		
	/** 退出一个状态
	*/
	virtual void OnExit(){};
	/** 更新状态逻辑
	*/
	virtual void Update(NavMap* pNav,NPC* pRole) =0;							
	/** 网络消息处理
	*/
	virtual void OnNetCmd(tagNetCmd* pNetCmd) =0;
	/** 游戏事件处理
	*/
	virtual void OnGameEvent(tagGameEvent* pEvent) =0;	

protected:
	FSM_NPC* m_pFSM; //所属状态机
};
