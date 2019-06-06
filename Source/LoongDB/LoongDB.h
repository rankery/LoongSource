//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: LoongDB.h
// author: Sxg
// actor:
// data: 2008-05-08
// last:
// brief: 数据库操作应用层(所有数据的合法性检查由外部进行,该类只提供接口)
//-----------------------------------------------------------------------------
#pragma once

struct tagRoleDataLoad;
struct tagRoleDataSave;
struct tagRecordDel;
struct tagSkillSave;
struct tagBuffSave;
struct tagNDBS_LoadRole;
struct tagSimRole;
struct tagCreateRoleInfo;
struct tagItem;
struct tagEquip;
struct tagEquipSpecUpdate;
struct tagEquipSpec;
struct tagCustomVCardData;
struct tagDBReputeData;
struct tagCreateGuildInfo;
struct tagCreateGuildMemInfo;
struct tagDBPetSoulCreate;
struct tagDBPetSkill;
struct tagDBPetAtt;
struct tagGuildBase;
struct tagGPInfoKey;
struct tagGPInfo;
struct tagVipStall2DB;
struct tagDBVNBPlayers;
struct tagFacilitiesLoad;
struct tagGuildSkillInfo;
struct tagExternalLink;

//-----------------------------------------------------------------------------
class CLoongDB
{
typedef VOID (CLoongDB::*Fun_pFormat3)(Beton::MyStream*, DWORD, DWORD);
typedef VOID (CLoongDB::*Fun_pFormat3_1)(Beton::MyStream*, DWORD, LPVOID);
typedef VOID (CLoongDB::*Fun_pFormat3_2)(Beton::MyStream*, LPCSTR, LPVOID);
typedef VOID (CLoongDB::*Fun_pProcRes)(LPVOID&, INT32*, Beton::QueryResult*);

public:
	CLoongDB();
	~CLoongDB();

public:
	BOOL Init(LPCTSTR szIniFileName);
	VOID Destroy();

private: //模版方法
	// 读取数据库信息(有结果集)
	BOOL Load(OUT LPVOID &pData, OUT INT32 *pNum, DWORD dwAccountID, 
				DWORD dwRoleID, Fun_pFormat3 pFunFormat, Fun_pProcRes pFunProcRes);

	// 插入，删除，更新等数据库操作模版方法(无结果集,且不需要数据库联接)
	BOOL Update(DWORD dwRoleID, LPVOID pData, INT32 nNum, INT32 nSzOne, 
				Fun_pFormat3_1 pFunFormat, OUT LPVOID *ppOutPointer = NULL);

	// 插入，删除，更新等数据库操作模版方法(无结果集,且不需要数据库联接)
	VOID UpdateLog(LPCSTR szTablePostfix, LPVOID pData, Fun_pFormat3_2 pFunFormat);

public: // 监控信息
	DWORD GetTotalReadTimes() const { return m_dwReadTimes; }
	DWORD GetTotalWriteTimes() const { return m_dwWriteTimes; }
	INT32 TestDBReadFunction();

public:
	// 创建角色
	BOOL CreateRole(OUT DWORD &dwNewRoleID, DWORD dwAccountID, const tagCreateRoleInfo &RoleInfo, DWORD &dwRoleNameCrc);
	// 备份角色相关数据
	DWORD BackupRole(DWORD dwAccountID, DWORD dwRoleID);
	// 删除角色
	DWORD DelRole(DWORD dwAccountID, DWORD dwRoleID);
	// 记录删除角色时的角色名称，Crc，时间及IP等
	BOOL RecordInfoDelRole(DWORD dwAccountID, DWORD dwRoleID, LPCTSTR szRoleName, LPCSTR szIP);
	// 恢复角色
	BOOL ResumeRole(DWORD dwRoleID);

	// 读取所有玩家简易信息
	BOOL LoadAllRoleInfo(LPVOID pData, INT &nRoleNum);
	// 获取某个帐号的所有角色基本信息,内存由外部分配
	BOOL LoadSimRole(DWORD dwAccountID, INT &nRoleNum, LPVOID pSimRole);
	// 读取玩家某个角色全部信息,内存由外部分配
	BOOL LoadRole(DWORD dwAccountID, DWORD dwRoleID, tagRoleDataLoad *pRoleData);
	// 存储角色相关信息
	VOID SaveRole(DWORD dwRoleID, tagRoleDataSave *pRoleData);
	// 获取所有玩家元宝交易账户信息
	BOOL LoadAllYBAccount(LPVOID pData, INT &nNum);
	// 获取所有玩家的元宝交易订单
	BOOL LoadAllYBOrder(LPVOID pData, INT &nNum);
	// 获取一个玩家的元宝交易订单
	BOOL LoadRoleAllYBOrder(LPVOID pData, INT&nNum, DWORD dwRoleID);

	//// 删除玩家的buff，功夫，item/equip等.用于清除数据库中玩家多余的记录
	//VOID DeleteRecord(DWORD dwRoleID, tagRecordDel *pRecord);

	// 读取固定活动数据
	BOOL LoadActivityData(LPVOID pData, INT &nNum);
	BOOL SaveActivityData(DWORD dwID, LPVOID pActivityData);

