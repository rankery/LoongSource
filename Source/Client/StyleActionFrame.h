#pragma once


#include "PersonalityActData.h"

struct tagNS_DuetMotionInvite;
struct tagNS_DuetMotionOnInvite;
struct tagRoleGetNameEvent;
struct tagNS_RoleStyleAction;
struct tagMouseMoveFinishEvent;

/** 尝试播放个性动作事件
*/
struct tagTryToPlayPersonalityActEvent : public tagGameEvent
{
	const tagPersonAct* pData;
	tagTryToPlayPersonalityActEvent(LPCTSTR szEventName, GameFrame* pSenderFrame,const tagPersonAct* pAct) 
		: tagGameEvent(szEventName,pSenderFrame),pData(pAct){}
};


/** \class StyleActionFrame
	\brief 个性动作
*/
class StyleActionFrame : public GameFrame
{
public:
	StyleActionFrame();
	~StyleActionFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();
private:
	BOOL EventHandler(tagGUIEvent* pEvent);
	VOID SafeDestroyUI();
	VOID ShowInviteMsgBox( const TCHAR* szInviteName );
	void MoveToTarget(const Vector3& vTargetPos);
	/** \游戏事件响应
	*/
	DWORD OnOpenClose_StyleActionEvent(tagGameEvent* );
	DWORD OnTryToPlayPersonalityActEvent(tagTryToPlayPersonalityActEvent* pEvent);
	DWORD OnStyleActionInviteMsgBox( tagMsgBoxEvent* pEvent );
	DWORD OnRoleGetNameEvent(tagRoleGetNameEvent* pEvent);
	DWORD OnMouseMoveFinishEvent( tagMouseMoveFinishEvent* pEvent );
	/** \网络消息响应
	*/
	DWORD OnNS_DuetMotionInvite( tagNS_DuetMotionInvite* pCmd, DWORD );
	DWORD OnNS_DuetMotionOnInvite( tagNS_DuetMotionOnInvite* pCmd, DWORD );
	DWORD OnNS_RoleStyleAction(tagNS_RoleStyleAction* pNetCmd, DWORD);        // 处理本地玩家交互动作的播放
private:
	TSFPTrunk<StyleActionFrame>			m_Trunk;
	TObjRef<GUISystem>					m_pGUI;
	TObjRef<NetCmdMgr>					m_pCmdMgr;
	TObjRef<NetSession>					m_pSession;
	GUIWnd*                             m_pWnd;
	GUIButton*							m_pBn_Close;
	DWORD								m_dwActionTargetID;			// 需要保存目标ID和动作结构体
	tagPersonAct                        m_sPersonActData;
	bool                                m_bWaitRemotePesponses;		// 正在等待对方回应
	bool                                m_bWaitLocalPesponses;		// 正在等待自己回应
	DWORD								m_dwInviteMsgBoxShowTime;
	DWORD								m_dwWaitRemoteResponseTime; // 开始等待服务器回应的时间点
	bool								m_bWaitForInviteRoleName;
	bool                                m_bMovingTo;                // 正在跑向目标
	bool                                m_bReadyToStart;			// 已经到达目标位置
};