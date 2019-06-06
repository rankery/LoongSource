#pragma once
#include "..\WorldDefine\ItemDefine.h"
#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\msg_item.h"
#include "..\WorldDefine\container_define.h"
#include "..\WorldDefine\msg_guild.h"
class ItemContainer;
class Item;
class Equipment;
class NetSession;
struct tagNS_BagExtend;
struct tagNS_WareExtend;
struct tagNS_SuitNum;
struct tagNS_Abrase;
struct tagItemContainerExtend;

/** \struct tagBeGoldStoneItemTable
	\breif 点金石物品表
*/
struct tagBeGoldStoneItemTable
{
	DWORD dwSrcItemID;
	DWORD dwDestItemID;
	DWORD dwGoldStoneType;
};

/** \struct tagBaiBaoRecordC
	\breif	百宝袋单条记录客户端存储结构
*/
struct tagBaiBaoRecordC
{
	EBaiBaoRecordType	eType;
	DWORD				dwTypeID;
	DWORD				dwRoleID;
	tagDWORDTime		sTime;
	tstring				strWords;
};

/**	\class ItemMgr
	\brief 物品管理器

*/
class ItemMgr
{
public:
	enum EPocketEncryptState// 行囊密码状态
	{
		EPES_Null			=	-1,		
		EPES_EnterPassword	=	0,		// 键入行囊密码
		EPES_SetupPassword	=	1,		// 设置行囊密码
		EPES_ModifyPassword	=	2,		// 修改行囊密码
		EPES_CancelPassword	=	3		// 取消行囊密码
	};
public:
	ItemMgr();
	~ItemMgr();

	static ItemMgr* Inst();


	VOID Init();
	VOID Update();
	VOID Destroy();
		
	// 获取各个容器指针
	ItemContainer* GetPocket(){return m_pPocket;}
	ItemContainer* GetThrone(){return m_pThrone;}
	ItemContainer* GetEquipment(){return m_pEquipment;}
	ItemContainer* GetStorge(){return m_pStorge;}
	ItemContainer* GetQuest(){return m_pQuest;}
    ItemContainer* GetGuildStore(){return m_pGuildStore;}
	
	// 根据模板ID获取物品的图标
	tstring GetItemIconPath(DWORD dwTypeID);

	// 获取背包中的物品
	Item* GetPocketItem(INT16 n16Pos);
	// 获得背包中的物品
	Item* GetPocketItemByID(INT64 n64ID);
	// 获取装备栏中的装备
	Equipment* GetCurEquip(EEquipPos ePos);
	// 获取装备栏中的装备
	Equipment* GetCurEquipByID(INT64 n64ID);
	// 获取相应容器中的物品
	Item* GetConItem(EItemConType ePos, INT16 n16Pos);
	// 获取相应容器中的物品
	Item* GetConItemByID(EItemConType ePos, INT64 n64ID);
	// 判断装备栏是否为空
	BOOL IsNotEquip(EEquipPos ePos);
	// 获取相应容器相应页第一个空位
	INT16 GetConBlank(EItemConType ePos, INT nIndex = 0);

	// 获取某套装当前件数
	INT8 GetSuitNum(DWORD dwSuitID){return m_mapSuitNum.Peek(dwSuitID);}
	// 获取某件装备龙魂能力是否激活
	bool GetLonghun(EEquipPos ePos, INT64 id);

	// 获取冷却时间
	VOID GetItemCurCDTime(DWORD dwTypeID, FLOAT& fCDCount, FLOAT& fRestoreRatio);

	// 获取相应特殊功用类型的物品ID
	INT64 GetSpecfuncItemID(EItemSpecFunc eType);
	// 获取当前主手武器类型
	EItemTypeEx GetHandWeaponType(EEquipPos ePos=EEP_RightHand);
	
	// 显示物品相关错误码
	VOID ShowItemErrorMsg(DWORD dwErrorCode);

	// 获取百宝袋记录列表
	VOID GetBaiBaoList(TList<tagBaiBaoRecordC> &listRecord);
	
