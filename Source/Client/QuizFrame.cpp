/********************************************************************
	created:	2009/08/08
	created:	8:8:2009   13:44
	filename: 	d:\loong_client\Client\QuizFrame.cpp
	file path:	d:\loong_client\Client
	file base:	QuizFrame
	file ext:	cpp
	author:		leili
	
	purpose:	答题界面实现
*********************************************************************/
#include "StdAfx.h"
#include "QuizFrame.h"
#include "RoleMgr.h"
#include "..\WorldDefine\msg_script.h"

//-----------------------------------------------------------------------------
// 构造函数
//-----------------------------------------------------------------------------
QuizFrame::QuizFrame( void )
: m_Trunk(this)
, m_pWnd(0)
, m_pStcDesc(0)
, m_pStcOptA(0)
, m_pStcOptB(0)
, m_pStcOptC(0)
, m_pStcOptD(0)
, m_pPshA(0)
, m_pPshB(0)
, m_pPshC(0)
, m_pPshD(0)
, m_pStcCD(0)
, m_pStcNum(0)
, m_pBtnSubmit(0)
, m_pBtnCancel(0)
, m_pBtnCloseWnd(0)
, m_curOpt(GT_INVALID)
, m_escapeTime(0)
, m_questTime(0)
, m_targetType(0)
, m_targetID(GT_INVALID)
{

}

//-----------------------------------------------------------------------------
// 析构函数
//-----------------------------------------------------------------------------
QuizFrame::~QuizFrame( void )
{

}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
BOOL QuizFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 销毁
//-----------------------------------------------------------------------------
BOOL QuizFrame::Destroy()
{
	GameFrame::Destroy();

	if( P_VALID(m_pWnd) )
		m_pGUI->AddToDestroyList(m_pWnd);
	m_pWnd = NULL;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 重新读入窗口界面
//-----------------------------------------------------------------------------
BOOL QuizFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\q_a.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	m_pStcDesc		= (GUIStatic*)		m_pWnd->GetChild( _T("qapic\\questionback") );
	m_pStcOptA		= (GUIStatic*)		m_pWnd->GetChild( _T("qapic\\qachooseback\\choose_a") );
	m_pStcOptB		= (GUIStatic*)		m_pWnd->GetChild( _T("qapic\\qachooseback\\choose_b") );
	m_pStcOptC		= (GUIStatic*)		m_pWnd->GetChild( _T("qapic\\qachooseback\\choose_b0") );
	m_pStcOptD		= (GUIStatic*)		m_pWnd->GetChild( _T("qapic\\qachooseback\\choose_b00") );
	m_pPshA			= (GUIPushButton*)	m_pWnd->GetChild( _T("qapic\\qachooseback\\choosefra\\abutt") );
	m_pPshB			= (GUIPushButton*)	m_pWnd->GetChild( _T("qapic\\qachooseback\\choosefra\\bbutt") );
	m_pPshC			= (GUIPushButton*)	m_pWnd->GetChild( _T("qapic\\qachooseback\\choosefra\\cbutt") );
	m_pPshD			= (GUIPushButton*)	m_pWnd->GetChild( _T("qapic\\qachooseback\\choosefra\\dbutt") );
	m_pStcCD		= (GUIStatic*)		m_pWnd->GetChild( _T("qapic\\cdpic\\cd") );
	m_pStcNum		= (GUIStatic*)		m_pWnd->GetChild( _T("qapic\\numpic\\qanum") );
	m_pBtnSubmit	= (GUIButton*)		m_pWnd->GetChild( _T("qapic\\submitbutt") );
	m_pBtnCancel	= (GUIButton*)		m_pWnd->GetChild( _T("qapic\\outbutt") );
	m_pBtnCloseWnd	= (GUIButton*)		m_pWnd->GetChild( _T("qapic\\closebutt") );

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&QuizFrame::EventHandler));

	m_pWnd->SetInvisible( FALSE );

	return TRUE;
}

VOID QuizFrame::Update()
{
	m_escapeTime += Kernel::Inst()->GetDeltaTimeDW();
	if( m_escapeTime > 1000 )//每隔1秒检测
	{
		if( m_escapeTime < m_questTime )
		{
			TCHAR szBuffer[16];
			_itot( (m_questTime-m_escapeTime)/1000 , szBuffer, 10);
			m_pStcCD->SetText( szBuffer );
			m_pStcCD->SetRefresh( TRUE );
		}
		else//时间到
		{
			m_pStcCD->SetText( _T("0") );
			m_pStcCD->SetRefresh( TRUE );
			//m_pBtnSubmit->SetEnable( false );
		}

		if (RoleMgr::Inst()->IsOutValidDist( m_targetID ) )
		{
			Hide();
			m_pMgr->AddToDestroyList(this);
		}
	}

	GameFrame::Update();
}

