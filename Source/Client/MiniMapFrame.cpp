#include "stdafx.h"
#include "MiniMapStatic.h"
#include "MiniMapFrame.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "MapMgr.h"
#include "MapLogicData.h"
#include "MapEvent.h"
#include "ZoneMapFrame.h"
#include "QuestMgr.h"
#include "UIEvent.h"
#include "GameSet.h"
#include "OnlineTips.h"
#include "WorldFrame.h"
#include "SpecialSkillFrame.h"
const INT g_nTileSize = 50;

MiniMapFrame::MiniMapFrame() :
m_Trunk( this ),
m_pWnd_MaxSize( NULL ),
m_pMSt_MiniMap( NULL ),
m_pSt_MapName( NULL ),
m_pSt_PosX( NULL ),
m_pSt_PosZ( NULL ),
m_pBn_MiniSize( NULL ),
m_pBn_Setting( NULL ),
m_pBn_AreaMap( NULL ),
m_pBn_StyleAction( NULL ),
m_pBn_TaskQuery( NULL ),
m_pBn_TaskTrace( NULL ),
m_pBn_ZoomIn( NULL ),
m_pBn_ZoomOut( NULL ),
m_pWnd_MiniSize( NULL ),
m_pSt_MapName_MiniSize( NULL ),
m_pSt_PosX_MiniSize( NULL ),
m_pSt_PosZ_MiniSize( NULL ),
m_pBn_MaxSize( NULL ),
m_pWnd_Set( NULL ),
m_pBn_Close_Set( NULL ),
m_ptLocalPlayerPos( 0, 0 ),
m_pBn_VipStall(NULL),
m_pBn_OnlineTip(NULL),
m_pBtnQuestHelp(NULL),
m_pPBtn_ContinuteSkill(NULL)
{
	for( int i = 0; i < EMME_Num; i++ )
		m_pBn_EleShow[i] = NULL;
}

MiniMapFrame::~MiniMapFrame()
{
}



