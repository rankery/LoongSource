//-------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: loong_guild.cpp
// author: Sxg
// actor:
// data: 2009-04-14
// last:
// brief: 帮派相关处理
//-------------------------------------------------------------------
#include "StdAfx.h"

#include "Loong.h"
#include "LoongDB.h"
#include "../ServerDefine/msg_guild.h"

#include <new>

#define MGuildMemMsgSz(nNum)	((nNum - 1) * sizeof(tagGuildMemLoad) + sizeof(tagNDBS_LoadAllGuildMember))
/****************************** 帮派属性相关 *******************************/
//-------------------------------------------------------------------
// 读入所有帮派属性
//-------------------------------------------------------------------
DWORD CLoong::LoadAllGuild(DWORD pDBMsg, DWORD dwDummy)
{
	//MGET_MSG(p, pDBMsg, NDBC_LoadAllGuild);

	// 预先分配一个足够大的空间
	CHAR *pBuffLoad = new CHAR[MAX_GUILD_BUFF_SIZE];
	ZeroMemory(pBuffLoad, sizeof(CHAR) * MAX_GUILD_BUFF_SIZE);
	
	INT32 nGuildNum = 0;
	if(m_pDB->LoadAllGuild(pBuffLoad, nGuildNum))
	{
		MTRANS_POINTER(pGuildLoad, pBuffLoad, tagGuildLoad);
		tagNDBS_LoadAllGuild send;
		for(INT32 i=0; i<nGuildNum; ++i)
		{
			memcpy(&send.sGuildLoad, pGuildLoad + i, sizeof(tagGuildLoad));

			SendMsg( &send, send.dwSize);
		}
	}

	SAFE_DEL_ARRAY(pBuffLoad);

	return E_Success;
}

//-------------------------------------------------------------------
// 检验帮派初始化
//-------------------------------------------------------------------
DWORD CLoong::CheckGuildInitOK(DWORD pDBMsg, DWORD dwDummy)
{
	tagNDBS_GuildInitOK send;

	SendMsg( &send, send.dwSize);

	return E_Success;
}

//-------------------------------------------------------------------
// 创建帮派
//-------------------------------------------------------------------
DWORD CLoong::CreateGuild(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_CreateGuild);

	if(!m_pDB->InsertGuild(&p->sCreateGuildInfo))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//-------------------------------------------------------------------
// 解散帮派
//-------------------------------------------------------------------
DWORD CLoong::DismissGuild(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_DismissGuild);

	if(!m_pDB->DeleteGuild(p->dwGuildID))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//-------------------------------------------------------------------
// 修改帮派宗旨
//-------------------------------------------------------------------
DWORD CLoong::ChangeGuildTenet(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_ChangeGuildTenet);

	if(!m_pDB->UpdateGuildTenet(p->dwGuildID, p->szNewTenet))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//-------------------------------------------------------------------
// 保存所有帮派属性
//-------------------------------------------------------------------
DWORD CLoong::SaveAllGuild(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_SaveAllGuild);

	if(!m_pDB->UpdateAllGuild(p->pGuildBase, p->nGuildNum))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//-------------------------------------------------------------------
// 保存指定帮派属性
//-------------------------------------------------------------------
DWORD CLoong::SaveGuildAtt(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_SaveGuildAtt);

	if(!m_pDB->UpdateGuildAtt(&p->pGuildBase))
	{
		return GT_INVALID;
	}

	return E_Success;
}

/*************************** 帮派成员属性相关 ***************************/

