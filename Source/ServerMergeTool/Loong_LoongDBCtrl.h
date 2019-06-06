#pragma once
#include "Singleton.h"
#include "Loong_CommonDef.h"

class CLoongLoongDBCtrl: public Singleton<CLoongLoongDBCtrl>
{
	typedef VOID (CLoongLoongDBCtrl::*Fun_pFormat)(Beton::MyStream*,EDBType);
	typedef BOOL (CLoongLoongDBCtrl::*Fun_pFormat3)(Beton::MyStream*,LPVOID&,EDBType);
	typedef BOOL (CLoongLoongDBCtrl::*Fun_pFormat4)(Beton::MyStream*,LPVOID&,LPCSTR,EDBType);
	
	//拼accountid形式的
	typedef VOID (CLoongLoongDBCtrl::*Fun_pAccountFormat)(Beton::MyStream*,DWORD,EDBType);

	//拼分页查询的
	typedef VOID (CLoongLoongDBCtrl::*Fun_pPagingFormat)(Beton::MyStream*,INT64,EDBType);



	typedef VOID (CLoongLoongDBCtrl::*Fun_pProcRes)(LPVOID&, Beton::QueryResult*);
private:
	//模版方法
	// 读取数据库信息(有结果集)
	BOOL Load(OUT LPVOID pData,EDBType eDBType,
		Fun_pFormat pFunFormat, Fun_pProcRes pFunProcRes);

	//按用户ID加载数据
	BOOL AccountLoad(OUT LPVOID pData,DWORD dwAccountID,EDBType eDBType,
		Fun_pAccountFormat pFunFormat, Fun_pProcRes pFunProcRes);

	//分页load
	EPagingState PagingLoad(OUT LPVOID pData,INT64 &n64Limit,EDBType eDBType,
		Fun_pPagingFormat pFunFormat, Fun_pProcRes pFunProcRes);


	// 插入数据库操作模版方法
	BOOL Insert( LPVOID pData, EDBType eDBType,Fun_pFormat3 pFunFormat);
	BOOL Insert( LPVOID pData, EDBType eDBType,LPCSTR pszTable,Fun_pFormat4 pFunFormat);

public:
	CLoongLoongDBCtrl(void);
	~CLoongLoongDBCtrl(void){ Destroy(); }


	//-------------------------------------------------------------------------
	// 初始化和销毁
	//-------------------------------------------------------------------------
	BOOL	Init(tagConfigDetail stConfig[EM_DB_TYPE_END]);
	VOID	Destroy();

private:

	DataBase		m_LoongDB[EM_DB_TYPE_END];	// Beton数据库

public:
	//得到roledata表最大序列号
	DWORD GetRoleDataMaxNum(EDBType eDBType);

	DWORD GetPetDataMaxNum(EDBType eDBType);

	//划拨物品最小序号
	INT64 GetItemMinSerialBill(EDBType eDBType);

	//内部物品最大序号
	INT64 GetMaxSerialInternal(EDBType eDBType);

	//外部物品最小序号
	INT64 GetMinSerialOutter(EDBType eDBType);

	//创建补偿物品
	BOOL CreateItem(INT64 n64SerialNum, DWORD ItemTypdID, DWORD AccountID, DWORD RoleID);

private:
	BOOL GetMinItemSerialInTable(LPCSTR szTable, INT64& n64ItemSerial,EDBType eDBType, INT64 n64LowLimit = GT_INVALID);
	BOOL GetMaxItemSerialInTable(LPCSTR szTable, INT64& n64ItemSerial,EDBType eDBType,INT64 n64UpLimit = GT_INVALID);

public:
	//Item 按account读取
	BOOL Loong_AccountQuery(std::vector <tagTWrap_Loong_Item*>&,DWORD dwAccountID,EDBType eDBType);

public:
	//equip paging query
	EPagingState Loong_PagingQuery(std::vector <tagTWrap_Loong_Equip*>&,INT64,EDBType eDBType);
	EPagingState Loong_PagingQuery(std::vector <tagTWrap_Loong_EquipDel*>&,INT64,EDBType eDBType);
	EPagingState Loong_PagingQuery(std::vector <tagTWrap_Loong_TaskDone*>&,INT64,EDBType eDBType);
	EPagingState Loong_PagingQuery(std::vector <tagTWrap_Loong_Task*>&,INT64,EDBType eDBType);
	EPagingState Loong_PagingQuery(std::vector <tagTWrap_Loong_ItemDel*>&,INT64,EDBType eDBType);

public:
/************************ 读取相关 **************************/
	BOOL Loong_Query(std::vector <tagTWrap_Loong_AccountCommon*>&,EDBType eDBType);

	BOOL Loong_Query(std::vector <tagTWrap_Loong_BlackList*>&,EDBType eDBType);

	BOOL Loong_Query(std::vector <tagTWrap_Loong_Buff*>&,EDBType eDBType);

