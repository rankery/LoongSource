#pragma once
#include "..\WorldDefine\shop_define.h"

class ItemButton;
class NetSession;
class Item;
class Equipment;
struct tagMsgInputBoxEvent;
struct tagMsgBoxEvent;
struct tagBuyItemEvent;
struct tagItemPosChangeExEvent;
struct tagNS_FeedbackFromShop;
struct tagNS_GetShopItems;
struct tagNS_GetShopEquips;
struct tagNS_BuyShopItem;
struct tagNS_BuyShopEquip;

struct tagShopEquipClient
{
	INT16 n16Index;		// 服务器中物品下标
	INT16 n16RemainNum;	// 该值为GT_INVALID时，表示该商品不限量
	Equipment* pEquip;
	const tagShopRareItemProto	*pRareProto;
};
struct tagShopItemClient
{
	INT16 n16Index;		// 服务器中物品下标
	INT16 n16RemainNum;	// 该值为GT_INVALID时，表示该商品不限量
	DWORD dwTypeID;
	const tagShopRareItemProto	*pRareProto;
	const tagItemProto			*pItemProto;
};

/** \class ShopShelfFrame
	\brief 普通商铺货架界面
*/
class ShopShelfFrame :
	public GameFrame
{
public:
	ShopShelfFrame(void);
	~ShopShelfFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

	virtual BOOL EscCancel();

    void ShowWnd(BOOL bVisable);
    bool IsOpen();

private:
	// 处理GUI消息
	DWORD EventHandler(tagGUIEvent* pEvent);
	// 显示当前菜单
	VOID ShowCurMenu(INT nMenu);
	// 显示当前页
	VOID ShowCurPage(INT nPage);

	// 处理游戏事件
	DWORD OnEventCheckSellItem(tagMsgBoxEvent* pGameEvent);
	DWORD OnEventClickGoodsInput(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventDragGoodsInput(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventBuyGoods(tagBuyItemEvent* pGameEvent);
	DWORD OnEventItemPosChange(tagItemPosChangeExEvent* pGameEvent);

	// 处理网络消息
	DWORD OnNetFeedbackFromShop(tagNS_FeedbackFromShop* pNetCmd, DWORD);
	DWORD OnNetGetShopItems(tagNS_GetShopItems* pNetCmd, DWORD);
	DWORD OnNetGetShopEquips(tagNS_GetShopEquips* pNetCmd, DWORD);
	DWORD OnNetBuyShopItem(tagNS_BuyShopItem* pNetCmd, DWORD);
	DWORD OnNetBuyShopEquip(tagNS_BuyShopEquip* pNetCmd, DWORD);

	// 显示商店错误码
	VOID ShowShopErrorMsg(DWORD dwErrorCode);

	// 设置当前页码
	VOID SetCurPage();
	// 设置店铺招牌
	VOID SetSignboard(LPCTSTR szSignboard);
	// 设置店铺货架
	VOID SetShelfName(INT nIndex, LPCTSTR szShelfName);

	// 翻页，TRUE为向下，FALSE为向上
	VOID PageDown(bool bFlag = TRUE);

	// 开启商店界面
	BOOL StartShopShelf();

	// 设置商店货架不同商品列表
	VOID SetShelfNormalList(const tagShopProtoClient* pShop);
	// 尝试点击货架中的商品
	VOID TryToClickItem(INT nPos, bool bShift=FALSE);
	// 尝试拖动货架中的商品
	VOID TryToDragItem(INT nPos, bool bShift=FALSE);
	// 拖动货架中的商品
	VOID OnDragItem(INT nPos, INT nKey, DWORD dwTypeID, INT16 n16Num);
	// 买商品
	VOID OnBuyShopItem(INT nKey, DWORD dwTypeID, INT16 n16Num);
	// 卖东西
	VOID OnSellItemToShop(INT16 n16Pos, DWORD dwTypeID);
	// 发送卖东西消息
	VOID SendSellToShop(INT64 nItemSerial);
	// 显示Tip
	VOID ShowGoodsTip(INT nPos);
	// 发送特殊商品列表请求
	VOID SendShelfSpecList(BYTE byMemu);

	// 获取当前商品TypeID,附带最大购买数
	DWORD GetGoodsTypeID(INT nKey, INT& nMaxCanBuy);
	DWORD GetGoodsTypeID(INT nKey);
	// 获取当前商品是否可以购买,并返回该商品64位id
	BOOL IsGoodsCanBuy(INT nKey, INT16 n16Num, INT64 &n64Serial);
	// 获取当前商品可购买最大数
	INT GetGoodsMaxCanBuy(INT32	nSilver, INT32 nCostNum, BYTE byLevel);

	// 清空数据
	VOID FreeMap();

	VOID DestroyMsgBox();

private:

	TSFPTrunk<ShopShelfFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<NetCmdMgr>			m_pCmdMgr;

	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndCaption;
	GUIPushButton*				m_pPbnMenu[MAX_SHOP_SHELF_NUM];		// 商店货架，显示不同种类的商品
	GUIButton*					m_pBtnExit;							// 退出按钮
	GUIButton*					m_pBtnClose;						// 关闭按钮
	GUIButton*					m_pBtnPageDown;						// 向下翻页 
	GUIButton*					m_pBtnPageUp;						// 向上翻页
	GUIStatic*					m_pStcPage;							// 当前页
	GUIStatic*					m_pStcTitle;						// 商店名称

	ItemButton*					m_pUnitItem[MAX_SHOP_ONE_SHELF];	// 单元格
	
	INT							m_nCurMenu;							// 当前货架
	INT							m_nCurPage[MAX_SHOP_SHELF_NUM];		// 当前页
	INT							m_nCurMaxPage[MAX_SHOP_SHELF_NUM];	// 当前货架最大页
	INT							m_nFirstBlank;						// 当前页第一个空位
			
	DWORD						m_dwShopNpcID;						// 商店NpcID
	Item*						m_pSellItem;						// 售出物品缓存
	INT							m_nClickKey;						// 点击数量输入位置缓存
	INT							m_nDragPos;							// 拖动数量输入位置缓存
	const tagShopProtoClient*	m_pShop;							// 商店静态模型


	TMap<INT, const tagShopItemProto*>		m_mapNormalGoods[MAX_SHOP_SHELF_NUM];	// 普通商品列表
	TMap<INT, tagShopItemClient*>			m_mapSpecItem[MAX_SHOP_SHELF_NUM];		// 特殊物品列表
	TMap<INT, tagShopEquipClient*>			m_mapSpecEquip[MAX_SHOP_SHELF_NUM];		// 特殊装备列表

};
