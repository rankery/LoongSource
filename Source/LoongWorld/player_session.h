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
#pragma once

#include "StdAfx.h"
#include "player_net_cmd_mgr.h"
#include "../ServerDefine/base_define.h"
#include "../ServerDefine/role_data_define.h"
#include "Mutex.h"
#include "gm_net_cmd_mgr.h"
#include "fatigue_guarder.h"

class Creature;

const INT CON_LOST	=	-1000000;		// 连接断开标志
const INT RET_TRANS	=	-2000000;		// 需上层处理

class PlayerSession
{
public:
	friend class DBSession;
	friend class WorldNetCmdMgr;

	PlayerSession(DWORD dwSessionID, DWORD dwInternalIndex, DWORD dwIP, BYTE byPrivilege,
				BOOL bGuard, DWORD dwAccOLSec, LPCSTR tszAccount,tagDWORDTime dwPreLoginTime,
				DWORD dwPreLoginIP);
	~PlayerSession();

	//-----------------------------------------------------------------------------
	// 各种Get
	//-----------------------------------------------------------------------------

	tagDWORDTime GetPreLoginTime()	const	{ return m_dwPreLoginTime;}
	DWORD		 GetPreLoginIP()	const	{ return m_dwPreLoginIP;}
	DWORD		 GetCurLoginIP()	const	{ return m_dwIP;}

	LPCSTR		GetAccount()		const	{ return m_szAccount;	}
	DWORD		GetGetIP()			const	{ return m_dwIP; }
	DWORD		GetSessionID()		const	{ return m_dwAccountID; }
	INT			GetMsgWaitNum()		const	{ return m_nMsgNum; }
	DWORD		GetInternalIndex()	const	{ return m_dwInternalIndex; }
	Role*		GetRole()			const	{ return m_pRole; }
	Role*		GetOtherInMap( DWORD dwRoleID ) const ;
	const FatigueGuarder&	GetFatigueGuarder()	const { return m_FatigueGarder; }
	INT			GetVNBExpRate()		const;
	INT			GetVNBLootRate()		const;
	LPCTSTR		GetVNBName()		const;
	BOOL		IsKicked() const			{ return m_bKicked; }

	// 帐号下角色通用属性相关操作
	const INT	GetBaiBaoYB()		const	{ return m_sAccountCommon.nBaiBaoYuanBao; }
	const INT64	GetWareSilver()		const	{ return m_sAccountCommon.n64WareSilver; }
	const INT16	GetWareSize()		const	{ return m_sAccountCommon.n16WareSize; }

	//----------------------------------------------------------------------------
	// 各种Set
	//----------------------------------------------------------------------------
	VOID		SetRole(Role* pRole)		{ m_pRole = pRole; }
	VOID		SetConnectionLost()			{ InterlockedExchange((LPLONG)(&m_bConnectionLost), TRUE); }
	VOID		SetKicked()					{ InterlockedExchange((LPLONG)(&m_bKicked), TRUE); }

	VOID		SetBaiBaoYB(INT nYuanBao)		{ m_sAccountCommon.nBaiBaoYuanBao	= nYuanBao; }
	VOID		SetWareSilver(INT64 n64Silver)	{ m_sAccountCommon.n64WareSilver	= n64Silver; }
	VOID		SetAccOLMin(DWORD dwState, DWORD dwAccOLMin)	{ m_FatigueGarder.SetAccOLTimeMin(dwState, dwAccOLMin);				}
	VOID		SetRoleLoading()				{ m_bRoleLoading = TRUE;	}
	VOID		SessionLogout();

	//----------------------------------------------------------------------------
	// 消息处理相关
	//----------------------------------------------------------------------------
	INT			HandleMessage();
	VOID		SendMessage(LPVOID pMsg, DWORD dwSize);
	VOID		BroadcastCurrChannel(LPCTSTR szMsg);

	//-----------------------------------------------------------------------------
	// 选人界面的判断
	//-----------------------------------------------------------------------------
	bool		IsRoleLoading()		const	{ return m_bRoleLoading; }
	bool		IsRoleEnuming()		const	{ return m_bRoleEnuming; }
	bool		IsRoleCreating()	const	{ return m_bRoleCreating; }
	bool		IsRoleDeleting()	const	{ return m_bRoleDeleting; }
	bool		IsInWorld()			const	{ return m_bRoleInWorld; }


