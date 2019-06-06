#pragma once

/** \
	\
	\
	\
*/
enum EItemStrategy
{
	EIST_ItemPocket = 0,	// 背包
	EIST_ItemStorage,		// 仓库
	EIST_ItemPlayerTrade,	// 玩家交易
	EIST_ItemShopVend,		// 商店售卖
	EIST_ItemStall,			// 摆摊
	EIST_ItemSmithCompose,	// 合成
	EIST_ItemInsurance,		// 保底符
    EIST_GuildStorage,      // 帮派仓库
	EIST_GuildUpgrade,      // 帮派升级

	// 注：每加入一个规则需要在函数ItemFrame::SetCurStrategy()中加入相应的关闭界面消息
	//--todo: more
	EIST_ItemEnd = 32		
};

class PocketFrame;
class ThroneFrame;
class StorageFrame;
class ItemProduceFrame;
class ShopShelfFrame;
class PlayerTradeFrame;
class Item;
class NetSession;
struct tagClickItemEvent;
struct tagOnTheRoleOfItemEvent;
struct tagPutDownItemEvent;
struct tagStartNPCTalkEvent;
/** class   ItemFrame
	breif   物品界面框架类
	remarks 管理物品相关界面之间的操作
*/
class ItemFrame :
	public GameFrame
{
	friend class FishMgr;
public:
	ItemFrame(void);
	~ItemFrame(void);
	
	// GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();

	// 获取当前规则
	EItemStrategy GetCurStrategy(){return m_eCurStrategy;}
	// 设置当前规则
	VOID SetCurStrategy(EItemStrategy eType);

private:
	// 处理游戏事件
	DWORD OnOpenPackage(tagGameEvent* pGameEvent);
	DWORD OnClosePackage(tagGameEvent* pGameEvent);
	DWORD OnOpenWare(tagGameEvent* pGameEvent);
	DWORD OnCloseWare(tagGameEvent* pGameEvent);
	DWORD OnCloseWareNow(tagGameEvent* pGameEvent);
	DWORD OnNpcTalk(tagStartNPCTalkEvent* pGameEvent);

	DWORD OnTryToUseItem(tagClickItemEvent* pGameEvent);
	DWORD OnTheRoleOfItem(tagOnTheRoleOfItemEvent* pGameEvent);
	DWORD OnRolePutDownItem(tagPutDownItemEvent* pGameEvent);
	DWORD OnPutDownItemCheck(tagMsgBoxEvent* pGameEvent);
	DWORD OnUseSpecItemCheck(tagMsgBoxEvent* pGameEvent);
	DWORD OnAddPotValue(tagMsgBoxEvent* pGameEvent);

	VOID SendPutDownItem(INT64 n64ItemSerial);

	VOID OnUseItem(Item* pItem);
	VOID OnUseSpecItem(Item* pItem);

	VOID ShowUseItemCheckDlg(Item* pSrcItem, Item* pDstItem = NULL);

private:

	TSFPTrunk<ItemFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>		m_pSession;

	PocketFrame*				m_pPocket;		// 行囊界面
	
	EItemStrategy				m_eCurStrategy;	// 当前策略
	DWORD						m_dwNpcGlobalID;// 当前NPCID
	DWORD						m_dwNpcTypeID;	// 当前NPCTypeID

	Item*						m_pOperSrcItem;	// 正在操作的物品的缓存
    INT64                       m_n64OperSrcItem;   // 正在操作的物品在背包中的ID
	Item*						m_pOperDstItem;	// 操作于某个物品的缓存
};

/** \function SetStrategy
	\blief	  设置物品右键规则
	\param	  eType为需要设置的规则
	\param	  eComType为需要比较的规则，EIST_ItemEnd为不比较，否则需要设置前的规则与比较规则相同才设置
*/
inline void SetStrategy(EItemStrategy eType, EItemStrategy eComType = EIST_ItemEnd)
{
	ItemFrame* pFrame = (ItemFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Item"));
	if(P_VALID(pFrame))
	{
		if(eComType == EIST_ItemEnd || eComType == pFrame->GetCurStrategy())
			pFrame->SetCurStrategy(eType);
	}
}
