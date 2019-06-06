#pragma once

class FSM_RP;
class Player;
/** \class GlobalState_RP
	\brief 远程玩家全局状态基类
*/
class GlobalState_RP
{
public:
	GlobalState_RP(void);
	virtual ~GlobalState_RP(void);
	void SetFSM(FSM_RP* pFSM){ m_pFSM=pFSM;}			

	//--
	/** 激活一个状态
	*/
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole,DWORD dwParam=0){};		
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,Player* pRole,DWORD dwParam=0){};		
	/** 更新状态逻辑
	*/
	virtual void Update(NavMap* pNav,Player* pRole) =0;							
	/** 游戏事件处理
	*/
	virtual void OnGameEvent(tagGameEvent* pEvent) =0;	
	/** 网络消息处理
	*/
	virtual void OnNetCmd(tagNetCmd* pNetCmd) =0;
	/** 是否处于关闭状态
	*/
	bool IsClosed(){ return m_bClosed;}

protected:
	FSM_RP* m_pFSM; //所属状态机
	bool m_bClosed;
};
