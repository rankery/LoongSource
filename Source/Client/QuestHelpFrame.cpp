#include "StdAfx.h"
#include "QuestHelpFrame.h"
#include "IconStatic.h"
#include "GameSet.h"
#include "QuestMgr.h"

QuestHelpFrame::QuestHelpFrame():m_Trunk(this),
m_pWnd(NULL),
m_pWndCaption(NULL),
m_pButClose(NULL),
m_pButExit(NULL),
m_pButHideFrame(NULL),
m_pButNext(NULL),
m_pButPre(NULL),
m_pButShowFrame(NULL),
m_pShowFrame(NULL),
m_pstcPage(NULL),
m_pStcQuestHelpPic(NULL),
m_dwCurPage(GT_INVALID),
m_dwTotalPicNum(GT_INVALID)
{

}

QuestHelpFrame::~QuestHelpFrame()
{

}

BOOL QuestHelpFrame::Destroy()
{
	GameFrame::Destroy();
	if( P_VALID(m_pWnd) )
		m_pGUI->DestroyWnd(m_pWnd);
	if(P_VALID(GetObj("QuestHelpInfo")))
		KillObj("QuestHelpInfo");
	return true;
}

VOID QuestHelpFrame::Update()
{
	float deltaTime =  Cool3D::Kernel::Inst()->GetDeltaTime();

	GameFrame::Update();
}

BOOL QuestHelpFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

	QuestMgr::Inst()->CloseQuestHelpFrame();
	return TRUE;
}

BOOL QuestHelpFrame::ReloadUI()
{
	tstring strPath = g_strLocalPath + _T("\\ui\\guide.xml");
	XmlElement element;
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());
	m_pGUI->ChangeXml(&element, "guide_win\\guide_pic\\pic\\img", "ClassName", "IconStatic");
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);

	m_pWndCaption = (GUIWnd*)m_pWnd->GetChild(_T("guide_pic"));
	m_pButClose = (GUIButton*)m_pWndCaption->GetChild(_T("close"));
	m_pButExit = (GUIButton*)m_pWndCaption->GetChild(_T("closebutt"));
	m_pButNext = (GUIButton*)m_pWndCaption->GetChild(_T("rightbutt"));
	m_pButPre = (GUIButton*)m_pWndCaption->GetChild(_T("leftbutt"));
	m_pstcPage = (GUIStatic*)m_pWndCaption->GetChild(_T("word2\\num"));
	m_pShowFrame = (GUIFrameMutex*)m_pWndCaption->GetChild(_T("word\\frm"));
	m_pButShowFrame = (GUIButton*)m_pShowFrame->GetChild(_T("yesbutt"));
	m_pButHideFrame = (GUIButton*)m_pShowFrame->GetChild(_T("nobutt"));
	m_pStcQuestHelpPic = (IconStatic*)m_pWndCaption->GetChild(_T("pic\\img"));

	if( P_VALID(m_pShowFrame) )
	{
		m_bShowFrame = GameSet::Inst()->Get( EGS_QuestHelp ) ? true : false;
		if( m_bShowFrame )
			m_pShowFrame->SetPush(m_pButShowFrame);
		else
			m_pShowFrame->SetPush(m_pButHideFrame);
	}
	m_pGUI->RegisterEventHandler( m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&QuestHelpFrame::EventHandler) );
	return true;
}

BOOL QuestHelpFrame::Init(GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam)
{
	if( !GameFrame::Init( pMgr, pFather, dwParam ) )
		return false;

	if(P_VALID(GetObj("QuestHelpInfo")))
		KillObj("QuestHelpInfo");
	m_pVarContainer = CreateObj("QuestHelpInfo", "VarContainer");
	LoadQuestHelpInfoFile();
	
	if( m_vecQuestHelpInfo.size() > 0 )
	{
		m_dwCurPage = m_vecQuestHelpInfo[0].dwStartPicPage;
		SetPicName(m_dwCurPage);
	}

	return true;
}

