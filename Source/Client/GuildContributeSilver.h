#pragma once

/**************************************
\!brief 缴纳商银界面
**************************************/

struct tagRefreshContributeSilverEvent;
struct tagCommerceInfo;

class GuildContributeSilverFrame : public GameFrame
{
public:
	GuildContributeSilverFrame();
	~GuildContributeSilverFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

private:
	DWORD EventHandler(tagGUIEvent *pEvent);
	DWORD OnEventContributeSilverRefresh(tagRefreshContributeSilverEvent *pEvent);
private:

	TSFPTrunk<GuildContributeSilverFrame> m_Trunk;
	TObjRef<GUISystem> m_pGUI;
	TObjRef<GameFrameMgr> m_pGameFrameMgr;

	GUIWnd *m_pWnd;
	GUIStatic *m_pStcGrade;
	GUIStatic *m_pStcGold;
	GUIStatic *m_pStcSilver;
	GUIStatic *m_pStcAimSilver;
	GUIStatic *m_pStcOwnSilver;
	GUIButton *m_pBtnContribute;
	GUIButton *m_pBtnCancel;

	const tagCommerceInfo *m_pCurrentCommerceInfo; 
	INT m_nAcceptLevel;		//接取时玩家等级
};