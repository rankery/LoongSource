#include "stdafx.h"
#include "TalentFrame.h"
#include "TalentTreeFrame.h"
#include "SkillBookFrame.h"

TalentFrame::TalentFrame():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndCaption = NULL;
	m_pWndSkillBook = NULL;
	m_pWndTalentTree = NULL;
	m_pBtnExit = NULL;
}

TalentFrame::~TalentFrame()
{
}

BOOL TalentFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	m_pMgr->RegisterEventHandle(_T("Open_Dower"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentFrame::OnOpenDower));
	m_pMgr->RegisterEventHandle(_T("Close_Dower"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentFrame::OnCloseDower));

	return bRet;
}

BOOL TalentFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}

	m_pMgr->UnRegisterEventHandler(_T("Open_Dower"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentFrame::OnOpenDower));
	m_pMgr->UnRegisterEventHandler(_T("Close_Dower"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentFrame::OnCloseDower));

	return bRet;
}

BOOL TalentFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\skill.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	
	CHAR szFullName[X_LONG_NAME];
	for (INT i=0; i<MAX_SKILL_BOOK_NUM; ++i)
	{
		sprintf(szFullName, "skillwin\\skillbackpic\\skilist\\skill%d\\skbutt%d", i+1, i+1);
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "SkillButton");
	}

	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&TalentFrame::EventHandler));

	m_pWndCaption = m_pWnd->GetChild(_T("skillbackpic"));
	m_pWndSkillBook = m_pWndCaption->GetChild(_T("skilist"));
	m_pWndTalentTree = m_pWndCaption->GetChild(_T("skillback"));
	
	m_pBtnExit = (GUIButton*)m_pWndCaption->GetChild(_T("new"));

	m_pTalentTree = (TalentTreeFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("TalentTree"), _T("TalentTreeFrame"), 0);
	m_pSkillBook = (SkillBookFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("SkillBook"), _T("SkillBookFrame"), 0);

	// 隐藏天资栏
	m_pWnd->SetInvisible(TRUE);

	return TRUE;
}

DWORD TalentFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnExit)//退出
			{
				m_pMgr->SendEvent( &tagGameEvent( _T("Close_Dower_UI"), this ) );
			} 
		}
		break;
	default :
		break;
	}
	return 0;
}

VOID TalentFrame::ShowWnd( BOOL bVal )
{
	m_pWnd->SetInvisible(!bVal);
	if(bVal)
	{
		m_pSkillBook->OnSkillBookOpen();
		m_pTalentTree->OnTalentTreeOpen();

		m_pWnd->FlipToTop();
		m_pWnd->SetRefresh();
	}	
}

DWORD TalentFrame::OnOpenDower( tagGameEvent* pGameEvent )
{
	ShowWnd(TRUE);

	return 0;
}

DWORD TalentFrame::OnCloseDower( tagGameEvent* pGameEvent )
{
	ShowWnd(FALSE);

	return 0;
}

BOOL TalentFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

	m_pMgr->SendEvent( &tagGameEvent( _T("Close_Dower_UI"), this ) );
	return TRUE;
}