//-------------------------------------------------------------------
// 读入所有帮派成员
//-------------------------------------------------------------------
DWORD CLoong::LoadAllGuildMember(DWORD pDBMsg, DWORD dwDummy)
{
	//MGET_MSG(p, pDBMsg, NDBC_LoadAllGuildMember);

	// 预先分配一个足够大的空间
	CHAR *pBuffLoad = new CHAR[MAX_GUILD_BUFF_SIZE];
	ZeroMemory(pBuffLoad, sizeof(CHAR) * MAX_GUILD_BUFF_SIZE);

	INT32 nGuildMemNum = 0;
	if(m_pDB->LoadAllGuildMember(pBuffLoad, nGuildMemNum))
	{
		CHAR *pBuffSend = new CHAR[MGuildMemMsgSz(MAX_GUILDMEM_NUM_PER_MSG)];

		tagNDBS_LoadAllGuildMember *pSend = (tagNDBS_LoadAllGuildMember*)(pBuffSend);
		ZeroMemory(pBuffSend, MGuildMemMsgSz(MAX_GUILDMEM_NUM_PER_MSG));
		MMSGINIT(pSend, tagNDBS_LoadAllGuildMember);
				
		MTRANS_POINTER(pCur, pBuffLoad, tagGuildMemLoad);
		INT32 nSendLeft = nGuildMemNum;
		INT32 nSendNum	= 0;
		while(nSendLeft > 0)
		{
			pCur = &pCur[nSendNum];
			
			if(nSendLeft > MAX_GUILDMEM_NUM_PER_MSG)
			{
				nSendNum = MAX_GUILDMEM_NUM_PER_MSG;
			}
			else
			{
				nSendNum = nSendLeft;
			}
			
			memcpy(pSend->sGuildMemberLoad, pCur, nSendNum * sizeof(tagGuildMemLoad));
			pSend->nAllGuildNum	= nSendNum;
			pSend->dwSize		= MGuildMemMsgSz(nSendNum);

			SendMsg( pSend, pSend->dwSize);

			nSendLeft -= nSendNum;
		}

		SAFE_DEL_ARRAY(pBuffSend);
	}

	SAFE_DEL_ARRAY(pBuffLoad);

	// 帮派成员初始化结束
	tagNDBS_GuildMemberInitOK send;
	SendMsg(&send, send.dwSize);

	return E_Success;
}

//-------------------------------------------------------------------
// 加入帮派
//-------------------------------------------------------------------
DWORD CLoong::JoinGuild(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_JoinGuild);
	
	if(!m_pDB->InsertGuildMember(p->sGuildMemInfo))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//-------------------------------------------------------------------
// 离开帮派
//-------------------------------------------------------------------
DWORD CLoong::LeaveGuild(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LeaveGuild);

	if(!m_pDB->DeleteGuildMember(p->dwRoleID))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//-------------------------------------------------------------------
// 帮派职位变化
//-------------------------------------------------------------------
DWORD CLoong::ChangeGuildPos(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_ChangeGuildPos);

	if(!m_pDB->UpdateGuildMemberPos(p->dwRoleID, p->n8NewPos))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 帮派仓库访问权限变更
//------------------------------------------------------------------------------
DWORD CLoong::GuildWarePrivilege(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_GuildWarePri);

	if (!m_pDB->GuildWarePrivilege(p->dwRoleID, p->bEnable))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 指定帮派仓库物品读取
//------------------------------------------------------------------------------
DWORD CLoong::LoadGuildWareItems(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LoadGuildWareItems);

	// 申请足够大的内存
	BYTE* pBuff = new BYTE[TEMP_GUILD_BUFF_SIZE];
	if (!P_VALID(pBuff))
	{
		return GT_INVALID;
	}
	ZeroMemory(pBuff, sizeof(BYTE) * TEMP_GUILD_BUFF_SIZE);

	tagNDBS_LoadGuildWareItems* pSend = (tagNDBS_LoadGuildWareItems*)pBuff;

	//做消息头
	tagNDBS_LoadGuildWareItems temp;
	pSend->dwID			= temp.dwID;
	pSend->dwSize		= temp.dwSize;
	pSend->dwGuildID	= p->dwGuildID;
	pSend->nItemNum		= GT_INVALID;

	LPVOID pTemp = (LPVOID)(pSend->byData);
	m_pDB->LoadGuildWareItems(pTemp, p->dwGuildID, pSend->nItemNum);

	if (GT_VALID(pSend->nItemNum))
	{
		// 重新设置消息大小
		pSend->dwSize		= sizeof(tagNDBS_LoadGuildWareItems) + ((BYTE*)pTemp - pSend->byData - 1) * sizeof(BYTE);

		// 反馈给World服务器
		SendMsg( pSend, pSend->dwSize);
	}

	// 释放
	SAFE_DEL_ARRAY(pBuff);

	return E_Success;
}

//------------------------------------------------------------------------------
// 保存更新的帮派仓库物品
//------------------------------------------------------------------------------
DWORD CLoong::SaveGuildWareItems(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_SaveGuildWare);

	m_pDB->UpdateItemPosAndUseRel(GT_INVALID, p->byData, p->nItemNum, NULL);

	return E_Success;
}

