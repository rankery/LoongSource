#pragma once
#include "Loong_CommonDef.h"
class CLoongMergeLogicCtrl
{
public:
	CLoongMergeLogicCtrl();
	~CLoongMergeLogicCtrl(void);

	//各种set get
public:
	//合并类型
	VOID SetMergeType(EDBMergeType eMergeType)		{ m_eMergeType = eMergeType;}
	
	//源2的游戏世界名
	VOID SetSrc2WorldName(tstring strSrc2WorldName)	{ m_strSrc2WorldName = strSrc2WorldName;}

	//源1的account表最大num
	VOID SetSrc1LoginMaxNum(DWORD dwSrc1LoginMaxNum)		{ m_dwSrc1LoginMaxNum = dwSrc1LoginMaxNum;}

	//源1roledata表最大num
	VOID SetSrc1RoleMaxNum(DWORD dwSrc1RoleMaxNum)		{ m_dwSrc1RoleMaxNum = dwSrc1RoleMaxNum;}

	//源1petdata表最大num
	VOID SetSrc1PetMaxNum(DWORD dwSrc1PetMaxNum)		{ m_dwSrc1PetMaxNum = dwSrc1PetMaxNum;}

	//内部物品最大序号
	VOID SetSrc1ItemMaxSerialInternal(INT64 n64ItemMaxSerialInternal)	{m_n64Src1ItemMaxSerialInternal = n64ItemMaxSerialInternal;}

	//外部物品最小序号
	VOID SetSrc1ItemMinSerialOutter(INT64 n64ItemMinSerialOutter)	{m_n64Src1ItemMinSerialOutter = n64ItemMinSerialOutter;}

	//划拨物品最小序号
	VOID SetSrc1ItemMinSerialBill(INT64 n64ItemMinSerialBill)	{m_n64Src1ItemMinSerialBill = n64ItemMinSerialBill;}

	//得到所有源1的 旧id
	const std::vector <DWORD> &GetAllOldSrc1AccountID()		{ return m_vecOldSrc1AccountID; }

	//源1有，源2也有的用户。旧id
	const std::map <DWORD ,DWORD> &GetOldSrc1VsSrc2()		{ return m_mpOldSrc1VsSrc2; }

	//源2独有的用户集合。 旧id
	const std::vector <DWORD> &GetOldSrc2OnlyAccountID()		{ return m_vecOldSrc2OnlyAccountID; }

	//得到所有帮派id
	const std::set <DWORD> &GetAllOldGuildID()			{ return m_setOldGuildID; }

	//得到玩家补偿
	const std::vector <tagRoleCompensation> &GetRoleCompensation() { return m_vecRoleCompensation; }

