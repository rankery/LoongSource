//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_commerce.cpp
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 帮会跑商管理
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#include "../ServerDefine/msg_guild.h"
#include "../ServerDefine/role_data.h"
#include "../ServerDefine/log_cmdid_define.h"

#include "../WorldDefine/msg_guild.h"

#include "guild.h"
#include "guild_commodity.h"
#include "guild_CofC.h"
#include "role.h"
#include "db_session.h"
#include "role_mgr.h"

//-----------------------------------------------------------------------------
// 构造和析构
//-----------------------------------------------------------------------------
GuildCommerce::GuildCommerce()
{
	m_pGuild	= NULL;
	m_bCommend	= FALSE;
	m_mapCommodity.Clear();
	m_vecRank.clear();
	m_bInitOK	= FALSE;
}

GuildCommerce::~GuildCommerce()
{
	Destory();
}

//-----------------------------------------------------------------------------
// 初始化和销毁
//-----------------------------------------------------------------------------
BOOL GuildCommerce::Init( Guild* pGuild, BOOL bRequest /*= FALSE*/ )
{
	if (!P_VALID(pGuild))
	{
		return FALSE;
	}
	m_pGuild	= pGuild;
	m_bCommend	= pGuild->GetGuildAtt().bCommendation;
	m_mapCommodity.Clear();
	m_vecRank.clear();

	if (bRequest)
	{
		// 向DB发送申请
		tagNDBC_LoadCommodity send;
		send.dwGuildID	= pGuild->GetGuildAtt().dwID;
		g_dbSession.Send(&send, send.dwSize);

		tagNDBC_LoadCommerceRank send_rank;
		send_rank.dwGuildID	= pGuild->GetGuildAtt().dwID;
		g_dbSession.Send(&send_rank, send_rank.dwSize);

		m_bInitOK		= FALSE;
	}
	else
	{
		m_bInitOK	= TRUE;
	}

	return TRUE;
}

VOID GuildCommerce::Destory()
{
	m_bInitOK	= FALSE;

	GuildCommodity* pCommodity = NULL;
	MapCommodity::TMapIterator iter_commodity = m_mapCommodity.Begin();
	while (m_mapCommodity.PeekNext(iter_commodity, pCommodity))
	{
		SAFE_DEL(pCommodity);
	}
	m_mapCommodity.Clear();

	VectorRank::iterator iter_rank = m_vecRank.begin();
	while (iter_rank != m_vecRank.end())
	{
		tagCommerceRank* pRankInfo	= *iter_rank;
		SAFE_DEL(pRankInfo);
		iter_rank++;
	}
	m_vecRank.clear();
}

