#pragma once
#include "gameframe.h"


struct tagGuildMemberClient;
struct tagGuildInfoReflashEvent;
class HttpPicEx;
struct tagGetHeadpicByID;

class GuildMemberInfoFrame :
	public GameFrame
{
public:
	GuildMemberInfoFrame(void);
	~GuildMemberInfoFrame(void);
	
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();

	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

private:
	DWORD EventHandler(tagGUIEvent* pEvent);

	DWORD OnEventGuildInfoUpdate(tagGuildInfoReflashEvent* pGameEvent);
	DWORD OnEventGuildInfoReflash(tagGuildInfoReflashEvent* pGameEvent);
	DWORD OnEventGuildPosChange(tagGuildInfoReflashEvent* pGameEvent);
	DWORD OnEventGuildRemoveMember(tagGuildInfoReflashEvent* pGameEvent);

	VOID ReflashInfo(const tagGuildMemberClient* pMember);
	DWORD OnEventGetHeadPicByID(tagGetHeadpicByID *pEvent);

private:
	TSFPTrunk<GuildMemberInfoFrame>		m_Trunk;
	TObjRef<NetSession>					m_pSession;
	TObjRef<GUISystem>					m_pGUI;

	GUIWnd*								m_pWnd;
	GUIWnd*								m_pWndInfo;
	GUIButton*							m_pBtnClose;		// 关闭
	GUIButton*							m_pBtnFriend;		// 加为好友
	GUIButton*							m_pBtnTeam;			// 组队邀请
	GUIButton*							m_pBtnChat;			// 设为私聊
	GUIButton*							m_pBtnReflash;		// 更新信息
	HttpPicEx*							m_pStcImage;		// 大头贴
	GUIStatic*							m_pStcName;			// 名称
	GUIStatic*							m_pStcPosition;		// 职位
	GUIStatic*							m_pStcLevel;		// 等级
	GUIStatic*							m_pStcSex;			// 性别
	GUIStatic*							m_pStcClass;		// 职业
	GUIStatic*							m_pStcCont;			// 帮派贡献
	GUIStatic*							m_pStcTotalCont;	// 累计贡献
	GUIStatic*							m_pStcKnowledge;	// 帮派阅历	
	GUIStatic*							m_pStcExploit;		// 帮派功勋

	DWORD								m_dwRoleID;			// 当前玩家
};
