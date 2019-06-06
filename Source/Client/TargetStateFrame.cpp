#include "stdafx.h"
#include "TargetStateFrame.h"
#include "ActiveStatic.h"
#include "RoleHeadPicIcon.h"
#include "WorldPickerFrame.h"
#include "RoleMgr.h"
#include "Role.h"
#include "Player.h"
#include "NPC.h"
#include "MapMgr.h"
#include "LocalPlayer.h"
#include "PlayerNameTab.h"
#include "RoleEvent.h"
#include "CreatureData.h"
#include "CombatEvent.h"
#include "ProfessionMgr.h"
#include "BuffUIList.h"
#include "EffectMgr.h"
#include "ChatEvent.h"
#include "TeamSys.h"
#include "SocialEvent.h"
#include "GameSet.h"
#include "GuildMgr.h"
#include "CombatSys.h"
#include "PetManager.h"
#include "Pet.h"
#include "TeamEvent.h"
#include "DispRoleInfoFrame.h"

TargetStateFrame::TargetStateFrame() :
m_Trunk( this ),
m_pWnd( NULL ),
m_pHead( NULL ),
m_pSt_PVP( NULL ),
m_pSt_Name( NULL ),
m_pSt_Level( NULL ),
m_pPro_Hp( NULL ),
m_pPro_Mp( NULL ),
m_pSt_AppendInfo( NULL ),
m_pBn_Menu( NULL ),
m_pWnd_Menu( NULL ),
m_pLb_Menu( NULL ),
m_pBuffUIList( NULL ),
m_dwCurShowRoleID( GT_INVALID ),
m_dwSelectEffectID( GT_INVALID ),
m_nMenuPanelHeightExt( 0 ),
m_nLastLevel( GT_INVALID ),
m_nLastMaxHp( GT_INVALID ),
m_nLastHp( GT_INVALID ),
m_nLastMaxMp( GT_INVALID ),
m_nLastMp( GT_INVALID ),
m_bLastNameGray( false )
{

}

TargetStateFrame::~TargetStateFrame()
{
}