BOOL QuizFrame::EscCancel()
{
	if( P_VALID(m_pWnd) && !m_pWnd->IsInvisible() )
	{
		Hide();
		m_pMgr->AddToDestroyList(this);

		return TRUE;
	}

	return GameFrame::EscCancel();
}

VOID QuizFrame::Show( void )
{
	if( P_VALID(m_pWnd) ) 
	{
		m_pWnd->SetInvisible(FALSE);
		m_pWnd->FlipToTop();
	}
}

VOID QuizFrame::Hide( void )
{
	if( P_VALID(m_pWnd) ) 
	{
		m_pWnd->SetInvisible(TRUE);
	}
}

BOOL QuizFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnCloseWnd || pWnd == m_pBtnCancel )//关闭窗口
			{
				Hide();
				m_pMgr->AddToDestroyList(this);

				// 是否需要向服务器发消息
				tagNC_DlgDefaultMsg msg;
				msg.eDlgTarget = (EMsgUnitType)m_targetType;
				msg.dwTargetID = m_targetID;
				msg.eDlgOption = (EDlgOption)5;
				TObjRef<NetSession>()->Send(&msg);
			}
			else if( pWnd == m_pBtnSubmit )//提交答案
			{
				if( m_curOpt != GT_INVALID )
				{
					// 向服务器发消息
					const tagOptAttr& optAttr = m_optAttrs[m_curOpt];

					tagNC_DlgDefaultMsg msg;
					msg.eDlgTarget = (EMsgUnitType)m_targetType;
					msg.dwTargetID = m_targetID;
					msg.eDlgOption = (EDlgOption)optAttr.step;
					TObjRef<NetSession>()->Send(&msg);

					m_pBtnSubmit->SetEnable( false );
				}
				else
				{
					//提示玩家必须选择一个答案
					//或者玩家没有选择，就使按钮不可用
				}
			}
		}
		break;

	case EGUIE_PushButtonDown:
		{
			if( pWnd == m_pPshA && pEvent->dwParam1 == EGUIBS_PushDown )//选项A
				m_curOpt = 0;
			else if( pWnd == m_pPshB && pEvent->dwParam1 == EGUIBS_PushDown )//选项B
				m_curOpt = 1;
			else if( pWnd == m_pPshC && pEvent->dwParam1 == EGUIBS_PushDown )//选项C
				m_curOpt = 2;
			else if( pWnd == m_pPshD && pEvent->dwParam1 == EGUIBS_PushDown )//选项D
				m_curOpt = 3;

			if( !m_pBtnSubmit->IsEnable() )
			{
				m_pBtnSubmit->SetEnable( true );
			}
		}
		break;
	}

	return TRUE;
}

VOID QuizFrame::SetContent( LPCTSTR szContent )
{
	m_pStcDesc->SetText( szContent );
	m_pBtnSubmit->SetEnable( false );
}

VOID QuizFrame::SetCD( DWORD cd )
{
	m_questTime = cd*1000;
	m_escapeTime = 0;

	TCHAR szBuffer[16];
	_itot(cd, szBuffer, 10);
	m_pStcCD->SetText( szBuffer );
}

VOID QuizFrame::SetNum( DWORD num )
{
	TCHAR szBuffer[16];
	_itot(num, szBuffer, 10);
	m_pStcNum->SetText( szBuffer );
}

VOID QuizFrame::SetOption( LPCTSTR szOption, DWORD param1, DWORD param2 )
{
	tagOptAttr optAttr;
	optAttr.id = param1;
	optAttr.step = param2;
	m_optAttrs.push_back(optAttr);

	switch( m_optAttrs.size() )
	{
	case 1:
		m_pStcOptA->SetText( szOption );
		break;
	case 2:
		m_pStcOptB->SetText( szOption );
		break;
	case 3:
		m_pStcOptC->SetText( szOption );
		break;
	case 4:
		m_pStcOptD->SetText( szOption );
		break;
	}
}

VOID QuizFrame::Clear( void )
{
	m_pStcDesc->SetText( _T("") );
	m_pStcOptA->SetText( _T("") );
	m_pStcOptB->SetText( _T("") );
	m_pStcOptC->SetText( _T("") );
	m_pStcOptD->SetText( _T("") );
	m_pPshA->SetState(EGUIBS_PopUp,FALSE);
	m_pPshB->SetState(EGUIBS_PopUp,FALSE);
	m_pPshC->SetState(EGUIBS_PopUp,FALSE);
	m_pPshD->SetState(EGUIBS_PopUp,FALSE);

	m_curOpt = GT_INVALID;
	m_optAttrs.clear();
}