	//-----------------------------------------------------------------------------
	// 角色通用属性相关
	//-----------------------------------------------------------------------------
	bool		IsHaveBagPsd()		const	{ return GetBagPsd() != GT_INVALID; }

	//-----------------------------------------------------------------------------
	// 角色相关
	//-----------------------------------------------------------------------------
	BOOL		FullLogin(Role* pRole, BOOL bFirst);
	VOID		LogoutPlayer();
	VOID		Refresh();

	//-----------------------------------------------------------------------------
	// 网络命令相关和GM命令
	//----------------------------------------------------------------------------
	static VOID RegisterAllPlayerCmd();
	static VOID RegisterALLSendCmd();
	static VOID UnRegisterALL();

	
	//-----------------------------------------------------------------------------
	// GM命令相关
	//-----------------------------------------------------------------------------
	BOOL		IsPrivilegeEnough(BYTE byPrivilege) const { return byPrivilege <= m_byPrivilege; }

	//-----------------------------------------------------------------------
	// 回收消息Msg(有用则再利用，无用则释放)
	//-----------------------------------------------------------------------
	VOID		RecycleMsg(LPBYTE pMsg);

	//-----------------------------------------------------------------------------
	// 更新
	//----------------------------------------------------------------------------
	INT			Update();

private:
	//-----------------------------------------------------------------------
	// 需要在所有地图线程上层处理的消息注册
	//-----------------------------------------------------------------------
	static VOID	RegisterWorldMsg(LPCSTR szCmd, NETMSGHANDLER fp, LPCTSTR szDesc, DWORD dwSize);

	//-----------------------------------------------------------------------
	// 消息处理相关
	//-----------------------------------------------------------------------
	VOID			SendSmallMessage(LPVOID pMsg, DWORD dwSize);
	VOID			SendLargeMessage(LPVOID pMsg, DWORD dwSize);

	//-----------------------------------------------------------------------
	// 底层包相关
	//-----------------------------------------------------------------------
	LPBYTE			RecvMsg(DWORD& dwSize);
	VOID			ReturnMsg(LPBYTE pMsg);
	VOID			SendMsg(LPBYTE pMsg, DWORD dwSize);

	//-----------------------------------------------------------------------
	// 选人界面相关
	//-----------------------------------------------------------------------
	BOOL			IsRoleExist(const DWORD dwRoleID) const;
	BOOL			AddRole(const DWORD dwRoleID);
	BOOL			RemoveRole(const DWORD dwRoleID);
	BOOL			CanSetSafeCode();
	BOOL			CanResetSafeCode() const;
	BOOL			CanCancelSafeCodeReset() const;

	//-----------------------------------------------------------------------
	// 帐号下角色通用属性相关操作
	//-----------------------------------------------------------------------
	const DWORD		GetBagPsd()		const { return m_sAccountCommon.dwBagPsdCrc; }
	const DWORD		GetSafeCode()	const { return m_sAccountCommon.sSafeCode.dwSafeCodeCrc; }

	VOID			SetBagPsd(DWORD dwNewPsdCrc);

	/************************************************************************
	** Handlers -- map thread
	*************************************************************************/

	//-----------------------------------------------------------------------
	// 进入游戏
	//-----------------------------------------------------------------------
	DWORD	HandleJoinGame(tagNetCmd* pCmd);