BOOL TargetStateFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle( _T("tagRolePickEvent"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnRolePickEvent));
	m_pMgr->RegisterEventHandle( _T("tagRoleGetNameEvent"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnRoleGetNameEvent));
	m_pMgr->RegisterEventHandle( _T("tagUpdataAvatarEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnUpdataAvatarEvent));
	m_pMgr->RegisterEventHandle( _T("tagRoleBuffChangeEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnRoleBuffChangeEvent));
	m_pMgr->RegisterEventHandle( _T("tagUpdataAvatarDisplaySetEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnUpdataAvatarDisplaySetEvent));
    m_pMgr->RegisterEventHandle( _T("RemotePlayerSetClass"),	        (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnChangePlayerClass));

	return TRUE;
}

BOOL TargetStateFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler( _T("tagRolePickEvent"),				 (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnRolePickEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagRoleGetNameEvent"),			 (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnRoleGetNameEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagUpdataAvatarEvent"),			 (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnUpdataAvatarEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagRoleBuffChangeEvent"),		 (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnRoleBuffChangeEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagUpdataAvatarDisplaySetEvent"),(FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnUpdataAvatarDisplaySetEvent));
    m_pMgr->UnRegisterEventHandler( _T("RemotePlayerSetClass"),	         (FRAMEEVENTPROC)m_Trunk.sfp1(&TargetStateFrame::OnChangePlayerClass));

	GameFrame::Destroy();

	SafeDestroyUI();

	return TRUE;
}


BOOL TargetStateFrame::ReloadUI()
{
	SafeDestroyUI();

	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\target.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	// 修改类名以便创建自定义控件
	m_pGUI->ChangeXml( &element, "targetwin\\att\\facepic", "ClassName", "RoleHeadPicIcon");

	for( INT i = 0; i < BUFFUI_NUM; i++ )
	{
		std::stringstream streamBuffStaticName;
	    streamBuffStaticName<<"targetwin\\buff"<<i+1;
		m_pGUI->ChangeXml( &element, streamBuffStaticName.str().c_str(), "ClassName", "ActiveStatic");
	}

	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	if( !P_VALID( m_pWnd ) )
		return FALSE;

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&TargetStateFrame::GUIEventHandler));

	m_pHead				= (RoleHeadPicIcon*)m_pWnd->GetChild( _T("att\\facepic") );
	m_pSt_PVP			= (GUIStatic*    )m_pWnd->GetChild( _T("att\\attack") );
	m_pSt_Name			= (GUIStatic*)m_pWnd->GetChild( _T("att\\name") );
	m_pSt_Level			= (GUIStatic*)m_pWnd->GetChild( _T("att\\grade") );
	m_pPro_Hp			= (GUIProgress*)m_pWnd->GetChild( _T("att\\hp") );
	m_pPro_Mp			= (GUIProgress*)m_pWnd->GetChild( _T("att\\mp") );
	m_pSt_AppendInfo    = (GUIStatic*)m_pWnd->GetChild( _T("att\\ai") );
	m_pBn_Menu			= (GUIPushButton*)m_pWnd->GetChild( _T("att\\teambutt") );

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
	strPath = g_strLocalPath + _T("\\ui\\systemexpand.xml");
	XmlElement eleMenu;
	if( !m_pGUI->LoadXml( &eleMenu, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &eleMenu, "expand", "Name", "TargetMenu");
	tagPoint ptWnd = m_pWnd->GetClient();
	stringstream streamX,streamY;
	streamX<<m_pWnd->GetClient().x + m_pWnd->GetSize().x;
	streamY<<m_pWnd->GetClient().y + 20;
	m_pGUI->ChangeXml( &eleMenu, "TargetMenu", "AlignControl", "\\desktop");
	m_pGUI->ChangeXml( &eleMenu, "TargetMenu", "AlignType", "NoAlign");
	m_pGUI->ChangeXml( &eleMenu, "TargetMenu\\listbutton1", "AlignControl", "\\desktop\\TargetMenu");
	m_pGUI->ChangeXml( &eleMenu, "TargetMenu", "LogicPos_x", streamX.str().c_str() );
	m_pGUI->ChangeXml( &eleMenu, "TargetMenu", "LogicPos_y", streamY.str().c_str() );

	m_pWnd_Menu	= m_pGUI->CreateWnd( _T("\\desktop"), &eleMenu );
	m_pGUI->RegisterEventHandler(m_pWnd_Menu->GetFullName().c_str(), m_Trunk.sfp1(&TargetStateFrame::GUIEventHandler));

	m_pLb_Menu = (GUIListBox*)m_pWnd_Menu->GetChild( _T("listbutton1") );
	m_nMenuPanelHeightExt = m_pWnd_Menu->GetSize().y - m_pLb_Menu->GetSize().y;

	m_pWnd_Menu->SetInvisible( TRUE );
	m_pSt_PVP->SetInvisible( TRUE );
	m_pWnd->SetInvisible( TRUE );
	
	m_pHead->FlipToTop();
	m_pHead->SetTopMost( TRUE );

	m_nLastLevel = GT_INVALID;
	m_nLastMaxHp = GT_INVALID;
	m_nLastHp	 = GT_INVALID;
	m_nLastMaxMp = GT_INVALID;
	m_nLastMp	 = GT_INVALID;

	return TRUE;
}

VOID TargetStateFrame::SafeDestroyUI()
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
}

VOID TargetStateFrame::Update()
{
	if( GT_INVALID == m_dwCurShowRoleID )
		return;

	Role* pRole = RoleMgr::Inst()->FindRole( m_dwCurShowRoleID );
	if( P_VALID( pRole ) && !( pRole->IS_KIND_OF( NPC ) && pRole->IsDead() ))
	{
		UpdataRoleData( pRole );
	}
	else
	{
		// 角色出了视野或死亡NPC，取消选中
		tagRolePickEvent evt( _T("tagRolePickEvent"), this );
		evt.eType = EWPT_LClick;
		evt.dwRoleID = GT_INVALID;
		m_pMgr->SendEvent( &evt );
	}

	// 防止滚动菜单
	if( 0 != m_pLb_Menu->GetCurrentTextRow() )
		m_pLb_Menu->SetCurrentTextRow( 0 );

	GameFrame::Update();
}

VOID TargetStateFrame::Render3D()
{
	if( P_VALID( m_pHead ) )
		m_pHead->Render3D();
}

BOOL TargetStateFrame::EscCancel()
{
	if( !P_VALID( m_pWnd ) || m_pWnd->IsInvisible() )
		return FALSE;

	tagRolePickEvent evt( _T("tagRolePickEvent"), this );
	evt.eType = EWPT_LClick;
	evt.dwRoleID = GT_INVALID;
	m_pMgr->SendEvent( &evt );

	return TRUE;
}

DWORD TargetStateFrame::OnRolePickEvent(tagRolePickEvent* pEvent)
{
	if( GT_INVALID != pEvent->dwRoleID )
		OnOpen( pEvent->dwRoleID );
	else
		OnClose();

	return 0;
}

DWORD TargetStateFrame::OnRoleGetNameEvent(tagRoleGetNameEvent* pEvent)
{
	if( GT_INVALID != pEvent->dwRoleID && m_dwCurShowRoleID == pEvent->dwRoleID )
			m_pSt_Name->SetText( pEvent->strRoleName.c_str() );

	return 0;
}

DWORD TargetStateFrame::OnUpdataAvatarEvent(tagUpdataAvatarEvent* pEvent)
{
	if( GT_INVALID != m_dwCurShowRoleID && pEvent->dwRoleID == m_dwCurShowRoleID )
	{
		Role* pRole = RoleMgr::Inst()->FindRole( pEvent->dwRoleID );
		if( P_VALID( pRole ) && pRole->IsAttributeInited() )
			m_pHead->UpdataHeadPic( pRole );
	}
	return 0;
}

DWORD TargetStateFrame::OnRoleBuffChangeEvent(tagRoleBuffChangeEvent* pEvent)
{
	if( GT_INVALID != m_dwCurShowRoleID && pEvent->dwRoleID == m_dwCurShowRoleID )
	{
		Role* pRole = RoleMgr::Inst()->FindRole( m_dwCurShowRoleID );
		if( P_VALID( pRole ) )
			RebuildBuffs( pRole );
	}
	return 0;
}

VOID TargetStateFrame::RebuildBuffs( Role* pRole )
{
	ASSERT( P_VALID( pRole ) );

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

BOOL TargetStateFrame::GUIEventHandler(tagGUIEvent* pEvent)
{
	const GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID( pWnd ) )
		return FALSE;
	switch( pEvent->eEvent )
	{
	case EGUIE_Deactive:
		if( !m_pWnd_Menu->IsInvisible() &&
			pEvent->dwParam1 != (DWORD)m_pWnd_Menu &&
			pEvent->dwParam1 != (DWORD)m_pLb_Menu )
		{
			m_pWnd_Menu->SetInvisible( TRUE );
			m_pBn_Menu->SetState( EGUIBS_PopUp );
		}
		break;
	case EGUIE_PushButtonDown:
		if( pWnd == m_pBn_Menu )
		{
			if( 1 == pEvent->dwParam1 )
			{
				m_pWnd_Menu->SetInvisible( FALSE );
				UpdataMenuItem();
			}
			else
			{
				m_pWnd_Menu->SetInvisible( TRUE );
			}
		}
		break;
	case EGUIE_ItemClick:
		if( pWnd == m_pLb_Menu )
		{
			if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_PrivateChat] )
			{
				tagPrivateChatEvent evt( _T("tagPrivateChatEvent"), this );
				evt.dwRoleID = m_dwCurShowRoleID;
				evt.strRoleName = m_pSt_Name->GetText();
				m_pMgr->SendEvent( &evt );
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
			else if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_Friend] )
			{
				tagAddFriendEvent evt( _T("SocialEvent_AddFriend"), this );
				evt.roleID = m_dwCurShowRoleID;
				m_pMgr->SendEvent( &evt );
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
			else if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_Trade] )
			{
				tagFollowRoleEvent evt( _T("TargetEvent_ReqTrade"), this );
				evt.dwTargetRoleID = m_dwCurShowRoleID;
				m_pMgr->SendEvent( &evt );
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
			else if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_Follow] )
			{
				tagFollowRoleEvent evt( _T("tagFollowRoleEvent"), this );
				evt.dwTargetRoleID = m_dwCurShowRoleID;
				m_pMgr->SendEvent( &evt );
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
			else if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_TeamInvite] )
			{
				TeamSys::Inst()->InviteJoinTeam( m_dwCurShowRoleID );
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
			else if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_TeamApply] )
			{
				//TeamSys::Inst()->ApplyJoinTeam( m_dwCurShowRoleID );
				tagApplyJoinTeamEvent evt( _T("tagApplyJoinTeamEvent"), this );
				evt.dwRoleID = m_dwCurShowRoleID;
				m_pMgr->SendEvent( &evt );

				m_pWnd_Menu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
			else if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_FactionInvite] )
			{
				GuildMgr::Inst()->SendGuildJoinReq( m_dwCurShowRoleID );
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
			else if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_PlayerInfo] )
			{
                tagDispPlayerState e(_T("tagDispPlayerState"), this);
                e.dwRoleID = m_dwCurShowRoleID;
                m_pMgr->SendEvent(&e);
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
			else if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_PlayerInfo2] )
			{
				tagOpenRoleInfoEvent e(_T("Open_RoleInfo"), this);
				e.dwRoleID = m_dwCurShowRoleID;
				m_pMgr->SendEvent(&e);
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
			else if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_AddToBlackList] )
			{
				tagAddBlackEvent evt( _T("SocialEvent_AddBlack"), this );
				evt.roleID = m_dwCurShowRoleID;
				m_pMgr->SendEvent( &evt );
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
            else if( pEvent->dwParam1 == m_nCurMenuIndex[EMI_PetTrade] )
            {
                tagFollowRoleEvent evt( _T("TargetEvent_ReqPetTrade"), this );
                evt.dwTargetRoleID = m_dwCurShowRoleID;
                m_pMgr->SendEvent( &evt );
                m_pWnd_Menu->SetInvisible( TRUE );
                m_pBn_Menu->SetState( EGUIBS_PopUp );
            }
			else if( pEvent->dwParam1 == m_nCurMenuIndex[EMT_MountInvite] )//发送邀请驾御消息
			{
				CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
				tagNC_MountInvite msg;
				msg.dwDstRoleID	= pCombatSys->GetCurTargetID();
				TObjRef<NetSession>()->Send(&msg);
				m_pWnd_Menu->SetInvisible( TRUE );
				m_pBn_Menu->SetState( EGUIBS_PopUp );
			}
		}
		break;
	default:
		break;
	}

	m_pBuffUIList->GUIEventHandler( pEvent, pWnd, m_dwCurShowRoleID );
	return FALSE;
}


