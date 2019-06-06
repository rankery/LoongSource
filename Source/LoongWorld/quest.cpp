//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: quest.h
// actor:
// data: 2008-9-1
// last:
// brief: 任务类
//-------------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/QuestDef.h"
#include "../WorldDefine/quest.h"

#include "role.h"
#include "quest.h"
#include "quest_mgr.h"
#include "script_mgr.h"

//-------------------------------------------------------------------------------
// constructor
//-------------------------------------------------------------------------------
Quest::Quest()
: ScriptData<ESD_Quest>(),m_pProto(NULL), m_pOwner(NULL),m_dwStartTime(GT_INVALID), m_nIndex(GT_INVALID), m_pScript(NULL)
{
	memset(m_n16CreatureCount, 0, sizeof(m_n16CreatureCount));
	memset(m_bSpeakNPC, false, sizeof(m_bSpeakNPC));
	memset(m_bTrigger, false, sizeof(m_bTrigger));
	memset(m_n16ItemCount, 0, sizeof(m_n16ItemCount));
	memset(m_bInvest, false, sizeof(m_bInvest));
}

//-------------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------------
Quest::~Quest()
{
	Destroy();
}

//-------------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------------
VOID Quest::Init(const tagQuestProto* pQuestProto, Role* pRole, INT nIndex)
{
	ASSERT( P_VALID(pQuestProto) && P_VALID(pRole) );
	ASSERT( nIndex >= 0 && nIndex < QUEST_MAX_COUNT );

	m_pProto = pQuestProto;
	m_pOwner = pRole;
	m_nIndex = nIndex;
	m_dwStartTime = g_world.GetWorldTime();
	m_pDynamicTarget = NULL;
	m_bComplete = FALSE;

	// 初始化各种任务状态
	memset(m_n16CreatureCount, 0, sizeof(m_n16CreatureCount));
	memset(m_bSpeakNPC, false, sizeof(m_bSpeakNPC));
	memset(m_bTrigger, false, sizeof(m_bTrigger));
	memset(m_n16ItemCount, 0, sizeof(m_n16ItemCount));
	memset(m_bInvest, false, sizeof(m_bInvest));

	// 初始化脚本
	m_pScript = g_ScriptMgr.GetQuestScript(m_pProto->id);

	// 初始化任务动态目标
	if(EQTM_DYNAMIC == pQuestProto->target_mode)
	{
		m_pDynamicTarget = new tagQuestDynamicTarget;

		// 初始化脚本动态目标
		if(P_VALID(m_pDynamicTarget) && P_VALID(m_pScript))
		{
			m_pScript->OnInit(pQuestProto->id, m_pOwner);
		}
	}

	// 组装玩家身上的物品个数
	InitQuestItem();
}

VOID Quest::Init(const tagQuestSave* pQuestSave, Role* pRole, INT nIndex)
{
	ASSERT( P_VALID(pQuestSave) && P_VALID(pRole) );
	ASSERT( nIndex >= 0 && nIndex < QUEST_MAX_COUNT );

	m_pProto = g_questMgr.GetQuestProto(pQuestSave->u16QuestID);
	if( !P_VALID(m_pProto) ) return;

	m_pOwner = pRole;
	m_nIndex = nIndex;
	m_dwStartTime = pQuestSave->dwAcceptTime;
	m_pDynamicTarget = NULL;
	m_bComplete = FALSE;

	// 初始化各种任务状态
	IFASTCODE->MemCpy(m_n16CreatureCount, pQuestSave->n16MonsterNum, sizeof(m_n16CreatureCount));
	IFASTCODE->MemCpy(m_ScriptData.dwData, pQuestSave->Data.dwData, sizeof(DWORD)*ESD_Quest);
	memset(m_bSpeakNPC, false, sizeof(m_bSpeakNPC));
	memset(m_bTrigger, false, sizeof(m_bTrigger));
	memset(m_n16ItemCount, 0, sizeof(m_n16ItemCount));
	memset(m_bInvest, false, sizeof(m_bInvest));

	// 初始化脚本
	m_pScript = g_ScriptMgr.GetQuestScript(m_pProto->id);

	// 初始化任务动态目标
	if(EQTM_DYNAMIC == m_pProto->target_mode)
	{
		m_pDynamicTarget = new tagQuestDynamicTarget;

		if(!P_VALID(m_pDynamicTarget))
			return;

		m_pDynamicTarget->eTargetType = pQuestSave->DynamicTarget.eTargetType;
		m_pDynamicTarget->dwQuestTipsID = pQuestSave->DynamicTarget.dwQuestTipsID;
		IFASTCODE->MemCpy(m_pDynamicTarget->dwTargetID, pQuestSave->DynamicTarget.dwTargetID, sizeof(DWORD)*DYNAMIC_TARGET_COUNT);
		IFASTCODE->MemCpy(m_pDynamicTarget->nTargetNum, pQuestSave->DynamicTarget.nTargetNum, sizeof(INT)*DYNAMIC_TARGET_COUNT);
	}

	// 组装玩家身上的物品个数
	InitQuestItem();
}

