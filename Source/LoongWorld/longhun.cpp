//------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: longhun.cpp
// author: Sxg
// actor:
// data: 2009-3-17
// last:
// brief: 龙魂能力激活机制处理类
//------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/ItemDefine.h"
#include "../ServerDefine/base_define.h"

#include "longhun.h"
#include "att_res.h"
#include "role.h"
#include "map.h"
//------------------------------------------------------------------------
// 构造&析构
//------------------------------------------------------------------------
LongHun::LongHun(Role *pRole)
{
	m_pRole			= pRole;
	m_n8NumPassive	= 0;
	m_n8NumActive	= 0;
	m_listActive.Clear();

	for(INT i=0; i<EEP_MaxEquip; ++i)
	{
		m_dwPassive[i]	= GT_INVALID;
		m_dwActive[i]	= GT_INVALID;
	}
}

LongHun::~LongHun()
{
}

//------------------------------------------------------------------------
// 增加&删除
//------------------------------------------------------------------------
VOID LongHun::Add(const tagEquip *pEquip, const INT16 n16EquipPos, BOOL bSend2Client)
{
	if(!P_VALID(pEquip))
	{
		return;
	}
	
	// 时装没有龙魂能力
	if(MIsFashion(pEquip))
	{
		return;
	}

	// 注意：该处不可与时装判断调换顺序
	ASSERT(n16EquipPos >= EEP_Equip_Start && n16EquipPos <= EEP_Equip_End);

	// 设置该件装备上的龙魂能力
	Set(pEquip, n16EquipPos, bSend2Client);

	// 该件装备影响的装备
	for(INT i=0; i<X_LONGHUN_EFFECT_POS_NUM; ++i)
	{
		INT16 n16Pos = g_sLongHunCanActivePos[n16EquipPos].eEquipPos[i];
		if(n16Pos < EEP_Equip_Start || n16Pos > EEP_Equip_End)	// (EEP_NULL == n16Pos)
		{
			continue;
		}

		// 检查是否已经激活
		if(GetLongHunID(n16Pos) != GT_INVALID)
		{
			continue;
		}

		// 获取装备栏上对应部位的装备
		tagEquip *pEquip1 = m_pRole->GetItemMgr().GetEquipBarEquip(n16Pos);
		if(!P_VALID(pEquip1))
		{
			continue;
		}

		// 处理装备上的龙魂能力
		Set(pEquip1, n16Pos, bSend2Client);

		// 激活特效
		if(bSend2Client && MIsWeapon(pEquip1))
		{
			SendEquipEffect2Client(n16Pos);
		}
	}
}

VOID LongHun::Remove(const INT16 n16EquipPos)
{
	// 不是非时装装备
	if(n16EquipPos < EEP_Equip_Start || n16EquipPos > EEP_Equip_End)
	{
		return;
	}

	// 当前位置是否已有龙魂被激活
	if(GetLongHunID(n16EquipPos) != GT_INVALID)
	{
		Unset(n16EquipPos, TRUE);
	}

	// 该件装备影响的装备
	for(INT i=0; i<X_LONGHUN_EFFECT_POS_NUM; ++i)
	{
		INT16 n16Pos = g_sLongHunCanActivePos[n16EquipPos].eEquipPos[i];
		if(n16Pos < EEP_Equip_Start || n16Pos > EEP_Equip_End)	// (EEP_NULL == n16Pos)
		{
			continue;
		}

		// 检查是否已经激活
		const DWORD dwLongHunID = GetLongHunID(n16Pos);
		if(GT_INVALID == dwLongHunID)
		{
			continue;
		}

		// 取消激活
		Unset(n16Pos, FALSE);

		// 获取装备栏上对应部位的装备
		tagEquip *pEquip1 = m_pRole->GetItemMgr().GetEquipBarEquip(n16Pos);
		if(!P_VALID(pEquip1))
		{
			// 若执行到此，说明龙魂能力处理逻辑有漏洞
			ASSERT(P_VALID(pEquip1));
			continue;
		}

		// 处理装备上的龙魂能力
		Set(pEquip1, n16Pos, FALSE);

		// 判断是否需要给客户端发消息
		if(GetLongHunID(n16Pos) == GT_INVALID)
		{
			SendRemove2Client(n16Pos);
			
			if(MIsWeaponByEquipPos(n16Pos))
			{
				SendEquipEffect2Client(n16Pos);
			}
		}
	}
}

