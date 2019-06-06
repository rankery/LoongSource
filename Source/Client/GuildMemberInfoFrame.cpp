#include "StdAfx.h"
#include "GuildMemberInfoFrame.h"
#include "PlayerNameTab.h"
#include "GuildMgr.h"
#include "GuildEvent.h"
#include "SocialEvent.h"
#include "ChatEvent.h"
#include "TeamEvent.h"
#include "ProfessionMgr.h"
#include "HttpPicEx.h"
#include "PlayerNameTab.h"
#include "..\WorldDefine\guild_define.h"

GuildMemberInfoFrame::GuildMemberInfoFrame(void):m_Trunk(this)
{
	m_pWnd			= NULL;
	m_pWndInfo		= NULL;
	m_pBtnClose		= NULL;
	m_pBtnFriend	= NULL;
	m_pBtnTeam		= NULL;
	m_pBtnChat		= NULL;
	m_pBtnReflash	= NULL;
	m_pStcImage		= NULL;
	m_pStcName		= NULL;
	m_pStcPosition	= NULL;
	m_pStcLevel		= NULL;
	m_pStcSex		= NULL;
	m_pStcClass		= NULL;
	m_pStcCont		= NULL;
	m_pStcTotalCont	= NULL;
	m_pStcExploit	= NULL;
	m_pStcKnowledge	= NULL;

	m_dwRoleID		= GT_INVALID;
}

GuildMemberInfoFrame::~GuildMemberInfoFrame(void)
{
}

BOOL GuildMemberInfoFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
	
	// 注册游戏事件处理函数
	m_pMgr->RegisterEventHandle(_T("GuildEvent_InfoReflash"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGuildInfoReflash));
	m_pMgr->RegisterEventHandle(_T("GuildEvent_InfoUpdate"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGuildInfoUpdate));
	m_pMgr->RegisterEventHandle(_T("GuildEvent_PosChange"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGuildPosChange));
	m_pMgr->RegisterEventHandle(_T("GuildEvent_RemoveMember"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGuildRemoveMember));

	
	const tagGuildMemberClient* pMember = GuildMgr::Inst()->GetMember((INT)dwParam);
	if(P_VALID(pMember))
	{
		ReflashInfo(pMember);
		
	}

	m_pMgr->RegisterEventHandle(_T("GuildEvent_InfoReflash"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGuildInfoUpdate));
	m_pMgr->RegisterEventHandle(_T("tagGetHeadpicByID"), (FRAMEEVENTPROC)(m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGetHeadPicByID)));
	
	return bRet;
}

BOOL GuildMemberInfoFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();
	
	// 注销游戏事件处理函数
	m_pMgr->UnRegisterEventHandler(_T("GuildEvent_InfoReflash"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGuildInfoReflash));
	m_pMgr->UnRegisterEventHandler(_T("GuildEvent_InfoUpdate"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGuildInfoUpdate));
	m_pMgr->UnRegisterEventHandler(_T("GuildEvent_PosChange"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGuildPosChange));
	m_pMgr->UnRegisterEventHandler(_T("GuildEvent_RemoveMember"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGuildRemoveMember));
	
	if(P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
	}

	m_pMgr->UnRegisterEventHandler(_T("GuildEvent_InfoReflash"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGuildInfoUpdate));
	m_pMgr->UnRegisterEventHandler(_T("tagGetHeadpicByID"), (FRAMEEVENTPROC)(m_Trunk.sfp1(&GuildMemberInfoFrame::OnEventGetHeadPicByID)));
	return bRet;
}

