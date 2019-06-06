/********************************************************************
	created:	2008/08/15
	created:	15:8:2008   17:21
	filename: 	d:\loong_client\Client\QuestTalkFrame.cpp
	file path:	d:\loong_client\Client
	file base:	QuestTalkFrame
	file ext:	cpp
	author:		leili
	
	purpose:	任务对话窗口实现，处理对话窗口的界面逻辑
	*********************************************************************/
#include "StdAfx.h"
#include "QuestTalkFrame.h"
#include "QuestMgr.h"
#include "QuestNPCFrame.h"
#include "ItemMgr.h"
#include "GameClientDefine.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "CombatSysUtil.h"
#include "ItemProtoData.h"
#include "CreatureData.h"
#include "RoleMgr.h"
#include "..\WorldDefine\currency_define.h"

//-----------------------------------------------------------------------------
// 构造函数
//-----------------------------------------------------------------------------
QuestTalkFrame::QuestTalkFrame( void )
: m_Trunk(this)
, m_pWnd(0)
, m_pStcCondition(0)
, m_pStcBonusExp(0)
, m_pStcBonusMoney1(0)
, m_pStcBonusMoney2(0)
, m_pBtnCloseWnd(0)
, m_pBtnOK(0)
, m_pBtnCancel(0)
, m_pCaption(0)
, m_canSelect(false)
, m_hasRewChoicesItem(false)
, m_isTrigger(false)
, m_isDynamic(false)
{
	memset( m_selIndex, 0, sizeof(bool)*QUEST_REW_ITEM );
	memset( m_BonusGoodsID, 0, sizeof(DWORD)*QUEST_REW_ITEM );
	memset( m_BonusSelsID, 0, sizeof(DWORD)*QUEST_REW_ITEM );
}

//-----------------------------------------------------------------------------
// 析构函数
//-----------------------------------------------------------------------------
QuestTalkFrame::~QuestTalkFrame( void )
{
	
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
BOOL QuestTalkFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 销毁
//-----------------------------------------------------------------------------
BOOL QuestTalkFrame::Destroy()
{
	GameFrame::Destroy();

	if( P_VALID(m_pWnd) )
	{
		m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&QuestTalkFrame::EventHandler));
		m_pGUI->AddToDestroyList(m_pWnd);
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 重新读入窗口界面
//-----------------------------------------------------------------------------
BOOL QuestTalkFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\tasktalk.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic1", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic2", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic3", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic4", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic5", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic6", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic7", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic8", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic9", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic10", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic1\\item1", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic2\\item2", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic3\\item3", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic4\\item4", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic5\\item5", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic6\\item6", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic7\\item7", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic8\\item8", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic9\\item9", "ClassName", "IconStatic");
	m_pGUI->ChangeXml( &element, "ttalkwin\\ttalkpic\\tt_pic2\\encpic\\itempic10\\item10", "ClassName", "IconStatic");

	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	if( !P_VALID( m_pWnd ) )
		return FALSE;

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&QuestTalkFrame::EventHandler));

	m_pCaption			= (GUIPatch*)	m_pWnd->GetChild(_T("ttalkpic"));
	m_pStcCondition		= (GUIStaticEx*)m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\tt_conpic\\contentWnd\\tcontent"));
	m_pBtnCloseWnd		= (GUIButton*)	m_pWnd->GetChild(_T("ttalkpic\\ttclosebutt"));
	m_pBtnOK			= (GUIButton*)	m_pWnd->GetChild(_T("ttalkpic\\npclistbutt"));
	m_pBtnCancel		= (GUIButton*)	m_pWnd->GetChild(_T("ttalkpic\\closebutt"));

	m_pStcBonusExp		= (GUIStatic*)	m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\encpic\\encnum1"));
	m_pStcBonusMoney1	= (GUIStatic*)	m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\encpic\\encnum3"));
	m_pStcBonusMoney2	= (GUIStatic*)	m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\encpic\\encnum4"));

	GUIStatic *pStatic	= (GUIStatic*)		m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\encpic\\word5"));
	m_pStcBonusSpecT.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\encpic\\word6"));
	m_pStcBonusSpecT.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\encpic\\word2"));
	m_pStcBonusSpecT.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\encpic\\word4"));
	m_pStcBonusSpecT.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\encpic\\encnum6"));
	m_pStcBonusSpecV.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\encpic\\encnum7"));
	m_pStcBonusSpecV.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\encpic\\encnum2"));
	m_pStcBonusSpecV.push_back( pStatic );
	pStatic				= (GUIStatic*)		m_pWnd->GetChild(_T("ttalkpic\\tt_pic2\\encpic\\encnum5"));
	m_pStcBonusSpecV.push_back( pStatic );

	for( int i = 0; i < QUEST_REW_ITEM ; i++ )
	{
		TCHAR szTmp[255];

		_stprintf(szTmp, _T("ttalkpic\\tt_pic2\\encpic\\itempic%d\\item%d"), i+1, i+1);
		IconStatic *pBonusGood = (IconStatic*)m_pWnd->GetChild( szTmp );
		m_stcBonusGoods.push_back( pBonusGood );
		pBonusGood->SetInvisible(TRUE);

		_stprintf(szTmp, _T("ttalkpic\\tt_pic2\\encpic\\itempic%d"), i+1);
		IconStatic *pBonusGoodOuter = (IconStatic*)m_pWnd->GetChild( szTmp );
		m_stcBonusGoodOuters.push_back( pBonusGoodOuter );
		pBonusGoodOuter->SetInvisible(FALSE);

		_stprintf(szTmp, _T("ttalkpic\\tt_pic2\\encpic\\itempic%d\\item%d"), i+6, i+6);
		IconStatic *pBonusSel = (IconStatic*)m_pWnd->GetChild( szTmp );
		m_stcBonusSels.push_back( pBonusSel );
		pBonusSel->SetInvisible(TRUE);

		_stprintf(szTmp, _T("ttalkpic\\tt_pic2\\encpic\\itempic%d"), i+6);
		IconStatic *pBonusSelOuter = (IconStatic*)m_pWnd->GetChild( szTmp );
		m_stcBonusSelOuters.push_back( pBonusSelOuter );
		pBonusSelOuter->SetInvisible(FALSE);
		pBonusSelOuter->SetPic( _T("data\\ui\\Common\\L_icon.bmp") );
	}

	m_pWnd->SetInvisible( TRUE );

	return TRUE;
}

