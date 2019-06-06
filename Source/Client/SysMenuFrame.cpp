#include "stdafx.h"
#include "SysMenuFrame.h"
#include "ZoneMapFrame.h"
#include "MapMgr.h"
#include "SocialMgr.h"
#include "HelpFrame.h"
#include "UIEvent.h"
#include "PetFrame.h"
#include "GuildFrame.h"
#include "Render2D.h"
#include "ToolTipCreator.h"

const tstring g_strStrTableKey[SysMenuFrame::EMI_Num] = 
{
	_T( "SysMenuEx_Help" ),
	_T( "SysMenuEx_Sociality" ),
	_T( "SysMenuEx_Faction" ),
	_T( "SysMenuEx_Pet" ),
	_T( "SysMenuEx_AreaMap" ),
	_T( "SysMenuEx_WorldMap" ),
	_T( "SysMenuEx_Encrypt" ),
	_T( "SysMenuEx_SysOptionMenu" ),
};

// 扩展菜单项说明与热键中间的字符串
const tstring g_strMenuExSpace = _T("  ");

SysMenuFrame::SysMenuFrame() :
m_Trunk( this ),
m_pWnd( NULL ),
m_pPanel( NULL ),
m_pBn_Role( NULL ),
m_pBn_Package( NULL ),
m_pBn_Dower( NULL ),
m_pBn_Task( NULL ),
m_pBn_Mall( NULL ),
m_pBn_Expand( NULL ),
m_pWndMenuEx( NULL ),
m_pListMenuEx( NULL ),
m_pBn_Guild( NULL ),
m_pBn_Pet( NULL ),
m_pBn_Sociality( NULL )
{

}

SysMenuFrame::~SysMenuFrame()
{
}