//------------------------------------------------------------------------------
// 获取所有帮派设施升级信息
//------------------------------------------------------------------------------
DWORD CLoong::LoadGuildFacilities(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LoadFacilitiesInfo);

	// 申请足够大的内存
	BYTE* pBuff = new BYTE[TEMP_GUILD_BUFF_SIZE];
	if (!P_VALID(pBuff))
	{
		return GT_INVALID;
	}
	ZeroMemory(pBuff, sizeof(BYTE) * TEMP_GUILD_BUFF_SIZE);

	tagNDBS_LoadFacilitiesInfo* pSend = (tagNDBS_LoadFacilitiesInfo*)pBuff;

	//做消息头
	tagNDBS_LoadFacilitiesInfo temp;
	pSend->dwID			= temp.dwID;
	pSend->dwSize		= temp.dwSize;
	pSend->dwGuildID	= p->dwGuildID;

	if (!m_pDB->LoadGuildFacilities((LPVOID)pSend->sFacilitiesInfo, pSend->nInfoNum, p->dwGuildID))
	{
		SAFE_DEL_ARRAY(pBuff);
		return GT_INVALID;
	}

	// 重新计算消息大小
	pSend->dwSize = sizeof(tagNDBS_LoadFacilitiesInfo) + (pSend->nInfoNum - 1) * sizeof(tagFacilitiesLoad);

	// 反馈给World服务器
	SendMsg( pSend, pSend->dwSize);

	// 释放内存
	SAFE_DEL_ARRAY(pBuff);

	return E_Success;
}

//------------------------------------------------------------------------------
// 插入帮派设施信息
//------------------------------------------------------------------------------
DWORD CLoong::InsertGuildFacilities(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_CreateFacilities);

	if (!m_pDB->InsertGuildFacilities(&p->sFacilitiesInfo))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 更新帮派设施信息
//------------------------------------------------------------------------------
DWORD CLoong::UpdateGuildFacilities(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_FacilitiesUpgrade);

	if (!m_pDB->UpdateGuildFacilities(&p->sFacilitiesInfo))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 删除帮派所有设施信息
//------------------------------------------------------------------------------
DWORD CLoong::RemoveAllFacilities(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_RemoveAllFacilities);

	if (!m_pDB->RemoveAllFacilities(p->dwGuildID))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 更新帮派成员贡献
//------------------------------------------------------------------------------
DWORD CLoong::ChangeMemberContribution(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_ChangeContrib);

	if (!m_pDB->ChangeMemberContribution((LPVOID)p))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 读取帮派技能信息
//------------------------------------------------------------------------------
DWORD CLoong::LoadGuildSkillInfo(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LoadGuildSkillInfo);

	// 分配足够大的空间
	CHAR *pBuffLoad = new CHAR[TEMP_GUILD_BUFF_SIZE];
	ZeroMemory(pBuffLoad, sizeof(CHAR) * TEMP_GUILD_BUFF_SIZE);

	tagNDBS_LoadGuildSkillInfo* pSend = (tagNDBS_LoadGuildSkillInfo*)pBuffLoad;
	
	// 做消息头
	tagNDBS_LoadGuildSkillInfo temp;
	pSend->dwID			= temp.dwID;
	pSend->dwSize		= temp.dwSize;
	pSend->dwGuildID	= p->dwGuildID;
	
	if (!m_pDB->LoadGuildSkillInfo(pSend->sGuildSkillInfo, p->dwGuildID, pSend->nInfoNum))
	{
		SAFE_DEL_ARRAY(pBuffLoad);
		return GT_INVALID;
	}

	// 重新计算消息大小
	pSend->dwSize = sizeof(tagNDBS_LoadGuildSkillInfo) + (pSend->nInfoNum - 1) * sizeof(tagGuildSkillInfo);

	// 反馈给World服务器
	SendMsg( pSend, pSend->dwSize);

	// 释放内存
	SAFE_DEL_ARRAY(pBuffLoad);

	return E_Success;
}

//------------------------------------------------------------------------------
// 创建帮派的技能信息
//------------------------------------------------------------------------------
DWORD CLoong::InsertGuildSkillInfo(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_CreateGuildSkill);

	if (!m_pDB->InsertGuildSkillInfo(p->dwGuildID, &p->sSkillInfo))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 保存帮派技能信息
//------------------------------------------------------------------------------
DWORD CLoong::SaveGuildSkillInfo(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_SaveGuildSkill);

	if (!m_pDB->UpdateGuildSkillInfo(p->dwGuildID, &p->sSkillInfo))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 设置当前研究帮派技能
//------------------------------------------------------------------------------
DWORD CLoong::ChangeResearchGuildSkill(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_ChangeResearchSkill);

	if (!m_pDB->ChangeResearchGuildSkill(p->dwGuildID, p->dwNewSkillID))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 删除指定帮派的技能信息
//------------------------------------------------------------------------------
DWORD CLoong::RemoveGuildSkillInfo(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_RemoveGuildSkill);

	if (!m_pDB->RemoveGuildSkillInfo(p->dwGuildID))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 读取指定帮派跑商信息
