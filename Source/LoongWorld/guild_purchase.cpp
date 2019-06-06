//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_purchase..cpp
// author: sunnee
// actor:
// data: 2009-07-13
// last:
// brief: 帮派团购类
//-------------------------------------------------------------------------------------------------------

#include "StdAfx.h"

#include "../WorldDefine/mall_define.h"
#include "../WorldDefine/msg_mall.h"
#include "../WorldDefine/ItemDefine.h"

#include "../ServerDefine/log_cmdid_define.h"
#include "../ServerDefine/msg_mall.h"
#include "../ServerDefine/msg_log.h"

#include "guild_purchase.h"
#include "guild.h"
#include "att_res.h"
#include "item_creator.h"
#include "mall.h"
#include "role_mgr.h"
#include "currency.h"
#include "role.h"
#include "guild_mgr.h"

//-------------------------------------------------------------------------------------------------------
// 构造与析构
//-------------------------------------------------------------------------------------------------------
GuildPurchase::GuildPurchase()
{
	m_pGuild = NULL;
	m_mapGPInfo.Clear();
}

GuildPurchase::~GuildPurchase()
{
	Destory();
}

//-------------------------------------------------------------------------------------------------------
// 初始化、更新、销毁
//-------------------------------------------------------------------------------------------------------
BOOL GuildPurchase::Init( DWORD dwGuildID )
{
	m_mapGPInfo.Clear();

	// 上层需保证pGuild的合法性
	ASSERT(GT_VALID(dwGuildID));
	m_pGuild = g_guildMgr.GetGuild(dwGuildID);
	if (!P_VALID(m_pGuild))
	{
		return FALSE;
	}

	return TRUE;
}

VOID GuildPurchase::Update(DWORD dwTime)
{
	INT64 nMapKey = GT_INVALID;
	tagGroupPurchase* pGPInfo = NULL;

	MapGPInfo::TMapIterator iter = m_mapGPInfo.Begin();
	while (m_mapGPInfo.PeekNext(iter, nMapKey, pGPInfo))
	{
		// 超过24小时的团购失败
		pGPInfo->dwRemainTime -= dwTime;
		if ((INT32)(pGPInfo->dwRemainTime) <= 0)
		{
			RemoveGroupPurchaseInfo(pGPInfo, FALSE);
		}
	}

	// 上层判断map是否为空
}

VOID GuildPurchase::Destory()
{
	tagGroupPurchase* pGPInfo = NULL;
	MapGPInfo::TMapIterator iter = m_mapGPInfo.Begin();
	while (m_mapGPInfo.PeekNext(iter, pGPInfo))
	{
		SAFE_DEL(pGPInfo);
	}
	m_mapGPInfo.Clear();
}

//-------------------------------------------------------------------------------------------------------
// 团购信息管理
//-------------------------------------------------------------------------------------------------------
BOOL GuildPurchase::Add( tagGroupPurchase* pGPInfo, BOOL bNotify2DB /*= TRUE*/ )
{
	ASSERT(pGPInfo);
	if (!P_VALID(pGPInfo))
	{
		return FALSE;
	}

	// 验证帮派
	if (pGPInfo->dwGuildID != m_pGuild->GetGuildAtt().dwID)
	{
		return FALSE;
	}

	// 插入
	INT64 nKey = GetKey(pGPInfo->dwRoleID, pGPInfo->dwMallID);
	if (m_mapGPInfo.IsExist(nKey))
	{
		return FALSE;
	}

	m_mapGPInfo.Add(nKey, pGPInfo);

	// 通知数据库
	if (bNotify2DB)
	{
		AddGPInfo2DB(pGPInfo);
	}

	return TRUE;
}

