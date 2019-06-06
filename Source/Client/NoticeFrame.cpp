#include "stdafx.h"
#include "NoticeFrame.h"
#include "..\WorldDefine\chat.h"
#define MAX_NOTICE_SUSPENDTIME 1000*10		//10秒
#define MAX_NOTICE_HEIGHT 200
#define MAX_NOTICE_WIDTH 300

NoticeFrame::NoticeFrame( void ):m_Trunk(this)
{

	m_pWnd				= NULL;
	m_pBtnClose			= NULL;
	m_pStcTitle			= NULL;
	m_pStcExContent		= NULL;
	m_eNoticeState		= ENoticeState_NULL;
}

NoticeFrame::~NoticeFrame( void )
{

}

BOOL NoticeFrame::Init( GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;
	
	//--注册网络消息
	m_pCmdMgr->Register("NS_RightNotice",	(NETMSGPROC)m_Trunk.sfp2(&NoticeFrame::OnNS_RightNotice), _T("NS_RightNotice"));

	return TRUE;
}

BOOL NoticeFrame::Destroy()
{
	if( P_VALID(m_pWnd) )
		m_pGUI->AddToDestroyList(m_pWnd);

	//--注销网络消息
	m_pCmdMgr->UnRegister("NS_RightNotice",	(NETMSGPROC)m_Trunk.sfp2(&NoticeFrame::OnNS_RightNotice));

	return GameFrame::Destroy();
}

BOOL NoticeFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\notice.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	stringstream strX, strY;
	strX << Device()->GetClientWidth() - MAX_NOTICE_WIDTH;
	strY << Device()->GetClientHeight();
	m_pGUI->ChangeXml(&ele, "notice_win", "LogicPos_y", strX.str().c_str());
	m_pGUI->ChangeXml(&ele, "notice_win", "LogicPos_y", strY.str().c_str());
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pWnd->SetInvisible(TRUE);

	//注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&NoticeFrame::EventHandler));

	m_pBtnClose		= (GUIButton*)m_pWnd->GetChild(_T("notice_backpic\\closebutt"));
	m_pStcTitle		= (GUIStatic*)m_pWnd->GetChild(_T("notice_backpic\\title"));
	m_pStcExContent	= (GUIStaticEx*)m_pWnd->GetChild(_T("notice_backpic\\content"));
	m_dwCurTime = timeGetTime();

	return TRUE;
}


VOID NoticeFrame::Update()
{
	switch(m_eNoticeState)
	{
	case ENoticeState_Up:
		{
			const tagPoint& ptView = m_pWnd->GetView();
			if(ptView.y < (LONG)(Device()->GetClientHeight()-MAX_NOTICE_HEIGHT) )
			{
				m_eNoticeState = ENoticeState_Suspend;
				m_dwCurTime = timeGetTime();
			}
			m_pWnd->Move(tagPoint(0, -5));
		}
		break;
	case ENoticeState_Suspend:
		{
			if( timeGetTime()-m_dwCurTime >= MAX_NOTICE_SUSPENDTIME  )
			{
				m_eNoticeState = ENoticeState_Down;
			}
		}
		break;
	case ENoticeState_Down:
		{
			const tagPoint& ptView = m_pWnd->GetView();
			if( ptView.y >= (LONG)Device()->GetClientHeight() )	
			{ 
				m_eNoticeState = ENoticeState_NULL;
				m_pWnd->SetInvisible(TRUE);
			}
			m_pWnd->Move(tagPoint(0, 5));
		}
		break;
	}
}

DWORD NoticeFrame::EventHandler( tagGUIEvent* pEvent )
{

	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;
	switch(pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnClose )
			{
				m_eNoticeState = ENoticeState_Down;
			}
		}
		break;
	default:
		break;
	}
	return TRUE;
}

DWORD NoticeFrame::OnNS_RightNotice(tagNS_RightNotice* pMsg, DWORD dwParam)
{
	INT nTitleLen = pMsg->nTitleLen;
	INT nLinkLen = pMsg->nLinkLen;
	INT nContentLen = pMsg->nContentLen;

	tstring strMsg = pMsg->szContent;
	tstring strTitle = strMsg.substr( 0, nTitleLen );
	tstring strLink = strMsg.substr( nTitleLen, nLinkLen );
	tstring strContent = strMsg.substr( nTitleLen+nLinkLen, nContentLen );

	if( P_VALID(m_pStcTitle) )
		m_pStcTitle->SetText(strTitle.c_str());
	if( P_VALID(m_pStcExContent) )
		m_pStcExContent->SetText(strContent.c_str());
	m_eNoticeState = ENoticeState_Up;
	m_pWnd->SetRefresh(TRUE);
	m_pWnd->SetInvisible(FALSE);
	return 0;
}