//--------------------------------------------------------------------------------
// 销毁
//--------------------------------------------------------------------------------
VOID Quest::Destroy()
{
	m_pOwner = NULL;
	m_pProto = NULL;
	m_dwStartTime = GT_INVALID;
	m_nIndex = GT_INVALID;

	m_pScript = NULL;
}

//--------------------------------------------------------------------------------
// 保存
//--------------------------------------------------------------------------------
VOID Quest::InitQuestSave(OUT tagQuestSave* pQuestSave)
{
	pQuestSave->u16QuestID = GetID();
	pQuestSave->dwAcceptTime = GetAcceptTime();
	IFASTCODE->MemCpy(pQuestSave->n16MonsterNum, m_n16CreatureCount, sizeof(pQuestSave->n16MonsterNum));
	IFASTCODE->MemCpy(pQuestSave->Data.dwData, m_ScriptData.dwData, sizeof(DWORD)*ESD_Quest);

	if(P_VALID(m_pDynamicTarget))
	{
		pQuestSave->DynamicTarget.eTargetType = m_pDynamicTarget->eTargetType;
		pQuestSave->DynamicTarget.dwQuestTipsID = m_pDynamicTarget->dwQuestTipsID;
		IFASTCODE->MemCpy(pQuestSave->DynamicTarget.dwTargetID, m_pDynamicTarget->dwTargetID, sizeof(DWORD)*DYNAMIC_TARGET_COUNT);
		IFASTCODE->MemCpy(pQuestSave->DynamicTarget.nTargetNum, m_pDynamicTarget->nTargetNum, sizeof(INT)*DYNAMIC_TARGET_COUNT);		
	}
}

//--------------------------------------------------------------------------------
// 生成消息结构
//--------------------------------------------------------------------------------
VOID Quest::GenerateMsgInfo(tagIncompleteQuestMsgInfo* pInfo, DWORD &dwMsgSize, DWORD &dwQuestSize)
{
	ASSERT( IsValid() );
	dwQuestSize = sizeof(tagIncompleteQuestMsgInfo);

	pInfo->u16QuestID = m_pProto->id;
	pInfo->dwAcceptTime = m_dwStartTime;
	IFASTCODE->MemCpy(pInfo->n16CreatureNum, m_n16CreatureCount, sizeof(pInfo->n16CreatureNum));
	IFASTCODE->MemCpy(pInfo->n16ItemNum, m_n16ItemCount, sizeof(pInfo->n16ItemNum));

	if(EQTM_DYNAMIC == m_pProto->target_mode)
	{
		pInfo->DynamicTarget.eTargetType = m_pDynamicTarget->eTargetType;
		pInfo->DynamicTarget.dwQuestTipsID = m_pDynamicTarget->dwQuestTipsID;
		IFASTCODE->MemCpy(pInfo->DynamicTarget.dwTargetID, m_pDynamicTarget->dwTargetID, sizeof(DWORD)*DYNAMIC_TARGET_COUNT);
		IFASTCODE->MemCpy(pInfo->DynamicTarget.nTargetNum, m_pDynamicTarget->nTargetNum, sizeof(INT)*DYNAMIC_TARGET_COUNT);
	}
	else
	{
		// 修正消息大小
		dwQuestSize-= sizeof(tagQuestDynamicTarget); 
		dwMsgSize -= sizeof(tagQuestDynamicTarget);
	}
}

//---------------------------------------------------------------------------------
// 得到初始物品列表
//---------------------------------------------------------------------------------
VOID Quest::GetInitQuestItem(INT16* const n16ItemCount)
{
	ASSERT( IsValid() );

	IFASTCODE->MemCpy(n16ItemCount, m_n16ItemCount, sizeof(m_n16ItemCount));
}

//---------------------------------------------------------------------------------
// 得到动态目标
//---------------------------------------------------------------------------------
VOID Quest::GetInitDynamicTarget(tagQuestDynamicTarget* pInfo, DWORD &dwSize)
{
	if(EQTM_DYNAMIC == m_pProto->target_mode)
	{
		pInfo->eTargetType = m_pDynamicTarget->eTargetType;
		pInfo->dwQuestTipsID = m_pDynamicTarget->dwQuestTipsID;
		IFASTCODE->MemCpy(pInfo->dwTargetID, m_pDynamicTarget->dwTargetID, sizeof(DWORD)*DYNAMIC_TARGET_COUNT);
		IFASTCODE->MemCpy(pInfo->nTargetNum, m_pDynamicTarget->nTargetNum, sizeof(INT)*DYNAMIC_TARGET_COUNT);
	}
	else
	{
		dwSize -= sizeof(tagQuestDynamicTarget);
	}
}