VOID TargetStateFrame::UpdataMenuItem()
{
	for( int i = 0; i < EMI_Num; i++ )
	{
		m_nCurMenuIndex[i] = GT_INVALID;
	}
	INT nMenuIndex = 0;
	SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMI_PrivateChat );
	m_nCurMenuIndex[EMI_PrivateChat] = nMenuIndex++;

	SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMI_Friend );
	m_nCurMenuIndex[EMI_Friend] = nMenuIndex++;

	SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMI_Trade );
	m_nCurMenuIndex[EMI_Trade] = nMenuIndex++;

	SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMI_Follow );
	m_nCurMenuIndex[EMI_Follow] = nMenuIndex++;

	//如果选中的不是本地玩家,选中的是远程玩家
	if( RoleMgr::Inst()->GetLocalPlayerID() != m_dwCurShowRoleID )
	{
		//本地玩家没有队伍 或者 是队长可以邀请其他玩家加入队伍
		if( TeamSys::Inst()->GetTeammateNum() == 0 || 
			( TeamSys::Inst()->IsLeader( RoleMgr::Inst()->GetLocalPlayerID() ) && !TeamSys::Inst()->IsTeamFull() && !TeamSys::Inst()->IsTeammate( m_dwCurShowRoleID ) ) )
		{
			SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMI_TeamInvite );
			m_nCurMenuIndex[EMI_TeamInvite] = nMenuIndex++;
		}

		//本地玩家没有队伍，可以申请加入其他队伍
		if( !TeamSys::Inst()->IsInTeam() )
		{
			SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMI_TeamApply );
			m_nCurMenuIndex[EMI_TeamApply] = nMenuIndex++;
		}
	}
	
	if( GuildMgr::Inst()->IsCanJionReq( m_dwCurShowRoleID ))// 如果可以邀请该玩家加入帮派，则显示帮派邀请
	{
		SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMI_FactionInvite );
		m_nCurMenuIndex[EMI_FactionInvite] = nMenuIndex++;
	}

	SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMI_PlayerInfo );
	m_nCurMenuIndex[EMI_PlayerInfo] = nMenuIndex++;

	SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMI_PlayerInfo2 );
	m_nCurMenuIndex[EMI_PlayerInfo2] = nMenuIndex++;

	SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMI_AddToBlackList );
	m_nCurMenuIndex[EMI_AddToBlackList] = nMenuIndex++;

    SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMI_PetTrade );
	m_nCurMenuIndex[EMI_PetTrade] = nMenuIndex++;

	// 正在驾御灵兽，当前驾御人数少于可驾御人数，被邀请的对象在自己周围10格之内
	Pet *pPet = RoleMgr::Inst()->GetLocalPlayer()->GetMount();
	Role *pTarget = NULL;
	CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
	map<DWORD,Role*>::const_iterator finder = RoleMgr::Inst()->GetRoleMap().find(pCombatSys->GetCurTargetID());
	if( finder != RoleMgr::Inst()->GetRoleMap().end() )
		pTarget = finder->second;
	if( P_VALID(pPet) 
		&& (PetManager::Inst()->GetPetCurrMountNum(pPet->GetID()) < PetManager::Inst()->GetPetMaxMountNum(pPet->GetID()))
		&& P_VALID(pTarget) && pTarget->IS_STATIC_CLASS(Player) )
	{
		if( Vec3DistSq( RoleMgr::Inst()->GetLocalPlayer()->GetPos(), pTarget->GetPos() ) < 250000 )
		{
			SetMenuFromStrTable( m_pLb_Menu, _T("TargetMenu_"), nMenuIndex, EMT_MountInvite );
			m_nCurMenuIndex[EMT_MountInvite] = nMenuIndex++;
		}
	}

	ForceResizeMenu( m_pLb_Menu, nMenuIndex );
}

