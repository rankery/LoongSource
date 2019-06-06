//--------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: suit.cpp
// author: Sxg
// actor:
// data: 2008-12-2
// last:
// brief: 套装类
//--------------------------------------------------------------------------------------------
#include "StdAfx.h"

#include "../ServerDefine/base_define.h"
#include "../WorldDefine/ItemDefine.h"
#include "../WorldDefine/suit_define.h"
#include "../WorldDefine/msg_item.h"

#include "suit.h"
#include "role.h"
//--------------------------------------------------------------------------------------------
// 构造&析构
//--------------------------------------------------------------------------------------------
Suit::Suit(Role *pRole)
{
	m_pRole = pRole;
}

Suit::~Suit()
{
	m_pRole = NULL;
}

//--------------------------------------------------------------------------------------------
// 穿装备
//--------------------------------------------------------------------------------------------
VOID Suit::Add(const tagEquip *pEquip, INT16 n16EquipPos, BOOL bSend2Client/* = TRUE*/)
{
	ASSERT(pEquip->n16Index == n16EquipPos);
	const tagEquipProto *pEquipProto = pEquip->pEquipProto;

	for(INT i=0; i<MAX_PEREQUIP_SUIT_NUM; ++i)
	{
		if(GT_INVALID == pEquipProto->dwSuitID[i])
		{
			break;
		}

		// 判断是否为套装部件
		if(!IsSuitPart(pEquip, pEquipProto, i, n16EquipPos))
		{
			continue;
		}

		// 添加到map中
		m_mapSuitNum.ModifyValue(pEquipProto->dwSuitID[i], 1);

		// 计算效果
		INT nCnt = m_mapSuitNum.Peek(pEquipProto->dwSuitID[i]);
		if(bSend2Client)
		{
			// 将新的套装个数发送到客户端
			tagNS_SuitNum sendSuitNum;
			sendSuitNum.dwSuitID	= pEquipProto->dwSuitID[i];
			sendSuitNum.n8Num		= nCnt;
			m_pRole->SendMessage(&sendSuitNum, sendSuitNum.dwSize);
		}

		if(nCnt < MIN_SUIT_EQUIP_NUM)
		{
			continue;
		}

		const tagSuitProto *pSuitProto = g_attRes.GetSuitProto(pEquipProto->dwSuitID[i]);
		if(!P_VALID(pSuitProto))
		{
			ASSERT(P_VALID(pSuitProto));
			continue;
		}

		for(INT n=0; n<MAX_SUIT_ATT_NUM; ++n)
		{
			if(nCnt < pSuitProto->n8ActiveNum[n])
			{
				break;
			}
			else if(pSuitProto->n8ActiveNum[n] == nCnt)
			{
				// 添加隐藏属性
				m_pRole->RegisterTriggerSuitSet(pSuitProto->dwTriggerID[n], 
								pSuitProto->dwBuffID[n], pEquipProto->dwSuitID[i]);
			}
		}

		// 特效
		if(pSuitProto->n8SpecEffectNum == nCnt)
		{
			m_pRole->SetSuitEffect(pEquipProto->dwSuitID[i]);

			if(bSend2Client && P_VALID(m_pRole->GetMap()))
			{
				// 将特效ID发送给客户端
				tagNS_SuitEffect sendSuitEffect;
				sendSuitEffect.dwRoleID			= m_pRole->GetID();
				sendSuitEffect.dwSuitEffectID	= pEquipProto->dwSuitID[i];

				m_pRole->GetMap()->SendBigVisTileMsg(m_pRole, &sendSuitEffect, sendSuitEffect.dwSize);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
// 脱装备
//--------------------------------------------------------------------------------------------
VOID Suit::Remove(const tagEquip *pEquip, INT16 n16OldIndex)
{
	const tagEquipProto *pEquipProto = pEquip->pEquipProto;

	for(INT i=0; i<MAX_PEREQUIP_SUIT_NUM; ++i)
	{
		if(GT_INVALID == pEquipProto->dwSuitID[i])
		{
			break;
		}

		// 判断是否为套装部件
		if(!IsSuitPart(pEquip, pEquipProto, i, n16OldIndex))
		{
			continue;
		}

		// 添加到map中
		m_mapSuitNum.ModifyValue(pEquipProto->dwSuitID[i], -1);

		// 计算效果
		INT nCnt = m_mapSuitNum.Peek(pEquipProto->dwSuitID[i]);

		// 将新的套装个数发送到客户端
		tagNS_SuitNum sendSuitNum;
		sendSuitNum.dwSuitID	= pEquipProto->dwSuitID[i];
		sendSuitNum.n8Num		= nCnt;
		m_pRole->SendMessage(&sendSuitNum, sendSuitNum.dwSize);

		INT nOrgCnt = nCnt + 1;
		if(1 == nOrgCnt)
		{
			m_mapSuitNum.Erase(pEquipProto->dwSuitID[i]);
			continue;
		}

		const tagSuitProto *pSuitProto = g_attRes.GetSuitProto(pEquipProto->dwSuitID[i]);
		if(!P_VALID(pSuitProto))
		{
			ASSERT(P_VALID(pSuitProto));
			continue;
		}

		// 先去除所有相关隐藏属性
		for(INT n=0; n<MAX_SUIT_ATT_NUM; ++n)
		{
			if(nOrgCnt < pSuitProto->n8ActiveNum[n])
			{
				break;
			}

			m_pRole->UnRegisterTriggerSuitSet(pSuitProto->dwTriggerID[n], 
							pSuitProto->dwBuffID[n], pEquipProto->dwSuitID[i]);
		}

		// 重新添加相关隐藏属性
		for(INT n=0; n<MAX_SUIT_ATT_NUM; ++n)
		{
			if(nCnt < pSuitProto->n8ActiveNum[n])
			{
				break;
			}

			m_pRole->RegisterTriggerSuitSet(pSuitProto->dwTriggerID[n], 
							pSuitProto->dwBuffID[n], pEquipProto->dwSuitID[i]);
		}


		// 特效
		if(pSuitProto->n8SpecEffectNum == nOrgCnt)
		{
			m_pRole->SetSuitEffect(GT_INVALID);

			if(P_VALID(m_pRole->GetMap()))
			{
				// 将特效ID发送给客户端
				tagNS_SuitEffect sendSuitEffect;
				sendSuitEffect.dwRoleID			= m_pRole->GetID();
				sendSuitEffect.dwSuitEffectID	= GT_INVALID;

				m_pRole->GetMap()->SendBigVisTileMsg(m_pRole, &sendSuitEffect, sendSuitEffect.dwSize);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
// 初始化发送到客户端的消息内容
//--------------------------------------------------------------------------------------------
VOID Suit::InitSendInitState(BYTE *pData)
{
	MTRANS_POINTER(p, pData, tagSuitInit);

	INT nEquipNum, i=0;
	DWORD dwSuitID;
	TMap<DWORD, INT>::TMapIterator iter = m_mapSuitNum.Begin();
	while(m_mapSuitNum.PeekNext(iter, dwSuitID, nEquipNum))
	{
		p[i].dwSuitID	= dwSuitID;
		p[i].nEquipNum	= nEquipNum;

		++i;
	}
}

//--------------------------------------------------------------------------------------------
// 判断该件套装部件是否需要统计
//--------------------------------------------------------------------------------------------
BOOL Suit::IsNeedCount(const tagEquip *pEquip, const tagEquipProto *pEquipProto, 
				 INT nSuitIndex, INT16 n16IndexOther)
{
	const tagEquip *pEquipOther = m_pRole->GetItemMgr().GetEquipBarEquip(n16IndexOther);
	if(P_VALID(pEquipOther) 
		&& pEquipOther->dwTypeID == pEquip->dwTypeID
		&& pEquipProto->bySuitMinQlty[nSuitIndex] <= pEquipOther->equipSpec.byQuality)
	{
		return FALSE;
	}

	return TRUE;
}