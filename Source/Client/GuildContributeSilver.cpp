#include "stdafx.h"
#include "..\WorldDefine\guild_define.h"
#include "..\WorldDefine\currency_define.h"
#include "CurrencyMgr.h"
#include "CombatSysUtil.h"
#include "GuildMgr.h"
#include "CombatSysUtil.h"
#include "GuildDefine.h"
#include "GuildEvent.h"
#include "GuildContributeSilver.h"


GuildContributeSilverFrame::GuildContributeSilverFrame()
	: m_Trunk(this)
	, m_pWnd(NULL)
	, m_pStcGrade(NULL)
	, m_pStcGold(NULL)
	, m_pStcSilver(NULL)
	, m_pStcAimSilver(NULL)
	, m_pStcOwnSilver(NULL)
	, m_pCurrentCommerceInfo(NULL)
	, m_nAcceptLevel(0)
{
}

GuildContributeSilverFrame::~GuildContributeSilverFrame()
{
	if (m_pWnd)
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
}

BOOL GuildContributeSilverFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName(), (VUIEVENTHANDLE)m_Trunk.sfp1(&GuildContributeSilverFrame::EventHandler));
	m_pGameFrameMgr->RegisterEventHandle(_T("Refresh_ContributeSilver"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildContributeSilverFrame::OnEventContributeSilverRefresh));
	return TRUE;
}

BOOL GuildContributeSilverFrame::Destroy()
{
	m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName(), (VUIEVENTHANDLE)m_Trunk.sfp1(&GuildContributeSilverFrame::EventHandler));
	m_pGameFrameMgr->UnRegisterEventHandler(_T("Refresh_ContributeSilver"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildContributeSilverFrame::OnEventContributeSilverRefresh));
	return GameFrame::Destroy();
}

BOOL GuildContributeSilverFrame::ReloadUI()
{
	tstring strPath = g_strLocalPath + _T("\\ui\\guild_contributesilver.xml");
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());
	m_pStcGrade = (GUIStatic*)m_pWnd->GetChild(_T("cbsback_pic\\cbsback\\grade"));
	m_pStcGold = (GUIStatic*)m_pWnd->GetChild(_T("cbsback_pic\\cbsback\\num3"));
	m_pStcSilver = (GUIStatic*)m_pWnd->GetChild(_T("cbsback_pic\\cbsback\\num4"));
	m_pStcAimSilver = (GUIStatic*)m_pWnd->GetChild(_T("cbsback_pic\\cbsback\\num1"));
	m_pStcOwnSilver = (GUIStatic*)m_pWnd->GetChild(_T("cbsback_pic\\cbsback\\num2"));
	m_pBtnContribute = (GUIButton*)m_pWnd->GetChild(_T("cbsback_pic\\cbsbutt0"));
	m_pBtnCancel = (GUIButton*)m_pWnd->GetChild(_T("cbsback_pic\\cbsbutt1"));
	m_pWnd->SetInvisible(TRUE);
	return TRUE;
}

BOOL GuildContributeSilverFrame::EscCancel()
{
	if (P_VALID(m_pWnd) && !m_pWnd->IsInvisible())
	{
		m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_ContributeSilver"), this));
		return TRUE;
	}
	return GameFrame::EscCancel();
}

DWORD GuildContributeSilverFrame::EventHandler(tagGUIEvent *pEvent)
{
	GUIWnd *pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnContribute)
			{
				// 如果帮派当前处于征战状态
				if (GuildMgr::Inst()->IsInSpecState(EGDSS_Warfare))
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildCommerceError_Code5")]);
					return 0;
				}
				// 如果当前拥有商银少于目标商银时候不可以交付
				if (GuildMgr::Inst()->GetCurrentOwnCommerceSilver() < m_pCurrentCommerceInfo->nPurposeTael)
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildCommerceError_Code6")], m_pCurrentCommerceInfo->nPurposeTael);
					return 0;
				}
				GuildMgr::Inst()->SendCompleteCommerce();
				m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_ContributeSilver"), this));

			}
			else if (pWnd == m_pBtnCancel)
			{
				m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_ContributeSilver"), this));
			}
		}
	}
	return 0;
}

DWORD GuildContributeSilverFrame::OnEventContributeSilverRefresh(tagRefreshContributeSilverEvent *pEvent)
{
	TCHAR buffer[512];
	_sntprintf(buffer, 512, _T("%d"), pEvent->nLevel);
	m_pStcGrade->SetText(buffer);
	const tagCommerceInfo *pInfo = pEvent->pCommerceInfo;
	INT32 nGold = MSilver2DBGold(pInfo->nDeposit);
	INT32 nSilver = MSilver2DBSilver(pInfo->nDeposit);
	_sntprintf(buffer, 512, _T("%d"), nGold);
	m_pStcGold->SetText(buffer);
	_sntprintf(buffer, 512, _T("%d"), nSilver);
	m_pStcSilver->SetText(buffer);
	_sntprintf(buffer, 512, _T("%d"), pInfo->nPurposeTael);
	m_pStcAimSilver->SetText(buffer);
	INT32 nOwnCommerceSilver = GuildMgr::Inst()->GetCurrentOwnCommerceSilver();
	_sntprintf(buffer, 512, _T("%d"), nOwnCommerceSilver);
	m_pStcOwnSilver->SetText(buffer);
	
	// 暂时由界面保存这些信息
	m_nAcceptLevel = pEvent->nLevel;
	m_pCurrentCommerceInfo = pEvent->pCommerceInfo;
	m_pWnd->SetInvisible(FALSE);
	return 0;
}