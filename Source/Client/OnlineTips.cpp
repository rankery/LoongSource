#include "stdafx.h"
#include "OnlineTips.h"
#include "QuestMainFrame.h"
#include "CharacterFrame.h"
#include "TalentFrame.h"
#include "QuestMgr.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "CurrencyMgr.h"
#include "Container.h"
#include "ItemMgr.h"


OnlineTips::OnlineTips()
	: m_Trunk(this)
	, m_pGUI(NULL)
	, m_pSession(NULL)
	, m_pGameFrameMgr(NULL)
	, m_pWnd(NULL)
	, m_pBtnCanAcceptQuestsNum(NULL)
	, m_pBtnNotCompleteQuestsNum(NULL)
	, m_pBtnCanAssignAttNum(NULL)
	, m_pBtnCanAssignTalentNum(NULL)
	, m_pBtnBBItemNum(NULL)
	, m_pBtnBBYuanBaoNum(NULL)
	, m_pBtnClose(NULL)
	, m_bShow(false)
{

}

OnlineTips::~OnlineTips()
{

}

BOOL OnlineTips::Init(GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), 
		m_Trunk.sfp1(&OnlineTips::EventHandler));
	
	m_pGameFrameMgr->RegisterEventHandle(_T("Refresh_Onlinetips"),  
		(FRAMEEVENTPROC)m_Trunk.sfp1(&OnlineTips::OnRefreshTips));
	

	ShowWnd(FALSE);
	return TRUE;
}

BOOL OnlineTips::Destroy()
{
	GameFrame::Destroy();
	m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName().c_str(), 
		m_Trunk.sfp1(&OnlineTips::EventHandler));
	m_pGameFrameMgr->UnRegisterEventHandler(_T("Refresh_Onlinetips"),  
		(FRAMEEVENTPROC)m_Trunk.sfp1(&OnlineTips::OnRefreshTips));
	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}


	return TRUE;
}

BOOL OnlineTips::ReloadUI()
 {
	if (P_VALID(m_pWnd))
		m_pGUI->AddToDestroyList(m_pWnd);

	tstring strPath = g_strLocalPath + _T("\\ui\\onlinetips.xml");
	
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());
	GUIPatch *pPatch = static_cast<GUIPatch*>(m_pWnd->GetChild(_T("onlinetips_backpic")));
	m_pBtnCanAcceptQuestsNum = static_cast<GUIButton*>(pPatch->GetChild(_T("butt1")));
	m_pBtnNotCompleteQuestsNum = static_cast<GUIButton*>(pPatch->GetChild(_T("butt2")));
	m_pBtnCanAssignAttNum = static_cast<GUIButton*>(pPatch->GetChild(_T("butt3")));
	m_pBtnCanAssignTalentNum = static_cast<GUIButton*>(pPatch->GetChild(_T("butt4")));
	m_pBtnBBItemNum = static_cast<GUIButton*>(pPatch->GetChild(_T("butt40")));
	m_pBtnBBYuanBaoNum = static_cast<GUIButton*>(pPatch->GetChild(_T("butt400")));
	m_pBtnClose = static_cast<GUIButton*>(pPatch->GetChild(_T("closebutt")));

	
	return TRUE;
}

void OnlineTips::ShowWnd(BOOL bShow)
{
	m_pWnd->SetInvisible(!bShow);
	m_bShow = bShow;
}

DWORD OnlineTips::EventHandler(tagGUIEvent *pEvent)
{
	GUIWnd *pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnCanAcceptQuestsNum)
			{
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Task_UI"), this ) );
				m_pMgr->SendEvent(&tagGameEvent(_T("QuestMainFrameEvent_OpenNPC"), this));
				
			}
			if (pWnd == m_pBtnNotCompleteQuestsNum)
			{
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Task_UI"), this ) );
				m_pMgr->SendEvent(&tagGameEvent(_T("QuestMainFrameEvent_CloseNPC"), this));
			}
			if (pWnd == m_pBtnCanAssignAttNum)
			{
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Role_UI"), this ) );
			}
			if (pWnd == m_pBtnCanAssignTalentNum)
			{
				m_pMgr->SendEvent( &tagGameEvent( _T("Open_Dower_UI"), this ) );			
			}
			if (pWnd == m_pBtnBBItemNum)
			{
				GameFrame* pFrame = m_pGameFrameMgr->GetFrame(_T("Throne"));
				if(!P_VALID(pFrame))
				{
					m_pGameFrameMgr->CreateFrame(m_strName.c_str(), _T("Throne"), _T("ThroneFrame"), 0);
				}
			}
			if (pWnd == m_pBtnBBYuanBaoNum)
			{
				GameFrame* pFrame = m_pGameFrameMgr->GetFrame(_T("Throne"));
				if(!P_VALID(pFrame))
				{
					m_pGameFrameMgr->CreateFrame(m_strName.c_str(), _T("Throne"), _T("ThroneFrame"), 0);
				}
			}
			if (pWnd == m_pBtnClose)
			{
				ShowWnd(FALSE);
			}
		} break;
	default : break;
	}
	return 0;
}

