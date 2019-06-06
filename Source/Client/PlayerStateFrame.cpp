#include "stdafx.h"
#include "PlayerStateFrame.h"
#include "RoleHeadPicIcon.h"
#include "ActiveStatic.h"
#include "RoleMgr.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "PlayerNameTab.h"
#include "RoleEvent.h"
#include "PlayerNameTab.h"
#include "CombatEvent.h"
#include "ProfessionMgr.h"
#include "BuffUIList.h"
#include "WorldPickerFrame.h"
#include "..\WorldDefine\msg_group.h"
#include "TeamSys.h"
#include "TeamEvent.h"
#include "GameSet.h"
#include "ActiveProgress.h"
#include "ToolTipCreator.h"

PlayerStateFrame::PlayerStateFrame() :
m_Trunk( this ),
m_pWnd( NULL ),
m_pHead( NULL ),
m_pSt_PVP( NULL ),
m_pSt_Name( NULL ),
m_pSt_Level( NULL ),
m_pPro_Hp( NULL ),
m_pPro_Mp( NULL ),
m_pPro_Sta( NULL ),
m_pSt_Vocation( NULL ),
m_pBn_Menu( NULL ),
m_pSt_Captain( NULL ),
m_pWnd_Menu( NULL ),
m_pWnd_SecondMenu( NULL ),
m_pLb_Menu( NULL ),
m_pLb_SecondMenu( NULL ),
m_pBuffUIList( NULL ),
m_ePKMode( EMPKI_Protected ),
m_eRefuseApplyMode( EMRefuse_No ),
m_ePickMode( EMPickI_InTurn ),
m_pWnd_Violence( NULL ),
m_pBn_Violence( NULL ),
m_pSt_ViolenceTime( NULL ),
m_dwViolenceTimeCount( 0 ),
m_dwViolenceStartCountDownTime( 0 ),
m_nLastSelMainMenu( GT_INVALID ),
m_nMenuPanelHeightExt( 0 ),
m_pPro_LoongYin( NULL ),
m_pPKMenu(NULL),
m_pPK_LbMenu(NULL)
{
}

PlayerStateFrame::~PlayerStateFrame()
{
}

