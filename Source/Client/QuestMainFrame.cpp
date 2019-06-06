/********************************************************************
	created:	2008/04/30
	created:	30:4:2008   12:01
	filename: 	d:\loong_client\Client\QuestMainFrame.cpp
	file path:	d:\loong_client\Client
	file base:	QuestMainFrame
	file ext:	cpp
	author:		leili
	
	purpose:	任务主界面实现，处理任务主窗口的界面逻辑
*********************************************************************/
#include "StdAfx.h"
#include "QuestMainFrame.h"
#include "QuestNPCFrame.h"
#include "ItemMgr.h"
#include "GameClientDefine.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "ItemProtoData.h"
#include "CreatureData.h"
#include "RoleMgr.h"
#include "CombatEvent.h"
#include "LocalPlayer.h"
#include "MapMgr.h"
#include "UIEvent.h"
#include "..\WorldDefine\currency_define.h"

namespace
{
	tstring& replace_all(tstring& str,const tstring& old_value,const tstring& new_value)
	{  
		while( true )
		{
			tstring::size_type pos(0);
			if( (pos=str.find(old_value)) != tstring::npos )
				str.replace(pos,old_value.length(),new_value);
			else
				break;
		}  
		return str;
	}
}

//-----------------------------------------------------------------------------
// 构造函数
//-----------------------------------------------------------------------------
QuestMainFrame::QuestMainFrame(void)
: m_Trunk(this)
, m_pWnd(0)
, m_pTreeTaskList(0)
, m_pBtnQuestNpc(0)
, m_pBtnActiveQuest(0)
, m_pBtnQuestTrack(0)
, m_pBtnDelQuest(0)
, m_pBtnCloseWnd1(0)
, m_pBtnCloseWnd2(0)
, m_pStcCondition(0)
, m_pStcBonusExp(0)
, m_pStcBonusMoney1(0)
, m_pStcBonusMoney2(0)
, m_pSrlCondition(0)
, m_showNPC(false)
, m_showTrack(true)
{
	memset( m_BonusGoodsID, 0, sizeof(DWORD)*QUEST_REW_ITEM );
	memset( m_BonusSelsID, 0, sizeof(DWORD)*QUEST_REW_ITEM );
}

//-----------------------------------------------------------------------------
// 析构函数
//-----------------------------------------------------------------------------
QuestMainFrame::~QuestMainFrame(void)
{
	
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
BOOL QuestMainFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pStcCondition->SetText( g_StrTable[_T("Quest_None")] );

	// 注册游戏事件
	pMgr->RegisterEventHandle( _T("QuestMainFrameEvent_CloseTrack"),(FRAMEEVENTPROC)m_Trunk.sfp1(&QuestMainFrame::OnEvent_CloseTrack));
	pMgr->RegisterEventHandle( _T("QuestMainFrameEvent_CloseNPC"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&QuestMainFrame::OnEvent_CloseNPC));
	pMgr->RegisterEventHandle( _T("QuestMainFrameEvent_OpenNPC"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&QuestMainFrame::OnEvent_OpenNPC));
	pMgr->RegisterEventHandle( _T("QuestMainFrameEvent_CancelQuest"),(FRAMEEVENTPROC)m_Trunk.sfp1(&QuestMainFrame::OnEvent_CancelQuest));

	return TRUE;
}

//-----------------------------------------------------------------------------
// 销毁
//-----------------------------------------------------------------------------
BOOL QuestMainFrame::Destroy()
{
	// 注销游戏事件
	m_pMgr->UnRegisterEventHandler( _T("QuestMainFrameEvent_CloseTrack"),(FRAMEEVENTPROC)m_Trunk.sfp1(&QuestMainFrame::OnEvent_CloseTrack));
	m_pMgr->UnRegisterEventHandler( _T("QuestMainFrameEvent_CloseNPC"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&QuestMainFrame::OnEvent_CloseNPC));
	m_pMgr->UnRegisterEventHandler( _T("QuestMainFrameEvent_OpenNPC"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&QuestMainFrame::OnEvent_OpenNPC));
	m_pMgr->UnRegisterEventHandler( _T("QuestMainFrameEvent_CancelQuest"),(FRAMEEVENTPROC)m_Trunk.sfp1(&QuestMainFrame::OnEvent_CancelQuest));

	if( P_VALID(m_pWnd) )
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}

	return GameFrame::Destroy();
}

