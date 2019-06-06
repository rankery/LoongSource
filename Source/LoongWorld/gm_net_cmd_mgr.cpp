//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: gm_net_cmd_mgr.cpp
// author: Sxg
// actor:
// data: 2008-11-05
// last:
// brief: GM 命令处理
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "gm_net_cmd_mgr.h"
#include "role.h"
#include "clandata.h"
#include "chat_mgr.h"
#include "role_mgr.h"
#include "../ServerDefine/msg_log.h"

//-----------------------------------------------------------------------------
// 构造&析构
//-----------------------------------------------------------------------------
GMCommandMgr::GMCommandMgr()
{
	//m_dwGMCmdID = GT_INVALID;
}

GMCommandMgr::~GMCommandMgr(){}

//-----------------------------------------------------------------------------
// 注册一条GM命令(注: 第一个参数必须为小写)
//-----------------------------------------------------------------------------
VOID GMCommandMgr::Register(LPCTSTR szName, GMCMDHANDLE pFun, BYTE byPrivilege, 
							LPCTSTR szDesc/*=NULL*/, BYTE byParamNum/*=1*/)
{
	DWORD dwID = m_pUtil->Crc32(szName);

	tagGMCommand* pCmd = m_mapGMCommand.Peek(dwID);
	if(P_VALID(pCmd))
	{
		ASSERT(0);	// 重复注册
		return;
	}

	pCmd = new tagGMCommand;
	pCmd->strCmd		= szName;		// 该字符串必须为小写
	pCmd->strDesc		= szDesc;
	pCmd->handler		= pFun;
	pCmd->byParamNum	= byParamNum;
	pCmd->byPrivilege	= byPrivilege;
	pCmd->n16ExeTimes	= 0;

	m_mapGMCommand.Add(dwID, pCmd);

	return;
}

