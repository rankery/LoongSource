#pragma once

#include "..\WorldDefine\ScriptMsgInfo.h"
#include "..\WorldDefine\role_info.h"
#include "ChatFrame.h"

class ActivityBroadcastHandler
{
public:
	static ActivityBroadcastHandler* Inst();

	~ActivityBroadcastHandler();

	void Init();

	void Destroy();

private:
	// 构造函数
	ActivityBroadcastHandler();

	// 注册网络消息NS_MsgBlob
	DWORD OnNS_MsgBlob(tagNS_MsgBlob *pCmd, DWORD);
	
	// 初始化msg_id与BroadcastImpl函数指针对应..
	void InitMsgID2BroadcastImplMap();
	
	// 处理取得一些名字的事件
	DWORD OnGetSomeNameOKEvent(tagRoleGetSomeNameEvent *pEvent);
	
	// 处理网络命令NS_MsgBlob
	void HandleNetMsg(tagNS_MsgBlob *pCmd);
	
	// 开始广播
	void StartBroadcast();
	
	// 清空获得的MsgUnit;
	void ClearMsgUnitVector();
	
	// 从取得的ROLENAME中得到名字
	tstring GetRoleName(DWORD dwRoleID) const;

	//通知广播
	inline void NotifyBroadcast(const TCHAR *szBuffer);
	
	// 释放消息数据资源
	void ReleaseMsgUnit();


//-------------------星宿下凡-----------------------------
	void BroadcastImpl1(const TCHAR *szBroadcastContent);
	void BroadcastImpl2(const TCHAR *szBroadcastContent);
	void BroadcastImpl3(const TCHAR *szBroadcastContent);
	void BroadcastImpl4(const TCHAR *szBroadcastContent);
	void BroadcastImpl5(const TCHAR *szBroadcastContent);
//--------------------------------------------------------

//-------------------烽火连天-----------------------------
	void BroadcastImpl10(const TCHAR *szBroadcastContent);
	void BroadcastImpl11(const TCHAR *szBroadcastContent);
	void BroadcastImpl12(const TCHAR *szBroadcastContent);
	void BroadcastImpl13(const TCHAR *szBroadcastContent);
	void BroadcastImpl14(const TCHAR *szBroadcastContent);
	void BroadcastImpl15(const TCHAR *szBroadcastContent);
	void BroadcastImpl16(const TCHAR *szBroadcastContent);
	void BroadcastImpl17(const TCHAR *szBroadcastContent);
//--------------------------------------------------------

	void BroadcastImpl30(const TCHAR *szBroadcastContent);
	void BroadcastImpl31(const TCHAR *szBroadcastContent);
	void BroadcastImpl35(const TCHAR *szBroadcastContent);
	
private:
	std::list<tagNS_MsgBlob*> m_MsgList;	// 缓存接收到的网络命令
	std::vector<DWORD> m_ReqIDList;			// 当前消息需要处理的名字ID
	std::vector<MsgUnit*> m_vecMsgUnit;		// 收到的数据
	bool m_bHandlingNow;					// 是否正在处理
	int m_msgID;							// 消息ID
	
	// 指向BroadcastImpl##的类成员函数指针
	typedef void (ActivityBroadcastHandler::*BroadcastImplPtr)(const TCHAR*);
	// msgid和类成员函数指针对应的map
	std::map<int, BroadcastImplPtr> m_mapMsgID2BroadcastImpl;
	
	TObjRef<NetSession>					m_pSession;
	TSFPTrunk<ActivityBroadcastHandler> m_trunk;
	TObjRef<NetCmdMgr>					m_pNetCmdMgr;
	TObjRef<GameFrameMgr>				m_pGameFrameMgr;
	TObjRef<GameScriptMgr>				m_pGSMgr;
};

void ActivityBroadcastHandler::NotifyBroadcast(const TCHAR *szBuffer)
{
	ChatFrame* pFrame = (ChatFrame*)m_pGameFrameMgr->GetFrame(_T("Chat"));
	if (P_VALID(pFrame))
	{
		pFrame->PushScrollText(szBuffer);
		pFrame->PushInfo(szBuffer, ESCC_Affiche);
	}
	
}