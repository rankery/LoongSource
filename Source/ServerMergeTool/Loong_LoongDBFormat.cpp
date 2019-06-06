#include "StdAfx.h"

#include "wx/wx.h"
#include "servermergetoolapp.h"
#include "Loong_LoongDBCtrl.h"



VOID CLoongLoongDBCtrl::FormatQueryAccountCommon(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("account_common",
		"AccountID,AccountName,SafeCodeCrc,ResetTime,BagPsdCrc,BaiBaoYuanBao,WareSize,WareSilver");
}
VOID CLoongLoongDBCtrl::FormatQueryBlackList(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("blacklist","RoleID,BlackID");
}
VOID CLoongLoongDBCtrl::FormatQueryBuff(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("buff", 
		"RoleID,"			"SrcUnitID,"		"SrcSkillID,"			"ItemTypeID,"		"ItemSerialID,"		
		"BuffID,"			"CurTick,"			"Level,"				"CurLapTimes,"		"EffectSkillID");
}
VOID CLoongLoongDBCtrl::FormatQueryClanData(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("clan_data", "RepXuanYuan,ConXuanYuan,ActCntXuanYuan,"
		"RepShenNong,ConShenNong,ActCntShenNong,"
		"RepFuXi,ConFuXi,ActCntFuXi,"
		"RepSanMiao,ConSanMiao,ActCntSanMiao,"
		"RepJiuLi,ConJiuLi,ActCntJiuLi,"
		"RepYueZhi,ConYueZhi,ActCntYueZhi,"
		"RepNvWa,ConNvWa,ActCntNvWa,"
		"RepGongGong,ConGongGong,ActCntGongGong,"
		"FameMask, RepRstTimeStamp,RoleID");
}
//enemy
VOID CLoongLoongDBCtrl::FormatQueryEnemy(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("enemy","RoleID,EnemyID");
}
/*
//武器
VOID CLoongLoongDBCtrl::FormatQueryEquip(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("equip",
		"SerialNum,"	"Quality,"			"PotValUsed,"	"MinUseLevel,"	"MaxUseLevel,"	
		"WuHun,"		"MinDmg,"			"MaxDmg,"		"Armor,"		"PotVal,"		
		"RoleAttEffect,""PosyTimes,"		"PosyEffect,"	"EngraveTimes,"	"EngraveAtt,"
		"HoleNum,"		"HoleGemID,"		"BrandLevel,"	"DerateVal,"	"LongFuLevel,"
		"WuXing,"		"CanCut,"			"LongInnerID,"	"LongOuterID,"	"SpecAtt,"	
		"Appearance,"	"Rein,"				"Savvy,"		"Fortune,"		"ColorID,"	
		"QltyModPct,"	"QltyModPctEx,"		"AttALimMod,"	"AttALimModPct,""FlareVal,"
		"PotValModPct" );
}
VOID CLoongLoongDBCtrl::FormatQueryEquipDel(Beton::MyStream *pStream,EDBType eDBType)
{
		pStream->SetSelect("equip_del",
		"SerialNum,"	"Quality,"			"PotValUsed,"	"MinUseLevel,"	"MaxUseLevel,"	
		"WuHun,"		"MinDmg,"			"MaxDmg,"		"Armor,"		"PotVal,"		
		"RoleAttEffect,""PosyTimes,"		"PosyEffect,"	"EngraveTimes,"	"EngraveAtt,"
		"HoleNum,"		"HoleGemID,"		"BrandLevel,"	"DerateVal,"	"LongFuLevel,"
		"WuXing,"		"CanCut,"			"LongInnerID,"	"LongOuterID,"	"SpecAtt,"	
		"Appearance,"	"Rein,"				"Savvy,"		"Fortune,"		"ColorID,"	
		"QltyModPct,"	"QltyModPctEx,"		"AttALimMod,"	"AttALimModPct,""FlareVal,"
		"PotValModPct" );
}
*/
VOID CLoongLoongDBCtrl::FormatQueryEquipBaiBao(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("equip_baibao",
		"SerialNum,"	"Quality,"			"PotValUsed,"	"MinUseLevel,"	"MaxUseLevel,"	
		"WuHun,"		"MinDmg,"			"MaxDmg,"		"Armor,"		"PotVal,"		
		"RoleAttEffect,""PosyTimes,"		"PosyEffect,"	"EngraveTimes,"	"EngraveAtt,"
		"HoleNum,"		"HoleGemID,"		"BrandLevel,"	"DerateVal,"	"LongFuLevel,"
		"WuXing,"		"CanCut,"			"LongInnerID,"	"LongOuterID,"	"SpecAtt,"	
		"Appearance,"	"Rein,"				"Savvy,"		"Fortune,"		"ColorID,"	
		"QltyModPct,"	"QltyModPctEx,"		"AttALimMod,"	"AttALimModPct,""FlareVal,"
		"PotValModPct," "PotIncTimes,"		"PurpleQltyIdfPct" );
}

VOID CLoongLoongDBCtrl::FormatQueryActClanTreasure(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("famehall_act_treasure",
		"TreasureID,"	"ActNameID," "RoleID," "ClanType," "ActTime");
}
//氏族进入快照
VOID CLoongLoongDBCtrl::FormatQueryFameHallEnterSnap(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("famehall_entersnap",
		"RoleID,"	"EnterNameID," "ClanType," "EnterTime");
}
//氏族声望重置时间快照
VOID CLoongLoongDBCtrl::FormatQueryRepRstTime(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("famehall_rep_rst_timestamp",
		"ClanType," "ResetTimeStamp");
}
//好友
VOID CLoongLoongDBCtrl::FormatQueryFriendSave(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("friend",
		"RoleID," "FriendID," "GroupID");
}
//好友度
VOID CLoongLoongDBCtrl::FormatQueryFriendshipSave(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("friendship",
		"RoleID," "FriendID," "FriVal");
}
//团购
VOID CLoongLoongDBCtrl::FormatQueryGroupPurchase(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("group_purchase",
		"GuildID," "RoleID," "MallID," "ItemPrice," "ParticipatorNum," "RequireNum,"
		"RemainTime," "ParticipatorList");
}
//帮派
VOID CLoongLoongDBCtrl::FormatQueryGuild(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("guild",
		"ID,"			"Name,"			"FounderNameID,"	"SpecState,"		"Level,"
		"HoldCity0,"	"HoldCity1,"	"HoldCity2,"		"Fund,"				"Material,"
		"Rep,"			"DailyCost,"	"Peace,"			"Rank,"				"Tenet,"
		"Symbol,"		"CreateTime,"	"GroupPurchase,"	"LeaderID,"			"RemainSpreadTimes,"
		"Commendation");
}
//帮派成员
VOID CLoongLoongDBCtrl::FormatQueryGuildMem(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("guild_member",
		"RoleID,"	"GuildID,"		"GuildPos,"		"TotalContrib,"		"CurContrib,"
		"Exploit,"	"Salary,"		"JoinTime,"		"CanUseGuildWare");
}
//item
VOID CLoongLoongDBCtrl::FormatQueryItem(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("item",
		"SerialNum,"	"Num,"				"TypeID,"		"Bind,"				"LockStat,"
		"UseTimes,"		"FirstGainTime,"	"CreateMode,"	"CreateID,"			"CreatorID,"	
		"CreateTime,"	"OwnerID,"			"AccountID,"	"ContainerTypeID,"	"Suffix,"
		"NameID");
}
//item baibao
VOID CLoongLoongDBCtrl::FormatQueryItemBaiBao(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("item_baibao",
		"SerialNum,"	"Num,"				"TypeID,"		"Bind,"				"LockStat,"
		"UseTimes,"		"FirstGainTime,"	"CreateMode,"	"CreateID,"			"CreatorID,"	
		"CreateTime,"	"OwnerID,"			"AccountID,"	"ContainerTypeID,"	"Suffix,"
		"NameID");
}
//item del
VOID CLoongLoongDBCtrl::FormatQueryItemDel(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("item_del",
		"SerialNum,"	"Num,"				"TypeID,"		"Bind,"				"LockStat,"
		"UseTimes,"		"FirstGainTime,"	"CreateMode,"	"CreateID,"			"CreatorID,"	
		"CreateTime,"	"OwnerID,"			"AccountID,"	"ContainerTypeID,"	"Suffix,"
		"NameID");
}
//item cd time
VOID CLoongLoongDBCtrl::FormatQueryItemCDTime(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("item_cdtime",
		"RoleID," "CDTime");
}
//item need log
VOID CLoongLoongDBCtrl::FormatQueryItemNeedLog(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("item_needlog",
		"TypeID," "NeedLog," "MinQlty");
}
//log baibao
VOID CLoongLoongDBCtrl::FormatQueryLogBaiBao(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("log_baibao", "AccountID,RoleID,LogTime,n16Type,TypeID,LeaveWords");
}
//role del
VOID CLoongLoongDBCtrl::FormatQueryRoleDel(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("role_del", "AccountID,RoleID,RoleName,IP,DeleteTime");
}
//roledata
VOID CLoongLoongDBCtrl::FormatQueryRoleData(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("roledata", 
		"RoleName,"			"RoleNameCrc,"		"DisplaySet,"		"Sex,"					"HairModelID,"
		"HairColorID,"		"FaceModelID,"		"FaceDetailID,"		"DressModelID,"			"MapID,"	
		"X,"				"Y,"				"Z,"				"FaceX,"				"FaceY,"	
		"FaceZ,"			"Class,"			"ClassEx,"			"Level,"				"ExpCurLevel,"	
		"HP,"				"MP,"				"AttPtAvail,"		"TalentPtAvail,"		"PhysiqueAdded,"	
		"StrengthAdded,"	"PneumaAdded,"		"InnerforceAdded,"	"TechniqueAdded,"		"AgilityAdded,"	
		"Injury,"			"Vitality,"			"Knowledge,"		"Morality,"				"Culture,"	
		"Credit,"			"Identity,"			"VIPPoint,"			"BagSize,"				"BagGold,"	
		"BagSilver,"		"BagYuanBao,"		"GuildID,"			"0,"					"0,"	
		"0,"				"CreateTime,"		"LoginTime,"		"LogoutTime,"			"OnlineTime,"	
		"CurOnlineTime,"	"Rage,"				"Endurance,"		"SGFlag,"				"CloseSGTime,"

		"TalentType1,"		"TalentType2,"		"TalentType3,"		"TalentType4,"			"TalentVal1,"		
		"TalentVal2,"		"TalentVal3,"		"TalentVal4,"		"RebornMapID,"			"TotalTax,"			
		"Morale,"			"GetMallFreeTime,"	"RemoteOpenSet,"	"CurTitleID,"			"ExchangeVolume,"	
		"RoleID,"			"AvatarEquip,"		"RemoveFlag,"		"RemoveTime,"			"AccountID,"		
		"ScriptData,"		"TreasureSum,"		"StallLevel,"		"StallDailyExp,"		"StallCurExp,"	
		"StallLastTime,"

		"Hostility,"		"DeadUnSetSafeGuardCountDown,"		"ItemTransportMapID,"	"ItemTransportX,"	
		"ItemTransportZ,"	"ItemTransportY,"	"Strength," 		"StrengthNum,"			"LoverID,"		
		"HaveWedding,"		"LastLessingTime,"	"LastLessingLevel,"	"NeedPrisonRevive");
}