//-----------------------------------------------------------------------------
// 接取跑商
//-----------------------------------------------------------------------------
DWORD GuildCommerce::AcceptCommerce( Role* pRole )
{
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 未初始化
	if (!m_bInitOK)
	{
		return GT_INVALID;
	}

	// 等级限制(这个地方可以读取脚本)
	if (pRole->GetLevel() < 20)
	{
		return E_GuildMember_Level_Limit;
	}

	// 帮派状态
	if (m_pGuild->IsInGuildStateAny(EGDSS_Warfare))
	{
		return E_Guild_State_Limit;
	}

	// 职位判断
	tagGuildMember* pMember = m_pGuild->GetMember(pRole->GetID());
	if (!P_VALID(pMember))
	{
		return E_Guild_MemberNotIn;
	}
	if (!m_pGuild->GetGuildPower(pMember->eGuildPos).bCommerce)
	{
		return E_Guild_Power_NotEnough;
	}

	// 保证金是否足够
	const tagCommerceInfo* pCommerceInfo = g_attRes.GetGuildCommerceInfo(pRole->GetLevel());
	if (pRole->GetCurMgr().GetBagSilver() < pCommerceInfo->sTaelInfo.nDeposit)
	{
		return E_BagSilver_NotEnough;
	}

	// 角色是否在跑商中
	if (pRole->IsInRoleState(ERS_Commerce))
	{
		return E_GuildCommerce_Status_Error;
	}

	// 扣除玩家金钱
	pRole->GetCurMgr().DecBagSilver(pCommerceInfo->sTaelInfo.nDeposit, ELCLD_Guild_Commerce);

	// 创建跑商容器
	GuildCommodity* pCommodity = m_mapCommodity.Peek(pRole->GetID());
	if (P_VALID(pCommodity))
	{
		SAFE_DEL(pCommodity);
		m_mapCommodity.Erase(pRole->GetID());
	}
	pCommodity = new GuildCommodity;
	pCommodity->Init(pRole->GetID(), pRole->GetLevel(), &pCommerceInfo->sTaelInfo, &pCommerceInfo->sRedoundInfo);
	m_mapCommodity.Add(pRole->GetID(), pCommodity);

	tagNDBC_CreateCommodity send;
	send.dwGuildID	= m_pGuild->GetGuildAtt().dwID;
	send.dwRoleID	= pRole->GetID();
	send.nLevel		= pRole->GetLevel();
	send.nTael		= pCommerceInfo->sTaelInfo.nBeginningTael;
	g_dbSession.Send(&send, send.dwSize);

	// 设置玩家状态
	pRole->SetRoleState(ERS_Commerce);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 完成跑商
//-----------------------------------------------------------------------------
DWORD GuildCommerce::CompleteCommerce( Role* pRole, INT32& nFund )
{
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}
	
	// 未初始化
	if (!m_bInitOK)
	{
		return GT_INVALID;
	}

	// 帮派状态
	if (m_pGuild->IsInGuildStateAny(EGDSS_Warfare))
	{
		return E_Guild_State_Limit;
	}

	// 玩家状态
	if (!pRole->IsInRoleState(ERS_Commerce))
	{
		return E_GuildCommerce_Status_Error;
	}

	// 判断商银是否达标
	GuildCommodity* pCommodity = m_mapCommodity.Peek(pRole->GetID());
	if (!P_VALID(pCommodity))
	{
		return E_GuildCommerce_Status_Error;
	}
	if (pCommodity->GetTaelProgress() < 100)
	{
		return E_GuildCommerce_Tael_NotEnough;
	}

	// 增加帮派资金
	nFund	= (INT32)(pCommodity->GetGain() * 0.7f + 0.5f);
	m_pGuild->IncGuildFund(pRole->GetID(), nFund, ELCLD_Guild_Commerce);

	// 玩家奖励
	const tagRedoundInfo* pRedoundInfo	= pCommodity->GetRedoundInfo();
	const tagTaelInfo* pTaelInfo		= pCommodity->GetTaelInfo();
	if (P_VALID(pRedoundInfo) && P_VALID(pTaelInfo))
	{
		pRole->ExpChange(pRedoundInfo->nExp);													// 经验
		m_pGuild->ChangeMemberContribution(pRole->GetID(), pRedoundInfo->nContribution, TRUE);	// 贡献

		//跑商利润×30%+保证金×90%（所收取商银×10%+保证金×120%）
		INT32 nRedoundSilver = 0;
		if (m_pGuild->IsInGuildStateAny(EGDSS_Distress | EGDSS_Shortage))
		{
			nRedoundSilver = (INT32)(pCommodity->GetGain() * 0.3f + pCommodity->GetTaelInfo()->nDeposit * 1.2f + 0.5f);
		}
		else
		{
			nRedoundSilver = (INT32)(pCommodity->GetGain() * 0.3f + pCommodity->GetTaelInfo()->nDeposit * 0.9f + 0.5f);
		}
		pRole->GetCurMgr().IncBagSilver(nRedoundSilver, ELCLD_Guild_Commerce);
	}

	//调脚本
	g_ScriptMgr.GetRoleScript()->OnCompleteCommerce(pRole,pCommodity->GetTaelProgress());


	// 设置玩家状态
	pRole->UnsetRoleState(ERS_Commerce);

	// 保存到排行榜
	Add2CommerceRank(pRole->GetID(), nFund);

	// 删除跑商容器
	SAFE_DEL(pCommodity);
	m_mapCommodity.Erase(pRole->GetID());

	tagNDBC_RemoveCommodity send;
	send.dwRoleID	= pRole->GetID();
	g_dbSession.Send(&send, send.dwSize);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 放弃跑商
//-----------------------------------------------------------------------------
DWORD GuildCommerce::AbandonCommerce( DWORD dwRoleID, BOOL bClearRank /*= FALSE*/ )
{
	if (!GT_VALID(dwRoleID))
	{
		return GT_INVALID;
	}

	// 未初始化
	if (!m_bInitOK)
	{
		return GT_INVALID;
	}

	// 清除跑商排名信息
	if (bClearRank)
	{
		RemoveCommerceRank(dwRoleID);
	}

	GuildCommodity* pCommodity	= m_mapCommodity.Peek(dwRoleID);
	if (!P_VALID(pCommodity))
	{
		return E_GuildCommerce_Status_Error;
	}

	// 增加帮派资金
	INT32 nFund	= (INT32)(pCommodity->GetGain() * 0.9f + 0.5f);
	m_pGuild->IncGuildFund(dwRoleID, nFund, ELCLD_Guild_Commerce);

	// 销毁商货容器
	SAFE_DEL(pCommodity);
	m_mapCommodity.Erase(dwRoleID);

	// 通知客户端
	Role* pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
	if (P_VALID(pRole))
	{
		pRole->UnsetRoleState(ERS_Commerce);
		tagNS_AbandonCommerce send;
		pRole->SendMessage(&send, send.dwSize);
	}

	// 通知数据库
	tagNDBC_RemoveCommodity send;
	send.dwRoleID	= dwRoleID;
	g_dbSession.Send(&send, send.dwSize);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 获取排行榜信息
//-----------------------------------------------------------------------------
DWORD GuildCommerce::GetCommerceRankInfo( tagCommerceRank* pRankInfo, INT& nRankNum, BOOL& bCommend )
{
	// 上层保证容量大小
	if (!P_VALID(pRankInfo))
	{
		return GT_INVALID;
	}

	// 未初始化
	if (!m_bInitOK)
	{
		return GT_INVALID;
	}

	nRankNum	= 0;
	bCommend	= m_bCommend;

	// 读取前十名Rank信息
	VectorRank	vecRankInfo(MAX_COMMERCE_RANK_INFO);
	partial_sort_copy(m_vecRank.begin(), m_vecRank.end(), vecRankInfo.begin(), vecRankInfo.end(), RankCompare());

	VectorRank::iterator iter_rank = vecRankInfo.begin();
	for (; iter_rank != vecRankInfo.end(); iter_rank++)
	{
		if (!P_VALID(*iter_rank))
		{
			break;
		}
		pRankInfo[nRankNum++] = *(*iter_rank);
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 开启/关闭跑商嘉奖
//-----------------------------------------------------------------------------
DWORD GuildCommerce::SwitchCommendation( DWORD dwRoleID, BOOL bSwitchON )
{
	if (!GT_VALID(dwRoleID))
	{
		return GT_INVALID;
	}

	// 未初始化
	if (!m_bInitOK)
	{
		return GT_INVALID;
	}

	// 权限
	tagGuildMember* pMember	= m_pGuild->GetMember(dwRoleID);
	if (!P_VALID(pMember))
	{
		return E_Guild_MemberNotIn;
	}
	if (!m_pGuild->GetGuildPower(pMember->eGuildPos).bSetCommend)
	{
		return E_Guild_Power_NotEnough;
	}

	// 帮派等级
	if (m_pGuild->GetGuildAtt().byLevel < 3)
	{
		return E_Guild_Level_Limit;
	}

	// 设置嘉奖状态
	if (m_bCommend != bSwitchON)
	{
		m_bCommend	= bSwitchON;

		tagNDBC_SetCommendation send;
		send.dwGuildID	= m_pGuild->GetGuildAtt().dwID;
		send.bCommend	= m_bCommend;
		g_dbSession.Send(&send, send.dwSize);

		// 上层广播嘉奖状态
	}
	else
	{
		return E_GuildCommerce_Commend_Error;
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 发放跑商奖励
//-----------------------------------------------------------------------------
VOID GuildCommerce::ProvideBonus()
{
	// 未初始化
	if (!m_bInitOK)			return;

	// 帮派等级
	if (m_pGuild->GetGuildAtt().byLevel < 3)	return;
	
	// 排行榜
	if (m_vecRank.empty())	return;

	// 嘉奖状态
	if (m_bCommend)
	{
		VectorRank vecResult(MAX_COMMEND_PLAYER);
		partial_sort_copy(m_vecRank.begin(), m_vecRank.end(), vecResult.begin(), vecResult.end(), RankCompare());

		for (INT n=1; n <= MAX_COMMEND_PLAYER; n++)
		{
			if (!P_VALID(vecResult[n-1]))
			{
				break;
			}

			// 根据排名发放嘉奖
			Role* pRole	= g_roleMgr.GetRolePtrByID(vecResult[n-1]->dwRoleID);
			switch (n)
			{
			case 1:
				m_pGuild->ChangeMemberContribution(vecResult[0]->dwRoleID, 20, TRUE);
				if (P_VALID(pRole))
				{
					pRole->GetCurMgr().IncWareSilver(vecResult[0]->nTael/100, ELCLD_Guild_Commerce);
				}
				else
				{
					CurrencyMgr::ModifyWareSilver(vecResult[0]->dwRoleID, vecResult[0]->nTael/100, ELCLD_Guild_Commerce);
				}
				break;

			case 2:
				m_pGuild->ChangeMemberContribution(vecResult[1]->dwRoleID, 10, TRUE);
				if (P_VALID(pRole))
				{
					pRole->GetCurMgr().IncWareSilver(vecResult[1]->nTael*5/1000, ELCLD_Guild_Commerce);
				}
				else
				{
					CurrencyMgr::ModifyWareSilver(vecResult[1]->dwRoleID, vecResult[1]->nTael*5/1000, ELCLD_Guild_Commerce);
				}
				break;

			case 3:
				m_pGuild->ChangeMemberContribution(vecResult[2]->dwRoleID, 5, TRUE);
				if (P_VALID(pRole))
				{
					pRole->GetCurMgr().IncWareSilver(vecResult[2]->nTael*2/1000, ELCLD_Guild_Commerce);
				}
				else
				{
					CurrencyMgr::ModifyWareSilver(vecResult[2]->dwRoleID, vecResult[2]->nTael*2/1000, ELCLD_Guild_Commerce);
				}
				break;

			default:
				break;
			}
		}
	}

	// 清空跑商排名信息
	RemoveCommerceRank(GT_INVALID);
}

//-----------------------------------------------------------------------------
// 载入跑商信息
//-----------------------------------------------------------------------------
DWORD GuildCommerce::LoadCommerceInfo( tagGuildCommerceInfo* pInfo, INT nInfoNum )
{
	if (nInfoNum < 0)
	{
		return GT_INVALID;
	}

	for (int n=0; n<nInfoNum; n++)
	{
		GuildCommodity* pCommodity	= new GuildCommodity;
		if (pCommodity->LoadCommodityInfo(&pInfo[n]) == E_Success)
		{
			m_mapCommodity.Add(pInfo[n].dwRoleID, pCommodity);
		}
	}

	// 初始化完成
	m_bInitOK	= TRUE;
	
	return E_Success;
}

//-----------------------------------------------------------------------------
// 载入跑商排名信息
//-----------------------------------------------------------------------------
DWORD GuildCommerce::LoadCommerceRankInfo(tagCommerceRank *pInfo, INT nInfoNum)
{
	if (nInfoNum < 0)
	{
		return GT_INVALID;
	}

	for (int n=0; n<nInfoNum; n++)
	{
		Add2CommerceRank(pInfo[n].dwRoleID, pInfo[n].nTael, pInfo[n].nTimes, FALSE);		
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 获得指定玩家商货
//-----------------------------------------------------------------------------
DWORD GuildCommerce::GetCommodityGoodInfo( Role* pRole, tagCommerceGoodInfo* pGoodInfo, INT& nGoodNum, INT32& nTael, INT& nLevel )
{
	if (!P_VALID(pRole) || !P_VALID(pGoodInfo))
	{
		return GT_INVALID;
	}

	// 判断玩家状态
	if (!pRole->IsInRoleState(ERS_Commerce))
	{
		return E_GuildCommerce_Status_Error;
	}

	// 取得跑商容器
	GuildCommodity* pCommodity = GetCommodity(pRole->GetID());
	if (!P_VALID(pCommodity))
	{
		return E_GuildCommerce_Status_Error;
	}

	// 获取商货基本信息
	if (!pCommodity->GetCommodityInfo(pGoodInfo, nGoodNum, nTael, nLevel))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 从商会购买商货
//-----------------------------------------------------------------------------
DWORD GuildCommerce::BuyGoods( Role* pRole, DWORD dwNPCID, DWORD dwGoodID, BYTE byBuyNum )
{
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 玩家状态
	if (!pRole->IsInRoleState(ERS_Commerce))
	{
		return E_GuildCommerce_Status_Error;
	}

	// 取得商会信息
	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	GuildCofC* pCofC = pMap->GetCofC(dwNPCID);
	if (!P_VALID(pCofC))
	{
		return E_CofC_NotExist;
	}

	// 检查是否需要占领城市
	if (!m_pGuild->IsHoldCity(pCofC->GetHolderCity()))
	{
		return E_CofC_HoldCity_Limit;
	}

	// 检查玩家跑商容器容量
	GuildCommodity* pCommodity = GetCommodity(pRole->GetID());
	if (!P_VALID(pCommodity))
	{
		return E_GuildCommerce_Status_Error;
	}

	DWORD dwErrorCode = pCommodity->IsFull(dwGoodID, byBuyNum);
	if (E_Success != dwErrorCode)
	{
		return dwErrorCode;
	}

	// 购买商货
	INT32 nCostTael = 0;
	dwErrorCode = pCofC->SellItem(pCommodity->GetTael(), dwGoodID, byBuyNum, nCostTael);
	if (E_Success != dwErrorCode)
	{
		return dwErrorCode;
	}

	// 给玩家发送商货
	pCommodity->AddGood(dwGoodID, byBuyNum, pCofC->GetGoodPrice(dwGoodID));
	pCommodity->DecTael(nCostTael);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 售卖商货给商会
//-----------------------------------------------------------------------------
DWORD GuildCommerce::SellGoods( Role* pRole, DWORD dwNPCID, DWORD dwGoodID, BYTE bySellNum )
{
	if (!P_VALID(pRole) || bySellNum <= 0)
	{
		return GT_INVALID;
	}

	// 玩家状态
	if (!pRole->IsInRoleState(ERS_Commerce))
	{
		return E_GuildCommerce_Status_Error;
	}

	// 取得商会信息
	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	GuildCofC* pCofC = pMap->GetCofC(dwNPCID);
	if (!P_VALID(pCofC))
	{
		return E_CofC_NotExist;
	}

	// 检查是否是特产商会
	if (pCofC->GetHolderCity() != 0)
	{
		return E_CofC_ItemCannotSell;
	}

	// 检查玩家跑商容器
	GuildCommodity* pCommodity = GetCommodity(pRole->GetID());
	if (!P_VALID(pCommodity))
	{
		return E_GuildCommerce_Status_Error;
	}

	DWORD dwErrorCode = pCommodity->IsExist(dwGoodID, bySellNum);
	if (E_Success != dwErrorCode)
	{
		return dwErrorCode;
	}

	// 出售商货
	INT32 nCost = pCofC->GetGoodPrice(dwGoodID);
	if (!GT_VALID(nCost))
	{
		return E_CofC_ItemCannotSell;
	}

	// 扣除玩家商货
	pCommodity->RemoveGood(dwGoodID, bySellNum, pCofC->GetGoodPrice(dwGoodID));
	pCommodity->IncTael(nCost*bySellNum);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 获取跑商初始信息
//-----------------------------------------------------------------------------
DWORD GuildCommerce::GetCommerceBeginningInfo( DWORD dwRoleID, INT& nLevel, tagTaelInfo& sTaelInfo )
{
	GuildCommodity* pCommodity = GetCommodity(dwRoleID);
	if (!P_VALID(pCommodity))
	{
		return E_GuildCommerce_Status_Error;
	}

	nLevel		= pCommodity->GetLevel();
	sTaelInfo	= *(pCommodity->GetTaelInfo());

	return E_Success;
}

//-----------------------------------------------------------------------------
// 保存到排行榜
//-----------------------------------------------------------------------------
VOID GuildCommerce::Add2CommerceRank( DWORD dwRoleID, INT32 nTael, INT nTimes /*= GT_INVALID*/, BOOL bSave2DB /*= TRUE*/ )
{
	if (!GT_VALID(dwRoleID) || nTael < 0)
	{
		return;
	}

	tagCommerceRank* pRankInfo = NULL;
	VectorRank::iterator iter_rank	= m_vecRank.begin();
	for (; iter_rank != m_vecRank.end(); iter_rank++)
	{
		if (!P_VALID(*iter_rank))
		{
			continue;
		}

		if ((*iter_rank)->dwRoleID == dwRoleID)
		{
			pRankInfo	= *iter_rank;
			break;
		}
	}
	if (iter_rank == m_vecRank.end())
	{
		pRankInfo				= new tagCommerceRank;
		pRankInfo->dwRoleID		= dwRoleID;
		pRankInfo->nTael		= nTael;
		if (!GT_VALID(nTimes))
		{
			nTimes = 1;
		}
		pRankInfo->nTimes		= nTimes;
		m_vecRank.push_back(pRankInfo);
	}
	else
	{
		(*iter_rank)->nTael += nTael;
		(*iter_rank)->nTimes++;
	}

	// 更新到数据库
	if (bSave2DB && P_VALID(pRankInfo))
	{
		tagNDBC_UpdateCommerceRank send_rank;
		send_rank.dwGuildID	= m_pGuild->GetGuildAtt().dwID;
		send_rank.sRankInfo	= *pRankInfo;
		g_dbSession.Send(&send_rank, send_rank.dwSize);
	}
}

//-----------------------------------------------------------------------------
// 清除跑商排名信息
//-----------------------------------------------------------------------------
VOID GuildCommerce::RemoveCommerceRank( DWORD dwRoleID )
{
	if (GT_VALID(dwRoleID))
	{
		// 检查该玩家排名信息是否存在
		VectorRank::iterator iter_rank	= m_vecRank.begin();
		while (iter_rank != m_vecRank.end())
		{
			if (!P_VALID(*iter_rank))
			{
				iter_rank++;
				continue;
			}
			if ((*iter_rank)->dwRoleID == dwRoleID)
			{
				// 删除改排名信息
				SAFE_DEL(*iter_rank);
				m_vecRank.erase(iter_rank);
				break;
			}
			iter_rank++;
		}
	}
	else
	{
		// 删除所有排名信息
		VectorRank::iterator iter_rank	= m_vecRank.begin();
		while (iter_rank != m_vecRank.end())
		{
			if (!P_VALID(*iter_rank))
			{
				iter_rank++;
				continue;
			}
			// 删除改排名信息
			SAFE_DEL(*iter_rank);
			iter_rank++;
		}
		m_vecRank.clear();
	}

	// 通知数据库
	tagNDBC_RemoveCommerceRank send_rank;
	send_rank.dwGuildID	= m_pGuild->GetGuildAtt().dwID;
	send_rank.dwRoleID	= dwRoleID;
	g_dbSession.Send(&send_rank, send_rank.dwSize);
}