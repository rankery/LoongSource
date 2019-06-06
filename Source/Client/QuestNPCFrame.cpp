/********************************************************************
	created:	2008/08/16
	created:	16:8:2008   18:06
	filename: 	d:\loong_client\Client\QuestNPCFrame.cpp
	file path:	d:\loong_client\Client
	file base:	QuestNPCFrame
	file ext:	cpp
	author:		leili
	
	purpose:	可接任务NPC界面实现
*********************************************************************/
#include "StdAfx.h"
#include "QuestNPCFrame.h"
#include "QuestMgr.h"
#include "CreatureData.h"
#include "CombatEvent.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "UIEvent.h"
#include "QuestFilterFrame.h"
#include "ChatFrame.h"

//-----------------------------------------------------------------------------
// 构造函数
//-----------------------------------------------------------------------------
QuestNPCFrame::QuestNPCFrame( void )
: m_Trunk(this)
, m_pWnd(0)
, m_pListNPCs(0)
, m_pBtnCloseWnd(0)
, m_curRow(0)
{

}

//-----------------------------------------------------------------------------
// 析构函数
//-----------------------------------------------------------------------------
QuestNPCFrame::~QuestNPCFrame( void )
{
	
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
BOOL QuestNPCFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle(_T("quest_filter_config"), (FRAMEEVENTPROC)m_Trunk.sfp1(&QuestNPCFrame::OnEventQuestFilterConfigChange));

	return TRUE;
}

//-----------------------------------------------------------------------------
// 销毁
//-----------------------------------------------------------------------------
BOOL QuestNPCFrame::Destroy()
{
	GameFrame::Destroy();
	m_pMgr->UnRegisterEventHandler(_T("quest_filter_config"), (FRAMEEVENTPROC)m_Trunk.sfp1(&QuestNPCFrame::OnEventQuestFilterConfigChange));
	QuestData *pQD = QuestMgr::Inst()->GetData();
	QuestData::EFilter eFilter = pQD->GetCurFilter();
	if (eFilter != QuestData::EF_NoFilter)
		pQD->SetLastFilter(eFilter);

	if( P_VALID(m_pWnd) )
		m_pGUI->AddToDestroyList(m_pWnd);


	return TRUE;
}

//-----------------------------------------------------------------------------
// 重新读入窗口界面
//-----------------------------------------------------------------------------
BOOL QuestNPCFrame::ReloadUI()
{
	return TRUE;
}

