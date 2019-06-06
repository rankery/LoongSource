#pragma once
#include "FSM_RP.h"

class Player;
/** \class LowerState_RP
	\brief 远程玩家下身状态基类
*/
class LowerState_RP
{
public:
	LowerState_RP(void);
	virtual ~LowerState_RP(void);
	void SetFSM(FSM_RP* pFSM){ m_pFSM=pFSM;}			

	//--
	/** 激活一个状态
	*/
	virtual void Active(tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole) =0;	
	/** 退出一个状态
	*/
	virtual void OnExit(){};
	/** 重新播放动作
	*/
	virtual void ReplayAni(Player* pRole){}
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
