#include "StdAfx.h"
#include "FestivalActivityPromptFrame.h"
#include "ActivityProtoData.h"
#include "LocalPlayer.h"
#include "MapMgr.h"
#include "UIEvent.h"
#include "RoleMgr.h"
#include "CombatEvent.h"

FestivalActivityPromptFrame::FestivalActivityPromptFrame(void) :
m_Trunk(this),
m_pWnd(NULL),
m_pOk(NULL),
m_pStcContent(NULL),
m_pScrollBar(NULL),
m_bCanDestroy(FALSE)
{
}

FestivalActivityPromptFrame::~FestivalActivityPromptFrame(void)
{
}

BOOL FestivalActivityPromptFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init( pMgr, pFather, dwParam ) )
		return FALSE;

	const ActivityMgr::ActivityVector& activities = ActivityMgr::Inst()->GetActivityVector();
	ActivityMgr::ActivityVector::const_iterator iter = activities.begin();
	ActivityMgr::ActivityVector::const_iterator iterEnd = activities.end();
	for( ; iter != iterEnd; iter++ )
	{
		if( NeedPrompt4FestivalActivity( *iter ) )
			m_needPromptActivities.push_back( *iter );
	}

	Prompt();
	return TRUE;
}

BOOL FestivalActivityPromptFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	if( P_VALID(m_pWnd) )
		m_pGUI->DestroyWnd( m_pWnd );

	return bRet;
}

BOOL FestivalActivityPromptFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\common15.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );
	if( !P_VALID( m_pWnd ) )
		return FALSE;

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler( m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&FestivalActivityPromptFrame::EventHandler) );

	m_pOk				= (GUIButton*)m_pWnd->GetChild( _T("comm_pic\\surebutt") );
	m_pStcContent		= (GUIStaticEx*)m_pWnd->GetChild( _T("comm_pic\\back\\word") );
	m_pScrollBar		= (GUIScrollBar*)m_pWnd->GetChild( _T("comm_pic\\back\\scroll") );

	m_pWnd->SetInvisible( TRUE );
	return TRUE;
}


VOID FestivalActivityPromptFrame::Update()
{
	
}

BOOL FestivalActivityPromptFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd( pEvent->dwWndID );

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pOk )
			{
				ShowWnd( FALSE );
				m_needPromptActivities.pop_front();
				if( m_needPromptActivities.size() <= 0 )
					m_bCanDestroy = TRUE;
				Prompt();
			}
		}
		break;
	case EGUIE_StaticExLinkClick:
		{
			if( m_vecIdLinks.size() <=0 && m_vecPosLinks.size() <= 0 )
				return TRUE;
			Navigate( pEvent );
		}
		break;
	default:
		break;
	}
	return TRUE;
}

VOID FestivalActivityPromptFrame::Navigate( tagGUIEvent* pEvent )
{
	if( !P_VALID(pEvent) )
		return;

	LPCTSTR szLink = (LPCTSTR)( pEvent->dwParam2 );
	Vector3 pos;

	//根据ID寻路
	if( (INT)pEvent->dwParam1 < 1000 )	
	{
		INT index = pEvent->dwParam1 - 1;
		if( index > (INT)m_vecIdLinks.size()-1 )
			return;

		tagIDLink& link = m_vecIdLinks[index];
		IMSG( _T("path map: %s typeid: %d\n"), link.mapName.c_str(), link.typeID );

		//不在本地图，进行跨场景寻路
		if( link.mapName != MapMgr::Inst()->GetCurMapName() )
		{
			if( QuestMgr::Inst()->MoveToMap( link.mapName.c_str(), pos ) != TRUE )
				return;
		}
		else
		{
			if( link.typeID == 1 )//触发器或者其他没有TypeID的东西
			{
				QuestData *pData = QuestMgr::Inst()->GetData();
				ASSERT( P_VALID(pData) );
				if( !pData->GetTriggerPosition( szLink, pos ) )
					return;
			}
			else
			{
				QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
				ASSERT( P_VALID(pQuery) );
				if( !pQuery->GetNPCPosition( link.typeID, pos ) )
					return;
			}
		}
	}
	else
	{
		INT index = pEvent->dwParam1-1000;
		if( index > (INT)m_vecPosLinks.size()-1 )
			return;

		tagPosLink& link = m_vecPosLinks[index];

		//不在本地图，进行跨场景寻路
		if( link.mapName != MapMgr::Inst()->GetCurMapName() )
		{
			if( QuestMgr::Inst()->MoveToMap( link.mapName.c_str(),pos ) != TRUE )
				return;
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

	IMSG( _T("target pos: x-%f,y-%f,z-%f\n"),pos.x,pos.y,pos.z );
	m_pMgr->SendEvent( &event );
}

VOID FestivalActivityPromptFrame::ShowWnd( BOOL bShow )
{
	m_pWnd->SetInvisible( !bShow );
}

VOID FestivalActivityPromptFrame::Prompt()
{
	if( m_needPromptActivities.size() <= 0 )
		return;

	m_curActivityProtoData = m_needPromptActivities.front();
	m_vecIdLinks.clear();
	m_vecPosLinks.clear();
	
	//将描述中的有关NPC,地物，等的导航信息提取并改变现实颜色
	QuestMgr::Inst()->ParseQuestView( m_curActivityProtoData.desc, m_vecIdLinks,m_vecPosLinks );
	tstring str = m_curActivityProtoData.name;
	str += _T("\\n ");
	str += m_curActivityProtoData.desc;
	m_pStcContent->SetText( str.c_str() );
	ShowWnd( TRUE );
}

//判断此活动需要不需要在角色登录的时候提示一下
bool FestivalActivityPromptFrame::NeedPrompt4FestivalActivity( const tagActivityProtoData &activity )
{
	if( activity.prompt == 0 || activity.mode != EActivityMode_Year )
		return false;

	//玩家必须在活动所在月以外不提示
	INT nCurMonth = ServerTime::Inst()->GetCurrentMonth();
	if(  nCurMonth < activity.startmonth || nCurMonth > activity.endmonth )
		return false;

	//必须是活动当天
	INT nCurDay = ServerTime::Inst()->GetCurrentDay();
	if( nCurDay < activity.startday || nCurDay > activity.endday )
		return false;

	//如果活动还没开始，但只差几个小时了，可以提示
	INT nCurHour = ServerTime::Inst()->GetCurrentHour();
	if( nCurHour > activity.endHour )
		return false;

	return true;
}

BOOL FestivalActivityPromptFrame::CanDestroy()
{
	return m_bCanDestroy;
}