//-----------------------------------------------------------------------------
// 设置对齐界面
//-----------------------------------------------------------------------------
BOOL QuestNPCFrame::ReloadUI( LPCSTR alignCtrl, LPCSTR alignType, LPCSTR szNewName/* = NULL*/ )
{
	XmlElement element;
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\npclist.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &element, "npclistwin", "AlignControl", alignCtrl);
	m_pGUI->ChangeXml( &element, "npclistwin", "AlignType", alignType);
	if( NULL != szNewName )
		m_pGUI->ChangeXml( &element, "npclistwin", "Name", szNewName );

	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	if( !P_VALID( m_pWnd ) )
		return FALSE;

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&QuestNPCFrame::EventHandler));
	
	


	m_pListNPCs		= (GUIListBox*) m_pWnd->GetChild(_T("npclistpic\\npclist"));
	m_pBtnCloseWnd	= (GUIButton*)	m_pWnd->GetChild(_T("npclistpic\\npctitle\\closebutt"));
	m_pPbnFilterByLevel = (GUIPushButton*)m_pWnd->GetChild(_T("npclistpic\\pic\\opbutt1"));
	m_pPbnFilterByType = (GUIPushButton*)m_pWnd->GetChild(_T("npclistpic\\pic\\opbutt2"));
	m_pPbnType1 = (GUIPushButton*)m_pWnd->GetChild(_T("npclistpic\\pic\\frm\\opbutt1"));
	m_pPbnType2 = (GUIPushButton*)m_pWnd->GetChild(_T("npclistpic\\pic\\frm\\opbutt2"));
	m_pStcType1 = (GUIStatic*)m_pWnd->GetChild(_T("npclistpic\\pic\\word3"));
	m_pStcType2 = (GUIStatic*)m_pWnd->GetChild(_T("npclistpic\\pic\\word4"));
	m_pBtnConfig = (GUIButton*)m_pWnd->GetChild(_T("npclistpic\\butt"));

	m_pListNPCs->SetColNum(1, 39);

	m_pWnd->SetInvisible( TRUE );

	QuestData::EFilter eFilter = QuestMgr::Inst()->GetData()->GetCurFilter();
	QuestData::EFilter eLastFilter = QuestMgr::Inst()->GetData()->GetLastFilter();
	if (eFilter == QuestData::EF_NoFilter)
	{
		
		if (eLastFilter == QuestData::EF_CustomFilter1)
			m_pPbnType1->SetState(EGUIBS_PushDown);
		if (eLastFilter == QuestData::EF_CustomFilter2)
			m_pPbnType2->SetState(EGUIBS_PushDown);
		m_pPbnFilterByType->SetState(EGUIBS_PopUp);


	}
	else
	{
		
		if (eFilter == QuestData::EF_CustomFilter1)
			m_pPbnType1->SetState(EGUIBS_PushDown);
		if (eFilter == QuestData::EF_CustomFilter2)
			m_pPbnType2->SetState(EGUIBS_PushDown);
		m_pPbnFilterByType->SetState(EGUIBS_PushDown);

	}
	
	if (QuestMgr::Inst()->GetData()->IsFilterByPlayerLevel())
	{
		m_pPbnFilterByLevel->SetState(EGUIBS_PushDown);
	}
	else
	{
		m_pPbnFilterByLevel->SetState(EGUIBS_PopUp);
	}


	UpdateUI();

	UpdateNPCList();
	


	if (QuestMgr::Inst()->GetData()->IsFilterByPlayerLevel() == TRUE || QuestMgr::Inst()->GetData()->GetCurFilter() != QuestData::EF_NoFilter)
	{
		if (RoleMgr::Inst()->GetLocalPlayer()->GetRoleLevel() > 10)
		{
			if (m_optflags.size() < 5)
			{
				ChatFrame *pChatFrame = (ChatFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Chat"));
				if( P_VALID(pChatFrame) )
				{
					pChatFrame->PushInfo( g_StrTable[_T("RoleChat_Questfileter")], ESCC_System );
				}
			}
		}
	}



	return TRUE;
}