//roledata del
VOID CLoongLoongDBCtrl::FormatQueryRoleDataDel(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("roledata_del", 
		"RoleName,"			"RoleNameCrc,"		"DisplaySet,"		"Sex,"					"HairModelID,"
		"HairColorID,"		"FaceModelID,"		"FaceDetailID,"		"DressModelID,"			"MapID,"	
		"X,"				"Y,"				"Z,"				"FaceX,"				"FaceY,"	
		"FaceZ,"			"Class,"			"ClassEx,"			"Level,"				"ExpCurLevel,"	
		"HP,"				"MP,"				"AttPtAvail,"		"TalentPtAvail,"		"PhysiqueAdded,"	
		"StrengthAdded,"	"PneumaAdded,"		"InnerforceAdded,"	"TechniqueAdded,"		"AgilityAdded,"	
		"Injury,"			"Vitality,"			"Knowledge,"		"Morality,"				"Culture,"	
		"Credit,"			"Identity,"			"VIPPoint,"			"BagSize,"				"BagGold,"	
		"BagSilver,"		"BagYuanBao,"		"GuildID,"			"0,"					"0,"	
		"0,"				"CreateTime,"		"LoginTime,"		"LogoutTime,"			"OnlineTime,"	
		"CurOnlineTime,"	"Rage,"				"Endurance,"		"SGFlag,"				"CloseSGTime,"

		"TalentType1,"		"TalentType2,"		"TalentType3,"		"TalentType4,"			"TalentVal1,"		
		"TalentVal2,"		"TalentVal3,"		"TalentVal4,"		"RebornMapID,"			"TotalTax,"			
		"Morale,"			"GetMallFreeTime,"	"RemoteOpenSet,"	"CurTitleID,"			"ExchangeVolume,"	
		"RoleID,"			"AvatarEquip,"		"RemoveFlag,"		"RemoveTime,"			"AccountID,"		
		"ScriptData,"		"TreasureSum,"		"StallLevel,"		"StallDailyExp,"		"StallCurExp,"	
		"StallLastTime,"

		"Hostility,"		"DeadUnSetSafeGuardCountDown,"		"ItemTransportMapID,"	"ItemTransportX,"	
		"ItemTransportZ,"	"ItemTransportY,"	"Strength," 		"StrengthNum,"			"Heroism,"
		"Wit,"				"Errantry,"			"Valor,"			"LoverID,"				"HaveWedding,"
		"LastLessingTime,"	"LastLessingLevel,"	"NeedPrisonRevive");
}
//skill
VOID CLoongLoongDBCtrl::FormatQuerySkill(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("skill","RoleID,ID,BiddenLevel,SelfLevel,Proficiency,CoolDown");

}
//task
VOID CLoongLoongDBCtrl::FormatQueryTask(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("task","TaskID,StartTime,TaskFlag,Monster0Num,Monster1Num,Monster2Num,"
		"Monster3Num,Monster4Num,Monster5Num,ScriptData, DynamicTarget, RoleID");
}
//task done
VOID CLoongLoongDBCtrl::FormatQueryTaskDone(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("task_done", "TaskID,Times,StartTime,RoleID");
}
//title
VOID CLoongLoongDBCtrl::FormatQueryTitle(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("title", "ID,Count,RoleID");
}
//名帖
VOID CLoongLoongDBCtrl::FormatQueryVisitingCard(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("visiting_card", "Level,Job,MateRoleID,FactionID,Position,Visibility,Sex,Constellation,ChineseZodiac,Area,Province,City,HeadUrl,Signature, RoleID");
}
//元宝
VOID CLoongLoongDBCtrl::FormatQueryYBAccount(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("ybaccount", "RoleID,YuanBao,Gold,Silver,bSellOrder,bBuyOrder");
}
//元宝交易订单
VOID CLoongLoongDBCtrl::FormatQueryYuanBaoOrder(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("yuanbaoorder", "dwID,RoleID,OrderType,Price,Number,OrderMode,StartTime,EndTime,AvgPrice,DealNum");
}
//bill item
VOID CLoongLoongDBCtrl::FormatQueryBillItem(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("bill_item", "token_id,account_id,item_id,item_num");
}
//bill yuan bao
VOID CLoongLoongDBCtrl::FormatQueryBillYuanbao(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("bill_yuanbao", "token_id,account_id,yuanbao");
}
//bill yuan bao log
VOID CLoongLoongDBCtrl::FormatQueryBillYuanbaoLog(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("bill_yuanbao_log", "token_id,account_id,yuanbao,time");
}

//pet_data
VOID CLoongLoongDBCtrl::FormatQueryPetData(Beton::MyStream *pStream,EDBType eDBType)
{

	pStream->SetSelect("pet_data",
		//	0			1			2			3
		"pet_id,		pet_name,	master_id,	type_id,"
		//	4			5			6			7
		"quality,		aptitude,	potential,	cur_spirit,"
		//	8			9			10			11
		"cur_exp,		step,		grade,		talent_count,	"
		//	12			13			14
		"wuxing_energy,	pet_state,	pet_lock");
}
//pet skill
VOID CLoongLoongDBCtrl::FormatQueryPetSkill(Beton::MyStream *pStream,EDBType eDBType)
{
	//			
	pStream->SetSelect("pet_skill", "petskill_typeid,	petid,	para1,	para2");
}