	BOOL Loong_Query(std::vector <tagTWrap_Loong_ClanData*>&,EDBType eDBType);

	BOOL Loong_Query(std::vector <tagTWrap_Loong_Enemy*>&,EDBType eDBType);

//	BOOL Loong_Query(std::vector <tagTWrap_Loong_Equip*>&,EDBType eDBType);

	BOOL Loong_Query(std::vector <tagTWrap_Loong_EquipBaiBao*>&,EDBType eDBType);

//	BOOL Loong_Query(std::vector <tagTWrap_Loong_EquipDel*>&,EDBType eDBType);

	//氏族珍宝激活表
	BOOL Loong_Query(std::vector <tagTWrap_Loong_ActClanTreasure*>&,EDBType eDBType);

	//氏族进入快照
	BOOL Loong_Query(std::vector <tagTWrap_Loong_FameHallEnterSnap*>&,EDBType eDBType);

	//氏族声望重置时间快照
	BOOL Loong_Query(std::vector <tagTWrap_Loong_RepRstTime*>&,EDBType eDBType);

	//好友列表(单向)
	BOOL Loong_Query(std::vector <tagTWrap_Loong_Friend*>&,EDBType eDBType);

	//友好度
	BOOL Loong_Query(std::vector <tagTWrap_Loong_FriendshipSave*>&,EDBType eDBType);

	//团购
	BOOL Loong_Query(std::vector <tagTWrap_Loong_GroupPurchase*>&,EDBType eDBType);

	//帮派
	BOOL Loong_Query(std::vector <tagTWrap_Loong_Guild*>&,EDBType eDBType);

	//帮派成员
	BOOL Loong_Query(std::vector <tagTWrap_Loong_GuildMem*>&,EDBType eDBType);

	//Item
	BOOL Loong_Query(std::vector <tagTWrap_Loong_Item*>&,EDBType eDBType);

	//Item baibao
	BOOL Loong_Query(std::vector <tagTWrap_Loong_ItemBaiBao*>&,EDBType eDBType);

	//Item del
	BOOL Loong_Query(std::vector <tagTWrap_Loong_ItemDel*>&,EDBType eDBType);

	//item cd time
	BOOL Loong_Query(std::vector <tagTWrap_Loong_ItemCDTime*>&,EDBType eDBType);

	//item_needlog
	BOOL Loong_Query(std::vector <tagTWrap_Loong_ItemNeedLog*>&,EDBType eDBType);

	//log baibao
	BOOL Loong_Query(std::vector <tagTWrap_Loong_LogBaiBao*>&,EDBType eDBType);

	//role del
	BOOL Loong_Query(std::vector <tagTWrap_Loong_RoleDel*>&,EDBType eDBType);

	//roledata
	BOOL Loong_Query(std::vector <tagTWrap_Loong_RoleData*>&,EDBType eDBType);

	//roledata del
	BOOL Loong_Query(std::vector <tagTWrap_Loong_RoleDataDel*>&,EDBType eDBType);

	//skill
	BOOL Loong_Query(std::vector <tagTWrap_Loong_Skill*>&,EDBType eDBType);

	//task
	BOOL Loong_Query(std::vector <tagTWrap_Loong_Task*>&,EDBType eDBType);

	//task done
	BOOL Loong_Query(std::vector <tagTWrap_Loong_TaskDone*>&,EDBType eDBType);

	//title
	BOOL Loong_Query(std::vector <tagTWrap_Loong_Title*>&,EDBType eDBType);

	//名帖
	BOOL Loong_Query(std::vector <tagTWrap_Loong_VisitingCard*>&,EDBType eDBType);

	//元宝
	BOOL Loong_Query(std::vector <tagTWrap_Loong_YBAccount*>&,EDBType eDBType);

	//元宝交易订单
	BOOL Loong_Query(std::vector <tagTWrap_Loong_YuanBaoOrder*>&,EDBType eDBType);