	// 设置当前行囊加密的状态，方便客户端处理服务器发送的消息NS_BagPsd
	void SetPocketEncryptState(EPocketEncryptState eState){m_eEncryptState = eState;}

    // 获取帮派仓库
    void OnGetGuildWare(tagNS_GetGuildWare* pMsg);
    // 清空帮派仓库
    void ClearGuildWare();

	//brief 从背包中得到第一个特殊功能的物品
	Item* GetFirstSpecFuncItemFromPocket( EItemSpecFunc eItemSpecFunc );

	// 材料是否在点金石物品表中
	bool IsInGoldStoneTable( DWORD dwItemId, DWORD dwGoldStoneType );

	/** \brief 从背包中得到第一个特殊功能的物品
		\param DWORD dwItemTypeID 可能不同的物品有 同样的特殊功能，比如普通鱼饵和高级鱼饵
		\param bEnableType 开不开启第二个参数，默认是不开启的
	*/
	Item* GetFirstSpecFuncItemFromPocket( EItemSpecFunc eItemSpecFunc, DWORD dwItemTypeID, BOOL bEnableType = FALSE );

protected:
	//--网络消息处理函数
	// 初始化物品
	DWORD NetInitLocalPlayerItem(tagNS_GetRoleInitState_Item* pNetCmd, DWORD);
	// 初始化物品冷却
	DWORD NetInitLocalPlayerItemCD(tagNS_GetRoleInitState_ItemCDTime* pNetCmd, DWORD);
	// 初始化套装
	DWORD NetInitLocalPlayerSuit(tagNS_GetRoleInitState_Suit* pNetCmd, DWORD);
	// 初始化龙魂能力
	DWORD NetInitLocalPlayerLonghun(tagNS_GetRoleInitState_Longhun* pNetCmd, DWORD);
	// 穿装备返回
	DWORD NetEquipment(tagNS_Equip* pNetCmd, DWORD);
	// 脱装备返回
	DWORD NetUnEquipment(tagNS_Unequip* pNetCmd, DWORD);
	// 换武器返回
	DWORD NetSwapWeapon(tagNS_SwapWeapon* pNetCmd, DWORD);
	// 物品位置改变，同一容器内
	BOOL NetItemPosChange(tagNS_ItemPosChange* pNetCmd, DWORD);
	// 物品位置改变，不同容器之间
	BOOL NetItemPosChangeEx(tagNS_ItemPosChangeEx* pNetCmd, DWORD);
	// 新增一个物品
	DWORD NetNewItemAdd(tagNS_NewItemAdd* pNetCmd, DWORD);
	// 新增一个装备
	DWORD NetNewEquipAdd(tagNS_NewEquipAdd* pNetCmd, DWORD);
	// 增加一个已知物品
	DWORD NetItemAdd(tagNS_ItemAdd* pNetCmd, DWORD);
	// 删除一个物品
	DWORD NetItemRemove(tagNS_ItemRemove* pNetCmd, DWORD);
	// 绑定物品
	DWORD NetItemBind(tagNS_ItemBind* pNetCmd, DWORD);
	// 装备属性改变
	DWORD NetEquipChange(tagNS_EquipChange* pNetCmd, DWORD);
	// 物品CD改变
	DWORD NetItemCDUpdate(tagNS_ItemCDUpdate* pNetCmd, DWORD);
	// 背包扩容
	DWORD NetBagExtend(tagNS_BagExtend* pNetCmd, DWORD);
	// 仓库扩容
	DWORD NetWareExtend(tagNS_WareExtend* pNetCmd, DWORD);
	// 套装件数
	DWORD NetSuitNumChange(tagNS_SuitNum* pNetCmd, DWORD);
	// 鉴定装备
	DWORD NetIdentifyEquip(tagNS_IdentifyEquip* pNetCmd, DWORD);
	// 磨石打磨
	DWORD NetAbraseWeapon(tagNS_Abrase* pNetCmd, DWORD);
    // 时装染色
    DWORD NetDyeFashion(tagNS_DyeFashion* pNetCmd, DWORD);
	// 行囊加密返回
	DWORD NetBagPassword(tagNS_BagPsd* pNetCmd, DWORD);
	// 武器崭新度变化
	DWORD NetWeaponNewnessChange(tagNS_RWNewessChange* pNetCmd, DWORD);
	// 处理自动整理
	DWORD NetContainerReorder(tagNS_ItemReorder* pNetCmd, DWORD);
	DWORD NetContainerReorderEx(tagNS_ItemReorderEx* pNetCmd, DWORD);
	// 处理装备龙魂能力出发机制
	DWORD NetEquipLonghunOn(tagNS_LongHunOn* pNetCmd, DWORD);
	DWORD NetEquipLonghunOff(tagNS_LongHunOff* pNetCmd, DWORD);
	// 处理百宝袋记录功能
	DWORD NetInitBaiBaoRecord(tagNS_InitBaiBaoRecord* pNetCmd, DWORD);
	DWORD NetSingleBaiBaoRecord(tagNS_SingleBaiBaoRecord* pNetCmd, DWORD);

