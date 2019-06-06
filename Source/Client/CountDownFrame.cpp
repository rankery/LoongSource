#include "stdafx.h"
#include "CountDownFrame.h"

CountDownFrame::CountDownFrame()
: m_pGUI(NULL)
, m_pFrameMgr(NULL)
, m_pWnd(NULL)
, m_pCDTime(NULL)
, m_nCDTime(0)
{
}

CountDownFrame::~CountDownFrame()
{
}

BOOL CountDownFrame::Init(GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;
	m_pWnd->SetInvisible(FALSE);
	return TRUE;
}

BOOL CountDownFrame::Destroy()
{
	GameFrame::Destroy();
	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
	return TRUE;
}

BOOL CountDownFrame::ReloadUI()
{
	if (P_VALID(m_pWnd))
		m_pGUI->AddToDestroyList(m_pWnd);
	
	tstring strPath = g_strLocalPath + _T("\\ui\\instance_countdown.xml");
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());

	m_pCDTime = (GUIStatic*)(m_pWnd->GetChild(_T("cd_pic")));
	m_pLeftTime = (GUIStatic*)(m_pCDTime->GetChild(_T("word")));
	return TRUE;
}

VOID CountDownFrame::Update()
{
	m_nCDTime -= static_cast<int>(Cool3D::Kernel::Inst()->GetDeltaTimeDW());
	if (m_nCDTime <= 0)
		m_nCDTime = 0;

	int hour;
	int min;
	int sec;
	
	sec = m_nCDTime / 1000;
	min = (sec / 60) % 60;
	hour = sec / 3600;
	sec = sec % 60;
	
	TCHAR szText[256];
	ZeroMemory(szText, sizeof(szText));
	// 格式化时间格式
	if (hour != 0)
	{
		if (min < 10)
		{
			if (sec < 10)
				_stprintf(szText, _T("%d:0%d:0%d"), hour, min, sec);
			else
				_stprintf(szText, _T("%d:0%d:%d"), hour, min, sec);
		}
		else
		{
			if (sec < 10)
				_stprintf(szText, _T("%d:%d:0%d"), hour, min, sec);
			else
				_stprintf(szText, _T("%d:%d:%d"), hour, min, sec);
		}
	}
	else
	{
		if (min < 10)
		{
			if (sec < 10)
				_stprintf(szText, _T("0%d:0%d"), min, sec);
			else
				_stprintf(szText, _T("0%d:%d"), min, sec);
		}
		else
		{
			if (sec < 10)
				_stprintf(szText, _T("%d:0%d"), min, sec);
			else
				_stprintf(szText, _T("%d:%d"), min, sec);
		}
	}
	m_pLeftTime->SetText(g_StrTable[_T("Instance_Time")]);
	m_pCDTime->SetText(szText);
}

VOID CountDownFrame::Hide()
{
	if (P_VALID(m_pWnd))
		m_pWnd->SetInvisible(TRUE);
}

VOID CountDownFrame::Show()
{
	if (P_VALID(m_pWnd))
		m_pWnd->SetInvisible(FALSE);
}
VOID CountDownFrame::SetCDTime(DWORD dwCDTime)
{
	m_nCDTime = static_cast<int>(dwCDTime);
}