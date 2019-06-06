//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_mgr.cpp
// author: Sxg
// actor:
// data: 2009-4-7
// last:
// brief: 帮派管理
//-------------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/filter.h"
#include "../WorldDefine/msg_guild.h"
#include "../ServerDefine/msg_guild.h"
#include "../ServerDefine/log_cmdid_define.h"

#include "guild_mgr.h"
#include "role_mgr.h"
#include "role.h"
#include "guild.h"
#include "script_mgr.h"
#include "mall.h"

GuildMgr g_guildMgr;
//-------------------------------------------------------------------------------
// 构造&析构
//-------------------------------------------------------------------------------
GuildMgr::GuildMgr()
{
	;
}

GuildMgr::~GuildMgr()
{
	Destroy();
}

//-------------------------------------------------------------------------------
// init
//-------------------------------------------------------------------------------
BOOL GuildMgr::Init()
{
	INT32 nTmp = 0;

	g_ScriptMgr.GetGlobal("guild_CreateRoleMinLevel", nTmp);
	m_GuildCfg.nCreateRoleMinLevel	= nTmp;

	g_ScriptMgr.GetGlobal("guild_GoldCreateNeeded", nTmp);
	m_GuildCfg.nGoldCreateNeeded	= nTmp;

	g_ScriptMgr.GetGlobal("guild_GuildLevelBegin", nTmp);
	m_GuildCfg.byGuildLevelBegin	= (BYTE)nTmp;

	g_ScriptMgr.GetGlobal("guild_GuildPeaceBegin", nTmp);
	m_GuildCfg.n16GuildPeaceBegin	= (INT16)nTmp;

	g_ScriptMgr.GetGlobal("guild_GuildRepBegin", nTmp);
	m_GuildCfg.nGuildRepBegin		= nTmp;

	g_ScriptMgr.GetGlobal("guild_GuildFundBegin", nTmp);
	m_GuildCfg.nGuildFundBegin		= nTmp;

	g_ScriptMgr.GetGlobal("guild_GuildMaterialBegin", nTmp);
	m_GuildCfg.nGuildMaterialBegin	= nTmp;

	g_ScriptMgr.GetGlobal("guild_GuildGroupPurchaseBegin", nTmp);
	m_GuildCfg.nGuildGroupPurchaseBegin = nTmp;

	g_ScriptMgr.GetGlobal("guild_JoinRoleMinLevel", nTmp);
	m_GuildCfg.nJoinRoleMinLevel	= nTmp;

	return TRUE;
}

//-------------------------------------------------------------------------------
// update
//-------------------------------------------------------------------------------
VOID GuildMgr::Update()
{
	Guild *pGuild;
	MapGuild::TMapIterator iter = m_mapGuild.Begin();
	while(m_mapGuild.PeekNext(iter, pGuild))
	{
		pGuild->Update();
	}
}

//-------------------------------------------------------------------------------
// destroy
//-------------------------------------------------------------------------------
VOID GuildMgr::Destroy()
{
	Guild *pGuild = NULL;
	MapGuild::TMapIterator iter = m_mapGuild.Begin();
	while(m_mapGuild.PeekNext(iter, pGuild))
	{
		SAFE_DEL(pGuild);
	}

	m_mapGuild.Clear();
}

//-------------------------------------------------------------------------------
// 和LoongDB连接后，所有帮派属性初始化
//-------------------------------------------------------------------------------
VOID GuildMgr::InitDBGuild(const tagGuildLoad* pGuildLoad)
{
	if(m_mapGuild.IsExist(pGuildLoad->dwID))
	{
		// LoongDB瞬断
		return;
	}
	
	Guild *pNewGuild = new Guild(pGuildLoad);
	if(!P_VALID(pNewGuild))
	{
		ASSERT(0);
		IMSG(_T("\n\n\nCaution:\tGuild(load from db: id=%u) create failed!\n\n"), pGuildLoad->dwID);
		return;
	}

	m_mapGuild.Add(pGuildLoad->dwID, pNewGuild);
}

