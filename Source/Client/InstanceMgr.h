#pragma once

#include "..\WorldDefine\msg_map.h"
#include "InstanceProtoData.h"


class CountDownFrame;
class QuestTargetWnd;
class InstanceProtoData;

struct tagLeaveTeamEvent;

class InstanceMgr
{
public:
	~InstanceMgr();
	/*!
	\brief 获取单件对象指针
	*/
	static InstanceMgr *Inst();

	/*!
	\brief 初始化
	*/
	BOOL Init();

	/*!
	\brief 销毁
	*/
	VOID Destroy();
	
	/*!
	\brief 加载副本属性表
	*/
	BOOL LoadInstanceProtoTable();

	
	VOID TryGotoInstance(DWORD dwTriggerID, DWORD dwMapID);
	VOID TryOutInstance(DWORD dwTriggerID, DWORD dwMapID);
	/*!
	\brief 发送触发副本消息
	*/
	VOID Send_TriggerInstance(DWORD dwTriggerID, DWORD dwLevel);

	/*!
	\brief 发送是否通知队友网络消息
	*/
	VOID Send_NotifyTeammate(bool bNofify);

	/*!
	\brief 发送是否同意进入副本网络消息
	*/
	VOID Send_AgreeGotoInstance(bool bAgree);

	/*!
	\brief 发送请求离开副本网络消息
	*/
	VOID Send_RequestLeaveInstance();
	
	/*!
	\brief 客户端检查是否能进入副本的条件
	*/
	BOOL IsAbleToGotoInstance();

	/*!
	\brief 进入前是否询问
	*/
	BOOL IsAskBeforeEnter();
	/*!
	\brief 是否能进行战斗
	*/
	BOOL IsAbleToCombat();
	/*!
	\brief 是否可以PK
	*/
	BOOL IsAbleToPK();
	/*!
	\brief 是否在保护模式下任然可以PK,即保护模式失效
	*/
	BOOL IsloseSafeGuard();

	/*!
	\brief 角色是否在副本中
	*/
	BOOL IsInInstance();

	/*!
	\brief 创建询问是否进入对话框
	*/		
	VOID CreateAskEnterMsgBox();	
	
	/*!
	\brief 创建询问是否离开副本对话框
	*/			
	VOID CreateAskOutMsgBox();
	
	/*!
	\brief 创建难度选择对话框
	*/
	VOID CreateDifficultyChooseOptionBox();
	
	EInstanceHardMode GetHardMode() const;
	
	
	
private:
	InstanceMgr();
	TSFPTrunk<InstanceMgr>		m_trunk;
	TObjRef<VarContainer>		m_pVarContainer;
	TObjRef<NetCmdMgr>			m_pNetCmdMgr;
	TObjRef<GameFrameMgr>		m_pGameFrameMgr;
	TObjRef<NetSession>			m_pSession;
	CountDownFrame				*m_pCountDownFrame;
	QuestTargetWnd				*m_pQuestTargetWnd;

	InstanceProtoData			m_instanceProtoData;	// 副本属性表数据
	DWORD						m_dwTargetMapID;		// 准备进入的副本ID
	DWORD						m_dwTriggerID;			// 副本触发器ID
	DWORD						m_dwOutTargetMapID;		// 准备出去的地图ID
	DWORD						m_dwOutTriggerID;		// 出去的触发器ID

	EInstanceHardMode			m_eHardMode;			// 副本难度模式
	BOOL						m_bIn;					// 当前是否在副本中
	
	/*!
	\brief 响应通知本地玩家是否召唤队友网络消息
	*/
	DWORD OnNS_InstanceNofity(tagNS_InstanceNofity *pCmd, DWORD);
	/*!
	\brief 响应通知本地玩家是否进入副本网络消息
	*/
	DWORD OnNS_InstanceAgree(tagNS_InstanceAgree *pCmd, DWORD);
	/*!
	\brief 响应副本完成网络消息
	*/
	DWORD OnNS_InstanceComplete(tagNS_InstanceComplete *pCmd, DWORD);
	
	DWORD OnNS_EnterInstance(tagNS_EnterInstance *pCmd, DWORD);
	
	DWORD OnInstanceNotifyBox(tagMsgBoxEvent *pEvent);

	DWORD OnInstanceAgreeBox(tagMsgBoxEvent *pEvent);

	DWORD OnInstanceCompleteBox(tagMsgBoxEvent *pEvent);

	DWORD OnActiveTransmitPoint(tagMsgBoxEvent *pEvent);

	DWORD OnAskEnterMsgBox(tagMsgBoxEvent *pEvent);

	DWORD OnAskOutMsgBox(tagMsgBoxEvent *pEvent);

	DWORD OnInstanceDifficultyBox(tagMsgOptionBoxEvent *pEvent);
	
	DWORD OnEnterInstanceEvent(tagGameEvent*);

	DWORD OnLeaveInstanceEvent(tagGameEvent*);

	DWORD OnLeaveTeamEvent(tagGameEvent*);
	
	VOID CreateQuestTargetWindow();

	VOID DestroyQuestTargetWindow();

	/*!
	\brief 各种属性重置
	*/
	VOID Reset();
};