//------------------------------------------------------------------------
// 初始化发送给客户端的数据
//------------------------------------------------------------------------
VOID LongHun::InitSendInitState(OUT INT16 n16Data[], INT8 n8Sz) const
{
	ASSERT(GetActiveNum() + GetPassiveNum() == n8Sz);

	if(0 == n8Sz)
	{
		return;
	}
	
	INT nIndex = 0;
	for(INT i=0; i<EEP_MaxEquip; ++i)
	{
		if(GetLongHunID(i) != GT_INVALID)
		{
			n16Data[nIndex++] = i;
			if(nIndex >= n8Sz)
			{
				return;
			}
		}
	}
}

//------------------------------------------------------------------------
// 计算龙附属性 -- 即五行属性
//------------------------------------------------------------------------
VOID LongHun::CalLongfuAtt(OUT BYTE byLongfuAtt[], const INT nSz, INT16 n16EquipPos)
{
	const tagEquip *pEquip = m_pRole->GetItemMgr().GetEquipBarEquip(n16EquipPos);
	if(P_VALID(pEquip))
	{
		for(INT i=0; i<nSz; ++i)
		{
			byLongfuAtt[i] += pEquip->GetLongfuAtt(i);
		}
	}
}

//------------------------------------------------------------------------
// 判断该件装备是可被否激活
//------------------------------------------------------------------------
BOOL LongHun::IsNeedCount(const INT16 n16EquipPos, 
						  const tagLongHunProto *pLongHunProto,
						  const BOOL bOuterLongHun)
{
	ASSERT(P_VALID(pLongHunProto));

	BYTE byLongfuAtt[X_WUXING_TYPE_NUM];
	ZeroMemory(byLongfuAtt, sizeof(byLongfuAtt));
	
	if(bOuterLongHun)
	{
		CalLongfuAtt(byLongfuAtt, X_WUXING_TYPE_NUM, g_sLongHunActivePos[n16EquipPos].eOuter);
	}
	else
	{
		CalLongfuAtt(byLongfuAtt, X_WUXING_TYPE_NUM, g_sLongHunActivePos[n16EquipPos].eInner[0]);
		CalLongfuAtt(byLongfuAtt, X_WUXING_TYPE_NUM, g_sLongHunActivePos[n16EquipPos].eInner[1]);
	}

	for(INT i=0; i<X_WUXING_TYPE_NUM; ++i)
	{
		if(byLongfuAtt[i] < pLongHunProto->byWuXingActive[i])
		{
			return FALSE;
		}
	}

	return TRUE;
}

//------------------------------------------------------------------------
// 设置龙魂能力
//------------------------------------------------------------------------
VOID LongHun::Set(const tagEquip *pEquip, const INT16 n16EquipPos, BOOL bSend2Client)
{
	// 若已激活，则不可再被激活
	if(GetLongHunID(n16EquipPos) != GT_INVALID)
	{
		return;
	}

	// 获取装备上的龙魂能力和类型
	DWORD	dwLongHunID = GT_INVALID;
	BOOL	bOuterLongHun = TRUE;

	pEquip->GetLongHunID(dwLongHunID, bOuterLongHun);
	
	// 指定装备上没有龙魂能力
	if(GT_INVALID == dwLongHunID)
	{
		return;
	}

	// 获取龙魂能力静态属性
	const tagLongHunProto *pLongHunProto = g_attRes.GetLongHunProto(dwLongHunID);
	if(!P_VALID(pLongHunProto))
	{
		MAttResCaution(_T("longhun_proto.xml"), _T("dwID"), dwLongHunID);
		ASSERT(0);
		return;
	}

	// 判断是否满足激活条件
	if(!IsNeedCount(n16EquipPos, pLongHunProto, bOuterLongHun))
	{
		return;
	}

	// 读取龙魂能力中trigger的类型
	const tagTriggerProto *pTriggerProto = g_attRes.GetTriggerProto(pLongHunProto->dwTriggerID);
	if(!P_VALID(pTriggerProto))
	{
		MAttResCaution(_T("trigger_proto.xml"), _T("dwTriggerID"), pLongHunProto->dwTriggerID);
		ASSERT(0);
		return;
	}

	// 根据trigger类型设置龙魂能力
	switch(pTriggerProto->eEventType)
	{
	case ETEE_Use:			// 使用
		m_dwActive[n16EquipPos] = dwLongHunID;
		m_listActive.PushBack(dwLongHunID);
		++m_n8NumActive;
		break;
	default:
		m_dwPassive[n16EquipPos] = dwLongHunID;
		++m_n8NumPassive;

		// trigger,buff注册等
		m_pRole->RegisterTriggerLongHunSet(pLongHunProto->dwTriggerID, pLongHunProto->dwBuffID1, n16EquipPos);
		m_pRole->RegisterTriggerLongHunSet(pLongHunProto->dwTriggerID, pLongHunProto->dwBuffID2, n16EquipPos);
		break;
	}

	// 龙魂能力激活特效 -- 武器才有
	if(MIsWeapon(pEquip))
	{
		INT nDisplayPos = m_pRole->GetEquipDisplayPos(n16EquipPos);
		ASSERT(nDisplayPos != GT_INVALID);
		
		if(bOuterLongHun)
		{
			m_pRole->SetEquipEffect(nDisplayPos, EEE_Simple);
		}
		else
		{
			m_pRole->SetEquipEffect(nDisplayPos, EEE_Complex);
		}
	}

	// 向客户端发送消息
	if(bSend2Client)
	{
		SendAdd2Client(n16EquipPos);
	}
}