//vip_stall
VOID CLoongLoongDBCtrl::FormatQueryVipStall(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("vip_stall", "StallID,RoleID,RemainTime");
}
//guild_commodity
VOID CLoongLoongDBCtrl::FormatQueryGuildCommodity(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("guild_commodity", "role_id,guild_id,role_level,tael,goods");
}
//guild_skill
VOID CLoongLoongDBCtrl::FormatQueryGuildSkill(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("guild_skill", "guild_id,skill_id,progress,level,researching");
}
//vip_netbar
VOID CLoongLoongDBCtrl::FormatQueryVipBar(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("vip_netbar", "accountid,login_time");
}
//guild_upgrade
VOID CLoongLoongDBCtrl::FormatQueryGuildUpgrade(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("guild_upgrade",
		"guild_id,"			"type,"			"level,"			"progress,"		"item_type_1,"
		"item_neednum_1,"	"item_type_2,"	"item_neednum_2,"	"item_type_3,"	"item_neednum_3,"
		"item_type_4,"		"item_neednum_4");
}
//commerce_rank
VOID CLoongLoongDBCtrl::FormatQueryCommerceRank(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("commerce_rank",
		"role_id,"			"guild_id,"			"times,"			"tael");
}
//activity
VOID CLoongLoongDBCtrl::FormatQueryActivity(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("activity","dwID,"			"ScriptData");
}
//留言
VOID CLoongLoongDBCtrl::FormatQueryLeftMsg(Beton::MyStream *pStream,EDBType eDBType)
{
	pStream->SetSelect("left_msg","msg_id,"	"roleid," "msg_data");
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//按用户查询item表
VOID CLoongLoongDBCtrl::FormatAccountQueryItem(Beton::MyStream *pStream,DWORD dwAccountID, EDBType eDBType)
{
	pStream->SetSelect("item",
		"SerialNum,"	"Num,"				"TypeID,"		"Bind,"				"LockStat,"
		"UseTimes,"		"FirstGainTime,"	"CreateMode,"	"CreateID,"			"CreatorID,"	
		"CreateTime,"	"OwnerID,"			"AccountID,"	"ContainerTypeID,"	"Suffix,"
		"NameID");

	pStream->SetWhere();

	pStream->FillString("AccountID = ") << dwAccountID;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//分页查询
VOID CLoongLoongDBCtrl::FormatPagingQueryEquip(Beton::MyStream *pStream,INT64 n64Limit, EDBType eDBType)
{
	pStream->SetSelect("equip",
		"SerialNum,"	"Quality,"			"PotValUsed,"	"MinUseLevel,"	"MaxUseLevel,"	
		"WuHun,"		"MinDmg,"			"MaxDmg,"		"Armor,"		"PotVal,"		
		"RoleAttEffect,""PosyTimes,"		"PosyEffect,"	"EngraveTimes,"	"EngraveAtt,"
		"HoleNum,"		"HoleGemID,"		"BrandLevel,"	"DerateVal,"	"LongFuLevel,"
		"WuXing,"		"CanCut,"			"LongInnerID,"	"LongOuterID,"	"SpecAtt,"	
		"Appearance,"	"Rein,"				"Savvy,"		"Fortune,"		"ColorID,"	
		"QltyModPct,"	"QltyModPctEx,"		"AttALimMod,"	"AttALimModPct,""FlareVal,"
		"PotValModPct," "PotIncTimes,"		"PurpleQltyIdfPct");
	/*
	//排序分页
	pStream->FillString(" join ( select SerialNum from equip");
	pStream->FillString(" order by SerialNum desc limit ")<<n64Limit;
	pStream->FillString(",")<<DB_PAGING;//分页
	pStream->FillString(")");

	pStream->FillString("as equip2 using(SerialNum)");
	*/
	pStream->FillString(" limit ")<<n64Limit;
	pStream->FillString(",")<<DB_PAGING;//分页
}

VOID CLoongLoongDBCtrl::FormatPagingQueryEquipDel(Beton::MyStream *pStream,INT64 n64Limit, EDBType eDBType)
{
	pStream->SetSelect("equip_del",
		"SerialNum,"	"Quality,"			"PotValUsed,"	"MinUseLevel,"	"MaxUseLevel,"	
		"WuHun,"		"MinDmg,"			"MaxDmg,"		"Armor,"		"PotVal,"		
		"RoleAttEffect,""PosyTimes,"		"PosyEffect,"	"EngraveTimes,"	"EngraveAtt,"
		"HoleNum,"		"HoleGemID,"		"BrandLevel,"	"DerateVal,"	"LongFuLevel,"
		"WuXing,"		"CanCut,"			"LongInnerID,"	"LongOuterID,"	"SpecAtt,"	
		"Appearance,"	"Rein,"				"Savvy,"		"Fortune,"		"ColorID,"	
		"QltyModPct,"	"QltyModPctEx,"		"AttALimMod,"	"AttALimModPct,""FlareVal,"
		"PotValModPct," "PotIncTimes,"		"PurpleQltyIdfPct" );

	pStream->FillString(" limit ")<<n64Limit;
	pStream->FillString(",")<<DB_PAGING;//分页
}

//分页查询
VOID CLoongLoongDBCtrl::FormatPagingQueryTaskDone(Beton::MyStream *pStream,INT64 n64Limit, EDBType eDBType)
{
	pStream->SetSelect("task_done", "TaskID,Times,StartTime,RoleID");

	pStream->FillString(" limit ")<<n64Limit;
	pStream->FillString(",")<<DB_PAGING;//分页
}

//分页查询
VOID CLoongLoongDBCtrl::FormatPagingQueryTask(Beton::MyStream *pStream,INT64 n64Limit, EDBType eDBType)
{
	pStream->SetSelect("task","TaskID,StartTime,TaskFlag,Monster0Num,Monster1Num,Monster2Num,"
		"Monster3Num,Monster4Num,Monster5Num,ScriptData, DynamicTarget, RoleID");

	pStream->FillString(" limit ")<<n64Limit;
	pStream->FillString(",")<<DB_PAGING;//分页
}

//分页查询
VOID CLoongLoongDBCtrl::FormatPagingQueryItemDel(Beton::MyStream *pStream,INT64 n64Limit, EDBType eDBType)
{
	pStream->SetSelect("item_del",
		"SerialNum,"	"Num,"				"TypeID,"		"Bind,"				"LockStat,"
		"UseTimes,"		"FirstGainTime,"	"CreateMode,"	"CreateID,"			"CreatorID,"	
		"CreateTime,"	"OwnerID,"			"AccountID,"	"ContainerTypeID,"	"Suffix,"
		"NameID");

	pStream->FillString(" limit ")<<n64Limit;
	pStream->FillString(",")<<DB_PAGING;//分页
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BOOL CLoongLoongDBCtrl::FormatInsertAccountCommon(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_AccountCommon*>);
	std::vector <tagTWrap_Loong_AccountCommon*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("account_common");
		pStream->FillString("AccountID=") << (*it)->dwAccountID;
		pStream->FillString(",AccountName='").FillString((*it)->szAccountName, pCon).FillString("'");
		pStream->FillString(",SafeCodeCrc=") << (*it)->stOthers.sSafeCode.dwSafeCodeCrc;
		pStream->FillString(",ResetTime=") << (*it)->stOthers.sSafeCode.dwTimeReset;
		pStream->FillString(",BagPsdCrc=") << (*it)->stOthers.dwBagPsdCrc;
		pStream->FillString(",BaiBaoYuanBao=") << (*it)->stOthers.nBaiBaoYuanBao;
		pStream->FillString(",WareSize=") << (*it)->stOthers.n16WareSize;
		pStream->FillString(",WareSilver=") << (*it)->stOthers.n64WareSilver;


		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}

	m_LoongDB[eDBType].ReturnConnection(pCon);

	return TRUE;
}
BOOL CLoongLoongDBCtrl::FormatInsertBlackList(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_BlackList*>);
	std::vector <tagTWrap_Loong_BlackList*>::iterator it = p->begin();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("blacklist");
		pStream->FillString("RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",BlackID=") << (*it)->dwBlackID;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
BOOL CLoongLoongDBCtrl::FormatInsertBuff(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_Buff*>);
	std::vector <tagTWrap_Loong_Buff*>::iterator it = p->begin();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("buff");
		pStream->FillString("RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",SrcUnitID=") << (*it)->stOthers.dwSrcUnitID;
		pStream->FillString(",SrcSkillID=") << (*it)->stOthers.dwSrcSkillID;
		pStream->FillString(",ItemTypeID=") << (*it)->stOthers.dwItemTypeID;
		pStream->FillString(",ItemSerialID=") << (*it)->stOthers.n64Serial;

		pStream->FillString(",BuffID=") << (*it)->stOthers.dwBuffID;
		pStream->FillString(",CurTick=") << (*it)->stOthers.nPersistTick;
		pStream->FillString(",Level=") << (*it)->stOthers.n8Level;
		pStream->FillString(",CurLapTimes=") << (*it)->stOthers.n8CurLapTimes;

		if((*it)->stOthers.n8ModifierNum > 0)
		{
			pStream->FillString(",EffectSkillID='");
			pStream->FillBlob((*it)->pcBlob, (*it)->stOthers.n8ModifierNum * sizeof(DWORD), pCon);
			pStream->FillString("'");
		}

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;
}
BOOL CLoongLoongDBCtrl::FormatInsertClanData(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_ClanData*>);
	std::vector <tagTWrap_Loong_ClanData*>::iterator it = p->begin();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("clan_data");
		pStream->FillString("RoleID=")		<< (*it)->dwRoleID;
		pStream->FillString(",RepXuanYuan=")		<< (*it)->stOthers.m_nReputation[ECLT_XuanYuan];
		pStream->FillString(",ConXuanYuan=")	<< (*it)->stOthers.m_nContribution[ECLT_XuanYuan];
		pStream->FillString(",ActCntXuanYuan=")	<< (*it)->stOthers.m_n8ActCount[ECLT_XuanYuan];

		pStream->FillString(",RepShenNong=")	<< (*it)->stOthers.m_nReputation[ECLT_ShenNong];
		pStream->FillString(",ConShenNong=")	<< (*it)->stOthers.m_nContribution[ECLT_ShenNong];
		pStream->FillString(",ActCntShenNong=")	<< (*it)->stOthers.m_n8ActCount[ECLT_ShenNong];

		pStream->FillString(",RepFuXi=")		<< (*it)->stOthers.m_nReputation[ECLT_FuXi];
		pStream->FillString(",ConFuXi=")		<< (*it)->stOthers.m_nContribution[ECLT_FuXi];
		pStream->FillString(",ActCntFuXi=")		<< (*it)->stOthers.m_n8ActCount[ECLT_FuXi];

		pStream->FillString(",RepSanMiao=")		<< (*it)->stOthers.m_nReputation[ECLT_SanMiao];
		pStream->FillString(",ConSanMiao=")		<< (*it)->stOthers.m_nContribution[ECLT_SanMiao];
		pStream->FillString(",ActCntSanMiao=")	<< (*it)->stOthers.m_n8ActCount[ECLT_SanMiao];

		pStream->FillString(",RepJiuLi=")		<< (*it)->stOthers.m_nReputation[ECLT_JiuLi];
		pStream->FillString(",ConJiuLi=")		<< (*it)->stOthers.m_nContribution[ECLT_JiuLi];
		pStream->FillString(",ActCntJiuLi=")	<< (*it)->stOthers.m_n8ActCount[ECLT_JiuLi];

		pStream->FillString(",RepYueZhi=")		<< (*it)->stOthers.m_nReputation[ECLT_YueZhi];
		pStream->FillString(",ConYueZhi=")		<< (*it)->stOthers.m_nContribution[ECLT_YueZhi];
		pStream->FillString(",ActCntYueZhi=")	<< (*it)->stOthers.m_n8ActCount[ECLT_YueZhi];

		pStream->FillString(",RepNvWa=")		<< (*it)->stOthers.m_nReputation[ECLT_NvWa];
		pStream->FillString(",ConNvWa=")		<< (*it)->stOthers.m_nContribution[ECLT_NvWa];
		pStream->FillString(",ActCntNvWa=")		<< (*it)->stOthers.m_n8ActCount[ECLT_NvWa];

		pStream->FillString(",RepGongGong=")	<< (*it)->stOthers.m_nReputation[ECLT_GongGong];
		pStream->FillString(",ConGongGong=")	<< (*it)->stOthers.m_nContribution[ECLT_GongGong];
		pStream->FillString(",ActCntGongGong=")	<< (*it)->stOthers.m_n8ActCount[ECLT_GongGong];

		pStream->FillString(",FameMask=")		<< (*it)->stOthers.m_u16FameMask;
		pStream->FillString(",RepRstTimeStamp=")<< (*it)->stOthers.m_dwtLastResetTime;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//enemy
BOOL CLoongLoongDBCtrl::FormatInsertEnemy(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_Enemy*>);
	std::vector <tagTWrap_Loong_Enemy*>::iterator it = p->begin();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("enemy");
		pStream->FillString("RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",EnemyID=") << (*it)->dwEnemyID;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//武器 songg
BOOL CLoongLoongDBCtrl::FormatInsertEquip(Beton::MyStream *pStream,LPVOID &pData,LPCSTR pszTable,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_Equip*>);
	std::vector <tagTWrap_Loong_Equip*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert(pszTable);
		pStream->FillString("SerialNum=") << (*it)->n64SerialNum;
		pStream->FillString(",Quality=") << (*it)->stOthers.equipSpec.byQuality;
		pStream->FillString(",MinUseLevel=") << (*it)->stOthers.equipSpec.byMinUseLevel;
		pStream->FillString(",MaxUseLevel=") << (*it)->stOthers.equipSpec.byMaxUseLevel;
		pStream->FillString(",AttALimMod=") << (*it)->stOthers.equipSpec.n16AttALimMod;
		pStream->FillString(",AttALimModPct=") << (*it)->stOthers.equipSpec.n16AttALimModPct;

		pStream->FillString(",WuHun=") << (*it)->stOthers.equipSpec.n16WuHun;
		pStream->FillString(",MinDmg=") << (*it)->stOthers.equipSpec.n16MinDmg;
		pStream->FillString(",MaxDmg=") << (*it)->stOthers.equipSpec.n16MaxDmg;
		pStream->FillString(",Armor=") << (*it)->stOthers.equipSpec.n16Armor;
		pStream->FillString(",PotVal=") << (*it)->stOthers.equipSpec.nPotVal;
		pStream->FillString(",PotValUsed=") << (*it)->stOthers.equipSpec.nPotValUsed;

		pStream->FillString(",PosyTimes=") << (*it)->stOthers.equipSpec.byPosyTimes;
		pStream->FillString(",EngraveTimes=") << (*it)->stOthers.equipSpec.byEngraveTimes;
		pStream->FillString(",HoleNum=") << (*it)->stOthers.equipSpec.byHoleNum;
		pStream->FillString(",BrandLevel=") << (*it)->stOthers.equipSpec.byBrandLevel;
		pStream->FillString(",LongFuLevel=") << (*it)->stOthers.equipSpec.byLongfuLevel;

		pStream->FillString(",CanCut=") << (*it)->stOthers.equipSpec.bCanCut;
		pStream->FillString(",LongInnerID=") << (*it)->stOthers.equipSpec.dwLongHunInnerID;
		pStream->FillString(",LongOuterID=") << (*it)->stOthers.equipSpec.dwLongHunOuterID;
		pStream->FillString(",SpecAtt=") << (*it)->stOthers.equipSpec.bySpecAtt;
		pStream->FillString(",FlareVal=") << (*it)->stOthers.equipSpec.byFlareVal;
		pStream->FillString(",QltyModPct=") << (*it)->stOthers.equipSpec.n16QltyModPct;
		pStream->FillString(",QltyModPctEx=") << (*it)->stOthers.equipSpec.n16QltyModPctEx;
		pStream->FillString(",PotValModPct=") << (*it)->stOthers.equipSpec.n16PotValModPct;
			
		// 时装属性
		pStream->FillString(",Appearance=") << (*it)->stOthers.equipSpec.n16Appearance;
		pStream->FillString(",Rein=") << (*it)->stOthers.equipSpec.byRein;
		pStream->FillString(",Savvy=") << (*it)->stOthers.equipSpec.bySavvy;
		pStream->FillString(",Fortune=") << (*it)->stOthers.equipSpec.byFortune;
		pStream->FillString(",ColorID=") << (*it)->stOthers.equipSpec.n8ColorID;

		pStream->FillString(",RoleAttEffect='").FillBlob((*it)->stOthers.equipSpec.nRoleAttEffect, sizeof((*it)->stOthers.equipSpec.nRoleAttEffect), pCon);
		pStream->FillString("',PosyEffect='").FillBlob((*it)->stOthers.equipSpec.PosyEffect, sizeof((*it)->stOthers.equipSpec.PosyEffect), pCon);
		pStream->FillString("',EngraveAtt='").FillBlob((*it)->stOthers.equipSpec.nEngraveAtt, sizeof((*it)->stOthers.equipSpec.nEngraveAtt), pCon);
		pStream->FillString("',HoleGemID='").FillBlob((*it)->stOthers.equipSpec.dwHoleGemID, sizeof((*it)->stOthers.equipSpec.dwHoleGemID), pCon);
		pStream->FillString("',DerateVal='").FillBlob((*it)->stOthers.equipSpec.nBrandAtt, sizeof((*it)->stOthers.equipSpec.nBrandAtt), pCon);
		pStream->FillString("',WuXing='").FillBlob((*it)->stOthers.equipSpec.byLongfuAtt, sizeof((*it)->stOthers.equipSpec.byLongfuAtt), pCon);
		pStream->FillString("'");


		//1.3.0新增
		pStream->FillString(",PotIncTimes=") << (*it)->stOthers.equipSpec.nPotIncTimes;
		pStream->FillString(",PurpleQltyIdfPct=") << (*it)->stOthers.equipSpec.dwPurpleQltyIdfPct;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;

}
BOOL CLoongLoongDBCtrl::FormatInsertActClanTreasure(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_ActClanTreasure*>);
	std::vector <tagTWrap_Loong_ActClanTreasure*>::iterator it = p->begin();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("famehall_act_treasure");
		pStream->FillString("TreasureID=") << (*it)->u16TreasureID;
		pStream->FillString(",ActNameID=") << (*it)->dwActNameID;
		pStream->FillString(",RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",ClanType=") << (*it)->byClanType;
		pStream->FillString(",ActTime=") << (*it)->dwActTime;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//氏族进入快照
BOOL CLoongLoongDBCtrl::FormatInsertFameHallEnterSnap(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_FameHallEnterSnap*>);
	std::vector <tagTWrap_Loong_FameHallEnterSnap*>::iterator it = p->begin();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("famehall_entersnap");
		pStream->FillString("RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",EnterNameID=") << (*it)->dwEnterNameID;
		pStream->FillString(",ClanType=") << (*it)->byClanType;
		pStream->FillString(",EnterTime=") << (*it)->dwEnterTime;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//氏族声望重置时间快照
BOOL CLoongLoongDBCtrl::FormatInsertRepRstTime(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_RepRstTime*>);
	std::vector <tagTWrap_Loong_RepRstTime*>::iterator it = p->begin();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("famehall_rep_rst_timestamp");
		pStream->FillString("ClanType=") << (*it)->byClanType;
		pStream->FillString(",ResetTimeStamp=") << (*it)->dwRepRstTimeStamp;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//好友
BOOL CLoongLoongDBCtrl::FormatInsertFriendSave(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_Friend*>);
	std::vector <tagTWrap_Loong_Friend*>::iterator it = p->begin();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("friend");
		pStream->FillString("RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",FriendID=") << (*it)->stOthers.dwFriendID;
		pStream->FillString(",GroupID=") << (*it)->stOthers.nGroupID;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//好友度
BOOL CLoongLoongDBCtrl::FormatInsertFriendshipSave(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_FriendshipSave*>);
	std::vector <tagTWrap_Loong_FriendshipSave*>::iterator it = p->begin();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("friendship");
		pStream->FillString("RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",FriendID=") << (*it)->stOthers.dwFriendID;
		pStream->FillString(",FriVal=") << (*it)->stOthers.nFriVal;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//团购
BOOL CLoongLoongDBCtrl::FormatInsertGroupPurchase(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_GroupPurchase*>);
	std::vector <tagTWrap_Loong_GroupPurchase*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("group_purchase");
		pStream->FillString("GuildID=") << (*it)->stOthers.dwGuildID;
		pStream->FillString(",RoleID=") << (*it)->stOthers.dwRoleID;
		pStream->FillString(",MallID=") << (*it)->stOthers.dwMallID;
		pStream->FillString(",ItemPrice=") << (*it)->stOthers.nPrice;
		pStream->FillString(",ParticipatorNum=") << (*it)->stOthers.nParticipatorNum;
		pStream->FillString(",RequireNum=") << (*it)->stOthers.nRequireNum;
		pStream->FillString(",RemainTime=") << (*it)->stOthers.nRemainTime;

		//响应者列表，此时只有发起人
		pStream->FillString(",ParticipatorList='");
		pStream->FillBlob((*it)->pcBlob, (*it)->dwBlobSize, pCon);
		pStream->FillString("'");

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;
}
//帮派
BOOL CLoongLoongDBCtrl::FormatInsertGuild(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_Guild*>);
	std::vector <tagTWrap_Loong_Guild*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("guild");
		pStream->FillString("ID=") << (*it)->stOthers.dwID;

		if(_tcsclen((*it)->stOthers.szName) != 0)
		{
			pStream->FillString(",Name='").FillBlob((*it)->stOthers.szName, (_tcsclen((*it)->stOthers.szName)+1) * sizeof(TCHAR), pCon);
			pStream->FillString("'");
		}

		pStream->FillString(",FounderNameID=") << (*it)->stOthers.dwFounderNameID;
		pStream->FillString(",SpecState=") << (*it)->stOthers.dwSpecState;
		pStream->FillString(",Level=") << (*it)->stOthers.byLevel;
		pStream->FillString(",HoldCity0=") << (*it)->stOthers.byHoldCity[0];
		pStream->FillString(",HoldCity1=") << (*it)->stOthers.byHoldCity[1];
		pStream->FillString(",HoldCity2=") << (*it)->stOthers.byHoldCity[2];
		pStream->FillString(",Fund=") << (*it)->stOthers.nFund;
		pStream->FillString(",Material=") << (*it)->stOthers.nMaterial;
		pStream->FillString(",Rep=") << (*it)->stOthers.nReputation;
		pStream->FillString(",DailyCost=") << (*it)->stOthers.nDailyCost;
		pStream->FillString(",Peace=") << (*it)->stOthers.n16Peace;
		pStream->FillString(",Rank=") << (*it)->stOthers.n16Rank;

		pStream->FillString(",LeaderID=") << (*it)->stOthers.dwLeaderRoleID;
		pStream->FillString(",RemainSpreadTimes=") << (*it)->stOthers.byAffairRemainTimes;
		pStream->FillString(",Commendation=") << (*it)->stOthers.bCommendation;

		if(_tcsclen((*it)->stOthers.szTenet) != 0)
		{
			pStream->FillString(",Tenet='").FillBlob((*it)->stOthers.szTenet, (_tcsclen((*it)->stOthers.szTenet)+1) * sizeof(TCHAR), pCon);
			pStream->FillString("'");
		}

		if(_tcsclen((*it)->stOthers.szSymbolURL) != 0)
		{
			pStream->FillString(",Symbol='").FillBlob((*it)->stOthers.szSymbolURL, (_tcsclen((*it)->stOthers.szSymbolURL)+1) * sizeof(TCHAR), pCon);
			pStream->FillString("'");
		}

		pStream->FillString(",CreateTime='").FillString((*it)->szDate);
		pStream->FillString("'");
		pStream->FillString(",GroupPurchase=") << (*it)->stOthers.nGroupPurchase;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;
}
//帮派成员
BOOL CLoongLoongDBCtrl::FormatInsertGuildMem(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_GuildMem*>);
	std::vector <tagTWrap_Loong_GuildMem*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("guild_member");
		pStream->FillString("RoleID=") << (*it)->stOthers.sGuildMember.dwRoleID;
		pStream->FillString(",GuildID=") << (*it)->stOthers.dwGuildID;
		pStream->FillString(",GuildPos=") << (*it)->stOthers.sGuildMember.eGuildPos;
		pStream->FillString(",TotalContrib=") << (*it)->stOthers.sGuildMember.nTotalContribution;
		pStream->FillString(",CurContrib=") << (*it)->stOthers.sGuildMember.nContribution;	
		pStream->FillString(",Exploit=") << (*it)->stOthers.sGuildMember.nExploit;
		pStream->FillString(",Salary=") << (*it)->stOthers.sGuildMember.nSalary;

		pStream->FillString(",JoinTime='").FillString((*it)->szDate);
		pStream->FillString("'");

		pStream->FillString(",CanUseGuildWare=") << (*it)->stOthers.sGuildMember.bUseGuildWare;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;
}
//item songg
BOOL CLoongLoongDBCtrl::FormatInsertItem(Beton::MyStream *pStream,LPVOID &pData,LPCSTR pszTable,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_Item*>);
	std::vector <tagTWrap_Loong_Item*>::iterator it = p->begin();

	CHAR szTime[X_DATATIME_LEN + 1];

	for(;it != p->end(); ++it)
	{
		pStream->SetInsert(pszTable);
		pStream->FillString("SerialNum=") << (*it)->stOthers.n64Serial;
		pStream->FillString(",TypeID=") << (*it)->stOthers.dwTypeID;

		pStream->FillString(",Num=") << (*it)->stOthers.n16Num;
		pStream->FillString(",Bind=") << (*it)->stOthers.byBind;
		pStream->FillString(",LockStat=") << (*it)->stOthers.bLock;
		pStream->FillString(",UseTimes=") << (*it)->stOthers.nUseTimes;

		pStream->FillString(",CreateMode=") << (*it)->stOthers.eCreateMode;
		pStream->FillString(",CreateID=") << (*it)->stOthers.dwCreateID;

		pStream->FillString(",CreatorID=") << (*it)->stOthers.dwCreatorID;
		pStream->FillString(",NameID=") << (*it)->stOthers.dwNameID;

		DwordTime2DataTime(szTime, sizeof(szTime), (*it)->stOthers.dwCreateTime);
		pStream->FillString(",CreateTime='").FillString(szTime);
		pStream->FillString("'");

		DwordTime2DataTime(szTime, sizeof(szTime), (*it)->stOthers.dw1stGainTime);
		pStream->FillString(",FirstGainTime='").FillString(szTime);
		pStream->FillString("'");

		pStream->FillString(",OwnerID=") << (*it)->stOthers.dwOwnerID;
		pStream->FillString(",AccountID=") << (*it)->stOthers.dwAccountID;
		pStream->FillString(",ContainerTypeID=") << (*it)->stOthers.eConType;
		pStream->FillString(",Suffix=") << (*it)->stOthers.n16Index;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}

	}
	return TRUE;

}
//item cd time
BOOL CLoongLoongDBCtrl::FormatInsertItemCDTime(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_ItemCDTime*>);
	std::vector <tagTWrap_Loong_ItemCDTime*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("item_cdtime");
		pStream->FillString("RoleID=") << (*it)->dwRoleID;

		if((*it)->vecCDTime.size())
		{
			pStream->FillString(",CDTime='");
			pStream->FillBlob(&(*it)->vecCDTime, sizeof(tagCDTime) * (*it)->vecCDTime.size(), pCon);
			pStream->FillString("'");
		}

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;
}
//item need log
BOOL CLoongLoongDBCtrl::FormatInsertItemNeedLog(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_ItemNeedLog*>);
	std::vector <tagTWrap_Loong_ItemNeedLog*>::iterator it = p->begin();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("item_needlog");
		pStream->FillString("TypeID=") << (*it)->dwTypeID;
		pStream->FillString(",NeedLog=") << (*it)->bNeedLog;
		pStream->FillString(",MinQlty=") << (*it)->sMinQlty;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//item log baibao