BOOL GuildMemberInfoFrame::ReloadUI()
{
	BOOL bRet = GameFrame::ReloadUI();
	//创建
	tstring strPath = g_strLocalPath + _T("\\ui\\partymember_information.xml");

	XmlElement ele;
	if (!m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str()))
		return FALSE;

	m_pGUI->ChangeXml(&ele, "pminformation_win\\pminfo_backpic\\pminfo_back\\head_back\\head", "ClassName", "HttpPicEx");
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName(), (VUIEVENTHANDLE)m_Trunk.sfp1(&GuildMemberInfoFrame::EventHandler));

	m_pBtnClose		= (GUIButton*)m_pWnd->GetChild(_T("pminfo_backpic\\closebutt"));
	m_pWndInfo		= m_pWnd->GetChild(_T("pminfo_backpic\\pminfo_back"));
	m_pBtnFriend	= (GUIButton*)m_pWndInfo->GetChild(_T("butt1"));
	m_pBtnTeam		= (GUIButton*)m_pWndInfo->GetChild(_T("butt2"));
	m_pBtnChat		= (GUIButton*)m_pWndInfo->GetChild(_T("butt3"));
	m_pBtnReflash	= (GUIButton*)m_pWndInfo->GetChild(_T("butt4"));
	m_pStcImage		= (HttpPicEx*)m_pWndInfo->GetChild(_T("head_back\\head"));
	m_pStcName		= (GUIStatic*)m_pWndInfo->GetChild(_T("name"));
	m_pStcPosition	= (GUIStatic*)m_pWndInfo->GetChild(_T("job"));
	m_pStcLevel		= (GUIStatic*)m_pWndInfo->GetChild(_T("grade"));
	m_pStcSex		= (GUIStatic*)m_pWndInfo->GetChild(_T("sex"));
	m_pStcClass		= (GUIStatic*)m_pWndInfo->GetChild(_T("occupation"));
	m_pStcCont		= (GUIStatic*)m_pWndInfo->GetChild(_T("num1"));
	m_pStcTotalCont	= (GUIStatic*)m_pWndInfo->GetChild(_T("num2"));
	m_pStcKnowledge	= (GUIStatic*)m_pWndInfo->GetChild(_T("num3"));
	m_pStcExploit	= (GUIStatic*)m_pWndInfo->GetChild(_T("num4"));


	return bRet;
}

DWORD GuildMemberInfoFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if(pWnd == m_pBtnClose)
			{
				m_pMgr->AddToDestroyList(this);
			}
			else if(pWnd == m_pBtnFriend)
			{
				tagAddFriendEvent event(_T("SocialEvent_AddFriend"), this);
				event.roleID = m_dwRoleID;
				m_pMgr->SendEvent(&event);
			}
			else if(pWnd == m_pBtnTeam)
			{
				tagInviteJoinTeamEvent event(_T("tagInviteJoinTeamEvent"), this);
				event.dwRoleID = m_dwRoleID;
				m_pMgr->SendEvent(&event);
			}
			else if(pWnd == m_pBtnChat)
			{
				tagPrivateChatEvent event(_T("tagPrivateChatEvent"), this);
				event.dwRoleID = m_dwRoleID;
				event.strRoleName = PlayerNameTab::Inst()->FindNameByID(m_dwRoleID);
				m_pMgr->SendEvent(&event);
			}
			else if(pWnd == m_pBtnReflash)
			{
				tagGuildInfoReflashEvent event(_T("GuildEvent_ReflashReq"), this);
				event.dwRoleID;
				m_pMgr->SendEvent(&event);
			}
		}
		break;
	}
	return 0;
}