BOOL PlayerStateFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle( _T("tagRoleGetNameEvent"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnRoleGetNameEvent));
	m_pMgr->RegisterEventHandle( _T("tagUpdataAvatarEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnUpdataAvatarEvent));
	m_pMgr->RegisterEventHandle( _T("tagPKStateStartCountDownEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnPKStateStartCountDownEvent));
	m_pMgr->RegisterEventHandle( _T("tagRoleBuffChangeEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnRoleBuffChangeEvent));
	m_pMgr->RegisterEventHandle( _T("tagUpdateLPAttEvent"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnUpdateLPAttEvent));
	m_pMgr->RegisterEventHandle( _T("tagLPRoleStateChangeEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnLPRoleStateChangeEvent));
	m_pMgr->RegisterEventHandle( _T("tagUpdateLPLevel"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnUpdateLPLevel));
	m_pMgr->RegisterEventHandle( _T("tagTeamLeaderChangeEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnTeamLeaderChangeEvent));
	m_pMgr->RegisterEventHandle( _T("tagUpdataAvatarDisplaySetEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnUpdataAvatarDisplaySetEvent));
    m_pMgr->RegisterEventHandle( _T("LocalPlayerSetClass"),	            (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnLocalPlayerSetClass));

	// 快捷键
	m_pKeyMap->Register( 0, g_StrTable[_T("PK_Mode0")],			(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&PlayerStateFrame::OnOpen_ViolenceOnlyHotKey), (DIK_A<<16)+DIK_LSHIFT, TRUE, FALSE, 0 );
	m_pKeyMap->Register( 0, g_StrTable[_T("PK_Mode1")],			(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&PlayerStateFrame::OnOpen_ViolenceAllHotKey), (DIK_A<<16)+DIK_LCONTROL, TRUE, FALSE, 0);

	return TRUE;
}

BOOL PlayerStateFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler( _T("tagRoleGetNameEvent"),			 (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnRoleGetNameEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagUpdataAvatarEvent"),			 (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnUpdataAvatarEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagPKStateStartCountDownEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnPKStateStartCountDownEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagRoleBuffChangeEvent"),		 (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnRoleBuffChangeEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagUpdateLPAttEvent"),			 (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnUpdateLPAttEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagLPRoleStateChangeEvent"),	 (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnLPRoleStateChangeEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagUpdateLPLevel"),				 (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnUpdateLPLevel));
	m_pMgr->UnRegisterEventHandler( _T("tagTeamLeaderChangeEvent"),		 (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnTeamLeaderChangeEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagUpdataAvatarDisplaySetEvent"),(FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnUpdataAvatarDisplaySetEvent));
    m_pMgr->UnRegisterEventHandler( _T("LocalPlayerSetClass"),           (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerStateFrame::OnLocalPlayerSetClass));

	m_pKeyMap->SetEnable( g_StrTable[_T("PK_Mode0")],	    FALSE );
	m_pKeyMap->SetEnable( g_StrTable[_T("PK_Mode1")],	    FALSE );

	GameFrame::Destroy();

	SafeDestroyUI();

	return TRUE;
}

BOOL PlayerStateFrame::ReloadUI()
{
	SafeDestroyUI();

	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\main_ui.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &element, "characteratt\\att\\facepic", "ClassName", "RoleHeadPicIcon");
	m_pGUI->ChangeXml( &element, "characteratt\\att\\hp", "ClassName", "ActiveProgress");
	m_pGUI->ChangeXml( &element, "characteratt\\att\\mp", "ClassName", "ActiveProgress");

	for( INT i = 0; i < BUFFUI_NUM; i++ )
	{
		std::stringstream streamBuffStaticName;
	    streamBuffStaticName<<"characteratt\\buff"<<i+1;
		m_pGUI->ChangeXml( &element, streamBuffStaticName.str().c_str(), "ClassName", "ActiveStatic");
	}

	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	if( !P_VALID( m_pWnd ) )
		return FALSE;

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&PlayerStateFrame::GUIEventHandler));

	m_pHead			= (RoleHeadPicIcon*)m_pWnd->GetChild( _T("att\\facepic") );
	m_pSt_PVP		= (GUIStatic*    )m_pWnd->GetChild( _T("att\\attack") );
	m_pSt_Name		= (GUIStatic*    )m_pWnd->GetChild( _T("att\\name") );
	m_pSt_Level		= (GUIStatic*    )m_pWnd->GetChild( _T("att\\grade") );
	m_pPro_Hp		= (ActiveProgress*)m_pWnd->GetChild( _T("att\\hp") );
	m_pPro_Mp		= (ActiveProgress*)m_pWnd->GetChild( _T("att\\mp") );
	m_pPro_Sta      = (GUIProgress*  )m_pWnd->GetChild( _T("att\\exp") );
	m_pPro_LoongYin	= (GUIProgress*  )m_pWnd->GetChild( _T("att\\progressbar4") );  
	m_pSt_Vocation  = (GUIStatic*    )m_pWnd->GetChild( _T("att\\vocation") );
	m_pBn_Menu		= (GUIPushButton*)m_pWnd->GetChild( _T("att\\teambutt") );
	m_pSt_Captain	= (GUIStatic*    )m_pWnd->GetChild( _T("att\\teampic") );

	m_pBuffUIList = new BuffUIList;
	for( INT i = 0; i < BUFFUI_NUM; i++ )
	{
		tstringstream streamBuffStaticName;
		tstringstream streamBuffProgressName;
		streamBuffStaticName<<_T("buff")<<i+1;
		streamBuffProgressName<<_T("pro")<<i+1;
		m_pBuffUIList->AddUI( (ActiveStatic*)m_pWnd->GetChild( streamBuffStaticName.str().c_str() ),
			                (GUIProgress*)m_pWnd->GetChild( streamBuffProgressName.str().c_str() ) );
	}


	// 创建菜单
	if( !CreateMenus() )
	{
		SafeDestroyUI();
		return FALSE;
	}

	// 创建“行凶”窗口
	strPath = g_strLocalPath + _T("\\ui\\pkbutt.xml");
	m_pWnd_Violence = m_pGUI->CreateFromFile( "VFS_System", strPath.c_str() );
	m_pBn_Violence = (GUIPushButton*)m_pWnd_Violence->GetChild( _T("pkbutt") );
	m_pSt_ViolenceTime = (GUIStatic*)m_pWnd_Violence->GetChild( _T("new") );
	m_pSt_ViolenceTime->SetText( _T("") );
	m_pSt_ViolenceTime->SetInvisible( TRUE );

	if( !CreatePKMenu() )
	{
		SafeDestroyUI();
		return FALSE;
	}

	m_pGUI->RegisterEventHandler(m_pWnd_Menu->GetFullName().c_str(), m_Trunk.sfp1(&PlayerStateFrame::GUIEventHandler));
	m_pGUI->RegisterEventHandler(m_pWnd_SecondMenu->GetFullName().c_str(), m_Trunk.sfp1(&PlayerStateFrame::GUIEventHandler));
	m_pGUI->RegisterEventHandler(m_pWnd_Violence->GetFullName().c_str(), m_Trunk.sfp1(&PlayerStateFrame::GUIEventHandler));
	m_pGUI->RegisterEventHandler(m_pPKMenu->GetFullName().c_str(), m_Trunk.sfp1(&PlayerStateFrame::GUIEventHandler));

	m_pSt_Captain->FlipToTop();
	m_pSt_Captain->SetTopMost( TRUE );
	m_pSt_PVP->SetInvisible( TRUE );
	m_pSt_Captain->SetInvisible( TRUE );

	m_pWnd_Menu->SetInvisible( TRUE );
	m_pWnd_SecondMenu->SetInvisible( TRUE );

	// 加载时设置玩家名字，以后就不再更新了
	tstring strName = PlayerNameTab::Inst()->FindNameByID( RoleMgr::Inst()->GetLocalPlayer()->GetID() );
	m_pSt_Name->SetText( strName.c_str() );

	return TRUE;
}

BOOL PlayerStateFrame::CreateMenus()
{
	tstring strPath = g_strLocalPath + _T("\\ui\\systemexpand.xml");
	XmlElement eleMenu;
	if( !m_pGUI->LoadXml( &eleMenu, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &eleMenu, "expand", "AlignControl", "\\desktop");
	m_pGUI->ChangeXml( &eleMenu, "expand", "AlignType", "NoAlign");
	{
		m_pGUI->ChangeXml( &eleMenu, "expand", "Name", "PlayerMenu");
		m_pGUI->ChangeXml( &eleMenu, "PlayerMenu\\listbutton1", "AlignControl", "\\desktop\\PlayerMenu");
		tagPoint ptWnd = m_pWnd->GetClient();
		stringstream streamX,streamY;
		streamX<<m_pWnd->GetClient().x + m_pWnd->GetSize().x;
		streamY<<m_pWnd->GetClient().y + 20;
		m_pGUI->ChangeXml( &eleMenu, "PlayerMenu", "LogicPos_x", streamX.str().c_str() );
		m_pGUI->ChangeXml( &eleMenu, "PlayerMenu", "LogicPos_y", streamY.str().c_str() );
		m_pWnd_Menu	= m_pGUI->CreateWnd( _T("\\desktop"), &eleMenu );
	}

	{
		m_pGUI->ChangeXml( &eleMenu, "PlayerMenu", "Name", "PlayerSecondMenu");
		m_pGUI->ChangeXml( &eleMenu, "PlayerSecondMenu\\listbutton1", "AlignControl", "\\desktop\\PlayerSecondMenu");
		tagPoint ptWnd = m_pWnd_Menu->GetClient();
		stringstream streamX,streamY;
		streamX<<m_pWnd_Menu->GetClient().x + m_pWnd_Menu->GetSize().x;
		streamY<<m_pWnd_Menu->GetClient().y;
		m_pGUI->ChangeXml( &eleMenu, "PlayerSecondMenu", "LogicPos_x", streamX.str().c_str() );
		m_pGUI->ChangeXml( &eleMenu, "PlayerSecondMenu", "LogicPos_y", streamY.str().c_str() );
		m_pWnd_SecondMenu	= m_pGUI->CreateWnd( _T("\\desktop"), &eleMenu );
	}

	m_pLb_Menu			= (GUIListBox*)m_pWnd_Menu->GetChild( _T("listbutton1") );
	m_pLb_SecondMenu	= (GUIListBox*)m_pWnd_SecondMenu->GetChild( _T("listbutton1") );
	m_nMenuPanelHeightExt = m_pWnd_Menu->GetSize().y - m_pLb_Menu->GetSize().y;

	return TRUE;
}

BOOL PlayerStateFrame::CreatePKMenu()
{
	tstring strPath = g_strLocalPath + _T("\\ui\\systemexpand.xml");
	XmlElement eleMenu;
	if( !m_pGUI->LoadXml( &eleMenu, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &eleMenu, "expand", "AlignControl", "\\desktop");
	m_pGUI->ChangeXml( &eleMenu, "expand", "AlignType", "NoAlign");
	{
		m_pGUI->ChangeXml( &eleMenu, "expand", "Name", "PlayerPKMenu");
		m_pGUI->ChangeXml( &eleMenu, "PlayerPKMenu\\listbutton1", "AlignControl", "\\desktop\\PlayerPKMenu");
		tagPoint ptWnd = m_pWnd_Violence->GetClient();
		stringstream streamX,streamY;
		streamX<<m_pWnd_Violence->GetClient().x + m_pWnd_Violence->GetSize().x;
		streamY<<m_pWnd_Violence->GetClient().y;
		m_pGUI->ChangeXml( &eleMenu, "PlayerPKMenu", "LogicPos_x", streamX.str().c_str() );
		m_pGUI->ChangeXml( &eleMenu, "PlayerPKMenu", "LogicPos_y", streamY.str().c_str() );
		m_pPKMenu = m_pGUI->CreateWnd( _T("\\desktop"), &eleMenu );
		m_pPKMenu->SetInvisible(TRUE);
	}

	m_pPK_LbMenu = (GUIListBox*)m_pPKMenu->GetChild( _T("listbutton1") );
	m_pPK_LbMenu->SetColWidth(0, 146);
	ForceResizeMenu( m_pPK_LbMenu, 2 );
	m_pPK_LbMenu->SetText( 0, 0, g_StrTable[_T("PK_Mode0")] );
	m_pPK_LbMenu->SetText( 1, 0, g_StrTable[_T("PK_Mode1")] );

	return TRUE;
}

VOID PlayerStateFrame::SafeDestroyUI()
{
	SAFE_DEL( m_pBuffUIList );

	if( P_VALID( m_pWnd ) )
	{
		m_pGUI->DestroyWnd( m_pWnd );
		m_pWnd = NULL;
	}

	if( P_VALID( m_pWnd_Menu ) )
	{
		m_pGUI->DestroyWnd( m_pWnd_Menu );
		m_pWnd_Menu = NULL;
	}

	if( P_VALID( m_pWnd_SecondMenu ) )
	{
		m_pGUI->DestroyWnd( m_pWnd_SecondMenu );
		m_pWnd_SecondMenu = NULL;
	}

	if( P_VALID( m_pWnd_Violence ) )
	{
		m_pGUI->DestroyWnd( m_pWnd_Violence );
		m_pWnd_Violence = NULL;
	}
	if( P_VALID( m_pPKMenu ) )
	{
		m_pGUI->DestroyWnd( m_pPKMenu );
		m_pPKMenu = NULL;
	}
}

VOID PlayerStateFrame::Update()
{
	m_pBuffUIList->Updata();
	UpdataSecondMenuDisplay();

	// 行凶界面更新
	if( RoleMgr::Inst()->GetLocalPlayer()->GetRoleState( ERS_PK ) || RoleMgr::Inst()->GetLocalPlayer()->GetRoleState( ERS_PKEX ))
	{
		m_pSt_ViolenceTime->SetInvisible( FALSE );
	//	if( EGUIBS_PushDown != m_pBn_Violence->GetState() )
	//		m_pBn_Violence->SetState( EGUIBS_PushDown );
		if( 0 != m_dwViolenceStartCountDownTime )
		{
			tstringstream stream;
			DWORD dwTimeLeft = m_dwViolenceTimeCount - ( timeGetTime() - m_dwViolenceStartCountDownTime );
			stream<<dwTimeLeft/1000+1;
			if( dwTimeLeft >= 0 && dwTimeLeft <= m_dwViolenceTimeCount )
			{
				m_pSt_ViolenceTime->SetText( stream.str().c_str() );
			}
		}
	}
	else// if( EGUIBS_PushDown == m_pBn_Violence->GetState() )
	{
		m_pBn_Violence->SetState( EGUIBS_PopUp );
		m_pSt_ViolenceTime->SetText( _T("") );
		m_pSt_ViolenceTime->SetInvisible( TRUE );
		m_dwViolenceStartCountDownTime = 0;
	}

	// 显示/隐藏HP MP文字
	if( GameSet::Inst()->Get( EGS_ShowHPMPText ) )
	{
		if( m_pPro_Hp->GetText().empty() )
		{
			tagUpdateLPAttEvent evt( _T("tagUpdateLPAttEvent"), this );
			evt.eType = ERA_HP;
			OnUpdateLPAttEvent( &evt );
			evt.eType = ERA_MP;
			OnUpdateLPAttEvent( &evt );
			evt.eType = ERA_Endurance;
			OnUpdateLPAttEvent( &evt );
			evt.eType = ERA_Rage;
			OnUpdateLPAttEvent( &evt );
		}
	}
	else
	{
		if( !m_pPro_Hp->GetText().empty() )
		{
			m_pPro_Hp->SetText( _T("") );
			m_pPro_Mp->SetText( _T("") );
			m_pPro_Sta->SetText( _T("") );
			m_pPro_LoongYin->SetText( _T("") );
		}
	}

	// 防止滚动菜单
	if( 0 != m_pLb_Menu->GetCurrentTextRow() )
		m_pLb_Menu->SetCurrentTextRow( 0 );
	if( 0 != m_pLb_SecondMenu->GetCurrentTextRow() )
		m_pLb_SecondMenu->SetCurrentTextRow( 0 );
	if( 0 != m_pPK_LbMenu->GetCurrentTextRow() )
		m_pPK_LbMenu->SetCurrentTextRow( 0 );

	GameFrame::Update();
}

VOID PlayerStateFrame::Render3D()
{
	if( P_VALID( m_pHead ) )
		m_pHead->Render3D();
}

DWORD PlayerStateFrame::OnRoleGetNameEvent(tagRoleGetNameEvent* pEvent)
{
	if( pEvent->dwRoleID == RoleMgr::Inst()->GetLocalPlayerID() )
	{
		m_pSt_Name->SetText( pEvent->strRoleName.c_str() );
	}
	return 0;
}

DWORD PlayerStateFrame::OnUpdataAvatarEvent(tagUpdataAvatarEvent* pEvent)
{
	if( pEvent->dwRoleID == RoleMgr::Inst()->GetLocalPlayerID() )
	{
		m_pHead->UpdataHeadPic( RoleMgr::Inst()->GetLocalPlayer() );
	}
	return 0;
}

DWORD PlayerStateFrame::OnPKStateStartCountDownEvent(tagPKStateStartCountDownEvent* pEvent)
{
	m_dwViolenceTimeCount = pEvent->dwTimeCount;
	m_dwViolenceStartCountDownTime = timeGetTime();
	return 0;
}

DWORD PlayerStateFrame::OnRoleBuffChangeEvent(tagRoleBuffChangeEvent* pEvent)
{
	Role* pRole = RoleMgr::Inst()->GetLocalPlayer();
	if( pRole->GetID() == pEvent->dwRoleID )
	{
		m_pBuffUIList->ClearBuffs();
		tagRoleBuff* pBuff = NULL;
		TList<tagRoleBuff*>& listBuff = pRole->GetBuffList();
		listBuff.ResetIterator();
		while( listBuff.PeekNext(pBuff) )
		{
			if( !P_VALID( pBuff ) )
				continue;
			if( !m_pBuffUIList->AddBuff( pBuff ) )
				break;
		}
	}
	return 0;
}

BOOL PlayerStateFrame::GUIEventHandler(tagGUIEvent* pEvent)
{
	const GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID( pWnd ) )
		return FALSE;
	switch( pEvent->eEvent )
	{
	case EGUIE_Deactive:
		if( !m_pWnd_Menu->IsInvisible() &&
			pEvent->dwParam1 != (DWORD)m_pWnd_Menu &&
			pEvent->dwParam1 != (DWORD)m_pLb_Menu &&
			pEvent->dwParam1 != (DWORD)m_pWnd_SecondMenu &&
			pEvent->dwParam1 != (DWORD)m_pLb_SecondMenu )
		{
			m_pWnd_Menu->SetInvisible( TRUE );
			m_pWnd_SecondMenu->SetInvisible( TRUE );
			m_pBn_Menu->SetState( EGUIBS_PopUp );
		}
		if( !m_pPKMenu->IsInvisible()
			&& pEvent->dwParam1 != (DWORD)m_pPKMenu 
			&& pEvent->dwParam1 != (DWORD)m_pLb_Menu  )
		{
			m_pPKMenu->SetInvisible( TRUE );
		}
		break;
	case EGUIE_Click:
		if( pWnd == m_pHead )
		{
			tagRolePickEvent evt( _T("tagRolePickEvent"), this );
			evt.eType = EWPT_LClick;
			evt.dwRoleID = RoleMgr::Inst()->GetLocalPlayerID();
			m_pMgr->SendEvent( &evt );
		}
		break;
	case EGUIE_PushButtonDown:
		if( pWnd == m_pBn_Menu )
		{
			if( 1 == pEvent->dwParam1 )                                                 // 菜单
			{
				UpdataMenuItem();
				m_pWnd_Menu->SetInvisible( FALSE );
				m_pWnd_SecondMenu->SetInvisible( TRUE );
				m_pLb_Menu->SetCurSelectedRow( GT_INVALID );
				m_nLastSelMainMenu = GT_INVALID;
			}
			else
			{
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pWnd_SecondMenu->SetInvisible( TRUE );
			}
		}
		else if( pWnd == m_pBn_Violence )
		{
			if( 1 == pEvent->dwParam1 )                                                 // 菜单
			{
				m_pPKMenu->SetInvisible( FALSE );
				m_pPK_LbMenu->SetCurSelectedRow( GT_INVALID );
			}
		}
		break;
	case EGUIE_ItemClick:
		if( pWnd == m_pLb_Menu )
		{
			if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_ExitTeam] )
			{
				TeamSys::Inst()->LeaveTeam();
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pWnd_SecondMenu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
		}
		else if( pWnd == m_pLb_SecondMenu )
		{
			OnSecondMenuItemClick( pEvent->dwParam1 );
		}
		else if( pWnd == m_pPK_LbMenu )
		{
			if( !RoleMgr::Inst()->GetLocalPlayer()->GetRoleState( ERS_PK ) || !RoleMgr::Inst()->GetLocalPlayer()->GetRoleState( ERS_PKEX ) )
			{
				tagOpenPKEvent evt( _T("tagOpenPKEvent"), this );
				evt.bPK = pEvent->dwParam1 > 0 ? false : true;
				m_pMgr->SendEvent( &evt );
				if( EGUIBS_PopUp != m_pBn_Violence->GetState() )
					m_pBn_Violence->SetState( EGUIBS_PopUp );
			}
			m_pPKMenu->SetInvisible(true);
		}
		break;
	default:
		break;
	}

	if( P_VALID( m_pBuffUIList ) )
		m_pBuffUIList->GUIEventHandler( pEvent, pWnd, RoleMgr::Inst()->GetLocalPlayer()->GetID() );

	return FALSE;
}

VOID PlayerStateFrame::UpdataMenuItem()
{
	LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
	if( !P_VALID( pLocalPlayer ) )
		return;

	for( int i = 0; i < EMI_Num; i++ )
	{
		m_nCurMenuIndex[i] = GT_INVALID;
	}
	INT nMenuIndex = 0;

	// PK模式
	BOOL bProtected = pLocalPlayer->GetRoleState() & ERS_Safeguard;
	if( bProtected )
		m_ePKMode = EMPKI_Protected;
	else
		m_ePKMode = EMPKI_Freedom;
	SetMenuFromStrTable( m_pLb_Menu, _T("LPMenu_PKMode_"), nMenuIndex, m_ePKMode );
	m_nCurMenuIndex[EMI_PKMode] = nMenuIndex++;

	// 队伍操作
	if( TeamSys::Inst()->GetTeammateNum() > 0 )
	{
		// 队长操作
		if( TeamSys::Inst()->IsLeader( pLocalPlayer->GetID() ) )
		{
			// 驱除队员
			SetMenuFromStrTable( m_pLb_Menu, _T("LPMenu_"), nMenuIndex, EMI_RemoveTeamMate );
			m_nCurMenuIndex[EMI_RemoveTeamMate] = nMenuIndex++;

			// 队长移交
			SetMenuFromStrTable( m_pLb_Menu, _T("LPMenu_"), nMenuIndex, EMI_CaptainHandOut );
			m_nCurMenuIndex[EMI_CaptainHandOut] = nMenuIndex++;

			//设置拒绝申请
			const ERefuseApplyMode eRefuseMode = TeamSys::Inst()->GetRefuseApplyMode();
			if( eRefuseMode == ERAM_Accept )
				m_eRefuseApplyMode = EMRefuse_No;
			else if( eRefuseMode == ERAM_Refuse )
				m_eRefuseApplyMode = EMRefuse_Yes;

			SetMenuFromStrTable( m_pLb_Menu, _T("LPMenu_RefuseMode_"), nMenuIndex, m_eRefuseApplyMode );
			m_nCurMenuIndex[EMI_RefuseApply] = nMenuIndex++;
		}

		// 拾取模式
		const EPickMode ePickMode = TeamSys::Inst()->GetPickMode();
		if( ePickMode == EPUM_Order )
			m_ePickMode = EMPickI_InTurn;
		else if( ePickMode == EPUM_Free )
			m_ePickMode = EMPickI_Freedom;

		SetMenuFromStrTable( m_pLb_Menu, _T("LPMenu_PickMode_"), nMenuIndex, m_ePickMode );
		m_nCurMenuIndex[EMI_PickMode] = nMenuIndex++;

		// 离开队伍
		SetMenuFromStrTable( m_pLb_Menu, _T("LPMenu_"), nMenuIndex, EMI_ExitTeam );
		m_nCurMenuIndex[EMI_ExitTeam] = nMenuIndex++;
	}

	ForceResizeMenu( m_pLb_Menu, nMenuIndex );
	m_pLb_Menu->SetCurSelectedRow( GT_INVALID );
}

VOID PlayerStateFrame::ForceResizeMenu( GUIListBox* pListBox, const INT nNumRow )
{
	tagPoint ptSizeListBox = pListBox->GetSize();
	ptSizeListBox.y = pListBox->GetRowHeight() * nNumRow;

	GUIWnd* pFather = pListBox->GetFather();
	if( P_VALID( pFather ) )
	{
		pFather->SetResizable( TRUE );
		tagPoint ptSizeFather = pFather->GetSize();
		ptSizeFather.y = ptSizeListBox.y + m_nMenuPanelHeightExt;
		pFather->Resize( ptSizeFather );
		pFather->SetResizable( FALSE );
	}
	pListBox->SetResizable( TRUE );
	pListBox->Resize( ptSizeListBox );
	pListBox->SetResizable( FALSE );
}

VOID PlayerStateFrame::SetMenuFromStrTable( GUIListBox* pListBox, const TCHAR* szBaseKey, const INT nLbIndex, const INT nStrIndex )
{
	tstringstream stream;
	stream<<szBaseKey<<nStrIndex;
	tstring strText = g_StrTable[ stream.str().c_str() ];
	pListBox->SetText( nLbIndex, 0, strText.c_str() );
}

VOID PlayerStateFrame::SetMenuFromStrTable( GUIListBox* pListBox, const TCHAR* szBaseKey, const INT nNumRow, const INT nStartRow, const INT nEndRow, const BOOL bAutoResize )
{
	if( bAutoResize )
		ForceResizeMenu( pListBox, nNumRow );

	for( INT i = nStartRow; i <= nEndRow; i++ )
	{
		tstringstream stream;
		stream<<szBaseKey<<i;
		tstring strText = g_StrTable[ stream.str().c_str() ];
		pListBox->SetText( i, 0, strText.c_str() );
	}
}

VOID PlayerStateFrame::UpdataSecondMenuDisplay()
{
	if( !P_VALID( m_pWnd_Menu ) )
		return;

	if( m_pWnd_Menu->IsInvisible() )
		return;

	const INT nCurSelMainMenu = m_pLb_Menu->GetCurSelectedRow();
	if( GT_INVALID == nCurSelMainMenu )
	{
		m_pWnd_SecondMenu->SetInvisible( TRUE );
		return;
	}

	if( nCurSelMainMenu == m_nLastSelMainMenu )
		return;

	m_nLastSelMainMenu = nCurSelMainMenu;

	if( nCurSelMainMenu == m_nCurMenuIndex[EMI_PKMode] )
	{
		SetMenuFromStrTable( m_pLb_SecondMenu, _T("LPMenu_PKMode_"), 2, 0, 1, TRUE );
		m_pWnd_SecondMenu->SetInvisible( FALSE );
	}
	else if( nCurSelMainMenu == m_nCurMenuIndex[EMI_RemoveTeamMate] )
	{
		if( TeamSys::Inst()->IsLeader( RoleMgr::Inst()->GetLocalPlayerID() ) )
		{
			ReBuildTeammateList( m_pLb_SecondMenu );
			m_pWnd_SecondMenu->SetInvisible( FALSE );
		}
		else
			m_pWnd_SecondMenu->SetInvisible( TRUE );
	}
	else if( nCurSelMainMenu == m_nCurMenuIndex[EMI_CaptainHandOut] )
	{
		if( TeamSys::Inst()->IsLeader( RoleMgr::Inst()->GetLocalPlayerID() ) )
		{
			ReBuildTeammateList( m_pLb_SecondMenu );
			m_pWnd_SecondMenu->SetInvisible( FALSE );
		}
		else
			m_pWnd_SecondMenu->SetInvisible( TRUE );
	}
	else if( nCurSelMainMenu == m_nCurMenuIndex[EMI_PickMode] )
	{
		if( TeamSys::Inst()->IsLeader( RoleMgr::Inst()->GetLocalPlayerID() ) )
		{
			SetMenuFromStrTable( m_pLb_SecondMenu, _T("LPMenu_PickMode_"), 2, 0, 1, TRUE );
			m_pWnd_SecondMenu->SetInvisible( FALSE );
		}
		else
			m_pWnd_SecondMenu->SetInvisible( TRUE );
	}
	else if( nCurSelMainMenu == m_nCurMenuIndex[EMI_RefuseApply] )
	{
		if( TeamSys::Inst()->IsLeader( RoleMgr::Inst()->GetLocalPlayerID() ) )
		{
			SetMenuFromStrTable( m_pLb_SecondMenu, _T("LPMenu_RefuseMode_"), 2, 0, 1, TRUE );
			m_pWnd_SecondMenu->SetInvisible( FALSE );
		}
		else
			m_pWnd_SecondMenu->SetInvisible( TRUE );
	}
	else
		m_pWnd_SecondMenu->SetInvisible( TRUE );

	if( !m_pWnd_SecondMenu->IsInvisible() )
	{
		ResetSecondMenuPos( nCurSelMainMenu );
		m_pLb_SecondMenu->SetCurSelectedRow( GT_INVALID );
		m_pWnd_SecondMenu->SetRefresh();
	}
}

VOID PlayerStateFrame::ResetSecondMenuPos( const INT nCurMainMenuSel )
{
	const BOOL bMoveable = m_pWnd_SecondMenu->IsMovable();
	m_pWnd_SecondMenu->SetMovable( TRUE );
	tagPoint ptDestPos = tagPoint( m_pWnd_Menu->GetClient().x + m_pWnd_Menu->GetSize().x, m_pWnd_Menu->GetClient().y );
	ptDestPos.y += nCurMainMenuSel * m_pLb_Menu->GetRowHeight();
	tagPoint ptOldPos = m_pWnd_SecondMenu->GetClient();
	m_pWnd_SecondMenu->Move( ptDestPos - ptOldPos );
	m_pWnd_SecondMenu->SetMovable( bMoveable );
}

VOID PlayerStateFrame::ReBuildTeammateList( GUIListBox* pListBox )
{
	const vector<tagTeammateData>& vecTeammates = TeamSys::Inst()->GetTeammateList();
	ForceResizeMenu( m_pLb_SecondMenu, vecTeammates.size() );
	for( size_t i = 0; i < vecTeammates.size(); i++ )
	{
		pListBox->SetText( i, 0, vecTeammates[i].strName.c_str() );
		pListBox->SetUserData( i, vecTeammates[i].dwRoleID );
	}
}

VOID PlayerStateFrame::OnSecondMenuItemClick( const INT nSelItem )
{
	if( GT_INVALID == nSelItem )
		return;

	const INT nCurSelMainMenu = m_pLb_Menu->GetCurSelectedRow();

	if( nCurSelMainMenu == m_nCurMenuIndex[EMI_PKMode] )
	{
		tagOpenSafeguardEvent evt( _T("tagOpenSafeguardEvent"), this );
		evt.bOpen = ( nSelItem == EMPKI_Protected );
		m_pMgr->SendEvent( &evt );
	}
	else if( nCurSelMainMenu == m_nCurMenuIndex[EMI_RemoveTeamMate] )
	{
		TeamSys::Inst()->KickTeammate( m_pLb_SecondMenu->GetUserData( nSelItem ) );
	}
	else if( nCurSelMainMenu == m_nCurMenuIndex[EMI_CaptainHandOut] )
	{
		TeamSys::Inst()->HandOutLeader( m_pLb_SecondMenu->GetUserData( nSelItem ) );
	}
	else if( nCurSelMainMenu == m_nCurMenuIndex[EMI_PickMode] )
	{
		if( nSelItem == EMPickI_InTurn )
			TeamSys::Inst()->SetPickMode( EPUM_Order );
		else if( nSelItem == EMPickI_Freedom )
			TeamSys::Inst()->SetPickMode( EPUM_Free );
	}
	else if( nCurSelMainMenu == m_nCurMenuIndex[EMI_RefuseApply] )
	{
		if( nSelItem == EMRefuse_No )
		{
			TeamSys::Inst()->SetRefuseApplyMode( ERAM_Accept );
			m_eRefuseApplyMode = EMRefuse_No;
		}
		else if( nSelItem == EMRefuse_Yes )
		{
			TeamSys::Inst()->SetRefuseApplyMode( ERAM_Refuse );
			m_eRefuseApplyMode = EMRefuse_Yes;
		}
	}
	else
		return;

	m_pWnd_SecondMenu->SetInvisible( TRUE );
	m_pWnd_Menu->SetInvisible( TRUE );
	m_pBn_Menu->SetState( EGUIBS_PopUp );
}

DWORD PlayerStateFrame::OnUpdateLPLevel(tagUpdateLPLevel* pEvent)
{
	tstringstream stream;
	stream<<pEvent->nCurLevel;
	m_pSt_Level->SetText( stream.str().c_str() );
	m_pSt_Level->SetRefresh( TRUE );
	if( FREE_PK_LEVEL == pEvent->nCurLevel && pEvent->nInc > 0 && pEvent->bLevelUp )
	{
		if( P_VALID(GetObj("FreePKTips")) )
			KillObj("FreePKTips");
		CreateObj("FreePKTips", "MsgBoxEx");
		TObjRef<MsgBoxEx>("FreePKTips")->Init( _T(""), 
			g_StrTable[_T("FreePKTips")], NULL, MsgBoxFlag(MBF_OK), TRUE, TRUE );
	}

	return 0;
}

DWORD PlayerStateFrame::OnUpdateLPAttEvent(tagUpdateLPAttEvent* pEvent)
{
	LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
	switch( pEvent->eType )
	{
	case ERA_HP:
	case ERA_MaxHP:
		{
			const int nMaxHp	= pLocalPlayer->GetAttribute( ERA_MaxHP );
			const int nHp		= pLocalPlayer->GetAttribute( ERA_HP );
			if( GT_INVALID != nMaxHp && GT_INVALID != nHp )
			{
				m_pPro_Hp->SetMaxValue( (FLOAT)nMaxHp );
				m_pPro_Hp->SetValue( (FLOAT)nHp, true );
				if( GameSet::Inst()->Get( EGS_ShowHPMPText ) )
				{
					tstringstream stream;
					stream<<nHp<<" / "<<nMaxHp;
					m_pPro_Hp->SetText( stream.str().c_str() );

					// 文字超出了控件，所以画布也需要刷新
					m_pPro_Mp->SetRefresh( TRUE );
				}
			}

			// 低于30%闪烁
			if( (float)nHp / nMaxHp < 0.3f )
				m_pPro_Hp->Glint( TRUE, 0x80 );
			else
				m_pPro_Hp->Glint( FALSE, 0x80 );
		}
		break;
	case ERA_MP:
	case ERA_MaxMP:
		{
			const int nMaxMp	= pLocalPlayer->GetAttribute( ERA_MaxMP );
			const int nMp		= pLocalPlayer->GetAttribute( ERA_MP );
			if( GT_INVALID != nMaxMp && GT_INVALID != nMp )
			{
				m_pPro_Mp->SetMaxValue( (FLOAT)nMaxMp );
				m_pPro_Mp->SetValue( (FLOAT)nMp, true );
				if( GameSet::Inst()->Get( EGS_ShowHPMPText ) )
				{
					tstringstream stream;
					stream<<nMp<<" / "<<nMaxMp;
					m_pPro_Mp->SetText( stream.str().c_str() );

					// 文字超出了控件，所以画布也需要刷新
					m_pPro_Mp->SetRefresh( TRUE );
				}
			}

			// 低于30%闪烁
			if( (float)nMp / nMaxMp < 0.3f )
				m_pPro_Mp->Glint( TRUE, 0x80 );
			else
				m_pPro_Mp->Glint( FALSE, 0x80 );
		}
		break;
	case ERA_Endurance:
	case ERA_MaxEndurance:
		{
			const int nMaxSta	= pLocalPlayer->GetAttribute( ERA_MaxEndurance );
			const int nSta      = pLocalPlayer->GetAttribute( ERA_Endurance );
			if( GT_INVALID != nMaxSta && GT_INVALID != nSta )
			{
				m_pPro_Sta->SetMaxValue( (FLOAT)nMaxSta );
				m_pPro_Sta->SetValue( (FLOAT)nSta, true );
				if( GameSet::Inst()->Get( EGS_ShowHPMPText ) )
				{
					tstringstream stream;
					stream<<nSta<<" / "<<nMaxSta;
					m_pPro_Sta->SetText( stream.str().c_str() );
				}
			}
		}
		break;
	case ERA_Rage:	//龙隐值
		{
			const int nMaxLoongYin = 100;
			const int nLoongYin = pLocalPlayer->GetAttribute( ERA_Rage );
			if( nLoongYin!=GT_INVALID )
			{
				m_pPro_LoongYin->SetMaxValue( (FLOAT)nMaxLoongYin );
				m_pPro_LoongYin->SetValue( (FLOAT)nLoongYin, true );
				if( GameSet::Inst()->Get( EGS_ShowHPMPText ) )
				{
					tstringstream stream;
					stream<<nLoongYin<<" / "<<nMaxLoongYin;
					m_pPro_LoongYin->SetText( stream.str().c_str() );
				}
			}

		}
		break;
	}
	return 0;
}

DWORD PlayerStateFrame::OnLPRoleStateChangeEvent(tagLPRoleStateChangeEvent* pEvent)
{
	LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
	bool bProtected = pLocalPlayer->GetRoleState( ERS_Safeguard );

	m_pBn_Violence->SetEnable( !bProtected );
	m_pBn_Violence->SetRefresh();

	// PK模式菜单
	if( !m_pWnd_Menu->IsInvisible() )
	{
		tstring strText;
		EMenuPKModeItem ePKMode;
		if( bProtected )
			ePKMode = EMPKI_Protected;
		else
			ePKMode = EMPKI_Freedom;
		if( m_ePKMode != ePKMode )
		{
			m_ePKMode = ePKMode;
			SetMenuFromStrTable( m_pLb_Menu, _T("LPMenu_PKMode_"), m_nCurMenuIndex[EMI_PKMode], m_ePKMode );
		}
	}

	// PVP图标
	m_pSt_PVP->SetInvisible( !pLocalPlayer->GetRoleState( ERS_PVP ) );

	return 0;
}

DWORD PlayerStateFrame::OnTeamLeaderChangeEvent(tagTeamLeaderChangeEvent* pEvent)
{
	// 队长图标
	if( RoleMgr::Inst()->GetLocalPlayerID() == pEvent->dwNewLeaderID )
		m_pSt_Captain->SetInvisible( FALSE );
	else
		m_pSt_Captain->SetInvisible( TRUE );

	return 0;
}

DWORD PlayerStateFrame::OnUpdataAvatarDisplaySetEvent(tagUpdataAvatarDisplaySetEvent* pEvent)
{
	if( pEvent->dwRoleID == RoleMgr::Inst()->GetLocalPlayerID() )
	{
		m_pHead->UpdataDisplaySet( RoleMgr::Inst()->GetLocalPlayer()->GetDisplaySet() );
	}
	return 0;
}

DWORD PlayerStateFrame::OnOpen_ViolenceOnlyHotKey(DWORD dwID, BOOL bDown)
{
	if( bDown )
	{
		tagOpenPKEvent evt( _T("tagOpenPKEvent"), this );
		evt.bPK = true;
		m_pMgr->SendEvent( &evt );
	}
	return 0;
}

DWORD PlayerStateFrame::OnOpen_ViolenceAllHotKey(DWORD dwID, BOOL bDown)
{
	if( bDown )
	{
		tagOpenPKEvent evt( _T("tagOpenPKEvent"), this );
		evt.bPK = false;
		m_pMgr->SendEvent( &evt );
	}
	return 0;
}

DWORD PlayerStateFrame::OnLocalPlayerSetClass( tagGameEvent* pEvent )
{
    RefreshClassText();

    return 0;
}

VOID PlayerStateFrame::RefreshClassText()
{
    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
    if (P_VALID(pLp))
    {
        m_pSt_Vocation->SetText(ProfessionMgr::Inst().GetClassName(pLp->GetPlayerClass()));
        m_pSt_Vocation->SetRefresh(TRUE);
    }
}