BOOL CLoongLoongDBCtrl::FormatInsertLogBaiBao(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_LogBaiBao*>);
	std::vector <tagTWrap_Loong_LogBaiBao*>::iterator it = p->begin();

	CHAR szTime[X_DATATIME_LEN + 1];
	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("log_baibao");
		pStream->FillString("AccountID=") << (*it)->dwAccountID;
		pStream->FillString(",RoleID=") << (*it)->stOthers.dwRoleID;

		// LoongDB设置时间
		if(DwordTime2DataTime(szTime, sizeof(szTime), (*it)->stOthers.dwTime))
		{
			pStream->FillString(",LogTime='").FillString(szTime);
			pStream->FillString("'");
		}

		pStream->FillString(",n16Type=") << (*it)->stOthers.n16Type;
		pStream->FillString(",TypeID=") << (*it)->stOthers.dwTypeID;

		//赠言
		pStream->FillString(",LeaveWords='");
		if((*it)->ptcWords != NULL)
		{
			INT nLen = (_tcsclen((*it)->ptcWords)+1) * sizeof(TCHAR);
			if (nLen > 0)
			{
				pStream->FillBlob((*it)->ptcWords, (*it)->stOthers.n16Size-sizeof(tagBaiBaoRecord)+sizeof((*it)->stOthers.szWords), pCon);
			}

		}
		pStream->FillString("'");
		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;

}
//item role del
BOOL CLoongLoongDBCtrl::FormatInsertRoleDel(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_RoleDel*>);
	std::vector <tagTWrap_Loong_RoleDel*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("role_del");
		pStream->FillString("AccountID=") << (*it)->dwAccountID;
		pStream->FillString(",RoleName='").FillString((*it)->szRoleName, pCon);
		pStream->FillString("',RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",IP='").FillString((*it)->szIP);
		pStream->FillString("',DeleteTime='").FillString((*it)->szDeleteTime).FillString("'");

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;

}
//item roledata 
BOOL CLoongLoongDBCtrl::FormatInsertRoleData(Beton::MyStream *pStream,LPVOID &pData,LPCSTR pszTable,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_RoleData*>);
	std::vector <tagTWrap_Loong_RoleData*>::iterator it = p->begin();

	CHAR szTime[X_DATATIME_LEN + 1];
	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		tagRoleDataConst	*pRoleDataConst = &(*it)->stOthers.RoleDataConst;
		tagRoleDataSave		*pRoleDataSave	= &(*it)->stOthers.RoleDataSave;

		pStream->SetInsert(pszTable);

		pStream->FillString("RoleName='").FillString(pRoleDataConst->szRoleName, pCon);
		pStream->FillString("',RoleNameCrc=") << pRoleDataConst->dwRoleNameCrc;
		pStream->FillString(",Sex=") << pRoleDataConst->Avatar.bySex;
		pStream->FillString(",HairModelID=") << pRoleDataConst->Avatar.wHairMdlID;
		pStream->FillString(",HairColorID=") << pRoleDataConst->Avatar.wHairTexID;
		pStream->FillString(",FaceModelID=") << pRoleDataConst->Avatar.wFaceMdlID;
		pStream->FillString(",FaceDetailID=") << pRoleDataConst->Avatar.wFaceDetailTexID;
		pStream->FillString(",DressModelID=") << pRoleDataConst->Avatar.wDressMdlID;

		pStream->FillString(",DisplaySet=") << pRoleDataSave->DisplaySet;
		pStream->FillString(",MapID=") << pRoleDataSave->dwMapID;
		pStream->FillString(",X=") << pRoleDataSave->fCoordinate[0];			// 坐标
		pStream->FillString(",Y=") << pRoleDataSave->fCoordinate[1];
		pStream->FillString(",Z=") << pRoleDataSave->fCoordinate[2];
		pStream->FillString(",FaceX=") << pRoleDataSave->fFace[0];	// 朝向
		pStream->FillString(",FaceY=") << pRoleDataSave->fFace[1];
		pStream->FillString(",FaceZ=") << pRoleDataSave->fFace[2];

		pStream->FillString(",Class=") << pRoleDataSave->eClass;
		pStream->FillString(",ClassEx=") << pRoleDataSave->eClassEx;
		pStream->FillString(",Level=") << pRoleDataSave->nLevel;
		pStream->FillString(",ExpCurLevel=") << pRoleDataSave->nCurExp;
		pStream->FillString(",HP=") << pRoleDataSave->nHP;
		pStream->FillString(",MP=") << pRoleDataSave->nMP;

		pStream->FillString(",AttPtAvail=") <<pRoleDataSave->nAttPoint;
		pStream->FillString(",TalentPtAvail=") <<pRoleDataSave->nTalentPoint;

		pStream->FillString(",PhysiqueAdded=") <<pRoleDataSave->nAttPointAdd[ERA_Physique];
		pStream->FillString(",StrengthAdded=") <<pRoleDataSave->nAttPointAdd[ERA_Strength];
		pStream->FillString(",PneumaAdded=") <<pRoleDataSave->nAttPointAdd[ERA_Pneuma];
		pStream->FillString(",InnerforceAdded=") <<pRoleDataSave->nAttPointAdd[ERA_InnerForce];
		pStream->FillString(",TechniqueAdded=") <<pRoleDataSave->nAttPointAdd[ERA_Technique];
		pStream->FillString(",AgilityAdded=") <<pRoleDataSave->nAttPointAdd[ERA_Agility];

		pStream->FillString(",Injury=") <<pRoleDataSave->nInjury;
		pStream->FillString(",Vitality=") <<pRoleDataSave->nVitality;
		pStream->FillString(",Knowledge=") <<pRoleDataSave->nKnowledge;
		pStream->FillString(",Morality=") <<pRoleDataSave->nMorality;
		pStream->FillString(",Culture=") <<pRoleDataSave->nCulture	;
		pStream->FillString(",Credit=") <<pRoleDataSave->nCredit;
		pStream->FillString(",Identity=") <<pRoleDataSave->nIdentity;
		pStream->FillString(",VIPPoint=") <<pRoleDataSave->nVIPPoint;

		pStream->FillString(",BagSize=") <<pRoleDataSave->n16BagSize;
		pStream->FillString(",BagGold=") <<pRoleDataSave->nBagGold;
		pStream->FillString(",BagSilver=") <<pRoleDataSave->nBagSilver;
		pStream->FillString(",BagYuanBao=") <<pRoleDataSave->nBagYuanBao;

		pStream->FillString(",GuildID=") <<pRoleDataSave->dwGuildID;

		if(DwordTime2DataTime(szTime, sizeof(szTime), pRoleDataSave->LoginTime))
		{
			pStream->FillString(",LoginTime='").FillString(szTime);
			pStream->FillString("'");

			DwordTime2DataTime(szTime, sizeof(szTime), pRoleDataSave->LogoutTime);
			pStream->FillString(",LogoutTime='").FillString(szTime);
			pStream->FillString("'");

			DwordTime2DataTime(szTime, sizeof(szTime), pRoleDataSave->CloseSafeGuardTime);
			pStream->FillString(",CloseSGTime='").FillString(szTime);
			pStream->FillString("'");

			DwordTime2DataTime(szTime, sizeof(szTime), pRoleDataSave->dwTimeGetMallFree);
			pStream->FillString(",GetMallFreeTime='").FillString(szTime);
			pStream->FillString("'");

			DwordTime2DataTime(szTime, sizeof(szTime), pRoleDataConst->CreateTime);
			pStream->FillString(",CreateTime='").FillString(szTime);
			pStream->FillString("'");
		}

		pStream->FillString(",OnlineTime=") << pRoleDataSave->nOnlineTime;
		pStream->FillString(",CurOnlineTime=") << pRoleDataSave->nCurOnlineTime;

		pStream->FillString(",Rage=") <<pRoleDataSave->nRage;
		pStream->FillString(",Endurance=") <<pRoleDataSave->nEndurance;
		pStream->FillString(",SGFlag=") <<pRoleDataSave->bSafeGuard;


		pStream->FillString(",TalentType1=") <<pRoleDataSave->talent[0].eType;
		pStream->FillString(",TalentType2=") <<pRoleDataSave->talent[1].eType;
		pStream->FillString(",TalentType3=") <<pRoleDataSave->talent[2].eType;
		pStream->FillString(",TalentType4=") <<pRoleDataSave->talent[3].eType;
		pStream->FillString(",TalentVal1=") <<pRoleDataSave->talent[0].nPoint;
		pStream->FillString(",TalentVal2=") <<pRoleDataSave->talent[1].nPoint;
		pStream->FillString(",TalentVal3=") <<pRoleDataSave->talent[2].nPoint;
		pStream->FillString(",TalentVal4=") <<pRoleDataSave->talent[3].nPoint;

		pStream->FillString(",RebornMapID=") <<pRoleDataSave->dwRebornMapID;
		pStream->FillString(",TotalTax=") <<pRoleDataSave->nTotalTax;
		pStream->FillString(",Morale=") <<pRoleDataSave->nMorale;

		pStream->FillString(",RemoteOpenSet=") <<pRoleDataSave->sRemoteOpenSet;
		pStream->FillString(",CurTitleID=") <<pRoleDataSave->u16ActiveTitleID;
		pStream->FillString(",ExchangeVolume=") <<pRoleDataSave->nExVolume;

		pStream->FillString(",RoleID=") <<(*it)->dwRoleID;

		pStream->FillString(",AvatarEquip='").FillBlob(&pRoleDataSave->AvatarEquip, SIZE_AVATAR_EQUIP, pCon).FillString("'");

		pStream->FillString(",RemoveFlag=") <<(*it)->bRemoveFlag;
		pStream->FillString(",RemoveTime='").FillString((*it)->szRemoveTime);
		pStream->FillString("'");
		pStream->FillString(",AccountID=") <<(*it)->dwAccountID;

		pStream->FillString(",ScriptData='");
		pStream->FillBlob(pRoleDataSave->Data.dwData, sizeof(DWORD)*ESD_Role, pCon);
		pStream->FillString("'");

		pStream->FillString(",TreasureSum=") <<pRoleDataSave->nTreasureSum;


		pStream->FillString(",StallLevel=") <<pRoleDataSave->byStallLevel;
		pStream->FillString(",StallDailyExp=") <<pRoleDataSave->nStallDailyExp;
		pStream->FillString(",StallCurExp=") <<pRoleDataSave->nStallCurExp;
		pStream->FillString(",StallLastTime=") <<pRoleDataSave->dwLastStallTime;


		pStream->FillString(",Hostility=") << pRoleDataSave->nHostility;
		pStream->FillString(",DeadUnSetSafeGuardCountDown=") << pRoleDataSave->nDeadUnSetSafeGuardCountDown;
		pStream->FillString(",ItemTransportMapID=") << pRoleDataSave->dwItemTransportMapID;
		pStream->FillString(",ItemTransportX=") << pRoleDataSave->fItemTransportX;
		pStream->FillString(",ItemTransportZ=") << pRoleDataSave->fItemTransportZ;
		pStream->FillString(",ItemTransportY=") << pRoleDataSave->fItemTransportY;

		//1.3.0 新增
		//"Strength," 		"StrengthNum,"	"LoverID,"				"HaveWedding,"
		//"LastLessingTime,"	"LastLessingLevel,"	"NeedPrisonRevive"
		pStream->FillString(",Strength=") << pRoleDataSave->dwStrength;
		pStream->FillString(",StrengthNum=") << pRoleDataSave->nStrengthNum;
		pStream->FillString(",LoverID=") << pRoleDataSave->dwLoverID;
		pStream->FillString(",HaveWedding=") << pRoleDataSave->bHaveWedding;
		pStream->FillString(",LastLessingTime=") << pRoleDataSave->timeLastLessing;
		pStream->FillString(",LastLessingLevel=") << pRoleDataSave->nLessingLevel;
		pStream->FillString(",NeedPrisonRevive=") << pRoleDataSave->bNeedPrisonRevive;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;
}
//skill
BOOL CLoongLoongDBCtrl::FormatInsertSkill(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_Skill*>);
	std::vector <tagTWrap_Loong_Skill*>::iterator it = p->begin();

	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("skill");
		pStream->FillString("RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",ID=") << (*it)->stOthers.dwID;
		pStream->FillString(",BiddenLevel=") << (*it)->stOthers.nLearnLevel;
		pStream->FillString(",SelfLevel=") << (*it)->stOthers.nSelfLevel;
		pStream->FillString(",Proficiency=") << (*it)->stOthers.nProficiency;
		pStream->FillString(",CoolDown=") << (*it)->stOthers.nCoolDown;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//task
BOOL CLoongLoongDBCtrl::FormatInsertTask(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_Task*>);
	std::vector <tagTWrap_Loong_Task*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("task");
		pStream->FillString("RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",TaskID=") << (*it)->stOthers.u16QuestID;
		pStream->FillString(",TaskFlag=") << (*it)->stOthers.dwQuestFlag;
		pStream->FillString(",StartTime=") << (*it)->stOthers.dwAcceptTime;

		for(INT32 i=0; i<QUEST_CREATURES_COUNT; ++i)
		{
			pStream->FillString(",Monster") << i;
			pStream->FillString("Num=") << (INT16)(*it)->stOthers.n16MonsterNum[i];
		}

		pStream->FillString(",ScriptData='");
		pStream->FillBlob((*it)->stOthers.Data.dwData, sizeof(DWORD)*ESD_Quest, pCon);
		pStream->FillString("'");

		if((*it)->stOthers.DynamicTarget.eTargetType != EQTT_NULL)
		{
			pStream->FillString(",DynamicTarget='");
			pStream->FillBlob(&(*it)->stOthers.DynamicTarget, sizeof(tagQuestDynamicTarget), pCon);
			pStream->FillString("'");
		}

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;
}
//task done
BOOL CLoongLoongDBCtrl::FormatInsertTaskDone(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_TaskDone*>);
	std::vector <tagTWrap_Loong_TaskDone*>::iterator it = p->begin();

	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("task_done");
		pStream->FillString("RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",TaskID=") << (*it)->stOthers.u16QuestID;
		pStream->FillString(",StartTime=") << (*it)->stOthers.dwStartTime;
		pStream->FillString(",Times=") << (*it)->stOthers.nTimes;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//title
BOOL CLoongLoongDBCtrl::FormatInsertTitle(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_Title*>);
	std::vector <tagTWrap_Loong_Title*>::iterator it = p->begin();

	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("title");
		pStream->FillString("RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",ID=") << (*it)->stOthers.u16TitleID;
		pStream->FillString(",Count=") << (*it)->stOthers.dwStateMark;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//名帖
BOOL CLoongLoongDBCtrl::FormatInsertVisitingCard(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_VisitingCard*>);
	std::vector <tagTWrap_Loong_VisitingCard*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("visiting_card");
		pStream->FillString("Level=") << (INT32)(*it)->stOthers.dwLevel;
		pStream->FillString(",Job=") << (INT32)(*it)->stOthers.dwJob;
		pStream->FillString(",MateRoleID=") << (INT32)(*it)->stOthers.dwMateRoleID;
		pStream->FillString(",FactionID=") << (INT32)(*it)->stOthers.dwFactionID;
		pStream->FillString(",Position=") << (INT32)(*it)->stOthers.dwPosition;

		pStream->FillString(",Visibility=") << (*it)->stOthers.customVCardData.bVisibility;
		pStream->FillString(",Sex=") << (*it)->stOthers.customVCardData.bySex;
		pStream->FillString(",Constellation=") << (*it)->stOthers.customVCardData.byConstellation;
		pStream->FillString(",ChineseZodiac=") << (*it)->stOthers.customVCardData.byChineseZodiac;
		pStream->FillString(",Area=") << (*it)->stOthers.customVCardData.byArea;
		pStream->FillString(",Province=") << (*it)->stOthers.customVCardData.byProvince;
		pStream->FillString(",City='").FillString((*it)->stOthers.customVCardData.chCity, pCon).FillString("'");
		pStream->FillString(",HeadUrl='").FillString((*it)->tcHeadUrl, pCon).FillString("'");
		pStream->FillString(",Signature='").FillString((*it)->tcSignature, pCon).FillString("'");
		pStream->FillString(",RoleID=") << (*it)->stOthers.dwRoleID;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;

}
//元宝
BOOL CLoongLoongDBCtrl::FormatInsertYBAccount(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_YBAccount*>);
	std::vector <tagTWrap_Loong_YBAccount*>::iterator it = p->begin();

	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("ybaccount");

		pStream->FillString("YuanBao=") <<(*it)->nGold;
		pStream->FillString(",Gold=") <<(*it)->nSilver;
		pStream->FillString(",Silver=") <<(*it)->nSilver;
		pStream->FillString(",bSellOrder=") <<(*it)->bSellOrder;
		pStream->FillString(",bBuyOrder=") <<(*it)->bBuyOrder;
		pStream->FillString(",RoleID=") << (*it)->dwRoleID;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//bill item
BOOL CLoongLoongDBCtrl::FormatInsertBillItem(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_BillItem*>);
	std::vector <tagTWrap_Loong_BillItem*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("bill_item");
		pStream->FillString("token_id='").FillString((*it)->szTokenID, pCon);
		pStream->FillString("',account_id=") << (*it)->dwAccountID;
		pStream->FillString(",item_id=") << (*it)->dwItemTypeID;
		pStream->FillString(",item_num=") << (*it)->nNum;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;
}
//bill_yuanbao
BOOL CLoongLoongDBCtrl::FormatInsertBillYuanbao(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_BillYuanBao*>);
	std::vector <tagTWrap_Loong_BillYuanBao*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("bill_yuanbao");
		pStream->FillString("token_id='").FillString((*it)->szTokenID, pCon);
		pStream->FillString("',account_id=") << (*it)->dwAccountID;
		pStream->FillString(",yuanbao=") << (*it)->nNum;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;

}
//bill_yuanbao_log
BOOL CLoongLoongDBCtrl::FormatInsertBillYuanbaoLog(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_BillYuanBaoLog*>);
	std::vector <tagTWrap_Loong_BillYuanBaoLog*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("bill_yuanbao_log");
		pStream->FillString("token_id='").FillString((*it)->szTokenID, pCon);
		pStream->FillString("',account_id=") << (*it)->dwAccountID;
		pStream->FillString(",yuanbao=") << (*it)->nNum;
		pStream->FillString(",time='").FillString((*it)->szTime, pCon).FillString("'");

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;

}
//元宝交易订单
BOOL CLoongLoongDBCtrl::FormatInsertYuanBaoOrder(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_YuanBaoOrder*>);
	std::vector <tagTWrap_Loong_YuanBaoOrder*>::iterator it = p->begin();

	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("yuanbaoorder");

		pStream->FillString("dwID=") <<(*it)->dwID;
		pStream->FillString(",RoleID=") << (*it)->dwRoleID;
		pStream->FillString(",OrderType=") << (*it)->eYBOType;
		pStream->FillString(",Price=") << (*it)->nPrice;
		pStream->FillString(",Number=") << (*it)->nNum;
		pStream->FillString(",OrderMode=") << (*it)->eYBOMode;
		pStream->FillString(",StartTime=") << (*it)->dwStartTime;
		pStream->FillString(",EndTime=") << (*it)->dwEndTime;
		pStream->FillString(",AvgPrice=") << (*it)->nAvgPrice;
		pStream->FillString(",DealNum=") << (*it)->nDealNum;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;

}
//pet_skill
BOOL CLoongLoongDBCtrl::FormatInsertPetSkill(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_PetSkill*>);
	std::vector <tagTWrap_Loong_PetSkill*>::iterator it = p->begin();

	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("pet_skill");

		pStream->FillString("petskill_typeid=")<<(*it)->stOthers.dwTypeID;// 原型id
		pStream->FillString(",petid=") <<(*it)->dwPetID;
		pStream->FillString(",para1=") << static_cast<DWORD>((*it)->stOthers.nPara1);// 冷却计时
		pStream->FillString(",para2=") << static_cast<DWORD>((*it)->stOthers.nPara2); // 使用计时

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;

}
//pet_data
BOOL CLoongLoongDBCtrl::FormatInsertPetData(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_PetData*>);
	std::vector <tagTWrap_Loong_PetData*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("pet_data");
		pStream->FillString("pet_name='").FillString((*it)->tszName, pCon).FillString("'");
		pStream->FillString(",master_id=") <<(*it)->dwMasterID;
		pStream->FillString(",type_id=") << (*it)->dwProtoID;
		pStream->FillString(",quality=") << (*it)->nQuality;
		pStream->FillString(",aptitude=") << (*it)->nAptitude;
		pStream->FillString(",potential=") << (*it)->nPotential;
		pStream->FillString(",cur_spirit=") << (*it)->nSpirit;
		pStream->FillString(",wuxing_energy=") << (*it)->nWuXingEnergy;
		pStream->FillString(",talent_count=") << (*it)->nTalentCount;
		pStream->FillString(",pet_state=") << (*it)->byPetState;
		pStream->FillString(",cur_exp=") << (*it)->nExpCur;
		pStream->FillString(",step=") << (*it)->nStep;
		pStream->FillString(",grade=") << (*it)->nGrade;
		pStream->FillString(",pet_id=") << (*it)->dwPetID;
		pStream->FillString(",pet_lock=") << (*it)->bLocked;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}

	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);
	return TRUE;

}