VOID TargetStateFrame::OnOpen( const DWORD dwRoleID )
{
	if( dwRoleID == m_dwCurShowRoleID )
		return;

	OnClose();

	Role* pRole = RoleMgr::Inst()->FindRole( dwRoleID );
	if( !P_VALID( pRole ) || ( !pRole->IsAttributeInited() ) )
		return;
	if( pRole->IS_KIND_OF( NPC ) )
	{
		NPC* pNpc = (NPC*)pRole;
		if( !pNpc->IsMonster() && !pNpc->IsNPC() )
			return;
	}
	m_pWnd->SetInvisible( FALSE );
	m_pHead->UpdataHeadPic( pRole );

	UpdataRoleData( pRole );
	
	m_dwCurShowRoleID = dwRoleID;
	if( IS_PLAYER( m_dwCurShowRoleID ) )
	{
		m_pSt_AppendInfo->SetInvisible( FALSE );

		m_pSt_PVP->SetInvisible( TRUE );
		if( RoleMgr::Inst()->GetLocalPlayer()->GetID() == m_dwCurShowRoleID )
			m_pBn_Menu->SetEnable( false );
		else
			m_pBn_Menu->SetEnable( true );

		m_dwSelectEffectID = EffectMgr::Inst()->PlayRoleEffect( dwRoleID, _T("Txqt01_1"), NULL, EffectMgr::EEST_XZ );
	}
	else
	{
		m_pSt_AppendInfo->SetInvisible( TRUE );
		m_pSt_PVP->SetInvisible( TRUE );	
		m_pBn_Menu->SetEnable( false );
		if( pRole->IS_KIND_OF( NPC ) )
		{
			NPC* pNpc = (NPC*)pRole;
			if( pNpc->IsMonster() )
				m_dwSelectEffectID = EffectMgr::Inst()->PlayRoleEffect( dwRoleID, _T("Txqt01_2"), NULL, EffectMgr::EEST_XZ );
			else
				m_dwSelectEffectID = EffectMgr::Inst()->PlayRoleEffect( dwRoleID, _T("Txqt01_1"), NULL, EffectMgr::EEST_XZ );
		}
	}
	// 设置职业
	SetPlayerClassName(m_dwCurShowRoleID);
	RebuildBuffs( pRole );
}