//-----------------------------------------------------------------------------
// 重新读入窗口界面
//-----------------------------------------------------------------------------
BOOL QuestMainFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\tasklist.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &element, "task\\taskwin\\taskpic\\tasklistpic\\tasklist", "ClassName", "AppliedTree");
	m_pGUI->ChangeXml( &element, "task\\taskwin\\taskpic\\encpic\\itempic1\\item1", "ClassName", "IconStatic" );
	m_pGUI->ChangeXml( &element, "task\\taskwin\\taskpic\\encpic\\itempic2\\item2", "ClassName", "IconStatic" );
	m_pGUI->ChangeXml( &element, "task\\taskwin\\taskpic\\encpic\\itempic3\\item3", "ClassName", "IconStatic" );
	m_pGUI->ChangeXml( &element, "task\\taskwin\\taskpic\\encpic\\itempic4\\item4", "ClassName", "IconStatic" );
	m_pGUI->ChangeXml( &element, "task\\taskwin\\taskpic\\encpic\\itempic5\\item5", "ClassName", "IconStatic" );
	m_pGUI->ChangeXml( &element, "task\\taskwin\\taskpic\\encpic\\itempic6\\item6", "ClassName", "IconStatic" );
	m_pGUI->ChangeXml( &element, "task\\taskwin\\taskpic\\encpic\\itempic7\\item7", "ClassName", "IconStatic" );
	m_pGUI->ChangeXml( &element, "task\\taskwin\\taskpic\\encpic\\itempic8\\item8", "ClassName", "IconStatic" );
	m_pGUI->ChangeXml( &element, "task\\taskwin\\taskpic\\encpic\\itempic9\\item9", "ClassName", "IconStatic" );
	m_pGUI->ChangeXml( &element, "task\\taskwin\\taskpic\\encpic\\itempic10\\item10", "ClassName", "IconStatic" );

	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	if( !P_VALID( m_pWnd ) )
		return FALSE;

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&QuestMainFrame::EventHandler));

	m_pTreeTaskList		= (AppliedTree*)	m_pWnd->GetChild(_T("taskwin\\taskpic\\tasklistpic\\tasklist"));
	m_pBtnQuestNpc		= (GUIButton*)		m_pWnd->GetChild(_T("taskwin\\npclistbutt"));
	m_pBtnActiveQuest	= (GUIButton*)		m_pWnd->GetChild(_T("taskwin\\dtaskbutt"));
	m_pBtnQuestTrack	= (GUIButton*)		m_pWnd->GetChild(_T("taskwin\\trackbutt"));
	m_pBtnDelQuest		= (GUIButton*)		m_pWnd->GetChild(_T("taskwin\\delbutt"));
	m_pBtnCloseWnd1		= (GUIButton*)		m_pWnd->GetChild(_T("taskwin\\closebutt1"));
	m_pBtnCloseWnd2		= (GUIButton*)		m_pWnd->GetChild(_T("taskwin\\closebutt"));
	m_pStcCondition		= (GUIStaticEx*)	m_pWnd->GetChild(_T("taskwin\\taskpic\\conwinpic\\contentwnd\\content"));
	m_pStcBonusExp		= (GUIStatic*)		m_pWnd->GetChild(_T("taskwin\\taskpic\\encpic\\encnum1"));
	m_pStcBonusMoney1	= (GUIStatic*)		m_pWnd->GetChild(_T("taskwin\\taskpic\\encpic\\monum1"));
	m_pStcBonusMoney2	= (GUIStatic*)		m_pWnd->GetChild(_T("taskwin\\taskpic\\encpic\\monum2"));
	m_pSrlCondition		= (GUIScrollBar*)	m_pWnd->GetChild(_T("taskwin\\taskpic\\conwinpic\\contentwnd\\scr2"));

	GUIStatic *pStatic	= (GUIStatic*)		m_pWnd->GetChild(_T("taskwin\\taskpic\\encpic\\eword5"));
	m_pStcBonusSpecT.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("taskwin\\taskpic\\encpic\\eword6"));
	m_pStcBonusSpecT.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("taskwin\\taskpic\\encpic\\eword2"));
	m_pStcBonusSpecT.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("taskwin\\taskpic\\encpic\\eword4"));
	m_pStcBonusSpecT.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("taskwin\\taskpic\\encpic\\necnum4"));
	m_pStcBonusSpecV.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("taskwin\\taskpic\\encpic\\encnum5"));
	m_pStcBonusSpecV.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("taskwin\\taskpic\\encpic\\encnum2"));
	m_pStcBonusSpecV.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("taskwin\\taskpic\\encpic\\encnum3"));
	m_pStcBonusSpecV.push_back( pStatic );

	for( int i = 0; i < 5 ; i++ )
	{
		TCHAR szTmp[255];

		_stprintf(szTmp, _T("taskwin\\taskpic\\encpic\\itempic%d\\item%d"), i+1, i+1);
		IconStatic *pBonusGood = (IconStatic*)m_pWnd->GetChild( szTmp );
		m_stcBonusGoods.push_back( pBonusGood );
		pBonusGood->SetInvisible(TRUE);

		_stprintf(szTmp, _T("taskwin\\taskpic\\encpic\\itempic%d\\item%d"), i+6, i+6);
		IconStatic *pBonusSel = (IconStatic*)m_pWnd->GetChild( szTmp );
		m_stcBonusSels.push_back( pBonusSel );
		pBonusSel->SetInvisible(TRUE);
	}

	m_pWnd->SetInvisible( TRUE );
	m_showNPC = true;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 界面消息事件处理
