#pragma once

/*************************
\!brief接取商银界面
*************************/

struct tagRefreshPickUpSilverEvent;
struct tagCommerceInfo;

class GuildPickUpSilverFrame : public GameFrame
{
public:
	GuildPickUpSilverFrame();
	~GuildPickUpSilverFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

private:
	DWORD EventHandler(tagGUIEvent *pEvent);
	DWORD OnEventPickUpSilverRefresh(tagRefreshPickUpSilverEvent *pEvent);

private:
	TSFPTrunk<GuildPickUpSilverFrame> m_Trunk;
	TObjRef<GUISystem> m_pGUI;
	TObjRef<GameFrameMgr> m_pGameFrameMgr;
	
	GUIWnd *m_pWnd;
	GUIStatic *m_pStcGrade;				// 等级
	GUIStatic *m_pStcGold;				// 金
	GUIStatic *m_pStcSilver;			// 银
	GUIStatic *m_pStcPickUpSilver;		// 接取商银
	GUIStatic *m_pStcAimSilver;			// 目标
	
	GUIButton *m_pBtnAccept;			// 接受
	GUIButton *m_pBtnCancel;			// 取消

	const tagCommerceInfo *m_pCurrentCommerceInfo;
};