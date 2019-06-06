#include "stdafx.h"
#include "QuestFilterFrame.h"
#include "QuestMgr.h"


QuestFilterFrame::QuestFilterFrame()
	: m_Trunk(this)
	, m_pWnd(NULL)
	, m_pBtnClose(NULL)
	, m_pBtnDefault(NULL)
	, m_pBtnOK(NULL)
	, m_pBtnCancel(NULL)
{
	ZeroMemory(m_pStcFilterDesc, sizeof(m_pStcFilterDesc));
	ZeroMemory(m_pPbnFilter1, sizeof(m_pPbnFilter1));
	ZeroMemory(m_pPbnFilter2, sizeof(m_pPbnFilter2));
	m_vecFilterCache1.reserve(FILTER_NUM);
	m_vecFilterCache2.reserve(FILTER_NUM);
}

QuestFilterFrame::~QuestFilterFrame()
{

}

BOOL QuestFilterFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&QuestFilterFrame::EventHandler));
	return TRUE;
}

BOOL QuestFilterFrame::Destroy()
{
	if (!GameFrame::Destroy())
		return FALSE;
	
	m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&QuestFilterFrame::EventHandler));

	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
	
	m_vecFilterCache1.clear();
	m_vecFilterCache2.clear();
	
	return TRUE;
}

BOOL QuestFilterFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\task_choose.xml");
	if (!m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str()))
		return FALSE;
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pBtnClose = (GUIButton*)m_pWnd->GetChild(_T("task_choose_pic\\closebutt"));
	m_pBtnDefault = (GUIButton*)m_pWnd->GetChild(_T("task_choose_pic\\resetbutt"));
	m_pBtnOK = (GUIButton*)m_pWnd->GetChild(_T("task_choose_pic\\surebutt"));
	m_pBtnCancel = (GUIButton*)m_pWnd->GetChild(_T("task_choose_pic\\canclebutt"));

	
	for (int i = 0; i < FILTER_NUM; ++i)
	{
		TCHAR szBuffer[512];
		_sntprintf(szBuffer, 512, _T("task_choose_pic\\task_choose_listbackpic\\word%d"), i + 1);
		m_pStcFilterDesc[i] = (GUIStatic*)m_pWnd->GetChild(szBuffer);
		_sntprintf(szBuffer, 512, _T("task_choose_pic\\task_choose_listbackpic\\opbutt%d"), i * 2 + 1);
		m_pPbnFilter1[i] = (GUIPushButton*)m_pWnd->GetChild(szBuffer);
		_sntprintf(szBuffer, 512, _T("task_choose_pic\\task_choose_listbackpic\\opbutt%d"), i * 2 + 2);
		m_pPbnFilter2[i] = (GUIPushButton*)m_pWnd->GetChild(szBuffer);
	}
	
	TCHAR szBuffer[256];
	for (int i = 0; i < FILTER_NUM; ++i)
	{
		_sntprintf(szBuffer, 256, _T("QuestCategoryName%d"), i);
		m_pStcFilterDesc[i]->SetText(g_StrTable[szBuffer]);
	}
	
	Show(FALSE);
	return TRUE;
}

BOOL QuestFilterFrame::EscCancel()
{
	if (m_pWnd->IsInvisible())
		return FALSE;
	Show(FALSE);
	return TRUE;

}

BOOL QuestFilterFrame::EventHandler( tagGUIEvent *pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnClose)
			{
				Show(FALSE);
			}
			else if (pWnd == m_pBtnDefault)
			{
				RestorDefaultFilterConfig();
				tagGameEvent event(_T("quest_filter_config"), this);
				m_pGameFrameMgr->SendEvent(&event);
				
			}
			else if (pWnd == m_pBtnOK)
			{
				// 将配置缓存写入自定义配置中
				FlushFilterCache();
				// 通知消息接受者
				tagGameEvent event(_T("quest_filter_config"), this);
				m_pGameFrameMgr->SendEvent(&event);
				Show(FALSE);
			}
			else if (pWnd == m_pBtnCancel)
			{
				Show(FALSE);
			}
		} break;
	case EGUIE_PushButtonDown:
		{
			for (int i = 0; i < FILTER_NUM; ++i)
			{
				if (pWnd == m_pPbnFilter1[i])
				{
					if (pEvent->dwParam1 == 1)
					{
						m_vecFilterCache1[i].n16Category = i + 1;
						m_vecFilterCache1[i].bSet = true;
					}

					if (pEvent->dwParam1 == 2)
					{
						m_vecFilterCache1[i].n16Category = i + 1;
						m_vecFilterCache1[i].bSet = false;
					}
				}
				if (pWnd == m_pPbnFilter2[i])
				{
					if (pEvent->dwParam1 == 1)
					{
						m_vecFilterCache2[i].n16Category = i + 1;
						m_vecFilterCache2[i].bSet = true;
					}
					if (pEvent->dwParam1 == 2)
					{
						m_vecFilterCache2[i].n16Category = i + 1;
						m_vecFilterCache2[i].bSet = false;
					}
				}
			}
		} break;
	}
	return TRUE;
}

