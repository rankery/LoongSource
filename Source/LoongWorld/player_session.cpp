//-----------------------------------------------------------------------------
//!\file player_session.h
//!\author Aslan
//!
//!\date 2008-06-10
//! last 2008-06-10
//!
//!\brief 玩家连接代理类
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "player_session.h"
#include "world_session.h"
#include "player_net_cmd_mgr.h"
#include "role.h"
#include "mutex.h"
#include "map.h"
#include "world_net_cmd_mgr.h"
#include "map_creator.h"
#include "vip_netbar.h"

#include "../WorldDefine/chat.h"
#include "../WorldDefine/chat_define.h"
#include "../WorldDefine/all_msg_cmd.h"
#include "../WorldDefine/msg_func_npc.h"
#include "../WorldDefine/msg_strength.h"
#include "../WorldDefine/msg_marriage.h"
#include "../WorldDefine/msg_player_preventlost.h"
#include "netcmd_viewer.h"

// Jason 2009-12-7 神龙赐福
#include "../WorldDefine/msg_player_preventlost.h"
#include "../WorldDefine/msg_item.h"

//需要广播字符串长度最大值
#define MAX_BROADCAST_MSG_LEN 200

// 定义两个宏，分别对应普通消息注册和世界消息注册
#define M_REGISTER_PLAYER_RECV_CMD(name, handler, desc)	m_PlayerNetMgr.RegisterRecvProc(#name, handler, desc, sizeof(tag##name))
#define M_REGISTER_WORLD_RECV_CMD(name, handler, desc)	RegisterWorldMsg(#name, handler, desc, sizeof(tag##name))

PlayerNetCmdMgr PlayerSession::m_PlayerNetMgr;
GMCommandMgr	PlayerSession::m_GMCommandMgr;

#include "../ServerDefine/role_data.h"
//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
PlayerSession::PlayerSession(DWORD dwSessionID, DWORD dwInternalIndex, DWORD dwIP, BYTE byPrivilege, BOOL bGuard, DWORD dwAccOLSec, LPCSTR szAccount,tagDWORDTime dwPreLoginTime, DWORD dwPreLoginIP)
: m_dwAccountID(dwSessionID), m_dwInternalIndex(dwInternalIndex), m_dwIP(dwIP), m_byPrivilege(byPrivilege), m_nMsgNum(0), m_FatigueGarder(this, bGuard, dwAccOLSec),
m_dwPreLoginTime(dwPreLoginTime),m_dwPreLoginIP(dwPreLoginIP)
{
	m_bRoleEnuming = false;
	m_bRoleEnumDone = false;
	m_bRoleEnumSuccess = false;

	m_bRoleLoading = false;
	m_bRoleDeleting = false;
	m_bRoleCreating = false;

	m_bRoleInWorld = false;
	m_bConnectionLost = false;
	m_bKicked = false;

	strncpy_s(m_szAccount, X_SHORT_NAME, szAccount, X_SHORT_NAME);

	m_pRole = NULL;
	g_VipNetBarMgr.PlayerLogin(m_dwAccountID, m_dwIP);
}


//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
PlayerSession::~PlayerSession()
{

}

//-----------------------------------------------------------------------
// 需要在所有地图线程上层处理的消息注册
//-----------------------------------------------------------------------
VOID PlayerSession::RegisterWorldMsg(LPCSTR szCmd, NETMSGHANDLER fp, LPCTSTR szDesc, DWORD dwSize)
{
	m_PlayerNetMgr.RegisterRecvProc(szCmd, &HandleRoleMsg2World, _T("need proc upper map thread"), dwSize);
	g_worldNetCmdMgr.RegisterRecvProc(szCmd, fp, szDesc, dwSize);
}