//-----------------------------------------------------------------------------
// 界面消息事件处理
//-----------------------------------------------------------------------------
BOOL QuestTalkFrame::EventHandler( tagGUIEvent* pEvent )
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

	case EGUIE_DragStart:
		{
			if( m_canSelect )
			{
				for( UINT i=0; i<QUEST_REW_ITEM; i++ )
				{
					if( pWnd == m_stcBonusSels[i] )
					{
						ASSERT( i<QUEST_REW_ITEM );
						m_selIndex[i] = !m_selIndex[i];

						ShowTip( pWnd, NULL );

						if( m_selIndex[i] )
						{
							m_stcBonusSelOuters[i]->SetPic( _T("data\\ui\\Common\\L_icon-l.bmp") );

							// 只能选择一个物品
							for( UINT j=0; j<QUEST_REW_ITEM; j++ )
							{
								if( i != j )
								{
									m_selIndex[j] = false;
									m_stcBonusSelOuters[j]->SetPic( _T("data\\ui\\Common\\L_icon.bmp") );
								}
							}

							break;
						}
						else
							m_stcBonusSelOuters[i]->SetPic( _T("data\\ui\\Common\\L_icon.bmp") );
					}
				}
			}
		}
		break;

	case EGUIE_Click:
		{
			if( pWnd == m_pBtnOK )
			{
				if( m_isAccept )
				{
					if( m_isTrigger )
					{
						QuestMgr::Inst()->SendAcceptTriggerQuest( m_questID );
					}
					else
					{
						QuestMgr::Inst()->SendAcceptQuest( m_questID );
					}
				}
				else
				{
					int index = -1;
					if( AlreadySelRewItem(index) )
					{
						QuestMgr::Inst()->SendCompleteQuest( m_questID, index );
					}
					else
					{
						CombatSysUtil::Inst()->ShowScreenCenterMsgEx( g_StrTable[_T("Quest_NoSelectRewItem")] );
					}
				}
			}
			else if( pWnd == m_pBtnCancel )
			{
				if( m_isAccept )
				{
					Hide();
					QuestMgr::Inst()->OnNPCTalk( QuestMgr::Inst()->GetTalkNPCGlobalID(),
						QuestMgr::Inst()->GetTalkNPCTypeID() );
				}
				else
				{
					Hide();
					QuestMgr::Inst()->OnNPCTalk( QuestMgr::Inst()->GetTalkNPCGlobalID(),
						QuestMgr::Inst()->GetTalkNPCTypeID() );
				}
			}
			else if( pWnd == m_pBtnCloseWnd )//关闭窗口
			{
				Hide();
				m_pMgr->AddToDestroyList(this);
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
						QuestData *pData = QuestMgr::Inst()->GetData();
						ASSERT( P_VALID(pData) );
						INT16 quality;
						pData->GetQuestItemQuality(m_questID,i,quality);
						ShowTip( pWnd, ToolTipCreator::Inst()->GetItemTips(m_BonusGoodsID[i],(BYTE)quality).c_str() );
						break;
					}
				}

				if( pWnd == (GUIWnd*)m_stcBonusSels[i] )
				{
					if( m_BonusSelsID[i] != 0 )
					{
						QuestData *pData = QuestMgr::Inst()->GetData();
						ASSERT( P_VALID(pData) );
						INT16 quality;
						pData->GetQuestChoiceItemQuality(m_questID,i,quality);
						ShowTip( pWnd, ToolTipCreator::Inst()->GetItemTips(m_BonusSelsID[i],(BYTE)quality).c_str() );
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
// 设置NPC对象名称
//-----------------------------------------------------------------------------
VOID QuestTalkFrame::SetCaption( LPCTSTR szCaption )
{
	m_pCaption->SetText( szCaption );
}

//-----------------------------------------------------------------------------
// 设置任务内容
//-----------------------------------------------------------------------------
VOID QuestTalkFrame::SetContent( UINT16 questID, LPCTSTR szContent, bool accept )
{
	QuestData *pData = QuestMgr::Inst()->GetData();
	ASSERT( P_VALID(pData) );

	const tagQuestProto* pQuestProto = pData->GetQuestProto(questID);
	ASSERT( P_VALID(pQuestProto) );

	// 对话框标题
	m_pCaption->SetText( pQuestProto->name );

	// 对话内容
	tstring strContent = szContent;
	vector<tagIDLink> idLinks;
	vector<tagPosLink> posLinks;
	QuestMgr::Inst()->ParseQuestView(strContent,idLinks,posLinks);

	m_pStcCondition->SetText( strContent.c_str() );
	tagPoint size = m_pStcCondition->GetSize();
	if( m_pStcCondition->GetTextTotalHeight() > size.y )
	{
		size.y = m_pStcCondition->GetTextTotalHeight();
		m_pStcCondition->SetResizable(TRUE);
		m_pStcCondition->Resize( size );
		m_pStcCondition->SetResizable(FALSE);
	}

	if( pQuestProto->target_mode == EQTM_DYNAMIC )//动态
	{
		m_isDynamic = true;
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
		// 金钱、经验奖励
		TCHAR szRewXP[10];
		_ultot(pQuestProto->rew_xp, szRewXP, 10);
		m_pStcBonusExp->SetText( szRewXP );

		TCHAR szRewGold[5],szRewSilver[5];
		_ultot( MSilver2DBGold(pQuestProto->rew_money), szRewGold, 10 );
		_ultot( MSilver2DBSilver(pQuestProto->rew_money), szRewSilver, 10 );
		m_pStcBonusMoney1->SetText( szRewGold );
		m_pStcBonusMoney2->SetText( szRewSilver );

		// 声望、贡献、属性、技能、buff
		QuestMgr::Inst()->SetSpeRew(pQuestProto,m_pStcBonusSpecT,m_pStcBonusSpecV);
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
	m_hasRewChoicesItem = false;
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

			m_hasRewChoicesItem = true;
		}
		else
		{
			m_BonusSelsID[i] = 0;
			m_stcBonusSels[i]->SetInvisible(TRUE);
		}
	}

	m_questID = questID;
	m_isAccept = accept;
	if( m_isAccept )
	{
		m_pBtnOK->SetText( g_StrTable[_T("QuestTalk_AcceptOK")] );
		m_pBtnCancel->SetText( g_StrTable[_T("QuestTalk_AcceptCancel")] );
	}
	else
	{
		m_pBtnOK->SetText( g_StrTable[_T("QuestTalk_CompleteOK")] );
		m_pBtnCancel->SetText( g_StrTable[_T("QuestTalk_CompleteCancel")] );
	}
}

VOID QuestTalkFrame::Show( void )
{
	if( P_VALID(m_pWnd) ) 
	{
		m_pWnd->SetInvisible(FALSE);
		m_pWnd->FlipToTop();
	}
}

VOID QuestTalkFrame::Hide( void )
{
	if( P_VALID(m_pWnd) ) 
		m_pWnd->SetInvisible(TRUE);
}

BOOL QuestTalkFrame::IsVisible( void ) const
{
	if( P_VALID(m_pWnd) )
	{
		return !m_pWnd->IsInvisible();
	}

	return FALSE;
}

bool QuestTalkFrame::AlreadySelRewItem( int& index )
{
	index = -1;

	if( m_hasRewChoicesItem )
	{
		for( int i=0; i<QUEST_REW_ITEM; i++ )
		{
			if( m_selIndex[i] )
			{
				index = i;
				return TRUE;
			}
		}
		return FALSE;
	}

	return TRUE;
}

VOID QuestTalkFrame::Update()
{
	m_escapeTime += Kernel::Inst()->GetDeltaTimeDW();
	if( m_escapeTime > 1000 )//每隔1秒检测距离目标NPC的距离
	{
		m_escapeTime = 0;

		if (RoleMgr::Inst()->IsOutValidDist( QuestMgr::Inst()->GetTalkNPCGlobalID() ) )
		{
			m_pMgr->AddToDestroyList(this);
		}
	}

	GameFrame::Update();
}

BOOL QuestTalkFrame::EscCancel()
{
	if( P_VALID(m_pWnd) && !m_pWnd->IsInvisible() )
	{
		Hide();
		m_pMgr->AddToDestroyList(this);

		return TRUE;
	}

	return GameFrame::EscCancel();
}