BOOL GuildPurchase::Remove( tagGroupPurchase* pGPInfo, BOOL bNotify2DB /*= TRUE*/ )
{
	ASSERT(pGPInfo);
	if (!P_VALID(pGPInfo))
	{
		return FALSE;
	}

	// 验证帮派
	if (pGPInfo->dwGuildID != m_pGuild->GetGuildAtt().dwID)
	{
		return FALSE;
	}

	// 插入
	BOOL bRet = m_mapGPInfo.Erase(GetKey(pGPInfo->dwRoleID, pGPInfo->dwMallID));

	// 通知数据库
	if (bRet && bNotify2DB)
	{
		RemoveGPInfo2DB(pGPInfo);
	}

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 获取团购信息
//-------------------------------------------------------------------------------------------------------
DWORD GuildPurchase::GetAllSimGPInfo( INT &nGPInfoNum, tagSimGPInfo* pData )
{
	if (m_mapGPInfo.Empty())
	{
		return E_GroupPurchase_NoInfo;
	}

	tagGroupPurchase* pGPInfo = NULL;
	nGPInfoNum = 0;

	MapGPInfo::TMapIterator iter = m_mapGPInfo.Begin();
	while (m_mapGPInfo.PeekNext(iter, pGPInfo))
	{
		if (!P_VALID(pGPInfo))
		{
			// 数据损坏
			ASSERT(pGPInfo);
			IMSG(_T("\nthere is a error GP Info in map.\n"));
			continue;
		}
		pData[nGPInfoNum].dwGuildID			= pGPInfo->dwGuildID;
		pData[nGPInfoNum].dwRoleID			= pGPInfo->dwRoleID;
		pData[nGPInfoNum].dwMallID			= pGPInfo->dwMallID;
		pData[nGPInfoNum].nPrice			= pGPInfo->nPrice;
		pData[nGPInfoNum].nParticipatorNum	= pGPInfo->nParticipatorNum;
		pData[nGPInfoNum].nRequireNum		= pGPInfo->nRequireNum;
		pData[nGPInfoNum].dwRemainTime		= pGPInfo->dwRemainTime;

		nGPInfoNum++;
	}

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 获取指定团购的响应者列表
//-------------------------------------------------------------------------------------------------------
DWORD GuildPurchase::GetParticipators( DWORD dwID, DWORD dwRoleID, DWORD *pData )
{
	if (m_mapGPInfo.Empty())
	{
		return E_GroupPurchase_NoInfo;
	}

	tagGroupPurchase* pGPInfo = NULL;
	INT64 nMapKey = GetKey(dwRoleID, dwID);

	pGPInfo = m_mapGPInfo.Peek(nMapKey);

	if (!P_VALID(pGPInfo) || !P_VALID(pGPInfo->listParticipators))
	{
		return E_GroupPurchase_NoInfo;
	}

	// 没有响应者的团购数据，不应该存在
	if (pGPInfo->listParticipators->Empty())
	{
		return E_GroupPurchase_NoInfo;
	}

	INT i = 0;
	pGPInfo->listParticipators->ResetIterator();
	while (pGPInfo->listParticipators->PeekNext(pData[i++]));

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 获取团购信息数量
//-------------------------------------------------------------------------------------------------------
INT GuildPurchase::GetGroupPurchaseInfoNum()
{
	return m_mapGPInfo.Size();
}

//-------------------------------------------------------------------------------------------------------
// 获取响应者数量
//-------------------------------------------------------------------------------------------------------
INT GuildPurchase::GetParticipatorNum( DWORD dwID, DWORD dwRoleID )
{
	if (m_mapGPInfo.Empty())
	{
		return 0;
	}

	tagGroupPurchase* pGPInfo = NULL;
	INT64 nMapKey = GetKey(dwRoleID, dwID);

	pGPInfo = m_mapGPInfo.Peek(nMapKey);

	if (!P_VALID(pGPInfo) || !P_VALID(pGPInfo->listParticipators))
	{
		return 0;
	}

	return pGPInfo->listParticipators->Size();
}

//-------------------------------------------------------------------------------------------------------
// 发起团购
//-------------------------------------------------------------------------------------------------------
DWORD GuildPurchase::LaunchGroupPurchase( Role *pRole, DWORD dwID, BYTE byScope, BYTE byIndex, INT nUnitPrice, OUT tagGroupPurchase* &pGPInfo, OUT DWORD& dwItemTypeID )
{
	ASSERT(P_VALID(pRole));
	ASSERT(g_mall.IsInitOK());
	ASSERT(nUnitPrice > 0);

	DWORD dwErrorCode = E_Success;

	// 检查索引合法性
	const tagMallItemProto *pProto = g_mall.GetMallItem(byIndex, EMIT_Item)->pMallItem;
	if (!P_VALID(pProto))
	{
		return GT_INVALID;
	}

	// 检查该物品是否可以团购
	switch(byScope)
	{
	case EGPS_SMALL:
		if (pProto->bySmallGroupDiscount == (BYTE)GT_INVALID)
		{
			return E_GroupPurchase_ItemNotAllowable;
		}
		break;

	case EGPS_MEDIUM:
		if (pProto->byMediumGroupDiscount == (BYTE)GT_INVALID)
		{
			return E_GroupPurchase_ItemNotAllowable;
		}
		break;

	case EGPS_BIG:
		if (pProto->byBigGroupDiscount == (BYTE)GT_INVALID)
		{
			return E_GroupPurchase_ItemNotAllowable;
		}
		break;

	default:
		return GT_INVALID;
		break;
	}

	// 上层负责检查人物帮派
	INT32 nGroupPurchase = m_pGuild->GetGuildAtt().nGroupPurchase;

	// id
	if(pProto->dwID != dwID)
	{
		return E_Mall_ID_Error;
	}

	// 检查是否已经发起过这个物品的团购
	INT64 nMapKey = GetKey(pRole->GetID(), dwID);
	if (m_mapGPInfo.IsExist(nMapKey))
		return E_GroupPurchase_AlreadyInitiate;

	INT nPrice = 0;

	// 单价
	if(pProto->dwTimeSaleEnd != GT_INVALID && g_world.GetWorldTime() < pProto->dwTimeSaleEnd)
	{
		// 促销
		nPrice = pProto->nSalePrice;
	}
	else
	{
		// 正常
		nPrice = pProto->nPrice;
	}

	// 金钱
	if(nPrice != nUnitPrice)
	{
		return E_Mall_YuanBao_Error;
	}

	// 计算团购价格
	nPrice *= pProto->byGroupPurchaseAmount;

	switch(byScope)
	{
	case EGPS_SMALL:
		nPrice = (INT)(nPrice * (pProto->bySmallGroupDiscount/100.0f - 0.2f*nGroupPurchase/100000));
		break;

	case EGPS_MEDIUM:
		nPrice = (INT)(nPrice * (pProto->byMediumGroupDiscount/100.0f - 0.2f*nGroupPurchase/100000));
		break;

	case EGPS_BIG:
		nPrice = (INT)(nPrice * (pProto->byBigGroupDiscount/100.0f - 0.2f*nGroupPurchase/100000));
		break;
	}

	// 检查玩家元宝是否足够
	if(nPrice > pRole->GetCurMgr().GetBagYuanBao() || nPrice <= 0)
	{
		return E_BagYuanBao_NotEnough;
	}

	// 设置传出参数
	pGPInfo = new tagGroupPurchase;
	if (!P_VALID(pGPInfo))
	{
		ASSERT(pGPInfo);
		return GT_INVALID;
	}

	pGPInfo->dwGuildID		= pRole->GetGuildID();
	pGPInfo->dwRoleID		= pRole->GetID();
	pGPInfo->dwMallID		= dwID;
	pGPInfo->nPrice			= nPrice;
	pGPInfo->dwRemainTime	= pProto->dwPersistTime * 60 * 60; // 以秒为单位

	switch (byScope)
	{
	case EGPS_SMALL:
		pGPInfo->nRequireNum = pProto->bySmallGroupHeadcount;
		break;

	case EGPS_MEDIUM:
		pGPInfo->nRequireNum = pProto->byMediumGroupHeadcount;
		break;

	case EGPS_BIG:
		pGPInfo->nRequireNum = pProto->byBigGroupHeadcount;
		break;
	}
	pGPInfo->nParticipatorNum = 1;
	pGPInfo->listParticipators = new TList<DWORD>;
	if (!P_VALID(pGPInfo->listParticipators))
	{
		ASSERT(pGPInfo->listParticipators);
		SAFE_DEL(pGPInfo);
		return GT_INVALID;
	}
	pGPInfo->listParticipators->PushBack(pGPInfo->dwRoleID);

	// 将该团购信息加入管理表
	Add(pGPInfo);

	// 传出物品TypeID
	dwItemTypeID = pProto->dwTypeID;

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 响应团购
//-------------------------------------------------------------------------------------------------------
DWORD GuildPurchase::RespondGroupPurchase( Role *pRole, DWORD dwID, DWORD dwRoleID, INT nPrice, OUT tagGroupPurchase* &pGPInfo )
{
	// 检验帮派信息
	if (!GT_VALID(pRole->GetGuildID()))
		return E_GroupPurchase_NotInGuild;

	if (!P_VALID(m_pGuild->GetMember(pRole->GetID())))
		return E_GroupPurchase_NotMember;

	// 检验是否是发起人
	if (dwRoleID == pRole->GetID())
	{
		return E_GroupPurchase_IsInitiate;
	}

	// 检验指定的团购是否已经结束
	INT64 nMapKey = GetKey(dwRoleID, dwID);
	pGPInfo = m_mapGPInfo.Peek(nMapKey);

	if (!P_VALID(pGPInfo) || (INT32)(pGPInfo->dwRemainTime) <= 0)
		return E_GroupPurchase_AlreadyEnd;

	// 非法信息应该删除
	if (!P_VALID(pGPInfo->listParticipators))
		return E_GroupPurchase_AlreadyEnd;

	// 检验是否已经响应过这个团购
	DWORD dwTmpRoleID = pRole->GetID();
	if (pGPInfo->listParticipators->IsExist(dwTmpRoleID))
	{
		return E_GroupPurchase_AlreadyInitiate;
	}

	// 金钱
	if(nPrice != pGPInfo->nPrice)
	{
		return E_Mall_YuanBao_Error;
	}

	// 检查玩家元宝是否足够
	if(nPrice > pRole->GetCurMgr().GetBagYuanBao() || nPrice <= 0)
	{
		return E_BagYuanBao_NotEnough;
	}

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 生成团购物品
//-------------------------------------------------------------------------------------------------------
DWORD GuildPurchase::CreateGPItems( DWORD dwID, OUT tagMallItemSell &itemSell )
{
	ASSERT(GT_VALID(dwID));

	//验证商品的合法性
	const tagMallItemProto *pProto = g_attRes.GetMallItemProto(dwID);

	if (!P_VALID(pProto))
	{
		return E_Mall_ID_Error;
	}

	// 创建物品
	tagItem *pItemNew = ItemCreator::CreateEx(EICM_Mall, GT_INVALID, pProto->dwTypeID, pProto->byGroupPurchaseAmount, EIQ_White);
	if(!P_VALID(pItemNew))
	{
		ASSERT(P_VALID(pItemNew));
		return E_Mall_CreateItem_Failed;
	}

	// 如果有赠品，则生成赠品
	tagItem *pPresentNew = NULL;
	if(pProto->dwPresentID != GT_INVALID)
	{
		pPresentNew = ItemCreator::CreateEx(EICM_Mall, GT_INVALID, 
			pProto->dwPresentID, (INT16)pProto->byPresentNum * pProto->byGroupPurchaseAmount, EIQ_White);
		if(!P_VALID(pPresentNew))
		{
			::Destroy(pItemNew);
			ASSERT(P_VALID(pPresentNew));
			return E_Mall_CreatePres_Failed;
		}
	}

	// 回馈赠点
	if (pProto->byExAssign >= 0)
	{
		itemSell.nExVolumeAssign = pProto->byExAssign;
	}

	// 设置传出参数
	itemSell.pItem			= pItemNew;
	itemSell.pPresent		= pPresentNew;
	itemSell.nYuanBaoNeed	= 0;				// 已经预付费
	itemSell.byRemainNum	= GT_INVALID;

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 删除指定团购信息
//-------------------------------------------------------------------------------------------------------
VOID GuildPurchase::RemoveGroupPurchaseInfo( tagGroupPurchase* &pGPInfo, BOOL bSuccess /*= TRUE*/ )
{
	ASSERT(pGPInfo);

	if (!P_VALID(pGPInfo))
		return;

	// 删除管理器中的信息
	INT64 nMapKey = GetKey(pGPInfo->dwRoleID, pGPInfo->dwMallID);
	m_mapGPInfo.Erase(nMapKey);

	// 若是团购失败则返还玩家元宝
	if (!bSuccess)
	{
		ReturnCost2Participator(pGPInfo);

		m_pGuild->ModifyGroupPurchase(FALSE);

		// 广播团购失败消息
		const tagMallItemProto *pProto = g_attRes.GetMallItemProto(pGPInfo->dwMallID);

		if (!P_VALID(pProto))
		{
			// 这里不会发生吧，以防万一
			ASSERT(pProto);
			return;
		}

		tagNS_RespondBroadCast send;
		send.eType = ERespondBroadCast_Lose;
		send.dwRoleID = pGPInfo->dwRoleID;
		send.dwTypeID = pProto->dwTypeID;
		m_pGuild->SendGuildMsg(&send, send.dwSize);
	}

	// 删除数据库中的信息
	RemoveGPInfo2DB(pGPInfo);

	// 删除信息本身
	SAFE_DEL(pGPInfo);

	// 上层检查是否删除this对象
}

//-------------------------------------------------------------------------------------------------------
// 删除该帮派所有团购信息(帮派解散)
//-------------------------------------------------------------------------------------------------------
VOID GuildPurchase::RemoveGroupPurchaseInfo()
{
	// 该帮派所有团购失败
	tagGroupPurchase *pGPInfo = NULL;
	if (!m_mapGPInfo.Empty())
	{
		MapGPInfo::TMapIterator iter = m_mapGPInfo.Begin();
		while (m_mapGPInfo.PeekNext(iter, pGPInfo))
		{
			ReturnCost2Participator(pGPInfo);
			SAFE_DEL(pGPInfo);
		}
	}
	m_mapGPInfo.Clear();

	// 删除数据库中该帮派的团购信息
	RemoveGuildGPInfo2DB();

	// 上层检测是否需要删除this对象
}

//-------------------------------------------------------------------------------------------------------
// 返还玩家元宝(团购失败)
//-------------------------------------------------------------------------------------------------------
VOID GuildPurchase::ReturnCost2Participator( tagGroupPurchase* pGPInfo )
{
	if (!P_VALID(pGPInfo) || !P_VALID(pGPInfo->listParticipators))
		return;

	DWORD tmpRoleID = GT_INVALID;

	pGPInfo->listParticipators->ResetIterator();
	while(pGPInfo->listParticipators->PeekNext(tmpRoleID))
	{
		// 检查玩家是够在线
		Role* pRole = g_roleMgr.GetRolePtrByID(tmpRoleID);
		if (!P_VALID(pRole))
		{
			// 向离线玩家发送元宝
			CurrencyMgr::ModifyBaiBaoYuanBao(tmpRoleID, pGPInfo->nPrice, ELCID_GroupPurchase_Faild);
		}
		else
		{
			// 向在线玩家发送元宝
			pRole->GetCurMgr().IncBaiBaoYuanBao(pGPInfo->nPrice, ELCID_GroupPurchase_Faild);
		}
	}
}

//-------------------------------------------------------------------------------------------------------
// 数据库操作
//-------------------------------------------------------------------------------------------------------
VOID GuildPurchase::AddGPInfo2DB( tagGroupPurchase* pGPInfo )
{
	ASSERT(P_VALID(pGPInfo));
	if (!P_VALID(pGPInfo))
		return;

	tagNDBC_AddGPInfo send;

	send.GPInfo.dwGuildID			= pGPInfo->dwGuildID;
	send.GPInfo.dwRoleID			= pGPInfo->dwRoleID;
	send.GPInfo.dwMallID			= pGPInfo->dwMallID;
	send.GPInfo.nPrice				= pGPInfo->nPrice;
	send.GPInfo.nParticipatorNum	= pGPInfo->nParticipatorNum;
	send.GPInfo.nRequireNum			= pGPInfo->nRequireNum;
	send.GPInfo.nRemainTime			= pGPInfo->dwRemainTime;
	send.GPInfo.dwParticipators[0]	= pGPInfo->dwRoleID;

	g_dbSession.Send(&send, send.dwSize);
}

VOID GuildPurchase::UpdateGPInfo2DB( tagGroupPurchase* pGPInfo )
{
	ASSERT(P_VALID(pGPInfo));
	if (!P_VALID(pGPInfo))
		return;

	tagNDBC_UpdateGPInfo send;

	send.GpInfoKey.dwGuildID	= pGPInfo->dwGuildID;
	send.GpInfoKey.dwRoleID		= pGPInfo->dwRoleID;
	send.GpInfoKey.dwMallID		= pGPInfo->dwMallID;
	send.dwNewParticipator		= pGPInfo->listParticipators->GetList().back();

	g_dbSession.Send(&send, send.dwSize);
}

VOID GuildPurchase::RemoveGPInfo2DB( tagGroupPurchase* pGPInfo )
{
	ASSERT(P_VALID(pGPInfo));
	if (!P_VALID(pGPInfo))
		return;

	tagNDBC_RemoveGPInfo send;

	send.GpInfoKey.dwGuildID	= pGPInfo->dwGuildID;
	send.GpInfoKey.dwRoleID		= pGPInfo->dwRoleID;
	send.GpInfoKey.dwMallID		= pGPInfo->dwMallID;

	g_dbSession.Send(&send, send.dwSize);
}

VOID GuildPurchase::RemoveGuildGPInfo2DB()
{
	ASSERT(P_VALID(m_pGuild));
	if (!P_VALID(m_pGuild))
	{
		return;
	}

	tagNDBC_RemoveGuildGPInfo send;

	send.dwGuildID = m_pGuild->GetGuildAtt().dwID;

	g_dbSession.Send(&send, send.dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 生成键值
//-------------------------------------------------------------------------------------------------------
INT64 GuildPurchase::GetKey( DWORD dwRoleID, DWORD dwID )
{
	ASSERT(GT_VALID(dwRoleID) && GT_VALID(dwID));
	if (!GT_VALID(dwRoleID) || !GT_VALID(dwID))
		return GT_INVALID;

	INT64 n64Key = dwRoleID;

	n64Key = (n64Key << 32) | dwID;

	return n64Key;
}

//-------------------------------------------------------------------------------------------------------
// 团购成功处理
//-------------------------------------------------------------------------------------------------------
DWORD GuildPurchase::DoSuccessStuff( tagGroupPurchase* pGPInfo )
{
	// 做一下简单验证
	if (m_pGuild->GetGuildAtt().dwID != pGPInfo->dwGuildID)
	{
		return GT_INVALID;
	}

	DWORD dwItemTypeID		= GT_INVALID;
	DWORD dwLaunchRoleID	= pGPInfo->dwRoleID;

	// 向所有列表中的玩家发送物品
	DWORD tmpRoleID = GT_INVALID;
	TList<DWORD>::TListIterator iter = pGPInfo->listParticipators->Begin();	// 上层已经保证了列表的合法性
	
	while(pGPInfo->listParticipators->PeekNext(iter, tmpRoleID))
	{
		// 生成物品
		tagMallItemSell	sItemSell;
		DWORD dwErrorCode = CreateGPItems(pGPInfo->dwMallID, sItemSell);
		if (dwErrorCode != E_Success)
		{
			return dwErrorCode;
		}
		else if (P_VALID(sItemSell.pItem))
		{
			dwItemTypeID = sItemSell.pItem->dwTypeID;
		}
		else
		{
			return GT_INVALID;
		}

		// 处理结果
		if(P_VALID(sItemSell.pItem))
		{		
			INT64 n64Serial = sItemSell.pItem->n64Serial;
			DWORD dwFstGainTime = g_world.GetWorldTime();
			INT16 n16BuyNum = sItemSell.pItem->n16Num;

			// 百宝袋历史记录
			ItemMgr::ProcBaiBaoRecord(sItemSell.pItem->dwTypeID, tmpRoleID, GT_INVALID, EBBRT_GroupPurchase, dwFstGainTime);

			// log
			g_mall.LogMallSell(tmpRoleID, GT_INVALID, *sItemSell.pItem, n64Serial, n16BuyNum, 
				dwFstGainTime, sItemSell.nYuanBaoNeed, 0, ELCLD_GroupPurchase_BuyItem);

			// 将物品放到百宝袋中
			Role *pTmpRole = g_roleMgr.GetRolePtrByID(tmpRoleID);
			if(P_VALID(pTmpRole))
			{
				pTmpRole->GetItemMgr().Add2BaiBao(sItemSell.pItem, ELCLD_GroupPurchase_BuyItem);
			}
			else
			{
				// 存储到item_baibao表中
				ItemMgr::InsertBaiBao2DB(sItemSell.pItem, tmpRoleID, ELCLD_GroupPurchase_BuyItem);

				// 删除物品
				::Destroy(sItemSell.pItem);
			}

			// 如果有赠品，则放到百宝袋中
			if(P_VALID(sItemSell.pPresent))
			{
				// 百宝袋历史记录
				ItemMgr::ProcBaiBaoRecord(sItemSell.pPresent->dwTypeID, tmpRoleID, GT_INVALID, EBBRT_Mall, dwFstGainTime);

				if(P_VALID(pTmpRole))
				{
					pTmpRole->GetItemMgr().Add2BaiBao(sItemSell.pPresent, ELCLD_GroupPurchase_BuyItem_Add);
				}
				else
				{
					// 存储到item_baibao表中
					ItemMgr::InsertBaiBao2DB(sItemSell.pPresent, tmpRoleID, ELCLD_GroupPurchase_BuyItem_Add);

					// 删除物品
					::Destroy(sItemSell.pItem);
				}
			}

			// 回馈赠点
			if (P_VALID(pTmpRole) && sItemSell.nExVolumeAssign > 0)
			{
				pTmpRole->GetCurMgr().IncExchangeVolume(sItemSell.nExVolumeAssign, ELCLD_GroupPurchase_BuyItem);
			}
		}
	}

	// 帮派团购指数+1
	m_pGuild->ModifyGroupPurchase(TRUE);

	// 删除该条团购消息记录
	RemoveGroupPurchaseInfo(pGPInfo);

	// 帮派内广播成功消息
	tagNS_RespondBroadCast send;
	send.eType = ERespondBroadCast_Success;
	send.dwRoleID = dwLaunchRoleID;
	send.dwTypeID = dwItemTypeID;
	m_pGuild->SendGuildMsg(&send, send.dwSize);

	return E_Success;
}