//-------------------------------------------------------------------------------
// 和LoongDB连接后，所有帮派成员初始化
//-------------------------------------------------------------------------------
VOID GuildMgr::InitDBGuildMember(const tagGuildMemLoad *pGuildMemberLoad, const INT32 nNum)
{
	ASSERT(nNum > 0);

/*	Guild *pChkGuild = GetGuild(pGuildMemberLoad[0].dwGuildID);
	if(P_VALID(pChkGuild))
	{
		// 检查LoongDB是否是初次连接
		if(pChkGuild->GetGuildMemberNum() > 0)
		{
			ASSERT(0);
			IMSG(_T("\n\nCaution: \n\tLoongDB connected is not the first time! \n"));
			IMSG(_T("\tIf you want to reload db data, please run clear in console and restart LoongDB Server!\n\n"));
			return;
		}
	}
*/	
	for(INT32 i=0; i<nNum; ++i)
	{
		Guild *pGuild = GetGuild(pGuildMemberLoad[i].dwGuildID);
		if(!P_VALID(pGuild))
		{
			// 上层处理逻辑有问题，或数据库(未使用事务处理)出现问题
			ASSERT(0);
			continue;
		}

		pGuild->InitDBGuildMember(pGuildMemberLoad[i].sGuildMember);
	}
}

//-------------------------------------------------------------------------------
// 检查是否所有帮派都初始化成功
//-------------------------------------------------------------------------------
BOOL GuildMgr::IsGuildInitOK()
{
	BOOL bRet	= TRUE;

	Guild* pGuild	= NULL;
	MapGuild::TMapIterator iter = m_mapGuild.Begin();
	while (m_mapGuild.PeekNext(iter, pGuild))
	{
		if (!P_VALID(pGuild))
		{
			continue;
		}

		if (!pGuild->IsGuildInitOK())
		{
			ASSERT(0);
			ILOG->Write(_T("Guild<id: %u> load failed! Please check db! \n"), pGuild->GetGuildAtt().dwID);
			bRet = FALSE;
		}
	}

	return bRet;
}

//-------------------------------------------------------------------------------
// 关闭LoongWorld时，保存所有帮派基本信息
//-------------------------------------------------------------------------------
VOID GuildMgr::SendAllGuild2DB()
{
	INT32 nGuildNum = GetGuildNum();
	if(nGuildNum <= 0)
	{
		return;
	}

	INT32 nMsgSz = sizeof(tagNDBC_SaveAllGuild) + (nGuildNum - 1) * sizeof(tagGuildBase);
	MCREATE_MSG(pSend, nMsgSz, NDBC_SaveAllGuild);
	pSend->nGuildNum	= nGuildNum;

	INT32 nIndex	= 0;
	Guild *pGuild	= NULL;
	MapGuild::TMapIterator iter = m_mapGuild.Begin();
	while(m_mapGuild.PeekNext(iter, pGuild))
	{
		pSend->pGuildBase[nIndex++] = (tagGuildBase)pGuild->GetGuildAtt();
	}

	ASSERT(nIndex == nGuildNum);
	g_dbSession.Send(pSend, pSend->dwSize);
	MDEL_MSG(pSend);
}

//-------------------------------------------------------------------------------
// 客户端连接后，初始化所在帮派信息
//-------------------------------------------------------------------------------
VOID GuildMgr::InitGuildInitState(Role *pRole, OUT tagGuildBase &sGuildBase, 
								  OUT tagGuildMember &sGuildMember)
{
	ASSERT(P_VALID(pRole));

	sGuildBase.dwID			= GT_INVALID;
	sGuildMember.dwRoleID	= GT_INVALID;

	Guild *pGuild = GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		if(m_mapGuild.Size() > 0)
		{
			// 执行到该处，说明该成员已被踢出
			pRole->SetGuildID(GT_INVALID);
			return;
		}
		
		// 执行到该处，有可能数据库中没有帮派，也可能读取失败 -- 建议数据库中永久存在一个帮派
		ASSERT(0);
		IMSG(_T("\n\n\tGuild load from db maybe have problem! Please check!\n\n"));
		return;
	}

	sGuildBase = (const tagGuildBase)pGuild->GetGuildAtt();
	sGuildBase.n16MemberNum = pGuild->GetGuildMemberNum();

	tagGuildMember *pMember = pGuild->GetMember(pRole->GetID());
	if(!P_VALID(pMember))
	{
		if(pGuild->GetGuildMemberNum() > 0)
		{
			// 执行到该处，说明该成员已被踢出
			pRole->SetGuildID(GT_INVALID);
		}
		else
		{
			// 说明帮派成员读取失败
			ASSERT(0);
			IMSG(_T("\n\nWarning:\n\tGuild members load failed! Please check!\n\n"));
		}
		
		return;
	}

	sGuildMember = *pMember;
}

