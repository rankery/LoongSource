#pragma once

class GoodsButton;
class IconStatic;
class Item;
struct tagNS_StallSetGoods;
struct tagNS_StallUnsetGoods;
struct tagNS_StallSetTitle;
struct tagNS_StallSetAdText;
struct tagNS_StallSetAdFlag;
struct tagNS_StallSetFinish;
struct tagStallStarEvent;

struct tagItemPosChangeExEvent;

class StallShelfFrame :
	public GameFrame
{
public:
	StallShelfFrame(void);
	~StallShelfFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();

	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

	VOID ReflashShelf(INT16 nIndex);

private:
	
	DWORD EventHandler(tagGUIEvent* pEvent);

	DWORD OnNetStallSetGoods(tagNS_StallSetGoods* pNetCmd, DWORD);
	DWORD OnNetStallUnsetGoods(tagNS_StallUnsetGoods* pNetCmd, DWORD);
	DWORD OnNetStallSetTitle(tagNS_StallSetTitle* pNetCmd, DWORD);
	DWORD OnNetStallSetAdText(tagNS_StallSetAdText* pNetCmd, DWORD);
	DWORD OnNetStallSetAdFlag(tagNS_StallSetAdFlag* pNetCmd, DWORD);
	DWORD OnNetStallSetFinish(tagNS_StallSetFinish* pNetCmd, DWORD);

	DWORD OnEventStallPriceInput(tagMsgMoneyBoxEvent* pGameEvent);
	DWORD OnEventStallAddItem(tagItemPosChangeExEvent* pGameEvent);
	DWORD OnEventStallNameInput(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventStallAdInput(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventShowStar(tagStallStarEvent *pEvent);

	VOID StartStall();

	VOID OnStallAddItem(INT16 n16PosSrc, INT16 n16PosDst = GT_INVALID);
	
	VOID SelGoods(INT16 nIndex);
	VOID SetShangjia(bool bShang);
	VOID ShowGoodsTip(INT16 nIndex);

	VOID AddGoods(Item* pItem, INT16 nIndex = GT_INVALID);
	VOID DelGoods(INT16 nIndex);


	VOID SendPriceInput(INT64 n64Default);

	// 消毁相关对话框
	VOID DestroyMsgBox();
	
private:
	TSFPTrunk<StallShelfFrame>		m_Trunk;
	TObjRef<NetSession>				m_pSession;
	TObjRef<NetCmdMgr>				m_pCmdMgr;
	TObjRef<Util>					m_pUtil;
	TObjRef<GUISystem>				m_pGUI;

	GUIWnd*							m_pWnd;
	GUIWnd*							m_pWndCaption;
	GUIButton*						m_pBtnShangjia;
	GUIButton*						m_pBtnXiajia;
	GUIButton*						m_pBtnStall;
	//GUIButton*						m_pBtnAd;
	GUIButton*						m_pBtnExit;
	GUIButton*						m_pBtnPageup;
	GUIButton*						m_pBtnPagedown;
	GUIStatic*						m_pStcTitle;
	GUIStatic*						m_pStcPage;
	//GUIStaticEx*					m_pStcSignboard;
	//GUIPushButton*					m_pPbnAdFlag;
	typedef std::vector<IconStatic*>	IconStaticList;
	IconStaticList					m_pStcStallSel;
	typedef std::vector<GoodsButton*>	GoodsButtonList;
	GoodsButtonList					m_pStcStallGoods;
	IconStatic						*m_pIstStar[10];

	INT16							m_nSelGoods;
	bool							m_bSelLock;

};