//------------------------------------------------------------------------------
DWORD CLoong::LoadGuildCommerceInfo(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LoadCommodity);

	// 分配足够大的空间
	CHAR *pBuffLoad = new CHAR[TEMP_GUILD_BUFF_SIZE];
	ZeroMemory(pBuffLoad, sizeof(CHAR) * TEMP_GUILD_BUFF_SIZE);

	tagNDBS_LoadCommodity* pSend = (tagNDBS_LoadCommodity*)pBuffLoad;

	// 做消息头
	tagNDBS_LoadCommodity temp;
	pSend->dwID			= temp.dwID;
	pSend->dwSize		= temp.dwSize;
	pSend->dwGuildID	= p->dwGuildID;

	m_pDB->LoadGuildCommerceInfo(pSend->sCommerceInfo, p->dwGuildID, pSend->nCommodityNum);

	// 重新计算消息大小
	pSend->dwSize = sizeof(tagNDBS_LoadCommodity) + (pSend->nCommodityNum - 1) * sizeof(tagGuildCommerceInfo);

	// 反馈给World服务器
	SendMsg( pSend, pSend->dwSize);

	// 释放内存
	SAFE_DEL_ARRAY(pBuffLoad);

	return E_Success;
}

//------------------------------------------------------------------------------
// 读取指定帮派跑商排名信息
//------------------------------------------------------------------------------
DWORD CLoong::LoadCommerceRankInfo(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_LoadCommerceRank);

	// 分配足够大的空间
	CHAR *pBuffLoad = new CHAR[TEMP_GUILD_BUFF_SIZE];
	ZeroMemory(pBuffLoad, sizeof(CHAR) * TEMP_GUILD_BUFF_SIZE);

	tagNDBS_LoadCommerceRank* pSend = (tagNDBS_LoadCommerceRank*)pBuffLoad;

	// 做消息头
	tagNDBS_LoadCommerceRank temp;
	pSend->dwID			= temp.dwID;
	pSend->dwSize		= temp.dwSize;
	pSend->dwGuildID	= p->dwGuildID;

	if (!m_pDB->LoadCommerceRankInfo(pSend->sRankInfo, p->dwGuildID, pSend->nRankNum))
	{
		SAFE_DEL_ARRAY(pBuffLoad);
		return GT_INVALID;
	}

	// 重新计算消息大小
	pSend->dwSize = sizeof(tagNDBS_LoadCommerceRank) + (pSend->nRankNum - 1) * sizeof(tagCommerceRank);

	// 反馈给World服务器
	SendMsg( pSend, pSend->dwSize);

	// 释放内存
	SAFE_DEL_ARRAY(pBuffLoad);

	return E_Success;
}

//------------------------------------------------------------------------------
// 创建跑商容器
//------------------------------------------------------------------------------
DWORD CLoong::CreateCommodityInfo(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_CreateCommodity);

	if (!m_pDB->InsertCommodityInfo((LPVOID)p))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 更新商银
//------------------------------------------------------------------------------
DWORD CLoong::UpdateCommodityTael(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_ChangeTael);

	if (!m_pDB->UpdateCommodityTael(p->dwRoleID, p->nTael))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 保存商货
//------------------------------------------------------------------------------
DWORD CLoong::SaveCommodityInfo(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_SaveCommodity);

	if (!m_pDB->SaveCommodityInfo(p->sGoodInfo, p->dwRoleID))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 清空商货信息
//------------------------------------------------------------------------------
DWORD CLoong::RemoveCommodityInfo(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_RemoveCommodity);

	if (!m_pDB->RemoveCommodityInfo(p->dwRoleID))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 设置跑商嘉奖状态
//------------------------------------------------------------------------------
DWORD CLoong::SetCommendation(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_SetCommendation);

	if (!m_pDB->SetCommendation(p->dwGuildID, p->bCommend))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 创建或者更新跑商排名
//------------------------------------------------------------------------------
DWORD CLoong::UpdateCommerceRankInfo(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_UpdateCommerceRank);

	if (!m_pDB->UpdateCommerceRankInfo(p->dwGuildID, &p->sRankInfo))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 删除跑商排名信息
//------------------------------------------------------------------------------
DWORD CLoong::RemoveCommerceRankInfo(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_RemoveCommerceRank);

	if (!m_pDB->RemoveCommerceRankInfo(p->dwGuildID, p->dwRoleID))
	{
		return GT_INVALID;
	}

	return E_Success;
}