/********************************************************************
	created:	2008/08/26
	created:	26:8:2008   15:17
	filename: 	d:\loong_client\Client\QuestQuery.cpp
	file path:	d:\loong_client\Client
	file base:	QuestQuery
	file ext:	cpp
	author:		leili
	
	purpose:	进行任务查询，可交接任务的NPC列表，已完成任务列表，玩家当前任务列表
*********************************************************************/
#include "StdAfx.h"
#include "QuestQuery.h"
#include "QuestMgr.h"
#include "MapMgr.h"
#include "CreatureData.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "CurrencyMgr.h"
#include "EffectMgr.h"
#include "Container.h"
#include "ItemMgr.h"
#include "SkillMgr.h"
#include "ReputeMgr.h"

namespace
{
	class TimeSort
	{
	public:
		bool operator()(const QuestQuery::tagActiveQuest& quest1,const QuestQuery::tagActiveQuest& quest2)
		{
			return quest1.week <= quest2.week;
		}
	};

	class QuestSort
	{
	public:
		bool operator()(const QuestQuery::tagActiveQuest& quest1,const QuestQuery::tagActiveQuest& quest2)
		{
			return quest1.questID <= quest2.questID;
		}
	};

	class LevelSort
	{
	public:
		bool operator()(const QuestQuery::tagActiveQuest& quest1,const QuestQuery::tagActiveQuest& quest2)
		{
			if( quest1.minLevel < quest2.minLevel )
				return true;
			if( quest1.minLevel > quest2.minLevel )
				return false;

			return quest1.maxLevel <= quest2.maxLevel;
		}
	};

	class AcceptNPCSort
	{
	public:
		bool operator()(const QuestQuery::tagActiveQuest& quest1,const QuestQuery::tagActiveQuest& quest2)
		{
			return quest1.acceptNPCID <= quest2.acceptNPCID;
		}
	};

	class CompleteSort
	{
	public:
		bool operator()(const QuestQuery::tagActiveQuest& quest1,const QuestQuery::tagActiveQuest& quest2)
		{
			if( quest1.all < quest2.all )
				return true;
			if( quest1.all > quest2.all )
				return false;

			return quest1.complete <= quest2.complete;
		}
	};
}

QuestQuery::QuestQuery( void )
{

}

QuestQuery::~QuestQuery( void )
{

}