//------------------------------------------------------------------------------
// 注册所有客户端的网络命令
//------------------------------------------------------------------------------
VOID PlayerSession::RegisterAllPlayerCmd()
{
	// 进入游戏
	M_REGISTER_PLAYER_RECV_CMD(NC_JoinGame,					&HandleJoinGame,				_T("Join Game"));

	// 选人界面
	M_REGISTER_PLAYER_RECV_CMD(NC_EnumRole,					&HandleRoleEnum,				_T("Enum Role"));
	M_REGISTER_PLAYER_RECV_CMD(NC_CreateRole,				&HandleRoleCreate,				_T("Create Role"));
	M_REGISTER_PLAYER_RECV_CMD(NC_DeleteRole,				&HandleRoleDelete,				_T("Delete Role"));
	M_REGISTER_PLAYER_RECV_CMD(NC_SelectRole,				&HandleRoleSelect,				_T("Select Role"));

	// 安全码
	M_REGISTER_PLAYER_RECV_CMD(NC_SafeCode,					&HandleRoleSetSafeCode,			_T("set safe code"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ResetSafeCode,			&HandleRoleResetSafeCode,		_T("reset safe code"));
	M_REGISTER_PLAYER_RECV_CMD(NC_CancelSafeCodeReset,		&HandleRoleCancelSafeCodeReset,	_T("cancel safe code reset"));

	// 人物属性
	M_REGISTER_PLAYER_RECV_CMD(NC_GetRoleInitState,			&HandleGetRoleInitAtt,			_T("Get Role Init State"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetRemoteRoleState,		&HandleGetRemoteUnitAtt,		_T("Get Remote Role State"));
	M_REGISTER_PLAYER_RECV_CMD(NC_LPRename,					&HandleChangeRoleName,			_T("Change Role Name"));
	
	// 行走
	M_REGISTER_PLAYER_RECV_CMD(NC_MouseWalk,				&HandleRoleWalk,				_T("Mouse Walk"));
	M_REGISTER_PLAYER_RECV_CMD(NC_KeyboardWalk,				&HandleRoleWalk,				_T("Keyboard Walk"));
	M_REGISTER_PLAYER_RECV_CMD(NC_StopWalk,					&HandleRoleStopWalk,			_T("Stop Walk"));
	M_REGISTER_PLAYER_RECV_CMD(NC_Jump,						&HandleRoleJump,				_T("Jump"));
	M_REGISTER_PLAYER_RECV_CMD(NC_Drop,						&HandleRoleDrop,				_T("Drop"));
	M_REGISTER_PLAYER_RECV_CMD(NC_VDrop,					&HandleRoleVDrop,				_T("Vertical Drop"));
	M_REGISTER_PLAYER_RECV_CMD(NC_Slide,					&HandleRoleSlide,				_T("Slide"));

	// 装备相关
	M_REGISTER_PLAYER_RECV_CMD(NC_Equip,					&HandleRoleEquip,				_T("Equip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_Unequip,					&HandleRoleUnequip,				_T("Unequip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_SwapWeapon,				&HandleRoleSwapWeapon,			_T("Swap Weapon"));
	M_REGISTER_PLAYER_RECV_CMD(NC_IdentifyEquip,			&HandleRoleIdentifyEquip,		_T("Identify Weapon"));

	// 角色外观显示设置
	M_REGISTER_PLAYER_RECV_CMD(NC_Fashion,					&HandleRoleSetFashion,			_T("set fashion"));
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleSetDisplay,			&HandleRoleSetDisplay,			_T("set display"));

	// 物品
	M_REGISTER_PLAYER_RECV_CMD(NC_ItemPosChange,			&HandleRoleChangeItemPos,		_T("Change Item Position"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ItemPosChangeEx,			&HandleRoleChangeItemPosEx,		_T("Change Item Position"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ItemReorder,				&HandleRoleReorderItem,			_T("container item reorder"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ItemReorderEx,			&HandleRoleReorderItemEx,		_T("container item reorder"));

	//使用记录式传送符设置传送坐标
	M_REGISTER_PLAYER_RECV_CMD(NC_SetTransportMap,			&HandleRoleSetTransportMap,		_T("set transport map point by special item" ));
	M_REGISTER_PLAYER_RECV_CMD(NC_UseTransportMap,			&HandleRoleUseTransportMap,		_T("ust notetaking item transmit to point" ));

	// 玩家间交易
	M_REGISTER_PLAYER_RECV_CMD(NC_ExchangeReq,				&HandleRoleExchangeReq,			_T("ExchangeReq"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ExchangeReqRes,			&HandleRoleExchangeReqRes,		_T("ExchangeReqRes"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ExchangeAdd,				&HandleRoleExchangeAdd,			_T("ExchangeAdd"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ExchangeDec,				&HandleRoleExchangeDec,			_T("ExchangeDec"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ExchangeMoney,			&HandleRoleExchangeMoney,		_T("ExchangeMoney"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ExchangeLock,				&HandleRoleExchangeLock,		_T("ExchangeLock"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ExchangeCancel,			&HandleRoleExchangeCancel,		_T("ExchangeCancel"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ExchangeVerify,			&HandleRoleExchangeVerify,		_T("ExchangeVerify"));

	// 商店
	M_REGISTER_PLAYER_RECV_CMD(NC_GetShopItems,				&HandleRoleGetShopItems,		_T("Get Goods(Item) List"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetShopEquips,			&HandleRoleGetShopEquips,		_T("Get Goods(Equip) List"));
	M_REGISTER_PLAYER_RECV_CMD(NC_BuyShopItem,				&HandleRoleBuyShopItem,			_T("Buy Item"));
	M_REGISTER_PLAYER_RECV_CMD(NC_BuyShopEquip,				&HandleRoleBuyShopEquip,		_T("Buy Equip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_SellToShop,				&HandleRoleSellToShop,			_T("Sell To Shop"));

	// 摆摊
	M_REGISTER_PLAYER_RECV_CMD(NC_StallStart,				&HandleRoleStallStart,			_T("start stall"));
	M_REGISTER_PLAYER_RECV_CMD(NC_StallSetGoods,			&HandleRoleStallSetGoods,		_T("set stall goods"));
	M_REGISTER_PLAYER_RECV_CMD(NC_StallUnsetGoods,			&HandleRoleStallUnsetGoods,		_T("unset stall goods"));
	M_REGISTER_PLAYER_RECV_CMD(NC_StallSetTitle,			&HandleRoleStallSetTitle,		_T("set stall title"));
	//M_REGISTER_PLAYER_RECV_CMD("NC_StallSetAdText",		&HandleRoleStallSetAdText,		_T("set stall ad"));
	//M_REGISTER_PLAYER_RECV_CMD("NC_StallSetAdFlag",		&HandleRoleStallSetAdFlag,		_T("set stall ad broadcast"));
	M_REGISTER_PLAYER_RECV_CMD(NC_StallSetFinish,			&HandleRoleStallSetFinish,		_T("set stall finish"));
	M_REGISTER_PLAYER_RECV_CMD(NC_StallClose,				&HandleRoleStallClose,			_T("close stall"));
	M_REGISTER_PLAYER_RECV_CMD(NC_StallGet,					&HandleRoleStallGet,			_T("get all stall goods"));
	M_REGISTER_PLAYER_RECV_CMD(NC_StallGetTitle,			&HandleRoleStallGetTitle,		_T("get stall title"));
	M_REGISTER_PLAYER_RECV_CMD(NC_StallBuy,					&HandleRoleStallBuy,			_T("buy stall goods"));
	M_REGISTER_PLAYER_RECV_CMD(NC_StallGetSpec,				&HandleRoleStallGetSpec,		_T("get stall spec goods"));

	// 驿站&乾坤石
	M_REGISTER_PLAYER_RECV_CMD(NC_Dak,						&HandleRoleDak,					_T("Move To Another Map"));

	// 磨石
	M_REGISTER_PLAYER_RECV_CMD(NC_Abrase,					&HandleRoleAbrase,				_T("abrase weapon"));

	// 角色仓库
	//m_PlayerNetMgr.Register("NC_WareOpen",				&HandleRoleWareOpen,			_T("open role ware"));
	M_REGISTER_PLAYER_RECV_CMD(NC_WareOpen,					&HandleRoleSideWareOpen,		_T("open portable ware"));
	M_REGISTER_PLAYER_RECV_CMD(NC_WareExtend,				&HandleRoleWareExtend,			_T("extend role ware space"));
	M_REGISTER_PLAYER_RECV_CMD(NC_BagExtend,				&HandleRoleBagExtand,			_T("extend role bag space"));

	// 角色仓库中存取金钱&元宝
	M_REGISTER_PLAYER_RECV_CMD(NC_SaveSilver,				&HandleRoleSaveSilver,			_T("save silver to ware"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetSilver,				&HandleRoleGetSilver,			_T("get silver from ware to bag"));
	//M_REGISTER_PLAYER_RECV_CMD("NC_SaveYuanBao",			&HandleRoleSaveYuanBao,			_T("save yuanbao to war"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetYuanBao,				&HandleRoleGetYuanBao,			_T("get yuanbao from ware to bag"));

	// 行囊加密相关
	M_REGISTER_PLAYER_RECV_CMD(NC_SetBagPsd,				&HandleRoleSetBagPsd,			_T("set bag password"));
	M_REGISTER_PLAYER_RECV_CMD(NC_UnsetBagPsd,				&HandleRoleUnsetBagPsd,			_T("cancel bag psd"));
	M_REGISTER_PLAYER_RECV_CMD(NC_OldBagPsd,				&HandleRoleCheckBagPsd,			_T("check old bag psd"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ResetBagPsd,				&HandleRoleResetBagPsd,			_T("modify bag psd"));
	M_REGISTER_PLAYER_RECV_CMD(NC_OpenBagPsd,				&HandleRoleOpenBagPsd,			_T("open bag need"));

	// 地图事件
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleMapTrigger,			&HandleRoleMapTrigger,			_T("Map Trigger"));
	M_REGISTER_PLAYER_RECV_CMD(NC_InstanceNotify,			&HandleRoleInstanceNotify,		_T("Notify Teamate Enter Instance"));
	M_REGISTER_PLAYER_RECV_CMD(NC_InstanceAgree,			&HandleRoleInstanceAgree,		_T("Agree Enter Instance"));
	M_REGISTER_PLAYER_RECV_CMD(NC_LeaveInstance,			&HandleRoleLeaveInstance,		_T("Role Leave Instance"));

	// 属性点相关
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleBidAttPoint,			&HandleRoleBidAttPoint,			_T("Role Bid Att"));
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleClearAttPoint,		&HandleRoleClearAttPoint,		_T("Role Clear Att"));

	// 天资技能相关
	M_REGISTER_PLAYER_RECV_CMD(NC_LearnSkill,				&HandleRoleLearnSkill,			_T("Role Learn Skill"));
	M_REGISTER_PLAYER_RECV_CMD(NC_LevelUpSkill,				&HandleRoleLevelUpSkill,		_T("Role's Skill Level Up"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ForgetSkill,				&HandleRoleForgetSkill,			_T("Role Forget Skill"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ClearTalent,				&HandleRoleClearTalent,			_T("Role Clear His/Her Skill"));

	// 战斗
	M_REGISTER_PLAYER_RECV_CMD(NC_EnterCombat,				&HandleRoleEnterCombat,			_T("Role Enter Combat"));
	M_REGISTER_PLAYER_RECV_CMD(NC_LeaveCombat,				&HandleRoleLeaveCombat,			_T("Role Level Combat"));
	M_REGISTER_PLAYER_RECV_CMD(NC_Skill,					&HandleRoleSkill,				_T("Role Skill"));
	M_REGISTER_PLAYER_RECV_CMD(NC_SkillInterrupt,			&HandleRoleInterruptSkill,		_T("Role Interrupt Skill"));

	// PK
	M_REGISTER_PLAYER_RECV_CMD(NC_SafeGuard,				&HandleRoleSafeGuard,			_T("Role Set/UnSet Safe Guard"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PK,						&HandleRoleSetPK,				_T("Role Set PK State"));

	// Buff
	M_REGISTER_PLAYER_RECV_CMD(NC_CancelBuff,				&HandleRoleCancelBuff,			_T("Role Cancel Buff"));

	// 个性动作
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleStyleAction,			&HandleRoleStyleAction,			_T("Role Style Action"));
	M_REGISTER_PLAYER_RECV_CMD(NC_DuetMotionInvite,			&HandleRoleDuetMotionInvite,	_T("Role Duet Action"));
	M_REGISTER_PLAYER_RECV_CMD(NC_DuetMotionOnInvite,		&HandleRoleDuetMotionOnInvite,	_T("Role Duet OnInvite"));
	M_REGISTER_PLAYER_RECV_CMD(NC_DuetMotionStart,			&HandleRoleDuetMotionStart,		_T("Role Duet Start"));

	// 复活
	M_REGISTER_PLAYER_RECV_CMD(NC_BindRebornMap,			&HandleRoleBindRebornMap,       _T("Bind Reborn Map"));
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleRevive,				&HandleRoleRevive,				_T("Revive"));

	// 聊天
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleChat,					&HandleRoleChat,				_T("Chat"));
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleGetID,				&HandleRoleGetID,				_T("RoleGetID"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetNamebyNameID,			&HandleRoleGetNameByNameID,		_T("RoleGetNamebyNameID"));
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleGetSomeName,			&HandleRoleGetSomeName,			_T("RoleGetSomeName"));

	// 装备展示
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleShowEquip,			&HandleRoleShowEquip,			_T("NC_RoleShowEquip"));
	// 物品展示
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleShowItem,				&HandleRoleShowItem,			_T("NC_RoleShowItem"));
	// 读取留言
	
	M_REGISTER_PLAYER_RECV_CMD(NC_LoadLeftMsg,				&HandleRoleLoadLeftMsg,			_T("NC_LoadLeftMsg"));

	// 任务
	M_REGISTER_PLAYER_RECV_CMD(NC_NPCAcceptQuest,			&HandleRoleNPCAcceptQuest,		_T("Quest"));
	M_REGISTER_PLAYER_RECV_CMD(NC_TriggerAcceptQuest,		&HandleRoleTriggerAcceptQuest,	_T("Quest"));
	M_REGISTER_PLAYER_RECV_CMD(NC_CompleteQuest,			&HandleRoleCompleteQuest,		_T("Quest"));
	M_REGISTER_PLAYER_RECV_CMD(NC_DeleteQuest,				&HandleRoleDeleteQuest,			_T("Quest"));
	M_REGISTER_PLAYER_RECV_CMD(NC_UpdateQuestNPCTalk,		&HandleRoleUpdateQuestNPCTalk,	_T("Quest"));

	//拾取物品
	M_REGISTER_PLAYER_RECV_CMD(NC_RolePickUpItem,			&HandleRolePickUpItem,			_T("PickUp"));
	//扔掉物品
	M_REGISTER_PLAYER_RECV_CMD(NC_RolePutDownItem,			&HandleRolePutDownItem,			_T("PutDown"));

	// 装备强化
	M_REGISTER_PLAYER_RECV_CMD(NC_ConsolidatePosy,			&HandleRolePosyEquip,			_T("Posy Equip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ConsolidateEngrave,		&HandleRoleEngraveEquip,		_T("Engrave Equip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_Inlay,					&HandleRoleInlayEquip,			_T("Inlay Equip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_Brand,					&HandleRoleBrandEquip,			_T("Brand Equip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_LoongSoul,				&HandleRoleLoongSoul,			_T("LoongSoul Equip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ConsolidateQuench,		&HandleRoleQuench,				_T("Quench Equip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_Produce,					&HandleRoleProduceItem,			_T("Produce Item"));
	M_REGISTER_PLAYER_RECV_CMD(NC_Decomposition,			&HandleRoleDeCompose,			_T("Decompose Item"));
	M_REGISTER_PLAYER_RECV_CMD(NC_Chisel,					&HandleRoleChisel,				_T("Chisel Equip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_DyeFashion,				&HandleRoleDyeFashion,			_T("Dye Fashion"));


	// Jason 2009-12-6
	M_REGISTER_PLAYER_RECV_CMD(NC_AddPot,					&HandleRoleAddEquipPotVal,			_T("Add Equip PotVal"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetLoongBenediction,					&HandleLessingLoong,			_T("Lessing Of Loong"));
	M_REGISTER_PLAYER_RECV_CMD(NC_BeGoldStone,					&HandleGoldStone,			_T("Gold stone"));

	// 使用物品
	M_REGISTER_PLAYER_RECV_CMD(NC_UseItem,					&HandleRoleUseItem,             _T("Use Item"));
	M_REGISTER_PLAYER_RECV_CMD(NC_UseItemInterrupt,			&HandleRoleInterruptUseItem,	_T("Interrupt Use Item"));

	// 好友相关
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleMakeFriend,			&HandleRoleMakeFriend,			_T("Make Friend"));
	M_REGISTER_PLAYER_RECV_CMD(NC_RoleCancelFriend,			&HandleRoleCancelFriend,		_T("Cancel Friend"));
	M_REGISTER_PLAYER_RECV_CMD(NC_UpdateFriGroup,			&HandleUpdateFriendGroup,		_T("Friend Group"));
	M_REGISTER_PLAYER_RECV_CMD(NC_MoveToBlackList,			&HandleMoveBlackList,			_T("Move BlackList"));
	M_REGISTER_PLAYER_RECV_CMD(NC_DeleteBlackList,			&HandleDeleteBlackList,			_T("Delete BlackList"));
	M_REGISTER_PLAYER_RECV_CMD(NC_SendGift,					&HandleRoleSendGift,			_T("Send Gift"));
	M_REGISTER_PLAYER_RECV_CMD(NC_SendGiftReply,			&HandleRoleSendGiftReply,		_T("Send Gift Repley"));

	// 小队相关
	M_REGISTER_PLAYER_RECV_CMD(NC_InviteJoinTeam,			&HandleRoleJoinTeam,			_T("Invite Join Team"));
	M_REGISTER_PLAYER_RECV_CMD(NC_InviteReply,				&HandleRoleJoinTeamReply,		_T("Invite Join Team Reply"));
	M_REGISTER_PLAYER_RECV_CMD(NC_KickMember,				&HandleRoleKickMember,			_T("Kick Member"));
	M_REGISTER_PLAYER_RECV_CMD(NC_LeaveTeam,				&HandleRoleLeaveTeam,			_T("Leave Team"));
	M_REGISTER_PLAYER_RECV_CMD(NC_SetPickMode,				&HandleRoleSetPickMol,			_T("Set Pick Mode"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ChangeLeader,				&HandleRoleChangeLeader,		_T("Change Leader"));
	M_REGISTER_PLAYER_RECV_CMD(NC_UpdateFriState,			&HandleUpdateFriOnline,			_T("Update Friend Online"));
	//Jason 申请入队
	M_REGISTER_PLAYER_RECV_CMD(NC_ApplyToJoinTeam,			&HandleApplyJoinTeam,		_T("Apply Join Team"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ApplyReply,				&HandleReplyForApplingToJoinTeam,			_T("Reply of Join Team"));

	// 称号相关
	M_REGISTER_PLAYER_RECV_CMD(NC_ActiveRoleTitle,			&HandleRoleActiveTitle,			_T("Active a title"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetRoleTitles,			&HandleRoleGetTitles,			_T("Get all obtained titles"));
	
	// 名帖相关
	M_REGISTER_PLAYER_RECV_CMD(NC_GetRoleVCard,				&HandleRoleGetVCard,			_T("Get VCard"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetRoleHeadPicURL,		&HandleGetHeadPicUrl,			_T("Get HeadPicUrl"));

	// 反外挂
	M_REGISTER_PLAYER_RECV_CMD(NC_GameGuarder,				&HandleGameGuarderMsg,			_T("Game Guarder"));

	// 声望
	M_REGISTER_PLAYER_RECV_CMD(NC_GetReputation,			&HandleGetRoleClanData,			_T("get role clan data"));

	// 名人堂
	M_REGISTER_PLAYER_RECV_CMD(NC_GetActClanTreasure,		&HandleGetActClanTreasure,		_T("get active treasure"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetFameHallRoles,			&HandleGetFameHallRoles,		_T("get famehall role top 50"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetReputeTop,				&HandleGetReputeTop,			_T("get reputation top 50"));

	// 百宝袋
	M_REGISTER_PLAYER_RECV_CMD(NC_InitBaiBaoRecord,			&HandleInitBaiBaoRecord,		_T("get baibao records"));
	
	// 防沉迷
	M_REGISTER_PLAYER_RECV_CMD(NC_GetFatigueInfo,			&HandleGetFatigueInfo,			_T("fatigue info"));

	// 宠物
	M_REGISTER_PLAYER_RECV_CMD(NC_GetPetAttr,				&HandleGetPetAttr,				_T("get pet att"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetSkillUse,				&HandlePetSkill,				_T("pet skill cmd"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetSkillStopWorking,		&HandlePetSkillStopWorking,		_T("pet skill stop work"));
	M_REGISTER_PLAYER_RECV_CMD(NC_UsePetEgg,				&HandleUsePetEgg,				_T("use pet egg"));
	M_REGISTER_PLAYER_RECV_CMD(NC_DeletePet,				&HandleDeletePet,				_T("delete pet"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetPetDispInfo,			&HandleGetPetDispInfo,			_T("get pet disp"));
	M_REGISTER_PLAYER_RECV_CMD(NC_SetPetState,				&HandleSetPetState,				_T("set pet state"));

	M_REGISTER_PLAYER_RECV_CMD(NC_PetEquip,					&HandlePetEquip,				_T("pet equip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetUnequip,				&HandlePetUnEquip,				_T("pet unequip"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetEquipPosSwap,			&HandlePetSwapEquipPos,			_T("pet swap equip pos"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetPetPourExpMoneyNeed,	&HandleGetPetPourExpMoneyNeed,	_T("pet pour exp money need"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetPourExp,				&HandlePetPourExp,				_T("pet pour exp"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetUpStep,				&HandlePetUpStep,				_T("pet up step"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetEnhance,				&HandlePetEnhance,				_T("pet enhance"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetFood,					&HandlePetFood,					_T("pet food"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetLearnSkill,			&HandlePetLearnSkill,			_T("pet learn skill"));

	M_REGISTER_PLAYER_RECV_CMD(NC_MountInvite,				&HandlePetInvite,				_T("invite sb mount"));
	M_REGISTER_PLAYER_RECV_CMD(NC_MountOnInvite,			&HandlePetOnInvite,				_T("on invited"));

	M_REGISTER_PLAYER_RECV_CMD(NC_PetSetLock,				&HandlePetSetLock,				_T("set pet lock"));

	// 玩家间宠物交易
	M_REGISTER_PLAYER_RECV_CMD(NC_PetExchangeReq,			&HandleRolePetExchangeReq,		_T("PetExchangeReq"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetExchangeReqRes,		&HandleRolePetExchangeReqRes,	_T("PetExchangeReqRes"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetExchangeAdd,			&HandleRolePetExchangeAdd,		_T("PetExchangeAdd"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetExchangeDec,			&HandleRolePetExchangeDec,		_T("PetExchangeDec"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetExchangeMoney,			&HandleRolePetExchangeMoney,	_T("PetExchangeMoney"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetExchangeLock,			&HandleRolePetExchangeLock,		_T("PetExchangeLock"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetExchangeCancel,		&HandleRolePetExchangeCancel,	_T("PetExchangeCancel"));
	M_REGISTER_PLAYER_RECV_CMD(NC_PetExchangeVerify,		&HandleRolePetExchangeVerify,	_T("PetExchangeVerify"));

	// npc
	M_REGISTER_PLAYER_RECV_CMD(NC_NPCTalk,					&HandleTalkToNPC,				_T("talk to npc"));

	// 跑商相关
	M_REGISTER_PLAYER_RECV_CMD(NC_GetCofCInfo,				&HandleGetCofCInfo,				_T("get CofC goods info"));
	M_REGISTER_PLAYER_RECV_CMD(NC_CloseCofC,				&HandleCloseCofC,				_T("close CofC"));
	M_REGISTER_PLAYER_RECV_CMD(NC_BuyCofCGoods,				&HandleBuyCofCGoods,			_T("buy goods from CofC"));
	M_REGISTER_PLAYER_RECV_CMD(NC_SellCofCGoods,			&HandleSellCofCGoods,			_T("sell goods to CofC"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetCommodityInfo,			&HandleGetCommodityInfo,		_T("get commodity info"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetTaelInfo,				&HandleGetTaelInfo,				_T("get commerce beginning info"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GetCommerceRank,			&HandleGetCommerceRank,			_T("get commerce rank info"));

	//	宝箱
	M_REGISTER_PLAYER_RECV_CMD(NC_TreasureChest,			&HandleOpenTreasureChest,		_T("open treasure chest"));
	M_REGISTER_PLAYER_RECV_CMD(NC_StopTreasureChest,		&HandleStopTreasureChest,		_T("stop roll item"));
	M_REGISTER_PLAYER_RECV_CMD(NC_AgainTreasureChest,		&HandleAgainTreasureChest,		_T("roll item again"));
	M_REGISTER_PLAYER_RECV_CMD(NC_ChestGetItem,				&HandleChestGetItem,			_T("get chest item"));

	// 返回角色选择
	M_REGISTER_PLAYER_RECV_CMD(NC_ReturnRoleSelect,			&HandleReturnRoleSelect,		_T("return role select"));
	// todo: add new register to here.
	//M_REGISTER_PLAYER_RECV_CMD( "",		&HandleRole,       _T(""));

	//--------------------------------------------------------------------------
	// GM命令
	//--------------------------------------------------------------------------
	M_REGISTER_PLAYER_RECV_CMD(NC_GMCommand,				&HandleGMCommand,				_T("GM Command"));
	m_GMCommandMgr.RegisterAll();


	//--------------------------------------------------------------------------
	// 需在地图线程上层处理的消息
	//--------------------------------------------------------------------------
	// 商城
	M_REGISTER_WORLD_RECV_CMD(NC_MallGet,					&PlayerSession::HandleRoleMallGet,						_T("mall get item"));
	M_REGISTER_WORLD_RECV_CMD(NC_MallUpdate,				&PlayerSession::HandleRoleMallUpdate,					_T("mall update"));
	M_REGISTER_WORLD_RECV_CMD(NC_MallBuyItem,				&PlayerSession::HandleRoleMallBuyItem,					_T("mall buy item"));
	M_REGISTER_WORLD_RECV_CMD(NC_MallBuyPack,				&PlayerSession::HandleRoleMallBuyPack,					_T("mall buy pack"));
	M_REGISTER_WORLD_RECV_CMD(NC_MallPresentItem,			&PlayerSession::HandleRoleMallPresentItem,				_T("mall buy item for friend"));
	M_REGISTER_WORLD_RECV_CMD(NC_MallPresentPack,			&PlayerSession::HandleRoleMallPresentPack,				_T("mall buy pack for friend"));
	M_REGISTER_WORLD_RECV_CMD(NC_MallFreeGetItem,			&PlayerSession::HandleRoleMallFreeGetItem,				_T("mall get free item"));
	M_REGISTER_WORLD_RECV_CMD(NC_LaunchGroupPurchase,		&PlayerSession::HandleRoleMallLaunchGroupPurchase,		_T("launch group purchase"));
	M_REGISTER_WORLD_RECV_CMD(NC_RespondGroupPurchase,		&PlayerSession::HandleRoleMallRespondGroupPurchase,		_T("respond group purchase"));
	M_REGISTER_WORLD_RECV_CMD(NC_GetSimGPInfo,				&PlayerSession::HandleRoleMallGetGroupPurchaseInfo,		_T("get guild group purchase info"));
	M_REGISTER_WORLD_RECV_CMD(NC_GetParticipators,			&PlayerSession::HandleRoleMallGetParticipators,			_T("get guild group purchase participators"));
	M_REGISTER_WORLD_RECV_CMD(NC_MallItemExchange,			&PlayerSession::HandleRoleMallItemExchange,				_T("mall item exchange"));
	M_REGISTER_WORLD_RECV_CMD(NC_MallPackExchange,			&PlayerSession::HandleRoleMallPackExchange,				_T("mall pack exchange"));


	// 元宝交易
	M_REGISTER_WORLD_RECV_CMD(NC_SaveYB2Account,			&PlayerSession::HandleRoleSaveYB2Account,				_T("save yuan bao to account"));
	M_REGISTER_WORLD_RECV_CMD(NC_SaveSilver2Account,		&PlayerSession::HandleRoleSaveSilver2Account,			_T("save silver to account"));
	M_REGISTER_WORLD_RECV_CMD(NC_DepositAccountYB,			&PlayerSession::HandleRoleDepositYBAccount,				_T("deposit yuan bao from account"));
	M_REGISTER_WORLD_RECV_CMD(NC_DepositAccountSilver,		&PlayerSession::HandleRoleDepositSilver,				_T("deposit silver from account"));
	M_REGISTER_WORLD_RECV_CMD(NC_SynYBTradeInfo,			&PlayerSession::HandleRoleGetYBTradeInfo,				_T("get yuan bao trade information"));
	M_REGISTER_WORLD_RECV_CMD(NC_SubmitSellOrder,			&PlayerSession::HandleRoleSubmitSellOrder,				_T("submit yuan bao sell order"));
	M_REGISTER_WORLD_RECV_CMD(NC_SubmitBuyOrder,			&PlayerSession::HandleRoleSubmitBuyOrder,				_T("submit yuan bao buy order"));
	M_REGISTER_WORLD_RECV_CMD(NC_DeleteOrder,				&PlayerSession::HandleRoleDeleteOrder,					_T("delete yuan bao trade order"));
	M_REGISTER_WORLD_RECV_CMD(NC_GetYuanbaoOrder,			&PlayerSession::HandleRoleGetYBOrder,					_T("get yuan bao order"));

	// 名人堂
	M_REGISTER_WORLD_RECV_CMD(NC_ActiveTreasure,			&PlayerSession::HandleActiveTreasure,					_T("active clan treasure"));

	// 帮派
	M_REGISTER_WORLD_RECV_CMD(NC_GuildCreate,				&PlayerSession::HandleCreateGuild,						_T("create guild"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildDismiss,				&PlayerSession::HandleDismissGuild,						_T("dismiss guild"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildJoinReq,				&PlayerSession::HandleJoinGuildReq,						_T("join guild req"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildJoinReqRes,			&PlayerSession::HandleJoinGuildReqRes,					_T("join guild req res"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildLeave,				&PlayerSession::HandleLeaveGuild,						_T("leave guild"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildKick,					&PlayerSession::HandleKickFromGuild,					_T("kick from guild"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildTurnover,				&PlayerSession::HandleTurnoverGuild,					_T("turnover guild"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildDemiss,				&PlayerSession::HandleDemissFromGuild,					_T("demiss from guild"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildAppoint,				&PlayerSession::HandleAppointForGuild,					_T("appoint for guild"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildTenetChange,			&PlayerSession::HandleChangeGuildTenet,					_T("change guild tenet"));

	// 帮派信息获取消息
	M_REGISTER_WORLD_RECV_CMD(NC_GuildGetAllMembers,		&PlayerSession::HandleGetGuildMembers,					_T("get all guild members"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildGetMemberEx,			&PlayerSession::HandleGetGuildMemberEx,					_T("get guild member ex"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildRefMember,			&PlayerSession::HandleRefreshGuildMember,				_T("refresh guild member"));
	M_REGISTER_WORLD_RECV_CMD(NC_GetGuildName,				&PlayerSession::HandleGetGuildName,						_T("get guild name"));
	M_REGISTER_WORLD_RECV_CMD(NC_GetGuildTenet,				&PlayerSession::HandleGetGuildTenet,					_T("get guild tenet"));
	M_REGISTER_WORLD_RECV_CMD(NC_SyncGuildInfo,				&PlayerSession::HandleSyncGuildInfo,					_T("sync guild base info"));

	// 帮派仓库
	M_REGISTER_WORLD_RECV_CMD(NC_GetGuildWare,				&PlayerSession::HandleGetGuildWareItems,				_T("get guild warehouse items"));
	M_REGISTER_WORLD_RECV_CMD(NC_GetGuildWarePriList,		&PlayerSession::HandleGetGuildWarePriList,				_T("get member warehouse privilege list"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildWarePri,				&PlayerSession::HandleGuildWarePrivilege,				_T("change member warehouse privilege"));

	// 帮派设施升级
	M_REGISTER_WORLD_RECV_CMD(NC_GetGuildUpgradeInfo,		&PlayerSession::HandleGetGuildFacilitiesInfo,			_T("get guild facilities info"));
	M_REGISTER_WORLD_RECV_CMD(NC_HandInItems,				&PlayerSession::HandleHandInItems,						_T("hand in items"));

	// 帮务发布
	M_REGISTER_WORLD_RECV_CMD(NC_SpreadGuildAffair,			&PlayerSession::HandleSpreadGuildAffair,				_T("spread guild affair"));

	// 帮派技能
	M_REGISTER_WORLD_RECV_CMD(NC_GetGuildSkillInfo,			&PlayerSession::HandleGetGuildSkillInfo,				_T("get guild skill info"));
	M_REGISTER_WORLD_RECV_CMD(NC_GuildSkillUpgrade,			&PlayerSession::HandleUpgradeGuildSkill,				_T("handin guild skill book"));
	M_REGISTER_WORLD_RECV_CMD(NC_LearnGuildSkill,			&PlayerSession::HandleLearnGuildSkill,					_T("learn guild skill"));
	M_REGISTER_WORLD_RECV_CMD(NC_SetResearchSkill,			&PlayerSession::HandleSetResearchSkill,					_T("set current research guild skill"));

	// 帮派跑商相关
	M_REGISTER_WORLD_RECV_CMD(NC_AcceptCommerce,			&PlayerSession::HandleAcceptCommerce,					_T("accept commerce"));
	M_REGISTER_WORLD_RECV_CMD(NC_CompleteCommerce,			&PlayerSession::HandleCompleteCommerce,					_T("complete commerce"));
	M_REGISTER_WORLD_RECV_CMD(NC_AbandonCommerce,			&PlayerSession::HandleAbandonCommerce,					_T("abandon commerce"));
	M_REGISTER_WORLD_RECV_CMD(NC_SwitchCommendation,		&PlayerSession::HandleSwitchCommendation,				_T("switch commend status"));

	// 角色名贴
	M_REGISTER_WORLD_RECV_CMD(NC_SetRoleVCard,				&PlayerSession::HandleRoleSetVCard,						_T("Set VCard"));

	// 客户端对话框发给服务的缺省消息
	M_REGISTER_WORLD_RECV_CMD(NC_DlgDefaultMsg,				&PlayerSession::HandleDlgDefaultMsg,					_T("dialog default message"));
	// 客户端触发服务器脚本的缺省消息
	M_REGISTER_WORLD_RECV_CMD(NC_DefaultRequest,			&PlayerSession::HandleDefaultRequest,					_T("Default Request"));

	// VIP摊位相关
	M_REGISTER_WORLD_RECV_CMD(NC_GetAllVIPStallInfo,		&PlayerSession::HandleGetAllVIPStallInfo,				_T("get all vip stall info"));
	M_REGISTER_WORLD_RECV_CMD(NC_UpdateVIPStallInfo,		&PlayerSession::HandleUpdateVIPStallInfo,				_T("get updated vip stall info"));
	M_REGISTER_WORLD_RECV_CMD(NC_ApplyVIPStall,				&PlayerSession::HandleApplyVIPStall,					_T("apply vip stall"));
	M_REGISTER_WORLD_RECV_CMD(NC_SpecVIPStallGet,			&PlayerSession::HandleSpecVIPStallGet,					_T("get vip stall goods info"));
	M_REGISTER_WORLD_RECV_CMD(NC_VIPStallBuy,				&PlayerSession::HandleBuyVIPStallGoods,					_T("buy vip stall goods"));

	M_REGISTER_WORLD_RECV_CMD(NC_GetVipNetBarName,			&PlayerSession::HandleGetVipNetBarName,					_T("get vipnetbar name"));


	//夫妻相关
	M_REGISTER_WORLD_RECV_CMD(NC_GetMarriageReq,		&PlayerSession::HandleGetMarriageReq,		_T("get marriage to somebody"));
	M_REGISTER_WORLD_RECV_CMD(NC_GetMarriageReqRes,	&PlayerSession::HandleGetMarriageReqRes,	_T("get marriage to somebody's response"));

	M_REGISTER_WORLD_RECV_CMD(NC_BreakMarriageReq,		&PlayerSession::HandleBreakMarriageReq,		_T("break marriage with somebody"));
	M_REGISTER_WORLD_RECV_CMD(NC_BreakMarriageReqRes,	&PlayerSession::HandleBreakMarriageReqRes,	_T("break marriage with somebody's response"));

	M_REGISTER_WORLD_RECV_CMD(NC_CelebrateWeddingReq,		&PlayerSession::HandleCelebrateWeddingReq,	_T("somebody make celebrate wedding request"));
	
//	RegisterWorldMsg("NC_UpdateWeddingStepReq",		&PlayerSession::HandleUpdateWeddingStepReq,	_T("applicant make update wedding step request"));

	// 获得其他玩家的装备信息
	M_REGISTER_WORLD_RECV_CMD(NC_GetRemoteRoleEquipInfo,	&PlayerSession::HandleGetSomeoneEquip,					_T("get someone's equip info"));

	// 职业系统
	M_REGISTER_WORLD_RECV_CMD(NC_SetClass,					&PlayerSession::HandleSetClass,							_T("set class"));
	M_REGISTER_WORLD_RECV_CMD(NC_ChangeClass,				&PlayerSession::HandleChangeClass,						_T("change class"));

	// 实力系统相关
	M_REGISTER_WORLD_RECV_CMD(NC_GetRankingInfo,			&PlayerSession::HandleGetRankings,						_T("get strength rankings"));
	M_REGISTER_WORLD_RECV_CMD(NC_GetSelfStrength,			&PlayerSession::HandleGetSelfStrength,					_T("get own strength"));

	// 离线提示
	M_REGISTER_WORLD_RECV_CMD(NC_ExitGamePrompt,			&PlayerSession::HandleRoleLeaveNotify,					_T("leave Notify"));

	// Jason v1.3.1 2009-12-21 宝石拆解
	M_REGISTER_PLAYER_RECV_CMD(NC_GetGemRemovalInfo,					&HandleGetGemRemovalInfo,			_T("GetGemRemovalInfo"));
	M_REGISTER_PLAYER_RECV_CMD(NC_GemRemoval,					&HandleGemRemoval,			_T("GemRemoval"));
}

//------------------------------------------------------------------------------------
// 注册所有的发送消息
//------------------------------------------------------------------------------------
VOID PlayerSession::RegisterALLSendCmd()
{
	// 进入游戏
	m_PlayerNetMgr.RegisterSendProc("NS_JoinGame");

	// 选人界面
	m_PlayerNetMgr.RegisterSendProc("NS_EnumRole");
	m_PlayerNetMgr.RegisterSendProc("NS_CreateRole");
	m_PlayerNetMgr.RegisterSendProc("NS_DeleteRole");
	m_PlayerNetMgr.RegisterSendProc("NS_SelectRole");
	m_PlayerNetMgr.RegisterSendProc("NS_SafeCode");
	m_PlayerNetMgr.RegisterSendProc("NS_ResetSafeCode");
	m_PlayerNetMgr.RegisterSendProc("NS_CancelSafeCodeReset");

	// 属性和状态
	m_PlayerNetMgr.RegisterSendProc("NS_SetState");
	m_PlayerNetMgr.RegisterSendProc("NS_UnSetState");
	m_PlayerNetMgr.RegisterSendProc("NS_SetRoleState");
	m_PlayerNetMgr.RegisterSendProc("NS_UnSetRoleState");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRoleInitState_Att");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRoleInitState_Skill");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRoleInitState_CompleteQuest");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRoleInitState_IncompleteQuest");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRoleInitState_Item");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRoleInitState_Suit");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRoleInitState_ItemCDTime");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRoleInitState_Money");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRemoteRoleState");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRemoteCreatureState");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleAttChangeSingle");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleAttChangeMutiple");
	m_PlayerNetMgr.RegisterSendProc("NS_RemoteAttChangeSingle");
	m_PlayerNetMgr.RegisterSendProc("NS_RemoteAttChangeMutiple");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleExpChange");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleLevelChange");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleBidAttPoint");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleClearAttPoint");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleAttPointAddChange");
	m_PlayerNetMgr.RegisterSendProc("NS_SendFriendBlackList");
	m_PlayerNetMgr.RegisterSendProc("NS_RemoveRemote");

	// 名字和ID
	m_PlayerNetMgr.RegisterSendProc("NS_RoleGetID");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleGetName");
	m_PlayerNetMgr.RegisterSendProc("NS_GetNamebyNameID");
	
	// 移动
	m_PlayerNetMgr.RegisterSendProc("NS_SyncWalk");
	m_PlayerNetMgr.RegisterSendProc("NS_SyncJump");
	m_PlayerNetMgr.RegisterSendProc("NS_SyncDrop");
	m_PlayerNetMgr.RegisterSendProc("NS_SyncVDrop");
	m_PlayerNetMgr.RegisterSendProc("NS_SyncSlide");
	m_PlayerNetMgr.RegisterSendProc("NS_SyncStand");
	m_PlayerNetMgr.RegisterSendProc("NS_MoveFailed");
	m_PlayerNetMgr.RegisterSendProc("NS_HitFly");
	m_PlayerNetMgr.RegisterSendProc("NS_MoveSpeedChange");

	// 聊天
	m_PlayerNetMgr.RegisterSendProc("NS_RoleChat");

	// 战斗
	m_PlayerNetMgr.RegisterSendProc("NS_Skill");
	m_PlayerNetMgr.RegisterSendProc("NS_SkillInterrupt");
	m_PlayerNetMgr.RegisterSendProc("NS_UseItem");
	m_PlayerNetMgr.RegisterSendProc("NS_UseItemInterrupt");
	m_PlayerNetMgr.RegisterSendProc("NS_HitTarget");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleHPChange");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleDead");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleRevive");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleReviveNotify");
	m_PlayerNetMgr.RegisterSendProc("NS_AddRoleBuff");
	m_PlayerNetMgr.RegisterSendProc("NS_RemoveRoleBuff");
	m_PlayerNetMgr.RegisterSendProc("NS_UpdateRoleBuff");
	m_PlayerNetMgr.RegisterSendProc("NS_StopAction");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleStyleAction");
	m_PlayerNetMgr.RegisterSendProc("NS_MonsterEnterCombat");

	// 生产
	m_PlayerNetMgr.RegisterSendProc("NS_Produce");
	m_PlayerNetMgr.RegisterSendProc("NS_Decomposition");
	m_PlayerNetMgr.RegisterSendProc("NS_ConsolidatePosy");
	m_PlayerNetMgr.RegisterSendProc("NS_ConsolidateEngrave");
	m_PlayerNetMgr.RegisterSendProc("NS_ConsolidateQuench");
	m_PlayerNetMgr.RegisterSendProc("NS_Inlay");
	m_PlayerNetMgr.RegisterSendProc("NS_Brand");
	m_PlayerNetMgr.RegisterSendProc("NS_LoongSoul");
	m_PlayerNetMgr.RegisterSendProc("NS_Chisel");
	m_PlayerNetMgr.RegisterSendProc("NS_DyeFashion");

	// 金钱和元宝
	m_PlayerNetMgr.RegisterSendProc("NS_BagSilver");
	m_PlayerNetMgr.RegisterSendProc("NS_WareSilver");
	m_PlayerNetMgr.RegisterSendProc("NS_BagYuanBao");
	m_PlayerNetMgr.RegisterSendProc("NS_BaiBaoYuanBao");

	// 交易
	m_PlayerNetMgr.RegisterSendProc("NS_ExchangeReq");
	m_PlayerNetMgr.RegisterSendProc("NS_ExchangeReqRes");
	m_PlayerNetMgr.RegisterSendProc("NS_ExchangeAdd_2Src");
	m_PlayerNetMgr.RegisterSendProc("NS_ExchangeAdd_2Dst");
	m_PlayerNetMgr.RegisterSendProc("NS_ExchangeDec");
	m_PlayerNetMgr.RegisterSendProc("NS_ExchangeMoney");
	m_PlayerNetMgr.RegisterSendProc("NS_ExchangeLock");
	m_PlayerNetMgr.RegisterSendProc("NS_ExchangeCancel");
	m_PlayerNetMgr.RegisterSendProc("NS_ExchangeFinish");

	// 职能NPC
	m_PlayerNetMgr.RegisterSendProc("NS_Dak");
	m_PlayerNetMgr.RegisterSendProc("NS_WareExtend");
	m_PlayerNetMgr.RegisterSendProc("NS_BagExtend");
	m_PlayerNetMgr.RegisterSendProc("NS_SaveSilver");
	m_PlayerNetMgr.RegisterSendProc("NS_GetSilver");
	m_PlayerNetMgr.RegisterSendProc("NS_SaveYuanBao");
	m_PlayerNetMgr.RegisterSendProc("NS_GetYuanBao");
	m_PlayerNetMgr.RegisterSendProc("NS_Abrase");

	// 组队
	m_PlayerNetMgr.RegisterSendProc("NS_InviteToLeader");
	m_PlayerNetMgr.RegisterSendProc("NS_InviteJoinTeam");
	m_PlayerNetMgr.RegisterSendProc("NS_InviteReply");
	m_PlayerNetMgr.RegisterSendProc("NS_KickMember");
	m_PlayerNetMgr.RegisterSendProc("NS_LeaveTeam");
	m_PlayerNetMgr.RegisterSendProc("NS_SetPickMode");
	m_PlayerNetMgr.RegisterSendProc("NS_ChangeLeader");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleStateToTeam");
	m_PlayerNetMgr.RegisterSendProc("NS_RolePosToTeam");

	// 物品和装备
	m_PlayerNetMgr.RegisterSendProc("NS_Equip");
	m_PlayerNetMgr.RegisterSendProc("NS_Unequip");
	m_PlayerNetMgr.RegisterSendProc("NS_SwapWeapon");
	m_PlayerNetMgr.RegisterSendProc("NS_AvatarEquipChange");
	m_PlayerNetMgr.RegisterSendProc("NS_IdentifyEquip");
	m_PlayerNetMgr.RegisterSendProc("NS_EquipChange");
	m_PlayerNetMgr.RegisterSendProc("NS_SuitEffect");
	m_PlayerNetMgr.RegisterSendProc("NS_SuitNum");
	m_PlayerNetMgr.RegisterSendProc("NS_ItemPosChange");
	m_PlayerNetMgr.RegisterSendProc("NS_ItemPosChangeEx");
	m_PlayerNetMgr.RegisterSendProc("NS_ItemAdd");
	m_PlayerNetMgr.RegisterSendProc("NS_NewItemAdd");
	m_PlayerNetMgr.RegisterSendProc("NS_NewEquipAdd");
	m_PlayerNetMgr.RegisterSendProc("NS_ItemRemove");
	m_PlayerNetMgr.RegisterSendProc("NS_ItemCDUpdate");

	// 掉落
	m_PlayerNetMgr.RegisterSendProc("NS_SyncGroundItem");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleGroundItemDisappear");
	m_PlayerNetMgr.RegisterSendProc("NS_PutDownItem");
	m_PlayerNetMgr.RegisterSendProc("NS_RolePutDownItem");
	m_PlayerNetMgr.RegisterSendProc("NS_RolePickUpItem");
	m_PlayerNetMgr.RegisterSendProc("NS_GetGroundItemInfo");

	// 地图
	m_PlayerNetMgr.RegisterSendProc("NS_EnterInstance");
	m_PlayerNetMgr.RegisterSendProc("NS_GotoNewMap");
	m_PlayerNetMgr.RegisterSendProc("NS_InstanceNofity");
	m_PlayerNetMgr.RegisterSendProc("NS_InstanceAgree");
	m_PlayerNetMgr.RegisterSendProc("NS_InstanceComplete");
	m_PlayerNetMgr.RegisterSendProc("NS_SyncInstanceTime");
	m_PlayerNetMgr.RegisterSendProc("NS_BindRebornMap");

	// PK
	m_PlayerNetMgr.RegisterSendProc("NS_SafeGuard");
	m_PlayerNetMgr.RegisterSendProc("NS_PK");
	m_PlayerNetMgr.RegisterSendProc("NS_RolePKStateChange");

	// 商店
	m_PlayerNetMgr.RegisterSendProc("NS_GetShopItems");
	m_PlayerNetMgr.RegisterSendProc("NS_GetShopEquips");
	m_PlayerNetMgr.RegisterSendProc("NS_BuyShopItem");
	m_PlayerNetMgr.RegisterSendProc("NS_BuyShopEquip");
	m_PlayerNetMgr.RegisterSendProc("NS_FeedbackFromShop");

	// 展示物品和装备
	m_PlayerNetMgr.RegisterSendProc("NS_RoleShowEquip");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleShowItem");

	// 社会关系
	m_PlayerNetMgr.RegisterSendProc("NS_LoginToFriend");
	m_PlayerNetMgr.RegisterSendProc("NS_LogoutToFriend");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleMakeFriend");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleCancelFriend");
	m_PlayerNetMgr.RegisterSendProc("NS_UpdateFriGroup");
	m_PlayerNetMgr.RegisterSendProc("NS_MoveToBlackList");
	m_PlayerNetMgr.RegisterSendProc("NS_DeleteBlackList");
	m_PlayerNetMgr.RegisterSendProc("NS_SendGiftToFri");
	m_PlayerNetMgr.RegisterSendProc("NS_SendGiftToSender");
	m_PlayerNetMgr.RegisterSendProc("NS_SendGiftBroadcast");
	m_PlayerNetMgr.RegisterSendProc("NS_SendBlackList");
	m_PlayerNetMgr.RegisterSendProc("NS_UpdateFriState");
	m_PlayerNetMgr.RegisterSendProc("NS_MakeFriNotice");
	m_PlayerNetMgr.RegisterSendProc("NS_UpdateFriValue");

	// 摆摊
	m_PlayerNetMgr.RegisterSendProc("NS_StallStart");
	m_PlayerNetMgr.RegisterSendProc("NS_StallSetGoods");
	m_PlayerNetMgr.RegisterSendProc("NS_StallUnsetGoods");
	m_PlayerNetMgr.RegisterSendProc("NS_StallSetTitle");
	//m_PlayerNetMgr.RegisterSendProc("NS_StallSetAdText");
	//m_PlayerNetMgr.RegisterSendProc("NS_StallSetAdFlag");
	m_PlayerNetMgr.RegisterSendProc("NS_StallSetFinish");
	m_PlayerNetMgr.RegisterSendProc("NS_StallClose");
	m_PlayerNetMgr.RegisterSendProc("NS_StallBuyRefresh");
	m_PlayerNetMgr.RegisterSendProc("NS_StallSetRefresh");
	m_PlayerNetMgr.RegisterSendProc("NS_StallUnsetRefresh");
	m_PlayerNetMgr.RegisterSendProc("NS_StallGet");
	m_PlayerNetMgr.RegisterSendProc("NS_StallGetTitle");
	m_PlayerNetMgr.RegisterSendProc("NS_StallBuy");
	m_PlayerNetMgr.RegisterSendProc("NS_StallGetSpec");

	// 天资及技能
	m_PlayerNetMgr.RegisterSendProc("NS_LearnSkill");
	m_PlayerNetMgr.RegisterSendProc("NS_AddSkill");
	m_PlayerNetMgr.RegisterSendProc("NS_LevelUpSkill");
	m_PlayerNetMgr.RegisterSendProc("NS_UpdateSkill");
	m_PlayerNetMgr.RegisterSendProc("NS_UpdateSkillCoolDown");
	m_PlayerNetMgr.RegisterSendProc("NS_ForgetSkill");
	m_PlayerNetMgr.RegisterSendProc("NS_ClearTalent");
	m_PlayerNetMgr.RegisterSendProc("NS_RemoveSkill");
	m_PlayerNetMgr.RegisterSendProc("NS_AddTalent");
	m_PlayerNetMgr.RegisterSendProc("NS_RemoveTalent");
	m_PlayerNetMgr.RegisterSendProc("NS_UpdateTalent");

	// 任务
	m_PlayerNetMgr.RegisterSendProc("NS_AcceptQuest");
	m_PlayerNetMgr.RegisterSendProc("NS_AddQuest");
	m_PlayerNetMgr.RegisterSendProc("NS_CompleteQuest");
	m_PlayerNetMgr.RegisterSendProc("NS_DeleteQuest");
	m_PlayerNetMgr.RegisterSendProc("NS_QuestUpdateKillCreature");
	m_PlayerNetMgr.RegisterSendProc("NS_QuestUpdateItem");
	m_PlayerNetMgr.RegisterSendProc("NS_QuestUpdateNPCTalk");
	m_PlayerNetMgr.RegisterSendProc("NS_QuestUpdateTrigger");



	// 声望
	m_PlayerNetMgr.RegisterSendProc("NS_GetReputation");

	// 反外挂
	m_PlayerNetMgr.RegisterSendProc("NS_GameGuarder");

	// 名人堂
	m_PlayerNetMgr.RegisterSendProc("NS_GetFameHallRoles");
	m_PlayerNetMgr.RegisterSendProc("NS_GetReputeTop");
	m_PlayerNetMgr.RegisterSendProc("NS_GetActClanTreasure");
	m_PlayerNetMgr.RegisterSendProc("NS_ActiveTreasure");
	m_PlayerNetMgr.RegisterSendProc("NS_NewActivedTreasure");
	m_PlayerNetMgr.RegisterSendProc("NS_TreasureActCountChange");
	m_PlayerNetMgr.RegisterSendProc("NS_BecomeFame");

	// 角色名贴
	m_PlayerNetMgr.RegisterSendProc("NS_GetRoleVCard");
	m_PlayerNetMgr.RegisterSendProc("NS_SetRoleVCard");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRoleHeadPicURL");
	
	// 称号
	m_PlayerNetMgr.RegisterSendProc("NS_ActiveRoleTitle");
	m_PlayerNetMgr.RegisterSendProc("NS_GetRoleTitles");
	m_PlayerNetMgr.RegisterSendProc("NS_NewTitles");
	m_PlayerNetMgr.RegisterSendProc("NS_RoleTitleChangeBroadcast");

	// 帮派
	m_PlayerNetMgr.RegisterSendProc("NS_GuildCreateBroad");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildDismissBroad");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildJoinReq");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildJoinReqRes");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildJoinBroad");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildLeaveBroad");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildKickBroad");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildTurnoverBroad");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildDemissBroad");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildAppointBroad");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildTenetChangeBroad");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildProcFailed");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildGetAllMembers");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildGetMemberEx");
	m_PlayerNetMgr.RegisterSendProc("NS_GuildRefMember");
	m_PlayerNetMgr.RegisterSendProc("NS_GetGuildName");
	m_PlayerNetMgr.RegisterSendProc("NS_GetGuildTenet");

	// 百宝袋
	m_PlayerNetMgr.RegisterSendProc("NS_InitBaiBaoRecord");
	m_PlayerNetMgr.RegisterSendProc("NS_SingleBaiBaoRecord");

	// NPC
	m_PlayerNetMgr.RegisterSendProc("NS_NPCTalk");

	// 宠物
	m_PlayerNetMgr.RegisterSendProc("NS_GetPetAttr");
	m_PlayerNetMgr.RegisterSendProc("NS_GetPetDispInfo");
	m_PlayerNetMgr.RegisterSendProc("NS_PetDispInfoChange");
	m_PlayerNetMgr.RegisterSendProc("NS_UsePetEgg");
	m_PlayerNetMgr.RegisterSendProc("NS_PetAttrChange");
	m_PlayerNetMgr.RegisterSendProc("NS_CallPet");

	m_PlayerNetMgr.RegisterSendProc("NS_PetEquip");
	m_PlayerNetMgr.RegisterSendProc("NS_PetUnequip");
	m_PlayerNetMgr.RegisterSendProc("NS_PetEquipPosSwap");
	m_PlayerNetMgr.RegisterSendProc("NS_GetPetPourExpMoneyNeed");
	
	m_PlayerNetMgr.RegisterSendProc("NS_PetSkillUse");
	m_PlayerNetMgr.RegisterSendProc("NS_AddPetSkill");
	m_PlayerNetMgr.RegisterSendProc("NS_RemovePetSkill");

	// 宠物交易
	m_PlayerNetMgr.RegisterSendProc("NS_PetExchangeReq");
	m_PlayerNetMgr.RegisterSendProc("NS_PetExchangeReqRes");
	m_PlayerNetMgr.RegisterSendProc("NS_PetExchangeAdd_2Src");
	m_PlayerNetMgr.RegisterSendProc("NS_PetExchangeAdd_2Dst");
	m_PlayerNetMgr.RegisterSendProc("NS_PetExchangeDec");
	m_PlayerNetMgr.RegisterSendProc("NS_PetExchangeMoney");
	m_PlayerNetMgr.RegisterSendProc("NS_PetExchangeLock");
	m_PlayerNetMgr.RegisterSendProc("NS_PetExchangeCancel");
	m_PlayerNetMgr.RegisterSendProc("NS_PetExchangeFinish");


	// 跑商相关
	m_PlayerNetMgr.RegisterSendProc("NS_GetCofCInfo");
	m_PlayerNetMgr.RegisterSendProc("NS_GetCommodityInfo");
	m_PlayerNetMgr.RegisterSendProc("NS_GetTaelInfo");
	m_PlayerNetMgr.RegisterSendProc("NS_GetCommerceRank");

	//	宝箱
	m_PlayerNetMgr.RegisterSendProc("NS_TreasureChest");
	m_PlayerNetMgr.RegisterSendProc("NS_StopTreasureChest");
	m_PlayerNetMgr.RegisterSendProc("NS_AgainTreasureChest");
	m_PlayerNetMgr.RegisterSendProc("NS_ChestGetItem");

	// 返回角色选择
	m_PlayerNetMgr.RegisterSendProc("NS_ReturnRoleSelect");

	//夫妻相关
	m_PlayerNetMgr.RegisterSendProc("NS_GetMarriageReq");
	m_PlayerNetMgr.RegisterSendProc("NS_GetMarriageReqRes");
	m_PlayerNetMgr.RegisterSendProc("NC_BreakMarriageReq");
	m_PlayerNetMgr.RegisterSendProc("NC_BreakMarriageReqRes");

	//m_PlayerNetMgr.RegisterSendProc("");
}

//------------------------------------------------------------------------------------
// 撤销所有客户端网络命令
//------------------------------------------------------------------------------------
VOID PlayerSession::UnRegisterALL()
{
	// 将本次统计信息写入log
	m_PlayerNetMgr.LogAllMsg();

	m_PlayerNetMgr.UnRegisterAll();
	m_GMCommandMgr.UnRegisterAll();
}

//------------------------------------------------------------------------------------
// 消息处理主函数（当返回GT_INVALID, 则说明连接已经丢失，要删除该session）
//------------------------------------------------------------------------------------
INT PlayerSession::HandleMessage()
{
	m_PlayerNetLock.Acquire();

	// 检测客户端是否已经失去连接
	if( m_bConnectionLost )
	{
		LogoutPlayer();
		m_PlayerNetLock.Release();
		return CON_LOST;
	}

	// 处理客户端消息
	DWORD dwSize = 0;
	LPBYTE pMsg = RecvMsg(dwSize);

	m_PlayerNetLock.Release();
	if( !P_VALID(pMsg) ) return 0;

	tagNetCmd* pCmd = (tagNetCmd*)pMsg;

	NETMSGHANDLER pHandler = m_PlayerNetMgr.GetHandler(pCmd, dwSize);

	TheNetCmdViewer.Log(m_dwAccountID, P_VALID(GetRole())?GetRole()->GetID():-1, *pCmd);

	DWORD dwRet = GT_INVALID;
	if( NULL != pHandler )
		dwRet = (this->*pHandler)(pCmd);

	// 判断是否需上层处理
	if( RET_TRANS == dwRet )
	{
		g_worldNetCmdMgr.Add(GetSessionID(), pMsg, dwSize);
	}
	else
	{
		RecycleMsg(pMsg);
	}


	// 如果发现session的m_bPushed被设置成TRUE，则说明客户端在此消息处理之后被移动出地图
	


	return 0;
}

//------------------------------------------------------------------------------------
// 发送消息接口
//------------------------------------------------------------------------------------
VOID PlayerSession::SendMessage(LPVOID pMsg, DWORD dwSize)
{
	if( !P_VALID(pMsg) || dwSize == 0 )
		return;

	m_PlayerNetLock.Acquire();

	// 记录发包数量
	m_PlayerNetMgr.CountServerMsg(*(DWORD*)pMsg);

	// 连接已经中断，不再发送了
	if( m_bConnectionLost || m_bKicked ) 
	{
		m_PlayerNetLock.Release();
		return;
	}

	if( dwSize > g_world.GetMaxMsgSize() )
	{
		g_world.SetMaxMsgSize(dwSize);
	}

	// 发送到网络底层
	SendMsg((LPBYTE)pMsg, dwSize);

	m_PlayerNetLock.Release();
}

//------------------------------------------------------------------------------
// 完全登陆玩家（要将玩家从选人界面session列表移动到一个mapmgr的session列表中）
//------------------------------------------------------------------------------
BOOL PlayerSession::FullLogin(Role* pRole, BOOL bFirst)
{
	ASSERT( P_VALID(pRole) );

	SetRole(pRole);

	m_bRoleLoading = false;

	DWORD dwSessionID = m_dwAccountID;	// 保证预先取出sessionID，线程安全

	// 将该角色加入到地图中
	if( !pRole->AddToWorld(bFirst) )
	{
		SetRole(NULL);
		return FALSE;
	}

	// 清空dressid
	if( bFirst )
	{
		tagNDBC_ClearRoleDressMdID send;
		send.dwRoleID	= pRole->GetID();
		send.wNewVal	= DressMdIDInvalid;
		g_dbSession.Send(&send, send.dwSize);
	}

	// 已经加入到地图中，从全局列表中删除自己
	g_worldSession.RemoveGlobalSession(dwSessionID);
	return TRUE;
}

//------------------------------------------------------------------------------
// 接收消息
//------------------------------------------------------------------------------
LPBYTE PlayerSession::RecvMsg(DWORD& dwSize)
{
	return g_worldSession.RecvMsg(dwSize, m_nMsgNum, m_dwInternalIndex);
}

//------------------------------------------------------------------------------
// 归还消息
//------------------------------------------------------------------------------
VOID PlayerSession::ReturnMsg(LPBYTE pMsg)
{
	g_worldSession.ReturnMsg(pMsg);
}

//------------------------------------------------------------------------------
// 发送消息
//------------------------------------------------------------------------------
VOID PlayerSession::SendMsg(LPBYTE pMsg, DWORD dwSize)
{
	g_worldSession.SendMsg(m_dwInternalIndex, pMsg, dwSize);
}

//------------------------------------------------------------------------------
// 登出玩家
//------------------------------------------------------------------------------
VOID PlayerSession::LogoutPlayer()
{
	Role* pRole = GetRole();
	if( !P_VALID(pRole) ) return;

	// 记录玩家要登出
	g_mapCreator.RoleLogOut(pRole->GetID());
}

//------------------------------------------------------------------------------
// 重置session的内容，用于返回选人界面
//------------------------------------------------------------------------------
VOID PlayerSession::Refresh()
{
	m_bRoleEnuming		=	false;
	m_bRoleEnumDone		=	false;
	m_bRoleEnumSuccess	=	false;
	m_bRoleLoading		=	false;
	m_bRoleDeleting		=	false;
	m_bRoleCreating		=	false;
	m_bRoleInWorld		=	false;

	m_pRole				=	NULL;
}

//------------------------------------------------------------------------------
// 判断是否帐号下是否有该角色
//------------------------------------------------------------------------------
BOOL PlayerSession::IsRoleExist(const DWORD dwRoleID) const
{
	if(GT_INVALID == dwRoleID)
	{
		return FALSE;
	}

	for(INT i=0; i<MAX_ROLENUM_ONEACCOUNT; ++i)
	{
		if(dwRoleID == m_dwRoleID[i])
		{
			return TRUE;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------------
// 帐号添加新角色
//------------------------------------------------------------------------------
BOOL PlayerSession::AddRole(const DWORD dwRoleID)
{
	for(INT i=0; i<MAX_ROLENUM_ONEACCOUNT; ++i)
	{
		if(GT_INVALID == m_dwRoleID[i])
		{
			m_dwRoleID[i] = dwRoleID;
			++m_n8RoleNum;
			return TRUE;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------------
// 删除帐号下角色
//------------------------------------------------------------------------------
BOOL PlayerSession::RemoveRole(const DWORD dwRoleID)
{
	for(INT i=0; i<MAX_ROLENUM_ONEACCOUNT; ++i)
	{
		if(dwRoleID == m_dwRoleID[i])
		{
			m_dwRoleID[i] = GT_INVALID;
			--m_n8RoleNum;
			return TRUE;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------------
// 判断是否可以设置安全码
//------------------------------------------------------------------------------
BOOL PlayerSession::CanSetSafeCode()
{
	// 没有设置过安全码
	if(GT_INVALID == m_sAccountCommon.sSafeCode.dwSafeCodeCrc)
	{
		return TRUE;
	}
		
	// 当前有安全码，而且没有重置过
	if(GT_INVALID == m_sAccountCommon.sSafeCode.dwTimeReset)
	{
		return FALSE;
	}

	// 重置时间未够72小时
	if(CalcTimeDiff(g_world.GetWorldTime(), m_sAccountCommon.sSafeCode.dwTimeReset) < MAX_SAFECODE_RESET_TIME)
	{
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------
// 判断是否可以重置安全码
//------------------------------------------------------------------------------
BOOL PlayerSession::CanResetSafeCode() const
{
	// 没有设置过安全码
	if(GT_INVALID == m_sAccountCommon.sSafeCode.dwSafeCodeCrc)
	{
		return FALSE;
	}

	// 当前有安全码，而且没有重置过
	if(GT_INVALID == m_sAccountCommon.sSafeCode.dwTimeReset)
	{
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------
// 判断是否可以取消安全码重置
//------------------------------------------------------------------------------
BOOL PlayerSession::CanCancelSafeCodeReset() const
{
	// 没有设置过安全码
	if(GT_INVALID == m_sAccountCommon.sSafeCode.dwSafeCodeCrc)
	{
		return FALSE;
	}

	// 当前有安全码，而且没有重置过
	if(GT_INVALID == m_sAccountCommon.sSafeCode.dwTimeReset)
	{
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------
// 在当前频道向九宫格内所有玩家广播消息
//------------------------------------------------------------------------------
VOID PlayerSession::BroadcastCurrChannel(LPCTSTR szMsg)
{
	if (!P_VALID(szMsg))
	{
		return;
	}

	size_t len = _tcslen(szMsg);
	INT	nSzMsg = len * sizeof(TCHAR);
	if (len > MAX_BROADCAST_MSG_LEN)
	{
		return;
	}

	if (!P_VALID(m_pRole))
	{
		return;
	}

	Map* pMap = m_pRole->GetMap();
	if (!P_VALID(pMap))
	{
		return;
	}

	TCHAR msg[1024] = {0};

	tagNS_RoleChat* pSend   = (tagNS_RoleChat*)msg;
	pSend->byChannel		= (BYTE)ESCC_Current;
	pSend->dwID				= TObjRef<Util>()->Crc32("NS_RoleChat");
	pSend->dwDestRoleID		= m_pRole->GetID();
	pSend->dwSrcRoleID		= m_pRole->GetID(); 
	pSend->dwErrorCode		= 0;
	pSend->dwSize			= sizeof(tagNS_RoleChat) + nSzMsg;
	IFASTCODE->MemCpy(pSend->szMsg, szMsg, nSzMsg);

	// 加字符串结束符
	pSend->szMsg[len] = _T('\0');

	pMap->SendBigVisTileMsg(m_pRole, (LPVOID)pSend, pSend->dwSize);
}

//-----------------------------------------------------------------------
// 回收消息Unit(有用则再利用，无用则释放)
//-----------------------------------------------------------------------
VOID PlayerSession::RecycleMsg(LPBYTE pMsg)
{
	g_worldSession.ReturnMsg(pMsg);
}

//-----------------------------------------------------------------------------
// 更新
//----------------------------------------------------------------------------
INT PlayerSession::Update()
{
	return HandleMessage();
}

//-----------------------------------------------------------------------------
// 查找当前地图中的玩家
//----------------------------------------------------------------------------
Role* PlayerSession::GetOtherInMap( DWORD dwRoleID ) const 
{
	MTRANS_ELSE_RET(pThisRole,	GetRole(),					Role,	NULL);
	MTRANS_ELSE_RET(pMap,		pThisRole->GetMap(),		Map,	NULL);
	MTRANS_ELSE_RET(pRole,		pMap->FindRole(dwRoleID),	Role,	NULL);
	return pRole;
}

INT PlayerSession::GetVNBExpRate() const
{
	return g_VipNetBarMgr.GetRate(m_dwIP, 0);
}

INT PlayerSession::GetVNBLootRate() const
{
	return g_VipNetBarMgr.GetRate(m_dwIP, 1);
}

VOID PlayerSession::SessionLogout()
{
	g_VipNetBarMgr.PlayerLogout(m_dwIP);

	m_PlayerNetLock.Acquire();
	SetConnectionLost();
	m_PlayerNetLock.Release();
}

LPCTSTR PlayerSession::GetVNBName() const
{
	return g_VipNetBarMgr.GetVNBName(m_dwIP);
}

// Jason 2009-12-7 神龙赐福
DWORD	PlayerSession::HandleLessingLoong(tagNetCmd * pCmd)
{
	Role * pRole = static_cast<Role*>( GetRole() );
	if( P_VALID( pRole ) )
	{
		return pRole->ReceiveLessingOfLoong();
	}
	return GT_INVALID;
}

DWORD PlayerSession::HandleGoldStone(tagNetCmd * pCmd)
{
	Role * pRole = static_cast<Role*>( GetRole() );
	tagNC_BeGoldStone * msg = (tagNC_BeGoldStone *)pCmd;
	DWORD dwErr = 0;
	if( P_VALID( pRole ) && P_VALID( msg ) )
	{
		dwErr = pRole->ProcessGoldStoneMsg(msg->n64SrcItemID,msg->n64DstItemID);
	}
	tagNS_BeGoldStone msg1;
	msg1.dwErrCode = dwErr;
	SendMessage  (&msg1,msg1.dwSize);
	return dwErr;
}