	//得到新的外部生产物品64位id
	INT64 GetNewItemSerialOutter()	{ return --m_n64ItemCurSerialOutter; }

public:
	//login 如果跨大区 先调这个方法
	BOOL Loong_LoginMerge(std::vector <tagTWrap_Longin_Account*> &vecSrc1,
						std::vector <tagTWrap_Longin_Account*> &vecSrc2,
						std::vector <tagTWrap_Longin_Account*> &vecTar);
	//login worldforbid表
	BOOL Loong_LoginMerge(std::vector <tagTWrap_Longin_WorldForbid*> &vecSrc1,
						std::vector <tagTWrap_Longin_WorldForbid*> &vecSrc2,
						std::vector <tagTWrap_Longin_WorldForbid*> &vecTar);
	//login 黑名单
	BOOL Loong_LoginMerge(std::vector <tagTWrap_Longin_BlackList*> &vecSrc1,
						std::vector <tagTWrap_Longin_BlackList*> &vecSrc2,
						std::vector <tagTWrap_Longin_BlackList*> &vecTar);
	//login 防沉迷
	BOOL Loong_LoginMerge(std::vector <tagTWrap_Longin_FatigueTime*> &vecSrc1,
						std::vector <tagTWrap_Longin_FatigueTime*> &vecSrc2,
						std::vector <tagTWrap_Longin_FatigueTime*> &vecTar);

public:
	//loong 如果没有跨大区 先调这个方法
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_AccountCommon*> &vecSrc1,
						std::vector <tagTWrap_Loong_AccountCommon*> &vecSrc2,
						std::vector <tagTWrap_Loong_AccountCommon*> &vecTar);

	//role data
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_RoleData*> &vecSrc1,
						std::vector <tagTWrap_Loong_RoleData*> &vecSrc2,
						std::vector <tagTWrap_Loong_RoleData*> &vecTar);

	//帮派
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_Guild*> &vecSrc1,
						std::vector <tagTWrap_Loong_Guild*> &vecSrc2,
						std::vector <tagTWrap_Loong_Guild*> &vecTar);

	//item 内部调下面三个方法
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_Item*> &vecSrc1,
						std::vector <tagTWrap_Loong_Item*> &vecSrc2,
						std::vector <tagTWrap_Loong_Item*> &vecTar);
	//item 实际处理方法
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_Item*> &vecSrc1,
						std::vector <tagTWrap_Loong_Item*> &vecSrc2,
						std::vector <tagTWrap_Loong_Item*> &vecTar,
						ETBItemType emTBType);
	//itembaibao
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_ItemBaiBao*> &vecSrc1,
						std::vector <tagTWrap_Loong_ItemBaiBao*> &vecSrc2,
						std::vector <tagTWrap_Loong_ItemBaiBao*> &vecTar);
	//itemdel
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_ItemDel*> &vecSrc1,
						std::vector <tagTWrap_Loong_ItemDel*> &vecSrc2,
						std::vector <tagTWrap_Loong_ItemDel*> &vecTar);
	//item_cdtime
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_ItemCDTime*> &vecSrc1,
						std::vector <tagTWrap_Loong_ItemCDTime*> &vecSrc2,
						std::vector <tagTWrap_Loong_ItemCDTime*> &vecTar);
	//buff
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_Buff*> &vecSrc1,
						std::vector <tagTWrap_Loong_Buff*> &vecSrc2,
						std::vector <tagTWrap_Loong_Buff*> &vecTar);

	//BlackList
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_BlackList*> &vecSrc1,
						std::vector <tagTWrap_Loong_BlackList*> &vecSrc2,
						std::vector <tagTWrap_Loong_BlackList*> &vecTar);

	//enemy
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_Enemy*> &vecSrc1,
						std::vector <tagTWrap_Loong_Enemy*> &vecSrc2,
						std::vector <tagTWrap_Loong_Enemy*> &vecTar);

	//equip
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_Equip*> &vecSrc1,
						std::vector <tagTWrap_Loong_Equip*> &vecSrc2,
						std::vector <tagTWrap_Loong_Equip*> &vecTar);

	//EquipDel
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_EquipDel*> &vecSrc1,
						std::vector <tagTWrap_Loong_EquipDel*> &vecSrc2,
						std::vector <tagTWrap_Loong_EquipDel*> &vecTar);

	//EquipBaiBao
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_EquipBaiBao*> &vecSrc1,
						std::vector <tagTWrap_Loong_EquipBaiBao*> &vecSrc2,
						std::vector <tagTWrap_Loong_EquipBaiBao*> &vecTar);

	//氏族
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_ClanData*> &vecSrc1,
						std::vector <tagTWrap_Loong_ClanData*> &vecSrc2,
						std::vector <tagTWrap_Loong_ClanData*> &vecTar);

	//名人堂
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_ActClanTreasure*> &vecSrc1,
						std::vector <tagTWrap_Loong_ActClanTreasure*> &vecSrc2,
						std::vector <tagTWrap_Loong_ActClanTreasure*> &vecTar);
	
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_FameHallEnterSnap*> &vecSrc1,
						std::vector <tagTWrap_Loong_FameHallEnterSnap*> &vecSrc2,
						std::vector <tagTWrap_Loong_FameHallEnterSnap*> &vecTar);

	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_RepRstTime*> &vecSrc1,
						std::vector <tagTWrap_Loong_RepRstTime*> &vecSrc2,
						std::vector <tagTWrap_Loong_RepRstTime*> &vecTar);

	//friend
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_Friend*> &vecSrc1,
						std::vector <tagTWrap_Loong_Friend*> &vecSrc2,
						std::vector <tagTWrap_Loong_Friend*> &vecTar);

	//friendship
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_FriendshipSave*> &vecSrc1,
						std::vector <tagTWrap_Loong_FriendshipSave*> &vecSrc2,
						std::vector <tagTWrap_Loong_FriendshipSave*> &vecTar);

	//GroupPurchase
	BOOL  Loong_LoongMerge(std::vector <tagTWrap_Loong_GroupPurchase*> &vecSrc1,
						std::vector <tagTWrap_Loong_GroupPurchase*> &vecSrc2,
						std::vector <tagTWrap_Loong_GroupPurchase*> &vecTar);

	//GuildMem
	BOOL  Loong_LoongMerge(std::vector <tagTWrap_Loong_GuildMem*> &vecSrc1,
						std::vector <tagTWrap_Loong_GuildMem*> &vecSrc2,
						std::vector <tagTWrap_Loong_GuildMem*> &vecTar);

	//item_needlog
	BOOL  Loong_LoongMerge(std::vector <tagTWrap_Loong_ItemNeedLog*> &vecSrc1,
						std::vector <tagTWrap_Loong_ItemNeedLog*> &vecSrc2,
						std::vector <tagTWrap_Loong_ItemNeedLog*> &vecTar);

	//log_baibao
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_LogBaiBao*> &vecSrc1,
						std::vector <tagTWrap_Loong_LogBaiBao*> &vecSrc2,
						std::vector <tagTWrap_Loong_LogBaiBao*> &vecTar);
	//RoleDel
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_RoleDel*> &vecSrc1,
						std::vector <tagTWrap_Loong_RoleDel*> &vecSrc2,
						std::vector <tagTWrap_Loong_RoleDel*> &vecTar);

	//pet_data
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_PetData*> &vecSrc1,
						std::vector <tagTWrap_Loong_PetData*> &vecSrc2,
						std::vector <tagTWrap_Loong_PetData*> &vecTar);
	//pet skill
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_PetSkill*> &vecSrc1,
						std::vector <tagTWrap_Loong_PetSkill*> &vecSrc2,
						std::vector <tagTWrap_Loong_PetSkill*> &vecTar);
	//skill
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_Skill*> &vecSrc1,
						std::vector <tagTWrap_Loong_Skill*> &vecSrc2,
						std::vector <tagTWrap_Loong_Skill*> &vecTar);
	//task
	BOOL  Loong_LoongMerge(std::vector <tagTWrap_Loong_Task*> &vecSrc1,
						std::vector <tagTWrap_Loong_Task*> &vecSrc2,
						std::vector <tagTWrap_Loong_Task*> &vecTar);
	//taskdone
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_TaskDone*> &vecSrc1,
						std::vector <tagTWrap_Loong_TaskDone*> &vecSrc2,
						std::vector <tagTWrap_Loong_TaskDone*> &vecTar);

	//title
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_Title*> &vecSrc1,
						std::vector <tagTWrap_Loong_Title*> &vecSrc2,
						std::vector <tagTWrap_Loong_Title*> &vecTar);

	//vip_stall
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_VipStall*> &vecSrc1,
						std::vector <tagTWrap_Loong_VipStall*> &vecSrc2,
						std::vector <tagTWrap_Loong_VipStall*> &vecTar);

	//VisitingCard
	BOOL  Loong_LoongMerge(std::vector <tagTWrap_Loong_VisitingCard*> &vecSrc1,
						std::vector <tagTWrap_Loong_VisitingCard*> &vecSrc2,
						std::vector <tagTWrap_Loong_VisitingCard*> &vecTar);

	//ybaccount
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_YBAccount*> &vecSrc1,
						std::vector <tagTWrap_Loong_YBAccount*> &vecSrc2,
						std::vector <tagTWrap_Loong_YBAccount*> &vecTar);

	//yuanbaoorder
	BOOL  Loong_LoongMerge(std::vector <tagTWrap_Loong_YuanBaoOrder*> &vecSrc1,
						std::vector <tagTWrap_Loong_YuanBaoOrder*> &vecSrc2,
						std::vector <tagTWrap_Loong_YuanBaoOrder*> &vecTar);

	//bill item
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_BillItem*> &vecSrc1,
						std::vector <tagTWrap_Loong_BillItem*> &vecSrc2,
						std::vector <tagTWrap_Loong_BillItem*> &vecTar);
	//bill yuanbao
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_BillYuanBao*> &vecSrc1,
						std::vector <tagTWrap_Loong_BillYuanBao*> &vecSrc2,
						std::vector <tagTWrap_Loong_BillYuanBao*> &vecTar);
	//bill yuanbao log
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_BillYuanBaoLog*> &vecSrc1,
						std::vector <tagTWrap_Loong_BillYuanBaoLog*> &vecSrc2,
						std::vector <tagTWrap_Loong_BillYuanBaoLog*> &vecTar);
	//guild_commodity
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_GuildCommodity*> &vecSrc1,
						std::vector <tagTWrap_Loong_GuildCommodity*> &vecSrc2,
						std::vector <tagTWrap_Loong_GuildCommodity*> &vecTar);
	//guild_skill
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_GuildSkill*> &vecSrc1,
						std::vector <tagTWrap_Loong_GuildSkill*> &vecSrc2,
						std::vector <tagTWrap_Loong_GuildSkill*> &vecTar);
	//vip_netbar
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_VipNetBar*> &vecSrc1,
						std::vector <tagTWrap_Loong_VipNetBar*> &vecSrc2,
						std::vector <tagTWrap_Loong_VipNetBar*> &vecTar);
	//guild_upgrade
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_GuildFacilities*> &vecSrc1,
						std::vector <tagTWrap_Loong_GuildFacilities*> &vecSrc2,
						std::vector <tagTWrap_Loong_GuildFacilities*> &vecTar);
	//commerce_rank
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_CommerceRank*> &vecSrc1,
						std::vector <tagTWrap_Loong_CommerceRank*> &vecSrc2,
						std::vector <tagTWrap_Loong_CommerceRank*> &vecTar);
	//activity
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_ActivityData*> &vecSrc1,
						std::vector <tagTWrap_Loong_ActivityData*> &vecSrc2,
						std::vector <tagTWrap_Loong_ActivityData*> &vecTar);
	//left msg
	BOOL Loong_LoongMerge(std::vector <tagTWrap_Loong_LeftMsg*> &vecSrc1,
						std::vector <tagTWrap_Loong_LeftMsg*> &vecSrc2,
						std::vector <tagTWrap_Loong_LeftMsg*> &vecTar);