//-----------------------------------------------------------------------------
// 注册所以GM命令(注意：注册GM命令时，命令名称必须为小写。如："item")
//-----------------------------------------------------------------------------
VOID GMCommandMgr::RegisterAll()
{
	Register(_T("addskill"),		&GMCommandMgr::HandleAddSkill,			4, _T("addskill dwTypeID"),					1);
	Register(_T("addbuff"),			&GMCommandMgr::HandleAddBuff,			2, _T("gm addbuff dwBuffID"),				1);
	Register(_T("addquest"),		&GMCommandMgr::HandleAddQuest,			4, _T("addquest nQuestId"),					1);
	Register(_T("addsuit"),			&GMCommandMgr::HandleAddSuit,			4, _T("gm addsuit dwSuitID nQlty"),			2);
	Register(_T("addequip"),		&GMCommandMgr::HandleAddEquip,			4, _T("gm addequip nType nLevel nQlty"),	3);
	Register(_T("affair"),			&GMCommandMgr::HandleResetAffairTimes,	4, _T("affair"),							0);
	Register(_T("att"),				&GMCommandMgr::HandleChangeRoleAtt,		4, _T("att eroleatt nval"),					2);
	Register(_T("addweaponatt"),	&GMCommandMgr::HandleAddAtt2Weapon,		4, _T("addweaponatt n16ItemIndex bAttType AttID"),		3);
	
	Register(_T("clandata"),		&GMCommandMgr::HandleClanData,			3, _T("clandata testNo clanType nVal"),		3);
	Register(_T("changescriptdata"),&GMCommandMgr::HandleChangeScriptData,	5, _T("changescriptdata nIndex dwValue"),	2);
	Register(_T("class"),			&GMCommandMgr::HandleSetVocation,		2, _T("class nType eVocation"),				2);
	Register(_T("clearbag"),		&GMCommandMgr::HandleClearBag,			1, _T("clearbag"),							0);
	Register(_T("cooloff"),			&GMCommandMgr::HandleCoolOff,			5, _T("gm cooloff"),						0);
	Register(_T("coolon"),			&GMCommandMgr::HandleCoolOn,			5, _T("gm coolon"),							0);
	Register(_T("contribute"),		&GMCommandMgr::HandleGetContribute,		4, _T("contribute nContribute"),			1);

	Register(_T("double"),			&GMCommandMgr::HandleDouble,			1, _T("double rate"),						3);
	
	Register(_T("exvolume"),		&GMCommandMgr::HandleGetExVolume,		2, _T("exvolume nExVolume"),				1);
	Register(_T("exp"),				&GMCommandMgr::HandleFillExp,			4, _T("exp nExp"),							1);
	Register(_T("equippot"),		&GMCommandMgr::HandleEquipPotInc,		4, _T("equippot nIndex nValue"),			2);
	Register(_T("engrave"),			&GMCommandMgr::HandleEngrave,			4, _T("engrave n16ItemIndex dwFormula"),	2);

	Register(_T("fill"),			&GMCommandMgr::HandleFillLevel,			4, _T("fill nLevel"),						1);
	Register(_T("fund"),			&GMCommandMgr::HandleGetFund,			4, _T("fund nFund"),						1);

	Register(_T("goto"),			&GMCommandMgr::HandleGoto,				1, _T("goto szMapName x z"),				3);
	Register(_T("gotorole"),		&GMCommandMgr::HandleGotoRole,			1, _T("gotorole szRoleName"),				1);
	Register(_T("guild"),			&GMCommandMgr::HandleGuildStatus,		4, _T("guild"),								2);
	Register(_T("guildfacility"),	&GMCommandMgr::HandleGuildFacility,		4, _T("guildfacility"),						2);
	
	Register(_T("item"),			&GMCommandMgr::HandleCreateItem,		4, _T("item dwTypeID nItemNum nQuality"),	3);
	Register(_T("invincible"),		&GMCommandMgr::HandleInvincible,		2, _T("invincible"),						0);
	
	Register(_T("kick"),			&GMCommandMgr::HandleKickRole,			2, _T("kick szRoleName"),					1);
	Register(_T("killmonster"),		&GMCommandMgr::HandleKillMonster,		3, _T("killmonster dwMonsterID"),			1);

	Register(_T("leftmsg"),			&GMCommandMgr::HandleLeftMsg,			0, _T("gm leftmsg nNum"),					1);
	Register(_T("launchgp"),		&GMCommandMgr::HandleLaunchGP,			1, _T("launch group purchase"),				4);
	Register(_T("loongsoul"),		&GMCommandMgr::HandleLoongSoul,			4, _T("loongsoul n16ItemIndex n64LoongSoulID"),		2);
	Register(_T("lurk"),			&GMCommandMgr::HandleLurk,				2, _T("Lurk nLurkLevel"),					1);

	Register(_T("moverole"),		&GMCommandMgr::HandleMoveRole,			1, _T("moverole szRoleName"),				1);
	Register(_T("material"),		&GMCommandMgr::HandleGetMaterial,		4, _T("material nMaterial"),				1);
	Register(_T("monster"),			&GMCommandMgr::HandleCreateMonster,		3, _T("create monster"),					1);
	Register(_T("maxonline"),		&GMCommandMgr::HandleResizeOnlineNum,	5, _T("maxonline nNum"),					3);

	Register(_T("noquestdone"),		&GMCommandMgr::HandleClearRoleQuest,	4, _T("noquestdone"),						0);
	Register(_T("nospeak"),			&GMCommandMgr::HandleNoSpeak,			2, _T("nospeak"),							0);

	Register(_T("pet"),				&GMCommandMgr::HandlePet,				1, _T("pet testNo petIndex petProtoID"),	3);
	Register(_T("posy"),			&GMCommandMgr::HandlePosy,				4, _T("posy n16ItemIndex dwFormula"),		2);
	Register(_T("proficiency"),		&GMCommandMgr::HandleProficiency,		4, _T("gm proficiency dwSkillID nValue"),	2);

	Register(_T("quest"),			&GMCommandMgr::HandleRoleQuest,			4, _T("quest dwQuestId bDone"),				2);

	//Register(_T("reputation"),	&GMCommandMgr::HandleSetReputation,		2, _T("reputation eClan nReputation"),		2);
	Register(_T("reloadscript"),	&GMCommandMgr::HandleReloadScripts,		5, _T("reload server scripts"),				0);
	Register(_T("roleguild"),		&GMCommandMgr::HandleRoleGuild,			4, _T("roleguild"),							2);
	Register(_T("respondgp"),		&GMCommandMgr::HandleRespondGP,			1, _T("respond group purchase"),			3);
	Register(_T("roleposition"),	&GMCommandMgr::HandleRolePosition,		2, _T("roleposition"),						1);

	Register(_T("skillcd"),			&GMCommandMgr::HandleClearSkillCD,		4, _T("skillcd talenttype exceptid	2"),	1);
	Register(_T("silver"),			&GMCommandMgr::HandleGetSilver,			4, _T("silver nGold nSilver"),				2);

	Register(_T("triggeroff"),		&GMCommandMgr::HandleTriggerOff,		4, _T("triggeroff"),						0);
	Register(_T("triggeron"),		&GMCommandMgr::HandleTriggerOn,			4, _T("triggeron"),							0);
	Register(_T("tael"),			&GMCommandMgr::HandleGetTael,			4, _T("tael nTael"),						1);
	Register(_T("title"),			&GMCommandMgr::HandleTitle,				3, _T("title event para1 para2"),			3);

	Register(_T("vnb"),				&GMCommandMgr::HandleVipNetBar,			1, _T("vnb testNo accountID ip"),			3);
	Register(_T("vcard"),			&GMCommandMgr::HandleVCard,				1, _T("vcard roleid"),						1);

	Register(_T("yuanbao"),			&GMCommandMgr::HandleGetYuanBao,		4, _T("yuanbao nYuanBao"),					1);

	// 测试宝箱节点
	Register(_T("changerolechest"),	&GMCommandMgr::HandleChangeRoleChestSum,1,	_T("change role chest sum"),			1);
	Register(_T("changeserverchest"),&GMCommandMgr::HandleChangeServerChestSum, 1,	_T("change server chest sum"),		1);

	Register(_T("speakoff"),		&GMCommandMgr::HandleNoSpeak,			3, _T("speakoff dwRoleID"),					1);
	Register(_T("speakon"),			&GMCommandMgr::HandleCancelNoSpeak,		3, _T("speakon dwRoleID"),					1);

	Register(_T("stallexp"),		&GMCommandMgr::HandleStallExp,			3, _T("stallexp nExp"),						1);
	/************************门相关***************************/
	Register(_T("door"),		&GMCommandMgr::HandleOpenCloseDoor,		0, _T("gm door ObjID bOpen"),	2);

	Register(_T("selectrole"),		&GMCommandMgr::HandleSelectRole,		0, _T("gm selectrole"),	0);

}