	// 读取实力排行榜
	BOOL LoadStrengthRankings(LPVOID pData, INT &nNum);
	// 更新实力排行榜，每天24点进行
	BOOL UpdateStrengthRankings();

public:
	// 获取roledata记录条数
	INT32 GetRoleCount() const { return m_nRoleNum; }
	// 获取YBaccount记录条数
	INT32 GetAccountCount() const { return m_nAccountNum; }
	// 获取yuanbaoorder记录条数
	INT32 GetOrderCount() const { return m_nOrderNum; }
	// 获取yuanbaoorder最大索引值
	DWORD GetMaxOrderID() const	{ return m_dwMaxOrderIndex; }
	VOID GetMaxOrderIndex(DWORD& dwMaxIndex);

	// 获取一个玩家yuanbaoorder记录的数量
	VOID GetRoleYBOrderNum(DWORD dwRoleID, INT32 &nOrderNum);

	//　活动固定活动数量
	INT32 GetActivityCount() const { return m_nActivityNum; }

	// 获取当前数据库中物品最大&最小序号
	//INT64 GetMaxSerial() const { return m_n64MaxSerial; }
	//INT64 GetMinSerial() const { return m_n64MinSerial; }

	VOID GetMaxMinSerial(INT64 &n64Max, INT64 &n64Min);

// 删除角色用
private:
	// 备份角色属性信息
	BOOL BackupRoleData(DWORD dwAccountID, DWORD dwRoleID);
	// 备份角色物品信息
	BOOL BackupItem(DWORD dwRoleID);

	// 删除角色属性信息
	BOOL DelRoleData(DWORD dwAccountID, DWORD dwRoleID);
	// 删除角色buff信息
	BOOL DelRoleBuff(DWORD dwRoleID);
	// 删除角色物品信息 //??装备处理
	BOOL DelRoleItem(DWORD dwRoleID);

// 物品装备表操作
public:
	BOOL InsertItem(const tagItem &item, const CHAR* pTable = "item");
	BOOL InsertEquipSpec(const tagEquip &equip, const CHAR *pTable = "equip");
	BOOL InsertEquip(const tagEquip &equip, BOOL bBaiBao = FALSE);

	BOOL SaveItem(const tagItem &item);
	BOOL SaveEquip(const tagEquip &equip);
	BOOL SaveEquipSpec(const tagEquip &equip);

	BOOL UpdateItemPosAndUseRel(DWORD dwRoleID, LPVOID pData, INT32 nNum, OUT LPVOID *ppOutPointer);
	BOOL UpdateEquipSpec(const tagEquipSpecUpdate &equipSpecUpdate);

	BOOL DelItem(INT64 n64Serial, CHAR *pTable = "item");
	BOOL DelEquipSpec(INT64 n64Serial, CHAR *pTable = "equip");
	BOOL DelEquip(INT64 n64Serial);

// 恢复武器操作
public:
	BOOL SaveDelEquipInfo(INT64 n64Serial);
	BOOL SaveDelItemInfo(INT64 n64Serial);

// 物品冷却
public:
	BOOL ReplaceItemCDTime(DWORD dwRoleID, LPVOID pData, INT32 nNum, OUT LPVOID *ppOutPointer);

public:
	// 元宝交易账户相关
	BOOL InsertYBAccount(DWORD dwRoleID);
	BOOL UpdateAccountYB(DWORD dwRoleID, INT32 nYuanBao);
	BOOL UpdateAccountSilver(DWORD dwRoleID, LPVOID pMoney);
	BOOL UpdateSubmitSell(DWORD dwRoleID, bool bSellOrder);
	BOOL UpdateSubmitBuy(DWORD dwRoleID, bool bBuyOrder);
	BOOL InsertOrder(DWORD dwRoleID, LPVOID pYBOrder);
	BOOL CompleteOrder(DWORD dwRoleID, LPVOID pComOrder);
	BOOL RefreshOrder(DWORD dwRoleID, LPVOID pRefreshOrder);
	BOOL DeleteYBOrder(DWORD dwRoleID, LPVOID pDeleteTime);
	
	BOOL LoadRoleItemSerial( DWORD dwRoleID, PVOID &pData, INT &nNum, DWORD dwConType);


