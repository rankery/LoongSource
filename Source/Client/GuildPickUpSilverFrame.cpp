#include "stdafx.h"
#include "GuildEvent.h"
#include "GuildDefine.h"
#include "..\WorldDefine\currency_define.h"
#include "GuildPickUpSilverFrame.h"
#include "GuildMgr.h"
#include "RoleMgr.h"
#include "CurrencyMgr.h"
#include "LocalPlayer.h"
#include "CombatSysUtil.h"

GuildPickUpSilverFrame::GuildPickUpSilverFrame()
	: m_Trunk(this)
	, m_pWnd(NULL)
	, m_pStcGrade(NULL)
	, m_pStcGold(NULL)
	, m_pStcSilver(NULL)
	, m_pStcPickUpSilver(NULL)
	, m_pStcAimSilver(NULL)
	, m_pBtnAccept(NULL)
	, m_pBtnCancel(NULL)
	, m_pCurrentCommerceInfo(NULL)
{
}

GuildPickUpSilverFrame::~GuildPickUpSilverFrame()
{
	if (m_pWnd)
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
}

BOOL GuildPickUpSilverFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName(), (VUIEVENTHANDLE)m_Trunk.sfp1(&GuildPickUpSilverFrame::EventHandler));
	m_pGameFrameMgr->RegisterEventHandle(_T("Refresh_PickUpSilver"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildPickUpSilverFrame::OnEventPickUpSilverRefresh));
	return TRUE;
}

BOOL GuildPickUpSilverFrame::Destroy()
{
	m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName(), (VUIEVENTHANDLE)m_Trunk.sfp1(&GuildPickUpSilverFrame::EventHandler));
	m_pGameFrameMgr->UnRegisterEventHandler(_T("Refresh_PickUpSilver"), (FRAMEEVENTPROC)m_Trunk.sfp1(&GuildPickUpSilverFrame::OnEventPickUpSilverRefresh));
	return GameFrame::Destroy();
}

BOOL GuildPickUpSilverFrame::ReloadUI()
{
	tstring strPath = g_strLocalPath + _T("\\ui\\guild_pickupsilver.xml");
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());
	m_pStcGrade = (GUIStatic*)m_pWnd->GetChild(_T("pusback_pic\\pusback\\grade"));
	m_pStcGold = (GUIStatic*)m_pWnd->GetChild(_T("pusback_pic\\pusback\\num3"));
	m_pStcSilver = (GUIStatic*)m_pWnd->GetChild(_T("pusback_pic\\pusback\\num4"));
	m_pStcPickUpSilver = (GUIStatic*)m_pWnd->GetChild(_T("pusback_pic\\pusback\\num1"));
	m_pStcAimSilver = (GUIStatic*)m_pWnd->GetChild(_T("pusback_pic\\pusback\\num2"));
	m_pBtnAccept = (GUIButton*)m_pWnd->GetChild(_T("pusback_pic\\pusbutt0"));
	m_pBtnCancel = (GUIButton*)m_pWnd->GetChild(_T("pusback_pic\\pusbutt1"));
	m_pWnd->SetInvisible(TRUE);
	return TRUE;
}

BOOL GuildPickUpSilverFrame::EscCancel()
{
	if (P_VALID(m_pWnd) && !m_pWnd->IsInvisible())
	{
		m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_PickUpSilver"), this));
		return TRUE;
	}
	return GameFrame::EscCancel();
}

DWORD GuildPickUpSilverFrame::EventHandler(tagGUIEvent *pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if (!P_VALID(pWnd))
		return 0;
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnAccept)
			{
				//取得玩家当前的钱钱
				INT64 nSilver = CurrencyMgr::Inst()->GetBagSilver();
				if (nSilver < (INT64)m_pCurrentCommerceInfo->nDeposit)
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildCommerceError_Code4")]);
					return 0;
				}
				GuildMgr::Inst()->SendAcceptCommerce();
				m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_PickUpSilver"), this));
			}
			else if (pWnd == m_pBtnCancel)
			{
				m_pGameFrameMgr->SendEvent(&tagGameEvent(_T("Close_PickUpSilver"), this));
			}
		} break;
	default: break;
	}
	return 0;
}

DWORD GuildPickUpSilverFrame::OnEventPickUpSilverRefresh(tagRefreshPickUpSilverEvent *pEvent)
{
	TCHAR buffer[512];
	INT nLv = RoleMgr::Inst()->GetLocalPlayer()->GetRoleLevel();
	_sntprintf(buffer, 512, _T("%d"), nLv);
	m_pStcGrade->SetText(buffer);
	const tagCommerceInfo *pInfo = pEvent->pCommerceInfo;
	INT32 nGold = MSilver2DBGold(pInfo->nDeposit);
	INT32 nSilver = MSilver2DBSilver(pInfo->nDeposit);
	_sntprintf(buffer, 512, _T("%d"), nGold);
	m_pStcGold->SetText(buffer);
	_sntprintf(buffer, 512, _T("%d"), nSilver);
	m_pStcSilver->SetText(buffer);
	_sntprintf(buffer, 512, _T("%d"), pInfo->nBeginningTael);
	m_pStcPickUpSilver->SetText(buffer);
	_sntprintf(buffer, 512, _T("%d"), pInfo->nPurposeTael);
	m_pStcAimSilver->SetText(buffer);
	// 交给界面暂时保存一下
	m_pCurrentCommerceInfo = pInfo;
	// 设置好了再显示
	m_pWnd->SetInvisible(FALSE);
	return 0;
}