	//bill_item
	BOOL Loong_Query(std::vector <tagTWrap_Loong_BillItem*>&,EDBType eDBType);
	//bill_yuanbao
	BOOL Loong_Query(std::vector <tagTWrap_Loong_BillYuanBao*>&,EDBType eDBType);
	//bill_yuanbao_log
	BOOL Loong_Query(std::vector <tagTWrap_Loong_BillYuanBaoLog*>&,EDBType eDBType);
	//pet_data
	BOOL Loong_Query(std::vector <tagTWrap_Loong_PetData*>&,EDBType eDBType);
	//pet skill
	BOOL Loong_Query(std::vector <tagTWrap_Loong_PetSkill*>&,EDBType eDBType);
	//vip_stall
	BOOL Loong_Query(std::vector <tagTWrap_Loong_VipStall*>&,EDBType eDBType);
	//guild_commodity
	BOOL Loong_Query(std::vector <tagTWrap_Loong_GuildCommodity*>&,EDBType eDBType);
	//guild_skill
	BOOL Loong_Query(std::vector <tagTWrap_Loong_GuildSkill*>&,EDBType eDBType);
	//vip_netbar
	BOOL Loong_Query(std::vector <tagTWrap_Loong_VipNetBar*>&,EDBType eDBType);
	//guild_upgrade
	BOOL Loong_Query(std::vector <tagTWrap_Loong_GuildFacilities*>&,EDBType eDBType);
	//commerce_rank
	BOOL Loong_Query(std::vector <tagTWrap_Loong_CommerceRank*>&,EDBType eDBType);
	//activity
	BOOL Loong_Query(std::vector <tagTWrap_Loong_ActivityData*>&,EDBType eDBType);
	//left msg
	BOOL Loong_Query(std::vector <tagTWrap_Loong_LeftMsg*>&,EDBType eDBType);
/************************ 写入相关 **************************/
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_AccountCommon*>&,EDBType eDBType);

	BOOL Loong_Insert(std::vector <tagTWrap_Loong_BlackList*>&,EDBType eDBType);

	BOOL Loong_Insert(std::vector <tagTWrap_Loong_Buff*>&,EDBType eDBType);

	BOOL Loong_Insert(std::vector <tagTWrap_Loong_ClanData*>&,EDBType eDBType);

	BOOL Loong_Insert(std::vector <tagTWrap_Loong_Enemy*>&,EDBType eDBType);

	BOOL Loong_Insert(std::vector <tagTWrap_Loong_Equip*>&,EDBType eDBType);

	BOOL Loong_Insert(std::vector <tagTWrap_Loong_EquipBaiBao*>&,EDBType eDBType);

	BOOL Loong_Insert(std::vector <tagTWrap_Loong_EquipDel*>&,EDBType eDBType);

	//氏族珍宝激活表
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_ActClanTreasure*>&,EDBType eDBType);

	//氏族进入快照
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_FameHallEnterSnap*>&,EDBType eDBType);

	//氏族声望重置时间快照
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_RepRstTime*>&,EDBType eDBType);

	//好友列表(单向)
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_Friend*>&,EDBType eDBType);

	//友好度
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_FriendshipSave*>&,EDBType eDBType);

	//团购
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_GroupPurchase*>&,EDBType eDBType);

	//帮派
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_Guild*>&,EDBType eDBType);

	//帮派成员
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_GuildMem*>&,EDBType eDBType);

	//Item
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_Item*>&,EDBType eDBType);

	//Item baibao
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_ItemBaiBao*>&,EDBType eDBType);

	//Item del
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_ItemDel*>&,EDBType eDBType);

	//item cd time
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_ItemCDTime*>&,EDBType eDBType);

	//item_needlog
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_ItemNeedLog*>&,EDBType eDBType);

	//log baibao
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_LogBaiBao*>&,EDBType eDBType);

	//role del
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_RoleDel*>&,EDBType eDBType);

	//roledata
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_RoleData*>&,EDBType eDBType);

	//roledata del
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_RoleDataDel*>&,EDBType eDBType);

	//skill
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_Skill*>&,EDBType eDBType);

	//task
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_Task*>&,EDBType eDBType);

	//task done
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_TaskDone*>&,EDBType eDBType);

	//title
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_Title*>&,EDBType eDBType);

	//名帖
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_VisitingCard*>&,EDBType eDBType);

	//元宝
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_YBAccount*>&,EDBType eDBType);

	//元宝交易订单
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_YuanBaoOrder*>&,EDBType eDBType);

	//bill_item
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_BillItem*>&,EDBType eDBType);
	//bill_yuanbao
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_BillYuanBao*>&,EDBType eDBType);
	//bill_yuanbao_log
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_BillYuanBaoLog*>&,EDBType eDBType);
	//pet_data
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_PetData*>&,EDBType eDBType);
	//pet_skill
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_PetSkill*>&,EDBType eDBType);
	//vip_stall
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_VipStall*>&,EDBType eDBType);
	//guild_commodity
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_GuildCommodity*>&,EDBType eDBType);
	//guild_skill
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_GuildSkill*>&,EDBType eDBType);
	//vip_netbar
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_VipNetBar*>&,EDBType eDBType);
	//guild_upgrade
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_GuildFacilities*>&,EDBType eDBType);
	//commerce_rank
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_CommerceRank*>&,EDBType eDBType);
	//activity
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_ActivityData*>&,EDBType eDBType);
	//留言
	BOOL Loong_Insert(std::vector <tagTWrap_Loong_LeftMsg*>&,EDBType eDBType);

//account query
private:
	VOID FormatAccountQueryItem(Beton::MyStream *pStream,DWORD dwAccountID,EDBType eDBType);