//-------------------------------------------------------------------------------
// 删除角色时清理相关数据
//-------------------------------------------------------------------------------
DWORD GuildMgr::ClearRoleRemove(DWORD dwRoleID)
{
	Guild *pGuild = NULL;
	tagGuildMember *pMember = NULL;
	MapGuild::TMapIterator iter = m_mapGuild.Begin();
	while(m_mapGuild.PeekNext(iter, pGuild))
	{
		pMember = pGuild->GetMember(dwRoleID);
		if(P_VALID(pMember))
		{
			return pGuild->ClearRoleRemove(dwRoleID);
		}
	}

	return E_Success;
}

//-------------------------------------------------------------------------------
// 帮派信息处理发送到客户端
//-------------------------------------------------------------------------------
VOID GuildMgr::SendGuildProcFailed2Client(PlayerSession *pSession, DWORD dwErrorCode)
{
	ASSERT(P_VALID(pSession));
	ASSERT(dwErrorCode != E_Success);

	tagNS_GuildProcFailed send;
	send.dwErrorCode = dwErrorCode;
	pSession->SendMessage(&send, send.dwSize);
}

//-------------------------------------------------------------------------------
// 创建帮派信息发送到LoongDB
//-------------------------------------------------------------------------------
VOID GuildMgr::SendCreateGuild2DB(const tagGuild& sGuild)
{
	INT32 nNumTCHAR = sGuild.strName.size();
	INT32 nMsgSz = sizeof(tagNDBC_CreateGuild) + nNumTCHAR * sizeof(TCHAR);
	
	MCREATE_MSG(pSend, nMsgSz, NDBC_CreateGuild);
	
	pSend->sCreateGuildInfo.dwGuildID			= sGuild.dwID;
	pSend->sCreateGuildInfo.dwCreateRoleNameID	= sGuild.dwFounderNameID;

	pSend->sCreateGuildInfo.nGuildRep			= sGuild.nReputation;
	pSend->sCreateGuildInfo.nGuildFund			= sGuild.nFund;
	pSend->sCreateGuildInfo.nGuildMaterial		= sGuild.nMaterial;
	pSend->sCreateGuildInfo.nGroupPurchase		= sGuild.nGroupPurchase;

	pSend->sCreateGuildInfo.n16GuildPeace		= sGuild.n16Peace;
	pSend->sCreateGuildInfo.byGuildLevel		= sGuild.byLevel;
	pSend->sCreateGuildInfo.byAffairRemainTimes	= sGuild.byAffairRemainTimes;

	_tcscpy_s(pSend->sCreateGuildInfo.szName, nNumTCHAR + 1, sGuild.strName.c_str());
	ASSERT(_T('\0') == pSend->sCreateGuildInfo.szName[nNumTCHAR]);

	g_dbSession.Send(pSend, pSend->dwSize);

	MDEL_MSG(pSend);
}

//-------------------------------------------------------------------------------
// 解散帮派信息发送到LoongDB
//-------------------------------------------------------------------------------
VOID GuildMgr::SendDismissGuild2DB(DWORD dwGuildID)
{
	tagNDBC_DismissGuild send;
	send.dwGuildID = dwGuildID;

	g_dbSession.Send(&send, send.dwSize);
}

