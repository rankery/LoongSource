#pragma once

class FSM_LP;
class LocalPlayer;
/** \class GlobalState_LP
	\brief 本地玩家全局状态基类
*/
class GlobalState_LP
{
public:
	GlobalState_LP(void);
	virtual ~GlobalState_LP(void);
	void SetFSM(FSM_LP* pFSM){ m_pFSM=pFSM;}			

	//--
	/** 激活一个状态
	*/
	virtual void Active(tagGameEvent* pEvent,NavMap* pNav,LocalPlayer* pRole) =0;		
	/** 更新状态逻辑
	*/
	virtual void Update(NavMap* pNav,LocalPlayer* pRole) =0;							
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
	FSM_LP* m_pFSM; //所属状态机
	bool m_bClosed;
};