	//-----------------------------------------------------------------------
	// 角色创建、删除、获取及选择
	//-----------------------------------------------------------------------
	DWORD	HandleRoleCreate(tagNetCmd* pCmd);
	DWORD	HandleRoleEnum(tagNetCmd* pCmd);
	DWORD	HandleRoleDelete(tagNetCmd* pCmd);
	DWORD	HandleRoleSelect(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 安全码处理
	//------------------------------------------------------------------------
	DWORD	HandleRoleSetSafeCode(tagNetCmd* pCmd);
	DWORD	HandleRoleResetSafeCode(tagNetCmd* pCmd);
	DWORD	HandleRoleCancelSafeCodeReset(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 人物属性获取
	//------------------------------------------------------------------------
	DWORD	HandleGetRoleInitAtt(tagNetCmd* pCmd);
	DWORD	HandleGetRemoteUnitAtt(tagNetCmd* pCmd);
	DWORD	HandleChangeRoleName(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 行走
	//------------------------------------------------------------------------
	DWORD	HandleRoleWalk(tagNetCmd* pCmd);
	DWORD	HandleRoleStopWalk(tagNetCmd* pCmd);
	DWORD	HandleRoleJump(tagNetCmd* pCmd);
	DWORD	HandleRoleDrop(tagNetCmd* pCmd);
	DWORD	HandleRoleVDrop(tagNetCmd* pCmd);
	DWORD	HandleRoleSlide(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 装备相关
	//------------------------------------------------------------------------
	DWORD	HandleRoleEquip(tagNetCmd* pCmd);
	DWORD	HandleRoleUnequip(tagNetCmd* pCmd);
	DWORD	HandleRoleSwapWeapon(tagNetCmd* pCmd);
	DWORD	HandleRoleIdentifyEquip(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 角色外观显示设置
	//------------------------------------------------------------------------
	DWORD	HandleRoleSetFashion(tagNetCmd* pCmd);
	DWORD	HandleRoleSetDisplay(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 物品操作相关
	//------------------------------------------------------------------------
	DWORD	HandleRoleChangeItemPos(tagNetCmd* pCmd);
	DWORD	HandleRoleChangeItemPosEx(tagNetCmd* pCmd);
	DWORD	HandleRoleReorderItem(tagNetCmd* pCmd);
	DWORD	HandleRoleReorderItemEx(tagNetCmd* pCmd);
	DWORD	HandleRoleSetTransportMap(tagNetCmd* pCmd); //使用记录式传送符
	DWORD	HandleRoleUseTransportMap(tagNetCmd* pCmd); //使用记录式传送符
	//------------------------------------------------------------------------
	// 玩家间交易相关
	//------------------------------------------------------------------------
	DWORD	HandleRoleExchangeReq(tagNetCmd* pCmd);
	DWORD	HandleRoleExchangeReqRes(tagNetCmd* pCmd);
	DWORD	HandleRoleExchangeAdd(tagNetCmd* pCmd);
	DWORD	HandleRoleExchangeDec(tagNetCmd* pCmd);
	DWORD	HandleRoleExchangeMoney(tagNetCmd* pCmd);
	DWORD	HandleRoleExchangeLock(tagNetCmd* pCmd);
	DWORD	HandleRoleExchangeCancel(tagNetCmd* pCmd);
	DWORD	HandleRoleExchangeVerify(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 商店相关
	//------------------------------------------------------------------------
	DWORD	HandleRoleGetShopItems(tagNetCmd* pCmd);
	DWORD	HandleRoleGetShopEquips(tagNetCmd* pCmd);
	DWORD	HandleRoleBuyShopItem(tagNetCmd* pCmd);
	DWORD	HandleRoleBuyShopEquip(tagNetCmd* pCmd);
	DWORD	HandleRoleSellToShop(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 摆摊相关
	//------------------------------------------------------------------------
	DWORD	HandleRoleStallStart(tagNetCmd* pCmd);
	DWORD	HandleRoleStallSetGoods(tagNetCmd* pCmd);
	DWORD	HandleRoleStallUnsetGoods(tagNetCmd* pCmd);
	DWORD	HandleRoleStallSetTitle(tagNetCmd* pCmd);
	DWORD	HandleRoleStallSetAdText(tagNetCmd* pCmd);
	DWORD	HandleRoleStallSetAdFlag(tagNetCmd* pCmd);
	DWORD	HandleRoleStallSetFinish(tagNetCmd* pCmd);
	DWORD	HandleRoleStallClose(tagNetCmd* pCmd);
	DWORD	HandleRoleStallGet(tagNetCmd* pCmd);
	DWORD	HandleRoleStallGetTitle(tagNetCmd* pCmd);
	DWORD	HandleRoleStallBuy(tagNetCmd* pCmd);
	DWORD	HandleRoleStallGetSpec(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 驿站&乾坤石
	//------------------------------------------------------------------------
	DWORD	HandleRoleDak(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 使用磨石
	//------------------------------------------------------------------------
	DWORD	HandleRoleAbrase(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 角色仓库
	//------------------------------------------------------------------------
	DWORD	HandleRoleSideWareOpen(tagNetCmd* pCmd);
	DWORD	HandleRoleWareExtend(tagNetCmd* pCmd);
	DWORD	HandleRoleBagExtand(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 角色仓库中存取金钱&元宝
	//------------------------------------------------------------------------
	DWORD	HandleRoleSaveSilver(tagNetCmd* pCmd);
	DWORD	HandleRoleGetSilver(tagNetCmd* pCmd);
	//DWORD	HandleRoleSaveYuanBao(tagNetCmd* pCmd);
	DWORD	HandleRoleGetYuanBao(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 行囊加密相关
	//------------------------------------------------------------------------
	DWORD	HandleRoleSetBagPsd(tagNetCmd* pCmd);
	DWORD	HandleRoleUnsetBagPsd(tagNetCmd* pCmd);
	DWORD	HandleRoleCheckBagPsd(tagNetCmd* pCmd);
	DWORD	HandleRoleResetBagPsd(tagNetCmd* pCmd);
	DWORD	HandleRoleOpenBagPsd(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 测试
	//------------------------------------------------------------------------
	DWORD	HandleRoleEnterWorld(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 聊天
	//------------------------------------------------------------------------
	DWORD   HandleRoleChat(tagNetCmd* pCmd);
	DWORD   HandleRoleGetID(tagNetCmd* pCmd);
	DWORD	HandleRoleGetNameByNameID(tagNetCmd* pCmd);
	DWORD	HandleRoleGetSomeName(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 装备展示
	//------------------------------------------------------------------------
	DWORD   HandleRoleShowEquip(tagNetCmd* pCmd);
	//------------------------------------------------------------------------
	// 物品展示
	//------------------------------------------------------------------------
	DWORD   HandleRoleShowItem(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 读取留言
	//------------------------------------------------------------------------
	DWORD   HandleRoleLoadLeftMsg(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 地图事件
	//------------------------------------------------------------------------
	DWORD	HandleRoleMapTrigger(tagNetCmd* pCmd);
	DWORD	HandleRoleInstanceNotify(tagNetCmd* pCmd);
	DWORD	HandleRoleInstanceAgree(tagNetCmd* pCmd);
	DWORD	HandleRoleLeaveInstance(tagNetCmd* pCmd);

	//-------------------------------------------------------------------------
	// 属性点相关
	//-------------------------------------------------------------------------
	DWORD	HandleRoleBidAttPoint(tagNetCmd* pCmd);
	DWORD	HandleRoleClearAttPoint(tagNetCmd* pCmd);

	//-------------------------------------------------------------------------
	// 天资技能相关
	//------------------------------------------------------------------------
	DWORD	HandleRoleLearnSkill(tagNetCmd* pCmd);
	DWORD	HandleRoleLevelUpSkill(tagNetCmd* pCmd);
	DWORD	HandleRoleForgetSkill(tagNetCmd* pCmd);
	DWORD	HandleRoleClearTalent(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 战斗系统
	//------------------------------------------------------------------------
	DWORD	HandleRoleEnterCombat(tagNetCmd* pCmd);
	DWORD	HandleRoleLeaveCombat(tagNetCmd* pCmd);
	DWORD	HandleRoleSkill(tagNetCmd* pCmd);
	DWORD	HandleRoleInterruptSkill(tagNetCmd* pCmd);

	DWORD	HandleRoleCancelBuff(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// PK系统
	//------------------------------------------------------------------------
	DWORD	HandleRoleSafeGuard(tagNetCmd* pCmd);
	DWORD	HandleRoleSetPK(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 复活
	//------------------------------------------------------------------------
	DWORD	HandleRoleBindRebornMap(tagNetCmd* pCmd);
	DWORD	HandleRoleRevive(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 通用函数 -- 需在地图线程上层处理的消息
	//------------------------------------------------------------------------
	DWORD   HandleRoleMsg2World(tagNetCmd* pCmd) { return RET_TRANS; }

	//------------------------------------------------------------------------
	// 通用命令 -- GM
	//------------------------------------------------------------------------
	DWORD   HandleGMCommand(tagNetCmd* pCmd);
	
	//------------------------------------------------------------------------
	// 任务相关
	//------------------------------------------------------------------------
	DWORD	HandleRoleNPCAcceptQuest(tagNetCmd* pCmd);
	DWORD	HandleRoleTriggerAcceptQuest(tagNetCmd* pCmd);
	DWORD	HandleRoleCompleteQuest(tagNetCmd* pCmd);
	DWORD   HandleRoleDeleteQuest(tagNetCmd* pCmd);
	DWORD	HandleRoleUpdateQuestNPCTalk(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 物品强化
	//------------------------------------------------------------------------
	DWORD	HandleRolePosyEquip(tagNetCmd* pCmd);
	DWORD	HandleRoleEngraveEquip(tagNetCmd* pCmd);
	DWORD	HandleRoleInlayEquip(tagNetCmd* pCmd);
	DWORD	HandleRoleBrandEquip(tagNetCmd* pCmd);
	DWORD	HandleRoleLoongSoul(tagNetCmd* pCmd);
	DWORD	HandleRoleQuench(tagNetCmd* pCmd);
	DWORD	HandleRoleChisel(tagNetCmd* pCmd);
	DWORD	HandleRoleDyeFashion(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 生产合成物品
	//------------------------------------------------------------------------
	DWORD	HandleRoleProduceItem(tagNetCmd* pCmd);

	// Jason 2009-12-6
	DWORD	HandleRoleAddEquipPotVal(tagNetCmd* pCmd);
	// Jason 2009-12-7
	DWORD	HandleLessingLoong(tagNetCmd * pCmd);

	// Jason 2009-12-8 点金石
	DWORD HandleGoldStone(tagNetCmd * pCmd);

	// Jason v1.3.1 2009-12-21
	DWORD HandleGemRemoval(tagNetCmd * pCmd);
	DWORD HandleGetGemRemovalInfo(tagNetCmd * pCmd);

	//------------------------------------------------------------------------
	// 点化, 装备分解
	//------------------------------------------------------------------------
	DWORD	HandleRoleDeCompose(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 使用物品
	//------------------------------------------------------------------------
	DWORD	HandleRoleUseItem(tagNetCmd* pCmd);
	DWORD	HandleRoleInterruptUseItem(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 拾取物品
	//------------------------------------------------------------------------
	DWORD	HandleRolePickUpItem(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 扔掉物品
	//------------------------------------------------------------------------
	DWORD	HandleRolePutDownItem(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 好友相关
	//------------------------------------------------------------------------
	DWORD   HandleRoleMakeFriend(tagNetCmd* pCmd);
	DWORD	HandleRoleCancelFriend(tagNetCmd* pCmd);
	DWORD	HandleUpdateFriendGroup(tagNetCmd* pCmd);
	DWORD	HandleMoveBlackList(tagNetCmd* pCmd);
	DWORD	HandleDeleteBlackList(tagNetCmd* pCmd);
	DWORD	HandleRoleSendGift(tagNetCmd* pCmd);
	DWORD	HandleRoleSendGiftReply(tagNetCmd* pCmd);
	DWORD   HandleUpdateFriOnline(tagNetCmd* pCmd);
	//Jason 2009-11-25 入队申请相关协议
	DWORD	HandleApplyJoinTeam(tagNetCmd *pCmd);
	DWORD	HandleReplyForApplingToJoinTeam(tagNetCmd * pCmd);

	//------------------------------------------------------------------------
	// 小队相关
	//------------------------------------------------------------------------
	DWORD   HandleRoleJoinTeam(tagNetCmd* pCmd);
	DWORD	HandleRoleJoinTeamReply(tagNetCmd* pCmd);
	DWORD	HandleRoleKickMember(tagNetCmd* pCmd);
	DWORD	HandleRoleLeaveTeam(tagNetCmd* pCmd);
	DWORD	HandleRoleSetPickMol(tagNetCmd* pCmd);
	DWORD	HandleRoleChangeLeader(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 称号相关
	//------------------------------------------------------------------------
	DWORD HandleRoleActiveTitle(tagNetCmd* pCmd);
	DWORD HandleRoleGetTitles(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 名帖相关
	//------------------------------------------------------------------------
	DWORD HandleRoleGetVCard(tagNetCmd* pCmd);
	DWORD HandleRoleSetVCard(tagNetCmd* pCmd);
	DWORD HandleGetHeadPicUrl(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 防沉迷相关
	//------------------------------------------------------------------------
	DWORD HandleGetFatigueInfo(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 名人堂相关
	//------------------------------------------------------------------------
	DWORD HandleGetFameHallRoles(tagNetCmd* pCmd);
	DWORD HandleGetReputeTop(tagNetCmd* pCmd);
	DWORD HandleGetActClanTreasure(tagNetCmd* pCmd);
	DWORD HandleActiveTreasure(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 声望相关
	//------------------------------------------------------------------------
	DWORD HandleGetRoleClanData(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 反外挂相关
	//------------------------------------------------------------------------
	DWORD HandleGameGuarderMsg(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 宠物相关
	//------------------------------------------------------------------------
	DWORD HandleGetPetAttr(tagNetCmd* pCmd);
	DWORD HandlePetSkill(tagNetCmd* pCmd);
	DWORD HandlePetSkillStopWorking(tagNetCmd* pCmd);
	DWORD HandleUsePetEgg(tagNetCmd* pCmd);
	DWORD HandleDeletePet(tagNetCmd* pCmd);
	DWORD HandleGetPetDispInfo(tagNetCmd* pCmd);
	DWORD HandleSetPetState(tagNetCmd* pCmd);

	DWORD HandlePetEquip(tagNetCmd* pCmd);
	DWORD HandlePetUnEquip(tagNetCmd* pCmd);
	DWORD HandlePetSwapEquipPos(tagNetCmd* pCmd);
	DWORD HandleGetPetPourExpMoneyNeed(tagNetCmd* pCmd);
	DWORD HandlePetPourExp(tagNetCmd* pCmd);
	DWORD HandlePetUpStep(tagNetCmd* pCmd);
	DWORD HandlePetEnhance(tagNetCmd* pCmd);
	DWORD HandlePetLearnSkill(tagNetCmd* pCmd);
	DWORD HandlePetInvite(tagNetCmd* pCmd);
	DWORD HandlePetOnInvite(tagNetCmd* pCmd);
	DWORD HandlePetFood(tagNetCmd* pCmd);
	DWORD HandlePetSetLock(tagNetCmd* pCmd);
	

	DWORD	HandleRolePetExchangeReq(tagNetCmd* pCmd);
	DWORD	HandleRolePetExchangeReqRes(tagNetCmd* pCmd);
	DWORD	HandleRolePetExchangeAdd(tagNetCmd* pCmd);
	DWORD	HandleRolePetExchangeDec(tagNetCmd* pCmd);
	DWORD	HandleRolePetExchangeMoney(tagNetCmd* pCmd);
	DWORD	HandleRolePetExchangeLock(tagNetCmd* pCmd);
	DWORD	HandleRolePetExchangeCancel(tagNetCmd* pCmd);
	DWORD	HandleRolePetExchangeVerify(tagNetCmd* pCmd);


	//------------------------------------------------------------------------
	// 百宝袋相关
	//------------------------------------------------------------------------
	DWORD HandleInitBaiBaoRecord(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// VIP摊位相关
	//------------------------------------------------------------------------
	DWORD HandleGetAllVIPStallInfo(tagNetCmd* pCmd);
	DWORD HandleUpdateVIPStallInfo(tagNetCmd* pCmd);
	DWORD HandleApplyVIPStall(tagNetCmd* pCmd);
	DWORD HandleSpecVIPStallGet(tagNetCmd* pCmd);
	DWORD HandleBuyVIPStallGoods(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// NPC相关
	//------------------------------------------------------------------------
	DWORD HandleTalkToNPC(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 金牌网吧相关
	//------------------------------------------------------------------------
	DWORD HandleGetVipNetBarName(tagNetCmd* pCmd);


	//结婚
	DWORD HandleGetMarriageReq(tagNetCmd* pCmd);
	DWORD HandleGetMarriageReqRes(tagNetCmd* pCmd);
	//离婚
	DWORD HandleBreakMarriageReq(tagNetCmd* pCmd);
	DWORD HandleBreakMarriageReqRes(tagNetCmd* pCmd);
	//婚礼
	DWORD HandleCelebrateWeddingReq(tagNetCmd* pCmd);
	DWORD HandleCelebrateWeddingReqRes(tagNetCmd* pCmd);
	//------------------------------------------------------------------------
	// 个性动作
	//------------------------------------------------------------------------
	DWORD	HandleRoleStyleAction(tagNetCmd* pCmd);
	DWORD	HandleRoleDuetMotionInvite(tagNetCmd* pCmd);
	DWORD	HandleRoleDuetMotionOnInvite(tagNetCmd* pCmd);
	DWORD	HandleRoleDuetMotionStart(tagNetCmd* pCmd);


	/************************************************************************
	** Handlers -- upper all map thread
	*************************************************************************/

	//------------------------------------------------------------------------
	// 商城相关
	//------------------------------------------------------------------------
	DWORD HandleRoleMallGet(tagNetCmd* pCmd);
	DWORD HandleRoleMallUpdate(tagNetCmd* pCmd);
	DWORD HandleRoleMallBuyItem(tagNetCmd* pCmd);
	DWORD HandleRoleMallBuyPack(tagNetCmd* pCmd);
	DWORD HandleRoleMallPresentItem(tagNetCmd* pCmd);
	DWORD HandleRoleMallPresentPack(tagNetCmd* pCmd);
	DWORD HandleRoleMallFreeGetItem(tagNetCmd* pCmd);
	DWORD HandleRoleMallLaunchGroupPurchase(tagNetCmd* pCmd);
	DWORD HandleRoleMallRespondGroupPurchase(tagNetCmd* pCmd);
	DWORD HandleRoleMallGetGroupPurchaseInfo(tagNetCmd* pCmd);
	DWORD HandleRoleMallGetParticipators(tagNetCmd* pCmd);
	DWORD HandleRoleMallItemExchange(tagNetCmd* pCmd);
	DWORD HandleRoleMallPackExchange(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 元宝交易相关
	//------------------------------------------------------------------------
	DWORD HandleRoleSaveYB2Account(tagNetCmd* pCmd);
	DWORD HandleRoleSaveSilver2Account(tagNetCmd* pCmd);
	DWORD HandleRoleDepositYBAccount(tagNetCmd* pCmd);
	DWORD HandleRoleDepositSilver(tagNetCmd* pCmd);
	DWORD HandleRoleGetYBTradeInfo(tagNetCmd* pCmd);
	DWORD HandleRoleSubmitSellOrder(tagNetCmd* pCmd);
	DWORD HandleRoleSubmitBuyOrder(tagNetCmd* pCmd);
	DWORD HandleRoleDeleteOrder(tagNetCmd* pCmd);
	DWORD HandleRoleGetYBOrder(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 帮派相关
	//------------------------------------------------------------------------
	DWORD HandleCreateGuild(tagNetCmd* pCmd);
	DWORD HandleDismissGuild(tagNetCmd* pCmd);
	DWORD HandleJoinGuildReq(tagNetCmd* pCmd);
	DWORD HandleJoinGuildReqRes(tagNetCmd* pCmd);
	DWORD HandleLeaveGuild(tagNetCmd* pCmd);
	DWORD HandleKickFromGuild(tagNetCmd* pCmd);
	DWORD HandleTurnoverGuild(tagNetCmd* pCmd);
	DWORD HandleDemissFromGuild(tagNetCmd* pCmd);
	DWORD HandleAppointForGuild(tagNetCmd* pCmd);
	DWORD HandleChangeGuildTenet(tagNetCmd* pCmd);

	DWORD HandleSyncGuildInfo(tagNetCmd* pCmd);

	DWORD HandleGetGuildMembers(tagNetCmd* pCmd);
	DWORD HandleGetGuildMemberEx(tagNetCmd* pCmd);
	DWORD HandleRefreshGuildMember(tagNetCmd* pCmd);
	DWORD HandleGetGuildName(tagNetCmd* pCmd);
	DWORD HandleGetGuildTenet(tagNetCmd* pCmd);

	DWORD HandleGetGuildWareItems(tagNetCmd* pCmd);
	DWORD HandleGetGuildWarePriList(tagNetCmd* pCmd);
	DWORD HandleGuildWarePrivilege(tagNetCmd* pCmd);

	DWORD HandleGetGuildFacilitiesInfo(tagNetCmd* pCmd);
	DWORD HandleHandInItems(tagNetCmd* pCmd);

	DWORD HandleSpreadGuildAffair(tagNetCmd* pCmd);

	DWORD HandleGetGuildSkillInfo(tagNetCmd* pCmd);
	DWORD HandleUpgradeGuildSkill(tagNetCmd* pCmd);
	DWORD HandleLearnGuildSkill(tagNetCmd* pCmd);
	DWORD HandleSetResearchSkill(tagNetCmd* pCmd);

	DWORD HandleGetCofCInfo(tagNetCmd* pCmd);
	DWORD HandleCloseCofC(tagNetCmd* pCmd);
	DWORD HandleBuyCofCGoods(tagNetCmd* pCmd);
	DWORD HandleSellCofCGoods(tagNetCmd* pCmd);
	DWORD HandleGetCommodityInfo(tagNetCmd* pCmd);
	DWORD HandleGetTaelInfo(tagNetCmd* pCmd);
	DWORD HandleGetCommerceRank(tagNetCmd* pCmd);

	DWORD HandleAcceptCommerce(tagNetCmd* pCmd);
	DWORD HandleCompleteCommerce(tagNetCmd* pCmd);
	DWORD HandleAbandonCommerce(tagNetCmd* pCmd);
	DWORD HandleSwitchCommendation(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 开启宝箱相关
	//------------------------------------------------------------------------
	DWORD HandleOpenTreasureChest(tagNetCmd* pCmd);
	DWORD HandleStopTreasureChest(tagNetCmd* pCmd);
	DWORD HandleAgainTreasureChest(tagNetCmd* pCmd);
	DWORD HandleChestGetItem(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 返回角色选择
	//------------------------------------------------------------------------
	DWORD HandleReturnRoleSelect(tagNetCmd* pCmd);

	// 客户端对话框发给服务的缺省消息
	DWORD HandleDlgDefaultMsg(tagNetCmd* pCmd);
	// 客户端触发服务器脚本的缺省消息
	DWORD HandleDefaultRequest(tagNetCmd* pCmd);

	// 获得其他玩家的装备信息
	DWORD HandleGetSomeoneEquip(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 职业相关
	//------------------------------------------------------------------------
	DWORD HandleSetClass(tagNetCmd* pCmd);
	DWORD HandleChangeClass(tagNetCmd* pCmd);

	//------------------------------------------------------------------------
	// 实力系统相关
	//------------------------------------------------------------------------
	DWORD HandleGetRankings(tagNetCmd* pCmd);
	DWORD HandleGetSelfStrength(tagNetCmd* pCmd);
	DWORD HandleGetStrengthRankings(INT nPage);
	DWORD HandleGetFameRankings(INT nPage);
	DWORD HandleGetLevelRankings(INT nPage);
	//DWORD HandleGetMoneyRankings();

	//------------------------------------------------------------------------
	// 离线提示
	//------------------------------------------------------------------------
	DWORD HandleRoleLeaveNotify(tagNetCmd* pCmd);

private:
	static PlayerNetCmdMgr	m_PlayerNetMgr;						// 对应的客户端消息管理器
	static GMCommandMgr		m_GMCommandMgr;						// GM命令管理器

	DWORD					m_dwAccountID;						// session id，对应的是帐号ID
	DWORD					m_dwInternalIndex;					// 底层的网络ID
	INT						m_nMsgNum;							// 网络底层未处理的消息数量

	bool					m_bRoleLoading;						// 选择人物时正在等待数据库返回
	bool					m_bRoleEnuming;						// 进入游戏时等待服务器选人将角色信息读取出来
	bool					m_bRoleEnumDone;					// 读取角色信息完毕
	bool					m_bRoleEnumSuccess;					// 读取角色信息是否成功
	bool					m_bRoleCreating;					// 等待创建角色
	bool					m_bRoleDeleting;					// 删除人物时等待数据库返回
	BYTE					m_byPrivilege;						// gm权限
	INT8					m_n8RoleNum;						// 已经创建的角色个数

	bool					m_bRoleInWorld;						// 在游戏世界中

	DWORD					m_dwRoleID[MAX_ROLENUM_ONEACCOUNT];	// 帐号下所有角色ID
	
	char					m_szAccount[X_SHORT_NAME];			// 玩家账号

	volatile BOOL			m_bConnectionLost;					// 连接是否已经断开
	volatile BOOL			m_bKicked;							// 是否已经被踢掉

	tagAccountCommon		m_sAccountCommon;					// 账号通用信息

	Role*					m_pRole;							// 对应的角色对象

	DWORD					m_dwIP;								// 客户端IP

	FatigueGuarder			m_FatigueGarder;					// 防沉迷


	tagDWORDTime			m_dwPreLoginTime;					// 上次登录时间
	DWORD					m_dwPreLoginIP;						// 上次登录ip

	FastMutex				m_PlayerNetLock;
};