VOID TargetStateFrame::OnClose()
{
	m_dwCurShowRoleID = GT_INVALID;
	m_pWnd->SetInvisible( TRUE );
	m_pWnd_Menu->SetInvisible( TRUE );
	EffectMgr::Inst()->StopEffect( m_dwSelectEffectID );
	m_dwSelectEffectID = GT_INVALID;

	// 恢复名字颜色
	m_pSt_Name->SetTextColor( 0xFFFFF7E0 );
	m_bLastNameGray = false;
}

VOID TargetStateFrame::UpdataPlayerData( Player* pPlayer )
{
	// PVP图标
	m_pSt_PVP->SetInvisible( !pPlayer->GetRoleState( ERS_PVP ) );
}

VOID TargetStateFrame::UpdataNPCData( NPC* pNpc )
{
	const bool bGray = !pNpc->IsBelongToLP();
	if( bGray != m_bLastNameGray )
	{
		m_pSt_Name->SetTextColor( bGray ? 0xFFC0C0C0 : 0xFFFFF7E0 );
		m_pSt_Name->SetRefresh( TRUE );
		m_bLastNameGray = bGray;
	}
}

VOID TargetStateFrame::UpdataRoleData( Role* pRole )
{
	ASSERT( P_VALID( pRole ) );

	// 显示/隐藏 HP MP文字
	if( GameSet::Inst()->Get( EGS_ShowHPMPText ) )
	{
		if( m_pPro_Hp->GetText().empty() )
		{
			m_nLastMaxHp = GT_INVALID;
			m_nLastHp	 = GT_INVALID;
			m_nLastMaxMp = GT_INVALID;
			m_nLastMp	 = GT_INVALID;
		}
	}
	else
	{
		if( !m_pPro_Hp->GetText().empty() )
		{
			m_pPro_Hp->SetText( _T("") );
			m_pPro_Mp->SetText( _T("") );

			// 文字超出了控件，所以画布也需要刷新
			m_pPro_Mp->SetRefresh( TRUE );
		}
	}

	if( pRole->IS_KIND_OF(Player) )
	{
		UpdataPlayerData( (Player*)pRole );
	}
	else if( pRole->IS_KIND_OF(NPC) )
	{
		UpdataNPCData( (NPC*)pRole );
	}

	// 角色名字
	if( pRole->GetID() != m_dwCurShowRoleID )
	{
		m_pSt_Name->SetText( pRole->GetRoleName().c_str() );
		m_pSt_Name->SetRefresh( TRUE );
	}

	const int nLevel    = pRole->GetRoleLevel();
	if( nLevel != m_nLastLevel )
	{
		tstringstream stream;
		stream<<nLevel;
		m_pSt_Level->SetText( stream.str().c_str() );
		m_pSt_Level->SetRefresh( TRUE );
		m_nLastLevel = nLevel;
	}

	const int nMaxHp	= pRole->GetAttribute( ERA_MaxHP );
	const int nHp		= pRole->GetAttribute( ERA_HP );
	const int nMaxMp	= pRole->GetAttribute( ERA_MaxMP );
	const int nMp		= pRole->GetAttribute( ERA_MP );
	if( nMaxHp != m_nLastMaxHp || nHp != m_nLastHp )
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
		m_nLastMaxHp = nMaxHp;
		m_nLastHp	 = nHp;
	}

	if( nMaxMp != m_nLastMaxMp || nMp != m_nLastMp )
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
		m_nLastMaxMp = nMaxMp;
		m_nLastMp    = nMp;
	}
	m_pBuffUIList->Updata();
}

