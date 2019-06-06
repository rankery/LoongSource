/********************************************************************
	created:	2008/11/04
	created:	4:11:2008   13:59
	filename: 	d:\loong_client\Client\QuestTrackFrame.cpp
	file path:	d:\loong_client\Client
	file base:	QuestTrackFrame
	file ext:	cpp
	author:		leili
	
	purpose:	任务追踪界面实现
*********************************************************************/
#include "StdAfx.h"
#include "QuestTrackFrame.h"
#include "QuestMgr.h"
#include "CreatureData.h"
#include "ItemProtoData.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "CombatEvent.h"

namespace
{
	struct tagTrack 
	{
		UINT16	questID;
		float	lastUpdateTime;
	};

	class TrackSort
	{
	public:
		bool operator()(const tagTrack& quest1,const tagTrack& quest2)
		{
			return quest1.lastUpdateTime > quest2.lastUpdateTime;
		}
	};
}

//-----------------------------------------------------------------------------
// 构造函数
//-----------------------------------------------------------------------------
QuestTrackFrame::QuestTrackFrame( void )
: m_Trunk(this)
, m_pWnd(0)
, m_pQuest(0)
, m_pBtnCloseWnd(0)
, m_trackCount(0)
{

}

//-----------------------------------------------------------------------------
// 析构函数
//-----------------------------------------------------------------------------
QuestTrackFrame::~QuestTrackFrame( void )
{
	
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
BOOL QuestTrackFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 销毁
//-----------------------------------------------------------------------------
BOOL QuestTrackFrame::Destroy()
{
	GameFrame::Destroy();

	if( P_VALID(m_pWnd) )
		m_pGUI->AddToDestroyList(m_pWnd);

	return TRUE;
}

//-----------------------------------------------------------------------------
// 重新读入窗口界面
//-----------------------------------------------------------------------------
BOOL QuestTrackFrame::ReloadUI()
{
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\tasktrack.xml");
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&QuestTrackFrame::EventHandler));

	m_pBtnCloseWnd	= (GUIButton*)	m_pWnd->GetChild(_T("tt_pic\\closebutt"));
	m_pQuest		= (GUIStaticEx*)m_pWnd->GetChild(_T("tt_pic\\tt_back\\tt_list"));
	m_pQuest->SetText(_T(""));

	//m_pWnd->SetInvisible( TRUE );

	return TRUE;
}

//-----------------------------------------------------------------------------
// 界面消息事件处理
//-----------------------------------------------------------------------------
BOOL QuestTrackFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnCloseWnd )// 关闭任务追踪界面
			{
				tagGameEvent e(_T("QuestMainFrameEvent_CloseTrack"), this);
				m_pMgr->SendEvent(&e);

				QuestMgr::Inst()->CloseTrackFrame();
			}
		}
		break;

	case EGUIE_StaticExLinkClick:
		{
			Vector3 pos;
			QuestData *pData = QuestMgr::Inst()->GetData();
			ASSERT( P_VALID(pData) );
			QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
			ASSERT( P_VALID(pQuery) );

			if( !pQuery->GetNPCPosition( pEvent->dwParam1, pos ) )//不在本地图，进行跨场景寻路
			{
				tstring mapName;
				if( !pData->GetPathPointMap( pEvent->dwParam1, mapName ) )//获得地图名称
					break;

				QuestMgr::Inst()->MoveToMap(mapName.c_str(),pos);
			}

			LocalPlayer *pLP = RoleMgr::Inst()->GetLocalPlayer();

			tagMouseMoveEvent event( _T("tagMouseMoveEvent"), NULL );
			event.start = pLP->GetPos();
			event.end = pos;
			event.validDist = 100.0f;

			m_pMgr->SendEvent( &event );
		}
		break;

	default:
		break;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 显示界面
//-----------------------------------------------------------------------------
VOID QuestTrackFrame::Show( void )
{
	if( P_VALID(m_pWnd) ) 
	{
		m_pWnd->SetInvisible(FALSE);
		m_pWnd->FlipToTop();
	}
}

//-----------------------------------------------------------------------------
// 隐藏界面
//-----------------------------------------------------------------------------
VOID QuestTrackFrame::Hide( void )
{
	if( P_VALID(m_pWnd) ) 
		m_pWnd->SetInvisible(TRUE);
}