void QuestHelpFrame::LoadQuestHelpInfoFile()
{
	const TCHAR* szQuestHelpFile = _T("data\\system\\quest\\QuestHelpFrame_Info.xml");
	list<tstring> QuestList;
	m_pVarContainer->Load("VFS_System", szQuestHelpFile, "QuestId", &QuestList);
	list<tstring>::iterator iter = QuestList.begin();

	for(; iter!=QuestList.end(); iter++)
	{
		tagQuestHelpInfo QuestInfo;
		QuestInfo.dwQuestId = m_pVarContainer->GetDword( _T("QuestId"), (*iter).c_str(), GT_INVALID );
		_tcscpy( QuestInfo.szStartPicName, m_pVarContainer->GetString(_T("StartPicName"), (*iter).c_str(), _T("")) );
		_tcscpy( QuestInfo.szFinishPicName, m_pVarContainer->GetString(_T("FinishPicName"), (*iter).c_str(), _T("")) );
		QuestInfo.dwStartPicPage = m_pVarContainer->GetDword( _T("StartPicPage"), (*iter).c_str(), GT_INVALID );
		QuestInfo.dwFinishPicPage = m_pVarContainer->GetDword( _T("FinishPicPage"), (*iter).c_str(), GT_INVALID );
		m_vecQuestHelpInfo.push_back( QuestInfo );
	}
	if( !m_vecQuestHelpInfo.empty() )
	{
		size_t nNum = m_vecQuestHelpInfo.size() - 1;
		if( m_vecQuestHelpInfo[nNum].dwFinishPicPage == GT_INVALID )
			m_dwTotalPicNum = m_vecQuestHelpInfo[nNum].dwStartPicPage;
		else if( m_vecQuestHelpInfo[nNum].dwStartPicPage == GT_INVALID )
			m_dwTotalPicNum = m_vecQuestHelpInfo[nNum].dwFinishPicPage;
		else
		{
			m_dwTotalPicNum = ( m_vecQuestHelpInfo[nNum].dwFinishPicPage > m_vecQuestHelpInfo[nNum].dwStartPicPage ) ? 
				m_vecQuestHelpInfo[nNum].dwFinishPicPage : m_vecQuestHelpInfo[nNum].dwStartPicPage;
		}
	}
}

DWORD QuestHelpFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pButExit || pWnd == m_pButClose )	
			{
				QuestMgr::Inst()->CloseQuestHelpFrame();
			}
			else if( pWnd == m_pButPre )
			{
				if(m_dwCurPage == 1 )
					m_dwCurPage = m_dwTotalPicNum;
				else
					m_dwCurPage--;
				SetPicName(m_dwCurPage);
				ShowFrame();
				
			}
			else if( pWnd == m_pButNext )
			{
				
				if(m_dwCurPage == m_dwTotalPicNum )
					m_dwCurPage = 1;
				else
					m_dwCurPage++;
				SetPicName(m_dwCurPage);
				ShowFrame();
			}
		}
		break;
	case EGUIE_PushButtonDown:
		{
			if( pWnd == m_pButShowFrame )
			{
				GameSet::Inst()->Set( EGS_QuestHelp, true );
				GameSet::Inst()->SaveQuestHelpSet();
				m_bShowFrame = true;
			}
			else if( pWnd == m_pButHideFrame )
			{
				GameSet::Inst()->Set( EGS_QuestHelp, false );
				GameSet::Inst()->SaveQuestHelpSet();
				m_bShowFrame = false;
			}
		}
		break;
	}

	return FALSE;
}

VOID QuestHelpFrame::ShowFrame()
{
	if( P_VALID(m_pWnd) )
	{
		m_pWnd->SetInvisible(FALSE);
		TCHAR szPage[X_SHORT_NAME] = {0};
		_sntprintf(szPage, sizeof(szPage)/sizeof(TCHAR), _T("%d"), m_dwCurPage );
		m_pstcPage->SetText(szPage);
		m_pstcPage->FlipToTop();
		m_pStcQuestHelpPic->SetPic(m_strPicName);
	}
}

VOID QuestHelpFrame::HideFrame()
{
	if( P_VALID(m_pWnd) )
	{
		m_pWnd->SetInvisible(TRUE);
	}
}

VOID QuestHelpFrame::SetPicName( const DWORD dwPage )
{
	for( size_t i = 0; i < m_vecQuestHelpInfo.size(); i++ )
	{
		if( dwPage == m_vecQuestHelpInfo[i].dwStartPicPage )
		{
			m_strPicName = m_vecQuestHelpInfo[i].szStartPicName;
			break;
		}
		if( dwPage == m_vecQuestHelpInfo[i].dwFinishPicPage )
		{
			m_strPicName = m_vecQuestHelpInfo[i].szFinishPicName;
			break;
		}
	}
}