//-----------------------------------------------------------------------------
BOOL QuestMainFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch( pEvent->eEvent )
	{
	case EGUIE_Active:
		{
			ShowTip(pWnd, NULL);
			Show();
		}
		break;

	case EGUIE_Drag:
		{
			QuestNPCFrame *pFrame = QuestMgr::Inst()->GetNPCFrame(_T("QuestNPCQuestMain"));
			if( P_VALID(pFrame) )
			{
				pFrame->Align();
			}
		}
		break;

	case EGUIE_Click:
		{
			if( pWnd == m_pBtnQuestNpc )			// 显示/隐藏可接任务NPC窗口
			{
				m_showNPC = !m_showNPC;
				if( m_showNPC )
					QuestMgr::Inst()->OpenNPCFrame( m_strName.c_str(), _T("QuestNPCQuestMain"), "\\desktop\\task\\taskwin", "npclist_quest" );
				else
					QuestMgr::Inst()->CloseNPCFrame( _T("QuestNPCQuestMain") );
			}
			else if( pWnd == m_pBtnActiveQuest )	// 显示/隐藏活动任务窗口
			{
				QuestMgr::Inst()->OpenActiveFrame();
			}
			else if( pWnd == m_pBtnQuestTrack )		// 显示/隐藏任务追踪窗口
			{
				m_showTrack = !m_showTrack;
				if( m_showTrack )
					QuestMgr::Inst()->OpenTrackFrame();
				else
					QuestMgr::Inst()->CloseTrackFrame();
			}
			else if( pWnd == m_pBtnDelQuest )		// 删除任务
			{
				QuestData *pData = QuestMgr::Inst()->GetData();
				ASSERT( P_VALID(pData) );

				DWORD treeID = m_pTreeTaskList->GetCurrentSelItem();
				tstring questName;
				pData->GetQuestName(m_tree2quest[treeID], questName);
				TCHAR szText[128]={0};
				_stprintf( szText, g_StrTable[_T("QuestMain_CancelQuest")], questName.c_str() );
				m_pMsgBox->Init( _T(""),
					szText,
					_T("QuestMainFrameEvent_CancelQuest"),
					(MsgBoxFlag)(MBF_OK|MBF_Cancel),
					TRUE);
			}
			else if( pWnd == m_pBtnCloseWnd1 || pWnd == m_pBtnCloseWnd2 )// 关闭主窗口
			{
				ShowTip(pWnd, NULL);
				m_pMgr->SendEvent( &tagGameEvent( _T("Close_Task_UI"), this ) );
			}
		}
		break;

	case EGUIE_StaticExLinkClick:
		{
			// [<link=linksIndex,0xffffffff>玄天龙城<link=0xffffffff,0>]
			const TCHAR *szLink = (const TCHAR*)(pEvent->dwParam2);
			Vector3 pos;
			QuestData *pData = QuestMgr::Inst()->GetData();
			ASSERT( P_VALID(pData) );

			if( (INT)pEvent->dwParam1 < 1000 )//根据ID寻路
			{
				INT index = pEvent->dwParam1-1;
				if( index > (INT)m_idLinks.size()-1 )
					break;

				tagIDLink& link = m_idLinks[index];
				IMSG( _T("path map: %s typeid: %d\n"), link.mapName.c_str(), link.typeID );

				if( link.mapName != MapMgr::Inst()->GetCurMapName() )//不在本地图，进行跨场景寻路
				{
					if( QuestMgr::Inst()->MoveToMap(link.mapName.c_str(),pos) != TRUE )
						break;
				}
				else
				{
					if( link.typeID == 1 )//触发器或者其他没有TypeID的东西
					{
						if( !pData->GetTriggerPosition( szLink, pos ) )
							break;
					}
					else
					{
						QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
						ASSERT( P_VALID(pQuery) );
						if( !pQuery->GetNPCPosition( link.typeID, pos ) )
							break;
					}
				}
			}
			else//根据坐标寻路
			{
				INT index = pEvent->dwParam1-1000;
				if( index > (INT)m_posLinks.size()-1 )
					break;

				tagPosLink& link = m_posLinks[index];

				if( link.mapName != MapMgr::Inst()->GetCurMapName() )//不在本地图，进行跨场景寻路
				{
					if( QuestMgr::Inst()->MoveToMap(link.mapName.c_str(),pos) != TRUE )
						break;
				}
				else
				{
					pos = link.pos;
				}
			}

			LocalPlayer *pLP = RoleMgr::Inst()->GetLocalPlayer();

			tagMouseMoveEvent event( _T("tagMouseMoveEvent"), NULL );
			event.start = pLP->GetPos();
			event.end = pos;
			event.validDist = 100.0f;

			IMSG(_T("target pos: x-%f,y-%f,z-%f\n"),pos.x,pos.y,pos.z);

			m_pMgr->SendEvent( &event );
		}
		break;

	case EGUIE_ItemClick:
		{
			if( pWnd == m_pTreeTaskList )//选择某个任务
			{
				TreeQuestMap::iterator iter = m_tree2quest.find(pEvent->dwParam1);
				if( iter != m_tree2quest.end() )
				{
					if( m_pInput->IsKeyDown(DIK_RCONTROL) || m_pInput->IsKeyDown(DIK_LCONTROL) )//跟踪选中的任务
					{
						if( QuestMgr::Inst()->TrackQuest( iter->second ) )//任务可以进行跟踪
						{
							QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
							ASSERT( P_VALID(pQuery) );

							// 完成的不显示跟踪标志
							if( !pQuery->QuestCanComplete( iter->second ) )
							{
								tagGUITreeItem *pItem = m_pTreeTaskList->GetItem( iter->first );

								QuestData *pData = QuestMgr::Inst()->GetData();
								ASSERT( P_VALID(pData) );

								TCHAR szText[20];
								tstring questName;
								pData->GetQuestName( iter->second, questName );
								INT questLvl = pData->GetQuestLevel( iter->second );
								if( questLvl > 0 )
								{
									_stprintf(szText, _T("[%d]"), questLvl);
									questName = szText + questName;
								}

								if( pQuery->QuestIsTracked(iter->second) )
								{
									questName = questName + g_StrTable[_T("Quest_TrackFlag")];
								}

								pItem->strShow = questName;
							}
						}
					}
					//else // 显示任务信息
					{
						SetQuestDesc(iter->second);

						QuestData *pData = QuestMgr::Inst()->GetData();
						ASSERT( P_VALID(pData) );
						if( pData->QuestCanDelete(iter->second) )
							m_pBtnDelQuest->SetEnable(true);
						else
							m_pBtnDelQuest->SetEnable(false);
						m_pBtnDelQuest->SetRefresh();
						m_pBtnQuestTrack->SetEnable(true);
						m_pBtnQuestTrack->SetRefresh();
					}
				}
				else
				{
					m_pBtnDelQuest->SetEnable(false);
					m_pBtnDelQuest->SetRefresh();
					m_pBtnQuestTrack->SetEnable(false);
					m_pBtnQuestTrack->SetRefresh();
				}
			}
		}
		break;

	case EGUIE_MoveIntoCtrl:
		{
			for( int i=0; i<QUEST_REW_ITEM; i++ )
			{
				if( pWnd == (GUIWnd*)m_stcBonusGoods[i] )
				{
					if( m_BonusGoodsID[i] != 0 )
					{
						TreeQuestMap::iterator iter = m_tree2quest.find(m_pTreeTaskList->GetCurrentSelItem());
						if( iter != m_tree2quest.end() )
						{
							QuestData *pData = QuestMgr::Inst()->GetData();
							ASSERT( P_VALID(pData) );
							INT16 quality;
							pData->GetQuestItemQuality(iter->second,i,quality);
							ShowTip( pWnd, ToolTipCreator::Inst()->GetItemTips(m_BonusGoodsID[i],(BYTE)quality).c_str() );
						}
						break;
					}
				}

				if( pWnd == (GUIWnd*)m_stcBonusSels[i] )
				{
					if( m_BonusSelsID[i] != 0 )
					{
						TreeQuestMap::iterator iter = m_tree2quest.find(m_pTreeTaskList->GetCurrentSelItem());
						if( iter != m_tree2quest.end() )
						{
							QuestData *pData = QuestMgr::Inst()->GetData();
							ASSERT( P_VALID(pData) );
							INT16 quality;
							pData->GetQuestChoiceItemQuality(iter->second,i,quality);
							ShowTip( pWnd, ToolTipCreator::Inst()->GetItemTips(m_BonusSelsID[i],(BYTE)quality).c_str() );
						}
					}
				}
			}
		}
		break;

	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(pWnd, NULL);
		}
		break;

	default:
		break;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 删除任务