//-------------------------------------------------------------------------------
// 创建帮派
//-------------------------------------------------------------------------------
DWORD GuildMgr::CreateGuild(Role* pCreator, LPCTSTR strGuildName, INT32 nNumTHCAR)
{
	ASSERT(P_VALID(pCreator));

	// 判断创建者是否已经加入一个帮派
	if(pCreator->IsInGuild())
	{
		// 其他地方有逻辑问题
		ASSERT(P_VALID(GetGuild(pCreator->GetGuildID())));

		return E_Guild_Create_AlreadyIn;
	}

	// 等级是否达到20级
	if(pCreator->GetLevel() < g_guildMgr.GetGuildCfg().nCreateRoleMinLevel)
	{
		return E_Guild_Create_LevelLimit;
	}

	// 获取名字 -- 确保有字符串结束符
	tstring szTmpGuildName(strGuildName, nNumTHCAR);

	// 是否与已建帮派重名
	DWORD dwNewGuildID = g_world.LowerCrc32(szTmpGuildName.c_str());
	if(m_mapGuild.IsExist(dwNewGuildID))
	{
		return E_Guild_Create_NameExist;
	}

	// 帮派名合法性判断
	DWORD dwErrorCode = Filter::CheckName(szTmpGuildName.c_str(), g_attRes.GetVariableLen().nGuildNameMax, 
							g_attRes.GetVariableLen().nGuildNameMin, g_attRes.GetNameFilterWords());
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 现金是否足够(10金)
	INT64 n64Cost = MGold2Silver(GetGuildCfg().nGoldCreateNeeded);
	if(pCreator->GetCurMgr().GetBagSilver() < n64Cost)
	{
		return E_BagSilver_NotEnough;
	}

	// 创建帮派
	Guild *pNewGuild = CreateGuild(pCreator, szTmpGuildName, dwNewGuildID);
	if(!P_VALID(pNewGuild))
	{
		ASSERT(0);
		return GT_INVALID;
	}

	// 扣除金钱
	pCreator->GetCurMgr().DecBagSilver(n64Cost, ELCLD_Guild_Create);

	// 保存到map中
	m_mapGuild.Add(pNewGuild->GetGuildAtt().dwID, pNewGuild);

	// 保存到数据库中
	SendCreateGuild2DB(pNewGuild->GetGuildAtt());

	return E_Guild_Create_Success;
}


//-------------------------------------------------------------------------------
// 创建帮派
//-------------------------------------------------------------------------------
Guild* GuildMgr::CreateGuild(Role* pCreator, const tstring& strGuildName, DWORD dwNewGuildID)
{
	Guild* pNewGuild = new Guild();
	if(!P_VALID(pNewGuild))
	{
		ASSERT(0);
		return NULL;
	}

	// 初始化帮派属性
	pNewGuild->InitCreate(pCreator, strGuildName, dwNewGuildID, g_guildMgr.GetGuildCfg());

	// 全服广播
	tagRoleInfo *pRoleInfo = g_roleMgr.GetRoleInfo(pCreator->GetID());
	if (P_VALID(pRoleInfo))
	{
		INT32 nRoleNameCnt = _tcsclen(pRoleInfo->szRoleName);
		INT32 nMsgSz = sizeof(tagNS_GuildCreateBroad) + (strGuildName.length() + nRoleNameCnt + 1) * sizeof(TCHAR);

		MCREATE_MSG(pSend, nMsgSz, NS_GuildCreateBroad);
		pSend->dwRoleID		= pCreator->GetID();
		pSend->dwRoleNameID	= pCreator->GetNameID();
		pSend->dwGuildID	= pNewGuild->GetGuildAtt().dwID;

		_tcscpy_s(pSend->szName, nRoleNameCnt + 1, pRoleInfo->szRoleName);
		_tcscpy_s((pSend->szName + nRoleNameCnt + 1), strGuildName.length() + 1, strGuildName.c_str());

		g_roleMgr.SendWorldMsg(pSend, pSend->dwSize);
		MDEL_MSG(pSend);
	}

	// 同步到周围玩家
	tagGuildMember* pMember = pNewGuild->GetMember(pCreator->GetID());
	Map*			pMap	= pCreator->GetMap();
	if (P_VALID(pMember) && P_VALID(pMap))
	{
		tagNS_RemoteRoleGuildInfoChange send;
		send.dwGuildID		= pNewGuild->GetGuildAtt().dwID;
		send.dwRoleID		= pCreator->GetID();
		send.n8GuildPos		= pMember->eGuildPos;

		pMap->SendBigVisTileMsg(pCreator, &send, send.dwSize);
	}

	return pNewGuild;
}