//-----------------------------------------------------------------------------
// 界面是否可见
//-----------------------------------------------------------------------------
BOOL QuestTrackFrame::IsVisible( void ) const
{
	if( P_VALID(m_pWnd) )
	{
		return !m_pWnd->IsInvisible();
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 跟踪某个任务
//-----------------------------------------------------------------------------
BOOL QuestTrackFrame::AddQuestTrack( UINT16 questID )
{
	QuestData *pData = QuestMgr::Inst()->GetData();
	ASSERT( P_VALID(pData) && _T("QuestTrackFrame获得QuestData错误") );

	m_trackCount++;

	tstring questName;
	pData->GetQuestName( questID, questName );

	tstringstream stream;
	TCHAR szQuestTrack[512];
	DWORD color = 0xFFFFF7E0;
	pData->GetQuestTypeColor(questID, color);

	stream << m_pQuest->GetText();

	// 任务名称
	_stprintf( szQuestTrack, 
		_T("%d.<color=0x%x>%s<color=0xFFFFF7E0>"),
		m_trackCount,
		color,
		questName.c_str());
	stream << szQuestTrack;

	QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
	ASSERT( P_VALID(pQuery) && _T("QuestTrackFrame获得QuestQuery错误") );

	// 任务完成状况
	if( pQuery->QuestCanComplete(questID) )
	{
		_stprintf( szQuestTrack,
			_T("(%s)\\n"),
			g_StrTable[_T("QuestTrack_Done")]);
		stream << szQuestTrack;
	}
	else
	{
		_stprintf( szQuestTrack,
			_T("(%s)\\n"),
			g_StrTable[_T("QuestTrack_Doing")]);
		stream << szQuestTrack;
	}

	const tagQuestProto* pQuest = pData->GetQuestProto(questID);

	const tagQuestDynamicTarget *pDynamicTarget = pData->GetQuestDynamicTarget(questID);
	if( P_VALID(pDynamicTarget) )//动态
	{
		UINT16 itemCount,creatureCount,itemSum,creatureSum;
		DWORD itemID,creatureID;

		switch( pDynamicTarget->eTargetType )
		{
		case EQTT_Kill:
			{
				// 怪物跟踪
				for( int i=0; i<1; i++ )
				{
					if( pData->GetQuestRequiredCreatureCount( questID, i, creatureID, creatureSum ) )
					{
						const tagCreatureProto* creatureProto = CreatureData::Inst()->FindNpcAtt(creatureID);
						ASSERT( creatureProto != NULL );

						pQuery->GetQuestCreatureCount( questID, i, creatureCount );
						if( creatureCount > creatureSum )
						{
							creatureCount = creatureSum;
						}

						_stprintf( szQuestTrack, 
							_T("  <color=0xFF00DE2A>%s<color=0xFFF6EB78>(%d/%d)<color=0xFFFFF7E0>\\n"),
							creatureProto->szName, 
							creatureCount, 
							creatureSum );
						stream << szQuestTrack;
					}
				}
			}
			break;
		case EQTT_Collect:
			{
				// 物品跟踪
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pData->GetQuestRequiredItemCount( questID, i, itemID, itemSum ) )
					{
						const tagItemDisplayInfo* itemProto = ItemProtoData::Inst()->FindItemDisplay(itemID);
						ASSERT( itemProto != NULL );

						pQuery->GetQuestItemCount( questID, i, itemCount );
						if( itemCount > itemSum )
						{
							itemCount = itemSum;
						}

						_stprintf( szQuestTrack, 
							_T("  <color=0xFF31A0FF>%s<color=0xFFF6EB78>(%d/%d)<color=0xFFFFF7E0>\\n"), 
							itemProto->szName, 
							itemCount, 
							itemSum );
						stream << szQuestTrack;
					}
				}
			}
			break;
		case EQTT_NPCTalk:
			{
				// 对话NPC跟踪
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pDynamicTarget->dwTargetID[i] == 0 )
						break;

					tstring strTalked;
					if( pData->NPCBeTalked( questID, i ) )
						strTalked = g_StrTable[_T("QuestTrack_NPCTalked")];
					else
						strTalked = g_StrTable[_T("QuestTrack_NPCNoTalk")];

					const tagCreatureProto *creatureProto = CreatureData::Inst()->FindNpcAtt( pDynamicTarget->dwTargetID[i] );
					ASSERT( P_VALID(creatureProto) );
					_stprintf( szQuestTrack,
						_T("  <color=0xFFABABFF><link=%x,0xFFABABFF>%s<link=0xffffffff,0> <color=0xFFF6EB78>(%s)<color=0xFFFFF7E0>\\n"),
						creatureProto->dwTypeID,
						creatureProto->szName,
						strTalked.c_str() );
					stream << szQuestTrack;
				}
			}
			break;
		case EQTT_Invest:
			{
				// 调查地物追踪
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pDynamicTarget->dwTargetID[i] == 0 )
						break;

					tstring strInves;
					if( pQuery->ObjBeInves( questID, i ) )
						strInves = g_StrTable[_T("QuestTrack_TriggerUpdated")];
					else
						strInves = g_StrTable[_T("QuestTrack_TriggerNoUpdate")];

					const tagCreatureProto *creatureProto = CreatureData::Inst()->FindNpcAtt( pDynamicTarget->dwTargetID[i] );
					ASSERT( P_VALID(creatureProto) );
					_stprintf( szQuestTrack,
						_T("  <color=0xFFABABFF><link=%x,0xFFABABFF>%s<link=0xffffffff,0> <color=0xFFF6EB78>(%s)<color=0xFFFFF7E0>\\n"),
						creatureProto->dwTypeID,
						creatureProto->szName,
						strInves.c_str() );
					stream << szQuestTrack;
				}
			}
			break;
		}
	}
	else
	{
		UINT16 itemCount,creatureCount,itemSum,creatureSum;
		DWORD itemID,creatureID;
		
		if (pQuest->only_one_item == TRUE)
		{
			_stprintf( szQuestTrack,
				_T("(%s)\\n"),
				g_StrTable[_T("QuestTrack_Only_One_Item")]);
			stream << szQuestTrack;
		}
		else if (pQuest->only_one_npc == TRUE)
		{
			_stprintf( szQuestTrack,
				_T("(%s)\\n"),
				g_StrTable[_T("QuestTrack_Only_One_NPC")]);
			stream << szQuestTrack;
		}
		else if (pQuest->only_one_creature == TRUE)
		{
			_stprintf( szQuestTrack,
				_T("(%s)\\n"),
				g_StrTable[_T("QuestTrack_Only_One_Creature")]);
			stream << szQuestTrack;
		}

		
		// 物品跟踪
		for( int i=0; i<QUEST_ITEMS_COUNT; i++ )
		{
			if( pData->GetQuestRequiredItemCount( questID, i, itemID, itemSum ) )
			{
				const tagItemDisplayInfo* itemProto = ItemProtoData::Inst()->FindItemDisplay(itemID);
				ASSERT( itemProto != NULL );

				pQuery->GetQuestItemCount( questID, i, itemCount );
				if( itemCount > itemSum )
				{
					itemCount = itemSum;
				}

				_stprintf( szQuestTrack, 
					_T("  <color=0xFF31A0FF>%s<color=0xFFF6EB78>(%d/%d)<color=0xFFFFF7E0>\\n"), 
					itemProto->szName, 
					itemCount, 
					itemSum );
				stream << szQuestTrack;
			}
		}

		// 怪物跟踪
		for( int i=0; i<QUEST_CREATURES_COUNT; i++ )
		{
			if( pData->GetQuestRequiredCreatureCount( questID, i, creatureID, creatureSum ) )
			{
				const tagCreatureProto* creatureProto = CreatureData::Inst()->FindNpcAtt(creatureID);
				ASSERT( creatureProto != NULL );

				pQuery->GetQuestCreatureCount( questID, i, creatureCount );
				if( creatureCount > creatureSum )
				{
					creatureCount = creatureSum;
				}

				_stprintf( szQuestTrack, 
					_T("  <color=0xFF00DE2A>%s<color=0xFFF6EB78>(%d/%d)<color=0xFFFFF7E0>\\n"),
					creatureProto->szName, 
					creatureCount, 
					creatureSum );
				stream << szQuestTrack;
			}
		}

		// 对话NPC跟踪
		for( int i=0; i<QUEST_NPC_COUNT; i++ )
		{
			if( pQuest->complete_req_npc[i] == 0 )
				break;

			tstring strTalked;
			if( pData->NPCBeTalked( questID, i ) )
				strTalked = g_StrTable[_T("QuestTrack_NPCTalked")];
			else
				strTalked = g_StrTable[_T("QuestTrack_NPCNoTalk")];

			const tagCreatureProto *creatureProto = CreatureData::Inst()->FindNpcAtt( pQuest->complete_req_npc[i] );
			ASSERT( creatureProto != NULL );
			_stprintf( szQuestTrack,
				_T("  <color=0xFFABABFF><link=%x,0xFFABABFF>%s<link=0xffffffff,0> <color=0xFFF6EB78>(%s)<color=0xFFFFF7E0>\\n"),
				creatureProto->dwTypeID,
				creatureProto->szName,
				strTalked.c_str() );
			stream << szQuestTrack;
		}

		// 触发器追踪
		for( int i=0; i<QUEST_TRIGGERS_COUNT; i++ )
		{
			if( pQuest->complete_req_trigger[i] == 0 )
				break;

			tstring strTrigger;
			if( pData->BeTriggered( questID, i ) )
				strTrigger = g_StrTable[_T("QuestTrack_TriggerUpdated")];
			else
				strTrigger = g_StrTable[_T("QuestTrack_TriggerNoUpdate")];

			QuestData *pData = QuestMgr::Inst()->GetData();
			tstring strName;
			pData->GetTriggerName( pQuest->complete_req_trigger[i], strName );
			_stprintf( szQuestTrack,
				_T("  <color=0xFFABABFF>%s <color=0xFFF6EB78>(%s)<color=0xFFFFF7E0>\\n"),
				strName.c_str(),
				strTrigger.c_str() );
			stream << szQuestTrack;
		}
	}

	// 任务回复
	TCHAR szCompleteNPC[128]={0};
	for( int i=0; i<QUEST_COMPLETE_NPC; i++ )
	{
		if( pQuest->complete_quest_npc[i] == 0 )
			break;

		const tagCreatureProto *creatureProto = CreatureData::Inst()->FindNpcAtt( pQuest->complete_quest_npc[i] );
		ASSERT( creatureProto != NULL );

		if( _tcslen(szCompleteNPC) > 0 )
		{
			_stprintf( szCompleteNPC,
				_T("%s,<color=0xFFABABFF><link=%x,0xFFABABFF>%s<link=0xffffffff,0><color=0xFFFFF7E0>"),
				szCompleteNPC,
				creatureProto->dwTypeID,
				creatureProto->szName );
		}
		else
		{
			_stprintf( szCompleteNPC,
				_T("<color=0xFFABABFF><link=%x,0xFFABABFF>%s<link=0xffffffff,0><color=0xFFFFF7E0>"),
				creatureProto->dwTypeID,
				creatureProto->szName );
		}
	}
	if( _tcslen(szCompleteNPC) > 0 )
	{
		_stprintf( szQuestTrack,
			_T("\\n%s%s"),
			g_StrTable[_T("QuestTrack_Reply")],
			szCompleteNPC);
		stream << szQuestTrack;
	}

	// 任务之间的空行
	stream << _T("\\n\\n");
	m_pQuest->SetText( stream.str().c_str() );

	// 滑动条拖动设置
	tagPoint size = m_pQuest->GetSize();
	if( m_pQuest->GetTextTotalHeight() > size.y )
	{
		size.y = m_pQuest->GetTextTotalHeight();
		m_pQuest->SetResizable(TRUE);
		m_pQuest->Resize( size );
		m_pQuest->SetResizable(FALSE);
	}

	m_pQuest->SetRefresh(TRUE);

	return TRUE;
}