//vip_stall
BOOL CLoongLoongDBCtrl::FormatInsertVipStall(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_VipStall*>);
	std::vector <tagTWrap_Loong_VipStall*>::iterator it = p->begin();

	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("vip_stall");
		pStream->FillString("RoleID=") << (*it)->dwOwnerID;
		pStream->FillString(",RemainTime=") <<(*it)->nRemainTime;
		pStream->FillString(",StallID=") << (*it)->byStallIndex;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;

}
//guild_commodity
BOOL CLoongLoongDBCtrl::FormatInsertGuildCommodity(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_GuildCommodity*>);
	std::vector <tagTWrap_Loong_GuildCommodity*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("guild_commodity");
		pStream->FillString("role_id=") << (*it)->stOthers.dwRoleID;
		pStream->FillString(",guild_id=") <<(*it)->dwGuildID;
		pStream->FillString(",role_level=") << (*it)->stOthers.nLevel;
		pStream->FillString(",tael=") << (*it)->stOthers.nTael;

		pStream->FillString(",goods='");
		pStream->FillBlob(&(*it)->stOthers.sGoodInfo, (*it)->dwGoodInfoSize, pCon);
		pStream->FillString("'");

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);

	return TRUE;
}
//guild_skill
BOOL CLoongLoongDBCtrl::FormatInsertGuildSkill(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_GuildSkill*>);
	std::vector <tagTWrap_Loong_GuildSkill*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("guild_skill");
		pStream->FillString("guild_id=") << (*it)->dwGuildID;
		pStream->FillString(",skill_id=") <<(*it)->stOthers.dwSkillID;
		pStream->FillString(",progress=") << (*it)->stOthers.n16Progress;
		pStream->FillString(",level=") << (*it)->stOthers.nLevel;
		pStream->FillString(",researching=") << (*it)->stOthers.bResearching;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);

	return TRUE;
}
//vip_netbar
BOOL CLoongLoongDBCtrl::FormatInsertVipBar(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_VipNetBar*>);
	std::vector <tagTWrap_Loong_VipNetBar*>::iterator it = p->begin();

	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("vip_netbar");
		pStream->FillString("accountid=") << (*it)->dwAccountID;
		pStream->FillString(",login_time=") <<(*it)->dwLoginTime;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//guild_upgrade
