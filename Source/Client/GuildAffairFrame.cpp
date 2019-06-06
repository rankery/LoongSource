#include "stdafx.h"
#include "GuildAffairFrame.h"
#include "GuildDefine.h"
#include "GuildMgr.h"
#include "CombatSysUtil.h"

GuildAffairFrame::GuildAffairFrame()
	: m_Trunk(this)
	, m_pWnd(NULL)
	, m_pListAffaires(NULL)
	, m_StcpTimes(NULL)
	, m_pBtnIssueQuest(NULL)
	, m_pBtnClose1(NULL)
	, m_pBtnClose2(NULL)
	, m_dwCurRow(0)
	, m_dwCurChooseRow(0)
	, m_byGuildLv(-1)
{
 
}

GuildAffairFrame::~GuildAffairFrame()
{
	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
}

BOOL GuildAffairFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName(), m_Trunk.sfp1(&GuildAffairFrame::EventHandler));
	m_pGameFrameMgr->RegisterEventHandle(_T("Refresh_GuildAffair"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildAffairFrame::OnEventRefresh));

	return TRUE;
}

BOOL GuildAffairFrame::Destroy()
{
	m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName(), m_Trunk.sfp1(&GuildAffairFrame::EventHandler));
	m_pGameFrameMgr->UnRegisterEventHandler(_T("Refresh_GuildAffair"),  (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildAffairFrame::OnEventRefresh));
	return GameFrame::Destroy();
}

BOOL GuildAffairFrame::ReloadUI()
{
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\guild_quest.xml");
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());

	m_pListAffaires = static_cast<GUIListBox*>(m_pWnd->GetChild(_T("guildquestback_pic\\listback_pic\\questlist")));
	m_StcpTimes = static_cast<GUIStatic*>(m_pWnd->GetChild(_T("guildquestback_pic\\times")));
	m_pBtnIssueQuest = static_cast<GUIButton*>(m_pWnd->GetChild(_T("guildquestback_pic\\surebutt")));
	m_pBtnClose1 = static_cast<GUIButton*>(m_pWnd->GetChild(_T("guildquestback_pic\\closebutt")));
	m_pBtnClose2 = static_cast<GUIButton*>(m_pWnd->GetChild(_T("guildquestback_pic\\closebutt2")));

	m_pListAffaires->SetColNum(4, 20);
	m_pListAffaires->SetColWidth(0, 93);
	m_pListAffaires->SetColWidth(1, 304);
	m_pListAffaires->SetColWidth(2, 99);
	m_pListAffaires->SetColWidth(3, 112);

	m_byGuildLv = GuildMgr::Inst()->GetGuildInfo()->byLevel;

	ShowQuest();

	const tagGuildBase *pBase = GuildMgr::Inst()->GetGuildInfo();
	tstringstream stream; 
	stream << pBase->byAffairRemainTimes;
	tstring strTimes;
	stream >> strTimes;
	m_StcpTimes->SetText(strTimes.c_str());
	
	return TRUE;
}

BOOL GuildAffairFrame::EscCancel()
{
	if (P_VALID(m_pWnd) && !m_pWnd->IsInvisible())
	{
		m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_GuildAffair"), this));
		return TRUE;
	}
	return GameFrame::EscCancel();
}

