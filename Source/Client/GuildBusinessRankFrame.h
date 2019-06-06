#pragma once

#include "..\WorldDefine\role_info.h"
struct tagRoleGetSomeNameEvent;

struct tagRefreshGuildBusinessRankEvent;
struct tagRefreshBusinessRankCommendStateEvent;

class GuildBusinessRankFrame : public GameFrame
{
public:
	GuildBusinessRankFrame();
	~GuildBusinessRankFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();
	bool IsShow() const;
	void Show(bool bShow);
private:
	DWORD EventHandler(tagGUIEvent *pEvent);
	DWORD OnRefreshEvent(tagRefreshGuildBusinessRankEvent *pEvent);
	DWORD OnGetSomeNameOKEvent(tagRoleGetSomeNameEvent *pEvent);
	DWORD OnRefreshStateEvent(tagRefreshBusinessRankCommendStateEvent *pEvent);
	
private:
	TSFPTrunk<GuildBusinessRankFrame> m_Trunk;
	TObjRef<GUISystem> m_pGUI;
	TObjRef<GameFrameMgr> m_pGameFrameMgr;

	GUIWnd *m_pWnd;
	GUIListBox *m_pListRank;
	GUIStatic *m_pStcCommendationState;	// 开启或关闭嘉奖的状态
	GUIButton *m_pBtnCommendationOpen;	// 开启嘉奖
	GUIButton *m_pBtnCommendationClose;	// 关闭嘉奖
	GUIButton *m_pBtnClose;				// 关闭
	BOOL m_bCommend;					// 嘉奖状态
	bool m_bShow;
	std::vector<DWORD> m_ReqIDList;		// 需要查找名字的表
	std::vector<tagCommerceRank> m_vecSaveRank;	// 如果没找到名字保存排名信息
};

