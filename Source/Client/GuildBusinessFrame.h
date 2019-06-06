#pragma once

/*************************
\!brief 帮派跑商界面
*************************/

class IconStaticEx;

const int COMMERCE_NUMBER = 10;
const int GOODS_NUMBER = 10;
const tstring strEmpty = _T("data\\ui\\main\\l_icon_back.bmp");
const tstring strBackEmpty = _T("data\\ui\\common\\l_icon.bmp");
const tstring strBackChoose = _T("data\\ui\\Common\\L_icon-l.bmp");

class GuildBusinessFrame : public GameFrame
{
public:
	GuildBusinessFrame();
	~GuildBusinessFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

	bool IsShow() const;
	void Show(bool bShow);
	VOID Update();



private:
	DWORD EventHandler(tagGUIEvent *pEvent);
	DWORD OnEventRefreshCOC(tagGameEvent *pEvent);
	DWORD OnEventRefreshCommodity(tagGameEvent *pEvent);
	DWORD OnEventBuyMsgInputBox(tagMsgInputBoxEvent *pEvent);
	DWORD OnEventSellMsgInputBox(tagMsgInputBoxEvent *pEvent);
	VOID SetDefaultCOCBagUI();
	VOID SetDefaultCommodityBagUI();
	VOID SetDefaultCOCBagBackUI();
	VOID SetDefaultCommodityBagBackUI();
	VOID OnCOCBagClick(int n);
	VOID OnCommodityBagClick(int n);

private:
	TSFPTrunk<GuildBusinessFrame> m_Trunk;
	TObjRef<GUISystem> m_pGUI;
	TObjRef<GameFrameMgr> m_pGameFrameMgr;

	GUIWnd *m_pWnd;
	IconStaticEx *m_pCommerceBack[COMMERCE_NUMBER];
	IconStaticEx *m_pCommerceItem[COMMERCE_NUMBER];
	GUIStatic *m_pStcCommerceName[COMMERCE_NUMBER];
	GUIStatic *m_pStcCommercePrice[COMMERCE_NUMBER];
	
	IconStaticEx *m_pGoodsBack[GOODS_NUMBER];
	IconStaticEx *m_pGoodsItem[GOODS_NUMBER];
	GUIStatic *m_pStcGoodsName[GOODS_NUMBER];
	GUIStatic *m_pStcGoodsPrice[GOODS_NUMBER];

	GUIStatic *m_pStcSilver;
	GUIStatic *m_pStcPurpose;

	GUIButton *m_pBtnClose;
	GUIButton *m_pBtnBuy;
	GUIButton *m_pBtnSell;
	GUIStatic *m_pStcCOCDesc;

	bool m_bShow;
	DWORD m_dwCurrentChooseGoodID;	//当前选中包里的商货ID
	DWORD m_dwCurrentChooseCommerceID;//当前选中商会的商货ID
	DWORD m_dwTime;
};
