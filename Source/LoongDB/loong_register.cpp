//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: Loong.cpp
// author: Sxg
// actor:
// data: 2008-05-12
// last:
// brief: 程序框架定义实现
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"

#include "Loong.h"
//-------------------------------------------------------------------------------------------------------
// 注册需要数据库操作的命令
//-------------------------------------------------------------------------------------------------------
VOID CLoong::RegisterDBCmd()
{
	m_MsgCmdMgr.Register("N_Certification",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::InitWorldOK),			_T(""));
	m_MsgCmdMgr.Register("NDBC_WorldShutDown",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::WorldShutDown),			_T(""));
	m_MsgCmdMgr.Register("NDBC_LoongWorldInitOK",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoongWorldInitOK),	_T(""));
	m_MsgCmdMgr.Register("NDBC_LoadAllRoleInfo",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadAllRoleInfo),		_T(""));
	m_MsgCmdMgr.Register("NDBC_LoadSimRole",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadSimRole),			_T(""));
	m_MsgCmdMgr.Register("NDBC_LoadRole",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadRole),			_T(""));
	m_MsgCmdMgr.Register("NDBC_CreateRole",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::CreateRole),			_T(""));
	m_MsgCmdMgr.Register("NDBC_DelRole",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::DelRole),				_T(""));
	m_MsgCmdMgr.Register("NDBC_ResumeRole",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::ResumeRole),			_T(""));
	//m_MsgCmdMgr.Register("NDBC_RecordDel",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DeleteRecord),		_T(""));
	m_MsgCmdMgr.Register("NDBC_SaveRole",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::SaveRole),			_T(""));
	m_MsgCmdMgr.Register("NDBC_ChangeRoleName",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::ChangeRoleName),		_T(""));
	m_MsgCmdMgr.Register("NDBC_ChangeRoleGuild",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::ChangeRoleGuild),		_T(""));
	m_MsgCmdMgr.Register("NDBC_ClearRoleDressMdID",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::ChangeRoleDressMdID),	_T(""));

	// 帐号通用属性相关
	m_MsgCmdMgr.Register("NDBC_SafeCodeSet",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::SetSafeCode),			_T(""));
	m_MsgCmdMgr.Register("NDBC_SafeCodeReset",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::ResetSafeCode),		_T(""));
	m_MsgCmdMgr.Register("NDBC_SafeCodeResetCancel",(NETMSGPROC)m_Trunk.sfp2(&CLoong::CancelSafeCodeReset), _T(""));
	m_MsgCmdMgr.Register("NDBC_WareSizeUpdate",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateWareSize),		_T(""));
	m_MsgCmdMgr.Register("NDBC_WareMoneyUpdate",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateWareMoney),		_T(""));
	m_MsgCmdMgr.Register("NDBC_BaiBaoYBUpdate",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateBaiBaoYuanBao), _T(""));

	m_MsgCmdMgr.Register("NDBC_ChangeBagPsd",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::ChangeBagPsd),		_T(""));

	// 好友
	m_MsgCmdMgr.Register("NDBC_InsertFriend",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertFriend),		_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateFriend",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateFriend),		_T(""));
	m_MsgCmdMgr.Register("NDBC_DeleteFriend",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DeleteFriend),		_T(""));
	m_MsgCmdMgr.Register("NDBC_DelAllFriRel",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DeleteAllFriRel),		_T(""));

	m_MsgCmdMgr.Register("NDBC_InsertFriVal",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertFriendship),	_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateFriVal",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateFriendship),	_T(""));
	m_MsgCmdMgr.Register("NDBC_DeleteFriVal",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DeleteFriendship),	_T(""));
	m_MsgCmdMgr.Register("NDBC_DelAllFriValRel",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::DeleteAllFriValRel),	_T(""));

	// 黑名单
	m_MsgCmdMgr.Register("NDBC_InsertBlack",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertBlack),			_T(""));
	m_MsgCmdMgr.Register("NDBC_DeleteBlack",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DeleteBlack),			_T(""));

	m_MsgCmdMgr.Register("NDBC_InsertEnemy",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertEnemy),			_T(""));
	m_MsgCmdMgr.Register("NDBC_DeleteEnemy",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DeleteEnemy),			_T(""));

	m_MsgCmdMgr.Register("NDBC_GetItemInfo",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::GetItemInfo),			_T(""));
	m_MsgCmdMgr.Register("NDBC_LoadItemNeedLog",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadItemNeedLog),		_T(""));

	// 任务
	m_MsgCmdMgr.Register("NDBC_AcceptQuest",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::AcceptQuest),			_T(""));
	m_MsgCmdMgr.Register("NDBC_CompleteQuest",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::CompleteQuest),		_T(""));
	m_MsgCmdMgr.Register("NDBC_DiscardQuest",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DiscardQuest),		_T(""));

	// 技能
	m_MsgCmdMgr.Register("NDBC_AddSkill",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertSkill),			_T(""));
	m_MsgCmdMgr.Register("NDBC_RemoveSkill",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DeleteSkill),			_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateSkill",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateSkill),			_T(""));

	// 物品装备
	m_MsgCmdMgr.Register("NDBC_NewItem",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertItem),			_T(""));
	m_MsgCmdMgr.Register("NDBC_NewEquip",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertEquip),			_T(""));
	m_MsgCmdMgr.Register("NDBC_DelItem",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::DelItem),				_T(""));
	m_MsgCmdMgr.Register("NDBC_DelEquip",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::DelEquip),			_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateEquipAtt",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateEquipSpec),		_T(""));

	// 百宝袋(item_baibao表)中物品
	m_MsgCmdMgr.Register("NDBC_LoadBaiBao",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadNewItem2BaiBao),	_T(""));
	m_MsgCmdMgr.Register("NDBC_LoadBaiBaoYuanBao",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadBaiBaoYuanBaoInfo),	_T(""));
	m_MsgCmdMgr.Register("NDBC_NewBaiBaoItem",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertBaiBaoItem),	_T(""));
	m_MsgCmdMgr.Register("NDBC_NewBaiBaoEquip",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertBaiBaoEquip),	_T(""));
	m_MsgCmdMgr.Register("NDBC_DelBaiBaoItem",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DelBaiBaoItem),		_T(""));
	m_MsgCmdMgr.Register("NDBC_DelBaiBaoEquip",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DelBaiBaoEquip),		_T(""));

	// 百宝袋Log信息
	m_MsgCmdMgr.Register("NDBC_AddBaiBaoLog",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertBaibaoLog),		_T(""));
	m_MsgCmdMgr.Register("NDBC_LoadBaiBaoLog",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::GetBaibaoLogInfo),	_T(""));

	// 称号
	m_MsgCmdMgr.Register("NDBC_TitleInsert",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertTitle),			_T(""));
	m_MsgCmdMgr.Register("NDBC_TitleDelete",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::RemoveTitle),			_T(""));

	// 角色名贴
	m_MsgCmdMgr.Register("NDBC_VCardChg",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateVCard),			_T(""));
	m_MsgCmdMgr.Register("NDBC_GetOffLineVCard",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadVCard),			_T(""));
	

	// 名人堂
	m_MsgCmdMgr.Register("NDBC_FameHallInitStart",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadFameHallInit),	_T(""));
	m_MsgCmdMgr.Register("NDBC_GetRepRankList",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadRepRank),			_T(""));
	m_MsgCmdMgr.Register("NDBC_InsertFameHallSnap", (NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertFameHallEnterSnap), _T(""));
	m_MsgCmdMgr.Register("NDBC_InsertActClanTreasure", (NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertActTreasure),_T(""));
	m_MsgCmdMgr.Register("NDBC_UpDateRepRstTime",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateRepRstTime),	_T(""));

	// 元宝交易
	m_MsgCmdMgr.Register("NDBC_LoadAllYBAccount",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadAllYBAccount),	_T(""));
	m_MsgCmdMgr.Register("NDBC_LoadAllOrder",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadAllYBOrder),		_T(""));
	m_MsgCmdMgr.Register("NDBC_InsertAccount",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertYBAccount),		_T(""));
	m_MsgCmdMgr.Register("NDBC_SynAccountYB",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateAccountYB),		_T(""));
	m_MsgCmdMgr.Register("NDBC_SynAccountSilver",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateAccountSilver), _T(""));
	m_MsgCmdMgr.Register("NDBC_SynSubmitSell",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateSubmitSell),	_T(""));
	m_MsgCmdMgr.Register("NDBC_SynSubmitBuy",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateSubmitBuy),		_T(""));
	m_MsgCmdMgr.Register("NDBC_InsertOrder",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertOrder),			_T(""));
	m_MsgCmdMgr.Register("NDBC_CompleteOrder",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::CompleteOrder),		_T(""));
	m_MsgCmdMgr.Register("NDBC_RefreshOrder",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::RefreshOrder),		_T(""));
	m_MsgCmdMgr.Register("NDBC_GetRoleYBOrder",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::GetRoleYBOrder),		_T(""));
	m_MsgCmdMgr.Register("NDBC_DeleteYBOrder",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DeleteYBOrder),		_T(""));

	// 帮派属性相关
	m_MsgCmdMgr.Register("NDBC_LoadAllGuild",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadAllGuild),		_T(""));
	m_MsgCmdMgr.Register("NDBC_GuildInitOK",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::CheckGuildInitOK),	_T(""));
	m_MsgCmdMgr.Register("NDBC_CreateGuild",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::CreateGuild),			_T(""));
	m_MsgCmdMgr.Register("NDBC_DismissGuild",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DismissGuild),		_T(""));
	m_MsgCmdMgr.Register("NDBC_ChangeGuildTenet",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::ChangeGuildTenet),	_T(""));
	m_MsgCmdMgr.Register("NDBC_SaveAllGuild",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::SaveAllGuild),		_T(""));
	m_MsgCmdMgr.Register("NDBC_SaveGuildAtt",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::SaveGuildAtt),		_T(""));

	// 帮派成员属性相关
	m_MsgCmdMgr.Register("NDBC_LoadAllGuildMember",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadAllGuildMember),		_T(""));
	m_MsgCmdMgr.Register("NDBC_JoinGuild",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::JoinGuild),				_T(""));
	m_MsgCmdMgr.Register("NDBC_LeaveGuild",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::LeaveGuild),				_T(""));
	m_MsgCmdMgr.Register("NDBC_ChangeGuildPos",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::ChangeGuildPos),			_T(""));
	m_MsgCmdMgr.Register("NDBC_GuildWarePri",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::GuildWarePrivilege),		_T(""));
	m_MsgCmdMgr.Register("NDBC_ChangeContrib",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::ChangeMemberContribution),_T(""));

	// 帮派仓库相关
	m_MsgCmdMgr.Register("NDBC_LoadGuildWareItems",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadGuildWareItems),		_T(""));
	m_MsgCmdMgr.Register("NDBC_SaveGuildWare",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::SaveGuildWareItems),		_T(""));

	// 帮派设施相关
	m_MsgCmdMgr.Register("NDBC_LoadFacilitiesInfo",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadGuildFacilities),		_T(""));
	m_MsgCmdMgr.Register("NDBC_CreateFacilities",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertGuildFacilities),	_T(""));
	m_MsgCmdMgr.Register("NDBC_FacilitiesUpgrade",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateGuildFacilities),	_T(""));
	m_MsgCmdMgr.Register("NDBC_RemoveAllFacilities",(NETMSGPROC)m_Trunk.sfp2(&CLoong::RemoveAllFacilities),		_T(""));

	// 帮派技能相关
	m_MsgCmdMgr.Register("NDBC_LoadGuildSkillInfo",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadGuildSkillInfo),		_T(""));
	m_MsgCmdMgr.Register("NDBC_CreateGuildSkill",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertGuildSkillInfo),	_T(""));
	m_MsgCmdMgr.Register("NDBC_SaveGuildSkill",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::SaveGuildSkillInfo),		_T(""));
	m_MsgCmdMgr.Register("NDBC_ChangeResearchSkill",(NETMSGPROC)m_Trunk.sfp2(&CLoong::ChangeResearchGuildSkill),_T(""));
	m_MsgCmdMgr.Register("NDBC_RemoveGuildSkill",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::RemoveGuildSkillInfo),	_T(""));

	// 帮派跑商相关
	m_MsgCmdMgr.Register("NDBC_LoadCommodity",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadGuildCommerceInfo),	_T(""));
	m_MsgCmdMgr.Register("NDBC_LoadCommerceRank",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadCommerceRankInfo),	_T(""));
	m_MsgCmdMgr.Register("NDBC_CreateCommodity",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::CreateCommodityInfo),		_T(""));
	m_MsgCmdMgr.Register("NDBC_ChangeTael",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateCommodityTael),		_T(""));
	m_MsgCmdMgr.Register("NDBC_SaveCommodity",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::SaveCommodityInfo),		_T(""));
	m_MsgCmdMgr.Register("NDBC_RemoveCommodity",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::RemoveCommodityInfo),		_T(""));
	m_MsgCmdMgr.Register("NDBC_SetCommendation",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::SetCommendation),			_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateCommerceRank",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateCommerceRankInfo),	_T(""));
	m_MsgCmdMgr.Register("NDBC_RemoveCommerceRank",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::RemoveCommerceRankInfo),	_T(""));

	// 宠物相关
	m_MsgCmdMgr.Register("NDBC_GetPetInfo",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::GetPetInfo),			_T(""));
	m_MsgCmdMgr.Register("NDBC_DelPetSoul",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::DeletePet),			_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdatePetSoul",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdatePet),			_T(""));
	m_MsgCmdMgr.Register("NDBC_CreatePetSoul",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::CreatePet),			_T(""));

	m_MsgCmdMgr.Register("NDBC_InsertPetSkill",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertPetSkill),		_T(""));
	m_MsgCmdMgr.Register("NDBC_DelPetSkill",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::DelPetSkill),			_T(""));

	//m_MsgCmdMgr.Register("NDBC_PetEquip",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::PetEquip),			_T(""));
	// 商城相关
	m_MsgCmdMgr.Register("NDBC_GetAllGPInfo",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadAllGPInfo),		_T(""));
	m_MsgCmdMgr.Register("NDBC_AddGPInfo",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::AddGPInfo),			_T(""));
	m_MsgCmdMgr.Register("NDBC_RemoveGPInfo",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::RemoveGPInfo),		_T(""));
	m_MsgCmdMgr.Register("NDBC_RemoveGuildGPInfo",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::RemoveGuildGPInfo),	_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateGPInfo",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateGPInfo),		_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateGPTime",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateGPTime),		_T(""));

	// VIP摊位相关
	m_MsgCmdMgr.Register("NDBC_GetAllVIPStallInfo",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::GetAllVIPStallInfo),	_T(""));
	m_MsgCmdMgr.Register("NDBC_ApplyVIPStall",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::ApplyVIPStall),		_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateVIPStall",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateVIPStall),		_T(""));

	m_MsgCmdMgr.Register("NDBC_UpdateVNBPlayer",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateVNBPlayer),		_T(""));
	m_MsgCmdMgr.Register("NDBC_GetVNBData",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadVNBData),			_T(""));
	//	开启宝箱相关
	m_MsgCmdMgr.Register("NDBC_UpdateTreasureSum",  (NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateTreasureSum), _T(""));

	// 固定活动
	m_MsgCmdMgr.Register("NDBC_LoadActivityData",   (NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadActivityData),	_T(""));
	m_MsgCmdMgr.Register("NDBC_SaveActivityData",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::SaveActivityData),	_T(""));

	//结婚离婚
	m_MsgCmdMgr.Register("NDBC_RoleGetMarriage",   (NETMSGPROC)m_Trunk.sfp2(&CLoong::RoleGetMarriage),	_T(""));
	m_MsgCmdMgr.Register("NDBC_RoleBreakMarriage",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::RoleBreakMarriage),	_T(""));


	// 在这里注册新的函数
	//m_MsgCmdMgr.Register("",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::),			_T(""));
	
	// 实力排行榜
	m_MsgCmdMgr.Register("NDBC_InitStrengthRankings",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadStrengthRankings),	_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateRankings",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateStrengthRankings),	_T(""));

	// log相关
	m_MsgCmdMgr.Register("NDBC_LogSilver",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogSilver),			_T(""));
	m_MsgCmdMgr.Register("NDBC_LogYuanBao",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogYuanBao),			_T(""));
	m_MsgCmdMgr.Register("NDBC_LogExVolume",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogExVolume),			_T(""));
	m_MsgCmdMgr.Register("NDBC_LogTimeStat",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogTimeStat),			_T(""));
	m_MsgCmdMgr.Register("NDBC_LogItem",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogItem),				_T(""));
	m_MsgCmdMgr.Register("NDBC_LogMallSell",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogMallSell),			_T(""));
	m_MsgCmdMgr.Register("NDBC_LogMallSellPack",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogMallSellPack),		_T(""));
	m_MsgCmdMgr.Register("NDBC_LogItemTimes",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogItemTimes),		_T(""));
	m_MsgCmdMgr.Register("NDBC_LogFund",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogFund),				_T(""));
	m_MsgCmdMgr.Register("NDBC_LogMaterial",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogMaterial),			_T(""));
	m_MsgCmdMgr.Register("NDBC_LogReputation",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogReputation),		_T(""));

	// 留言相关
	m_MsgCmdMgr.Register("NDBC_LoadLeftMsg",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadLeftMsg),			_T(""));
	m_MsgCmdMgr.Register("NDBC_InsertLeftMsg",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::InsertLeftMsg),		_T(""));