BOOL CLoongLoongDBCtrl::FormatInsertGuildUpgrade(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_GuildFacilities*>);
	std::vector <tagTWrap_Loong_GuildFacilities*>::iterator it = p->begin();

	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("guild_upgrade");
		pStream->FillString("guild_id=") << (*it)->dwGuildID;
		pStream->FillString(",type=") <<(*it)->eType;
		pStream->FillString(",level=") << (*it)->byLevel;
		pStream->FillString(",progress=") << (*it)->n16Progress;

		for (int n=1; n<=MAX_UPGRADE_NEED_ITEM_TYPE; n++)
		{
			pStream->FillString(",item_type_") << n;
			pStream->FillString("=") << (*it)->dwItemTypeID[n-1];
			pStream->FillString(",item_neednum_") << n;
			pStream->FillString("=") << (*it)->nItemNeed[n-1];
		}

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//commerce_rank
BOOL CLoongLoongDBCtrl::FormatInsertCommerceRank(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_CommerceRank*>);
	std::vector <tagTWrap_Loong_CommerceRank*>::iterator it = p->begin();

	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("commerce_rank");
		pStream->FillString("role_id=") << (*it)->stOthers.dwRoleID;
		pStream->FillString(",guild_id=") <<(*it)->dwGuildID;
		pStream->FillString(",times=") << (*it)->stOthers.nTimes;
		pStream->FillString(",tael=") << (*it)->stOthers.nTael;

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	return TRUE;
}
//activity
BOOL CLoongLoongDBCtrl::FormatInsertActivity(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_ActivityData*>);
	std::vector <tagTWrap_Loong_ActivityData*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("activity");
		pStream->FillString("dwID=") << (*it)->dwID;

		pStream->FillString(",ScriptData='");
		pStream->FillBlob(&(*it)->Data, sizeof(tagTWrap_Loong_ActivityData), pCon);
		pStream->FillString("'");

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);

	return TRUE;
}
//留言
BOOL CLoongLoongDBCtrl::FormatInsertLeftMsg(Beton::MyStream *pStream,LPVOID &pData,EDBType eDBType)
{
	MTRANS_POINTER(p, pData, std::vector <tagTWrap_Loong_LeftMsg*>);
	std::vector <tagTWrap_Loong_LeftMsg*>::iterator it = p->begin();

	Beton::Connection* pCon = m_LoongDB[eDBType].GetFreeConnection();
	for(;it != p->end(); ++it)
	{
		pStream->SetInsert("left_msg");
		pStream->FillString("msg_id=") << (*it)->dwMsgID;
		pStream->FillString(",roleid=") << (*it)->dwRoleID;

		pStream->FillString(",msg_data='");
		pStream->FillBlob((*it)->pcBlob, (*it)->dwBlobSize, pCon);
		pStream->FillString("'");

		if(!m_LoongDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
	}
	// 释放连接
	m_LoongDB[eDBType].ReturnConnection(pCon);

	return TRUE;
}