//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
BOOL SysMenuFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle( _T("Open_Task_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnOpen_Task_UIEvent));
	m_pMgr->RegisterEventHandle( _T("Open_Role_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnOpen_Role_UIEvent));
	m_pMgr->RegisterEventHandle( _T("Open_Dower_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnOpen_Dower_UIEvent));
	m_pMgr->RegisterEventHandle( _T("Open_Guild_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnOpen_Guild_UIEvent));
	
	m_pMgr->RegisterEventHandle( _T("Close_Guild_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Guild_UIEvent));
	m_pMgr->RegisterEventHandle( _T("Close_Role_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Role_UIEvent));
	m_pMgr->RegisterEventHandle( _T("Close_Package_UI"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Package_UIEvent));
	m_pMgr->RegisterEventHandle( _T("Close_Dower_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Dower_UIEvent));
	m_pMgr->RegisterEventHandle( _T("Close_Task_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Task_UIEvent));
	m_pMgr->RegisterEventHandle( _T("Close_Mall_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Mall_UIEvent));
	m_pMgr->RegisterEventHandle( _T("Close_Pet_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Pet_UIEvent));
	m_pMgr->RegisterEventHandle( _T("Close_Sociality_UI"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Sociality_UIEvent));

    m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_Role_Info")], (INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_RoleInfoKey), DIK_T, TRUE, FALSE, 0 );
    m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_Role_Repute")],(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_RoleEquipKey), DIK_R, TRUE, FALSE, 0 );
    m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_Role_Title")],(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_RoleTitleKey), DIK_J, TRUE, FALSE, 0 );

	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_Role")],		(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_RoleHotKey), DIK_C, TRUE, FALSE, 0, (INPUTMAPCHANGEEVENTHANDLE)m_Trunk.sfp3(&SysMenuFrame::OnOpenClose_RoleHotKeyChange) );
	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_Package")],	(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_PackageHotKey), DIK_B, TRUE, FALSE, 0, (INPUTMAPCHANGEEVENTHANDLE)m_Trunk.sfp3(&SysMenuFrame::OnOpenClose_PackageHotKeyChange) );
	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_Dower")],		(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_DowerHotKey), DIK_K, TRUE, FALSE, 0, (INPUTMAPCHANGEEVENTHANDLE)m_Trunk.sfp3(&SysMenuFrame::OnOpenClose_DowerHotKeyChange) );
	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_Task")],		(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_TaskHotKey), DIK_Q, TRUE, FALSE, 0, (INPUTMAPCHANGEEVENTHANDLE)m_Trunk.sfp3(&SysMenuFrame::OnOpenClose_TaskHotKeyChange) );
	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_ZoneMap")],	(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_ZoneMapHotKey), DIK_N, TRUE, FALSE, 0, (INPUTMAPCHANGEEVENTHANDLE)m_Trunk.sfp3(&SysMenuFrame::OnOpenClose_ZoneMapHotKeyChange) );
	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_WorldMap")],	(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_WorldMapHotKey), DIK_M, TRUE, FALSE, 0, (INPUTMAPCHANGEEVENTHANDLE)m_Trunk.sfp3(&SysMenuFrame::OnOpenClose_WorldMapHotKeyChange) );
	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_Social")],	(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_SocialityHotKey), DIK_F, TRUE, FALSE, 0, (INPUTMAPCHANGEEVENTHANDLE)m_Trunk.sfp3(&SysMenuFrame::OnOpenClose_SocialityHotKeyChange) );
	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_Help")],		(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_HelpHotKey), DIK_H, TRUE, FALSE, 0, (INPUTMAPCHANGEEVENTHANDLE)m_Trunk.sfp3(&SysMenuFrame::OnOpenClose_HelpHotKeyChange) );
	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_Pet")],		(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_PetHotKey), DIK_P, TRUE, FALSE, 0, (INPUTMAPCHANGEEVENTHANDLE)m_Trunk.sfp3(&SysMenuFrame::OnOpenClose_PetHotKeyChange) );
	m_pKeyMap->Register( 0, g_StrTable[_T("Hotkey_Guild")],		(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SysMenuFrame::OnOpenClose_Guild), DIK_G, TRUE, FALSE, 0, (INPUTMAPCHANGEEVENTHANDLE)m_Trunk.sfp3(&SysMenuFrame::OnOpenClose_GuildChange) );

	// 初始化热键显示
	for( int i = 0; i < EMI_Num; i++ )
	{
		m_strMenuExItemText[i] = g_StrTable[ g_strStrTableKey[i].c_str() ];
	}

	OnOpenClose_RoleHotKeyChange( 0, 0, DIK_C );
	OnOpenClose_PackageHotKeyChange( 0, 0, DIK_B );
	OnOpenClose_DowerHotKeyChange( 0, 0, DIK_K );
	OnOpenClose_TaskHotKeyChange( 0, 0, DIK_Q );
	OnOpenClose_ZoneMapHotKeyChange( 0, 0, DIK_N );
	OnOpenClose_WorldMapHotKeyChange( 0, 0, DIK_M );
	OnOpenClose_SocialityHotKeyChange( 0, 0, DIK_F );
	OnOpenClose_HelpHotKeyChange( 0, 0, DIK_H );
	OnOpenClose_PetHotKeyChange( 0, 0, DIK_P );
	OnOpenClose_GuildChange( 0, 0, DIK_G );
	
	m_pListMenuEx->SetText( EMI_Encrypt, 0, m_strMenuExItemText[EMI_Encrypt].c_str() );
	tstring strSysOptionMenu = ToolTipCreator::Inst()->GetHotKeyTips( m_strMenuExItemText[EMI_SysOptionMenu].c_str(), 0, DIK_ESCAPE, g_strMenuExSpace.c_str(), false );
	m_pListMenuEx->SetText( EMI_SysOptionMenu, 0, strSysOptionMenu.c_str() );
	UpdataMenuSize();

	return TRUE;
}

//-----------------------------------------------------------------------------
// Destroy
//-----------------------------------------------------------------------------
BOOL SysMenuFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler( _T("Open_Role_UI"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnOpen_Role_UIEvent));
	m_pMgr->UnRegisterEventHandler( _T("Open_Dower_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnOpen_Dower_UIEvent));
	m_pMgr->UnRegisterEventHandler( _T("Open_Task_UI"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnOpen_Task_UIEvent));
	m_pMgr->UnRegisterEventHandler( _T("Close_Role_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Role_UIEvent));
	m_pMgr->UnRegisterEventHandler( _T("Close_Package_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Package_UIEvent));
	m_pMgr->UnRegisterEventHandler( _T("Close_Dower_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Dower_UIEvent));
	m_pMgr->UnRegisterEventHandler( _T("Close_Task_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Task_UIEvent));
	m_pMgr->UnRegisterEventHandler( _T("Close_Mall_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Mall_UIEvent));
	m_pMgr->UnRegisterEventHandler( _T("Open_Guild_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnOpen_Guild_UIEvent));
	m_pMgr->UnRegisterEventHandler( _T("Close_Guild_UI"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Guild_UIEvent));
	m_pMgr->UnRegisterEventHandler( _T("Close_Pet_UI"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Pet_UIEvent));
	m_pMgr->UnRegisterEventHandler( _T("Close_Sociality_UI"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&SysMenuFrame::OnClose_Sociality_UIEvent));

	m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_Role")],	    FALSE );
    m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_Role_Info")],   FALSE );
    m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_Role_Repute")], FALSE );
    m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_Role_Title")],  FALSE );
	m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_Package")],     FALSE );
	m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_Dower")],	    FALSE );
	m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_Task")],	    FALSE );
	m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_ZoneMap")],     FALSE );
	m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_WorldMap")],    FALSE );
	m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_Social")],	    FALSE );
	m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_Help")],	    FALSE );
    m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_Pet")],	        FALSE );
	m_pKeyMap->SetEnable( g_StrTable[_T("Hotkey_Guild")],	    FALSE );


	GameFrame::Destroy();

	SafeDestroyUI();

	return TRUE;
}


//-----------------------------------------------------------------------------
// ReloadUI
//-----------------------------------------------------------------------------
BOOL SysMenuFrame::ReloadUI()
{
	SafeDestroyUI();

	// 创建
	tstring strPath = g_strLocalPath + _T("\\ui\\systembuttons.xml");
	m_pWnd = m_pGUI->CreateFromFile( "VFS_System", strPath.c_str() );

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&SysMenuFrame::GUIEventHandlerMenu));

	m_pPanel = m_pWnd->GetChild( _T("systembutt") );
	m_pBn_Role =     (GUIPushButton*)m_pPanel->GetChild( _T("equipbutton") );
	m_pBn_Package =  (GUIPushButton*)m_pPanel->GetChild( _T("packagebutton") );
	m_pBn_Dower =    (GUIPushButton*)m_pPanel->GetChild( _T("skilbutton") );
	m_pBn_Task =     (GUIPushButton*)m_pPanel->GetChild( _T("taskbutton") );
	m_pBn_Mall =     (GUIPushButton*)m_pPanel->GetChild( _T("mallbutton") );
	m_pBn_Guild =	 (GUIPushButton*)m_pPanel->GetChild(_T("guildbutt"));
    m_pBn_Pet   =    (GUIPushButton*)m_pPanel->GetChild(_T("petbutt"));
	m_pBn_Sociality = (GUIPushButton*)m_pPanel->GetChild( _T("friendbutt") );
	m_pBn_Expand =   (GUIButton*)m_pPanel->GetChild( _T("expandbutton") );

	// 保存按钮描述
	m_strTip_Role = m_pBn_Role->GetTips();
	m_strTip_Package = m_pBn_Package->GetTips();
	m_strTip_Dower = m_pBn_Dower->GetTips();
	m_strTip_Task = m_pBn_Task->GetTips();
	m_strTip_Mall = m_pBn_Mall->GetTips();
	m_strTip_Guild = m_pBn_Guild->GetTips();
	m_strTip_Pet = m_pBn_Pet->GetTips();
	m_strTip_Sociality = m_pBn_Sociality->GetTips();

	// 创建扩展菜单
	XmlElement element;
	strPath = g_strLocalPath + _T("\\ui\\systemexpand.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &element, "expand\\listbutton1", "TextAlign", "LeftTop");
	m_pWndMenuEx = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWndMenuEx->GetFullName().c_str(), m_Trunk.sfp1(&SysMenuFrame::GUIEventHandlerMenuEx));

	m_pListMenuEx = (GUIListBox*)m_pWndMenuEx->GetChild( _T("listbutton1") );

	m_pWndMenuEx->SetInvisible( TRUE );

	return TRUE;
}

VOID SysMenuFrame::SafeDestroyUI()
{
	if( P_VALID( m_pWndMenuEx ) )
	{
		m_pGUI->DestroyWnd( m_pWndMenuEx );
		m_pWndMenuEx = NULL;
	}

	if( P_VALID( m_pWnd ) )
	{
		m_pGUI->DestroyWnd( m_pWnd );
		m_pWnd = NULL;
	}
}

//-----------------------------------------------------------------------------
//! 系统菜单事件处理
//! 
//-----------------------------------------------------------------------------
BOOL SysMenuFrame::GUIEventHandlerMenu(tagGUIEvent* pEvent)
{
	const GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_PushButtonDown:  
		if( pWnd == m_pBn_Role )                       // 打开/关闭角色界面
		{
			if( 1 == pEvent->dwParam1 )
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Role"), this ) );
			else
				m_pMgr->SendEvent( &tagGameEvent( _T("Close_Role"), this ) );
			m_pBn_Role->SetRefresh( TRUE );
		}
		else if( pWnd == m_pBn_Package )               // 打开/关闭行囊界面
		{
			if( 1 == pEvent->dwParam1 )
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Package"), this ) );
			else
				m_pMgr->SendEvent( &tagGameEvent( _T("Close_Package"), this ) );
			m_pBn_Package->SetRefresh( TRUE );
		}
		else if( pWnd == m_pBn_Dower )                 // 打开/关闭天资界面
		{
			if( 1 == pEvent->dwParam1 )
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Dower"), this ) );
			else
				m_pMgr->SendEvent( &tagGameEvent( _T("Close_Dower"), this ) );
			m_pBn_Dower->SetRefresh( TRUE );
		}
		else if( pWnd == m_pBn_Task )                  // 打开/关闭任务界面
		{
			if( 1 == pEvent->dwParam1 )
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Task"), this ) );
			else
				m_pMgr->SendEvent( &tagGameEvent( _T("Close_Task"), this ) );
			m_pBn_Task->SetRefresh( TRUE );
		}
		else if( pWnd == m_pBn_Mall )                  // 打开/关闭商城界面
		{
			if( 1 == pEvent->dwParam1 )
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Mall"), this ) );
			else
				m_pMgr->SendEvent( &tagGameEvent( _T("Close_Mall"), this ) );
			m_pBn_Mall->SetRefresh( TRUE );
		}
		else if (pWnd == m_pBn_Guild)
		{
			if (1 == pEvent->dwParam1)
			{
				m_pMgr->SendEvent(&tagGameEvent(_T("Open_Faction"), this));
			}
			else
				m_pMgr->SendEvent(&tagGameEvent(_T("Close_Faction"), this));
			m_pBn_Guild->SetRefresh( TRUE );
		}
        else if (pWnd == m_pBn_Pet)
        {
            if (1 == pEvent->dwParam1)
                m_pMgr->SendEvent( &tagGameEvent( _T("Open_Pet"), this ) );
            else
                m_pMgr->SendEvent( &tagGameEvent(_T("Close_Pet"), this ) );
			m_pBn_Pet->SetRefresh( TRUE );
        }
		else if (pWnd == m_pBn_Sociality)
		{
			if (1 == pEvent->dwParam1)
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Sociality"), this ) );
			else
				m_pMgr->SendEvent( &tagGameEvent(_T("Close_Sociality"), this ) );
			m_pBn_Sociality->SetRefresh( TRUE );
		}
		break;

	case EGUIE_Click:
		if( pWnd == m_pBn_Expand )					   //“菜单”按钮事件响应，打开扩展菜单
		{
			if( m_pWndMenuEx->IsInvisible() )
			{
				m_pWndMenuEx->SetInvisible( FALSE );
				m_pWndMenuEx->FlipToTop();
				m_pGUI->SetActive( m_pWndMenuEx );
			}
			else
			{
				m_pWndMenuEx->SetInvisible( TRUE );
			}
		}
		break;
	default:
		break;
	}// switch( pEvent->eEvent )

	return FALSE;
}

//-----------------------------------------------------------------------------
//! 系统扩展菜单事件处理
//! 
//-----------------------------------------------------------------------------
BOOL SysMenuFrame::GUIEventHandlerMenuEx(tagGUIEvent* pEvent)
{
	const GUIWnd* pWnd = m_pGUI->GetWnd( pEvent->dwWndID );
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Deactive:                                        // 鼠标点击其他区域，关闭扩展菜单
		if( !m_pWndMenuEx->IsInvisible() &&
			pEvent->dwParam1 != (DWORD)m_pWndMenuEx &&
			pEvent->dwParam1 != (DWORD)m_pListMenuEx &&
			pEvent->dwParam1 != (DWORD)m_pBn_Expand )
		{
			m_pWndMenuEx->SetInvisible( TRUE );
		}
		break;
	case EGUIE_ItemClick:
		if( pWnd == m_pListMenuEx )                             // 系统扩展菜单条目被点击
		{
			switch( pEvent->dwParam1 )
			{
			case EMI_Help:
				m_pMgr->SendEvent(&tagGameEvent(_T("Open_NewHelp"), this));
				m_pWndMenuEx->SetInvisible( TRUE );
				break;
			case EMI_Sociality:	                                        // 打开人际界面
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Sociality"), this ) );
				m_pWndMenuEx->SetInvisible( TRUE );
				break;
			case EMI_Faction:                                           // 打开帮派界面
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Faction"), this ) );
				m_pWndMenuEx->SetInvisible( TRUE );
				break;
			case EMI_Pet:                                               // 打开宠物界面
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Pet"), this ) );                  
				m_pWndMenuEx->SetInvisible( TRUE );
				break;
			case EMI_ZoneMap:                                           // 打开区域地图
				{
					tagOpenMapWndEvent evt;
					evt.bWorldMap = false;
					m_pMgr->SendEvent( &evt );
					m_pWndMenuEx->SetInvisible( TRUE );
					break;
				}
			case EMI_WorldMap:                                          // 打开世界地图
				{
					tagOpenMapWndEvent evt;
					evt.bWorldMap = true;
					m_pMgr->SendEvent( &evt );
					m_pWndMenuEx->SetInvisible( TRUE );
				}
				break;
			case EMI_Encrypt:                                           // 打开行囊加密
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Encrypt"), this ) );
				m_pWndMenuEx->SetInvisible( TRUE );
				break;
			case EMI_SysOptionMenu:                                     // 打开系统选单
				{
					GameFrame* pFrame = m_pMgr->GetFrame( _T("SysOptionMenu") );
					if( !P_VALID( pFrame ) )
					{
						m_pMgr->CreateFrame( m_strName.c_str(), _T("SysOptionMenu"), _T("SysOptionMenuFrame"), 0);
					}
					m_pWndMenuEx->SetInvisible( TRUE );
					break;
				}
			default:
				break;
			}
		}// if( pWnd == m_pListMenuEx )
		break;
	default:
		break;
	}// switch( pEvent->eEvent )

	return FALSE;
}

VOID SysMenuFrame::UpdataMenuSize()
{
	if( !P_VALID(m_pListMenuEx) )
		return;

	tagPoint ptSizeOldListBox = m_pListMenuEx->GetSize();
	tagPoint ptSizeListBox;
	ptSizeListBox.y = m_pListMenuEx->GetRowHeight() * EMI_Num;
	const int nRowNum = m_pListMenuEx->GetTotalRowNum();
	for( int i = 0; i < EMI_Num; i++ )
	{
		if( (LPCTSTR)GT_INVALID == m_pListMenuEx->GetText( i, 0 ) )
			continue;

		tagPoint ptSize;
		m_pRender->GetTextSize( m_pListMenuEx->GetText( i, 0 ), m_pListMenuEx->GetFont(), ptSize );
		if( ptSizeListBox.x < ptSize.x )
			ptSizeListBox.x = ptSize.x;
	}

	tagPoint ptListBoxDelta = ptSizeListBox - ptSizeOldListBox;
	GUIWnd* pFather = m_pListMenuEx->GetFather();
	if( P_VALID( pFather ) )
	{
		tagPoint ptOffset( -ptListBoxDelta.x, -ptListBoxDelta.y );
		BOOL bMoveable = pFather->IsMovable();
		pFather->SetMovable( TRUE );
		pFather->Move( ptOffset );
		pFather->SetMovable( bMoveable );

		BOOL bResizable = pFather->IsResizable();
		pFather->SetResizable( TRUE );
		tagPoint ptSizeFather = pFather->GetSize();
		ptSizeFather = ptSizeFather + ptListBoxDelta;
		pFather->Resize( ptSizeFather );
		pFather->SetResizable( bResizable );
	}

	BOOL bResizable = m_pListMenuEx->IsResizable();
	m_pListMenuEx->SetResizable( TRUE );
	m_pListMenuEx->Resize( ptSizeListBox );
	m_pListMenuEx->SetResizable( bResizable );
	m_pListMenuEx->SetColWidth( 0, m_pListMenuEx->GetSize().x );
}

DWORD SysMenuFrame::OnClose_Role_UIEvent(tagGameEvent* )
{
	if( EGUIBS_PushDown == m_pBn_Role->GetState() )
		m_pBn_Role->SetState( EGUIBS_PopUp );

	return 0;
}

DWORD SysMenuFrame::OnClose_Package_UIEvent(tagGameEvent* )
{
	if( EGUIBS_PushDown == m_pBn_Package->GetState() )
		m_pBn_Package->SetState( EGUIBS_PopUp );

	return 0;
}

DWORD SysMenuFrame::OnClose_Dower_UIEvent(tagGameEvent* )
{
	if( EGUIBS_PushDown == m_pBn_Dower->GetState() )
		m_pBn_Dower->SetState( EGUIBS_PopUp );

	return 0;
}

DWORD SysMenuFrame::OnClose_Task_UIEvent(tagGameEvent* )
{
	if( EGUIBS_PushDown == m_pBn_Task->GetState() )
		m_pBn_Task->SetState( EGUIBS_PopUp );

	return 0;
}

DWORD SysMenuFrame::OnOpen_Role_UIEvent( tagGameEvent* )
{
	if( EGUIBS_PushDown != m_pBn_Role->GetState() )
		m_pBn_Role->SetState( EGUIBS_PushDown );

	return 0;
}

DWORD SysMenuFrame::OnOpen_Dower_UIEvent( tagGameEvent* )
{
	if( EGUIBS_PushDown != m_pBn_Dower->GetState() )
		m_pBn_Dower->SetState( EGUIBS_PushDown );

	return 0;
}



DWORD SysMenuFrame::OnOpen_Task_UIEvent( tagGameEvent* )
{
	if( EGUIBS_PushDown != m_pBn_Task->GetState() )
		m_pBn_Task->SetState( EGUIBS_PushDown );

	return 0;
}

DWORD SysMenuFrame::OnClose_Mall_UIEvent(tagGameEvent* )
{
	if( EGUIBS_PushDown == m_pBn_Mall->GetState() )
		m_pBn_Mall->SetState( EGUIBS_PopUp );

	return 0;
}

DWORD SysMenuFrame::OnOpen_Guild_UIEvent(tagGameEvent*)
{
	if( EGUIBS_PushDown != m_pBn_Guild->GetState() )
		m_pBn_Guild->SetState( EGUIBS_PushDown );

	return 0;
}

DWORD SysMenuFrame::OnClose_Guild_UIEvent(tagGameEvent*)
{
	if( EGUIBS_PushDown == m_pBn_Guild->GetState() )
		m_pBn_Guild->SetState( EGUIBS_PopUp );

	return 0;
}

DWORD SysMenuFrame::OnClose_Pet_UIEvent(tagGameEvent*)
{
	if( EGUIBS_PushDown == m_pBn_Pet->GetState() )
		m_pBn_Pet->SetState( EGUIBS_PopUp );

	return 0;
}

DWORD SysMenuFrame::OnClose_Sociality_UIEvent(tagGameEvent*)
{
	if( EGUIBS_PushDown == m_pBn_Sociality->GetState() )
		m_pBn_Sociality->SetState( EGUIBS_PopUp );

	return 0;
}

DWORD SysMenuFrame::OnOpenClose_RoleHotKey( DWORD dwID, BOOL bDown )
{
	if( bDown )
    {
		m_pBn_Role->SetState( m_pBn_Role->GetState() == EGUIBS_PushDown ? EGUIBS_PopUp: EGUIBS_PushDown );
        m_pMgr->SendEvent( &tagGameEvent( _T("OpenEquipState"), this ) );
    }
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_RoleInfoKey( DWORD dwID, BOOL bDown )
{
    if( bDown )
    {
        m_pBn_Role->SetState( m_pBn_Role->GetState() == EGUIBS_PushDown ? EGUIBS_PopUp: EGUIBS_PushDown );
        m_pMgr->SendEvent( &tagGameEvent( _T("OpenInfomation"), this ) );
    }
    return 0;
}

DWORD SysMenuFrame::OnOpenClose_RoleEquipKey( DWORD dwID, BOOL bDown )
{
    if( bDown )
    {
        m_pBn_Role->SetState( m_pBn_Role->GetState() == EGUIBS_PushDown ? EGUIBS_PopUp: EGUIBS_PushDown );
        m_pMgr->SendEvent( &tagGameEvent( _T("OpenReputation"), this ) );
    }
    return 0;
}

DWORD SysMenuFrame::OnOpenClose_RoleTitleKey( DWORD dwID, BOOL bDown )
{
    if( bDown )
    {
        m_pBn_Role->SetState( m_pBn_Role->GetState() == EGUIBS_PushDown ? EGUIBS_PopUp: EGUIBS_PushDown );
        m_pMgr->SendEvent( &tagGameEvent( _T("OpenTitle"), this ) );
    }
    return 0;
}

DWORD SysMenuFrame::OnOpenClose_PackageHotKey( DWORD dwID, BOOL bDown )
{
	if( bDown )
		m_pBn_Package->SetState( m_pBn_Package->GetState() == EGUIBS_PushDown ? EGUIBS_PopUp: EGUIBS_PushDown );
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_DowerHotKey( DWORD dwID, BOOL bDown )
{
	if( bDown )
		m_pBn_Dower->SetState( m_pBn_Dower->GetState() == EGUIBS_PushDown ? EGUIBS_PopUp: EGUIBS_PushDown );
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_TaskHotKey( DWORD dwID, BOOL bDown )
{
	if( bDown )
		m_pBn_Task->SetState( m_pBn_Task->GetState() == EGUIBS_PushDown ? EGUIBS_PopUp: EGUIBS_PushDown );
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_ZoneMapHotKey( DWORD dwID, BOOL bDown )
{
	if( bDown )
	{
		tagOpenCloseMapWndEvent evt;
		evt.bWorldMap = false;
		m_pMgr->SendEvent( &evt );
	}
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_WorldMapHotKey( DWORD dwID, BOOL bDown )
{
	if( bDown )
	{
		tagOpenCloseMapWndEvent evt;
		evt.bWorldMap = true;
		m_pMgr->SendEvent( &evt );
	}
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_SocialityHotKey( DWORD dwID, BOOL bDown )
{
	if( bDown )
		m_pBn_Sociality->SetState( m_pBn_Sociality->GetState() == EGUIBS_PushDown ? EGUIBS_PopUp: EGUIBS_PushDown );
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_HelpHotKey(DWORD dwID, BOOL bDown)
{
	if( bDown )
	{
		HelpFrame *pFrame = (HelpFrame*)(m_pMgr->GetFrame( _T("HelpFrame")));
		if(P_VALID(pFrame))
		{
			if (pFrame->IsVisible())
				m_pMgr->SendEvent( &tagGameEvent( _T("Close_NewHelp"), this ) );
			else
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_NewHelp"), this ) );
		}
	}
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_PetHotKey( DWORD dwID, BOOL bDown )
{
    if (bDown)
        m_pBn_Pet->SetState( m_pBn_Pet->GetState() == EGUIBS_PushDown ? EGUIBS_PopUp: EGUIBS_PushDown );

    return 0;
}

DWORD SysMenuFrame::OnOpenClose_Guild(DWORD dwID, BOOL bDown)
{
	if( bDown )
	{
		GuildFrame *pFrame = (GuildFrame*)(m_pMgr->GetFrame( _T("Guild")));
		if(P_VALID(pFrame))
		{
			if (!pFrame->IsShow())
				//m_pMgr->SendEvent( &tagGameEvent( _T("Open_Faction"), this ) );
				m_pBn_Guild->SetState( EGUIBS_PushDown );
			else
				//m_pMgr->SendEvent( &tagGameEvent( _T("Close_Faction"), this ) );
				m_pBn_Guild->SetState( EGUIBS_PopUp );
		}
		else
		{
			if (m_pBn_Guild->GetState() == EGUIBS_PopUp)
				m_pBn_Guild->SetState(EGUIBS_PushDown);
			else if (m_pBn_Guild->GetState() == EGUIBS_PushDown)
				m_pBn_Guild->SetState(EGUIBS_PopUp);
		}
	}
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_RoleHotKeyChange(DWORD dwID, DWORD key1, DWORD key2)
{
	m_pBn_Role->SetTips( ToolTipCreator::Inst()->GetHotKeyTips( m_strTip_Role.c_str(), key1, key2 ).c_str() );
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_PackageHotKeyChange(DWORD dwID, DWORD key1, DWORD key2)
{
	m_pBn_Package->SetTips( ToolTipCreator::Inst()->GetHotKeyTips( m_strTip_Package.c_str(), key1, key2 ).c_str() );
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_DowerHotKeyChange(DWORD dwID, DWORD key1, DWORD key2)
{
	m_pBn_Dower->SetTips( ToolTipCreator::Inst()->GetHotKeyTips( m_strTip_Dower.c_str(), key1, key2 ).c_str() );
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_TaskHotKeyChange(DWORD dwID, DWORD key1, DWORD key2)
{
	m_pBn_Task->SetTips( ToolTipCreator::Inst()->GetHotKeyTips( m_strTip_Task.c_str(), key1, key2 ).c_str() );
	return 0;
}

DWORD SysMenuFrame::OnOpenClose_GuildChange(DWORD dwID, DWORD key1, DWORD key2)
{
	m_pBn_Guild->SetTips( ToolTipCreator::Inst()->GetHotKeyTips( m_strTip_Guild.c_str(), key1, key2 ).c_str() );

	tstring strMenuText = ToolTipCreator::Inst()->GetHotKeyTips( m_strMenuExItemText[EMI_Faction].c_str(), key1, key2, g_strMenuExSpace.c_str(), false );
	m_pListMenuEx->SetText( EMI_Faction, 0, strMenuText.c_str() );
	UpdataMenuSize();

	return 0;
}

DWORD SysMenuFrame::OnOpenClose_PetHotKeyChange(DWORD dwID, DWORD key1, DWORD key2)
{
	m_pBn_Pet->SetTips( ToolTipCreator::Inst()->GetHotKeyTips( m_strTip_Pet.c_str(), key1, key2 ).c_str() );

	tstring strMenuText = ToolTipCreator::Inst()->GetHotKeyTips( m_strMenuExItemText[EMI_Pet].c_str(), key1, key2, g_strMenuExSpace.c_str(), false );
	m_pListMenuEx->SetText( EMI_Pet, 0, strMenuText.c_str() );
	UpdataMenuSize();

	return 0;
}

DWORD SysMenuFrame::OnOpenClose_SocialityHotKeyChange(DWORD dwID, DWORD key1, DWORD key2)
{
	m_pBn_Sociality->SetTips( ToolTipCreator::Inst()->GetHotKeyTips( m_strTip_Sociality.c_str(), key1, key2 ).c_str() );

	tstring strMenuText = ToolTipCreator::Inst()->GetHotKeyTips( m_strMenuExItemText[EMI_Sociality].c_str(), key1, key2, g_strMenuExSpace.c_str(), false );
	m_pListMenuEx->SetText( EMI_Sociality, 0, strMenuText.c_str() );
	UpdataMenuSize();

	return 0;
}

DWORD SysMenuFrame::OnOpenClose_HelpHotKeyChange(DWORD dwID, DWORD key1, DWORD key2)
{
	tstring strMenuText = ToolTipCreator::Inst()->GetHotKeyTips( m_strMenuExItemText[EMI_Help].c_str(), key1, key2, g_strMenuExSpace.c_str(), false );
	m_pListMenuEx->SetText( EMI_Help, 0, strMenuText.c_str() );
	UpdataMenuSize();

	return 0;
}

DWORD SysMenuFrame::OnOpenClose_ZoneMapHotKeyChange(DWORD dwID, DWORD key1, DWORD key2)
{
	tstring strMenuText = ToolTipCreator::Inst()->GetHotKeyTips( m_strMenuExItemText[EMI_ZoneMap].c_str(), key1, key2, g_strMenuExSpace.c_str(), false );
	m_pListMenuEx->SetText( EMI_ZoneMap, 0, strMenuText.c_str() );
	UpdataMenuSize();

	return 0;
}

DWORD SysMenuFrame::OnOpenClose_WorldMapHotKeyChange(DWORD dwID, DWORD key1, DWORD key2)
{
	tstring strMenuText = ToolTipCreator::Inst()->GetHotKeyTips( m_strMenuExItemText[EMI_WorldMap].c_str(), key1, key2, g_strMenuExSpace.c_str(), false );
	m_pListMenuEx->SetText( EMI_WorldMap, 0, strMenuText.c_str() );
	UpdataMenuSize();

	return 0;
}