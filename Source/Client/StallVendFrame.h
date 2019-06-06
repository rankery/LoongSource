#pragma once

class ItemButton;
class IconStatic;
class TemporaryContainer;
class Item;
struct tagTemporaryItem;
struct tagNS_StallGet;
struct tagNS_StallBuy;
struct tagNS_StallBuyRefresh;
struct tagNS_VIPStallBuy;
struct tagStallStarEvent;

class StallVendFrame :
	public GameFrame
{
public:
	StallVendFrame(void);
	~StallVendFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();

	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

private:

	DWORD EventHandler(tagGUIEvent* pEvent);

	DWORD OnNetStallGet(tagNS_StallGet* pNetCmd, DWORD);
	DWORD OnNetStallBuy(tagNS_StallBuy* pNetCmd, DWORD);
	DWORD OnNetStallBuyRefresh(tagNS_StallBuyRefresh* pNetCmd, DWORD);
	DWORD OnNS_VIPStallBuy(tagNS_VIPStallBuy* pMsg, DWORD);

	DWORD OnEventStallBuyNumber(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventShowStar(tagStallStarEvent *pEvent);

	VOID SelGoods(INT16 nIndex, BOOL bForce = FALSE);

	VOID BuySelGoods(bool bShift);
	VOID BuyGoods(tagTemporaryItem* pTemporary, INT16 n16Quantity = 1);

	VOID ShowGoodsTip(INT16 nIndex);

	VOID AddItem(Item* pItem, INT64 n64Price, INT16 nIndex);
	VOID DelItem(INT16 nIndex);
	VOID UpdateItem(Item* pItem, INT64 n64Price, INT16 nIndex);

	VOID DestroyMsgBox();

private:
	TSFPTrunk<StallVendFrame>		m_Trunk;
	TObjRef<NetSession>				m_pSession;
	TObjRef<NetCmdMgr>				m_pCmdMgr;
	TObjRef<Util>					m_pUtil;
	TObjRef<GUISystem>				m_pGUI;

	GUIWnd*							m_pWnd;
	GUIWnd*							m_pWndCaption;
	GUIButton*						m_pBtnBuy;
	GUIButton*						m_pBtnCancel;
	GUIButton*						m_pBtnExit;
	GUIButton*						m_pBtnPageup;
	GUIButton*						m_pBtnPagedown;
	GUIStatic*						m_pStcTitle;
	GUIStatic*						m_pStcPage;
	//GUIStaticEx*					m_pStcSignboard;
	typedef std::vector<IconStatic*>	IconStaticList;
	IconStaticList					m_pStcStallSel;
	typedef std::vector<ItemButton*>	ItemButtonList;
	ItemButtonList					m_pStcStallGoods;

	INT16							m_nSelGoods;
	bool							m_bSelLock;
	DWORD							m_dwStallRoleID;

	TemporaryContainer*				m_pContainerStall;
	TList<Item*>					m_listStallItem;

	IconStatic						*m_pIstStar[10];

};
