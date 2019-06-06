#pragma once
#include "..\WorldDefine\mall_define.h"
#include "..\WorldDefine\msg_mall.h"

/** class MallFrame
	brief 商城
*/
class NetSession;
class IconStatic;
class PlayerTryDressWnd;
class ItemButton;
class MallButton;
struct tagMsgMallGiftEvent;
struct tagRoleYuanbaoChange;
struct tagExchangeVolumeChange;
class HttpPicEx;
#define MALL_MAXITEM_PERPAGE 18
#define MALL_ITEM_STATE 5
#define MALL_MAX_PROMOTE_ITEM 5
#define MALL_ITEM_SUBTYPE	8
#define SPECIAL_TYPE 2
#define MALL_MAXPACK_PERPAGE 7
#define MALL_PACKPRO_NUM 5
class MallFrame : public GameFrame
{
public:
	enum EMallItemType 
	{
		EMIT_All		= -2,
		EMIT_Package	= -1,
	};

	enum EMallItemStateType			//商品状态图标:位于商品图标下方
	{
		EMIST_Promote	= 0,		//促销
		EMIST_GroupBuy	= 1,		//团购
		EMIST_Present	= 2,		//赠送
		EMIST_Num		= 3,		//限量
		EMIST_Exchange	= 4,		//兑换
	};

	enum EMamllItemTipsType
	{
		EMITT_All		= 0,
		EMITT_Package,
		EMITT_Promote,
	};

	enum EMallItemOperateState
	{
		EMIOS_NULL				= 0,
		EMIOS_NormalItem,
		EMIOS_PackItem,
		EMIOS_Promote,
		EMIOS_Free,
	};

public:
	MallFrame(void);
	~MallFrame(void);

    tstring GetGUIFullName()
    {
        if(P_VALID(m_pWnd))
            return m_pWnd->GetFullName();
        else 
            return _T("");
    };

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Render3D();
	virtual VOID Update();
	virtual BOOL EscCancel();

	// 非打包商品归类
	void ClassifiedMallItem();

	void ShowWnd(BOOL bVal);

	//更新avatar
	void UpdateAvatar();

	//获得先择商品
	DWORD GetMallItemHopeTypeID() { return m_dwSelectItemID; }
	EMallItemOperateState GetOpereateState() { return m_eOperateState; }


private:
	// 处理GUI消息
	DWORD EventHandler(tagGUIEvent* pEvent);

	//刷新商品选择框
	void RefreshMallItemFlag();

	//刷新推荐商品
	void RefreshPromoteItem();

	//刷新免费领取商品
	void RefreshFreeItem();

	//显示当前页的商品(按类别)
	void ShowCurPageMallItemByType(INT nPage);

	//显示当前页的商品(全部)
	void ShowCurPageMallItemByAll(INT nPage);

	//显示当前页的礼包
	void ShowCurPageMallPack(INT nPage);

	//判断显示商品
	void JudgeShowMallItem(INT nMenu);

	//--翻页
	void PageUp();
	void PageDown();

	//显示当前页码
	void SetCurrentPage();

	//设置要购买的商品ID
	void SetHopeBuyItemID(DWORD dwID) { m_dwSelectItemID = dwID; }

	//设置元宝
	void SetYuanBao();
	void SetExchangeVolume();

	//兑换商品
	void ExchangeMallItem();

	//购买商品
	void BuyMallItem();
	void BuyMallItemPromote();

	//领取免费商品
	void FreeGetMallItem();

	//隐藏没有物品的普通商品控件
	void HideCtrlMallItem();
	void HideCtrlPromoteItem();
	void HideCtrlMallPack();