//---------------------------------------------------------------------------------
// 所需要的任务物品是否已经足够
//---------------------------------------------------------------------------------
BOOL Quest::IsQuestItemFull(DWORD dwItemTypeID)
{
	if( FALSE == IsValid() ) return FALSE;

	// 如果该任务没有获得物品，则直接返回
	if( !(m_pProto->quest_flags & Quest_Flag_ITEM) ) return FALSE;

	// 检测任务物品是否已满
	for(INT n = 0; n < QUEST_ITEMS_COUNT; ++n)
	{
		if( !P_VALID(m_pProto->complete_req_item[n]) ) continue;
			//break;

		if( m_pProto->complete_req_item[n] != dwItemTypeID )
			continue;

		return m_n16ItemCount[n] >= m_pProto->complete_req_item_num[n];
	}

	return FALSE;
}

//---------------------------------------------------------------------------------
// 任务现在是否可以完成
//---------------------------------------------------------------------------------
BOOL Quest::IsCanComplete(INT nChoiceItemIndex, Creature* pNPC)
{
	BOOL bComplete = FALSE;
	if( m_pProto->only_one_condition )
	{
		bComplete = IsCanCompleteCreatureKill(TRUE)	||
					IsCanCompleteItem(TRUE)			||
					IsCanCompleteNPCTalk(TRUE)		||
					IsCanCompleteTrigger(TRUE)		||
					IsCanCompleteMoney(TRUE)		||
					IsCanCompleteLevel(TRUE)		||
//					IsCanCompleteInvest(TRUE)		||
					IsCanCompleteMap(TRUE);
	}
	else
	{
		bComplete = IsCanCompleteCreatureKill()	&&
					IsCanCompleteItem()			&&
					IsCanCompleteNPCTalk()		&&
					IsCanCompleteTrigger()		&&
					IsCanCompleteMoney()		&&
					IsCanCompleteLevel()		&&
//					IsCanCompleteInvest()		&&
					IsCanCompleteMap();
	}

	if( P_VALID(m_pScript) )
	{
		bComplete = bComplete && m_pScript->CheckComplete(m_pProto->id, m_pOwner, pNPC);
	}

	return bComplete && IsCanCompleteReward(nChoiceItemIndex);
}

//----------------------------------------------------------------------------------
// 事件触发
//----------------------------------------------------------------------------------
VOID Quest::OnEvent(EQuestEvent eQuestType, DWORD dwEventMisc1/*=0*/, DWORD dwEventMisc2/*=0*/, DWORD dwEventMisc3/*=0*/)
{
	switch(eQuestType)
	{
	case EQE_Kill:
		OnEventCreatureKill(dwEventMisc1, dwEventMisc2);
		break;

	case EQE_Item:
		OnEventItem(dwEventMisc1, dwEventMisc2, dwEventMisc3);
		break;

	case EQE_Talk:
		OnEventNPCTalk(dwEventMisc1, dwEventMisc2);
		break;

	case EQE_Trigger:
		OnEventTrigger(dwEventMisc1);
		break;

	case EQE_Invest:
		OnEventInvest(dwEventMisc1,dwEventMisc2);
		break;

	case EQE_DlgDefault:
		OnEventDlgDefault(dwEventMisc1);
		break;

	default:
		break;
	}
}

