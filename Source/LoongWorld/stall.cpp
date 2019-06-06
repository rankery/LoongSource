//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: stall.h
// author: Sxg
// actor:
// data: 2008-12-17
// last:
// brief: 摆摊
//-------------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/stall_define.h"
#include "../WorldDefine/filter.h"
#include "../WorldDefine/msg_common_errorcode.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "../ServerDefine/role_data_define.h"

#include "stall.h"
#include "role.h"
#include "item_mgr.h"
#include "item_creator.h"
#include "vip_stall.h"

INT Stall::m_SzStallItem = sizeof(tagMsgStallGoods) - 1 + SIZE_ITEM;
INT Stall::m_SzStallEquip = sizeof(tagMsgStallGoods) - 1 + SIZE_EQUIP;
//-----------------------------------------------------------------------------
// 摆摊物品结构
//-----------------------------------------------------------------------------
struct tagStallGoods
{
	INT64	n64Serial;			// 待出售的物品&装备64位ID
	INT64	n64UnitPrice;		// 单价

	tagStallGoods() { n64Serial = GT_INVALID; n64UnitPrice = 0; }
};

//-------------------------------------------------------------------------------
// 构造&析构
//-------------------------------------------------------------------------------
Stall::Stall(Role *pRole, const tagRoleDataSave* pRoleData)
{
	ASSERT(P_VALID(pRole) && P_VALID(pRoleData));

	m_pRole			= pRole;
	m_nTotalTax		= pRoleData->nTotalTax;
	m_byLevel		= pRoleData->byStallLevel;
	m_nDailyExp		= pRoleData->nStallDailyExp;
	m_nCurExp		= pRoleData->nStallCurExp;
	m_bBroadcast	= FALSE;

	m_dwLastUpgradeTime = pRoleData->dwLastStallTime;

	// 根据缴税总额计算摊位招牌模型等级和税收减免
	//MCalStallMode(m_byLevel, m_fDeTaxRate, nTotalTax);

	m_n8GoodsNum	= 0;
	m_fCityTaxRate	= 0.0f;
	m_fDeTaxRate	= 0.0f;
	m_strTitle		= NULL;
	m_strAd			= NULL;
	m_pStallGoods	= NULL;
}

Stall::~Stall()
{
	Destroy();
}

//-------------------------------------------------------------------------------
// 摆摊
//-------------------------------------------------------------------------------
DWORD Stall::Init(FLOAT fCityTaxRate)
{
// 	// 防沉迷
// 	if (m_pRole->GetEarnRate() < 10000)
// 		return GT_INVALID;

	DWORD dwErrorCode = CanStall();
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}
	
	m_n8GoodsNum	= 0;
	m_fCityTaxRate	= fCityTaxRate;

	// 申请内存
	m_pStallGoods = new tagStallGoods[STALL_MAX_DISPLAY];
	if(!P_VALID(m_pStallGoods))
	{
		return GT_INVALID;
	}

	// 设置为摆摊设置状态
	m_pRole->SetRoleState(ERS_StallSet, FALSE);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 收摊