//-----------------------------------------------------------------------------
VOID QuestMainFrame::DeleteQuest( UINT16 questID )
{
	ASSERT( P_VALID(m_pWnd) );

	TreeQuestMap::iterator iter;
	for( iter = m_tree2quest.begin(); iter != m_tree2quest.end(); ++iter )
	{
		if( iter->second == questID )	// 删除此任务的Item
		{
			m_pTreeTaskList->RemoveItem( iter->first );
			m_tree2quest.erase(iter);
			break;
		}
	}

	if( m_tree2quest.empty() )	// 当前任务全部删除完毕后，需要在任务描述框中显示“没有正在进行的任务”
	{
		m_pStcCondition->SetText( g_StrTable[_T("Quest_None")] );
		m_pStcBonusExp->SetText(_T(""));
		m_pStcBonusMoney1->SetText(_T(""));
		m_pStcBonusMoney2->SetText(_T(""));

		for( size_t i = 0; i < m_pStcBonusSpecT.size(); i++ )
			m_pStcBonusSpecT[i]->SetText(_T(""));
		for( size_t i = 0; i < m_pStcBonusSpecV.size(); i++ )
			m_pStcBonusSpecV[i]->SetText(_T(""));
		for( UINT i=0; i<m_stcBonusGoods.size(); i++ )
		{
			m_stcBonusGoods[i]->SetInvisible(TRUE);
		}
		for( UINT i=0; i<m_stcBonusSels.size(); i++ )
		{
			m_stcBonusSels[i]->SetInvisible(TRUE);
		}
		m_pBtnDelQuest->SetEnable(true);
	}
	else
	{
		m_pTreeTaskList->SetCurrentSelItem(m_pTreeTaskList->GetCurrentSelItem(), TRUE);
	}
}