//-----------------------------------------------------------------------------
// 界面消息事件处理
//-----------------------------------------------------------------------------
BOOL QuestNPCFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnCloseWnd )//关闭窗口
			{
				QuestMgr::Inst()->CloseNPCFrame(m_strName.c_str());
			}
			else if (pWnd == m_pBtnConfig)
			{
				QuestFilterFrame *pFrame = (QuestFilterFrame*)m_pMgr->GetFrame(_T("QuestFilter"));
				if (P_VALID(pFrame))
					pFrame->Show(TRUE);
			}
		}
		break;

	case EGUIE_ItemDblClick:
		{
			if( pWnd == m_pListNPCs )
			{
				if( m_curRow > pEvent->dwParam1 )
				{
					DWORD& npcID=m_optflags[pEvent->dwParam1];
					QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
					ASSERT( P_VALID(pQuery) );
					Vector3 npcPos;
					if( pQuery->GetNPCPosition( npcID, npcPos ) )
					{
						LocalPlayer *pLP = RoleMgr::Inst()->GetLocalPlayer();

						tagMouseMoveEvent event( _T("tagMouseMoveEvent"), NULL );
						event.start = pLP->GetPos();
						event.end = npcPos;
						event.validDist = 100.0f;
						m_pMgr->SendEvent( &event );
					}
				}
			}
		}
		break;

	case EGUIE_ItemClick:
		{
			if( pWnd == m_pListNPCs )
			{
				if( m_curRow > pEvent->dwParam1 )
				{
					DWORD& npcID=m_optflags[pEvent->dwParam1];
					QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
					ASSERT( P_VALID(pQuery) );
					Vector3 npcPos;
					if( pQuery->GetNPCPosition( npcID, npcPos ) )
					{
						tagSetFlagEvent event;
						event.x = npcPos.x;
						event.z = npcPos.z;

						m_pMgr->SendEvent( &event );
					}
				}
			}
		}
		break;
	case EGUIE_PushButtonDown:
		{
			// 点击按类型过滤任务
			if (pWnd == m_pPbnFilterByType)
			{	
				// 按下
				if (pEvent->dwParam1 == 1)
				{
					QuestData::EFilter eLastFilter = QuestMgr::Inst()->GetData()->GetLastFilter();
					QuestMgr::Inst()->GetData()->SetCurFilter(eLastFilter);
					UpdateNPCList();
				}
				// 弹起
				else if (pEvent->dwParam1 == 2)
				{
					QuestData::EFilter eFilter = QuestMgr::Inst()->GetData()->GetCurFilter();
					QuestMgr::Inst()->GetData()->SetLastFilter(eFilter);
					QuestMgr::Inst()->GetData()->SetCurFilter(QuestData::EF_NoFilter);
					
					UpdateNPCList();
				}
			}
			else if (pWnd == m_pPbnType1)
			{
				if (pEvent->dwParam1 == 1)
				{
					if (m_pPbnFilterByType->GetState() == EGUIBS_PushDown)
					{
						QuestMgr::Inst()->GetData()->SetCurFilter(QuestData::EF_CustomFilter1);
						UpdateNPCList();
					}
					else
					{
						QuestMgr::Inst()->GetData()->SetLastFilter(QuestData::EF_CustomFilter1);
					}
				}
			}
			else if (pWnd == m_pPbnType2)
			{
				if (pEvent->dwParam1 == 1)
				{
					if (m_pPbnFilterByType->GetState() == EGUIBS_PushDown)
					{
						QuestMgr::Inst()->GetData()->SetCurFilter(QuestData::EF_CustomFilter2);
						UpdateNPCList();
					}
					else
					{
						QuestMgr::Inst()->GetData()->SetLastFilter(QuestData::EF_CustomFilter2);
					}
				}
			}
			else if (pWnd == m_pPbnFilterByLevel)
			{
				if (pEvent->dwParam1 == 1)
				{
					QuestMgr::Inst()->GetData()->SetFilterByPlayerLevel(TRUE);
					UpdateNPCList();
				}
				else if (pEvent->dwParam1 == 2)
				{
					QuestMgr::Inst()->GetData()->SetFilterByPlayerLevel(FALSE);
					UpdateNPCList();
				}
			}

		}

	default:
		break;
	}

	return TRUE;
}

DWORD QuestNPCFrame::OnEventQuestFilterConfigChange( tagGameEvent *pEvent )
{
	UpdateUI();
	UpdateNPCList();

	return 0;
}


VOID QuestNPCFrame::Align( void )
{
	if( P_VALID( m_pWnd ) )
		m_pWnd->Align();
}

