#include "stdafx.h"
#include "GuildEvent.h"
#include "PlayerNameTab.h"
#include "GuildBusinessRankFrame.h"
#include "GuildMgr.h"
#include "CombatSysUtil.h"


GuildBusinessRankFrame::GuildBusinessRankFrame()
	: m_Trunk(this)
	, m_pWnd(NULL)
	, m_pListRank(NULL)
	, m_pStcCommendationState(NULL)
	, m_pBtnCommendationOpen(NULL)
	, m_pBtnCommendationClose(NULL)
	, m_pBtnClose(NULL)
	, m_bCommend(FALSE)
	, m_bShow(false)
{
}

GuildBusinessRankFrame::~GuildBusinessRankFrame()
{
	if (m_pWnd)
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
}

BOOL GuildBusinessRankFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName(), (VUIEVENTHANDLE)m_Trunk.sfp1(&GuildBusinessRankFrame::EventHandler));
	m_pGameFrameMgr->RegisterEventHandle(_T("Refresh_GuildBusinessRank"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessRankFrame::OnRefreshEvent));
	m_pGameFrameMgr->RegisterEventHandle(_T("Refresh_GuildBusinessRankCommendState"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessRankFrame::OnRefreshStateEvent));
	m_pGameFrameMgr->RegisterEventHandle(_T("GetSomeNameOK"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessRankFrame::OnGetSomeNameOKEvent));
	return TRUE;
}

BOOL GuildBusinessRankFrame::Destroy()
{
	m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName(), (VUIEVENTHANDLE)m_Trunk.sfp1(&GuildBusinessRankFrame::EventHandler));
	m_pGameFrameMgr->UnRegisterEventHandler(_T("Refresh_GuildBusinessRank"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessRankFrame::OnRefreshEvent));
	m_pGameFrameMgr->UnRegisterEventHandler(_T("Refresh_GuildBusinessRankCommendState"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessRankFrame::OnRefreshStateEvent));
	m_pGameFrameMgr->UnRegisterEventHandler(_T("GetSomeNameOK"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildBusinessRankFrame::OnGetSomeNameOKEvent));
	return GameFrame::Destroy();
}

BOOL GuildBusinessRankFrame::ReloadUI()
{
	tstring strPath = g_strLocalPath + _T("\\ui\\runforbusiness_ranking.xml");
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());

	m_pListRank = (GUIListBox*)m_pWnd->GetChild(_T("rfbrankingback_pic\\rfbranking_pic\\list"));
	m_pStcCommendationState = (GUIStatic*)m_pWnd->GetChild(_T("rfbrankingback_pic\\choose"));
	m_pBtnCommendationOpen = (GUIButton*)m_pWnd->GetChild(_T("rfbrankingback_pic\\butt0"));
	m_pBtnCommendationClose = (GUIButton*)m_pWnd->GetChild(_T("rfbrankingback_pic\\butt1"));
	m_pBtnClose = (GUIButton*)m_pWnd->GetChild(_T("rfbrankingback_pic\\closebutt"));
	
	m_pListRank->SetColNum(4, 20);
	m_pListRank->SetColWidth(0, 65);
	m_pListRank->SetColWidth(1, 108);
	m_pListRank->SetColWidth(2, 72);
	m_pListRank->SetColWidth(3, 146);
	// 先隐藏
	Show(false);
	return TRUE;
}

BOOL GuildBusinessRankFrame::EscCancel()
{
	if (P_VALID(m_pWnd) && !m_pWnd->IsInvisible())
	{
		m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_CommerceRank"), this));
		return TRUE;
	}
	return GameFrame::EscCancel();
}

DWORD GuildBusinessRankFrame::EventHandler(tagGUIEvent *pEvent)
{
	GUIWnd *pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnCommendationOpen)
			{
				EGuildMemberPos ePos = GuildMgr::Inst()->GetLpInfo()->eGuildPos;
				const tagGuildPower *pPower = GuildMgr::Inst()->GetPowerList(ePos);
				if (!pPower->bSetCommend)
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildCommerceError_Code9")]);
					return 0;
				}
				const tagGuildBase *pGuild = GuildMgr::Inst()->GetGuildInfo();
				if (P_VALID(pGuild))
				{
					if (pGuild->byLevel < 3)
					{
						CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildCommerceError_Code10")]);
						return 0;
					}

				}
				GuildMgr::Inst()->SendSwitchCommendation(TRUE);
			}
			else if (pWnd == m_pBtnCommendationClose)
			{
				EGuildMemberPos ePos = GuildMgr::Inst()->GetLpInfo()->eGuildPos;
				const tagGuildPower *pPower = GuildMgr::Inst()->GetPowerList(ePos);
				if (!pPower->bSetCommend)
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildCommerceError_Code9")]);
					return 0;
				}
				const tagGuildBase *pGuild = GuildMgr::Inst()->GetGuildInfo();
				if (P_VALID(pGuild))
				{
					if (pGuild->byLevel < 3)
					{
						CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildCommerceError_Code10")]);
						return 0;
					}
				}
				GuildMgr::Inst()->SendSwitchCommendation(FALSE);
			}
			else if (pWnd == m_pBtnClose)
			{
				m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_CommerceRank"), NULL));
			}
		}
	}
	return 0;
}