public:
	//干掉所有中间数据 重新初始化变量
	VOID ClearMemory();

private:
	//更新roleid 供源2使用
	VOID Update_Loong_RoleID(DWORD &dwRoleID);

	//更新物品64位id
	VOID Update_Loong_Item64SerialNum(INT64 &n64Serial);

	//转换成小写后的crc32
	DWORD LowerCrc32(LPCTSTR str,DWORD strLen);

public:
	std::vector <tagRoleCompensation> m_vecRoleCompensation; //玩家补偿

//合服用对照表
private:
	//std::map<tagItemNumKey ,INT64> m_mpOldItemNumvsNewItemNum;//新旧item序号对照表

	std::map <tagAccountIDKey,DWORD> m_mpOldAccountVsNewAccount; //跨大区合并需要 旧的accountid与新的accountid的对照表

	std::map <tagGuildIDKey,DWORD> m_mpOldGuildIDvsNewGuildID; //帮派id

	std::map <DWORD ,DWORD> m_mpAccountIDvsWareSize;//新的accountid和仓库size的对照表

	std::map <DWORD ,ETBRoleRemoveType> m_mpRoleIDvsRemoveType; //新的roleid和其remove类型的对照表

	std::set <DWORD> m_setOldGuildID; //所有老的帮派id

	std::map <DWORD ,DWORD> m_mpAccountIDvsRoleIDforCB; //用于处理“合服时被删除角色”角色仓库中绑定物品 改绑定(change bind)使用

	//m_vecSrc1AccountID、m_mpSrc1VsSrc2、m_vecSrc2OnlyAccountID 是用来进行account级转换的对照表
	std::vector <DWORD> m_vecOldSrc1AccountID; //源1的用户集合。 旧id

	std::map <DWORD ,DWORD> m_mpOldSrc1VsSrc2; //源1有，源2也有的用户。旧id
											//key:代表源1的AccountID。value:代表与源1对应的同一用户,在源2中的AccountID。
			
	std::vector <DWORD> m_vecOldSrc2OnlyAccountID; //源2独有的用户集合。 旧id