VOID QuestNPCFrame::UpdateNPCList( void )
{
	ASSERT( P_VALID(m_pWnd) && _T("主界面不存在") );

	m_pListNPCs->Clear();
	m_optflags.clear();
	m_curRow = 0;

	QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
	ASSERT( P_VALID(pQuery) );

	const QuestQuery::NPCSet npcs = pQuery->GetAcceptQuestNPCs();
	QuestQuery::NPCSet::const_iterator iter;
	for( iter=npcs.begin(); iter!=npcs.end(); ++iter )
	{
		const tagCreatureProto* creatureProto = CreatureData::Inst()->FindNpcAtt(*iter);
		ASSERT( P_VALID(creatureProto) );

		Vector3 targetPos;
		QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
		ASSERT( P_VALID(pQuery) );

		

		tstringstream strInfo;
		strInfo << creatureProto->szName;
		if( pQuery->GetNPCPosition( creatureProto->dwTypeID, targetPos ) )
		{
			LocalPlayer *pLP = RoleMgr::Inst()->GetLocalPlayer();
			Vector3 rolePos = pLP->GetPos();
			float dist = Vec3Dist(rolePos,targetPos);
			if( dist > 100000.0f )//>1000米，很远
			{
				strInfo << _T("(") << g_StrTable[_T("Quest_FarDist")] << _T(")");
			}
			else if( dist > 10000.0f )//>100米
			{
				strInfo << _T("(") << int(dist/50.0f) << _T(")");
			}
			else//附近
			{
				strInfo << _T("(") << g_StrTable[_T("Quest_NearDist")] << _T(")");
			}
		}

		BOOL bAddToList = FALSE;
		// 取得当前NPC的任务列表
		QuestQuery *pQQ = QuestMgr::Inst()->GetQuery();
		QuestData *pQD = QuestMgr::Inst()->GetData();
		vector<UINT16> quests;
		pQQ->GetNPCAcceptQuests(*iter, quests);
		vector<UINT16>::iterator iend = quests.end();
		for (vector<UINT16>::iterator iIter = quests.begin(); iIter != iend; ++iIter)
		{
			INT16 n16Category = pQD->GetQuestCategoryByQuestID(*iIter);
			// 如果有一个type在配置里面
			if (pQD->IsFiltered(n16Category) == TRUE)
			{
				bAddToList = TRUE;
				break;
			}
		}
		BOOL bAddToListByLv = TRUE;

		// 根据等级过滤
		if (pQD->IsFilterByPlayerLevel() == TRUE)
		{
			bAddToListByLv = FALSE;
			//取得玩家等级
			INT nLevel = RoleMgr::Inst()->GetLocalPlayer()->GetRoleLevel();
			for (vector<UINT16>::iterator iIter = quests.begin(); iIter != iend; ++iIter)
			{
				INT nQuestLevel = pQD->GetQuestLevel(*iIter);
				if (nQuestLevel != 0)
				{
					if (nQuestLevel >= nLevel - 1)
					{
						bAddToListByLv = TRUE;
						break;
					}
				}
				else
				{
					bAddToListByLv = TRUE;
					break;
				}
			}
		}
		

		if (bAddToList && bAddToListByLv)
		{
			m_pListNPCs->SetText(m_curRow++, 0, strInfo.str().c_str());
			m_optflags.push_back(*iter);
		}
	}
}

BOOL QuestNPCFrame::IsVisible( void ) const
{
	if( P_VALID( m_pWnd ) && !m_pWnd->IsInvisible() )
		return TRUE;
	else
		return FALSE;
}

VOID QuestNPCFrame::Show( void )
{
	if( P_VALID(m_pWnd) ) 
	{
		m_pWnd->SetInvisible(FALSE);
		m_pWnd->FlipToTop();
	}
}

VOID QuestNPCFrame::Hide( void )
{
	if( P_VALID(m_pWnd) ) 
		m_pWnd->SetInvisible(TRUE);
}

BOOL QuestNPCFrame::EscCancel()
{
	if( P_VALID(m_pWnd) && !m_pWnd->IsInvisible() )
	{
		QuestMgr::Inst()->CloseNPCFrame(m_strName.c_str());

		return TRUE;
	}

	return GameFrame::EscCancel();
}

VOID QuestNPCFrame::UpdateUI( void )
{
	QuestData *pQD = QuestMgr::Inst()->GetData();
	const QuestData::QuestFilterConfigMap& defFilter1 = pQD->GetFilter(QuestData::EF_DefFilter1);
	const QuestData::QuestFilterConfigMap& defFilter2 = pQD->GetFilter(QuestData::EF_DefFilter2);
	const QuestData::QuestFilterConfigMap& customFilter1 = pQD->GetFilter(QuestData::EF_CustomFilter1);
	const QuestData::QuestFilterConfigMap& customFilter2 = pQD->GetFilter(QuestData::EF_CustomFilter2);

	if (customFilter1 != defFilter1)
	{
		m_pStcType1->SetText(g_StrTable[_T("QuestTypeFilter_cus1")]);
		m_pStcType1->SetRefresh(TRUE);
	}
	else
	{
		m_pStcType1->SetText(g_StrTable[_T("QuestTypeFilter_def1")]);
		m_pStcType1->SetRefresh(TRUE);
	}
	if (customFilter2 != defFilter2)
	{
		m_pStcType2->SetText(g_StrTable[_T("QuestTypeFilter_cus2")]);
		m_pStcType2->SetRefresh(TRUE);
	}
	else
	{
		m_pStcType2->SetText(g_StrTable[_T("QuestTypeFilter_def2")]);
		m_pStcType2->SetRefresh(TRUE);
	}
}