//------------------------------------------------------------------------------------
// 杀怪事件
//------------------------------------------------------------------------------------
VOID Quest::OnEventCreatureKill(DWORD dwCreatureTypeID, INT nCreatureLevel)
{
	if( !P_VALID(m_pProto) ) return;

	// 调用脚本
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnCreatureKill(m_pProto->id, m_pOwner, dwCreatureTypeID);
	}

	// 动态击杀目标计数
	if(P_VALID(m_pDynamicTarget) && EQTT_Kill == m_pDynamicTarget->eTargetType)
	{
		for(int i = 0; i < DYNAMIC_TARGET_COUNT; ++i)
		{
			BOOL bFind = FALSE;
			if(m_pDynamicTarget->dwTargetID[i] == dwCreatureTypeID	  ||
				m_pDynamicTarget->dwTargetID[i+1] == dwCreatureTypeID ||
				m_pDynamicTarget->dwTargetID[i+2] == dwCreatureTypeID ||
				m_pDynamicTarget->dwTargetID[i+3] == dwCreatureTypeID)
			{
				m_n16CreatureCount[i] += 1;
				bFind = TRUE;
			}

			if( bFind )
			{
				// 发送更新
				tagNS_QuestUpdateKillCreature send;
				send.u16QuestID = GetID();
				send.nCreatureIndex = i;
				send.n16NewKillNum = m_n16CreatureCount[i];
				m_pOwner->SendMessage(&send, send.dwSize);

				// 找到了，就不需要往下找了
				break;
			}
		}
			
		return;
	}

	// 如果该任务没有杀怪，则直接返回
	if( !(m_pProto->quest_flags & Quest_Flag_KILL) ) return;
	
	for(INT n = 0; n < QUEST_CREATURES_COUNT; n++)
	{
		if( !P_VALID(m_pProto->complete_req_creature[n*3]) )
			break;

		BOOL bFind = FALSE;
		if( m_pProto->creature_level )
		{
			if( m_pProto->complete_req_creature[n*3]	== nCreatureLevel ||
				m_pProto->complete_req_creature[n*3+1]	== nCreatureLevel ||
				m_pProto->complete_req_creature[n*3+2]	== nCreatureLevel
			)
			{
				if( m_n16CreatureCount[n] < m_pProto->complete_req_creature_num[n] )
				{
					m_n16CreatureCount[n] += 1;
					bFind = TRUE;
				}
			}
		}
		else
		{
			if( m_pProto->complete_req_creature[n*3]	== dwCreatureTypeID ||
				m_pProto->complete_req_creature[n*3+1]	== dwCreatureTypeID ||
				m_pProto->complete_req_creature[n*3+2]	== dwCreatureTypeID
				)
			{
				if( m_n16CreatureCount[n] < m_pProto->complete_req_creature_num[n] )
				{
					m_n16CreatureCount[n] += 1;
					bFind = TRUE;
				}
			}
		}

		if( bFind )
		{
			// 发送更新
			tagNS_QuestUpdateKillCreature send;
			send.u16QuestID = GetID();
			send.nCreatureIndex = n;
			send.n16NewKillNum = m_n16CreatureCount[n];
			m_pOwner->SendMessage(&send, send.dwSize);

			// 找到了，就不需要往下找了
			break;
		}
		
	}
}