	// Jason 外部链接
	BOOL LoadExtLinks(tagExternalLink *& links,int & num);

private:
	VOID FormatInsertItem(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatSaveItem(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pItem);

	VOID FormatInsertEquipSpec(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatSaveEquipSpec(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pEquip);
	VOID FormatUpdateEquipSpec(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pEquipSpecUpdate);

	VOID FormatCommonEquipSpec(Beton::MyStream *pStream, const tagEquipSpec *pEquipSpec);

	VOID FormatDelItem(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatDelEquipSpec(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatSaveActivity(Beton::MyStream *pStream, DWORD dwID, LPVOID pActivityData);

// 技能
public:
	BOOL InsertSkill(DWORD dwRoleID, tagSkillSave *pData);
	BOOL DelSkill(DWORD dwRoleID, DWORD dwSkillID);
	BOOL UpdateSkill(DWORD dwRoleID, tagSkillSave *pData);

private:
	VOID FormatInsertSkill(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatDelSkill(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);

private:
	// 保存角色属性信息
	BOOL SaveRoleData(DWORD dwRoleID, const tagRoleDataSave *pRoleData);
	// 保存氏族信息
	BOOL SaveClanData(DWORD dwRoleID, IN LPVOID pData, BOOL bChange, OUT LPVOID* ppOutPointer);
	//BOOL SaveItemEquip(DWORD dwRoleID, LPVOID pData, INT32 nNum, OUT LPVOID *ppOutPointer);
	BOOL SaveSkill(DWORD dwRoleID, LPVOID pData, INT32 nNum, OUT LPVOID *ppOutPointer);
	BOOL SaveRoleBuff(DWORD dwRoleID, LPVOID pBuff, INT32 nNum, OUT LPVOID *ppOutPointer);
	BOOL SaveTitle(DWORD dwRoleID, LPVOID pData, INT32 nNum, OUT LPVOID *ppOutPointer);

private:	
	// 保存角色属性信息
	VOID FormatSaveRoleData(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatUpdateItemPosAndUseRel(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatSaveSkill(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatSaveRoleBuff(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pBuff);
	VOID FormatSaveTitle(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatSaveTask(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pTask);
	VOID FormatSaveTaskDone(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pTask);
	VOID FormatDelAllRoleBuff(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDummy);


// 读取数据信息方法,内存由外部分配
private:
	BOOL LoadRoleData(LPVOID &pData, DWORD dwAccountID, DWORD dwRoleID);
	BOOL LoadSkill(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
	BOOL LoadRoleBuff(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
	BOOL LoadTitle(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
	BOOL LoadItem(LPVOID &pData, INT32 &nNum, DWORD dwAccountID, DWORD dwRoleID);
	BOOL LoadFriend(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
	BOOL LoadEnemy(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
	BOOL LoadTask(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
	BOOL LoadTaskDone(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
	BOOL LoadSimTaskDone(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
	BOOL LoadItemCDTime(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
	BOOL LoadFriendship(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
	BOOL LoadFriendshipEx(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
	BOOL LoadBlackList(LPVOID &pData, INT32 &nNum, DWORD dwRoleID);
public:
	BOOL LoadAccountCommon(LPVOID pData, DWORD dwAccountID);
	BOOL LockAccountCommon();

// 读取数据信息SQL语句格式化
private:
	VOID FormatLoadAllRoleInfo(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadSimRole(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadRoleData(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadSkill(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadRoleBuff(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadTitle(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadItem(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	//VOID FormatLoadEquip(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadFriend(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadEnemy(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadTask(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadTaskDone(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadSimTaskDone(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadItemCDTime(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadFriendship(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadFriendshipEx(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadBlackList(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);

	VOID FormatLoadYBAccount(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadYBOrder(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadRoleYBOrder(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);

	VOID FormatLoadAccountCommon(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadSafeCode(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);

	VOID FormatActivityData(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);

	VOID FormatStrengthRankings(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);

	VOID FormatLoadRoleItemSerials(Beton::MyStream *pStream, DWORD dwMisc, DWORD dwRoleID);
	
// 检验并保存查询结果
private: 
	VOID ProcResLoadAllRoleInfo(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadSimRole(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadRoleData(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadSkill(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadRoleBuff(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadTitle(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadItem(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	//VOID ProcResLoadEquip(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadFriend(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadEnemy(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadTask(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadTaskDone(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadSimTaskDone(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadItemCDTime(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadFriendship(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadBlackList(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadYBAccount(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadYBOrder(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadRoleOrder(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

	VOID ProcResLoadAccountCommon(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadSafeCode(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

	VOID ProcResLoadActivityData(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

	VOID ProcResLoadStrengthRankings(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

	VOID ProcResLoadRoleItemSerials( OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes );

// 实时处理外部接口(为了处理上方便)
public:
	// 好友相关
	BOOL InsertFriend(DWORD dwRoleID, LPVOID pFriendSave);
	BOOL UpdateFriend(DWORD dwRoleID, LPVOID pFriendSave);
	BOOL DeleteFriend(DWORD dwRoleID, DWORD dwFriendID);
	BOOL DeleteAllFriRel(DWORD dwRoleID);

	// 友好度相关
	BOOL InsertFriendship(DWORD dwRoleID, LPVOID pFriendshipSave);
	BOOL UpdateFriendship(DWORD dwRoleID, LPVOID pFriendshipSave);
	BOOL DeleteFriendship(DWORD dwRoleID, DWORD dwFriendID);
	BOOL DeleteAllFriValRel(DWORD dwRoleID);

	// 黑名单
	BOOL InsertBlack(DWORD dwRoleID, DWORD dwBlackID);
	BOOL DeleteBlack(DWORD dwRoleID, DWORD dwBlackID);

	// 仇敌
	BOOL InsertEnemy(DWORD dwRoleID, DWORD dwEnemyID);
	BOOL DeleteEnemy(DWORD dwRoleID, DWORD dwEnemyID);

	// 称号
	BOOL InsertTitle(DWORD dwRoleID, LPVOID pData);
	BOOL RemoveTitle(DWORD dwRoleID, LPVOID pData);
	// 任务
	BOOL InsertTask(DWORD dwRoleID, LPVOID pAcceptQuest);
	BOOL SaveTask(DWORD dwRoleID, LPVOID pTask, INT32 nNum, OUT LPVOID *ppOutPointer);
	BOOL SaveTaskDone(DWORD dwRoleID, LPVOID pTask, INT32 nNum);
	BOOL DeleteTask(DWORD dwRoleID, DWORD dwTaskID);

	//结婚离婚
	BOOL RoleGetMarriage(DWORD dwRoleID1, DWORD dwRoleID2);
	BOOL RoleBreakMarriage(DWORD dwRoleID1, DWORD dwRoleID2);

// 实时处理SQL语句格式化
private: 
	//结婚离婚
	VOID FormatRoleGetMarriage(Beton::MyStream *pStream, DWORD dwRoleID1, LPVOID pData);
	VOID FormatRoleBreakMarriage(Beton::MyStream *pStream, DWORD dwRoleID1, LPVOID pData);

	// 好友
	VOID FormatInsertFriend(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendSave);
	VOID FormatUpdateFriend(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendSave);
	VOID FormatDeleteFriend(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendID);
	VOID FormatDeleteAllFriRel(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDummy);

	// 友好度
	VOID FormatInsertFriendship(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendshipSave);
	VOID FormatUpdateFriendship(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendshipSave);
	VOID FormatDeleteFriendship(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendID);
	VOID FormatDeleteAllFriValRel(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDummy);
	
	// 黑名单
	VOID FormatInsertBlack(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pBlackID);
	VOID FormatDeleteBlack(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pBlackID);

	// 仇敌
	VOID FormatInsertEnemy(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pEnemyID);
	VOID FormatDeleteEnemy(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pEnemyID);

	// 称号
	VOID FormatInsertTitle(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatRemoveTitle(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	// 任务
	VOID FormatInsertTask(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pTaskID);
	VOID FormatDeleteTask(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pTaskID);

	// 元宝交易账户
	VOID FormatInsertYBAccount(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatAccountYB(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatAccountSilver(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pMoney);
	VOID FormatSubmitSell(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatSubmitBuy(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatInsertOrder(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatCompleteOrder(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatRefreshOrder(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatDeleteYBOrder(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);

public:
	// 读取百宝袋物品
	BOOL LoadItemBaiBao(DWORD dwAccountID, INT32 nNumLoad, INT64 n64Serial, INT32 &nNumLoaded, INT32 &nDataSize, LPVOID pData);
	// 读取百宝袋中元宝数量
	BOOL LoadBaiBaoYuanBao(DWORD dwAccountID, INT32& nYuanBao);
	// 读取百宝袋Log记录
	BOOL LoadBaibaoLogInfo(DWORD dwAccountID, INT32 &nNumLoaded, INT32 &nDataSize, LPVOID pData);
	// 插入一条Log信息
	BOOL InsertBaibaoLogInfo(DWORD dwAccountID, LPVOID pData);

private:
	VOID FormatLoadItemBaiBao(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadBaiBaoYuanBao(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID ProcResLoadBaiBaoYuanBao(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
//	VOID ProcResLoadItemBaiBao(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

	VOID FormatLoadBaiBaoLog(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID ProcResLoadBaiBaoLog(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResCountBaiBaoLog(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

	VOID FormatInsertBaiBaoLog(Beton::MyStream *pStream, DWORD dwAccountID, LPVOID pData);
	VOID FormatDeleteBaiBaoLog(Beton::MyStream *pStream, DWORD dwAccountID, LPVOID pData);

	VOID FormatLoadItemNeedLog(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID ProcResLoadItemNeedLog(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

public:
	VOID GetPetCountAndMaxPetID(INT32 &nCount, DWORD &dwMaxPetID);

private:
	VOID ResetLoadNumLim(INT32 nLoadNum)	{ m_nLoadNumLim = nLoadNum; }
	VOID ResetLoadSerial(INT64 n64Serial)	{ m_n64LoadSerial = n64Serial; }
	
	// 获取roledata记录条数和最大RoleID
	VOID GetRoleCountAndMaxRoleID(INT32 &nCount, DWORD &dwMaxRoleID);
	// 获取pet_data记录条数和最大pet_id
	// 获取YBAccount记录的数量
	VOID GetYBAccountNum(INT32 &nAccountNum);

	// 获取yuanbaoorder记录的数量
	VOID GetYBOrderNum(INT32 &nOrderNum);

	// 获得固定活动数量
	VOID GetActivityNum(INT32 &nOrderNum);
	
	VOID GetMaxMinSerial(const char* pTable, INT64 &n64Max, INT64 &n64Min);

private:
	// 初始化物品读物字段
	VOID InitItemLoad(Beton::MyStream *pStream);

/************************ 修改角色属性 **************************/
public:
	BOOL ChangeRoleName(DWORD dwAccountID, DWORD dwRoleID, LPCTSTR szNewRoleName);
	BOOL ChangeRoleGuild(DWORD dwRoleID, DWORD dwGuildID);
	BOOL SetRoleRemoveFlag(DWORD dwRoleID, DWORD dwTime);
	BOOL ChangeRoleDressMdID(DWORD dwRoleID, WORD wValue);

private:
	VOID FormatUpdateChangeRoleGuild(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatUpdateSetRoleRemoveFlag(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pTime);
	VOID FormatUpdateChangeRoleDressMdID(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatUpdateSetRoleCommerceStatus(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);

/************************ 帐号相关 **************************/
public:
	BOOL InsertAccountCommon(DWORD dwAccountID, const CHAR* szAccount);
	BOOL UpdateSafeCode(DWORD dwAccountID, DWORD dwSafeCodeCrc);
	BOOL UpdateSafeCodeResetTime(DWORD dwAccountID, DWORD dwTimeReset);
	BOOL UpdateBagPsd(DWORD dwAccountID, DWORD dwNewBagPsd);
	BOOL UpdateWareSize(DWORD dwAccountID, LPVOID pData);
	BOOL UpdateWareMoney(DWORD dwAccountID, INT64 n64WareSilver);
	BOOL UpdateBaiBaoYuanBao(DWORD dwAccountID, LPVOID pData);

private:
	VOID FormatInsertAccountCommon(Beton::MyStream *pStream, DWORD dwAccountID, LPVOID pData);
	VOID FormatUpdateSafeCode(Beton::MyStream *pStream, DWORD dwAccountID, LPVOID pData);
	VOID FormatUpdateSafeCodeResetTime(Beton::MyStream *pStream, DWORD dwAccountID, LPVOID pData);
	VOID FormatUpdateBagPsd(Beton::MyStream *pStream, DWORD dwAccountID, LPVOID pData);
	VOID FormatUpdateWareSize(Beton::MyStream *pStream, DWORD dwAccountID, LPVOID pData);
	VOID FormatUpdateWareMoney(Beton::MyStream *pStream, DWORD dwAccountID, LPVOID pData);
	VOID FormatUpdateBaiBaoYuanBao(Beton::MyStream *pStream, DWORD dwAccountID, LPVOID pData);


/************************ 名帖相关 **************************/
public:
	BOOL InsertVCard(DWORD dwRoleID);
	BOOL UpdateVCard(DWORD dwRoleID, const PVOID pData);
	BOOL DelVCard(DWORD dwRoleID);	
	BOOL LoadVCard(LPVOID &pData, DWORD dwRoleID);
private:
	VOID FormatInsertVCard(Beton::MyStream *pStream, DWORD dwRoleID, PVOID pDum);
	VOID FormatUpdateVCard(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatDelVCard(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDum);
	VOID FormatLoadVCard(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
private:
	VOID ProcResLoadVCard(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);


/************************ 氏族相关 **************************/
public:
	BOOL InsertClanData(DWORD dwRoleID);
	BOOL DelClanData(DWORD dwRoleID);
	BOOL LoadClanData(LPVOID &pData, DWORD dwRoleID);
private:
	VOID FormatInsertClanData(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDum);
	VOID FormatUpdateClanData(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDum);
	VOID FormatDelClanData(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDum);
	VOID FormatLoadClanData(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
private:
	VOID ProcResLoadClanData(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);


/************************ 名人堂相关 **************************/
public:	// 名人堂
	BOOL LoadActTreasures(LPVOID& pData, INT& nTotalNum);
	BOOL LoadFameHallSnaps(LPVOID& pData, INT& nTotalNum);
	BOOL LoadRepRstTimeStamp(LPVOID& pData);
	BOOL LoadRepRank(BYTE byClanType, PVOID pData, INT32 &nNum);

	BOOL InsertFameHallEnterSnap(PVOID pData);
	BOOL InsertActTreasure(PVOID pData);

	BOOL UpdateRepRstTime(BYTE byClanType, DWORD dwRstTime);

private:
	VOID FormatLoadActTreasures(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadFameSnaps(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadRstTimeStamps(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatLoadRepRank(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);

	VOID FormatInsertActTreasure(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDum);
	VOID FormatInsertFameHallEnterSnap(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDum);

	VOID FormatUpdateRepRstTime(Beton::MyStream *pStream, DWORD dwClanType, LPVOID pData);

private:
	VOID ProcResLoadActTreasures(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadFameSnaps(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadRstTimeStamps(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadRepRank(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);


/************************ 帮派属性表相关 **************************/
public:
	BOOL LoadAllGuild(LPVOID pBuff, INT32& nGuildNum);
	BOOL InsertGuild(tagCreateGuildInfo* pCreateGuildInfo);
	BOOL DeleteGuild(DWORD dwGuildID);
	BOOL UpdateGuildTenet(DWORD dwGuildID, LPTSTR szNewTenet);
	BOOL UpdateAllGuild(tagGuildBase *pGuildBase, INT32 nGuildNum);
	BOOL UpdateGuildAtt(tagGuildBase *pGuildBase);

private:
	VOID FormatLoadAllGuild(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatInsertGuild(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatDeleteGuild(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatUpdateGuildTenet(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData);
	VOID FormatUpdateAllGuild(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData);

	VOID ProcResLoadAllGuild(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);


/************************ 帮派成员属性表相关 **************************/
public:
	BOOL LoadAllGuildMember(LPVOID pBuff, INT32& nGuildMemNum);
	BOOL InsertGuildMember(const tagCreateGuildMemInfo& sCreateGuildMemInfo);
	BOOL DeleteGuildMember(DWORD dwRoleID);
	BOOL UpdateGuildMemberPos(DWORD dwRoleID, INT8 n8NewPos);
	BOOL GuildWarePrivilege(DWORD dwRoleID, BOOL bEnable);
	BOOL ChangeMemberContribution(LPVOID pData);

private:
	VOID FormatLoadAllGuildMember(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatInsertGuildMember(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatDeleteGuildMember(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatUpdateGuildMemberPos(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatGuildWarePrivilege(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatChangeMemberContribution(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);

	VOID ProcResLoadAllGuildMember(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

/************************ 帮派仓库相关 **************************/
public:
	BOOL LoadGuildWareItems(LPVOID &pBuff, DWORD dwGuildID, INT32& nItemNum);

private:
	VOID FormatLoadGuildWareItems(Beton::MyStream *pStream, DWORD dwGuildID, DWORD dwRoleID);

/************************ 帮派设施相关 **************************/
public:
	BOOL LoadGuildFacilities(LPVOID pBuff, INT32& nInfoNum, DWORD dwGuildID);
	BOOL InsertGuildFacilities(tagFacilitiesLoad* pInfo);
	BOOL UpdateGuildFacilities(tagFacilitiesLoad* pInfo);
	BOOL RemoveAllFacilities(DWORD dwGuildID);

private:
	VOID FormatLoadGuildFacilities(Beton::MyStream *pStream, DWORD dwGuildID, DWORD dwRoleID);
	VOID FormatInsertGuildFacilities(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData);
	VOID FormatUpdateGuildFacilities(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatRemoveAllFacilities(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData);

	VOID ProcResLoadGuildFacilities(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

/************************ 帮派技能相关 **************************/
public:
	BOOL LoadGuildSkillInfo(LPVOID pData, DWORD dwGuildID, INT& nNum);
	BOOL InsertGuildSkillInfo(DWORD dwGuildID, tagGuildSkillInfo* pSkillInfo);
	BOOL UpdateGuildSkillInfo(DWORD dwGuildID, tagGuildSkillInfo* pSkillInfo);
	BOOL ChangeResearchGuildSkill(DWORD dwGuildID, DWORD dwSkillID);
	BOOL RemoveGuildSkillInfo(DWORD dwGuildID);

private:
	VOID FormatLoadGuildSkillInfo(Beton::MyStream *pStream, DWORD dwGuildID, DWORD dwRoleID);
	VOID FormatInsertGuildSkillInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData);
	VOID FormatUpdateGuildSkillInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData);
	VOID FormatChangeResearchGuildSkill(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pSkillID);
	VOID FormatClearResearchGuildSkill(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pSkillID);
	VOID FormatRemoveGuildSkillInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pSkillID);
	
	VOID ProcResLoadGuildSkillInfo(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

/************************ 帮派跑商相关 **************************/
public:
	BOOL LoadGuildCommerceInfo(LPVOID pData, DWORD dwGuildID, INT& nNum);
	BOOL LoadCommerceRankInfo(LPVOID pData, DWORD dwGuildID, INT& nNum);
	BOOL InsertCommodityInfo(LPVOID pData);
	BOOL UpdateCommodityTael(DWORD dwRoleID, INT32 nTael);
	BOOL SaveCommodityInfo(LPVOID pData, DWORD dwRoleID);
	BOOL RemoveCommodityInfo(DWORD dwRoleID);
	BOOL SetCommendation(DWORD dwGuildID, BOOL bCommend);
	BOOL UpdateCommerceRankInfo(DWORD dwGuildID, LPVOID pData);
	BOOL RemoveCommerceRankInfo(DWORD dwGuildID, DWORD dwRoleID);

private:
	VOID FormatLoadGuildCommerceInfo(Beton::MyStream *pStream, DWORD dwGuildID, DWORD dwRoleID);
	VOID FormatLoadCommerceRankInfo(Beton::MyStream *pStream, DWORD dwGuildID, DWORD dwRoleID);
	VOID FormatInsertCommodityInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData);
	VOID FormatUpdateCommodityTael(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID nTael);
	VOID FormatSaveCommodityInfo(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatRemoveCommodityInfo(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatSetCommendation(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData);
	VOID FormatUpdateCommerceRankInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData);
	VOID FormatRemoveCommerceRankInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData);

	VOID ProcResLoadGuildCommerceInfo(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadCommerceRankInfo(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

/************************ 宠物相关 **************************/
public:
	BOOL InsertPetSoul(const tagDBPetSoulCreate* pCreateData, DWORD& dwPetID);
	BOOL DeletePetSoul(DWORD dwPetID);
	BOOL DelPetEquip(DWORD dwPetID);
	
	BOOL InsertPetSkill(PVOID pData, DWORD dwPetID);
	BOOL UpdatePetSkill(PVOID &pData, const INT nNum, DWORD dwPetID);
	BOOL DelPetSkill(DWORD dwPetSkillID, DWORD dwPetID);

	BOOL LoadPets(OUT PVOID& pData, DWORD dwMasterID, INT32& nNum);
	BOOL SavePets(DWORD dwRoleID, IN LPVOID pData, INT32 nNum, OUT LPVOID* ppOutPointer);
	BOOL SaveAPet(OUT PVOID& pData, DWORD dwPetID);
	BOOL UpdatePetAtt( const tagDBPetAtt* pPetAtt, DWORD& dwPetID);

	BOOL LoadAPet(OUT PVOID& pData, DWORD dwPetID);
	BOOL LoadPetSoul(LPVOID pData, DWORD dwPetID);
	BOOL LoadPetSkill(OUT LPVOID& pData, DWORD dwPetID, INT32& nNum);
	BOOL LoadPetEquip(OUT LPVOID& pData, DWORD dwPetID, INT32& nNum);
	BOOL LoadRolePetIDs(DWORD pPetIDs[], DWORD dwMasterID, INT32& nNum);

private:
	VOID FormatInsertPetSoul(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatDeletePetSoul(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);

	VOID FormatInsertPetSkill(Beton::MyStream *pStream, DWORD dwPetID, LPVOID pData);
	VOID FormatUpdatePetSkill(Beton::MyStream *pStream, DWORD dwPetID, LPVOID pData);
	VOID FormatUpdatePetAtt( Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData );
	
	VOID FormatDelPetSkill( Beton::MyStream *pStream, DWORD dwPetID, LPVOID pData );

	VOID FormatLoadPetSoul(Beton::MyStream *pStream, DWORD dwPetID, DWORD dwMasterID);
	VOID FormatLoadPetSkill(Beton::MyStream *pStream, DWORD dwMasterID, DWORD dwPetID);
	VOID FormatLoadPetIDs(Beton::MyStream *pStream, DWORD dwPetID, DWORD dwMasterID);
	VOID FormatLoadPetEquip(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwPetID);
	

	VOID ProcResLoadPetSoul(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadPetSkill(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadPetIDs(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

/************************ 商城相关 **************************/
public:
	INT32 GetGPInfoCount() const { return m_nGroupPurchaseInfoNum; }
	INT32 GetGPInfoSize() const { return m_nGroupPurchaseInfoSize; }

	BOOL LoadAllGPInfo(OUT PVOID pData, INT32 &nInfoNum);
	BOOL AddGPInfo(tagGPInfo* pGPInfo);
	BOOL RemoveGPInfo(tagGPInfoKey* pGPInfoKey);
	BOOL RemoveGuildGPInfo(DWORD dwGuildID);
	BOOL UpdateGPInfo(tagGPInfoKey* pGPInfoKey, DWORD dwNewParticipator);
	BOOL UpdateGPTime();

private:
	VOID FormatLoadAllGPInfo(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatAddGPInfo(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatRemoveGPInfo(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatRemoveGuildGPInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData);
	VOID FormatUpdateGPInfo(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatUpdateGPTime(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);

private:
	VOID GetGPInfoNumAndSize(INT32 &nGPInfoNum, INT32 &nDataSize);

	VOID ProcResLoadAllGPInfo(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

/*********************** VIP摊位相关 ************************/
public:
	BOOL GetAllVIPStallInfo(OUT PVOID pData);
	BOOL ApplyVIPStall(tagVipStall2DB* pInfo);
	BOOL UpdateVIPStallTime();

private:
	VOID FormatGetAllVIPStallInfo(Beton::MyStream *pStream, DWORD dwAccountID, DWORD dwRoleID);
	VOID FormatApplyVIPStall(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatUpdateVIPStallTime(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatInsertVIPStall(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);

private:
	VOID ProcResGetAllVIPStallInfo(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
/************************ 金牌网吧相关 **************************/
public:
	BOOL UpdateVNBPlayer(DWORD dwAccountID, DWORD dwLoginTime);
	BOOL LoadVNBData(OUT PVOID pData, DWORD dwDate);
private:

	VOID FormatLoadHistroyVNBPlayers(Beton::MyStream *pStream, DWORD dwDate, DWORD dwData);
	VOID FormatLoadTodayVNBPlayers(Beton::MyStream *pStream, DWORD dwDate, DWORD dwData);
	VOID FormatUpdateVNBPlayer(Beton::MyStream *pStream, DWORD dwAccountID, PVOID pData);

	VOID ProcResLoadVNBPlayers(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

	/************************ 留言相关 **************************/
public:
	BOOL LoadLeftMsgIndexes(DWORD* pIndexes, DWORD dwRoleID, INT &nNum);
	BOOL LoadLeftMsg(DWORD dwRoleID, PVOID pLeftMsg, DWORD &dwSize);
	BOOL DelLeftMsg(DWORD dwRoleID, DWORD dwDateTime);
	BOOL InsertLeftMsg(DWORD dwRoleID, DWORD dwDateTime, PVOID pLeftMsg);
	BOOL ClearLeftMsg(DWORD dwRoleID);
	BOOL HasLeftMsg(DWORD dwRoleID);

private:
	VOID FormatLoadLeftMsgIndexes(Beton::MyStream *pStream, DWORD dwDumy, DWORD dwRoleID);
	VOID FormatLoadLeftMsg(Beton::MyStream *pStream, DWORD dwDumy, DWORD dwRoleID);
	VOID FormatDelLeftMsg(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDumy);
	VOID FormatInsertLeftMsg(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);
	VOID FormatClearLeftMsg(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData);

	VOID ProcResLoadLeftMsg(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);
	VOID ProcResLoadLeftMsgIndexes(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes);

public:
	BOOL UpdateRoleAtt(DWORD dwRoleID, PVOID pData);
	BOOL UpdateRoleAttPoint(DWORD dwRoleID, LPVOID pMsg);
	BOOL UpdateRoleTalentPoint(DWORD dwRoleID, LPVOID pMsg);
	BOOL UpdateRoleItemTranport(DWORD dwRoleID, LPVOID pMsg);
private:
	VOID FormateUpdateRoleAtt(Beton::MyStream* pStream, DWORD dwRoleID, PVOID pData);
	VOID FormateUpdateRoleAttPoint(Beton::MyStream* pStream, DWORD dwRoleID, PVOID pData);
	VOID FormateUpdateRoleTalentPoint(Beton::MyStream* pStream, DWORD dwRoleID, PVOID pData);
	VOID FormateUpdateRoleItemTransport( Beton::MyStream* pStream, DWORD dwRoleID, PVOID pData );
/*********************** 开启宝箱计数相关 ************************/
public:
	BOOL UpdateTreasureSum(DWORD dwRoleID, INT& nSum);

private:
	VOID FormatUpdateTreasureSum(Beton::MyStream* pStream, DWORD dwRoleID, LPVOID pData);

/************************ log 相关 **************************/
public:
	// 读取需记log的物品TypeID
	BOOL LoadItemNeedLog(LPVOID pTypeID, INT32 &nItemNum);

public:
	// 根据日期创建log表
	BOOL CreateLogMoneyTable(LPCSTR strPostfix);
	BOOL CreateLogYuanBaoTable(LPCSTR strPostfix);
	BOOL CreateLogExVolumeTable(LPCSTR strPostfix);

	BOOL CreateLogTimeStatTable(LPCSTR strPostfix);

	BOOL CreateLogItemTable(LPCSTR strPostfix);
	BOOL CreateLogItemTimesTable(LPCSTR strPostfix);
	BOOL CreateLogMallSellTable(LPCSTR strPostfix);
	BOOL CreateLogMallSellPackTable(LPCSTR strPostfix);

	BOOL CreateLogFundTable(LPCSTR strPostfix);
	BOOL CreateLogMaterialTable(LPCSTR strPostfix);
	BOOL CreateLogReputationTable(LPCSTR strPostfix);
	
	BOOL CreateLogGMCmd(LPCSTR strPostfix);
	BOOL CreateLogPet(LPCSTR strPostfix);

	// 插入log记录
	VOID InsertMoneyLog(LPCSTR szTablePostfix, LPVOID pData);
	VOID InsertYuanBaoLog(LPCSTR szTablePostfix, LPVOID pData);
	VOID InsertExVolumeLog(LPCSTR szTablePostfix, LPVOID pData);

	VOID InsertTimeStatLog(LPCSTR szTablePostfix, LPVOID pData);

	VOID InsertItemLog(LPCSTR szTablePostfix, LPVOID pData);
	VOID InsertItemTimesLog(LPCSTR szTablePostfix, LPVOID pData);
	VOID InsertMallSellLog(LPCSTR szTablePostfix, LPVOID pData);
	VOID InsertMallSellPackLog(LPCSTR szTablePostfix, LPVOID pData);

	VOID InsertFundLog(LPCSTR szTablePostfix, LPVOID pData);
	VOID InsertMaterialLog(LPCSTR szTablePostfix, LPVOID pData);
	VOID InsertReputationLog(LPCSTR szTablePostfix, LPVOID pData);

	VOID InsertGMCmdLog(LPCSTR szTablePostfix, LPVOID pData);
	VOID InsertPetLog(LPCSTR szTablePostfix, LPVOID pData);

private:
	// 根据日期创建log表 -- 通用方法
	BOOL CreateLogTable(LPCSTR strTable, LPCSTR strPostfix);

	// 格式化插入记录语句
	VOID FormatInsertMoneyLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);
	VOID FormatInsertYuanBaoLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);
	VOID FormatInsertExVolumeLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);

	VOID FormatInsertTimeStatLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);

	VOID FormatInsertItemLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);
	VOID FormatInsertItemTimesLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);
	VOID FormatInsertMallSellLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);
	VOID FormatInsertMallSellPackLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);

	VOID FormatInsertFundLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);
	VOID FormatInsertMaterialLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);
	VOID FormatInsertReputationLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);

	VOID FormatInsertGMCmdLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);
	VOID FormatInsertPetLog(Beton::MyStream *pStream, LPCSTR szTablePostfix, LPVOID pData);
/*********************** 锁表 ************************/
public:
	BOOL LockTables(LPCSTR szTableNames);
	BOOL UnlockTables();

private:
	VOID FormatLockTables(Beton::MyStream *pStream, DWORD dwDummy, LPVOID pData);
	VOID FormatUnlockTables(Beton::MyStream *pStream, DWORD dwDummy, LPVOID pData);

/************************ 压测相关 **************************/
public:
	BOOL TestLoadRole(LPVOID &pRoleData, DWORD dwAccountID, DWORD dwRoleID);
	BOOL TestLoadRoleItem(LPVOID &pData, INT32 &nNum, DWORD dwAccountID, DWORD dwRoleID);
	/************************ 属性 **************************/
private:
	TObjRef<Util>		m_pUtil;

	Beton::DataBase*	m_pDBLoong;
	Beton::DataBase*	m_pDBLog;

	volatile BOOL		m_bInitOK;

	DWORD				m_dwMaxRoleID;
	INT32				m_nRoleNum;
	INT32				m_nAccountNum;
	INT32				m_nOrderNum;
	DWORD				m_dwMaxOrderIndex;
	INT32				m_nLoadNumLim;
	INT64				m_n64LoadSerial;
	INT32				m_nGroupPurchaseInfoNum;
	INT32				m_nGroupPurchaseInfoSize;
	DWORD				m_dwMaxPetID;
	INT32				m_nPetNum;
	INT32				m_nActivityNum;
	CHAR				m_szTime[X_DATATIME_LEN + 1];	// 时间转换用字符串
	TCHAR				m_szRoleName[64];				// 角色名称转换为小写用

	//INT64				m_n64MaxSerial;			// 物品最大64位序号 -- 内部生成物品
	//INT64				m_n64MinSerial;			// 物品最小64位序号 -- 运营发放物品

	volatile DWORD		m_dwReadTimes;			// 处理读取消息次数
	volatile DWORD		m_dwWriteTimes;			// 处理写消息次数
};



/***************************** 内联实现 ************************************/

//-----------------------------------------------------------------------------
// 读取角色属性信息
//-----------------------------------------------------------------------------
inline BOOL CLoongDB::TestLoadRole(LPVOID &pRoleData, DWORD dwAccountID, DWORD dwRoleID)
{		
	if(!LoadRoleData(pRoleData, dwAccountID, dwRoleID))
	{
		//pRoleData->dwSize = 0;
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 读取角色物品
//-----------------------------------------------------------------------------
inline BOOL CLoongDB::TestLoadRoleItem(LPVOID &pData, INT32 &nNum, DWORD dwAccountID, DWORD dwRoleID)
{
	return LoadItem(pData, nNum, dwAccountID, dwRoleID);
}