//------------------------------------------------------------------------------
// 取消注册
//------------------------------------------------------------------------------
VOID GMCommandMgr::UnRegisterAll()
{
	m_mapGMCommand.ResetIterator();
	tagGMCommand* pCmd = NULL;
	while(m_mapGMCommand.PeekNext(pCmd))
	{
		SAFE_DEL(pCmd);
	}

	m_mapGMCommand.Clear();
}

//-----------------------------------------------------------------------------
// 执行 command
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::Excute(LPCTSTR szCommand, Role *pRole)
{
	ASSERT(P_VALID(pRole));

	std::vector<tstring> command;
	m_pUtil->StringToToken(command, szCommand);
	command.resize(10);	// 不可能超过10个参数
	std::vector<DWORD> vp;

	if(command.size() == 0)
		return GT_INVALID;

	// 得到实际的参数
	for(INT n=1; n<(INT)command.size(); ++n)
	{
		vp.push_back((DWORD)command[n].c_str());
	}

	transform(command[0].begin(), command[0].end(), command[0].begin(), tolower);	
	DWORD dwID = m_pUtil->Crc32(command[0].c_str());	// 转为小写再运算

	tagGMCommand *pCmd = m_mapGMCommand.Peek(dwID);
	if(!P_VALID(pCmd))
	{
		IMSG(_T("Unknow GM command recved[%s]\r\n"), command[0].c_str());
		return GT_INVALID;
	}

	if(!P_VALID(pRole->GetSession()))
	{
		ASSERT(0);
		return GT_INVALID;
	}

	// 命令权限判断
	if(!pRole->GetSession()->IsPrivilegeEnough(pCmd->byPrivilege))
	{
		IMSG(_T("No enough privilege<RoleID:%u> to execute<gm %s>!!!!!!!!!!!!!\r\n"), 
			pRole->GetID(), pCmd->strCmd.c_str());
		return GT_INVALID;
	}

	if((BYTE)vp.size() < pCmd->byParamNum)
	{
		return GT_INVALID;
	}

	++pCmd->n16ExeTimes;

	DWORD dwRet = (this->*pCmd->handler)(vp, pRole);

	TCHAR szCmd[X_LONG_STRING] = {0};
	_stprintf(szCmd, _T("gm %s"), szCommand);

	LogGMCmd(pRole->GetID(), szCmd, dwRet);

	return dwRet;
}

DWORD GMCommandMgr::HandleLeftMsg( const std::vector<DWORD>& vectParam, Role* pGM )
{
	INT nType = _ttoi((LPCTSTR)vectParam[0]);

	if ( TRUE == g_msgMgr.LoadLeftMsg(pGM->GetID()) )
		return E_Success;
	else
		return E_SystemError;
}

VOID GMCommandMgr::LogGMCmd(DWORD dwRoleID, LPCTSTR szCmd, DWORD dwErrorCode)
{
	tagNDBC_LogGMCmd send;

	tagRoleInfo* pRoleInfo = g_roleMgr.GetRoleInfo(dwRoleID);
	if (!P_VALID(pRoleInfo))
	{
		return;
	}
	
	send.sLogGMCmd.dwRoleID	= dwRoleID;
	send.sLogGMCmd.dwErrorCode	= dwErrorCode;
	_tcscpy(send.sLogGMCmd.szGMCmd, szCmd);

	g_dbSession.Send(&send, send.dwSize);
}