bool GuildBusinessRankFrame::IsShow() const
{
	return m_bShow;
}

void GuildBusinessRankFrame::Show(bool bShow)
{
	m_bShow = bShow;
	if (bShow)
		m_pWnd->SetInvisible(FALSE);
	else
		m_pWnd->SetInvisible(TRUE);
}

DWORD GuildBusinessRankFrame::OnRefreshStateEvent(tagRefreshBusinessRankCommendStateEvent *pEvent)
{
	m_bCommend = pEvent->bCommend;

	if (m_bCommend)
	{
		m_pStcCommendationState->SetText(g_StrTable[_T("GuildText_CommendOpen")]);
		m_pBtnCommendationOpen->SetEnable(false);
		m_pBtnCommendationClose->SetEnable(true);
	}
	else
	{
		m_pStcCommendationState->SetText(g_StrTable[_T("GuildText_CommendClose")]);
		m_pBtnCommendationOpen->SetEnable(true);
		m_pBtnCommendationClose->SetEnable(false);
	}
	m_pStcCommendationState->SetRefresh(TRUE);
	return 0;
}

DWORD GuildBusinessRankFrame::OnRefreshEvent(tagRefreshGuildBusinessRankEvent *pEvent)
{

	m_bCommend = pEvent->bCommend;
	
	if (m_bCommend)
	{
		m_pStcCommendationState->SetText(g_StrTable[_T("GuildText_CommendOpen")]);
		m_pBtnCommendationOpen->SetEnable(false);
		m_pBtnCommendationClose->SetEnable(true);
	}
	else
	{
		m_pStcCommendationState->SetText(g_StrTable[_T("GuildText_CommendClose")]);
		m_pBtnCommendationOpen->SetEnable(true);
		m_pBtnCommendationClose->SetEnable(false);
	}

	std::vector<tagCommerceRank>::const_iterator end = pEvent->vecRankInfo.end();
	for (std::vector<tagCommerceRank>::const_iterator iter = pEvent->vecRankInfo.begin(); 
		iter != end; ++iter)
	{
		// 先全部加入请求列表
		m_ReqIDList.push_back(iter->dwRoleID);
	}
	std::vector<DWORD> cantfind;
	// 看看能不能从playernametab中找到所有的名字
	bool bRet = PlayerNameTab::Inst()->CanGetBundleOfNames(m_ReqIDList, cantfind);
	// 如果能全部找到
	if (bRet)
	{
		int n = 0;
		std::vector<tagCommerceRank>::const_iterator end = pEvent->vecRankInfo.end();
		for (std::vector<tagCommerceRank>::const_iterator iter = pEvent->vecRankInfo.begin(); 
			iter != end; ++iter)
		{	
			TCHAR buffer[512];
			_sntprintf(buffer, 512, _T("%d"), n + 1);
			m_pListRank->SetText(n, 0, buffer);
			m_pListRank->SetText(n, 1, PlayerNameTab::Inst()->FindNameByID(iter->dwRoleID));
			_sntprintf(buffer, 512, _T("%d"), iter->nTimes);
			m_pListRank->SetText(n, 2, buffer);
			_sntprintf(buffer, 512, _T("%d"), iter->nTael);
			m_pListRank->SetText(n, 3, buffer);
			n++;
		}
		Show(true);
	}
	else
	{
		m_ReqIDList.clear();
		// 请求列表变为不能取得名字的列表
		m_ReqIDList.assign(cantfind.begin(), cantfind.end());
		m_vecSaveRank.clear();
		// 保存排名信息
		m_vecSaveRank.assign(pEvent->vecRankInfo.begin(), pEvent->vecRankInfo.end());
	}
	return 0;
}

DWORD GuildBusinessRankFrame::OnGetSomeNameOKEvent(tagRoleGetSomeNameEvent *pEvent)
{
	// 判断获得的名字是否是我请求的名字
	if (pEvent->IDs == m_ReqIDList)
	{
		int n = 0;
		std::vector<tagCommerceRank>::const_iterator end = m_vecSaveRank.end();
		for (std::vector<tagCommerceRank>::const_iterator iter = m_vecSaveRank.begin(); 
			iter != end; ++iter)
		{	
			TCHAR buffer[512];
			_sntprintf(buffer, 512, _T("%d"), n + 1);
			m_pListRank->SetText(n, 0, buffer);
			m_pListRank->SetText(n, 1, PlayerNameTab::Inst()->FindNameByID(iter->dwRoleID));
			_sntprintf(buffer, 512, _T("%d"), iter->nTimes);
			m_pListRank->SetText(n, 2, buffer);
			_sntprintf(buffer, 512, _T("%d"), iter->nTael);
			m_pListRank->SetText(n, 3, buffer);
			n++;
		}
		Show(true);
	}
	return 0;
}