//合服用中间数据
private:
	std::vector <tagTWrap_Loong_Item> m_vecOverItem; //超出物品 最终写入item_baibao
private:

	EDBMergeType m_eMergeType;

	tstring		m_strSrc2WorldName; //源2世界名

	INT64		m_n64Src1ItemMaxSerialInternal;		//源1 内部生成物品		这个号从500000000000往上加 不包括500000000000
	INT64		m_n64Src1ItemMinSerialOutter;		//源1 外部生成物品		这个号从400000000000往下减 包括400000000000
	INT64		m_n64Src1ItemMinSerialBill;			//源1 直充生成物品最小序号	这个号从300000000000往下减 不包括300000000000

	INT64		m_n64ItemCurSerialInternal;			//当前内部生成物品		这个号从500000000000往上加 不包括500000000000
	INT64		m_n64ItemCurSerialOutter;			//当前外部生成物品		这个号从400000000000往下减 包括400000000000
	INT64		m_n64ItemCurSerialBill;				//当前直充生成物品最小序号	这个号从300000000000往下减 不包括300000000000

	DWORD		m_dwSrc1LoginMaxNum;				//src1 login account表 id最大数

	DWORD		m_dwSrc1RoleMaxNum;					//源1玩家最大数 用于roleid追加

	DWORD		m_dwSrc1PetMaxNum;					//源1 宠物最大petid			这个号从3000000001往上加　不包括3000000001

	DWORD		m_dwSrc1VipStallMaxNum;				//源1 vip摊位最大数

	DWORD		m_dwSrc1WorldNameCrc;				//源1 游戏世界名的crc

};
