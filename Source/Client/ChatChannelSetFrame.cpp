#include "stdafx.h"
#include "ChatChannelSetFrame.h"
#include "ChatEvent.h"

ChatChannelSetFrame::ChatChannelSetFrame( void ):m_Trunk(this)
{
	m_pWnd			= NULL;
	m_pBtn_Exit		= NULL;
	m_pBtn_OK		= NULL;
	m_pBtn_Cancel	= NULL;
	for( int i=0; i<NUM_CHAT_CHANNEL-1; ++i )
	{
		m_pPBtn_Channel[i]	= NULL;
		m_pPBtn_ChannelSet[i]	= NULL;
	}
	m_nCurChannel	= GT_INVALID;
}

ChatChannelSetFrame::~ChatChannelSetFrame( void )
{

}

BOOL ChatChannelSetFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
		

	return bRet;
}

BOOL ChatChannelSetFrame::Destroy()
{
	BOOL ret = GameFrame::Destroy();

	if( P_VALID(m_pWnd) )
		m_pGUI->DestroyWnd(m_pWnd);

	return ret;
}

BOOL ChatChannelSetFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath +  _T("\\ui\\optchannle.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);

	//注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&ChatChannelSetFrame::EventHandler));

	m_pBtn_Exit = (GUIButton*)m_pWnd->GetChild(_T("optchannleback\\closebutt"));
	m_pBtn_OK = (GUIButton*)m_pWnd->GetChild(_T("optchannleback\\surebutt"));
	m_pBtn_Cancel = (GUIButton*)m_pWnd->GetChild(_T("optchannleback\\canclebutt"));

	TCHAR szBuff[X_LONG_NAME] = {0};
	for( int i=0; i<NUM_CHAT_CHANNEL-1; ++i )
	{
		_sntprintf( szBuff, X_LONG_NAME, _T("optchannleback\\optchannle\\optchafra\\cha%d"), i+1 );
		m_pPBtn_Channel[i] = (GUIPushButton*)m_pWnd->GetChild(szBuff);

		_sntprintf( szBuff, X_LONG_NAME, _T("optchannleback\\optcontentback\\back0\\opt%d"), i+1 );
		m_pPBtn_ChannelSet[i] = (GUIPushButton*)m_pWnd->GetChild(szBuff);
	}

	m_pPBtn_Channel[0]->SetState(EGUIBS_PushDown, FALSE);

	return TRUE;
}

VOID ChatChannelSetFrame::OnEvent( tagGameEvent* pEvent )
{

}

VOID ChatChannelSetFrame::Update()
{
	GameFrame::Update();
}

BOOL ChatChannelSetFrame::EscCancel()
{
	if( !m_pWnd->IsInvisible() )
	{
		m_pMgr->AddToDestroyList(this);
		return TRUE;
	}
	return FALSE;
}

DWORD ChatChannelSetFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch(pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtn_Exit )
			{
				m_pMgr->AddToDestroyList(this);
			}

			if( pWnd == m_pBtn_OK )
			{
				SetChannelParam();
			}

			if( pWnd == m_pBtn_Cancel )
			{
				CurrentChannelSet(m_nCurChannel);
			}
		}
		break;
	case EGUIE_PushButtonDown:
		{
			if( _tcsnccmp(pWnd->GetName().c_str(), _T("cha"), 3)==0 )
			{
				for( int i=0; i<NUM_CHAT_CHANNEL-1; ++i )
				{
					if( pWnd == m_pPBtn_Channel[i] )
					{
						m_nCurChannel = i;
						CurrentChannelSet(m_nCurChannel);
						break;
					}
				}
			}
		}
		break;
	default:
		break;
	}

	return 0;
}

void ChatChannelSetFrame::CheckChatChannelSetParam( DWORD *dwParam )
{
	for( int i=0; i<NUM_CHAT_CHANNEL-1; ++i )
	{
		m_dwChannelSet[i] = dwParam[i];
	}
}

void ChatChannelSetFrame::CurrentChannelSet( INT nChannel )
{
	for( int i=0; i<NUM_CHAT_CHANNEL-1; ++i )
	{
		m_pPBtn_ChannelSet[i]->SetState( EGUIBS_PopUp );
	}
	if( (m_dwChannelSet[nChannel]&ERCC_Current) != ERCC_NULL )
		m_pPBtn_ChannelSet[0]->SetState( EGUIBS_PushDown, FALSE );
	if( (m_dwChannelSet[nChannel]&ERCC_Troop) != ERCC_NULL )
		m_pPBtn_ChannelSet[1]->SetState( EGUIBS_PushDown, FALSE );
	if( (m_dwChannelSet[nChannel]&ERCC_Guild) != ERCC_NULL )
		m_pPBtn_ChannelSet[2]->SetState( EGUIBS_PushDown, FALSE );
	if( (m_dwChannelSet[nChannel]&ERCC_Pvt) != ERCC_NULL )
		m_pPBtn_ChannelSet[3]->SetState( EGUIBS_PushDown, FALSE );
	if( (m_dwChannelSet[nChannel]&ERCC_World) != ERCC_NULL )
		m_pPBtn_ChannelSet[4]->SetState( EGUIBS_PushDown, FALSE );	
	if( ((m_dwChannelSet[nChannel]&ERCC_System) != ERCC_NULL) || ((m_dwChannelSet[nChannel]&ERCC_Affiche) != ERCC_NULL) )
		m_pPBtn_ChannelSet[5]->SetState( EGUIBS_PushDown, FALSE );	
	if( (m_dwChannelSet[nChannel]&ERCC_Combat) != ERCC_NULL )
		m_pPBtn_ChannelSet[6]->SetState( EGUIBS_PushDown, FALSE );
}

void ChatChannelSetFrame::SetChannelParam()
{
	m_dwChannelSet[m_nCurChannel] = ERCC_NULL;
	for( int i=0; i<NUM_CHAT_CHANNEL-1; ++i )
	{
		if( m_pPBtn_ChannelSet[i]->GetState() == EGUIBS_PushDown )
		{
			m_dwChannelSet[m_nCurChannel] |= TransChannelParam( i );
		}
	}

	tagChannelSetEvent evt(_T("tagChannelSetEvent"), NULL);
	evt.nChannel = m_nCurChannel;
	evt.nParam = m_dwChannelSet[m_nCurChannel];
	m_pMgr->SendEvent(&evt);
}

ERecvChatChannel ChatChannelSetFrame::TransChannelParam( INT nParam )
{
	ERecvChatChannel ret = ERCC_NULL;
	switch( nParam )
	{
	case 0:
		ret = ERecvChatChannel(ret|ERCC_Current);
		break;
	case 1:
		ret = ERecvChatChannel(ret|ERCC_Troop);
		break;
	case 2:
		ret = ERecvChatChannel(ret|ERCC_Guild);
		break;
	case 3:
		ret = ERecvChatChannel(ret|ERCC_Pvt);
		break;
	case 4:
		ret = ERecvChatChannel(ret|ERCC_World);
		break;
	case 5:
		ret = ERecvChatChannel(ret|ERCC_System|ERCC_Affiche);
		break;
	case 6:
		ret = ERecvChatChannel(ret|ERCC_Combat);
		break;
	default:
		ret = ERecvChatChannel(ret|ERCC_Current);
		break;
	}
	return ret;
}