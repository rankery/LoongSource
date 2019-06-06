#include "StdAfx.h"
#include "wx/wx.h"
#include "Loong_OutputCtrl.h"
#include "ServerMergeToolApp.h"

CLoongOutputCtrl g_clsOutput;

CLoongOutputCtrl::CLoongOutputCtrl(void)
{
	InitializeCriticalSection(&m_csOutput);	
}

CLoongOutputCtrl::~CLoongOutputCtrl(void)
{
	DeleteCriticalSection(&m_csOutput);	
}
VOID CLoongOutputCtrl::Init(LPVOID pOutputWnd)
{
	ASSERT(pOutputWnd);
	m_pOutputWnd = pOutputWnd;

}

VOID CLoongOutputCtrl::Output(LPCTSTR szStr)
{
	EnterCriticalSection(&m_csOutput);

	wxTextPos pos = ((wxTextCtrl *)m_pOutputWnd)->GetLastPosition();
	((wxTextCtrl *)m_pOutputWnd)->Replace(pos,-1,szStr);

	//Ð´ÈÕÖ¾
	ILOG->Write(szStr);

	LeaveCriticalSection(&m_csOutput);
}