//------------------------------------------------------------------------
// 取消对应位置龙魂能力的设置
//------------------------------------------------------------------------
VOID LongHun::Unset(const INT16 n16EquipPos, BOOL bSend2Client)
{
	DWORD dwLongHunID = GetLongHunID(n16EquipPos);

	ASSERT(dwLongHunID != GT_INVALID);

	// 获取龙魂能力静态属性
	const tagLongHunProto *pLongHunProto = g_attRes.GetLongHunProto(dwLongHunID);
	if(!P_VALID(pLongHunProto))
	{
		MAttResCaution(_T("longhun_proto.xml"), _T("dwID"), dwLongHunID);
		ASSERT(0);
		return;
	}

	// 读取龙魂能力中trigger的类型
	const tagTriggerProto *pTriggerProto = g_attRes.GetTriggerProto(pLongHunProto->dwTriggerID);
	if(!P_VALID(pTriggerProto))
	{
		MAttResCaution(_T("trigger_proto.xml"), _T("dwTriggerID"), pLongHunProto->dwTriggerID);
		ASSERT(0);
		return;
	}

	// 根据trigger类型反设置龙魂能力
	switch(pTriggerProto->eEventType)
	{
	case ETEE_Use:			// 使用
		m_dwActive[n16EquipPos] = GT_INVALID;
		m_listActive.Erase(dwLongHunID);
		--m_n8NumActive;
		break;
	default:
		m_dwPassive[n16EquipPos] = GT_INVALID;
		--m_n8NumPassive;

		// trigger,buff反注册等
		m_pRole->UnRegisterTriggerLongHunSet(pLongHunProto->dwTriggerID, pLongHunProto->dwBuffID1, n16EquipPos);
		m_pRole->UnRegisterTriggerLongHunSet(pLongHunProto->dwTriggerID, pLongHunProto->dwBuffID2, n16EquipPos);
		break;
	}

	// 取消龙魂能力激活特效 -- 武器才有
	if(MIsWeaponByEquipPos(n16EquipPos))
	{
		m_pRole->SetEquipEffect(m_pRole->GetEquipDisplayPos(n16EquipPos), EEE_Null);
	}

	if(bSend2Client)
	{
		SendRemove2Client(n16EquipPos);
	}
}

//------------------------------------------------------------------------
// 将龙魂能力激活状态变化发送给客户端
//------------------------------------------------------------------------
VOID LongHun::SendAdd2Client(const INT16 n16EquipPos)
{
	tagNS_LongHunOn send;
	send.n16EquipPos = n16EquipPos;
	m_pRole->SendMessage(&send, send.dwSize);
}

VOID LongHun::SendRemove2Client(const INT16 n16EquipPos)
{
	tagNS_LongHunOff send;
	send.n16EquipPos = n16EquipPos;
	m_pRole->SendMessage(&send, send.dwSize);
}

VOID LongHun::SendEquipEffect2Client(const INT16 n16EquipPos)
{
	Map *pMap = m_pRole->GetMap();
	if(!P_VALID(pMap))
	{
		return;
	}

	INT nDisplayPos = m_pRole->GetEquipDisplayPos(n16EquipPos);
	ASSERT(EAE_RWeapon == nDisplayPos || EAE_LWeapon == nDisplayPos);

	tagNS_EquipEffectChange send;
	send.dwRoleID		= m_pRole->GetID();
	send.byDisplayPos	= (BYTE)nDisplayPos;
	send.byEquipEffect	= m_pRole->GetAvatarEquip().AvatarEquip[nDisplayPos].byEquipEffect;
	pMap->SendBigVisTileMsg(m_pRole, &send, send.dwSize);
}