BOOL MiniMapFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle( _T("OpenMap"),					(FRAMEEVENTPROC)m_Trunk.sfp1(&MiniMapFrame::OnOpenMap));
	m_pMgr->RegisterEventHandle( _T("tagLeaveMapAreaEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MiniMapFrame::OnLeaveMapArea));
	m_pMgr->RegisterEventHandle( _T("tagEnterMapAreaEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MiniMapFrame::OnEnterMapArea));
	m_pMgr->RegisterEventHandle( _T("tagSetFlagEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&MiniMapFrame::OnSetFlagEvent));
	m_pMgr->RegisterEventHandle( _T("tagMouseMoveFinishEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MiniMapFrame::OnMouseMoveFinishEvent));

	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_MiniMapZoomIn")],		(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&MiniMapFrame::OnZoomIn_HotKey), DIK_ADD, TRUE );
	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_MiniMapZoomOut")],	(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&MiniMapFrame::OnZoomOut_HotKey), DIK_SUBTRACT, TRUE );

	return TRUE;
}

BOOL MiniMapFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler( _T("OpenMap"),					(FRAMEEVENTPROC)m_Trunk.sfp1(&MiniMapFrame::OnOpenMap));
	m_pMgr->UnRegisterEventHandler( _T("tagLeaveMapAreaEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&MiniMapFrame::OnLeaveMapArea));
	m_pMgr->UnRegisterEventHandler( _T("tagEnterMapAreaEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&MiniMapFrame::OnEnterMapArea));
	m_pMgr->UnRegisterEventHandler( _T("tagSetFlagEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&MiniMapFrame::OnSetFlagEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagMouseMoveFinishEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&MiniMapFrame::OnMouseMoveFinishEvent));

	m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_MiniMapZoomIn")],	 FALSE );
	m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_MiniMapZoomOut")],   FALSE );

	GameFrame::Destroy();

	SafeDestroyUI();

	return TRUE;
}


BOOL MiniMapFrame::ReloadUI()
{
	SafeDestroyUI();

	// 加载小地图界面
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\minimap.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &element, "minimap\\map", "ClassName", "MiniMapStatic");

	// 小地图中需要显示的图标
	std::vector<std::string> vecControlName( EMME_Num );
	std::vector<tstring> vecPicFilename( EMME_Num );
	vecControlName[EMME_LocalPlayer]		= "player";
	vecControlName[EMME_NatuRes]			= "collection";
	vecControlName[EMME_Nest]				= "nestmark";
	vecControlName[EMME_NormalMonster]		= "monster";
	vecControlName[EMME_Boss]				= "bossmark";
	vecControlName[EMME_NormalNPC]			= "npcmark";
	vecControlName[EMME_AcceptQuestNPC]		= "npcdoable";
	vecControlName[EMME_CompleteQuestNPC]   = "npcfinish";
	vecControlName[EMME_Teammate]			= "party";
	vecControlName[EMME_OtherPlayer]		= "other";
	tstring strDestMarkControlName;

	// Alpha图路径
	tstring strAlphaTexFilename;

	// 地图显示区域
	tagRect rcMap;

	// 读取贴图路径及地图显示区域
	XmlElement* pElement = element.FirstChildElement();
	while( pElement )
	{
		const std::string strName = pElement->Attribute("Name");
		if( strName == "maprect" )
		{
			pElement->Attribute( "LogicPos_x", (int*)&rcMap.left );
			pElement->Attribute( "LogicPos_y", (int*)&rcMap.top );
			pElement->Attribute( "Size_x", (int*)&rcMap.right );
			pElement->Attribute( "Size_y", (int*)&rcMap.bottom );
			rcMap.right += rcMap.left;
			rcMap.bottom += rcMap.top;

			XmlElement* pEleRes = pElement->FirstChildElement();
			while ( pEleRes )
			{
				const std::string strNameRes = pEleRes->Attribute("Name");
				for( UINT i = 0; i < vecControlName.size(); i++ )
				{
					if( strNameRes == vecControlName[i] )
					{
						vecPicFilename[i] = m_pUtil->Unicode8ToUnicode( pEleRes->Attribute( "PicFileName" ) );
						break;
					}
				}
				if( strNameRes == "mark" )
				{
					const char* szPicFileName = pEleRes->Attribute( "PicFileName" );
					if( NULL != szPicFileName )
						strDestMarkControlName = m_pUtil->Unicode8ToUnicode( szPicFileName );
				}
				pEleRes = pEleRes->NextSiblingElement();
			}
		}
		else if( strName == "alphatex" )
		{
			strAlphaTexFilename = m_pUtil->Unicode8ToUnicode( pElement->Attribute( "PicFileName" ) );
		}
		pElement = pElement->NextSiblingElement();
	}

	// 创建窗口
	m_pWnd_MaxSize = m_pGUI->CreateWnd( _T("\\desktop"), &element );
	if( !P_VALID( m_pWnd_MaxSize ) )
	{
		SafeDestroyUI();
		return FALSE;
	}

	// 销毁用来保存数据的控件
	m_pGUI->DestroyWnd( m_pWnd_MaxSize->GetChild( _T("maprect") ) );
	m_pGUI->DestroyWnd( m_pWnd_MaxSize->GetChild( _T("alphatex") ) );
	
	strPath = g_strLocalPath + _T("\\ui\\minmap.xml");
	m_pWnd_MiniSize = m_pGUI->CreateFromFile( "VFS_System", strPath.c_str() );

	if( !P_VALID( m_pWnd_MiniSize )  )
	{
		SafeDestroyUI();
		return FALSE;
	}


	// 加载小地图配置界面
	strPath = g_strLocalPath + _T("\\ui\\mm_set.xml");
	m_pWnd_Set = m_pGUI->CreateFromFile( "VFS_System", strPath.c_str() );
	if( !P_VALID( m_pWnd_Set ) )
	{
		SafeDestroyUI();
		return FALSE;
	}

	m_pGUI->RegisterEventHandler( m_pWnd_MaxSize->GetFullName().c_str(),   m_Trunk.sfp1(&MiniMapFrame::OnWnd_MaxSize));
	m_pGUI->RegisterEventHandler( m_pWnd_MiniSize->GetFullName().c_str(),  m_Trunk.sfp1(&MiniMapFrame::OnWnd_MiniSize));
	m_pGUI->RegisterEventHandler( m_pWnd_Set->GetFullName().c_str(),       m_Trunk.sfp1(&MiniMapFrame::OnWnd_Set));

	m_pMSt_MiniMap          = (MiniMapStatic*)m_pWnd_MaxSize->GetChild( _T("map") );
	m_pSt_MapName           = (GUIStatic*)m_pMSt_MiniMap->GetChild( _T("mapname") );
	m_pSt_PosX              = (GUIStatic*)m_pMSt_MiniMap->GetChild( _T("xzone") );
	m_pSt_PosZ              = (GUIStatic*)m_pMSt_MiniMap->GetChild( _T("yzone") );
	m_pBn_MiniSize          = (GUIButton*)m_pMSt_MiniMap->GetChild( _T("mapbutt") );
	m_pBn_Setting           = (GUIButton*)m_pMSt_MiniMap->GetChild( _T("but1") );
	m_pBn_AreaMap           = (GUIButton*)m_pMSt_MiniMap->GetChild( _T("but2") );
	m_pBn_TaskQuery         = (GUIButton*)m_pMSt_MiniMap->GetChild( _T("but4") );
	m_pBn_TaskTrace         = (GUIButton*)m_pMSt_MiniMap->GetChild( _T("but5") );
	m_pBn_ZoomIn            = (GUIButton*)m_pMSt_MiniMap->GetChild( _T("but11") );
	m_pBn_ZoomOut           = (GUIButton*)m_pMSt_MiniMap->GetChild( _T("but10") );
	m_pBn_VipStall			= (GUIButton*)m_pMSt_MiniMap->GetChild( _T("but3") );
	m_pBn_OnlineTip			= (GUIButton*)m_pMSt_MiniMap->GetChild( _T("but6") );
	m_pBn_StyleAction       = (GUIButton*)m_pMSt_MiniMap->GetChild( _T("but12") );
	m_pPBtn_ContinuteSkill	= (GUIPushButton*)m_pMSt_MiniMap->GetChild( _T("but13") );
	m_pPBtn_ContinuteSkill->SetState( EGUIBS_PushDown, FALSE );
	m_pBtnQuestHelp			= (GUIButton*)m_pMSt_MiniMap->GetChild( _T("but14") );

	m_pSt_MapName_MiniSize  = (GUIStatic*)m_pWnd_MiniSize->GetChild( _T("map_minipic\\mapname") );
	m_pSt_PosX_MiniSize     = (GUIStatic*)m_pWnd_MiniSize->GetChild( _T("map_minipic\\xzone1") );
	m_pSt_PosZ_MiniSize     = (GUIStatic*)m_pWnd_MiniSize->GetChild( _T("map_minipic\\yzone1") );
	m_pBn_MaxSize           = (GUIButton*)m_pWnd_MiniSize->GetChild( _T("map_minipic\\mapbutt1") );

	m_pBn_EleShow[EMME_Nest]				= (GUIPushButton*)m_pWnd_Set->GetChild( _T("ms_pic\\opt3") );
	m_pBn_EleShow[EMME_NormalMonster]		= (GUIPushButton*)m_pWnd_Set->GetChild( _T("ms_pic\\opt1") );
	m_pBn_EleShow[EMME_Boss]				= (GUIPushButton*)m_pWnd_Set->GetChild( _T("ms_pic\\opt2") );
	m_pBn_EleShow[EMME_NormalNPC]			= (GUIPushButton*)m_pWnd_Set->GetChild( _T("ms_pic\\opt5") );
	m_pBn_EleShow[EMME_AcceptQuestNPC]		= (GUIPushButton*)m_pWnd_Set->GetChild( _T("ms_pic\\opt6") );
	m_pBn_EleShow[EMME_CompleteQuestNPC]	= (GUIPushButton*)m_pWnd_Set->GetChild( _T("ms_pic\\opt7") );
	m_pBn_EleShow[EMME_Teammate]			= (GUIPushButton*)m_pWnd_Set->GetChild( _T("ms_pic\\opt4") );
	m_pBn_EleShow[EMME_OtherPlayer]			= (GUIPushButton*)m_pWnd_Set->GetChild( _T("ms_pic\\opt8") );

	m_pBn_Ok_Set     = (GUIButton*)m_pWnd_Set->GetChild( _T("ms_pic\\surebutt") );
	m_pBn_Cancel_Set = (GUIButton*)m_pWnd_Set->GetChild( _T("ms_pic\\canclebutt") );
	m_pBn_Close_Set  = (GUIButton*)m_pWnd_Set->GetChild( _T("ms_pic\\closebutt") );

	// 初始化贴图及数据
	m_pMSt_MiniMap->SetMapRect( rcMap );

	if( !m_pMSt_MiniMap->SetAlphaTex( strAlphaTexFilename ) )
	{
		SafeDestroyUI();
		return FALSE;
	}

	for( UINT i = 0; i< vecControlName.size(); i++ )
	{
		if( !m_pMSt_MiniMap->SetElementImg( i, vecPicFilename[i] ) )
		{
			SafeDestroyUI();
			return FALSE;
		}
	}
	m_pMSt_MiniMap->SetDestMarkImg( strDestMarkControlName );

	m_pWnd_MaxSize->SetInvisible( FALSE );
	m_pWnd_MiniSize->SetInvisible( TRUE );
	m_pWnd_Set->SetInvisible( TRUE );

	// 设置小地图配置
	for( int i = EGS_MM_Start; i <= EGS_MM_End; i++ )
	{
		m_pMSt_MiniMap->SetElementShow( EGStoEMME(i), GameSet::Inst()->Get(i) );
	}
	return TRUE;
}

VOID MiniMapFrame::SafeDestroyUI()
{
	if( P_VALID( m_pWnd_Set ) )
	{
		m_pGUI->DestroyWnd(m_pWnd_Set);
		m_pWnd_Set = NULL;		
	}
	if( P_VALID(m_pWnd_MaxSize) )
	{
		m_pGUI->DestroyWnd(m_pWnd_MaxSize);
		m_pWnd_MaxSize = NULL;
	}
	if( P_VALID(m_pWnd_MiniSize) )
	{
		m_pGUI->DestroyWnd(m_pWnd_MiniSize);
		m_pWnd_MiniSize = NULL;
	}
}

DWORD MiniMapFrame::OnOpenMap(tagGameEvent*)
{
	ResetMapData();
	return 0;
}

DWORD MiniMapFrame::OnLeaveMapArea( tagLeaveMapAreaEvent* pEvent )
{
	ResetMapData();
	return 0;
}

DWORD MiniMapFrame::OnEnterMapArea( tagEnterMapAreaEvent* pEvent )
{
	ResetMapData();
	return 0;
}

DWORD MiniMapFrame::OnSetFlagEvent( tagSetFlagEvent* pEvent )
{
	if( !P_VALID(m_pMSt_MiniMap) )
		return 0;

	m_pMSt_MiniMap->ShowDestMark( MapMgr::Inst()->GetCurMapID(), Vector2( pEvent->x, pEvent->z ) );
	return 0;
}

DWORD MiniMapFrame::OnMouseMoveFinishEvent( tagMouseMoveFinishEvent* pEvent )
{
	m_pMSt_MiniMap->CloseDestMark();
	return 0;
}

VOID MiniMapFrame::Update()
{
	Vector3 vPos = RoleMgr::Inst()->GetLocalPlayer()->GetPos();
	tagPoint ptPos;
	ptPos.x = (LONG)vPos.x;
	ptPos.y = (LONG)vPos.z;
	if( m_ptLocalPlayerPos.x != ptPos.x )
	{
		tstringstream stream;
		stream<<_T("x ")<<ptPos.x/g_nTileSize;
		m_pSt_PosX->SetText( stream.str().c_str() );
		m_pSt_PosX_MiniSize->SetText( stream.str().c_str() );
		m_pWnd_MiniSize->SetRefresh( TRUE );
		m_pWnd_MaxSize->SetRefresh( TRUE );
	}
	if( m_ptLocalPlayerPos.y != ptPos.y )
	{
		tstringstream stream;
		stream<<_T("z ")<<ptPos.y/g_nTileSize;
		m_pSt_PosZ->SetText( stream.str().c_str() );
		m_pSt_PosZ_MiniSize->SetText( stream.str().c_str() );
		m_pWnd_MiniSize->SetRefresh( TRUE );
		m_pWnd_MaxSize->SetRefresh( TRUE );
	}

	return GameFrame::Update();
}

//-----------------------------------------------------------------------------
//! 事件处理
//! 
//-----------------------------------------------------------------------------
BOOL MiniMapFrame::OnWnd_MaxSize(tagGUIEvent* pEvent)
{
	const GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		if( pWnd == m_pBn_MiniSize )                                   // 最小化
		{
			m_pWnd_MaxSize->SetInvisible( TRUE );
			m_pWnd_MiniSize->SetInvisible( FALSE );

			// 传给最小化窗口一个WM_MOUSEMOVE事件,防止不移动鼠标点击最大化按钮(最小化窗口上的)没反应的现象
			m_pGUI->SetActive( m_pBn_MaxSize );
			tagGUIInputMsg msg = pEvent->msg;
			msg.dwType = WM_MOUSEMOVE;
			m_pGUI->OnWindowsMsg( msg.dwType, msg.dwParam1, msg.dwParam2 );
			m_pBn_MaxSize->OnInputMessage( &msg );
		}
		else if( pWnd == m_pBn_Setting )                               // 小地图配置
		{
			m_pWnd_Set->SetInvisible( !m_pWnd_Set->IsInvisible() );
			if( !m_pWnd_Set->IsInvisible() )
			{
				for( int i = 0; i < EMME_Num; i++ )
				{
					if( !P_VALID( m_pBn_EleShow[i] ) )
						continue;
					if( m_pMSt_MiniMap->GetElementShow( i ) )
						m_pBn_EleShow[i]->SetState( EGUIBS_PushDown );
					else
						m_pBn_EleShow[i]->SetState( EGUIBS_PopUp );
				}
				m_pWnd_Set->FlipToTop();
			}
		}
		else if( pWnd == m_pBn_AreaMap)                                // 区域地图
		{
			tagOpenCloseMapWndEvent evt;
			evt.bWorldMap = false;
			m_pMgr->SendEvent( &evt );
		}
		else if( pWnd == m_pBn_VipStall )                              // 世界地图
		{
			tagGameEvent evt(_T("OpenVipStall"), NULL);
			m_pMgr->SendEvent(&evt);
		}
		else if (pWnd == m_pBn_OnlineTip)
		{
			OnlineTips *pFrame = (OnlineTips*)(TObjRef<GameFrameMgr>()->GetFrame(_T("OnlineTips")));
			if (!P_VALID(pFrame))
			{
				OnlineTips *pFrame = (OnlineTips*)(TObjRef<GameFrameMgr>()->CreateFrame(GetWorldFrm()->GetName().c_str(), _T("OnlineTips"), _T("OnlineTipsFrame"), 0));
				pFrame->ShowWnd(TRUE);
			}
			else
			{
				if (pFrame->IsShow())
					pFrame->ShowWnd(FALSE);
				else
					pFrame->ShowWnd(TRUE);
			}
		}
		else if( pWnd == m_pBn_TaskQuery )                             // 活动任务查询
		{
			if( QuestMgr::Inst()->IsActiveFrameOpened() )
				QuestMgr::Inst()->CloseActiveFrame();
			else
				QuestMgr::Inst()->OpenActiveFrame();
		}
		else if( pWnd == m_pBn_TaskTrace )                             // 任务追踪
		{
			if( QuestMgr::Inst()->IsTrackFrameOpened() )
				QuestMgr::Inst()->CloseTrackFrame();
			else
				QuestMgr::Inst()->OpenTrackFrame();
		}
		else if( pWnd == m_pBn_ZoomIn )                                // 放大
		{
			OnZoomIn_HotKey( 0, TRUE );
		}
		else if( pWnd == m_pBn_ZoomOut )                               // 缩小
		{
			OnZoomOut_HotKey( 0, TRUE );
		}
		else if( pWnd == m_pBn_StyleAction )                           // 个性动作
		{
			m_pMgr->SendEvent( &tagGameEvent( _T("OpenClose_StyleAction"), this ) );
		}
		else if( pWnd == m_pBtnQuestHelp )								// 任务帮助
		{
			if( QuestMgr::Inst()->IsQuestHelpFrameOpened() )
				QuestMgr::Inst()->CloseQuestHelpFrame();
			else
				QuestMgr::Inst()->OpenQuestHelpFrame();
		}
		break;
	case EGUIE_PushButtonDown:
		{
			if( pWnd == m_pPBtn_ContinuteSkill )
			{
				SpecialSkillFrame* pFrame = GetSpecialSkillFrame();
				if( P_VALID(pFrame) )
				{
					//激活
					if( pEvent->dwParam1==1 )
						pFrame->SetActiveState(TRUE);
					else
						pFrame->SetActiveState(FALSE);
				}
			}
		}
		break;
	default:
		break;
	}// switch( pEvent->eEvent )

	return FALSE;
}


BOOL MiniMapFrame::OnWnd_MiniSize(tagGUIEvent* pEvent)
{
	const GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		if( pWnd == m_pBn_MaxSize )                               //最大化
		{
			m_pWnd_MiniSize->SetInvisible( TRUE );
			m_pWnd_MaxSize->SetInvisible( FALSE );

			// 传给最大化窗口一个WM_MOUSEMOVE事件,防止不移动鼠标点击最小化按钮(最大化窗口上的)没反应的现象
			m_pGUI->SetActive( m_pBn_MiniSize );
			tagGUIInputMsg msg = pEvent->msg;
			msg.dwType = WM_MOUSEMOVE;
			m_pGUI->OnWindowsMsg( msg.dwType, msg.dwParam1, msg.dwParam2 );
			m_pBn_MiniSize->OnInputMessage( &msg );
		}
		break;
	default:
		break;
	}// switch( pEvent->eEvent )

	return FALSE;
}


BOOL MiniMapFrame::OnWnd_Set(tagGUIEvent* pEvent)
{
	const GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		if( pWnd == m_pBn_Ok_Set )                              // 确定
		{
			for( int i = 0; i < EMME_Num; i++ )
			{
				if( !P_VALID(m_pBn_EleShow[i]) )
					continue;
				m_pMSt_MiniMap->SetElementShow( i, m_pBn_EleShow[i]->GetState() );
			}

			// 保存
			for( int i = EGS_MM_Start; i <= EGS_MM_End; i++ )
			{
				const BOOL bShow = m_pMSt_MiniMap->GetElementShow( EGStoEMME(i) );
				GameSet::Inst()->Set( i, bShow );
			}
			GameSet::Inst()->SaveMiniMapSettings();

			m_pWnd_Set->SetInvisible( TRUE );
		}
		else if( pWnd == m_pBn_Cancel_Set )                     // 取消
		{
			m_pWnd_Set->SetInvisible( TRUE );
		}
		else if( pWnd == m_pBn_Close_Set )                      // 关闭
		{
			m_pWnd_Set->SetInvisible( TRUE );
		}
		break;
	default:
		break;
	}// switch( pEvent->eEvent )

	return FALSE;
}

VOID MiniMapFrame::ResetMapData()
{
	if( !P_VALID( m_pWnd_MaxSize ) )
		return;

	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	if( NULL == pLP )
		return;

	const DWORD dwMapID = MapMgr::Inst()->GetCurMapID();
	const tagMapLogicData* pLogicData = MapLogicData::Inst()->FindMapLogicData( dwMapID );
	ASSERT( P_VALID( pLogicData ) );

	MiniMapStatic::tagMapData data;
	const int nAreaIndex = MapMgr::Inst()->GetMapAreaIndex( pLP->GetPos(), pLP->GetRoleSize(), EMA_Display );
	BOOL bShowAreaMap = FALSE;
	if( -1 != nAreaIndex )
	{
		const vector<tagMapArea>& mapArea = MapMgr::Inst()->GetGameMap().GetMapRect();
		if( mapArea[nAreaIndex].dwMiniMapSize != 0 )
			bShowAreaMap = TRUE;
	}
	if( !bShowAreaMap )
	{
		m_pSt_MapName_MiniSize->SetText( pLogicData->szShowName.c_str() );
		m_pSt_MapName->SetText( pLogicData->szShowName.c_str() );
		MiniMapStatic::tagMapData data;
		data.vWorldSize = pLogicData->vSize * g_nTileSize;
		m_pMSt_MiniMap->ResetMapData( data );

		TCHAR szMapFileName[MAX_PATH];
		_stprintf( szMapFileName, _T("%s\\map\\%s\\%s.tga"), g_strLocalPath.c_str(), pLogicData->szName.c_str(), pLogicData->szName.c_str() );
		m_pMSt_MiniMap->SetColorTex( tstring(szMapFileName) );
	}
	else
	{
		const vector<tagMapArea>& mapArea = MapMgr::Inst()->GetGameMap().GetMapRect();
		const tagMapArea& area = mapArea[nAreaIndex];
		AABBox boxArea = area.box;
		const Vector3 vCenter = area.box.GetCenter();
		const float fHalfSize = max( area.box.max.x - area.box.min.x, area.box.max.z - area.box.min.z ) * 0.5f;
		boxArea.min = Vector3( vCenter.x - fHalfSize, vCenter.y - fHalfSize, vCenter.z - fHalfSize );
		boxArea.max = Vector3( vCenter.x + fHalfSize, vCenter.y + fHalfSize, vCenter.z + fHalfSize );

		data.vWorldPos.x = boxArea.min.x;
		data.vWorldPos.y = boxArea.min.z;
		data.vWorldSize.x = boxArea.max.x - boxArea.min.x;
		data.vWorldSize.y = boxArea.max.z - boxArea.min.z;
		m_pMSt_MiniMap->ResetMapData( data );

		TCHAR szMapFileName[MAX_PATH];
		_stprintf( szMapFileName, _T("%s\\map\\%s\\%s_%x.tga"), g_strLocalPath.c_str(), pLogicData->szName.c_str(), pLogicData->szName.c_str(), area.dwObjID );
		m_pMSt_MiniMap->SetColorTex( tstring( szMapFileName ) );

		const tagMapShowAreaData* pShowAreaData = MapLogicData::Inst()->FindMapShowAreaData( dwMapID, area.dwObjID );
		if( P_VALID( pShowAreaData ) )
		{
			m_pSt_MapName_MiniSize->SetText( pShowAreaData->szTitle.c_str() );
			m_pSt_MapName->SetText( pShowAreaData->szTitle.c_str() );
		}
	}
}


DWORD MiniMapFrame::OnZoomIn_HotKey( DWORD dwID, BOOL bDown )
{
	if( bDown )
	{
		m_pMSt_MiniMap->SetZoom( m_pMSt_MiniMap->GetZoom() - 0.1f );
		if( m_pMSt_MiniMap->GetZoom() < 0.8f )
			m_pMSt_MiniMap->SetZoom( 0.8f );
	}
	return 0;
}

DWORD MiniMapFrame::OnZoomOut_HotKey( DWORD dwID, BOOL bDown )
{
	if( bDown )
	{
		m_pMSt_MiniMap->SetZoom( m_pMSt_MiniMap->GetZoom() + 0.1f );
		if( m_pMSt_MiniMap->GetZoom() > 1.2f )
			m_pMSt_MiniMap->SetZoom( 1.2f );
	}
	return 0;
}

INT MiniMapFrame::EGStoEMME( const INT nEGS ) const
{
	ASSERT( nEGS >= EGS_MM_Start && nEGS <= EGS_MM_End );
	switch( nEGS )
	{
		case EGS_MM_ShowNatuRes:			return EMME_NatuRes;
		case EGS_MM_ShowNest:				return EMME_Nest;
		case EGS_MM_ShowNormalMonster:		return EMME_NormalMonster;
		case EGS_MM_ShowBoss:				return EMME_Boss;
		case EGS_MM_ShowNormalNPC:			return EMME_NormalNPC;
		case EGS_MM_ShowAcceptQuestNPC:		return EMME_AcceptQuestNPC;
		case EGS_MM_ShowCompleteQuestNPC:	return EMME_CompleteQuestNPC;
		case EGS_MM_ShowTeammate:			return EMME_Teammate;
		case EGS_MM_ShowOtherPlayer:		return EMME_OtherPlayer;
		default: ASSERT(0); return GT_INVALID;
	}
}