//-----------------------------------------------------------------------------
// 建立当前地图上可接任务NPC列表
//-----------------------------------------------------------------------------
VOID QuestQuery::BuildAcceptQuestNPCs( void )
{
	QuestData *pQD = QuestMgr::Inst()->GetData();
	ASSERT( P_VALID(pQD) && _T("QuestQuery:获得QuestData错误") );

	// 静态
	m_acceptQuestNPCs.clear();

	MapNPCMap::const_iterator mapNPCi;
	for( mapNPCi = m_mapNPCMap.begin(); mapNPCi != m_mapNPCMap.end(); ++mapNPCi )
	{
		const tagMapNPCInfo& npcInfo = mapNPCi->second;

		// 查看是否有可接任务
		vector<UINT16> quests;
		pQD->GetNPCAcceptQuests( npcInfo.dwTypeID, quests );
		for( size_t questi = 0; questi < quests.size(); questi++ )
		{
			if( QuestCanAccept( quests[questi] ) )
			{
				m_acceptQuestNPCs.insert( npcInfo.dwTypeID );
				break;
			}
		}
	}

	// 动态
	m_dynAcceptQuestNPCs.clear();

	NPCSet::const_iterator dynNPCi;
	for( dynNPCi = m_dynamicNPCs.begin(); dynNPCi != m_dynamicNPCs.end(); ++dynNPCi )
	{
		// 查看是否有可接任务
		vector<UINT16> quests;
		pQD->GetNPCAcceptQuests( *dynNPCi, quests );
		for( size_t questi = 0; questi < quests.size(); questi++ )
		{
			if( QuestCanAccept( quests[questi] ) )
			{
				m_dynAcceptQuestNPCs.insert( *dynNPCi );
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// 建立当前地图上可交任务NPC列表
//-----------------------------------------------------------------------------
VOID QuestQuery::BuildCompleteQuestNPCs( void )
{
	m_completeQuestNPCs.clear();

	QuestsMap::const_iterator iter;
	for( iter = m_currentQuests.begin(); iter != m_currentQuests.end(); ++iter )
	{
		UINT16 questID = iter->first;

		if( QuestCanComplete(questID) )//任务可以完成
		{
			QuestData *pData = QuestMgr::Inst()->GetData();
			const tagQuestProto *pProto = pData->GetQuestProto(questID);

			for( int i=0; i<QUEST_COMPLETE_NPC; i++ )
			{
				if( pProto->complete_quest_npc[i] == 0 )
					break;

				m_completeQuestNPCs.insert(pProto->complete_quest_npc[i]);
			}
		}
	}

	NPCSet::const_iterator showi;
	for( showi = m_completeQuestNPCs.begin(); showi != m_completeQuestNPCs.end(); ++showi )
	{
		DWORD npcID = *showi;
		IMSG( _T("complete npc id: %d\r\n"), npcID );
	}
}

//-----------------------------------------------------------------------------
// 建立当前地图上可更新任务NPC列表
//-----------------------------------------------------------------------------
VOID QuestQuery::BuildUpdateQuestNPCs( void )
{
	m_updateQuestNPCs.clear();

	QuestData *pData = QuestMgr::Inst()->GetData();

	QuestsMap::const_iterator iter;
	for( iter = m_currentQuests.begin(); iter != m_currentQuests.end(); ++iter )
	{
		UINT16 questID = iter->first;

		const tagQuestDynamicTarget *pDynamicTarget = pData->GetQuestDynamicTarget(questID);
		if( P_VALID(pDynamicTarget) )//动态
		{
			if( pDynamicTarget->eTargetType == EQTT_NPCTalk )
			{
				for( INT i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pDynamicTarget->dwTargetID[i] == 0 )
						break;

					if( NPCCanUpdateQuest( pDynamicTarget->dwTargetID[i], questID ) )
						m_updateQuestNPCs.insert( pDynamicTarget->dwTargetID[i] );
				}
			}
		}
		else//把任务静态数据中的对话NPC加入列表
		{
			QuestData *pData = QuestMgr::Inst()->GetData();
			const tagQuestProto *pProto = pData->GetQuestProto(questID);
			for( INT i=0; i<QUEST_NPC_COUNT; i++ )
			{
				if( pProto->complete_req_npc[i] == 0 )
					break;

				if( NPCCanUpdateQuest( pProto->complete_req_npc[i], questID ) )
					m_updateQuestNPCs.insert( pProto->complete_req_npc[i] );
			}
		}
	}

	if( m_updateQuestNPCs.size() > 0 )
	{
		RoleMgr::Inst()->UpdataGameObjectQuestEffect();
	}
}

//-----------------------------------------------------------------------------
// 从任务中增加可调查地物
//-----------------------------------------------------------------------------
VOID QuestQuery::AddInvesObject( UINT16 questID )
{
	QuestsMap::const_iterator finder = m_currentQuests.find(questID);
	if( finder != m_currentQuests.end() )
	{
		QuestData *pQD = QuestMgr::Inst()->GetData();
		ASSERT( P_VALID(pQD) && _T("QuestQuery:获得QuestData错误") );

		const tagQuestDynamicTarget *pDynamicTarget = pQD->GetQuestDynamicTarget(questID);
		if( P_VALID(pDynamicTarget) )//动态
		{
			if( pDynamicTarget->eTargetType == EQTT_Invest )
			{
				for( INT i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					const DWORD& typeID = pDynamicTarget->dwTargetID[i];
					if( typeID != 0 )
					{
						InvesObjMap::iterator iter = m_invesObjects.find(typeID);
						if( iter!=m_invesObjects.end() )
						{
							iter->second++;
						}
						else
							m_invesObjects[typeID] = 1;
					}
					else
						break;
				}
			}
		}
		else
		{
			const tagQuestProto *pProto = pQD->GetQuestProto(questID);

			// 从任务结构中查找可调查地物，加入map
			for( INT i=0; i<QUEST_ITEMS_COUNT; i++ )
			{
				const DWORD& typeID = pProto->investigate_objects[i];
				if( typeID != 0 )
				{
					InvesObjMap::iterator iter = m_invesObjects.find(typeID);
					if( iter!=m_invesObjects.end() )
					{
						iter->second++;
					}
					else
						m_invesObjects[typeID] = 1;
				}
			}
		}
	}

	RoleMgr::Inst()->UpdataGameObjectQuestEffect();
}

//-----------------------------------------------------------------------------
// 从任务中删除可调查地物
//-----------------------------------------------------------------------------
VOID QuestQuery::DeleteInvesObject( UINT16 questID )
{
	QuestsMap::const_iterator finder = m_currentQuests.find(questID);
	if( finder != m_currentQuests.end() )
	{
		const tagQuestTrack& track = finder->second;
		QuestData *pQD = QuestMgr::Inst()->GetData();
		ASSERT( P_VALID(pQD) && _T("QuestQuery:获得QuestData错误") );

		const tagQuestDynamicTarget *pDynamicTarget = pQD->GetQuestDynamicTarget(questID);
		if( P_VALID(pDynamicTarget) )//动态
		{
			if( pDynamicTarget->eTargetType == EQTT_Invest )
			{
				for( INT i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					const DWORD& typeID = pDynamicTarget->dwTargetID[i];
					if( typeID != 0 )
					{
						if( !track.invesObj[i] )//未被调查过
						{
							InvesObjMap::iterator iter = m_invesObjects.find(typeID);
							if( iter!=m_invesObjects.end() )
							{
								iter->second--;
								if( iter->second < 1 )
									m_invesObjects.erase(iter);
							}
						}
					}
				}
			}
		}
		else
		{
			const tagQuestProto *pProto = pQD->GetQuestProto(questID);

			// 从任务结构中找到可调查地物，删除
			for( INT i=0; i<QUEST_ITEMS_COUNT; i++ )
			{
				const DWORD& typeID = pProto->investigate_objects[i];
				if( typeID != 0 )
				{
					if( pProto->complete_req_item_num[i] > track.itemCount[i] )//如果处在可调查状态才需要删除
					{
						InvesObjMap::iterator iter = m_invesObjects.find(typeID);
						if( iter!=m_invesObjects.end() )
						{
							iter->second--;
							if( iter->second < 1 )
								m_invesObjects.erase(iter);
						}
					}
				}
			}
		}
	}

	RoleMgr::Inst()->UpdataGameObjectQuestEffect();
}

VOID QuestQuery::DeleteInvesObject( UINT16 questID,INT index )
{
	if( index >= DYNAMIC_TARGET_COUNT )
		return;

	QuestsMap::const_iterator finder = m_currentQuests.find(questID);
	if( finder != m_currentQuests.end() )
	{
		QuestData *pQD = QuestMgr::Inst()->GetData();
		const tagQuestDynamicTarget *pDynamicTarget = pQD->GetQuestDynamicTarget(questID);
		if( P_VALID(pDynamicTarget) )//动态
		{
			if( pDynamicTarget->eTargetType == EQTT_Invest )
			{
				const DWORD& typeID = pDynamicTarget->dwTargetID[index];
				InvesObjMap::iterator iter = m_invesObjects.find(typeID);
				if( iter!=m_invesObjects.end() )
				{
					iter->second--;
					if( iter->second < 1 )
						m_invesObjects.erase(iter);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// 从任务中更新可调查地物，有掉落部分
//-----------------------------------------------------------------------------
VOID QuestQuery::UpdateInvesObject( UINT16 questID )
{
	QuestsMap::const_iterator finder = m_currentQuests.find(questID);
	IMSG(_T("update invesobj questid: %d\r\n"),questID);
	if( finder != m_currentQuests.end() )
	{
		const tagQuestTrack& track = finder->second;
		QuestData *pQD = QuestMgr::Inst()->GetData();
		ASSERT( P_VALID(pQD) && _T("QuestQuery:获得QuestData错误") );

		const tagQuestProto *pProto = pQD->GetQuestProto(questID);
		bool bOnlyOneCompleted =false;

		// 从任务结构中找到可调查地物，删除，然后判断是否已经收集完成，如果没完成，加入map
		for( INT i=0; i<QUEST_ITEMS_COUNT; i++ )
		{
			const DWORD& typeID = pProto->investigate_objects[i];
			if( typeID != 0 )
			{
				InvesObjMap::iterator iter = m_invesObjects.find(typeID);
				if( iter == m_invesObjects.end() )
				{
					m_invesObjects[typeID] = 1;
					iter = m_invesObjects.find(typeID);
				}

				iter->second--;
				if( pProto->complete_req_item_num[i] > track.itemCount[i] )
					iter->second++;
				else if( pProto->only_one_item )//只要收集一种物品即可
				{
					bOnlyOneCompleted = true;
				}

				if( iter->second < 1 )
					m_invesObjects.erase(iter);
			}
		}

		if( bOnlyOneCompleted )
		{
			for( INT i=0; i<QUEST_ITEMS_COUNT; i++ )
			{
				const DWORD& typeID = pProto->investigate_objects[i];
				if( typeID != 0 )
				{
					InvesObjMap::iterator iter = m_invesObjects.find(typeID);
					if( iter!=m_invesObjects.end() )
					{
						if( pProto->complete_req_item_num[i] > track.itemCount[i] )
							iter->second--;

						if( iter->second < 1 )
							m_invesObjects.erase(iter);
					}
				}
			}
		}
	}

	RoleMgr::Inst()->UpdataGameObjectQuestEffect();
}

VOID QuestQuery::AddTriggerEffect( UINT16 questID )
{
	QuestsMap::const_iterator finder = m_currentQuests.find(questID);
	if( finder != m_currentQuests.end() )
	{
		QuestData *pQD = QuestMgr::Inst()->GetData();
		ASSERT( P_VALID(pQD) && _T("QuestQuery:获得QuestData错误") );

		const tagQuestProto *pProto = pQD->GetQuestProto(questID);

		// 从任务结构中查找触发器，加入map
		for( INT i=0; i<QUEST_TRIGGERS_COUNT; i++ )
		{
			const DWORD& typeID = pProto->complete_req_trigger[i];
			if( typeID != 0 )
			{
				TriggerEffectMap::iterator iter = m_triggers.find(typeID);
				if( iter!=m_triggers.end() )
				{
					iter->second.refNum++;
				}
				else//播放特效
				{
					Vector3 pos;
					AABBox box;
					tagTriggerEffect tagEffect;
					tagEffect.refNum = 1;
					tagEffect.effectID = GT_INVALID;
					if( MapMgr::Inst()->GetTriggerPos(typeID,pos,box) )
					{
						tagEffect.effectID = EffectMgr::Inst()->PlayEffect(pos,0.0f,_T("txqt03_1"));
					}
					
					m_triggers[typeID] = tagEffect;
				}
			}
		}
	}
}

VOID QuestQuery::DeleteTriggerEffect( UINT16 questID )
{
	QuestsMap::const_iterator finder = m_currentQuests.find(questID);
	if( finder != m_currentQuests.end() )
	{
		QuestData *pQD = QuestMgr::Inst()->GetData();
		ASSERT( P_VALID(pQD) && _T("QuestQuery:获得QuestData错误") );

		const tagQuestProto *pProto = pQD->GetQuestProto(questID);

		// 从任务结构中找到触发器，删除
		for( INT i=0; i<QUEST_TRIGGERS_COUNT; i++ )
		{
			const DWORD& typeID = pProto->complete_req_trigger[i];
			if( typeID != 0 )
			{
				if( !pQD->BeTriggered(questID,i) )//如果没有触发，才需要删除
				{
					TriggerEffectMap::iterator iter = m_triggers.find(typeID);
					if( iter!=m_triggers.end() )
					{
						iter->second.refNum--;
						if( iter->second.refNum < 1 )//删除特效
						{
							EffectMgr::Inst()->StopEffect( iter->second.effectID );
							m_triggers.erase(iter);
						}
					}
				}
			}
		}
	}
}

VOID QuestQuery::UpdateTriggerEffect( UINT16 questID )
{
	QuestsMap::const_iterator finder = m_currentQuests.find(questID);
	if( finder != m_currentQuests.end() )
	{
		const tagQuestTrack& track = finder->second;
		QuestData *pQD = QuestMgr::Inst()->GetData();
		ASSERT( P_VALID(pQD) && _T("QuestQuery:获得QuestData错误") );

		const tagQuestProto *pProto = pQD->GetQuestProto(questID);
		bool bOnlyOneCompleted =false;

		// 从任务结构中找到触发器，删除，然后判断是否已经触发完成，如果没完成，加入map
		for( INT i=0; i<QUEST_TRIGGERS_COUNT; i++ )
		{
			const DWORD& typeID = pProto->complete_req_trigger[i];
			if( typeID != 0 )
			{
				TriggerEffectMap::iterator iter = m_triggers.find(typeID);
				if( iter == m_triggers.end() )
				{
					tagTriggerEffect effect;
					effect.refNum = 1;
					effect.effectID = GT_INVALID;
					m_triggers[typeID] = effect;
					iter = m_triggers.find(typeID);
				}

				iter->second.refNum--;
				if( !pQD->BeTriggered(questID,i) )//触发器尚未被触发
					iter->second.refNum++;
				else if( pProto->only_one_trigger )//只要触发一个即可
				{
					bOnlyOneCompleted = true;
				}

				if( iter->second.refNum < 1 )
				{
					EffectMgr::Inst()->StopEffect( iter->second.effectID );
					m_triggers.erase(iter);
				}
				else if( iter->second.effectID == GT_INVALID )
				{
					Vector3 pos;
					AABBox box;
					if( MapMgr::Inst()->GetTriggerPos(typeID,pos,box) )
					{
						iter->second.effectID = EffectMgr::Inst()->PlayEffect(pos,0.0f,_T("txqt03_1"));
					}
				}
			}
		}

		if( bOnlyOneCompleted )
		{
			for( INT i=0; i<QUEST_ITEMS_COUNT; i++ )
			{
				const DWORD& typeID = pProto->complete_req_trigger[i];
				if( typeID != 0 )
				{
					TriggerEffectMap::iterator iter = m_triggers.find(typeID);
					if( iter!=m_triggers.end() )
					{
						if( !pQD->BeTriggered(questID,i) )
							iter->second.refNum--;

						if( iter->second.refNum < 1 )
						{
							EffectMgr::Inst()->StopEffect( iter->second.effectID );
							m_triggers.erase(iter);
						}
					}
				}
			}
		}
	}
}

VOID QuestQuery::UpdateTriggerEffect( void )
{
	TriggerEffectMap::iterator iter;
	for( iter=m_triggers.begin(); iter!=m_triggers.end(); ++iter )
	{
		tagTriggerEffect& tagEffect = iter->second;
		Vector3 pos;
		AABBox box;

		if( !MapMgr::Inst()->GetTriggerPos(iter->first,pos,box) )//不在本地图的触发器
		{
			tagEffect.effectID = GT_INVALID;
		}
		else if( MapMgr::Inst()->GetTriggerPos(iter->first,pos,box) && tagEffect.effectID == GT_INVALID )//在本地图的触发器开始播放特效
		{
			tagEffect.effectID = EffectMgr::Inst()->PlayEffect(pos,0.0f,_T("txqt03_1"));
		}
	}
}

//-----------------------------------------------------------------------------
// 是否为有效的可调查地物
//-----------------------------------------------------------------------------
BOOL QuestQuery::IsInvesObject( DWORD typeID )
{
	InvesObjMap::iterator finder = m_invesObjects.find(typeID);
	if( finder != m_invesObjects.end() )
		return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得某个NPC身上可接任务列表
//-----------------------------------------------------------------------------
VOID QuestQuery::GetNPCAcceptQuests( DWORD npcID,vector<UINT16>& acceptQuests ) const
{
	acceptQuests.clear();

	QuestData *pData = QuestMgr::Inst()->GetData();
	if( P_VALID(pData) )
	{
		vector<UINT16> quests;
		pData->GetNPCAcceptQuests(npcID, quests);
		for( size_t i = 0; i < quests.size(); i++ )
		{
			if( QuestCanAccept( quests[i] ) )
			{
				acceptQuests.push_back(quests[i]);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// 获得某个NPC身上可交任务列表
//-----------------------------------------------------------------------------
VOID QuestQuery::GetNPCCompleteQuests( DWORD npcID,vector<UINT16>& completeQuests ) const
{
	completeQuests.clear();

	QuestData *pData = QuestMgr::Inst()->GetData();
	QuestsMap::const_iterator iter;
	for( iter = m_currentQuests.begin(); iter != m_currentQuests.end(); ++iter )
	{
		const UINT16& questID = iter->first;
		const tagQuestProto *pQuestProto = pData->GetQuestProto(questID);

		for( INT i=0; i<QUEST_COMPLETE_NPC; i++ )
		{
			if( pQuestProto->complete_quest_npc[i] == 0 )
				break;

			if( pQuestProto->complete_quest_npc[i] == npcID && QuestCanComplete( questID ) )
			{
				completeQuests.push_back(questID);
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// 获得某个NPC身上可更新任务列表
//-----------------------------------------------------------------------------
VOID QuestQuery::GetNPCUpdateQuests( DWORD npcID,vector<UINT16>& updateQuests ) const
{
	updateQuests.clear();

	QuestData *pData = QuestMgr::Inst()->GetData();
	QuestsMap::const_iterator iter;
	for( iter = m_currentQuests.begin(); iter != m_currentQuests.end(); ++iter )
	{
		const UINT16& questID = iter->first;

		const tagQuestDynamicTarget *pDynamicTarget = pData->GetQuestDynamicTarget(questID);
		if( P_VALID(pDynamicTarget) )//动态
		{
			if( pDynamicTarget->eTargetType == EQTT_NPCTalk )
			{
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pDynamicTarget->dwTargetID[i] == 0 )
						break;

					if( pDynamicTarget->dwTargetID[i] == npcID && NPCCanUpdateQuest( npcID, questID ) )
					{
						updateQuests.push_back(questID);
						break;
					}
				}
			}

			continue;
		}

		const tagQuestProto *pQuestProto = pData->GetQuestProto(questID);

		for( INT i=0; i<QUEST_NPC_COUNT; i++ )
		{
			if( pQuestProto->complete_req_npc[i] == 0 )
				break;

			if( pQuestProto->complete_req_npc[i] == npcID && NPCCanUpdateQuest( npcID, questID ) )
			{
				updateQuests.push_back(questID);
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// 获得某个任务已经获得某个物品的数量
//-----------------------------------------------------------------------------
BOOL QuestQuery::GetQuestItemCount( UINT16 questID,DWORD num,UINT16& count ) const
{
	if( num >= QUEST_ITEMS_COUNT )
		return FALSE;

	QuestsMap::const_iterator iter = m_currentQuests.find(questID);
	if( iter != m_currentQuests.end() )
	{
		const tagQuestTrack& track = iter->second;
		count = track.itemCount[num];

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得某个任务已经击杀某个怪物的数量
//-----------------------------------------------------------------------------
BOOL QuestQuery::GetQuestCreatureCount( UINT16 questID,DWORD num,UINT16& count ) const
{
	if( num >= QUEST_CREATURES_COUNT )
		return FALSE;

	QuestsMap::const_iterator iter = m_currentQuests.find(questID);
	if( iter != m_currentQuests.end() )
	{
		const tagQuestTrack& track = iter->second;
		count = track.creatureCount[num];

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得某个任务已经调查的数量
//-----------------------------------------------------------------------------
BOOL QuestQuery::ObjBeInves( UINT16 questID,DWORD num ) const
{
	if( num >= DYNAMIC_TARGET_COUNT )
		return FALSE;

	QuestsMap::const_iterator iter = m_currentQuests.find(questID);
	if( iter != m_currentQuests.end() )
	{
		const tagQuestTrack& track = iter->second;
		return (track.invesObj[num]==true);
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 设置某个任务已经获得某个物品的数量
//-----------------------------------------------------------------------------
BOOL QuestQuery::SetQuestItemCount( UINT16 questID,DWORD index,UINT16 count )
{
	if( index >= QUEST_ITEMS_COUNT )
		return FALSE;

	QuestsMap::iterator iter = m_currentQuests.find(questID);
	if( iter != m_currentQuests.end() )
	{
		tagQuestTrack& track = iter->second;
		track.itemCount[index] = count;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 设置某个任务已经击杀某个怪物的数量
//-----------------------------------------------------------------------------
BOOL QuestQuery::SetQuestCreatureCount( UINT16 questID,DWORD index,UINT16 count )
{
	if( index >= QUEST_CREATURES_COUNT )
		return FALSE;

	QuestsMap::iterator iter = m_currentQuests.find(questID);
	if( iter != m_currentQuests.end() )
	{
		tagQuestTrack& track = iter->second;
		track.creatureCount[index] = count;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 设置某个任务已经调查地物
//-----------------------------------------------------------------------------
BOOL QuestQuery::SetQuestInvesObj( UINT16 questID,INT index )
{
	if( index >= QUEST_ITEMS_COUNT )
		return FALSE;

	QuestsMap::iterator iter = m_currentQuests.find(questID);
	if( iter != m_currentQuests.end() )
	{
		tagQuestTrack& track = iter->second;
		track.invesObj[index] = true;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 判断某个任务是否可接
//-----------------------------------------------------------------------------
BOOL QuestQuery::QuestCanAccept( UINT16 questID ) const
{
	QuestData *pData = QuestMgr::Inst()->GetData();
	const tagQuestProto *pQuestProto = pData->GetQuestProto(questID);
	if( !P_VALID(pQuestProto) )//不存在此任务
	{
		TCHAR szText[100];
		_stprintf( szText, _T("Don't find quest %d!"), questID );
		::MessageBox(NULL, szText, _T("error"), MB_OK);
		return FALSE;
	}

	// 玩家已有此任务 并且 此任务非重复完成 返回FALSE 现在只判断repeatable
	QuestsMap::const_iterator finder = m_currentQuests.find(questID);
	if( finder != m_currentQuests.end() )
	{
		return FALSE;
	}

	LocalPlayer *pPlayer = RoleMgr::Inst()->GetLocalPlayer();

	// 要求最大等级不等于0 并且 角色等级>任务要求最大等级
	if( pQuestProto->accept_req_max_level != 0 && pPlayer->GetRoleLevel() > pQuestProto->accept_req_max_level )
	{
		return FALSE;
	}

	// 要求最小等级不等于0 并且 角色等级<任务要求最小等级
	if( pQuestProto->accept_req_min_level != 0 && pPlayer->GetRoleLevel() < pQuestProto->accept_req_min_level )
	{
		return FALSE;
	}

	// 不是重复任务 并且 已经完成过
	if( !pQuestProto->repeatable && m_doneQuests.find( pQuestProto->id ) != m_doneQuests.end() )
	{
		return FALSE;
	}

	// 前序任务没有完成
	BYTE byPreQuestOK = 0x01;
	if( pQuestProto->prev_quest_id != 0 && m_doneQuests.find( pQuestProto->prev_quest_id ) == m_doneQuests.end() )
	{
		byPreQuestOK = 0x00;
	}
	BYTE byPreQuest2OK = 0x01;
	if( pQuestProto->prev_quest2_id != 0 && m_doneQuests.find( pQuestProto->prev_quest2_id ) == m_doneQuests.end() )
	{
		byPreQuest2OK = 0x00;
	}
	if( pQuestProto->prev_quest_relation == 0 && (byPreQuestOK&byPreQuest2OK) == 0 )//前序任务必须都完成
	{
		return FALSE;
	}
	else if( pQuestProto->prev_quest_relation == 1 && (byPreQuestOK|byPreQuest2OK) == 0 )//完成一个前序任务即可
	{
		return FALSE;
	}
	

	// 如果任务有职业限制
	if (pQuestProto->class_type != 0 && pQuestProto->class_type != static_cast<INT>(RoleMgr::Inst()->GetLocalPlayer()->GetPlayerClass()))
	{
		return FALSE;
	}

	// 完成了不能完成的前序任务 或者 接取了互斥的平行任务
	if( pQuestProto->uncomplete_quest_id != 0 
		&& ( m_currentQuests.find( pQuestProto->uncomplete_quest_id ) != m_currentQuests.end() 
		|| m_doneQuests.find( pQuestProto->uncomplete_quest_id ) != m_doneQuests.end() ) )
	{
		return FALSE;
	}

	// 没有所需要的物品
	for( int i=0; i<QUEST_ITEMS_COUNT; i++ )
	{
		if( pQuestProto->accept_req_item[i] == 0 )
			break;

		INT itemSum = 0;
		ItemContainer* pItemContainer = ItemMgr::Inst()->GetPocket();
		itemSum = pItemContainer->GetItemQuantity( pQuestProto->accept_req_item[i] );

		pItemContainer = ItemMgr::Inst()->GetQuest();
		itemSum += pItemContainer->GetItemQuantity( pQuestProto->accept_req_item[i] );

		if( itemSum < pQuestProto->accept_req_item_num[i] )
		{
			return FALSE;
		}
	}

	// 没有所需要的技能
	for( int i=0; i<QUEST_SKILLS_COUNT; i++ )
	{
		if( pQuestProto->accept_req_skill[i] == 0 )
			break;

		const tagSkillData *pSkill = SkillMgr::Inst()->GetSkillData( pQuestProto->accept_req_skill[i] );
		if( !P_VALID(pSkill) )
			return FALSE;

		if( pSkill->nLevel < pQuestProto->accept_req_skill_val[i] )
			return FALSE;
	}

	// 周期任务
	if( pQuestProto->period )
	{
		DoneTimeMap::const_iterator finder = m_doneTimes.find( pQuestProto->id );
		if( finder != m_doneTimes.end() )
		{
			const tagDWORDTime& doneTime = finder->second;
			tagDWORDTime curTime = ServerTime::Inst()->CalCurrentServerDwordTime();

			if( curTime.year == doneTime.year 
				&& curTime.month == doneTime.month 
				&& curTime.day == doneTime.day )//今天已做过
			{
				return FALSE;
			}

			if( pQuestProto->period_type == EQuestPeriodic_WEEK )//每周任务
			{
				if( ServerTime::Inst()->GetCurrentWeekDay() != pQuestProto->week )//今天不是指定周几
				{
					return FALSE;
				}
			}
		}
	}

	// 声望
	for( int i=0; i<QUEST_REPUTATIONS_COUNT; i++ )
	{
		if( pQuestProto->accept_req_rep[i] == 0 )
			break;

		INT repValue = ReputeMgr::Inst()->GetReputeValueEx( EReputationType(pQuestProto->accept_req_rep[i]) );
		if( repValue < pQuestProto->accept_req_min_rep[i] || repValue > pQuestProto->accept_req_max_rep[i] )
		{
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 判断某个任务是否可交
//-----------------------------------------------------------------------------
BOOL QuestQuery::QuestCanComplete( UINT16 questID ) const
{
	// 玩家没有此任务返回FALSE
	QuestsMap::const_iterator iterQuest = m_currentQuests.find(questID);
	if( iterQuest == m_currentQuests.end() )
	{
		return FALSE;
	}

	QuestData *pData = QuestMgr::Inst()->GetData();
	const tagQuestProto *pQuestProto = pData->GetQuestProto(questID);
	ASSERT( P_VALID(pQuestProto) );

	const tagQuestTrack& questTrack = iterQuest->second;
	LocalPlayer *pPlayer = RoleMgr::Inst()->GetLocalPlayer();

	const tagQuestDynamicTarget *pDynamicTarget = pData->GetQuestDynamicTarget(questID);
	if( P_VALID(pDynamicTarget) )//动态
	{
		switch( pDynamicTarget->eTargetType )
		{
		case EQTT_Kill:
			{
				// 完成任务所需要杀的怪和数量
				for( int i=0; i<1; i++ )
				{
					if( pDynamicTarget->dwTargetID[i] != 0 )
					{
						if( questTrack.creatureCount[i] < pDynamicTarget->nTargetNum[i] )
						{
							return FALSE;
						}
					}
					else
						break;
				}
			}
			break;
		case EQTT_Collect:
			{
				// 完成任务所需要的物品和数量
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pDynamicTarget->dwTargetID[i] != 0 )
					{
						if( pQuestProto->only_one_item == TRUE )
						{
							// 收集完其中一种就算完成
							if( questTrack.itemCount[i] >= pDynamicTarget->nTargetNum[i] )
								break;
						}
						else
						{
							if( questTrack.itemCount[i] < pDynamicTarget->nTargetNum[i] )
								return FALSE;
						}
					}
					else if( pQuestProto->only_one_item == TRUE && i != 0 )
						return FALSE;
					else
						break;
				}
			}
			break;
		case EQTT_NPCTalk:
			{
				// 与NPC对话，顺序由服务器判定，客户端不需要判断
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pDynamicTarget->dwTargetID[i] != 0 )
					{
						if( pQuestProto->only_one_npc == TRUE )//只需要与一个人对话
						{
							if( pData->NPCBeTalked( questID,i ) )
								break;
						}
						else
						{
							if( !pData->NPCBeTalked( questID,i ) )
								return FALSE;
						}
					}
					else if( pQuestProto->only_one_npc == TRUE && i != 0 )
						return FALSE;
					else
						break;
				}
			}
			break;
		case EQTT_Invest:
			{
				// 完成调查
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pDynamicTarget->dwTargetID[i]!=0 && !questTrack.invesObj[i] )
					{
						return FALSE;
					}
				}
			}
			break;
		}

		return TRUE;
	}

	// 完成任务需要的等级不等于0 并且 角色等级<完成任务需要的等级
	if( pQuestProto->complete_req_level != 0 && pPlayer->GetRoleLevel() < pQuestProto->complete_req_level )
	{
		return FALSE;
	}

	// 是否结婚

	// 情侣数量
	
	bool conditionIsExist = false;

	{
		// 完成任务所需要的物品和数量
		int i;
		for( i=0; i<QUEST_ITEMS_COUNT; i++ )
		{
			if( pQuestProto->complete_req_item[i] != 0 )
			{
				if( pQuestProto->only_one_item == TRUE )//收集完其中一种就算完成
				{
					if( questTrack.itemCount[i] >= pQuestProto->complete_req_item_num[i] )
					{
						conditionIsExist = true;
						break;
					}
				}
				else
				{
					if( questTrack.itemCount[i] < pQuestProto->complete_req_item_num[i] )
						return FALSE;
				}

				conditionIsExist = true;
			}
		}
		if( pQuestProto->only_one_item == TRUE && i == QUEST_ITEMS_COUNT )
			return FALSE;
		if( pQuestProto->only_one_condition && conditionIsExist )//是否完成一项条件即可完成整个任务
			return TRUE;

		conditionIsExist = false;

		// 完成任务所需要杀的怪和数量
		for( i=0; i<QUEST_CREATURES_COUNT; i++ )
		{
			if( pQuestProto->complete_req_creature[i*3] != 0 )
			{
				if( pQuestProto->only_one_creature == TRUE )
				{
					// 击杀完其中一种就算完成
					if( questTrack.creatureCount[i] >= pQuestProto->complete_req_creature_num[i]  )
					{
						conditionIsExist = true;
						break;
					}
				}
				else
				{
					if( questTrack.creatureCount[i] < pQuestProto->complete_req_creature_num[i]  )
						return FALSE;
				}

				conditionIsExist = true;
			}
		}
		if( pQuestProto->only_one_creature == TRUE && i == QUEST_CREATURES_COUNT )
			return FALSE;
		if( pQuestProto->only_one_condition && conditionIsExist )//是否完成一项条件即可完成整个任务
			return TRUE;
	}

	conditionIsExist = false;

	// 与NPC对话，顺序由服务器判定，客户端不需要判断
	for( int i=0; i<QUEST_NPC_COUNT; i++ )
	{
		if( pQuestProto->complete_req_npc[i] != 0 )
		{
			if( pQuestProto->only_one_npc == TRUE )//只需要与一个人对话
			{
				if( pData->NPCBeTalked( questID,i ) )
				{
					conditionIsExist = true;
					break;
				}
			}
			else
			{
				if( !pData->NPCBeTalked( questID,i ) )
					return FALSE;
			}

			conditionIsExist = true;
		}
		else if( pQuestProto->only_one_npc == TRUE && i != 0 )
			return FALSE;
		else
			break;
	}
	if( pQuestProto->only_one_condition && conditionIsExist )//是否完成一项条件即可完成整个任务
	{
		return TRUE;
	}

	//conditionIsExist = false;

	//// 完成调查
	//for( int i=4; i<QUEST_ITEMS_COUNT; i++ )
	//{
	//	if( pQuestProto->investigate_objects[i]!=0 && !questTrack.invesObj[i-4] )
	//	{
	//		return FALSE;
	//	}

	//	conditionIsExist = true;
	//}
	//if( pQuestProto->only_one_condition && conditionIsExist )//是否完成一项条件即可完成整个任务
	//{
	//	return TRUE;
	//}

	conditionIsExist = false;

	// 完成任务所需要的触发器
	for( int i=0; i<QUEST_TRIGGERS_COUNT; i++ )
	{
		if( pQuestProto->complete_req_trigger[i] != 0 )
		{
			if( pQuestProto->only_one_trigger == TRUE )
			{
				if( pData->BeTriggered( questID,i ) )//只需要触发一个
				{
					conditionIsExist = true;
					break;
				}
			}
			else
			{
				if( !pData->BeTriggered( questID,i ) )
					return FALSE;
			}

			conditionIsExist = true;
		}
		else if( pQuestProto->only_one_trigger == TRUE && i !=0 )
			return FALSE;
		else
			break;
	}
	if( pQuestProto->only_one_condition && conditionIsExist )//是否完成一项条件即可完成整个任务
	{
		return TRUE;
	}

	// 完成任务所需要的金钱
	if( CurrencyMgr::Inst()->GetBagSilver() < pQuestProto->complete_req_money )
	{
		return FALSE;
	}
	if( pQuestProto->only_one_condition )//是否完成一项条件即可完成整个任务
	{
		return TRUE;
	}

	// 完成任务所需要的地图ID

	// 完成任务所需要的地图X、Y、Z坐标，半径

	// 任务完成事件类型，物品或者技能

	return TRUE;
}

//-----------------------------------------------------------------------------
// 判断对于某个NPC某个任务是否可更新
//-----------------------------------------------------------------------------
BOOL QuestQuery::NPCCanUpdateQuest( DWORD npcID, UINT16 questID ) const
{
	// 玩家没有此任务返回FALSE
	QuestsMap::const_iterator iterQuest = m_currentQuests.find(questID);
	if( iterQuest == m_currentQuests.end() )
	{
		return FALSE;
	}

	QuestData *pData = QuestMgr::Inst()->GetData();

	const tagQuestDynamicTarget *pDynamicTarget = pData->GetQuestDynamicTarget(questID);
	if( P_VALID(pDynamicTarget) )//动态
	{
		if( pDynamicTarget->eTargetType == EQTT_NPCTalk )
		{
			for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
			{
				if( pDynamicTarget->dwTargetID[i] == 0 )
					break;

				if( pDynamicTarget->dwTargetID[i] == npcID && !pData->NPCBeTalked( questID,i ) )//是当前NPC 并且 没有对话
				{
					return TRUE;
				}
			}
		}
	}
	else
	{
		const tagQuestProto *pQuestProto = pData->GetQuestProto(questID);
		ASSERT( P_VALID(pQuestProto) );

		for( int i=0; i<QUEST_NPC_COUNT; i++ )
		{
			const DWORD& reqNPCID = pQuestProto->complete_req_npc[i];
			if( reqNPCID == 0 )//没有所需对话NPC
			{
				return FALSE;
			}
			else
			{
				if( pQuestProto->only_in_order && reqNPCID != npcID && !pData->NPCBeTalked( questID,i ) )//必须按顺序对话 并且 前序NPC没有对话
				{
					return FALSE;
				}

				if( reqNPCID == npcID && !pData->NPCBeTalked( questID,i ) )//是当前NPC 并且 没有对话
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 判断某个任务是否可以跟踪
//-----------------------------------------------------------------------------
BOOL QuestQuery::QuestCanTrack( UINT16 questID ) const
{
	QuestData *pData = QuestMgr::Inst()->GetData();

	const tagQuestDynamicTarget *pDynamicTarget = pData->GetQuestDynamicTarget(questID);
	if( P_VALID(pDynamicTarget) )//动态
	{
		switch( pDynamicTarget->eTargetType )
		{
		case EQTT_Kill:
			{
				// 是否需要杀怪
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pDynamicTarget->dwTargetID[i] != 0 )
						return TRUE;
				}
			}
			break;
		case EQTT_Collect:
			{
				// 是否需要物品
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pDynamicTarget->dwTargetID[i] != 0 )
						return TRUE;
				}
			}
			break;
		case EQTT_NPCTalk:
			{
				// 是否需要与NPC对话
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pDynamicTarget->dwTargetID[i] != 0 )
						return TRUE;
				}
			}
			break;
		}
	}

	const tagQuestProto *pQuestProto = pData->GetQuestProto(questID);
	ASSERT( P_VALID(pQuestProto) );

	// 是否需要物品
	for( int i=0; i<QUEST_ITEMS_COUNT; i++ )
	{
		if( pQuestProto->complete_req_item[i] != 0 )
			return TRUE;
	}

	// 是否需要杀怪
	for( int i=0; i<QUEST_CREATURES_COUNT; i++ )
	{
		if( pQuestProto->complete_req_creature[i*3] != 0 )
			return TRUE;
	}

	// 是否需要与NPC对话
	for( int i=0; i<QUEST_NPC_COUNT; i++ )
	{
		if( pQuestProto->complete_req_npc[i] != 0 )
			return TRUE;
	}

	// 是否需要触发器
	for( int i=0; i<QUEST_TRIGGERS_COUNT; i++ )
	{
		if( pQuestProto->complete_req_trigger[i] != 0 )
			return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 判断某个任务是否正在跟踪
//-----------------------------------------------------------------------------
BOOL QuestQuery::QuestIsTracked( UINT16 questID ) const
{
	QuestsMap::const_iterator finder = m_currentQuests.find( questID );
	if( finder != m_currentQuests.end() )
	{
		const tagQuestTrack& track = finder->second;
		if( track.isTracked )
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 更新某个任务状态，任务完成返回TRUE，反之返回FALSE
//-----------------------------------------------------------------------------
BOOL QuestQuery::UpdateQuestState( UINT16 questID )
{
	if( QuestCanComplete( questID ) )
	{
		// 添加完成任务NPC到可完成NPC列表去
		QuestData *pData = QuestMgr::Inst()->GetData();
		ASSERT( P_VALID(pData) );

		const tagQuestProto *pQuest = pData->GetQuestProto(questID);
		if( P_VALID(pQuest) )
		{
			if( pQuest->complete_quest_npc[0] == 0 )//自动完成
			{
				QuestMgr::Inst()->SendCompleteQuest(questID,0);
			}
			else
			{
				for( int i=0; i<QUEST_COMPLETE_NPC; i++ )
				{
					if( pQuest->complete_quest_npc[i] != 0 )
					{
						m_completeQuestNPCs.insert( pQuest->complete_quest_npc[i] );
					}
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 建立当前地图的NPC列表
//-----------------------------------------------------------------------------
VOID QuestQuery::BuildMapQuestNPCs( void )
{
	m_mapNPCMap.clear();

	GameMap& gm = MapMgr::Inst()->GetGameMap();
	const std::map<DWORD,tagMapNPC>& npcs = gm.GetNpc();
	std::map<DWORD,tagMapNPC>::const_iterator iter;
	for( iter = npcs.begin(); iter != npcs.end(); ++iter )
	{
		const tagMapNPC& npcInfo = iter->second;

		MapNPCMap::iterator finder = m_mapNPCMap.find(npcInfo.dwTypeID);
		if( finder != m_mapNPCMap.end() )
		{
			finder->second.vPos.push_back(npcInfo.vPos);
		}
		else
		{
			tagMapNPCInfo mapNPCInfo;
			mapNPCInfo.dwTypeID = npcInfo.dwTypeID;
			mapNPCInfo.vPos.push_back(npcInfo.vPos);
			m_mapNPCMap.insert( MapNPC_Pair(mapNPCInfo.dwTypeID, mapNPCInfo) );
		}
	}
}

//-----------------------------------------------------------------------------
// 建立动态的NPC列表
//-----------------------------------------------------------------------------
VOID QuestQuery::BuildDynamicQuestNPCs( list<tstring>& strNPCs )
{
	m_dynamicNPCs.clear();

	list<tstring>::const_iterator iter;
	for( iter=strNPCs.begin(); iter!=strNPCs.end(); ++iter )
	{
		m_dynamicNPCs.insert( (DWORD)_tstol((*iter).c_str()) );
	}
}

//-----------------------------------------------------------------------------
// 获得某个NPC的位置
//-----------------------------------------------------------------------------
BOOL QuestQuery::GetNPCPosition( DWORD npcID,Vector3& pos,bool bRandom ) const
{
	MapNPCMap::const_iterator finder = m_mapNPCMap.find(npcID);

	if( finder != m_mapNPCMap.end() )
	{
		const tagMapNPCInfo& mapNPC = finder->second;

		if( bRandom )
		{
			INT index = TObjRef<Util>()->RandomInRange(0,mapNPC.vPos.size()-1);
			pos = mapNPC.vPos[index];
		}
		else
		{
			pos = mapNPC.vPos[0];
		}

		return TRUE;
	}

	return FALSE;
}

BOOL QuestQuery::GetNPCPosition( DWORD npcID,vector<Vector3>& posArray ) const
{
	MapNPCMap::const_iterator finder = m_mapNPCMap.find(npcID);

	if( finder != m_mapNPCMap.end() )
	{
		const tagMapNPCInfo& mapNPC = finder->second;

		posArray.resize( mapNPC.vPos.size() );
		memcpy( &posArray[0], &mapNPC.vPos[0], posArray.size() * sizeof(Vector3) );

		return TRUE;
	}

	return FALSE;
}

BOOL QuestQuery::IsAcceptNPC( DWORD npcID ) const
{
	NPCSet::const_iterator finder = m_acceptQuestNPCs.find( npcID );
	if( finder != m_acceptQuestNPCs.end() )
		return TRUE;

	finder = m_dynAcceptQuestNPCs.find( npcID );
	if( finder != m_dynAcceptQuestNPCs.end() )
		return TRUE;




	return FALSE;
}

BOOL QuestQuery::IsCompleteNPC( DWORD npcID ) const
{
	NPCSet::const_iterator finder = m_completeQuestNPCs.find( npcID );
	if( finder != m_completeQuestNPCs.end() )
		return TRUE;
	else
		return FALSE;
}

BOOL QuestQuery::IsUpdateNPC( DWORD npcID ) const
{
	NPCSet::const_iterator finder = m_updateQuestNPCs.find( npcID );
	if( finder != m_updateQuestNPCs.end() )
		return TRUE;
	else
		return FALSE;
}

BOOL QuestQuery::QuestIsCompleted( UINT16 questID ) const
{
	map<UINT16,INT>::const_iterator finder = m_doneQuests.find( questID );
	if( finder != m_doneQuests.end() )
	{
		if( finder->second > 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL QuestQuery::QuestIsAccepted( UINT16 questID ) const
{
	QuestsMap::const_iterator finder = m_currentQuests.find( questID );
	if( finder != m_currentQuests.end() )
	{
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 对活动任务进行排序
//-----------------------------------------------------------------------------
VOID QuestQuery::SortActiveQuest( ESortType eSort )
{
	switch( eSort )
	{
	case EST_Time:
		{
			TimeSort sortObj;
			sort(m_activeQuests.begin(),m_activeQuests.end(),sortObj);
		}
		break;

	case EST_Quest:
		{
			QuestSort sortObj;
			sort(m_activeQuests.begin(),m_activeQuests.end(),sortObj);
		}
		break;

	case EST_Level:
		{
			LevelSort sortObj;
			sort(m_activeQuests.begin(),m_activeQuests.end(),sortObj);
		}
		break;

	case EST_AcceptNPC:
		{
			AcceptNPCSort sortObj;
			sort(m_activeQuests.begin(),m_activeQuests.end(),sortObj);
		}
		break;

	case EST_Complete:
		{
			CompleteSort sortObj;
			sort(m_activeQuests.begin(),m_activeQuests.end(),sortObj);
		}
		break;
	}
}

UINT16 QuestQuery::GetActiveQuestID( DWORD index ) const
{
	if( index < m_activeQuests.size() )
		return m_activeQuests[index].questID;

	return -1;
}

DWORD QuestQuery::GetActiveQuestNPCID( DWORD index ) const
{
	if( index < m_activeQuests.size() )
		return m_activeQuests[index].acceptNPCID;

	return GT_INVALID;
}

VOID QuestQuery::Reset( void )
{
	m_acceptQuestNPCs.clear();
	m_dynAcceptQuestNPCs.clear();
	m_completeQuestNPCs.clear();
	m_updateQuestNPCs.clear();
	m_currentQuests.clear();
	m_mapNPCMap.clear();
	m_activeQuests.clear();
	m_invesObjects.clear();
	m_triggers.clear();
	m_doneQuests.clear();
	m_doneTimes.clear();
}