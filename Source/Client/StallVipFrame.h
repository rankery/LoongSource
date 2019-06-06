#pragma once

struct tagNS_ApplyVIPStall;
struct tagNS_SpecVIPStallGet;
/*
	class: StallVipFrame
	brief: VIP摊位

*/
class StallVipFrame : public GameFrame
{
public:
	StallVipFrame(void);
	~StallVipFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();


private:
	DWORD EventHandler(tagGUIEvent* pEvent);

	//网络消息
	DWORD OnNS_ApplyVIPStall(tagNS_ApplyVIPStall* pMsg, DWORD dwParam);
	DWORD OnNS_SpecVIPStallGet(tagNS_SpecVIPStallGet* pMsg, DWORD dwParam);

	//游戏事件
	DWORD OnAllVipStallInfoEvent(tagGameEvent* pEventGame);
	DWORD OnRentVipStallEvent(tagMsgBoxEvent* pEventGame);

private:
	TSFPTrunk<StallVipFrame>		m_Trunk;
	TObjRef<NetSession>				m_pSession;
	TObjRef<NetCmdMgr>				m_pCmdMgr;
	TObjRef<Util>					m_pUtil;
	TObjRef<GUISystem>				m_pGUI;

	GUIWnd*							m_pWnd;
	GUIButton*						m_pBtnRent;				//申请
	GUIButton*						m_pBtnCheck;			//查看
	GUIButton*						m_pBtnCancel;			//关闭
	GUIListBox*						m_pLVipShop;			//摊位


	INT								m_nCurSelectedRow;		//当前所选摊位

};