//-----------------------------------------------------------------------------
// 加入任务
//-----------------------------------------------------------------------------
VOID QuestMainFrame::AddQuest( UINT16 questID, bool sel )
{
	if( !P_VALID(m_pWnd) )
		return;

	if( m_tree2quest.empty() )	// 加入第一个任务时，需要做些额外处理，插入根节点
	{
		m_pTreeTaskList->InsterItem(0, _T("Root"), _T("根节点"));
	}

	QuestData *pData = QuestMgr::Inst()->GetData();
	ASSERT( P_VALID(pData) );

	TCHAR szText[20];
	tstring questName;
	pData->GetQuestName(questID,questName);
	INT questLvl = pData->GetQuestLevel(questID);
	if( questLvl > 0 )
	{
		_stprintf(szText, _T("[%d]"), questLvl);
		questName = szText + questName;
	}

	DWORD parentID = GT_INVALID;
	DWORD treeID = GT_INVALID;
	INT16 questType = 0;
	tstring strTypeName = _T("");
	pData->GetQuestType(questID, questType);
	pData->GetQuestTypeName(questType, strTypeName);

	TCHAR szTmp[X_SHORT_NAME];
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), questType);
	parentID = m_pTreeTaskList->InsterItem(_T("\\Root"), szTmp, strTypeName.c_str(), TRUE);
	_stprintf( szText, _T("%d_%d"), questID, questLvl );
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("\\Root\\%d"), questType);
	treeID = m_pTreeTaskList->InsterItem(szTmp, szText, questName.c_str(), TRUE );

	// 加入任务失败
	if( !P_VALID(treeID) )
		return;

	// 设置颜色
	DWORD color = 0xFFFFF7E0;
	pData->GetQuestTypeColor(questID, color);
	tagGUITreeItem *pItem = NULL;
	if( P_VALID( parentID ) )
	{
		pItem = m_pTreeTaskList->GetItem( parentID );
		pItem->dwColor = color;
	}
	
	color = pData->GetQuestLevelColor( questID, RoleMgr::Inst()->GetLocalPlayer()->GetRoleLevel() );
	pItem = m_pTreeTaskList->GetItem( treeID );
	pItem->dwColor = color;

	QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
	ASSERT( P_VALID(pQuery) );
	if( pQuery->QuestCanComplete(questID) )
	{
		pItem->strShow += g_StrTable[_T("Quest_CompleteFlag")];
	}

	m_tree2quest[treeID] = questID;
	if( sel )
	{
		m_pTreeTaskList->SetCurrentSelItem(treeID, TRUE);
		tagGUIEvent event(m_pSrlCondition, EGUIE_Scroll);
		event.dwParam1 = (DWORD)0;
		event.dwParam2 = (DWORD)0;
		event.dwParam3 = (DWORD)0;
		m_pSrlCondition->SendEvent(&event);
		m_pSrlCondition->SetValue(0);
	}
}

//-----------------------------------------------------------------------------
// 根据玩家等级重新设置任务颜色
//-----------------------------------------------------------------------------
VOID QuestMainFrame::ChangeColor( void )
{
	_ChangeColor( m_pTreeTaskList->GetRoot() );

	m_pTreeTaskList->SetRefresh(TRUE);
}