//	m_MsgCmdMgr.Register("NDBC_ReplaceLeftMsg",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateLeftMsg),		_T(""));
	m_MsgCmdMgr.Register("NDBC_ClearLeftMsg",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::ClearLeftMsg),		_T(""));

	// 保存玩家经验和点
	m_MsgCmdMgr.Register("NDBC_UpdateRoleAtt",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateRoleAtt),		_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateRoleAttPoint",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateRoleAttPoint),		_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateRoleTalentPoint",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateRoleTalentPoint),		_T(""));
	m_MsgCmdMgr.Register("NDBC_UpdateRoleItemTransport",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::UpdateRoleItemTransport),		_T(""));

	// 压力测试专用
	m_MsgCmdMgr.Register("NDBC_TestLoadRole",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::TestLoadRole),		_T(""));
	m_MsgCmdMgr.Register("NDBC_TestLoadRoleItem",	(NETMSGPROC)m_Trunk.sfp2(&CLoong::TestLoadRoleItem),	_T(""));
	m_MsgCmdMgr.Register("NDBC_TestSaveItem",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::TestSaveItem),		_T(""));
	m_MsgCmdMgr.Register("NDBC_TestSaveEquip",		(NETMSGPROC)m_Trunk.sfp2(&CLoong::TestSaveEquip),		_T(""));

	// Gm命令
	m_MsgCmdMgr.Register("NDBC_LogGMCmd",			(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogGMCmd),			_T(""));

	// 宠物
	m_MsgCmdMgr.Register("NDBC_LogPet",				(NETMSGPROC)m_Trunk.sfp2(&CLoong::LogPet),				_T(""));

	//m_MsgCmdMgr.Register("NDBC_ReplaceItemCDTime", (NETMSGPROC)m_Trunk.sfp2(&CLoong::ReplaceItemCDTime), _T(""));

	// Jason external links
	m_MsgCmdMgr.Register("NDBC_GetExternalLinks",				(NETMSGPROC)m_Trunk.sfp2(&CLoong::LoadExtLinks),				_T("external link"));
}