	//--处理游戏事件
	DWORD OnBuyMallItemInputEvent(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnBuyMallPromoteItemInputEvet(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnAffirmBuyMallItemEvent(tagMsgBoxEvent* pGameEvent);
	DWORD OnMsgMallGiftEvent(tagMsgMallGiftEvent* pGameEvent);
	DWORD OnBagYuanbaoChange(tagRoleYuanbaoChange* pGameEvent);
	DWORD OnExchangeVolumeChange(tagExchangeVolumeChange* pGameEvent);
	DWORD OnExchangePackItem(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnExchangeMallItem(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnAffirmExchangePackItem(tagMsgBoxEvent* pGameEvent);
	DWORD OnAffirmExchangeMallItem(tagMsgBoxEvent* pGameEvent);

	//--处理网络消息

	//--发送网络消息
	VOID OnSendBuyMallItem(const tagMallItemProto* pItem, INT16 n16Num); 
	VOID OnSendBuyMallPack(const tagMallPackProto* pPack, INT16 n16Num);

	//--处理网络消息
	DWORD OnNS_MallBuyItem(tagNS_MallBuyItem* pMsg, DWORD dwParam);
	DWORD OnNS_MallFreeGetItem(tagNS_MallFreeGetItem* pMsg, DWORD dwParam);
	DWORD OnNS_MallPresentItem(tagNS_MallPresentItem* pMsg, DWORD dwParam);
	DWORD OnNS_MallBuyPack(tagNS_MallBuyPack* pMsg, DWORD dwParam);
	DWORD OnNS_MallPresentPack(tagNS_MallPresentPack* pMsg, DWORD dwParam);
	DWORD OnNS_MallItemExchange(tagNS_MallItemExchange* pMsg, DWORD dwParam);
	DWORD OnNS_MallPackExchange(tagNS_MallPackExchange* pMsg, DWORD dwParam);

	//--显示物品Tips
	void ShowMallGoodsTips(int nGoodsPos, int nStatePos);
	void ShowMallPromoteGoodsTips(int nGoodsPos, int nStatePos);
	void ShowMallPackTips(int nGoodsPos, int nStatePos);

	//--各属性Tips
	void ShowPromoteProTips(const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos, EMamllItemTipsType eType=EMITT_All);
	void ShowGroupBuyProTips(const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos, EMamllItemTipsType eType=EMITT_All);
	void ShowPresentProTips(const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos, EMamllItemTipsType eType=EMITT_All);
	void ShowNumProTips(const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos, EMamllItemTipsType eType=EMITT_All);
	void ShowExchangeProTips(const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos, EMamllItemTipsType eType=EMITT_All);

	//--各属性判断
	bool JudgePromotePro(const tagMallItemProtoBase* pItem);
	bool JudgeGroupBuyPro(const tagMallItemProtoBase* pItem);
	bool JudgePresentPro(const tagMallItemProtoBase* pItem);
	bool JudgeNumPro(const tagMallItemProtoBase* pItem);
	bool JudgeExchangePro(const tagMallItemProtoBase* pItem);

	// 试穿
	void JudgeTryDress(DWORD dwTypeID);

	// 设置赠送最大个数
	void SetPresentMaxNum();

	void SetOperateState(EMallItemOperateState eType=EMIOS_NULL);	
	
	//直充
	void YuanBaoAbundance();

	//公告区
	void ShowMallNotice(BOOL bVal);

	//-------------------------------------------------------------------------------------------
	//元宝交易
	//-------------------------------------------------------------------------------------------

private:

	TSFPTrunk<MallFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<NetCmdMgr>			m_pCmdMgr;

	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndCaption;		
	PlayerTryDressWnd*			m_pWndTryDress;							// 试穿窗口
	GUIStatic*					m_pStcYuanBao;							// 元宝
	GUIStatic*					m_pExchangeVolume;						// 兑换卷
	GUIButton*					m_pBtnClose;							// 关闭
	GUIPushButton*				m_pPBtnYuanBaoTrade;					// 元宝交易
	GUIPushButton*				m_pPBtnGroupBuy;						// 帮派团购
	GUIPushButton*				m_pPBtnNotice;							// 商城公告
	GUIButton*					m_pBtnYuanBaoAbundance;					// 元宝充值
	GUIButton*					m_pBtnReset;							// 试穿重置

	GUIStatic*					m_pStcFetchBack;						// 免费领取背景
	IconStatic*					m_pStcFetchSel;							// 免费领取选择框
	ItemButton*					m_pFetchUnitItem;						// 免费领取物品
	GUIButton*					m_pBtnFetch;							// 免费领取按钮	
	GUIStatic*					m_pBtnFetchTitle;						// 免费领取物品名

	GUIStatic*					m_pStcGoodsBack;						// 商器区背景
	GUIStatic*					m_pStcGoodsBackEx;						// 普通商品
	GUIStatic*					m_pUnitItemBack[MALL_MAXITEM_PERPAGE];	// 商品区
	IconStatic*					m_pUnitItemSel[MALL_MAXITEM_PERPAGE];	// 商品选择框
	MallButton*					m_pUnitItem[MALL_MAXITEM_PERPAGE];		// 商品
	GUIStatic*					m_pStcItemName[MALL_MAXITEM_PERPAGE];	// 商品名字
	GUIStatic*					m_pStcInfo[MALL_MAXITEM_PERPAGE];		// 商品信息
	IconStatic*					m_pStcState[MALL_MAXITEM_PERPAGE][MALL_ITEM_STATE];// 商品状态
	GUIButton*					m_pBtnPDItem;							// 下一页(普通商品)
	GUIButton*					m_pBtnPUItem;							// 上一页(普通商品)
	GUIStatic*					m_pStcCPItem;							// 当前页提示(普通商品)
	GUIButton*					m_pBtnPDPack;							// 下一页(礼包商品)
	GUIButton*					m_pBtnPUPack;							// 上一页(礼包商品)
	GUIStatic*					m_pBtnCPPack;							// 当前页提示(礼包商品)

	GUIButton*					m_pBtnBuy;								// 购买
	GUIButton*					m_pBtnPresent;							// 赠送
	GUIButton*					m_pBtnGroupBuy;							// 团购
	GUIButton*					m_pBtnExchange;							// 兑换

	GUIStatic*					m_pStcProBack;							// 推荐商品背景
	GUIStatic*					m_pProItemBack[MALL_MAX_PROMOTE_ITEM];
	IconStatic*					m_pUnitProItemSel[MALL_MAX_PROMOTE_ITEM];
	MallButton*					m_pUnitProItem[MALL_MAX_PROMOTE_ITEM];
	IconStatic*					m_pProItemState[MALL_MAX_PROMOTE_ITEM][MALL_ITEM_STATE];
	GUIStatic*					m_pProItemName[MALL_MAX_PROMOTE_ITEM];
	GUIStatic*					m_pProItmeInfo[MALL_MAX_PROMOTE_ITEM];

	GUIStatic*					m_pStcPackBack;							//礼包背景
	IconStatic*					m_pUnitPackBack[MALL_MAXPACK_PERPAGE];	//每个礼包
	IconStatic*					m_pUnitPackSel[MALL_MAXPACK_PERPAGE][MALL_PACKPRO_NUM];	//礼包选择框
	MallButton*					m_pUnitPack[MALL_MAXPACK_PERPAGE][MALL_PACKPRO_NUM];	//礼包里的商品
	IconStatic*					m_pStcPackName[MALL_MAXPACK_PERPAGE];	// 礼包名字
	IconStatic*					m_pStcPackInfo[MALL_MAXPACK_PERPAGE];	// 礼包信息
	IconStatic*					m_pStcPackState[MALL_MAXPACK_PERPAGE][MALL_ITEM_STATE];// 礼包状态
	GUIStatic*					m_pStcPackNew[MALL_MAXPACK_PERPAGE];	// 新礼包新提示
	GUIStatic*					m_pStcPackHot[MALL_MAXPACK_PERPAGE];	// 新礼包热提示



	GUIPushButton*				m_pPBtnGoods;							// 商城商品分类标签
	GUIPushButton*				m_pPBtnAll;								// 全部商品按钮
	GUIPushButton*				m_pPBtnPackage;							// 打包商品按钮
	GUIPushButton*				m_pPBtnSubMall[MALL_ITEM_SUBTYPE];		// 商品分类按钮

	GUIPushButton*				m_pPBtnMall;							// 刷新商品区按钮

	GUIStatic*					m_pWndNotice;							// 公告
	HttpPicEx*					m_pHttp_MallNotice;						


	TMap<DWORD, tagMallItemProto*>	m_mapMallItem[MALL_ITEM_SUBTYPE];	// 商品分类列表

	INT							m_nCurMenu;								// 当前商品分类货架:0-7 全部:-2 打包:-1
	INT							m_nCurPage[MALL_ITEM_SUBTYPE];			// 当前页
	INT							m_nCurMaxPage[MALL_ITEM_SUBTYPE];		// 当前货架最大页

	INT							m_nSpecialPage[SPECIAL_TYPE];			// 全部:0 打包:1
	INT							m_nSpecialMaxPage[SPECIAL_TYPE];

	DWORD						m_dwSelectItemID;						// 要购买的商品ID
	INT16						m_n16BuyNum;							// 购买的数量
	vector<tagMsgMallGiftEvent>	m_vecGiftEvent;							// 缓存赠送
	bool						m_bSelectPromote;
	EMallItemOperateState		m_eOperateState;
	tstring						m_strMallNotice;						// 缓存的商城公告

	//-------------------------------------------------------------------------------------------
	//元宝交易
	//-------------------------------------------------------------------------------------------
    GUIStatic*					m_pStcYuanbaoBack;						// 元宝交易背景

	//-------------------------------------------------------------------------------------------
	//团购
	//-------------------------------------------------------------------------------------------
	GUIStatic*					m_pStcGroupBuyBack;						// 团购背景
};