DWORD OnlineTips::OnRefreshTips(tagGUIEvent *pEvent)
{
	RefreshCanAcceptQuestsNum();
	RefreshNotCompleteQuestsNum();
	RefreshCanAssignAttNum();
	RefreshCanAssignTalentNum();
	RefreshBBItemNum();
	RefreshBBYuanBaoNum();
	return 0;
}

void OnlineTips::RefreshCanAcceptQuestsNum()
{
	QuestQuery *pQQ = QuestMgr::Inst()->GetQuery();
	QuestData *pQD = QuestMgr::Inst()->GetData();
	// 获取可接任务的所有NPC
	const QuestQuery::NPCSet& npcset = pQQ->GetAcceptQuestNPCs();
	int nNum = 0;
	QuestQuery::NPCSet::const_iterator begin = npcset.begin();
	QuestQuery::NPCSet::const_iterator end = npcset.end();
	for (; begin != end; ++begin)
	{
		vector<UINT16> quests;
		pQD->GetNPCAcceptQuests(*begin, quests);
		for(size_t questi = 0; questi < quests.size(); ++questi)
		{
			if(pQQ->QuestCanAccept(quests[questi]) && (!pQQ->QuestIsCompleted(quests[questi])))
			{
				nNum++;
			}
		}
	}

	TCHAR buffer[20];
	_sntprintf(buffer, 20, g_StrTable[_T("OnlineTips_CanAcceptQuestsNum")], nNum);
	m_pBtnCanAcceptQuestsNum->SetText(buffer);
}

void OnlineTips::RefreshNotCompleteQuestsNum()
{
	QuestQuery *pQQ = QuestMgr::Inst()->GetQuery();
	const QuestQuery::QuestsMap &QuestMap =  pQQ->GetCurrentQuests();
	int nNum = QuestMap.size();
	TCHAR buffer[20];
	_sntprintf(buffer, 20, g_StrTable[_T("OnlineTips_NotCompleteQuestsNum")], nNum);
	m_pBtnNotCompleteQuestsNum->SetText(buffer);
}

void OnlineTips::RefreshCanAssignAttNum()
{
	int nNum = RoleMgr::Inst()->GetLocalPlayer()->GetAttribute(ERA_AttPoint);
	TCHAR buffer[20];
	_sntprintf(buffer, 20, g_StrTable[_T("OnlineTips_CanAssignAttNum")], nNum);
	m_pBtnCanAssignAttNum->SetText(buffer);
}

void OnlineTips::RefreshCanAssignTalentNum()
{
	int nNum = RoleMgr::Inst()->GetLocalPlayer()->GetAttribute(ERA_TalentPoint);
	TCHAR buffer[20];
	_sntprintf(buffer, 20, g_StrTable[_T("OnlineTips_CanAssignTalentNum")], nNum);
	m_pBtnCanAssignTalentNum->SetText(buffer);
}

BOOL OnlineTips::EscCancel()
{
	if( m_pWnd->IsInvisible() )
		return FALSE;

	ShowWnd(FALSE);
	return TRUE;
}

void OnlineTips::RefreshBBItemNum()
{
	INT32 nNum = ItemMgr::Inst()->GetThrone()->GetContainerItemQuantity();
	TCHAR szBuffer[20];
	if (nNum > 99)
	{
		_sntprintf(szBuffer, 20, g_StrTable[_T("OnlineTips_BBItemNum2")]);
	}
	else
	{
		_sntprintf(szBuffer, 20, g_StrTable[_T("OnlineTips_BBItemNum1")], nNum);
	}
	m_pBtnBBItemNum->SetText(szBuffer);
}

void OnlineTips::RefreshBBYuanBaoNum()
{
	INT nYuanBao = CurrencyMgr::Inst()->GetBaibaoYuanbao();
	TCHAR szBuffer[20];
	if (nYuanBao > 99)
	{
		_sntprintf(szBuffer, 20, g_StrTable[_T("OnlineTips_BBYuanBaoNum2")]);
	}
	else
	{
		_sntprintf(szBuffer, 20, g_StrTable[_T("OnlineTips_BBYuanBaoNum1")], nYuanBao);
	}
	m_pBtnBBYuanBaoNum->SetText(szBuffer);
}