//paging query
private:
	VOID FormatPagingQueryEquip(Beton::MyStream *pStream,INT64 n64Limit,EDBType eDBType);
	VOID FormatPagingQueryEquipDel(Beton::MyStream *pStream,INT64 n64Limit, EDBType eDBType);
	VOID FormatPagingQueryTaskDone(Beton::MyStream *pStream,INT64 n64Limit,EDBType eDBType);
	VOID FormatPagingQueryTask(Beton::MyStream *pStream,INT64 n64Limit,EDBType eDBType);
	VOID FormatPagingQueryItemDel(Beton::MyStream *pStream,INT64 n64Limit,EDBType eDBType);

private:

	//格式化sql语句
	VOID FormatQueryAccountCommon(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryBlackList(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryBuff(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryClanData(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryEnemy(Beton::MyStream *pStream,EDBType eDBType);
//	VOID FormatQueryEquip(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryEquipBaiBao(Beton::MyStream *pStream,EDBType eDBType);
//	VOID FormatQueryEquipDel(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryActClanTreasure(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryFameHallEnterSnap(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryRepRstTime(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryFriendSave(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryFriendshipSave(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryGroupPurchase(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryGuild(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryGuildMem(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryItem(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryItemBaiBao(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryItemDel(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryItemCDTime(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryItemNeedLog(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryLogBaiBao(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryRoleDel(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryRoleData(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryRoleDataDel(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQuerySkill(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryTask(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryTaskDone(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryTitle(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryVisitingCard(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryYBAccount(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryYuanBaoOrder(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryBillItem(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryBillYuanbao(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryBillYuanbaoLog(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryPetData(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryPetSkill(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryVipStall(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryGuildCommodity(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryGuildSkill(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryVipBar(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryGuildUpgrade(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryCommerceRank(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryActivity(Beton::MyStream *pStream,EDBType eDBType);
	VOID FormatQueryLeftMsg(Beton::MyStream *pStream,EDBType eDBType);

	//格式化sql语句
	BOOL FormatInsertAccountCommon(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertBlackList(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertBuff(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertClanData(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertEnemy(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);

	BOOL FormatInsertEquip(Beton::MyStream *pStream,LPVOID &pData,LPCSTR pszTable,EDBType eDBType);

	BOOL FormatInsertActClanTreasure(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertFameHallEnterSnap(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertRepRstTime(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertFriendSave(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertFriendshipSave(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertGroupPurchase(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertGuild(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertGuildMem(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);

	BOOL FormatInsertItem(Beton::MyStream *pStream,LPVOID &pData,LPCSTR pszTable,EDBType eDBType);

	BOOL FormatInsertItemCDTime(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertItemNeedLog(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertLogBaiBao(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertRoleDel(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);

	BOOL FormatInsertRoleData(Beton::MyStream *pStream,LPVOID &pData,LPCSTR pszTable,EDBType eDBType);

	BOOL FormatInsertSkill(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertTask(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertTaskDone(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertTitle(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertVisitingCard(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertYBAccount(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertYuanBaoOrder(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertBillItem(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertBillYuanbao(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertBillYuanbaoLog(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertPetData(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertPetSkill(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertVipStall(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertGuildCommodity(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertGuildSkill(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertVipBar(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertGuildUpgrade(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertCommerceRank(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertActivity(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);
	BOOL FormatInsertLeftMsg(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType);

	//查询结果操作
	VOID ProcResQueryAccountCommon(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryBlackList(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryBuff(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryClanData(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryEnemy(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryEquip(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryActClanTreasure(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryFameHallEnterSnap(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryRepRstTime(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryFriendSave(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryFriendshipSave(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryGroupPurchase(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryGuild(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryGuildMem(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryItem(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryItemCDTime(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryItemNeedLog(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryLogBaiBao(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryRoleDel(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryRoleData(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQuerySkill(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryTask(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryTaskDone(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryTitle(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryVisitingCard(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryYBAccount(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryYuanBaoOrder(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryBillItem(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryBillYuanbao(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryBillYuanbaoLog(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryPetData(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryPetSkill(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryVipStall(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryGuildCommodity(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryGuildSkill(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryVipBar(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryGuildUpgrade(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryCommerceRank(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryActivity(OUT LPVOID &pData, Beton::QueryResult *pRes);
	VOID ProcResQueryLeftMsg(OUT LPVOID &pData, Beton::QueryResult *pRes);

};

//Beton回调函数
VOID LoongDBCallBack(DataBase* pDB, INT nReason, INT nParam);

#define sLoongBeton CLoongLoongDBCtrl::getSingleton()