//-------------------------------------------------------------------------------
// 解散帮派
//-------------------------------------------------------------------------------
DWORD GuildMgr::DismissGuild(Role* pLeader, DWORD dwGuildID)
{
	ASSERT(P_VALID(pLeader));
	ASSERT(pLeader->GetGuildID() == dwGuildID);
	
	// 找到帮派
	Guild *pGuild = GetGuild(dwGuildID);
	if(!P_VALID(pGuild))
	{
		return E_Guild_NotExist;
	}

	// 判断是否可以删除
	DWORD dwErrorCode = pGuild->DismissGuild(pLeader);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 清除该帮派的团购信息
	g_mall.RemoveGuildPurchaseInfo(dwGuildID);

	// 全服广播
	INT32 nGuildNameCnt = pGuild->GetGuildAtt().strName.size();
	INT32 nMsgSz = sizeof(tagNS_GuildDismissBroad) + nGuildNameCnt * sizeof(TCHAR);

	MCREATE_MSG(pSend, nMsgSz, NS_GuildDismissBroad);
	pSend->dwRoleID	= pLeader->GetID();
	_tcscpy_s(pSend->szGuildName, nGuildNameCnt + 1, pGuild->GetGuildAtt().strName.c_str());
	g_roleMgr.SendWorldMsg(pSend, pSend->dwSize);
	MDEL_MSG(pSend);

	// 同步到周围玩家
	Map* pMap	= pLeader->GetMap();
	if (P_VALID(pMap))
	{
		tagNS_RemoteRoleGuildInfoChange send;
		send.dwGuildID		= GT_INVALID;
		send.dwRoleID		= pLeader->GetID();
		send.n8GuildPos		= EGMP_Null;

		pMap->SendBigVisTileMsg(pLeader, &send, send.dwSize);
	}

	// 解散帮派
	SAFE_DEL(pGuild);
	m_mapGuild.Erase(dwGuildID);

	// 保存到数据库中
	SendDismissGuild2DB(dwGuildID);
	
	return E_Success;
}


//-------------------------------------------------------------------------------
// 向帮派仓库添加操作事件
//-------------------------------------------------------------------------------
VOID GuildMgr::AddGuildWareEvent(DWORD dwSender, EEventType eType, DWORD dwSize, void *pMsg)
{
	Role* pRole = g_roleMgr.GetRolePtrByID(dwSender);
	if (!P_VALID(pRole))
	{
		// 玩家不在线
		return;
	}

	DWORD dwGuildID = pRole->GetGuildID();
	if (!GT_VALID(dwGuildID))
	{
		// 玩家没有帮派
		return;
	}

	Guild* pGuild = GetGuild(dwGuildID);
	if (!P_VALID(pGuild))
	{
		// 帮派不存在
		return;
	}
	
	pGuild->GetGuildWarehouse().AddEvent(dwSender, eType, dwSize, pMsg);
}

//-------------------------------------------------------------------------------
// 每日重置帮派事务状态并扣除消耗
//-------------------------------------------------------------------------------
VOID GuildMgr::DailyGuildReset()
{
	Guild *pGuild = NULL;
	MapGuild::TMapIterator iter = m_mapGuild.Begin();
	while(m_mapGuild.PeekNext(iter, pGuild))
	{
		if (!P_VALID(pGuild))
		{
			continue;
		}
		pGuild->DailyGuildReset();
	}
}

//-------------------------------------------------------------------------------
// 帮派技能整点升级
//-------------------------------------------------------------------------------
VOID GuildMgr::UpgradeGuildSkill()
{
	Guild *pGuild = NULL;
	MapGuild::TMapIterator iter = m_mapGuild.Begin();
	while(m_mapGuild.PeekNext(iter, pGuild))
	{
		if (!P_VALID(pGuild))
		{
			continue;
		}
		pGuild->GetGuildSkill().SkillUpgradeOnClock();
	}
}