//-------------------------------------------------------------------------------
DWORD Stall::Destroy()
{
	// 是否收摊
	if(!m_pRole->IsInRoleStateAny(ERS_Stall | ERS_StallSet))
	{
		return E_Stall_Role_Pull;
	}
	
	// 重置摆摊状态
	if(m_pRole->IsInRoleState(ERS_Stall))
	{
		m_pRole->UnsetRoleState(ERS_Stall);
	}
	
	if(m_pRole->IsInRoleState(ERS_StallSet))
	{
		// 取消摆摊设置状态，不需要向周围玩家发送消息
		m_pRole->UnsetRoleState(ERS_StallSet, FALSE);
	}

	// 通知VIP摊位状态变更
	tagVIPStall* pVIPStall = g_VIPStall.GetVIPStallInfo(m_pRole->GetID());
	if (P_VALID(pVIPStall))
	{
		EVIPStallStatus eStatus = EVSS_Close;
		g_VIPStall.AddEvent(m_pRole->GetID(), EVT_SetStallStatus, 1, &eStatus);
	}

	m_bBroadcast = FALSE;

	// 重置物品使用状态
	if(P_VALID(m_pStallGoods))
	{
		tagItem *pGoods;
		for(INT i=0; i<STALL_MAX_DISPLAY; ++i)
		{
			if(m_pStallGoods[i].n64Serial != GT_INVALID)
			{
				pGoods = m_pRole->GetItemMgr().GetBagItem(m_pStallGoods[i].n64Serial);
				if(P_VALID(pGoods))
				{
					pGoods->SetUsing(FALSE);
				}
			}
		}
	}

	m_n8GoodsNum	= 0;
	m_fCityTaxRate	= 0.0f;

	// 释放内存
	SAFE_DEL_ARRAY(m_strTitle);
	SAFE_DEL_ARRAY(m_strAd);
	SAFE_DEL_ARRAY(m_pStallGoods);

	// 移除摆摊buff
	m_pRole->RemoveBuff(Buff::GetIDFromTypeID(STALL_BUFF_ID), TRUE);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 广播广告
//-------------------------------------------------------------------------------
VOID Stall::Update()
{
	if (!m_pRole->IsInRoleState(ERS_Stall))
		return;

	// 广告
	if(m_bBroadcast && m_strAd != NULL && g_world.GetWorldTick() - m_dwLastAdBroadcastTick >= STALL_BROADCAST_INTERVAL_TICK)
	{
		// 在当前频道广播
		if(P_VALID(m_pRole->GetSession()))
		{
			m_pRole->GetSession()->BroadcastCurrChannel(m_strAd);
		}

		m_dwLastAdBroadcastTick = g_world.GetWorldTick();
	}

	// 摊位升级
	if (CalcTimeDiff(GetCurrentDWORDTime(), m_dwLastUpgradeTime) >= STALL_GAINEXP_INTERVAL_TIME)
	{
		if (IsSameDay(m_dwLastUpgradeTime))
		{
			StallUpgrade(FALSE);
		}
		else
		{
			StallUpgrade(TRUE);
		}

		m_dwLastUpgradeTime = GetCurrentDWORDTime();
	}
}

//-------------------------------------------------------------------------------
// 设置标题
//-------------------------------------------------------------------------------
DWORD Stall::SetTitle(LPCTSTR strTitle)
{
	if(!m_pRole->IsInRoleState(ERS_StallSet))
	{
		// 非法消息才会执行到此处
		ASSERT(0);
		return GT_INVALID;
	}
	
	INT nSzTitle = g_attRes.GetVariableLen().nStallTitleMax + 1;
	
	DWORD dwErrorCode = Filter::CheckName(strTitle, nSzTitle - 1);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	if(NULL == m_strTitle)
	{
		m_strTitle = new TCHAR[nSzTitle];
	}

	ZeroMemory(m_strTitle, nSzTitle * sizeof(TCHAR));
	_tcsncpy(m_strTitle, strTitle, nSzTitle - 1);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 设置广告
//-------------------------------------------------------------------------------
DWORD Stall::SetAdText(LPCTSTR strAd)
{
	if(!m_pRole->IsInRoleState(ERS_StallSet))
	{
		// 非法消息才会执行到此处
		ASSERT(0);
		return GT_INVALID;
	}
	
	INT nSzAd = STALL_MAX_ADTEXT_NUM;

	DWORD dwErrorCode = Filter::CheckName(strAd, nSzAd - 1);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	if(NULL == m_strAd)
	{
		m_strAd = new TCHAR[nSzAd];
	}

	ZeroMemory(m_strAd, nSzAd * sizeof(TCHAR));
	_tcsncpy(m_strAd, strAd, nSzAd - 1);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 设置广告播放标志
//-------------------------------------------------------------------------------
DWORD Stall::SetAdFlag(bool bBroadcast)
{
	if(!m_pRole->IsInRoleStateAny(ERS_Stall | ERS_StallSet))
	{
		// 非法消息才会执行到此处
		ASSERT(0);
		return GT_INVALID;
	}

	m_bBroadcast = bBroadcast;

	return E_Success;
}

//-------------------------------------------------------------------------------
// 商品上架
//-------------------------------------------------------------------------------
DWORD Stall::SetGoods(const INT64 n64Serial, const INT64 n64UnitPrice, const BYTE byIndex)
{
	if(!m_pRole->IsInRoleStateAny(ERS_Stall | ERS_StallSet))
	{
		// 非法消息才会执行到此处
		ASSERT(0);
		return GT_INVALID;
	}
	
	//// 是否已初始化
	//if(NULL == m_pStallGoods)
	//{
	//	// 非法消息才会执行到此处
	//	ASSERT(0);
	//	return GT_INVALID;
	//}

	// 金钱数量合法性判断
	if(n64UnitPrice < STALL_MIN_SOLD_SLIVER 
		|| n64UnitPrice > STALL_MAX_SOLD_SLIVER)
	{
		// 非法消息才会执行到此处
		ASSERT(0);
		return GT_INVALID;
	}
	
	// 下标判断
	if(byIndex >= STALL_MAX_DISPLAY)
	{
		// 非法消息才会执行到此处
		ASSERT(0);
		return GT_INVALID;
	}

	// 栏位判断
	if(m_pStallGoods[byIndex].n64Serial != GT_INVALID)
	{
		// 非法消息才会执行到此处
		ASSERT(0);
		return GT_INVALID;
	}
	
	// 取得物品
	tagItem *pGoods = m_pRole->GetItemMgr().GetBagItem(n64Serial);
	if(!P_VALID(pGoods))
	{
		return E_Stall_ItemNotFound_InBag;
	}

	// 是否已处于使用状态
	if(pGoods->bUsing)
	{
		return E_Stall_Goods_InUsed;
	}

	// 判断物品是否可以出售
	if(!m_pRole->GetItemMgr().CanExchange(*pGoods))
	{
		return E_Stall_ItemCanNot_Sold;
	}

	// 设置摆摊商品
	m_pStallGoods[byIndex].n64Serial = n64Serial;
	m_pStallGoods[byIndex].n64UnitPrice = n64UnitPrice;

	pGoods->SetUsing(TRUE);

	// 计数
	++m_n8GoodsNum;

	return E_Success;
}

//-------------------------------------------------------------------------------
// 商品下架
//-------------------------------------------------------------------------------
DWORD Stall::UnSetGoods(const BYTE byIndex)
{
	if(!m_pRole->IsInRoleStateAny(ERS_Stall | ERS_StallSet))
	{
		// 非法消息才会执行到此处
		ASSERT(0);
		return GT_INVALID;
	}

	// 下标判断
	if(byIndex >= STALL_MAX_DISPLAY)
	{
		// 非法消息才会执行到此处
		ASSERT(0);
		return GT_INVALID;
	}

	// 栏位判断
	if(GT_INVALID == m_pStallGoods[byIndex].n64Serial)
	{
		// 非法消息，或商品已经出售
		return E_Stall_Goods_BeSold;
	}

	// 获得商品，重置使用标志
	tagItem *pGoods = m_pRole->GetItemMgr().GetBagItem(m_pStallGoods[byIndex].n64Serial);
	if(P_VALID(pGoods))
	{
		pGoods->SetUsing(FALSE);
	}
	
	// 下架商品
	UnSet(byIndex);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 获取摊位标题
//-------------------------------------------------------------------------------
DWORD Stall::GetStallTitle(OUT LPTSTR pSzTitle)
{
	// 是否收摊
	if(!m_pRole->IsInRoleState(ERS_Stall))
	{
		return E_Stall_Role_Pull;
	}

	if(NULL == m_strTitle)
	{
		pSzTitle[0] = _T('\0');
	}
	else
	{
		_tcsncpy(pSzTitle, m_strTitle, STALL_MAX_TITLE_NUM - 1);
		pSzTitle[STALL_MAX_TITLE_NUM - 1] = _T('\0');
	}

	return E_Success;
}

//-------------------------------------------------------------------------------
// 获得摊位上指定位置上的物品
//-------------------------------------------------------------------------------
DWORD Stall::GetSpecGoods(BYTE byIndex, OUT LPVOID pData, OUT INT &nGoodsSz)
{
	nGoodsSz	= 0;

	if(GT_INVALID == m_pStallGoods[byIndex].n64Serial)
	{
		return E_Stall_Goods_BeSold;
	}

	MTRANS_POINTER(pCur, pData, tagMsgStallGoods);
	pCur->n64UnitPrice	= m_pStallGoods[byIndex].n64UnitPrice;
	pCur->byIndex		= byIndex;

	tagItem *pGoods = m_pRole->GetItemMgr().GetBagItem(m_pStallGoods[byIndex].n64Serial);
	if(!P_VALID(pGoods))
	{
		// 没有在背包中找到(可能被玩家使用、丢弃等)
		UnSet(byIndex);
		return E_Stall_Goods_BeSold;
	}

	if(MIsEquipment(pGoods->dwTypeID))
	{
		pCur->byItem	= 0;
		IFASTCODE->MemCpy(pCur->byData, pGoods, SIZE_EQUIP);
		nGoodsSz = m_SzStallEquip;
	}
	else
	{
		pCur->byItem	= 1;
		IFASTCODE->MemCpy(pCur->byData, pGoods, SIZE_ITEM);
		nGoodsSz = m_SzStallItem;
	}

	return E_Success;
}

//-------------------------------------------------------------------------------
// 获得摊位上的所有物品
//-------------------------------------------------------------------------------
DWORD Stall::GetGoods(OUT LPVOID pData, OUT BYTE &byGoodsNum, OUT INT &nGoodsSz)
{
	byGoodsNum	= 0;
	nGoodsSz	= 0;
	
	// 是否收摊
	if(!m_pRole->IsInRoleState(ERS_Stall))
	{
		return E_Stall_Role_Pull;
	}

	INT8 n8Num = 0;
	MTRANS_POINTER(pCur, pData, BYTE);

	INT nGoodsSzTmp = 0;
	for(INT i=0; i<STALL_MAX_DISPLAY; ++i)
	{
		if(GetSpecGoods(i, pCur, nGoodsSzTmp) != E_Success)
		{
			continue;
		}

		pCur += nGoodsSzTmp;
		nGoodsSz += nGoodsSzTmp;

		++n8Num;
		if(m_n8GoodsNum == n8Num)
		{
			break;
		}
	}

	byGoodsNum	= m_n8GoodsNum;

	return E_Success;
}

//-------------------------------------------------------------------------------
// 出售商品
//-------------------------------------------------------------------------------
DWORD Stall::Sell(Role *pRole, INT64 n64UnitSilver, 
				  INT64 n64Serial, BYTE byIndex, INT16 n16Num, OUT INT16 &n16RemainNum)
{
	// 下标判断
	if(byIndex >= STALL_MAX_DISPLAY || n16Num <= 0)
	{
		// 非法消息才会执行到此处
		ASSERT(0);
		return GT_INVALID;
	}
	
	// 是否收摊
	if(!m_pRole->IsInRoleState(ERS_Stall))
	{
		return E_Stall_Role_Pull;
	}
	
	// 栏位判断
	if(GT_INVALID == m_pStallGoods[byIndex].n64Serial)
	{
		return E_Stall_Goods_BeSold;
	}

	// 检查商品
	if(n64Serial != m_pStallGoods[byIndex].n64Serial)
	{
		return E_Stall_Goods_Refresh;
	}

	// 检查售价
	if(m_pStallGoods[byIndex].n64UnitPrice != n64UnitSilver)
	{
		return E_Stall_Goods_Refresh_Price;
	}

	// 根据税率计算玩家获得的金钱
	INT64 n64Total		= n64UnitSilver * n16Num;
	INT64 n64TotalPay	= INT64((FLOAT)n64Total * (1.0f - max(0, (m_fCityTaxRate - m_fDeTaxRate))));
	INT32 nTax			= (INT32)(n64Total - n64TotalPay);

	// 检查买方玩家金钱
	if(pRole->GetCurMgr().GetBagSilver() < n64TotalPay)
	{
		return E_Stall_CustomerMoney_NotEnough;
	}

	// 检查买方玩家背包
	if(pRole->GetItemMgr().GetBagFreeSize() < 1)
	{
		return E_Stall_CustomerBug_NoFreeSpace;
	}

	tagItem *pGoods = m_pRole->GetItemMgr().GetBagItem(n64Serial);
	if(!P_VALID(pGoods))
	{
		UnSet(byIndex);
		return E_Stall_Goods_NotInBag;
	}
	
	// 检查物品数量
	if(pGoods->n16Num < n16Num)
	{
		return E_Stall_GoodsNum_NotEnough;
	}

	// 设置传出参数
	n16RemainNum = pGoods->n16Num - n16Num;
	
	// 交易 -- 扣除物品和金钱
	tagItem *pItem;
	DWORD	dwErrorCode;
	if(n16Num == pGoods->n16Num)
	{
		pItem = pGoods;

		UnSet(byIndex);
		dwErrorCode = m_pRole->GetItemMgr().TakeOutFromBag(pItem->n64Serial, ELCLD_Stall_BeSold, TRUE);
	}
	else
	{
		// 生成新的堆物品
		pItem = ItemCreator::Create(*pGoods, n16Num);

		dwErrorCode = m_pRole->GetItemMgr().DelFromBag(n64Serial, (DWORD)ELCLD_Stall_BeSold, n16Num);
	}

	if(dwErrorCode != E_Success)
	{
		// 如上层判断有遗漏，此时可能会有内存泄漏
		ASSERT(0);
		return dwErrorCode;
	}

	if(!pRole->GetCurMgr().DecBagSilver(n64TotalPay, (DWORD)ELCLD_Stall_Buy))
	{
		// 是否归还物品//??
		
		// 如上层判断有遗漏，此时可能会有内存泄漏
		ASSERT(0);
		return dwErrorCode;
	}

	// 重置使用状态
	pItem->SetUsing(FALSE);

	// 交易 -- 获得物品和金钱
	m_pRole->GetCurMgr().IncBagSilver(n64TotalPay, (DWORD)ELCLD_Stall_BeSold);
	pRole->GetItemMgr().Add2Bag(pItem, (DWORD)ELCLD_Stall_Buy, TRUE);

	// 累计税收
	if(nTax != 0)
	{
		ASSERT(m_nTotalTax < STALL_MAX_TAX);
		m_nTotalTax = min(m_nTotalTax + nTax, STALL_MAX_TAX);
	}
	
	
	return E_Success;
}

//-------------------------------------------------------------------------------
// 判断是否可以摆摊
//-------------------------------------------------------------------------------
DWORD Stall::CanStall()
{
	// 判断是否已经处于摆摊或摆摊设置状态
	if(m_pRole->IsInRoleStateAny(ERS_Stall | ERS_StallSet))
	{
		return E_Stall_Role_InStall;
	}

	//// 判断是否处于备战状态
	//if(m_pRole->IsInRoleStateAny(ERS_Combat | ERS_PK | ERS_PVP))
	//{
	//	return E_Stall_RoleState_NotPermit;
	//}

	// 等级>=30
	if(m_pRole->GetLevel() < STALL_MIN_ROLE_LEVEL)
	{
		return E_Stall_Role_Level_2Low;
	}
	
	// 场景检查
	if(!m_pRole->IsInRoleState(ERS_StallArea))
	{
		return E_Stall_Area_NotAllowed;
	}

	// 空间4个格子 -- 该功能由客户端实现

	return E_Success;
}

//-------------------------------------------------------------------------------
// 下架商品
//-------------------------------------------------------------------------------
VOID Stall::UnSet(const BYTE byIndex)
{
	ASSERT(byIndex < STALL_MAX_DISPLAY);
	
	m_pStallGoods[byIndex].n64Serial = GT_INVALID;
	m_pStallGoods[byIndex].n64UnitPrice = 0;

	// 计数
	--m_n8GoodsNum;
}

//-------------------------------------------------------------------------------
// 上架商品完成，向周围玩家广播
//-------------------------------------------------------------------------------
DWORD Stall::SetFinish()
{ 
	// 是否已处理摆摊状态
	if(m_pRole->IsInRoleState(ERS_Stall))
	{
		return GT_INVALID;
	}
	
	// 是否处于摆摊设置状态
	if(!m_pRole->IsInRoleState(ERS_StallSet))
	{
		return GT_INVALID;
	}

	if(IsEmpty())
	{
		return E_Stall_Goods_Empty;
	}

	// 场景检查 -- 再次确认是否在摆摊区域
	if(!m_pRole->IsInRoleState(ERS_StallArea))
	{
		return E_Stall_Area_NotAllowed;
	}

	// 重新设置状态
	m_pRole->SetRoleState(ERS_Stall);
	m_pRole->UnsetRoleState(ERS_StallSet, FALSE);

	// 加摆摊的buff
	m_pRole->TryAddBuff(m_pRole, g_attRes.GetBuffProto(STALL_BUFF_ID), NULL, NULL, NULL);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 粗略计算摊位上的商品所用内存，即存入tagMsgStallGoods结构时的大小
//-------------------------------------------------------------------------------
INT32 Stall::CalMemSzGoodsUsed() const
{
	return m_n8GoodsNum * m_SzStallEquip;
}

//-------------------------------------------------------------------------------
// 摊位升级
//-------------------------------------------------------------------------------
VOID Stall::StallUpgrade(BOOL bNextDay)
{
	// 当天经验清零
	if (bNextDay)
		m_nDailyExp = 0;

	// 摊位等级上限
	if (m_byLevel >= STALL_MAX_LEVEL)
		return;

	// 当日经验上限
	if (m_nDailyExp >= STALL_MAX_DAILY_EXP)
		return;

	// 人物等级限制
	if (m_pRole->GetLevel() < STALL_EXP_LEVEL_LIMIT[m_byLevel])
		return;

	// 经验提升
	m_nDailyExp++;
	m_nCurExp++;

	// 检查升级
	INT nLevelExp = MCalStallLevelExp(m_byLevel);
	if (m_nCurExp >= nLevelExp)
	{
		m_byLevel++;
		m_nCurExp -= nLevelExp;
	}
}

//-------------------------------------------------------------------------------
// 检查与当前时间是否是同一天
//-------------------------------------------------------------------------------
BOOL Stall::IsSameDay( DWORD dwTime )
{
	tagDWORDTime dwSrcTime(dwTime);
	tagDWORDTime dwCurTime = GetCurrentDWORDTime();

	if ((dwCurTime.year == dwSrcTime.year) && (dwCurTime.month == dwSrcTime.month) && (dwCurTime.day == dwSrcTime.day))
	{
		return TRUE;
	}

	return FALSE;
}

//-------------------------------------------------------------------------------
// 保存摊位信息(同角色一起保存)
//-------------------------------------------------------------------------------
VOID Stall::Save2DB( tagRoleDataSave* pRoleData )
{
	if (!P_VALID(pRoleData))	return;

	pRoleData->byStallLevel		= m_byLevel;
	pRoleData->nStallDailyExp	= m_nDailyExp;
	pRoleData->nStallCurExp		= m_nCurExp;
	pRoleData->dwLastStallTime	= m_dwLastUpgradeTime;
}

//-------------------------------------------------------------------------------
// 摊位获得经验(GM命令用)
//-------------------------------------------------------------------------------
DWORD Stall::GainExp( INT32 nExp )
{
	// 参数为-1时重置摊位等级经验
	if (!GT_VALID(nExp))
	{
		m_nCurExp	= 0;
		m_nDailyExp	= 0;
		m_byLevel	= 1;
		return E_Success;
	}

	// 其他非正值判定为非法
	if (nExp <= 0)
	{
		return GT_INVALID;
	}

	// 人物等级限制
	if (m_pRole->GetLevel() < STALL_EXP_LEVEL_LIMIT[m_byLevel])
		return GT_INVALID;

	// 经验提升(这里不限制每日经验上限)
	m_nCurExp += nExp;
	m_nCurExp = m_nCurExp < 0 ? 0 : m_nCurExp;

	// 计算当前经验合适的等级
	while (m_byLevel < STALL_MAX_LEVEL)
	{
		INT nLevelExp = MCalStallLevelExp(m_byLevel);
		if ((m_nCurExp >= nLevelExp) && (m_pRole->GetLevel() >= STALL_EXP_LEVEL_LIMIT[m_byLevel]))
		{
			m_byLevel++;
			m_nCurExp -= nLevelExp;
		}
		else
		{
			break;
		}
	}

	// 若已经满级则清空经验
	if (m_byLevel == STALL_MAX_LEVEL)
	{
		m_nCurExp	= 0;
		m_nDailyExp	= 0;
	}

	return E_Success;
}