//-------------------------------------------------------------------------------
// 物品事件
//-------------------------------------------------------------------------------
VOID Quest::OnEventItem(DWORD dwItemTypeID, INT nNum, BOOL bAdd)
{
	if( !P_VALID(m_pProto) ) return;
	if( nNum <= 0 ) return;
	if( m_bComplete ) return;

	// 动态收集物品计数
	if(P_VALID(m_pDynamicTarget) && EQTT_Collect == m_pDynamicTarget->eTargetType)
	{
		for(INT i = 0; i < DYNAMIC_TARGET_COUNT; ++i)
		{
			BOOL bFind = FALSE;
			if(m_pDynamicTarget->dwTargetID[i] == dwItemTypeID)
			{
				if( bAdd) m_n16ItemCount[i] += nNum;
				else	  m_n16ItemCount[i] -= nNum;

				bFind = TRUE;
			}

			if( bFind )
			{
				// 发送更新
				tagNS_QuestUpdateItem send;
				send.u16QuestID = GetID();
				send.nItemIndex = i;
				send.n16NewItemNum = m_n16ItemCount[i];
				m_pOwner->SendMessage(&send, send.dwSize);

				// 找到了，就不需要往下找了
				break;
			}
		}

		return;
	}

	// 如果该任务没有获得物品，则直接返回
	if( !(m_pProto->quest_flags & Quest_Flag_ITEM) ) return;

	// 依次找对应的物品
	for(INT n = 0; n < QUEST_ITEMS_COUNT; ++n)
	{
		if( !P_VALID(m_pProto->complete_req_item[n]) ) continue;
			//break;

		BOOL bFind = FALSE;

		if( m_pProto->complete_req_item[n] == dwItemTypeID )
		{
			if( bAdd )	m_n16ItemCount[n] += nNum;
			else		m_n16ItemCount[n] -= nNum;

			bFind = TRUE;
		}

		if( bFind )
		{
			// 发送更新
			tagNS_QuestUpdateItem send;
			send.u16QuestID = GetID();
			send.nItemIndex = n;
			send.n16NewItemNum = m_n16ItemCount[n];
			m_pOwner->SendMessage(&send, send.dwSize);

			// 找到了，就不需要往下找了
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// NPC对话事件
//-----------------------------------------------------------------------------
VOID Quest::OnEventNPCTalk(DWORD dwNPCID, DWORD dwNPCTypeID)
{
	if( !P_VALID(m_pProto) ) return;

	// 动态NPC对话统计
	if(P_VALID(m_pDynamicTarget) && EQTT_NPCTalk == m_pDynamicTarget->eTargetType)
	{
		for(INT i = 0; i < DYNAMIC_TARGET_COUNT; ++i)
		{
			BOOL bFind = FALSE;

			if( m_pDynamicTarget->dwTargetID[i] == dwNPCTypeID )
			{
				if( !m_bSpeakNPC[i] )
				{
					m_bSpeakNPC[i] = true;
					bFind = TRUE;
				}
			}
			
			if( bFind )
			{
				// 发送更新
				tagNS_QuestUpdateNPCTalk send;
				send.u16QuestID = GetID();
				send.nIndex = i;
				send.dwErrorCode = E_Success;
				m_pOwner->SendMessage(&send, send.dwSize);

				// 调用NPC对话脚本
				if( P_VALID(m_pScript) )
				{
					m_pScript->OnNPCTalk(m_pProto->id, m_pOwner, dwNPCID, dwNPCTypeID);
				}

				// 找到了，就不需要往下找了
				break;
			}
		}

		return;
	}

	// 如果该任务没有获得物品，则直接返回
	if( !(m_pProto->quest_flags & Quest_Flag_NPC_TALK) ) return;

	// 依次找对应的物品
	for(INT n = 0; n < QUEST_NPC_COUNT; ++n)
	{
		if( !P_VALID(m_pProto->complete_req_npc[n]) )
			break;

		BOOL bFind = FALSE;

		if( m_pProto->complete_req_npc[n] == dwNPCTypeID )
		{
			if( !m_bSpeakNPC[n] )
			{
				m_bSpeakNPC[n] = true;
				bFind = TRUE;
			}
		}
		else
		{
			// 跟NPC的ID不一样，如果该NPC之前没有对过话，则不再向后检查了
			if( m_pProto->only_in_order )
			{
				if( false == m_bSpeakNPC[n] )
				{
					tagNS_QuestUpdateNPCTalk send;
					send.u16QuestID = GetID();
					send.nIndex = n;
					send.dwErrorCode = E_CanUpdateQuest_Fail;
					m_pOwner->SendMessage(&send, send.dwSize);

					break;
				}
			}
		}

		if( bFind )
		{
			// 发送更新
			tagNS_QuestUpdateNPCTalk send;
			send.u16QuestID = GetID();
			send.nIndex = n;
			send.dwErrorCode = E_Success;
			m_pOwner->SendMessage(&send, send.dwSize);

			// 调用NPC对话脚本
			if( P_VALID(m_pScript) )
			{
				m_pScript->OnNPCTalk(m_pProto->id, m_pOwner, dwNPCID, dwNPCTypeID);
			}


			// 找到了，就不需要往下找了
			break;
		}
	}

	// 没有找到，也返回客户端
	tagNS_QuestUpdateNPCTalk send;
	send.u16QuestID = GetID();
	send.nIndex = GT_INVALID;
	send.dwErrorCode = E_CanUpdateQuest_Fail;
	m_pOwner->SendMessage(&send, send.dwSize);
}

//--------------------------------------------------------------------------------
// 触发器事件
//--------------------------------------------------------------------------------
VOID Quest::OnEventTrigger(DWORD dwTriggerSerial)
{
	if( !P_VALID(m_pProto) ) return;

	// 如果该任务没有获得物品，则直接返回
	if( !(m_pProto->quest_flags & Quest_Flag_Trigger) ) return;

	// 依次找对应的物品
	for(INT n = 0; n < QUEST_TRIGGERS_COUNT; ++n)
	{
		if( !P_VALID(m_pProto->complete_req_trigger[n]) )
			break;

		BOOL bFind = FALSE;

		if( m_pProto->complete_req_trigger[n] == dwTriggerSerial )
		{
			m_bTrigger[n] = true;
			bFind = TRUE;
		}

		if( bFind )
		{
			// 发送更新
			tagNS_QuestUpdateTrigger send;
			send.u16QuestID = GetID();
			send.nIndex = n;
			m_pOwner->SendMessage(&send, send.dwSize);

			// 找到了，就不需要往下找了
			break;
		}
	}
}

//--------------------------------------------------------------------------------
// 调查地物事件
//--------------------------------------------------------------------------------
VOID Quest::OnEventInvest(DWORD dwNPCID, DWORD dwNPCTypeID)
{
	if( !P_VALID(m_pProto) ) return;

	// 动态NPC对话统计
	if(P_VALID(m_pDynamicTarget) && EQTT_Invest == m_pDynamicTarget->eTargetType)
	{
		for(INT i = 0; i < DYNAMIC_TARGET_COUNT; ++i)
		{
			BOOL bFind = FALSE;

			if( m_pDynamicTarget->dwTargetID[i] == dwNPCTypeID )
			{
				if( !m_bInvest[i] )
				{
					m_bInvest[i] = true;
					bFind = TRUE;
				}
			}

			if( bFind )
			{
				// 发送更新
				tagNS_QuestUpdateInveset send;
				send.u16QuestID = GetID();
				send.nIndex = i;
				m_pOwner->SendMessage(&send, send.dwSize);

				if(P_VALID(m_pScript))
				{
					m_pScript->OnInvest(m_pProto->id, m_pOwner, dwNPCTypeID);
				}

				// 找到了，就不需要往下找了
				break;
			}
		}

		return;
	}

	// 如果该任务没有获得物品，则直接返回
	if( !(m_pProto->quest_flags & Quest_Flag_Invest) ) return;

	// 依次找对应的物品
	for(INT n = 0; n < DYNAMIC_TARGET_COUNT; ++n)
	{
		if( !P_VALID(m_pProto->investigate_objects[n]) && !P_VALID(m_pProto->investigate_objects[n+4]) )
			break;

		BOOL bFind = FALSE;

		if( m_pProto->investigate_objects[n] == dwNPCTypeID || m_pProto->investigate_objects[n+4] == dwNPCTypeID)
		{
			m_bInvest[n] = true;
			bFind = TRUE;
		}

		if( bFind )
		{
			// 发送更新
			tagNS_QuestUpdateInveset send;
			send.u16QuestID = GetID();
			send.nIndex = n;
			m_pOwner->SendMessage(&send, send.dwSize);

			if(P_VALID(m_pScript))
			{
				m_pScript->OnInvest(m_pProto->id, m_pOwner, dwNPCTypeID);
			}

			// 找到了，就不需要往下找了
			break;
		}
	}
}

//--------------------------------------------------------------------------------
// 服务器可控对话框缺省事件
//--------------------------------------------------------------------------------
VOID Quest::OnEventDlgDefault(DWORD dwDlgOption)
{
	if(!P_VALID(m_pScript))		return;

	m_pScript->OnDlgDefault(m_pProto->id, m_pOwner, dwDlgOption);
}

//--------------------------------------------------------------------------------
// 初始化任务所需要的物品
//--------------------------------------------------------------------------------
VOID Quest::InitQuestItem()
{
	if( !P_VALID(m_pProto) ) return;

	// 判断动态收集的物品
	if (P_VALID(m_pDynamicTarget) && EQTT_Collect == m_pDynamicTarget->eTargetType)
	{
		for(int i = 0; i < DYNAMIC_TARGET_COUNT; ++i)
		{
			if(m_pDynamicTarget->nTargetNum[i] == 0)
				break;

			m_n16ItemCount[i]  = m_pOwner->GetItemMgr().GetBagSameItemCount(m_pDynamicTarget->dwTargetID[i])
				+ m_pOwner->GetItemMgr().GetQuestBagSameItemCount(m_pDynamicTarget->dwTargetID[i]);
		}

		return;
	}

	if( !(m_pProto->quest_flags & Quest_Flag_ITEM) )
		return;

	for(INT n = 0; n < QUEST_ITEMS_COUNT; ++n)
	{
		if( !P_VALID(m_pProto->complete_req_item[n]) )
			break;

		m_n16ItemCount[n] = m_pOwner->GetItemMgr().GetBagSameItemCount(m_pProto->complete_req_item[n])
						+ m_pOwner->GetItemMgr().GetQuestBagSameItemCount(m_pProto->complete_req_item[n]);
	}
}

//----------------------------------------------------------------------------------
// 杀怪是否满足
//----------------------------------------------------------------------------------
BOOL Quest::IsCanCompleteCreatureKill(BOOL bOnlyOne)
{
	// 判断动态击杀目标
	if (P_VALID(m_pDynamicTarget) && EQTT_Kill == m_pDynamicTarget->eTargetType)
	{
		for(int i = 0; i < DYNAMIC_TARGET_COUNT; ++i)
		{
			if(m_pDynamicTarget->nTargetNum[i] == 0)
				continue;

			if(m_n16CreatureCount[i] < m_pDynamicTarget->nTargetNum[i])
				return FALSE;
		}

		return TRUE;
	}

	if( !(m_pProto->quest_flags & Quest_Flag_KILL) )
		return (bOnlyOne ? FALSE : TRUE);

	// 满足一个就可以
	if( m_pProto->only_one_creature )
	{
		for(INT n = 0; n < QUEST_CREATURES_COUNT; n++)
		{
			if( !P_VALID(m_pProto->complete_req_creature[n*3]) )
				break;

			if( m_n16CreatureCount[n] >= m_pProto->complete_req_creature_num[n] )
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	// 全部
	else
	{
		for(INT n = 0; n < QUEST_CREATURES_COUNT; n++)
		{
			if( !P_VALID(m_pProto->complete_req_creature[n*3]) )
				break;

			if( m_n16CreatureCount[n] < m_pProto->complete_req_creature_num[n] )
			{
				return FALSE;
			}	
		}
		return TRUE;
	}
}

//----------------------------------------------------------------------------
// 物品是否满足
//----------------------------------------------------------------------------
BOOL Quest::IsCanCompleteItem(BOOL bOnlyOne)
{
	// 判断动态收集的物品
	if (P_VALID(m_pDynamicTarget) && EQTT_Collect == m_pDynamicTarget->eTargetType)
	{
		for(int i = 0; i < DYNAMIC_TARGET_COUNT; ++i)
		{
			if(m_pDynamicTarget->nTargetNum[i] == 0)
				continue;

			if(m_n16ItemCount[i] < m_pDynamicTarget->nTargetNum[i])
				return FALSE;
		}

		return TRUE;
	}

	if( !(m_pProto->quest_flags & Quest_Flag_ITEM) )
		return (bOnlyOne ? FALSE : TRUE);

	// 满足一个就可以
	if( m_pProto->only_one_item )
	{
		for(INT n = 0; n < QUEST_ITEMS_COUNT; n++)
		{
			if( !P_VALID(m_pProto->complete_req_item[n]) ) continue;
				//break;

			if( m_n16ItemCount[n] >= m_pProto->complete_req_item_num[n] )
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	// 全部
	else
	{
		for(INT n = 0; n < QUEST_ITEMS_COUNT; n++)
		{
			if( !P_VALID(m_pProto->complete_req_item[n]) ) continue;
				//break;

			if( m_n16ItemCount[n] < m_pProto->complete_req_item_num[n] )
			{
				return FALSE;
			}	
		}
		return TRUE;
	}
}

//----------------------------------------------------------------------------
// NPC是否满足
//----------------------------------------------------------------------------
BOOL Quest::IsCanCompleteNPCTalk(BOOL bOnlyOne)
{
	if (P_VALID(m_pDynamicTarget) && EQTT_NPCTalk == m_pDynamicTarget->eTargetType)
	{
		for(int i = 0; i < DYNAMIC_TARGET_COUNT; ++i)
		{
			if(m_pDynamicTarget->dwTargetID[i] == 0)
				continue;

			if( !m_bSpeakNPC[i])
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	if( !(m_pProto->quest_flags & Quest_Flag_NPC_TALK) )
		return (bOnlyOne ? FALSE : TRUE);

	// 满足一个就可以
	if( m_pProto->only_one_npc )
	{
		for(INT n = 0; n < QUEST_NPC_COUNT; n++)
		{
			if( !P_VALID(m_pProto->complete_req_npc[n]) )
				break;

			if( m_bSpeakNPC[n] )
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	// 全部
	else
	{
		for(INT n = 0; n < QUEST_NPC_COUNT; n++)
		{
			if( !P_VALID(m_pProto->complete_req_npc[n]) )
				break;

			if( !m_bSpeakNPC[n] )
			{
				return FALSE;
			}	
		}
		return TRUE;
	}
}

//----------------------------------------------------------------------------
// 触发器是否满足
//----------------------------------------------------------------------------
BOOL Quest::IsCanCompleteTrigger(BOOL bOnlyOne)
{
	if( !(m_pProto->quest_flags & Quest_Flag_Trigger) )
		return (bOnlyOne ? FALSE : TRUE);

	// 满足一个就可以
	if( m_pProto->only_one_trigger )
	{
		for(INT n = 0; n < QUEST_TRIGGERS_COUNT; n++)
		{
			if( !P_VALID(m_pProto->complete_req_trigger[n]) )
				break;

			if( m_bTrigger[n] )
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	// 全部
	else
	{
		for(INT n = 0; n < QUEST_TRIGGERS_COUNT; n++)
		{
			if( !P_VALID(m_pProto->complete_req_trigger[n]) )
				break;

			if( !m_bTrigger[n] )
			{
				return FALSE;
			}	
		}
		return TRUE;
	}
}

//----------------------------------------------------------------------------
// 调查地物是否满足
//----------------------------------------------------------------------------
BOOL Quest::IsCanCompleteInvest(BOOL bOnlyOne)
{
	if (P_VALID(m_pDynamicTarget) && EQTT_Invest == m_pDynamicTarget->eTargetType)
	{
		for(int i = 0; i < DYNAMIC_TARGET_COUNT; ++i)
		{
			if(m_pDynamicTarget->dwTargetID[i] == GT_INVALID)
				continue;

			if( !m_bInvest[i])
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	if( !(m_pProto->quest_flags & Quest_Flag_Invest) )
		return (bOnlyOne ? FALSE : TRUE);

	for(INT n = 0; n < DYNAMIC_TARGET_COUNT; n++)
	{
		if( !P_VALID(m_pProto->investigate_objects[n]) && !P_VALID(m_pProto->investigate_objects[n+4]) )
			break;

		if( !m_bInvest[n] )
		{
			return FALSE;
		}	
	}
	return TRUE;
}

//----------------------------------------------------------------------------
// 金钱
//----------------------------------------------------------------------------
BOOL Quest::IsCanCompleteMoney(BOOL bOnlyOne)
{
	if( 0 == m_pProto->complete_req_money )
		return (bOnlyOne ? FALSE : TRUE);

	if( m_pOwner->GetCurMgr().GetBagSilver() < m_pProto->complete_req_money )
		return FALSE;

	return TRUE;
}

//----------------------------------------------------------------------------
// 等级
//----------------------------------------------------------------------------
BOOL Quest::IsCanCompleteLevel(BOOL bOnlyOne)
{
	if( 0 == m_pProto->complete_req_level )
		return (bOnlyOne ? FALSE : TRUE);

	if( m_pOwner->GetLevel() < m_pProto->complete_req_level )
		return FALSE;

	return TRUE;
}

//----------------------------------------------------------------------------
// 地图
//----------------------------------------------------------------------------
BOOL Quest::IsCanCompleteMap(BOOL bOnlyOne)
{
	// todo:
	return TRUE;
}

//----------------------------------------------------------------------------
// 奖励
//----------------------------------------------------------------------------
BOOL Quest::IsCanCompleteReward(INT nChoiceItemIndex)
{
	INT16 n16BagSpace = 0;
	INT16 n16QuestSpace = 0;
	INT16 n16TmpBagSpace = 0;
	INT16 n16TmpQuestSpace = 0;
	INT16 n16TmpMaxDummy;
	
	// 查看奖励的物品背包能否放得下
	for(INT n = 0; n < QUEST_REW_ITEM; ++n)
	{
		if( !P_VALID(m_pProto->rew_item[n]) )
			break;

		// 得到这个物品会在背包里占用的个数
		if(!ItemMgr::CalSpaceUsed(m_pProto->rew_item[n], 
			m_pProto->rew_item_num[n], n16TmpBagSpace, n16TmpQuestSpace, n16TmpMaxDummy))
		{
			return FALSE;
		}

		n16BagSpace		+= n16TmpBagSpace;
		n16QuestSpace	+= n16TmpQuestSpace;
	}

	// 如果有可选奖励，则查看奖励物品中的最大个数
	INT nTmp = nChoiceItemIndex;
	if(nTmp >= 0 && nTmp < QUEST_REW_ITEM
		&& P_VALID(m_pProto->rew_choice_item[nTmp]))
	{
		// 得到这个物品会在背包里占用的个数
		if(!ItemMgr::CalSpaceUsed(m_pProto->rew_choice_item[nTmp], 
			m_pProto->rew_choice_item_num[nTmp], n16TmpBagSpace, n16TmpQuestSpace, n16TmpMaxDummy))
		{
			return FALSE;
		}

		n16BagSpace		+= n16TmpBagSpace;
		n16QuestSpace	+= n16TmpQuestSpace;
	}

	// 得到背包里空闲的栏位能否加上这些物品
	if( m_pOwner->GetItemMgr().GetBagFreeSize() < n16BagSpace
		|| m_pOwner->GetItemMgr().GetQuestItemBagFreeSize() < n16QuestSpace)
	{
		return FALSE;
	}

	return TRUE;
}

//---------------------------------------------------------------------------------
// 任务接取时
//---------------------------------------------------------------------------------
VOID Quest::OnAccept(Creature* pNPC)
{
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnAccept(m_pProto->id, m_pOwner, pNPC);
	}
}

//----------------------------------------------------------------------------------
// 任务完成时
//----------------------------------------------------------------------------------
VOID Quest::OnComplete(Creature* pNPC)
{
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnComplete(m_pProto->id, m_pOwner, pNPC);
	}
}

//-----------------------------------------------------------------------------------
// 任务取消时
//-----------------------------------------------------------------------------------
VOID Quest::OnCancel()
{
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnCancel(m_pProto->id, m_pOwner);
	}
}

