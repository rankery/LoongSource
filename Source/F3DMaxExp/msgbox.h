#pragma once
#include <windows.h>
#include <string>
#include <assert.h>
#ifndef tstring
#if _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif
using namespace std;

class MsgBox
{
public:
	MsgBox()
		: m_hWnd(NULL)
		, m_outPutWndID(0xFFFFFFFF)
	{

	}

	~MsgBox()
	{

	}

	void Create(HINSTANCE hInstance,UINT nResID,UINT outputID)
	{
		if(!::IsWindow(m_hWnd))
		{
			m_hWnd=CreateDialog(hInstance,MAKEINTRESOURCE(nResID)
				,GetActiveWindow(),MsgBox::DlgProc);
		}
		ShowWindow(m_hWnd,SW_SHOW);
		UpdateWindow(m_hWnd);
		m_szBuf.erase(m_szBuf.begin(),m_szBuf.end());
		m_outPutWndID=outputID;
	}

	void Destroy()
	{

	}

	void ShowMsg(const TCHAR* lpszFormat,...)
	{
		va_list args;
		va_start(args, lpszFormat);
		
		int nBuf;
		TCHAR szBuffer[2048];
		
		nBuf = _vsntprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		assert(nBuf < sizeof(szBuffer)); //Output truncated as it was > sizeof(szBuffer)
		va_end(args);
		
		m_szBuf+=szBuffer;
		
		HWND hWnd=GetDlgItem(m_hWnd,m_outPutWndID);
		SendMessage(hWnd,WM_SETTEXT,0,(LPARAM)m_szBuf.c_str());
		int nLineCount=SendMessage(hWnd,EM_GETLINECOUNT,0,0);
		SendMessage(hWnd,EM_LINESCROLL,0,nLineCount);
		UpdateWindow(m_hWnd);
	}

	static BOOL CALLBACK DlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
	{
		switch(message)
		{
		case WM_INITDIALOG:
			CenterWindow(hWnd,GetParent(hWnd));//a max api
			return TRUE;
			break;
		case WM_CLOSE:
			//EndDialog(hWnd, 0);
			ShowWindow(hWnd,SW_HIDE);
			return TRUE;
		}
		
		return FALSE;
	}

	const HWND GetHWND() const { return m_hWnd; }

private:
	UINT		m_outPutWndID;
	HWND		m_hWnd;
	tstring		m_szBuf;
};