//-----------------------------------------------------------------------------
// 更新任务跟踪
//-----------------------------------------------------------------------------
VOID QuestTrackFrame::UpdateQuestTrack( void )
{
	QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
	ASSERT( P_VALID(pQuery) && _T("QuestTrackFrame获得QuestQuery错误") );

	m_trackCount = 0;
	m_pQuest->SetText(_T(""));

	// 获得已经跟踪的任务列表
	vector<tagTrack> tracks;
	const QuestQuery::QuestsMap& quests = pQuery->GetCurrentQuests();
	for( QuestQuery::QuestsMap::const_iterator iter = quests.begin(); iter!=quests.end(); ++iter )
	{
		const QuestQuery::tagQuestTrack& track = iter->second;
		if( track.isTracked )
		{
			tagTrack trackInfo;
			trackInfo.questID = iter->first;
			trackInfo.lastUpdateTime = track.lastUpdateTime;

			tracks.push_back( trackInfo );
		}
	}

	// 按最近更新时间进行排序，显示
	TrackSort sortObj;
	sort(tracks.begin(),tracks.end(),sortObj);
	for( size_t i=0; i<tracks.size(); i++ )
	{
		tagTrack& track = tracks[i];
		AddQuestTrack( track.questID );
	}

	m_pQuest->SetRefresh(TRUE);
}

BOOL QuestTrackFrame::CanTrackQuest( UINT16 questID )
{
	if( m_trackCount >= 50 )
		return FALSE;

	return TRUE;
}