DWORD GuildAffairFrame::EventHandler(tagGUIEvent *pEvent)
{
	GUIWnd *pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_ItemClick:
		{
			if (pWnd == m_pListAffaires)
			{
				m_dwCurChooseRow = pEvent->dwParam1;
			}
		} break;
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnIssueQuest)
			{
				if (m_dwCurChooseRow < m_vecAffairInfo.size())
				{
					const tagGuildAffairInfo *pInfo = m_vecAffairInfo[m_dwCurChooseRow];
					const tagGuildBase *pBase = GuildMgr::Inst()->GetGuildInfo();
					if (pBase->byAffairRemainTimes == 0)
					{
						CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildText_NotEnoughAffaires")]);
						return 0;
					}
					else if (pBase->nFund < pInfo->nFund || pBase->nMaterial < pInfo->nMaterial)
					{					
						CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildText_NotEnoughMoney")]);
						return 0;

					}
					else if (
						(GuildMgr::Inst()->IsInSpecState(EGDSS_Shortage) == true) || 
						(GuildMgr::Inst()->IsInSpecState(EGDSS_Distress) == true) ||
						(GuildMgr::Inst()->IsInSpecState(EGDSS_Chaos) == true) ||
						(GuildMgr::Inst()->IsInSpecState(EGDSS_Warfare) == true) 
						)
					{
						CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildText_StateError")]);
						return 0;
					}
					else
					{
						GuildMgr::Inst()->SendSpreadGuildAffair(pInfo->dwID);
					}
				}
			}
			if (pWnd == m_pBtnClose1 || pWnd == m_pBtnClose2)
			{
				m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_GuildAffair"), this));
			}
		} break;
	}
	return 0;
}

void GuildAffairFrame::ShowQuest()
{
	m_vecAffairInfo.clear();
	const std::vector<tagGuildAffairInfo*>& AffairInfo = GuildMgr::Inst()->GetAffairInfo();
	std::vector<tagGuildAffairInfo*>::const_iterator end = AffairInfo.end();
	for (std::vector<tagGuildAffairInfo*>::const_iterator iter = AffairInfo.begin(); iter != end; ++iter)
	{
		if (IsConditionSatisfaction(*iter))
			m_vecAffairInfo.push_back(*iter);
	}

	m_pListAffaires->Clear();
	for (m_dwCurRow = 0; m_dwCurRow < m_vecAffairInfo.size(); ++m_dwCurRow)
	{
		tagGuildAffairInfo *pInfo = m_vecAffairInfo[m_dwCurRow];
		m_pListAffaires->SetText(m_dwCurRow, 0, pInfo->strName.c_str());
		m_pListAffaires->SetText(m_dwCurRow, 1, pInfo->strDesc.c_str());
		
		tstring strFund;
		tstringstream stream;
		stream << pInfo->nFund;
		stream >> strFund;
		m_pListAffaires->SetText(m_dwCurRow, 2, strFund.c_str());
		stream.clear();
		tstring strMaterial;
		stream << pInfo->nMaterial;
		stream >> strMaterial;
		m_pListAffaires->SetText(m_dwCurRow, 3, strMaterial.c_str());
	}
}

bool GuildAffairFrame::IsConditionSatisfaction(tagGuildAffairInfo *pInfo)
{
	const tagGuildBase *pGuildBase = GuildMgr::Inst()->GetGuildInfo();
	if (static_cast<INT32>(pGuildBase->byLevel) < pInfo->nGuildLv)
	{
		return false;
	}
	
	bool b = false;
	for (INT32 i = 0; i < MAX_GUILD_HOLDCITY; ++i)
	{
		if (pInfo->nCity == static_cast<INT32>(pGuildBase->byHoldCity[i]))
		{
			b = true;
			break;
		}
	}
	if (!b)
		return false;

	return true;
}

DWORD GuildAffairFrame::OnEventRefresh(tagGameEvent *pEvent)
{
	if (GuildMgr::Inst()->GetGuildInfo()->byLevel != m_byGuildLv)
	{
		ShowQuest();
		m_byGuildLv = GuildMgr::Inst()->GetGuildInfo()->byLevel;
	}
	const tagGuildBase *pBase = GuildMgr::Inst()->GetGuildInfo();
	tstringstream stream; 
	stream << pBase->byAffairRemainTimes;
	tstring strTimes;
	stream >> strTimes;
	m_StcpTimes->SetText(strTimes.c_str());
	m_StcpTimes->SetRefresh(TRUE);
	return 0;
}