VOID QuestMainFrame::_ChangeColor( tagGUITreeItem* pItem )
{
	if( pItem != NULL )
	{
		if( pItem->listChildren.empty() )//任务节点，开始变色
		{
			tstring strQuestID = pItem->strText;
			tstring::size_type pos(0);
			if( (pos=strQuestID.find(_T("_"))) != tstring::npos )
			{
				strQuestID.replace(pos,strQuestID.length()-pos,_T(""));
				UINT16 questID = (UINT16)_tstoi(strQuestID.c_str());

				QuestData *pData = QuestMgr::Inst()->GetData();
				ASSERT( P_VALID(pData) );

				pItem->dwColor = pData->GetQuestLevelColor( questID, RoleMgr::Inst()->GetLocalPlayer()->GetRoleLevel() );
			}
		}
		else//继续迭代
		{
			std::list<tagGUITreeItem*>::iterator iter;
			for( iter=pItem->listChildren.begin(); iter!=pItem->listChildren.end(); ++iter )
			{
				_ChangeColor( *iter );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// 显示窗口
//-----------------------------------------------------------------------------
VOID QuestMainFrame::Show( bool bReset )
{
	if( P_VALID(m_pWnd) ) 
	{
		if( bReset )
		{
			tagGUIEvent event(m_pSrlCondition, EGUIE_Scroll);
			event.dwParam1 = (DWORD)0;
			event.dwParam2 = (DWORD)0;
			event.dwParam3 = (DWORD)0;
			m_pSrlCondition->SendEvent(&event);
			m_pSrlCondition->SetValue(0);
		}

		m_pWnd->SetInvisible(FALSE); 
		m_pWnd->FlipToTop();

		QuestNPCFrame *pFrame = QuestMgr::Inst()->GetNPCFrame(_T("QuestNPCQuestMain"));
		if( P_VALID(pFrame) )
			pFrame->Show();
		else if( m_showNPC )
			QuestMgr::Inst()->OpenNPCFrame( m_strName.c_str(), _T("QuestNPCQuestMain"), "\\desktop\\task\\taskwin", "npclist_quest" );
	}
}

//-----------------------------------------------------------------------------
// 隐藏窗口
//-----------------------------------------------------------------------------
VOID QuestMainFrame::Hide( void )
{
	if( P_VALID(m_pWnd) ) 
		m_pWnd->SetInvisible(TRUE);

	QuestMgr::Inst()->CloseNPCFrame( _T("QuestNPCQuestMain"), true );
}

//-----------------------------------------------------------------------------
// 窗口是否可见
//-----------------------------------------------------------------------------
BOOL QuestMainFrame::IsVisible( void ) const
{
	if( P_VALID(m_pWnd) )
	{
		return !m_pWnd->IsInvisible();
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 显示某个任务的描述信息
//-----------------------------------------------------------------------------
VOID QuestMainFrame::SetQuestDesc( UINT16 questID )
{
	tstring desc;
	QuestData *pData = QuestMgr::Inst()->GetData();
	ASSERT( P_VALID(pData) && _T("任务系统获得QuestData错误") );

	const tagQuestProto* pQuestProto = pData->GetQuestProto( questID );
	ASSERT( P_VALID(pQuestProto) );

	tstringstream stream;
	//stream << g_StrTable[_T("Quest_DescAcceptConditions")]
	//	<< _T("\\n    ") << pQuestProto->conditions;

	// 解析over_view
	tstring strView = pQuestProto->over_view;
	QuestMgr::Inst()->ParseQuestView(strView,m_idLinks,m_posLinks);
	
	stream << g_StrTable[_T("Quest_DescOverview")] << _T("\\n    ") << strView;

	if( pData->QuestRequireItemOrCreature(questID) )
		stream << _T("\\n \\n")	<< g_StrTable[_T("Quest_DescCompleteConditions")] << _T("\\n");

	QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
	ASSERT( P_VALID(pQuery) && _T("QuestMainFrame获得QuestQuery错误") );

	const tagQuestDynamicTarget *pDynamicTarget = pData->GetQuestDynamicTarget(questID);
	if( P_VALID(pDynamicTarget) )//动态
	{
		UINT16 itemCount,creatureCount,itemSum,creatureSum;
		DWORD itemID,creatureID;

		switch( pDynamicTarget->eTargetType )
		{
		case EQTT_Kill:
			{
				// 怪物信息
				for( int i=0; i<1; i++ )
				{
					if( pData->GetQuestRequiredCreatureCount( questID , i, creatureID, creatureSum ) )
					{
						const tagCreatureProto* creatureProto = CreatureData::Inst()->FindNpcAtt(creatureID);
						ASSERT( creatureProto != NULL );

						TCHAR szCreatureID[15];
						_ui64tot(creatureID,szCreatureID,16);

						pQuery->GetQuestCreatureCount( questID , i, creatureCount );
						if( creatureCount > creatureSum )
						{
							creatureCount = creatureSum;
						}

						stream << _T("    ") << g_StrTable[_T("Quest_TrackKillCreatureDesc")] << _T(" ")
							<< creatureProto->szName << _T(" ") << creatureCount << _T("/") << creatureSum << _T("\\n");
					}
				}
			}
			break;
		case EQTT_Collect:
			{
				// 物品信息
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pData->GetQuestRequiredItemCount( questID , i, itemID, itemSum ) )
					{
						const tagItemDisplayInfo* itemProto = ItemProtoData::Inst()->FindItemDisplay(itemID);
						ASSERT( itemProto != NULL );

						TCHAR szItemID[15];
						_ui64tot(itemID,szItemID,16);

						pQuery->GetQuestItemCount( questID , i, itemCount );
						if( itemCount > itemSum )
						{
							itemCount = itemSum;
						}

						stream << _T("    ") << g_StrTable[_T("Quest_TrackGetItemDesc")] << _T(" ") 
							<< itemProto->szName << _T(" ") << itemCount << _T("/") << itemSum << _T("\\n");
					}
				}
			}
			break;
		}

		m_pStcBonusExp->SetText(_T("????"));
		m_pStcBonusMoney1->SetText(_T("????"));
		m_pStcBonusMoney2->SetText(_T("????"));

		//// 物品奖励
		//for( int i=0; i<QUEST_REW_ITEM; i++ )
		//{
		//	m_stcBonusGoods[i]->SetPic( _T("data\\ui\\Mission\\L_item_unknown.tga") );
		//	m_BonusGoodsID[i] = 0;

		//	m_stcBonusGoods[i]->SetInvisible(FALSE);
		//}

		//// 可选物品奖励
		//for( int i=0; i<QUEST_REW_ITEM; i++ )
		//{
		//	m_stcBonusSels[i]->SetPic( _T("data\\ui\\Mission\\L_item_unknown.tga") );
		//	m_BonusSelsID[i] = pQuestProto->rew_choice_item[i];

		//	m_stcBonusSels[i]->SetInvisible(FALSE);
		//}
	}
	else
	{
		UINT16 itemCount,creatureCount,itemSum,creatureSum;
		DWORD itemID,creatureID;

		// 物品信息
		for( int i=0; i<QUEST_ITEMS_COUNT; i++ )
		{
			if( pData->GetQuestRequiredItemCount( questID , i, itemID, itemSum ) )
			{
				const tagItemDisplayInfo* itemProto = ItemProtoData::Inst()->FindItemDisplay(itemID);
				ASSERT( itemProto != NULL );

				TCHAR szItemID[15];
				_ui64tot(itemID,szItemID,16);

				pQuery->GetQuestItemCount( questID , i, itemCount );
				if( itemCount > itemSum )
				{
					itemCount = itemSum;
				}

				stream << _T("    ") << g_StrTable[_T("Quest_TrackGetItemDesc")] << _T(" ") 
					<< itemProto->szName << _T(" ") << itemCount << _T("/") << itemSum << _T("\\n");
			}
		}

		// 怪物信息
		for( int i=0; i<QUEST_CREATURES_COUNT; i++ )
		{
			if( pData->GetQuestRequiredCreatureCount( questID , i, creatureID, creatureSum ) )
			{
				const tagCreatureProto* creatureProto = CreatureData::Inst()->FindNpcAtt(creatureID);
				ASSERT( creatureProto != NULL );

				TCHAR szCreatureID[15];
				_ui64tot(creatureID,szCreatureID,16);

				pQuery->GetQuestCreatureCount( questID , i, creatureCount );
				if( creatureCount > creatureSum )
				{
					creatureCount = creatureSum;
				}

				stream << _T("    ") << g_StrTable[_T("Quest_TrackKillCreatureDesc")] << _T(" ")
					<< creatureProto->szName << _T(" ") << creatureCount << _T("/") << creatureSum << _T("\\n");
			}
		}

		// 奖励
		TCHAR szRewXP[10];
		_ultot(pQuestProto->rew_xp, szRewXP, 10);
		m_pStcBonusExp->SetText( szRewXP );

		TCHAR szRewGold[5],szRewSilver[5];
		_ultot( MSilver2DBGold(pQuestProto->rew_money), szRewGold, 10 );
		_ultot( MSilver2DBSilver(pQuestProto->rew_money), szRewSilver, 10 );
		m_pStcBonusMoney1->SetText( szRewGold );
		m_pStcBonusMoney2->SetText( szRewSilver );
	}

	// 物品奖励
	for( int i=0; i<QUEST_REW_ITEM; i++ )
	{
		if( pQuestProto->rew_item[i] != 0 )
		{
			tstring strPic = ItemMgr::Inst()->GetItemIconPath( pQuestProto->rew_item[i] );
			m_stcBonusGoods[i]->SetPic( strPic );

			TCHAR szNum[5];
			_stprintf(szNum, _T("%4d"), pQuestProto->rew_item_num[i]);
			m_stcBonusGoods[i]->SetText( szNum );
			m_BonusGoodsID[i] = pQuestProto->rew_item[i];

			m_stcBonusGoods[i]->SetInvisible(FALSE);
		}
		else
		{
			m_BonusGoodsID[i] = 0;
			m_stcBonusGoods[i]->SetInvisible(TRUE);
		}
	}

	// 可选物品奖励
	for( int i=0; i<QUEST_REW_ITEM; i++ )
	{
		if( pQuestProto->rew_choice_item[i] != 0 )
		{
			tstring strPic = ItemMgr::Inst()->GetItemIconPath( pQuestProto->rew_choice_item[i] );
			m_stcBonusSels[i]->SetPic( strPic );

			TCHAR szNum[5];
			_stprintf(szNum, _T("%4d"), pQuestProto->rew_choice_item_num[i]);
			m_stcBonusSels[i]->SetText( szNum );
			m_BonusSelsID[i] = pQuestProto->rew_choice_item[i];

			m_stcBonusSels[i]->SetInvisible(FALSE);
		}
		else
		{
			m_BonusSelsID[i] = 0;
			m_stcBonusSels[i]->SetInvisible(TRUE);
		}
	}

	m_pStcCondition->SetText( stream.str().c_str() );
	tagPoint size = m_pStcCondition->GetSize();
	if( m_pStcCondition->GetTextTotalHeight() > size.y )
	{
		size.y = m_pStcCondition->GetTextTotalHeight();
		m_pStcCondition->SetResizable(TRUE);
		m_pStcCondition->Resize( size );
		m_pStcCondition->SetResizable(FALSE);
	}

	// 声望、贡献、属性、技能、buff
	QuestMgr::Inst()->SetSpeRew(pQuestProto,m_pStcBonusSpecT,m_pStcBonusSpecV);
}

VOID QuestMainFrame::SetQuestCompleteFlag( UINT16 questID )
{
	TreeQuestMap::const_iterator iter;
	for( iter = m_tree2quest.begin(); iter != m_tree2quest.end(); ++iter )
	{
		if( iter->second == questID )
		{
			tagGUITreeItem *pItem = m_pTreeTaskList->GetItem( iter->first );

			QuestData *pData = QuestMgr::Inst()->GetData();
			ASSERT( P_VALID(pData) );

			tstring questName;
			TCHAR szLvl[20];
			pData->GetQuestName( iter->second, questName );
			INT questLvl = pData->GetQuestLevel(questID);
			if( questLvl > 0 )
			{
				_stprintf(szLvl, _T("[%d]"), questLvl);
				questName = szLvl + questName;
			}
			pItem->strShow = questName + g_StrTable[_T("Quest_CompleteFlag")];
		}
	}
}

VOID QuestMainFrame::SetQuestTrackFlag( UINT16 questID )
{
	TreeQuestMap::const_iterator iter;
	for( iter = m_tree2quest.begin(); iter != m_tree2quest.end(); ++iter )
	{
		if( iter->second == questID )
		{
			tagGUITreeItem *pItem = m_pTreeTaskList->GetItem( iter->first );

			QuestData *pData = QuestMgr::Inst()->GetData();
			ASSERT( P_VALID(pData) );

			tstring questName;
			TCHAR szLvl[20];
			pData->GetQuestName( iter->second, questName );
			INT questLvl = pData->GetQuestLevel(questID);
			if( questLvl > 0 )
			{
				_stprintf(szLvl, _T("[%d]"), questLvl);
				questName = szLvl + questName;
			}
			pItem->strShow = questName + g_StrTable[_T("Quest_TrackFlag")];
		}
	}
}

VOID QuestMainFrame::UpdateQuestDesc( UINT16 questID )
{
	if( questID == m_tree2quest[ m_pTreeTaskList->GetCurrentSelItem() ] )
	{
		SetQuestDesc( questID );
	}
}

DWORD QuestMainFrame::OnEvent_CloseTrack( tagGameEvent* pEvent )
{
	m_showTrack = false;

	return 0;
}

DWORD QuestMainFrame::OnEvent_CloseNPC( tagGameEvent* pEvent )
{
	m_showNPC = false;

	return 0;
}

DWORD QuestMainFrame::OnEvent_OpenNPC(tagGameEvent* pEvent)
{
	m_showNPC = true;
	return 0;
}


DWORD QuestMainFrame::OnEvent_CancelQuest( tagMsgBoxEvent* pEvent )
{
	if(MBF_OK == pEvent->eResult)
	{
		DWORD treeID = m_pTreeTaskList->GetCurrentSelItem();
		QuestMgr::Inst()->SendDeleteQuest(m_tree2quest[treeID]);
	}

	return 0;
}

BOOL QuestMainFrame::EscCancel()
{
	if( P_VALID(m_pWnd) && !m_pWnd->IsInvisible() )
	{
		ShowTip(m_pWnd, NULL);
		m_pMgr->SendEvent( &tagGameEvent( _T("Close_Task_UI"), this ) );

		return TRUE;
	}

	return GameFrame::EscCancel();
}

VOID QuestMainFrame::InitFocus( void )
{
	if( !P_VALID(m_pWnd) )
		return;

	if( !m_tree2quest.empty() )
	{
		DWORD dwCrc32 = m_pUtil->Crc32(_T("\\Root"));
		tagGUITreeItem *pItem = m_pTreeTaskList->GetItem(dwCrc32);
		while( !pItem->listChildren.empty() )
		{
			pItem = *(pItem->listChildren.begin());
		}

		m_pTreeTaskList->SetCurrentSelItem( m_pUtil->Crc32( pItem->strName.c_str() ), TRUE );
		tagGUIEvent event(m_pSrlCondition, EGUIE_Scroll);
		event.dwParam1 = (DWORD)0;
		event.dwParam2 = (DWORD)0;
		event.dwParam3 = (DWORD)0;
		m_pSrlCondition->SendEvent(&event);
		m_pSrlCondition->SetValue(0);
	}
}