VOID QuestFilterFrame::FlushFilterCache()
{
	for (int i = 0; i < FILTER_NUM; ++i)
	{
		QuestMgr::Inst()->GetData()->SetQuestFilter(QuestData::EF_CustomFilter1, m_vecFilterCache1[i].n16Category, m_vecFilterCache1[i].bSet);
		QuestMgr::Inst()->GetData()->SetQuestFilter(QuestData::EF_CustomFilter2, m_vecFilterCache2[i].n16Category, m_vecFilterCache2[i].bSet);
	}
}

VOID QuestFilterFrame::Show( BOOL bShow )
{
	if (bShow)
	{
		UpdateData();
		m_pWnd->SetInvisible(FALSE);
		m_pWnd->FlipToTop();
	}
	else
	{
		m_pWnd->SetInvisible(TRUE);
	}
}

VOID QuestFilterFrame::UpdateData()
{
	m_vecFilterCache1.clear();
	m_vecFilterCache2.clear();

	QuestData *pQD = QuestMgr::Inst()->GetData();
	const QuestData::QuestFilterConfigMap &filter1 = pQD->GetFilter(QuestData::EF_CustomFilter1);
	for (QuestData::QuestFilterConfigMap::const_iterator iter = filter1.begin(); iter != filter1.end(); ++iter)
	{
		FilterCache fc;
		fc.n16Category = iter->first;
		fc.bSet = iter->second;
		m_vecFilterCache1.push_back(fc);
	}
	
	const QuestData::QuestFilterConfigMap &filter2 = pQD->GetFilter(QuestData::EF_CustomFilter2);
	for (QuestData::QuestFilterConfigMap::const_iterator iter = filter2.begin(); iter != filter2.end(); ++iter)
	{
		FilterCache fc;
		fc.n16Category = iter->first;
		fc.bSet = iter->second;
		m_vecFilterCache2.push_back(fc);
	}

	UpdateUI();

}



VOID QuestFilterFrame::RestorDefaultFilterConfig()
{
	m_vecFilterCache1.clear();
	m_vecFilterCache2.clear();

	QuestData *pQD = QuestMgr::Inst()->GetData();
	const QuestData::QuestFilterConfigMap &deffilter1 = pQD->GetFilter(QuestData::EF_DefFilter1);
	for (QuestData::QuestFilterConfigMap::const_iterator iter = deffilter1.begin(); iter != deffilter1.end(); ++iter)
	{
		FilterCache fc;
		fc.n16Category = iter->first;
		fc.bSet = iter->second;
		m_vecFilterCache1.push_back(fc);
	}
	
	const QuestData::QuestFilterConfigMap &deffilter2 = pQD->GetFilter(QuestData::EF_DefFilter2);
	for (QuestData::QuestFilterConfigMap::const_iterator iter = deffilter2.begin(); iter != deffilter2.end(); ++iter)
	{
		FilterCache fc;
		fc.n16Category = iter->first;
		fc.bSet = iter->second;
		m_vecFilterCache2.push_back(fc);
	}

	FlushFilterCache();

	UpdateUI();

}

VOID QuestFilterFrame::UpdateUI()
{
	for (int i = 0; i < FILTER_NUM; ++i)
	{
		if (m_vecFilterCache1[i].bSet == TRUE)
			m_pPbnFilter1[i]->SetState(EGUIBS_PushDown);
		else
			m_pPbnFilter1[i]->SetState(EGUIBS_PopUp);
	}

	for (int i = 0; i < FILTER_NUM; ++i)
	{
		if (m_vecFilterCache2[i].bSet == TRUE)
			m_pPbnFilter2[i]->SetState(EGUIBS_PushDown);
		else
			m_pPbnFilter2[i]->SetState(EGUIBS_PopUp);
	}
}