	//响应打开仓库
	DWORD NetWareOpen( tagNS_WareOpen* pNetCmd, DWORD );

	DWORD NetBeGoldStone( tagNS_BeGoldStone *pNetCmd, DWORD );

	//--处理游戏事件
	// 响应背包扩容
	DWORD OnEventBagExtend(tagItemContainerExtend* pGameEvent);
	// 响应仓库扩容
	DWORD OnEventWareExtend(tagItemContainerExtend* pGameEvent);
	// 确定背包扩容
	DWORD OnEventBagExtendCheck(tagMsgBoxEvent* pGameEvent);
	// 确认仓库扩容
	DWORD OnEventWareExtendCheck(tagMsgBoxEvent* pGameEvent);

	
	// 获取相应容器类型的指针
	ItemContainer* GetContainer(EItemConType eType);
	
	// 新增一个物品;
	VOID AddNewItem(Item* pItem);

	// 新增一条百宝袋记录
	VOID AddNewBaiBaoRecord(tagBaiBaoRecord* pRecord);

private:
	void LoadBeGoldStoneItemTable();

private:
	TSFPTrunk<ItemMgr>		m_Trunk;
	TObjRef<Util>			m_pUtil;
	TObjRef<NetCmdMgr>		m_pCmdMgr;
	TObjRef<NetSession>		m_pSession;
	TObjRef<GameFrameMgr>	m_pFrameMgr;
	TObjRef<VarContainer>	m_pVarContainer;

	ItemContainer*			m_pPocket;		// 背包
	ItemContainer*			m_pStorge;		// 仓库
	ItemContainer*			m_pEquipment;	// 装备栏
	ItemContainer*			m_pThrone;		// 百宝袋
	ItemContainer*			m_pQuest;		// 任务物品栏
    ItemContainer*          m_pGuildStore;  // 帮派仓库

	TMap<DWORD, DWORD>		m_mapCurStartTime;		// 冷却时间开始表
	TMap<DWORD, INT8>		m_mapSuitNum;			// 套装件数
	TMap<INT16, INT8>		m_mapLonghunOn;			// 龙魂激活与否
	bool					m_bUseSilverExtend;		// 使用金钱扩充标志位
	bool					m_bNotEnoughYuanbao;	// 元宝不足标志位
	bool					m_bNotEnoughSilver;		// 金钱不足标志位
	bool					m_bNotInitBaiBaoRecord;	// 百宝袋更新标志位
	TList<tagBaiBaoRecordC>	m_listBaiBaoRecord;		// 百宝袋记录列表
	vector<tagBeGoldStoneItemTable> m_vecBeGoldStoneItemTable;	// 点金石物品表

	EPocketEncryptState		m_eEncryptState;		// 行囊密码当前接收状态
};