VOID TargetStateFrame::ForceResizeMenu( GUIListBox* pListBox, const INT nNumRow )
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

VOID TargetStateFrame::SetMenuFromStrTable( GUIListBox* pListBox, const TCHAR* szBaseKey, const INT nLbIndex, const INT nStrIndex )
{
	tstringstream stream;
	stream<<szBaseKey<<nStrIndex;
	tstring strText = g_StrTable[ stream.str().c_str() ];
	pListBox->SetText( nLbIndex, 0, strText.c_str() );
}


VOID TargetStateFrame::SetMenuFromStrTable( GUIListBox* pListBox, const TCHAR* szBaseKey, const INT nNumRow, const INT nStartRow, const INT nEndRow, const BOOL bAutoResize )
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

DWORD TargetStateFrame::OnUpdataAvatarDisplaySetEvent(tagUpdataAvatarDisplaySetEvent* pEvent)
{
	if( pEvent->dwRoleID == m_dwCurShowRoleID )
	{
		Role* pRole = RoleMgr::Inst()->FindRole( pEvent->dwRoleID );
		if( P_VALID( pRole ) && pRole->IS_KIND_OF(Player) )
		{
			Player* pPlayer = (Player*)pRole;
			m_pHead->UpdataDisplaySet( pPlayer->GetDisplaySet() );
		}
	}
	return 0;
}

VOID TargetStateFrame::SetPlayerClassName( DWORD dwRoleID )
{
    Role* pRole = RoleMgr::Inst()->FindRole(dwRoleID);
	if( pRole )
	{
		if (pRole->IS_KIND_OF(Player))
		{
			Player* pPlayer = static_cast<Player*>(pRole);
			m_pSt_AppendInfo->SetText(ProfessionMgr::Inst().GetClassName(pPlayer->GetPlayerClass()));
		}
		else if( pRole->IS_KIND_OF(NPC) )
		{
			NPC *pNpc = static_cast<NPC*>(pRole);
			if( pNpc->GetProto()->szMonsterAI != NULL )
			{
				m_pSt_AppendInfo->SetInvisible( FALSE );
				m_pSt_AppendInfo->SetText( pNpc->GetProto()->szMonsterAI );
				m_pSt_AppendInfo->SetRefresh(TRUE);
			}
		}
	}
}

DWORD TargetStateFrame::OnChangePlayerClass( tagGameEvent* pEvent )
{
    if (m_pWnd->IsInvisible())
        return 0;

    SetPlayerClassName(m_dwCurShowRoleID);

    return 0;
}