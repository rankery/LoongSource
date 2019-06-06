//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: gm_net_cmd_mgr.h
// author: Sxg
// actor:
// data: 2008-11-05
// last:
// brief: GM 命令处理
//-----------------------------------------------------------------------------
#pragma once

class Role;
//-----------------------------------------------------------------------------
class GMCommandMgr
{
	typedef DWORD (GMCommandMgr::*GMCMDHANDLE)(const std::vector<DWORD>&, Role*);

public:
	GMCommandMgr();
	~GMCommandMgr();

public:
	DWORD Excute(LPCTSTR szCommand, Role *pRole);
	VOID RegisterAll();
	VOID UnRegisterAll();

public:
	VOID LogGMCmd(DWORD dwRoleID, LPCTSTR szCmd, DWORD dwErrorCode);
//public:
//	VOID SetGMCmdID(DWORD dwCmdID) { if(GT_INVALID == m_dwGMCmdID) { m_dwGMCmdID = dwCmdID;} }

private:
	VOID Register(LPCTSTR szName, GMCMDHANDLE pFun, BYTE byPrivilege, LPCTSTR szDesc=NULL, BYTE byParamNum=1);

private: // GM 命令处理函数
	// GM自身
	DWORD HandleCreateItem(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleGetSilver(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleGetYuanBao(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleGetExVolume(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleClearBag(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleGoto(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleGotoRole(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleFillExp(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleFillLevel(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleEquipPotInc(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleAddQuest(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleClearSkillCD(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleCoolOff(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleCoolOn(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleChangeRoleAtt(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleSetReputation(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleRoleQuest(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleClearRoleQuest(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleAddBuff(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleSetVocation(const std::vector<DWORD>& vectParam, Role* pGM);

	// 对玩家
	DWORD HandleMoveRole(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleKickRole(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleSelectRole(const std::vector<DWORD>& vectParam, Role* pGM);

	// 脚本
	DWORD HandleReloadScripts(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleChangeScriptData(const std::vector<DWORD>& vectParam, Role* pGM);

	// 游戏世界
	DWORD HandleDouble(const std::vector<DWORD>& vectParam, Role* pGM);

	// 调整可登陆玩家人数
	DWORD HandleResizeOnlineNum(const std::vector<DWORD>& vectParam, Role* pGM);

	// 测试氏族数据用
	DWORD HandleClanData(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleVipNetBar(const std::vector<DWORD>& vectParam, Role* pGM);

	// 测试宠物
	DWORD HandlePet(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleAddSkill(const std::vector<DWORD>& vectParam, Role* pGM);

	// 帮派测试
	DWORD HandleGetFund(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleGetMaterial(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleGetContribute(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleResetAffairTimes(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleGetTael(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleGuildStatus(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleGuildFacility(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleRoleGuild(const std::vector<DWORD>& vectParam, Role* pGM);

	// 帮派团购测试用
	DWORD HandleLaunchGP(const std::vector<DWORD>& vectParam, Role* pGM);	
	DWORD HandleRespondGP(const std::vector<DWORD>& vectParam, Role* pGM);	

	// 测试名帖
	DWORD HandleVCard(const std::vector<DWORD>& vectParam, Role* pGM);

	// 测试称号
	DWORD HandleTitle( const std::vector<DWORD>& vectParam, Role* pGM );

	// 在玩家当前坐标刷出怪物　
	DWORD HandleCreateMonster( const std::vector<DWORD>& vectParam, Role* pGM);

	// 是否计算技能的触发率
	DWORD HandleTriggerOff(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleTriggerOn(const std::vector<DWORD>& vectParam, Role* pGM);

	// 装备強化
	DWORD HandleEngrave(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandlePosy(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleLoongSoul(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleAddAtt2Weapon(const std::vector<DWORD>& vectParam, Role* pGM);
	
	// 调整生产技能熟练度
	DWORD HandleProficiency(const std::vector<DWORD>& vectParam, Role* pGM);

	// 增加装备套装
	DWORD HandleAddSuit(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleAddEquip(const std::vector<DWORD>& vectParam, Role* pGM);

	DWORD HandleLeftMsg(const std::vector<DWORD>& vectParam, Role* pGM);

	// 测试宝箱节点
	DWORD HandleChangeRoleChestSum(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleChangeServerChestSum(const std::vector<DWORD>& vectParam, Role* pGM);

	// 开关门
	DWORD HandleOpenCloseDoor(const std::vector<DWORD>& vectParam, Role* pGM);
	
	DWORD HandleLurk(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleInvincible(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleRolePosition(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleNoSpeak(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleCancelNoSpeak(const std::vector<DWORD>& vectParam, Role* pGM);
	DWORD HandleKillMonster(const std::vector<DWORD>& vectParam, Role* pGM);

	// 摊位经验
	DWORD HandleStallExp(const std::vector<DWORD>& vectParam, Role* pGM);

private:
	struct tagGMCommand
	{
		tstring		strCmd;		// 命令名
		tstring		strDesc;	// 描述
		GMCMDHANDLE	handler;	// 函数指针
		BYTE		byParamNum;	// 参数个数
		BYTE		byPrivilege;// GM命令权限
		INT16		n16ExeTimes;// 执行次数
	};

	TObjRef<Util>				m_pUtil;
	//DWORD						m_dwGMCmdID;

	TMap<DWORD, tagGMCommand*>	m_mapGMCommand;
};