VOID GuildMemberInfoFrame::ReflashInfo( const tagGuildMemberClient* pMember )
{
	if(P_VALID(pMember))
	{
		m_dwRoleID = pMember->sInfo.dwRoleID;
		
		TCHAR szTmp[X_SHORT_NAME];

		m_pStcName->SetText(PlayerNameTab::Inst()->FindNameByID(m_dwRoleID));
		
		m_pStcPosition->SetText(GuildMgr::Inst()->GetMemberPos(pMember->sInfo.n8GuildPos).c_str());
		
		_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("Role_Level")], pMember->sInfo.byLevel);
		m_pStcLevel->SetText(szTmp);
		
		m_pStcSex->SetText(GuildMgr::Inst()->GetMemberSex(pMember->sInfo.bySex).c_str());

		m_pStcClass->SetText(ProfessionMgr::Inst().GetClassName((EClassType)(pMember->sInfo.byClass)));

		_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), pMember->sInfo.nCurContrib);
		m_pStcCont->SetText(szTmp);
		_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), pMember->sInfoEx.nTotalContrib);
		m_pStcTotalCont->SetText(szTmp);
		if (pMember->sInfoEx.nKnowledge != GT_INVALID)
		{
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), pMember->sInfoEx.nKnowledge);
			m_pStcKnowledge->SetText(szTmp);
		}
		else
			m_pStcKnowledge->SetText(_T(""));
		_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), pMember->sInfoEx.nExploit);
		m_pStcExploit->SetText(szTmp);
		
		// 请求头像
		tstring strPic = PlayerNameTab::Inst()->FindHeadPicURLbyID(pMember->sInfo.dwRoleID);
		if (!strPic.empty())
		{
			m_pStcImage->SetPicURL(strPic.c_str());
		}
		else
		{
			if (pMember->sInfo.bySex == ES_Woman)
				m_pStcImage->SetDefaultPic(_T("data\\ui\\state\\l_status-image-female.bmp"));
			if (pMember->sInfo.bySex == ES_Man)
				m_pStcImage->SetDefaultPic(_T("data\\ui\\state\\l_status-image-male.bmp"));
		}

		m_pWndInfo->SetRefresh(true);
	}
}

DWORD GuildMemberInfoFrame::OnEventGuildInfoUpdate( tagGuildInfoReflashEvent* pGameEvent )
{
	// 强制刷新当前信息
	const tagGuildMemberClient* pMember = GuildMgr::Inst()->GetMemberByID(pGameEvent->dwRoleID);
	if(P_VALID(pMember))
	{
		ReflashInfo(pMember);
	}
	return 0;
}

DWORD GuildMemberInfoFrame::OnEventGuildInfoReflash( tagGuildInfoReflashEvent* pGameEvent )
{
	// 是同一个成员才刷新
	if(pGameEvent->dwRoleID == m_dwRoleID)
	{
		const tagGuildMemberClient* pMember = GuildMgr::Inst()->GetMemberByID(pGameEvent->dwRoleID);
		if(P_VALID(pMember))
		{
			ReflashInfo(pMember);
		}
	}
	return 0;
}

DWORD GuildMemberInfoFrame::OnEventGuildPosChange( tagGuildInfoReflashEvent* pGameEvent )
{
	// 是同一个成员则刷新职位
	if(pGameEvent->dwRoleID == m_dwRoleID)
	{
		const tagGuildMemberClient* pMember = GuildMgr::Inst()->GetMemberByID(pGameEvent->dwRoleID);
		if(P_VALID(pMember))
		{
			m_pStcPosition->SetText(GuildMgr::Inst()->GetMemberPos(pMember->sInfo.n8GuildPos).c_str());
			m_pStcPosition->SetRefresh(true);
		}
	}
	return 0;
}

DWORD GuildMemberInfoFrame::OnEventGuildRemoveMember( tagGuildInfoReflashEvent* pGameEvent )
{
	// 是同一个成员则关闭窗口
	if(pGameEvent->dwRoleID == m_dwRoleID)
	{
		m_pMgr->AddToDestroyList(this);
	}
	return 0;
}

BOOL GuildMemberInfoFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

	m_pMgr->AddToDestroyList(this);
	return TRUE;
}

DWORD GuildMemberInfoFrame::OnEventGetHeadPicByID(tagGetHeadpicByID *pEvent)
{
	if (pEvent->dwRoleID == m_dwRoleID)
	{
		m_pStcImage->SetPicURL(pEvent->strImageURL.c_str());
		m_pStcImage->SetRefresh();
	}
	return 0;
}