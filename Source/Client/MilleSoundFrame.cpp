#include "StdAfx.h"
#include "MilleSoundFrame.h"
#include "PlayerNameTab.h"

MilleSoundFrame::MilleSoundFrame(void):
m_Trunk(this),
m_pStcImg( NULL ),
m_pStcName( NULL ),
m_pStcExContent( NULL ),
m_bDisplaying( FALSE ),
m_dwMsgPersistTime( 4000 ),
m_dwWndCloseTime( 3000 )
{
}

MilleSoundFrame::~MilleSoundFrame(void)
{
}

BOOL MilleSoundFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init( pMgr, pFather, dwParam );

	m_pFrameMgr->RegisterEventHandle( _T("tagRoleGetNameEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&MilleSoundFrame::OnRecvNetRoleGetName) );
	m_pFrameMgr->RegisterEventHandle( _T("tagGetHeadpicByID"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MilleSoundFrame::OnRecvHeadpicByID) );

	//m_pCmdMgr->Register( "NS_RoleChat",(NETMSGPROC)m_Trunk.sfp2(&MilleSoundFrame::NetRecvMsgNS_RoleChat), _T("NS_RoleChat") );

	return bRet;
}


BOOL MilleSoundFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	m_pFrameMgr->UnRegisterEventHandler( _T("tagRoleGetNameEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MilleSoundFrame::OnRecvNetRoleGetName) );
	m_pFrameMgr->UnRegisterEventHandler( _T("tagGetHeadpicByID"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MilleSoundFrame::OnRecvHeadpicByID) );
	
	//m_pCmdMgr->UnRegister( "NS_RoleChat", (NETMSGPROC)m_Trunk.sfp2(&MilleSoundFrame::NetRecvMsgNS_RoleChat) );
	
	if( P_VALID(m_pWnd) )
	{
		m_pGUI->DestroyWnd( m_pWnd );
	}

	return bRet;
}


BOOL MilleSoundFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\talk_notice.xml");
	
	m_pGUI->LoadXml( &ele, "VFS_System", strPath.c_str() );
	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &ele );

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler( m_pWnd->GetFullName().c_str(), m_Trunk.sfp1( &MilleSoundFrame::EventHandler ) );
	
	m_pStcImg = (GUIStatic*)m_pWnd->GetChild( _T("back\\imgback\\img") );
	m_pStcName = (GUIStatic*)m_pWnd->GetChild( _T("back\\name") );
	m_pStcExContent = (GUIStaticEx*)m_pWnd->GetChild( _T("back\\word") );
	
	m_pWnd->SetInvisible( TRUE );
	return TRUE;
}

VOID MilleSoundFrame::Update()
{
	//队列里没有要显示的传音
	if( m_lstCast.size() <= 0 )
	{	
		m_pWnd->SetInvisible( TRUE );
		m_pWnd->SetRefresh( TRUE );
		m_bDisplaying = FALSE;
		return;
	}

	//系统可以显示一条传音
	if( m_bDisplaying == FALSE )
	{
		ShowCastInfo();
		m_dwStartTime = timeGetTime();
		m_bDisplaying = TRUE;
		return;
	}

	//当前传音计时
	DWORD dwCurTime = timeGetTime();


	if( dwCurTime - m_dwStartTime > m_dwMsgPersistTime )
	{
		//队列里有其他传音等待，第一个是当前传音
		if( m_lstCast.size() > 1 )
		{
			m_lstCast.pop_front();
			m_bDisplaying = FALSE;
			return;
		}
		
		//队列里没有其他传音等待了，就多显示一会
		if( (dwCurTime - m_dwStartTime) > (m_dwMsgPersistTime + m_dwWndCloseTime) )
		{	
			m_lstCast.pop_front();
			m_pWnd->SetInvisible( TRUE );
			m_pWnd->SetRefresh( TRUE );
			m_bDisplaying = FALSE;
		}
	}

	GameFrame::Update();
}

DWORD MilleSoundFrame::EventHandler(tagGUIEvent* pEvent)
{
	return 0;
}

VOID MilleSoundFrame::ShowCastInfo()
{
	tagScroText text = m_lstCast.front();
	if( !text.url.empty() )
	{
		m_pStcImg->SetText( text.url.c_str() );
	}
	m_pStcName->SetText( text.szName.c_str() );
	m_pStcExContent->SetText( text.szInfo.c_str() );
	m_pWnd->SetInvisible( FALSE );
	m_pWnd->SetRefresh( TRUE );
}

VOID MilleSoundFrame::PushCastText( LPCTSTR szText, DWORD dwRoleID, float fTime )
{
	tstring strName = PlayerNameTab::Inst()->FindNameByID(dwRoleID);
	tstring strUrl = PlayerNameTab::Inst()->FindHeadPicURLbyID(dwRoleID);
	if(!strName.empty() && !strUrl.empty())
	{
		tagScroText text;
		text.szInfo = szText ;
		text.szName = strName;
		text.fTime = fTime;
		text.url = strUrl;
		m_lstCast.push_back( text );
	}
	else
	{
		tagScroText text;
		text.szInfo = szText;
		text.fTime = fTime;
		text.dwRoleID = dwRoleID;
		text.szName = strName;
		text.url = strUrl;
		m_mapCastNoName.insert( make_pair(dwRoleID,text ) );
	}
}

DWORD MilleSoundFrame::OnRecvNetRoleGetName( tagRoleGetNameEvent* pMsg )
{
	if( !P_VALID(pMsg) || !GT_VALID(pMsg->dwRoleID) || pMsg->bResult == FALSE )
		return 0;

	std::map<DWORD,tagScroText>::iterator itFind = m_mapCastNoName.find( pMsg->dwRoleID );
	if( itFind == m_mapCastNoName.end() )
		return 0;

	if( itFind->second.url.empty() )
	{
		itFind->second.szName =  pMsg->strRoleName;
	}
	else
	{
		tagScroText textNew;
		textNew.szName = pMsg->strRoleName;
		textNew.fTime = itFind->second.fTime;
		textNew.url = itFind->second.url;
		m_lstCast.push_back( textNew );
		m_mapCastNoName.erase( itFind );
	}

	return 0;
}

DWORD MilleSoundFrame::OnRecvHeadpicByID( tagGetHeadpicByID* pGameEvent )
{
	if( !P_VALID(pGameEvent) || !GT_VALID(pGameEvent->dwRoleID) )
		return 0;

	std::map<DWORD,tagScroText>::iterator itFind = m_mapCastNoName.find( pGameEvent->dwRoleID );
	if( itFind == m_mapCastNoName.end() )
		return 0;

	if( itFind->second.szName.empty() )
	{
		itFind->second.url =  pGameEvent->strImageURL;
	}
	else
	{
		tagScroText textNew;
		textNew.szName = itFind->second.szName;
		textNew.fTime = itFind->second.fTime;
		textNew.url = pGameEvent->strImageURL;
		textNew.szInfo = itFind->second.szInfo;
		m_lstCast.push_back( textNew );
		m_mapCastNoName.erase( itFind );
	}

	return 0;
}

VOID MilleSoundFrame::Move( tagPoint& pos )
{
	if( P_VALID( m_pWnd ) )
	{
		m_pWnd->SetMovable( TRUE );
		m_pWnd->Move( pos );
		m_pWnd->SetMovable( FALSE );
		m_pWnd->SetRefresh( TRUE );
	}
}