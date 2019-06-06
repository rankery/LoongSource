/********************************************************************
	created:	2008/12/10
	created:	10:12:2008   11:31
	filename: 	d:\loong_client\Client\SocialListFrame.h
	file path:	d:\loong_client\Client
	file base:	SocialListFrame
	file ext:	h
	author:		leili
	
	purpose:	好友列表界面声明
*********************************************************************/

#pragma once

#include "SocialMgr.h"

struct tagRoleGetNameEvent;
struct tagRoleGetIDEvent;

class SocialListFrame : public GameFrame
{
	enum EMenuItem
	{
		EMI_Chat,				//私聊
		EMI_GiveGift,			//赠送礼物
		EMI_PlayerInfo,			//玩家资料
		EMI_InviteGroup,		//邀请组队
		EMI_ApplyGroup,			//申请组队
		EMI_MoveTo,				//移动至
		EMI_CopyName,			//复制名称
		EMI_Num,
	};

public:
	SocialListFrame(void);
	virtual ~SocialListFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

	VOID		 Show(void);
	VOID		 Hide(void);

	VOID		 RefreshList(SocialData::EListType listType);
	VOID		 RefreshList(void);

private:
	BOOL		 EventHandler(tagGUIEvent* pEvent);
	BOOL		 EventHandlerChild1(tagGUIEvent* pEvent);
	BOOL		 EventHandlerChildGroup(tagGUIEvent* pEvent);
	BOOL		 EventHandlerChildGift(tagGUIEvent* pEvent);

	DWORD		 OnAddFriend(tagMsgInputBoxEvent* pEvent);
	DWORD		 OnAddBlack(tagMsgInputBoxEvent* pEvent);
	DWORD		 OnDeleteFriend(tagMsgBoxEvent* pEvent);
	DWORD		 OnGetRoleName(tagRoleGetNameEvent* pEvent);
	DWORD		 OnSetReply(tagMsgInputBoxEvent* pEvent);
	DWORD		 OnGetRoleID(tagRoleGetIDEvent* pEvent);
	DWORD		 OnGetRoleIDError(tagGameEvent* pEvent);
	VOID		 DeleteBlack(DWORD roleID);

	VOID		 ShowFriendList(SocialData::EListType listType, bool bShowOutline);
	VOID		 ShowBlackList(bool bShowOutline);

	VOID		 ResizeMenu(GUIListBox* pListBox, int row, int width);
	VOID		 MoveMenu(GUIListBox* pListBox, const tagPoint& ptDest);

	VOID		 MoveToList(SocialData::EListType listType, int row);

	VOID		 SetFriendValPic(DWORD row,DWORD friendVal);

	VOID		 UpdateMenu(void);

private:
	TSFPTrunk<SocialListFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<MsgInputBox>		m_pMsgInputBox;
	TObjRef<MsgBox>				m_pMsgBox;

	GUIWnd						*m_pWnd;				// 主界面
	GUIListBox					*m_pListFriends;		// 好友列表
	GUIPushButton				*m_pPshFriend1;			// 好友Tab1
	GUIPushButton				*m_pPshFriend2;			// 好友Tab2
	GUIPushButton				*m_pPshFriend3;			// 好友Tab3
	GUIPushButton				*m_pPshFriend4;			// 好友Tab4
	GUIPushButton				*m_pPshBlack;			// 黑名单Tab
	GUIButton					*m_pBtnAddFriend;		// 增加好友按钮
	GUIButton					*m_pBtnAddBlack;		// 增加黑名单按钮
	GUIButton					*m_pBtnDelete;			// 删除按钮
	GUIButton					*m_pBtnClose;			// 关闭窗口按钮
	GUIPushButton				*m_pPshSwitchOnline;	// 离线开关按钮
	GUIStatic					*m_pLevelCaption;		// 等级标题
	GUIPushButton				*m_pPshAutoReply;		// 自动回复按钮

	SocialData::EListType		m_curListTab;			// 当前ListTab
	bool						m_showOutline;			// 是否显示离线好友或黑名单
	bool						m_autoReply;			// 是否自动回复
	bool						m_bIsInitUI;			// 是否初始化UI

	GUIWnd						*m_pWndChild1;			// 一级子菜单
	GUIListBox					*m_pListMenu1;			// 一级列表
	GUIWnd						*m_pWndChildGroup;		// 分组 二级子菜单
	GUIListBox					*m_pListMenuGroup;		// 分组 二级列表
	GUIWnd						*m_pWndChildGift;		// 礼物 二级子菜单
	GUIListBox					*m_pListMenuGift;		// 礼物 二级列表

	DWORD						m_curRow;
	int							m_clickRow;				// 当前选中的Row

	enum EAddType
	{
		EAT_None	= -1,
		EAT_Friend	= 0,
		EAT_Black
	};
	EAddType					m_eAddType;

	typedef std::pair<DWORD,DWORD> DelayNameRow;
	std::list<DelayNameRow>	m_delayNameRows;

	TList<DWORD>				m_gifts;				// 背包中的礼物列表

	enum EUIState
	{
		EUIS_OutOfChild1	= 1,
		EUIS_OutOfChild2	= 2,
	};
	DWORD						m_uiState;
	bool						m_bPreHideChild;
	float						m_startHideChildTime;

	INT							m_nCurMenuIndex[EMI_Num];  // 当前目标菜单选项对应ListBox行索引
};