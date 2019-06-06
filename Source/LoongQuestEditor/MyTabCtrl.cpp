// MyTabCtrl.cpp : implementation file
//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMyTabCtrl Class
// Written by: Brooks Younce
// Date: Jan 19 2007
// XP style compatability idea modified from Luuk Weltevreden http://www.codeproject.com/wtl/ThemedDialog.asp
// This class allows easy inserting of controls into your Tab control, with minimal coding.
// Free for public use, but must not be misrepresented, please leave comments attached.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyTabCtrl.h"


// CMyTabCtrl dialog

IMPLEMENT_DYNAMIC(CMyTabCtrl, CDialog)
CMyTabCtrl::CMyTabCtrl()
{
}

CMyTabCtrl::~CMyTabCtrl()
{
	if(m_hBrush){::DeleteObject(m_hBrush);}
}


BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnTcnSelchange)
	ON_MESSAGE(WM_BUTTONPRESSED,ButtonPressed)
	ON_MESSAGE(WM_UPDOWN,UpDownButton)
END_MESSAGE_MAP()


// CMyTabCtrl message handlers

void CMyTabCtrl::OnDestroy()
{
	CTabCtrl::OnDestroy();

	// TODO: Add your message handler code here
	//CLEAN UP MEMORY
	INT_PTR iCount = obArray.GetCount();
	for(INT_PTR i=0; i<iCount; i++)
	{
		ITEM* pItem = (ITEM*)obArray[i];
		pItem->wnd->DestroyWindow();
		delete pItem->wnd; //cleanup item from struct
		pItem->wnd = NULL;
		delete pItem;	   //cleanup struct	
		pItem = NULL;
	}
}

void CMyTabCtrl::SetAllItemDisable()
{
	INT_PTR iCount = obArray.GetCount();
	for(INT_PTR i=0; i<iCount; i++)
	{
		ITEM* pItem = (ITEM*)obArray[i];
		pItem->wnd->EnableWindow( 0 );
	}
}

void CMyTabCtrl::SetAllItemEnable()
{
	INT_PTR iCount = obArray.GetCount();
	for(INT_PTR i=0; i<iCount; i++)
	{
		ITEM* pItem = (ITEM*)obArray[i];
		pItem->wnd->EnableWindow( 1 );
	}
}

void CMyTabCtrl::CreateCheckBox(BOOL bCheck, LPCTSTR sCaption, int nID, int iTab, UINT uLocation, int iX, int iY)
{
	CRect rc;
	GetClientRect(&rc);//GetWindowRect
	int x = rc.left + MARGIN_LEFT;
	int y = rc.top + MARGIN_TOP;
	if(uLocation & P_BELOW){y = iLastBottom;}
	if(uLocation & P_RIGHT){x = iLastRight;}
	if(uLocation & P_LEFT){x = iLastLeft;}
	if(uLocation & P_TOP){y = iLastTop;}
	x += iX;
	y += iY;

	CDC* dcPtr = GetDC();
	CFont* fontPtr = GetFont();
	fontPtr = dcPtr->SelectObject(fontPtr);
	CSize sz = dcPtr->GetTextExtent(sCaption, (int)_tcslen(sCaption));
	fontPtr = dcPtr->SelectObject(fontPtr);
	ReleaseDC(dcPtr);
	sz.cy += 4;
	sz.cx += 16;

	CButton* pButton = new CButton;
	pButton->Create(sCaption,WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_TABSTOP,CRect(x,y,x+sz.cx,y+sz.cy),this,nID);
	pButton->SetFont(GetFont());	
	if(iTab == 0){pButton->ShowWindow(SW_SHOW);}else{pButton->ShowWindow(SW_HIDE);}
	CheckDlgButton(nID,(UINT)bCheck);
	iLastBottom = y + sz.cy;
	iLastRight = x + sz.cx;
	iLastLeft = x;
	iLastTop = y;
	
	//save the item struct to the object array
	ITEM* pItem = new ITEM;
	pItem->bTabStop = TRUE;
	pItem->iTab = iTab;
	pItem->wnd = (CWnd*)pButton;
	obArray.Add((CObject*)pItem);
}

void CMyTabCtrl::CreateRadioBox(BOOL bCheck, LPCTSTR sCaption, int nID, int iTab, UINT uLocation, int iX, int iY, DWORD dwStyle)
{
	CRect rc;
	GetClientRect(&rc);//GetWindowRect
	int x = rc.left + MARGIN_LEFT;
	int y = rc.top + MARGIN_TOP;
	if(uLocation & P_BELOW){y = iLastBottom;}
	if(uLocation & P_RIGHT){x = iLastRight;}
	if(uLocation & P_LEFT){x = iLastLeft;}
	if(uLocation & P_TOP){y = iLastTop;}
	x += iX;
	y += iY;

	CDC* dcPtr = GetDC();
	CFont* fontPtr = GetFont();
	fontPtr = dcPtr->SelectObject(fontPtr);
	CSize sz = dcPtr->GetTextExtent(sCaption, (int)_tcslen(sCaption));
	fontPtr = dcPtr->SelectObject(fontPtr);
	ReleaseDC(dcPtr);
	sz.cy += 4;
	sz.cx += 16;

	CButton* pButton = new CButton;
	pButton->Create(sCaption,WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_TABSTOP|dwStyle,CRect(x,y,x+sz.cx,y+sz.cy),this,nID);
	pButton->SetFont(GetFont());
	if(iTab == 0){pButton->ShowWindow(SW_SHOW);}else{pButton->ShowWindow(SW_HIDE);}
	CheckDlgButton(nID,(UINT)bCheck);
	iLastBottom = y + sz.cy;
	iLastRight = x + sz.cx;
	iLastLeft = x;
	iLastTop = y;

	//save the item struct to the object array
	ITEM* pItem = new ITEM;
	pItem->bTabStop = TRUE;
	pItem->iTab = iTab;
	pItem->wnd = (CWnd*)pButton;
	obArray.Add((CObject*)pItem);
}

void CMyTabCtrl::CreateButton(LPCTSTR sCaption, int nID, int iTab, UINT uLocation, int iX, int iY, int iLen)
{
	CRect rc;
	GetClientRect(&rc);//GetWindowRect
	int x = rc.left + MARGIN_LEFT;
	int y = rc.top + MARGIN_TOP;
	if(uLocation & P_BELOW){y = iLastBottom;}
	if(uLocation & P_RIGHT){x = iLastRight;}
	if(uLocation & P_LEFT){x = iLastLeft;}
	if(uLocation & P_TOP){y = iLastTop;}
	x += iX;
	y += iY;

	CDC* dcPtr = GetDC();
	CFont* fontPtr = GetFont();
	fontPtr = dcPtr->SelectObject(fontPtr);
	CSize sz = dcPtr->GetTextExtent(sCaption, (int)_tcslen(sCaption));
	fontPtr = dcPtr->SelectObject(fontPtr);
	ReleaseDC(dcPtr);
	sz.cy += 8;

	if(iLen != 0){sz.cx = iLen;}
	CButton* pButton = new CButton;
	pButton->Create(sCaption,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP,CRect(x,y,x+sz.cx,y+sz.cy),this,nID);
	pButton->SetFont(GetFont());
	if(iTab == 0){pButton->ShowWindow(SW_SHOW);}else{pButton->ShowWindow(SW_HIDE);}
	iLastBottom = y + sz.cy;
	iLastRight = x + sz.cx;
	iLastLeft = x;
	iLastTop = y;
	//save the item struct to the object array
	ITEM* pItem = new ITEM;
	pItem->bTabStop = TRUE;
	pItem->iTab = iTab;
	pItem->wnd = (CWnd*)pButton;
	obArray.Add((CObject*)pItem);
}

void CMyTabCtrl::CreateEdit(LPCTSTR sCaption, int nID, int iTab, UINT uLocation, int iX, int iY, int iLen, int iHeight, BOOL bScroll)
{
	CRect rc;
	GetClientRect(&rc);//GetWindowRect
	int x = rc.left + MARGIN_LEFT;
	int y = rc.top + MARGIN_TOP;
	if(uLocation & P_BELOW){y = iLastBottom;}
	if(uLocation & P_RIGHT){x = iLastRight;}
	if(uLocation & P_LEFT){x = iLastLeft;}
	if(uLocation & P_TOP){y = iLastTop;}
	x += iX;
	y += iY;
	CSize sz;
	sz.cy = iHeight;
	sz.cx = iLen;
	if(iLen != 0 ){sz.cx = iLen;}
	CEdit* pEdit = new CEdit;
	if ( bScroll == TRUE )
	{
		pEdit->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER|WS_VSCROLL|ES_WANTRETURN|ES_MULTILINE,CRect(x,y,x+sz.cx,y+sz.cy),this,nID);
	}
	else
	{
		pEdit->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_WANTRETURN|ES_MULTILINE,CRect(x,y,x+sz.cx,y+sz.cy),this,nID);
	}
	pEdit->SetFont(GetFont());
	if(iTab == 0){pEdit->ShowWindow(SW_SHOW);}else{pEdit->ShowWindow(SW_HIDE);}
	pEdit->SetLimitText(256);
	pEdit->SetWindowText(sCaption);
	iLastBottom = y + sz.cy;
	iLastRight = x + sz.cx;
	iLastLeft = x;
	iLastTop = y;
	//save the item struct to the object array
	ITEM* pItem = new ITEM;
	pItem->bTabStop = TRUE;
	pItem->iTab = iTab;
	pItem->wnd = (CWnd*)pEdit;
	obArray.Add((CObject*)pItem);
}

void CMyTabCtrl::CreateStatic(LPCTSTR sCaption, int nID, int iTab, UINT uLocation, int iX, int iY)
{
	CRect rc;
	GetClientRect(&rc);//GetWindowRect
	int x = rc.left + MARGIN_LEFT;
	int y = rc.top + MARGIN_TOP;
	if(uLocation & P_BELOW){y = iLastBottom;}
	if(uLocation & P_RIGHT){x = iLastRight;}
	if(uLocation & P_LEFT){x = iLastLeft;}
	if(uLocation & P_TOP){y = iLastTop;}
	x += iX;
	y += iY;
	CSize sz = GetDC()->GetTextExtent(sCaption);
	sz.cy += 4;
	//if(iLen != 0 ){sz.cx = iLen;}
	CStatic* pStatic = new CStatic;
	pStatic->Create(sCaption,WS_CHILD|WS_VISIBLE,CRect(x,y,x+sz.cx,y+sz.cy),this,nID);
	pStatic->SetFont(GetFont());
	if(iTab == 0){pStatic->ShowWindow(SW_SHOW);}else{pStatic->ShowWindow(SW_HIDE);}
	iLastBottom = y + sz.cy;
	iLastRight = x + sz.cx;
	iLastLeft = x;
	iLastTop = y;
	//save the item struct to the object array
	ITEM* pItem = new ITEM;
	pItem->bTabStop = FALSE;
	pItem->iTab = iTab;
	pItem->wnd = (CWnd*)pStatic;
	obArray.Add((CObject*)pItem);
}

void CMyTabCtrl::CreateGroupBox(LPCTSTR sCaption, int nID, int iTab, int width, int height, UINT uLocation, int iX, int iY)
{
	CRect rc;
	GetClientRect(&rc);//GetWindowRect
	int x = rc.left + MARGIN_LEFT;
	int y = rc.top + MARGIN_TOP;
	if(uLocation & P_BELOW){y = iLastBottom;}
	if(uLocation & P_RIGHT){x = iLastRight;}
	if(uLocation & P_LEFT){x = iLastLeft;}
	if(uLocation & P_TOP){y = iLastTop;}
	x += iX;
	y += iY;
	CButton* pStatic = new CButton;
	pStatic->Create(sCaption,WS_CHILD|WS_VISIBLE|BS_GROUPBOX,CRect(x,y,x+width,y+height),this,nID);
	pStatic->SetFont(GetFont());
	if(iTab == 0){pStatic->ShowWindow(SW_SHOW);}else{pStatic->ShowWindow(SW_HIDE);}
	iLastBottom = y+height;
	iLastRight = x+width;
	iLastLeft = x;
	iLastTop = y;
	//save the item struct to the object array
	ITEM* pItem = new ITEM;
	pItem->bTabStop = FALSE;
	pItem->iTab = iTab;
	pItem->wnd = (CWnd*)pStatic;
	obArray.Add((CObject*)pItem);
}

int CMyTabCtrl::BottomOf(int nID)
{
	CRect rc;
	GetDlgItem(nID)->GetWindowRect(&rc);
	ScreenToClient(rc);
	return rc.bottom - MARGIN_TOP;
}

int CMyTabCtrl::RightOf(int nID)
{
	CRect rc;
	GetDlgItem(nID)->GetWindowRect(&rc);
	ScreenToClient(rc);
	return rc.right - MARGIN_LEFT;
}

int CMyTabCtrl::LeftOf(int nID)
{
	CRect rc;
	GetDlgItem(nID)->GetWindowRect(&rc);
	ScreenToClient(rc);
	return rc.left - MARGIN_LEFT;
}

int CMyTabCtrl::TopOf(int nID)
{
	CRect rc;
	GetDlgItem(nID)->GetWindowRect(&rc);
	ScreenToClient(rc);
	return rc.top - MARGIN_TOP;
}

//CString CMyTabCtrl::GetItemText(int nID)
//{
//	CString s;
//	GetDlgItem(nID)->GetWindowText(s);
//	return s;
//}

wstring CMyTabCtrl::GetItemText( int nID )
{
	CString s;
	
	GetDlgItem(nID)->GetWindowText(s);
	s.Replace(_T("\r\n"), _T(""));
	wstring wstr = (LPCTSTR)s;
	return wstr;
}

int CMyTabCtrl::GetItemTextLength(int nID)
{
	CString s;
	GetDlgItem(nID)->GetWindowText(s);
	return s.GetLength();
}

int CMyTabCtrl::GetItemTextAsInt(int nID)
{
	CString s;
	GetDlgItem(nID)->GetWindowText(s);
	return _ttoi(s.GetBuffer());
}

DWORD CMyTabCtrl::GetItemTextAsDWORD( int nID )
{
	CString s;
	GetDlgItem(nID)->GetWindowText(s);
	return _ttol(s.GetBuffer());
}

float CMyTabCtrl::GetItemTextAsFloat( int nID )
{
	CString s;
	GetDlgItem(nID)->GetWindowText(s);
	return (float)_tstof(s.GetBuffer());
}

BOOL CMyTabCtrl::GetItemCheckState(int nID)
{ 
	return (BOOL)IsDlgButtonChecked(nID);
}

void CMyTabCtrl::SetItemCheckState( int nID, BOOL bValue )
{
	((CButton*)GetDlgItem( nID ))->SetCheck( bValue );
}

void CMyTabCtrl::SetItemText( int nID, LPCTSTR strValue )
{
	GetDlgItem( nID )->SetWindowTextW( strValue );
}

void CMyTabCtrl::SetItemText( int nID, DWORD dwValue )
{
	wstringstream wss;
	wstring wstr;
	wss << dwValue << endl;
	wss >> wstr;
	GetDlgItem( nID )->SetWindowTextW( wstr.c_str() );
}

void CMyTabCtrl::SetItemText( int nID, FLOAT fValue )
{
	wstringstream wss;
	wstring wstr;
	wss << fValue << endl;
	wss >> wstr;
	GetDlgItem( nID )->SetWindowTextW( wstr.c_str() );
}

void CMyTabCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	ShowTab( GetCurFocus() );
}

void CMyTabCtrl::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	ShowTab( GetCurFocus() );
	*pResult = 0;
}

void CMyTabCtrl::ShowTab(int iTab)
{
	iCtrl = -1;//reset any control which may be selected in a tab

	if(iCurrentTab == iTab){return;}//already focused on selected tab
	
	//save old tab location so we can hide its controls
	int iOldTab = iCurrentTab;
	iCurrentTab = iTab;
		
	//check if tab is in focus, if not, put it in focus
	int t = GetCurFocus();
	if(iTab != t){SetCurSel(iTab);}

	//hide old tabs controls, show the selected tab's controls
	INT_PTR iCount = obArray.GetCount();
	for(INT_PTR i=0; i<iCount; i++)
	{
		ITEM* pItem = (ITEM*)obArray[i];
		if(pItem->iTab == iOldTab){pItem->wnd->ShowWindow(SW_HIDE);}
		else if(pItem->iTab == iCurrentTab){pItem->wnd->ShowWindow(SW_SHOW);}
	}
}

HBRUSH CMyTabCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CTabCtrl::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(m_bThemeActive && nCtlColor == CTLCOLOR_STATIC)
	{
		CRect rc;
		// Set the background mode to transparent
		::SetBkMode(pDC->m_hDC,TRANSPARENT);
		return m_hBrush;
		//return ::CreateSolidBrush(RGB(255,0,0));
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CMyTabCtrl::Init()
{
	iCurrentTab = 0;
	iLastBottom = 0;
	iLastRight = 0;
	iLastLeft = 0;
	iLastTop = 0;
	iCtrl = -1;
	m_hBrush = NULL;

	//UpdateBackgroundBrush
	try
	{
		HMODULE hinstDll;
		// Check if the application is themed
		hinstDll = LoadLibrary(_T("UxTheme.dll"));
		if(hinstDll)
		{
			typedef BOOL (*ISAPPTHEMEDPROC)();
			ISAPPTHEMEDPROC pIsAppThemed;
			pIsAppThemed = (ISAPPTHEMEDPROC)::GetProcAddress(hinstDll, "IsAppThemed");
			if(pIsAppThemed){m_bThemeActive = pIsAppThemed();}
			FreeLibrary(hinstDll);
		}

		// Destroy old brush
		if(m_hBrush){::DeleteObject(m_hBrush);}
		m_hBrush = NULL;

		// Only do this if the theme is active
		if(m_bThemeActive)
		{
			RECT rc;
			// Get tab control dimensions
			GetWindowRect(&rc);//m_ctab	
			// Get the tab control DC
			CDC* pDC = GetDC();	//m_ctab		
			HDC hDC = pDC->m_hDC;//m_ctab		
			// Create a compatible DC
			HDC hDCMem = ::CreateCompatibleDC(hDC);
			HBITMAP hBmp = ::CreateCompatibleBitmap(hDC, rc.right - rc.left, rc.bottom - rc.top);
			HBITMAP hBmpOld = (HBITMAP)(::SelectObject(hDCMem, hBmp));
			// Tell the tab control to paint in our DC
			SendMessage(WM_PRINTCLIENT, (WPARAM)(hDCMem), (LPARAM)(PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT));//m_cTab			
			// Create a pattern brush from the bitmap selected in our DC
			//m_hBrush = ::CreatePatternBrush(hBmp);
			// Create a Soild brush from a specific pixel which the exact color we need
			m_hBrush = ::CreateSolidBrush(::GetPixel(hDCMem,5,5));
			// Restore the bitmap
			::SelectObject(hDCMem, hBmpOld);
			// Cleanup
			::DeleteObject(hBmp);
			::DeleteDC(hDCMem);
			ReleaseDC(pDC);//m_ctab
		}
	}
	catch(...)
	{
	}
}

//intercept messages before they are dispatched
BOOL CMyTabCtrl::PreTranslateMessage(MSG* pMsg)
{
//#if _DEBUG
//	if(pMsg->message != WM_MOUSEMOVE && pMsg->message != WM_PAINT)
//	{TRACE("msg:%d lParam:%d wParam:%d HWND:%d\n",pMsg->message,pMsg->lParam,pMsg->wParam,pMsg->hwnd);}
//#endif

	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_DOWN)
		{
			PostMessage(WM_UPDOWN,0,0);
			return TRUE;
		}
		if(pMsg->wParam == VK_UP)
		{
			PostMessage(WM_UPDOWN,1,0);
			return TRUE;
		}
		else if(pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_LEFT || pMsg->wParam == VK_TAB)
		{
			iCtrl = -1;//unselect any child control which may be selected
		}
		else if(pMsg->wParam == VK_ESCAPE)//pMsg->wParam == VK_RETURN 
		{return TRUE;}
	}
	else if(pMsg->message == WM_KEYUP)
	{
		if(pMsg->wParam == VK_SPACE)
		{
			PostMessage(WM_BUTTONPRESSED,(WPARAM)pMsg->hwnd,1);
		}
	}
	else if(pMsg->message == WM_LBUTTONUP)
	{
		PostMessage(WM_BUTTONPRESSED,(WPARAM)pMsg->hwnd,0);
	}
	return CTabCtrl::PreTranslateMessage(pMsg);
}

LRESULT CMyTabCtrl::UpDownButton(WPARAM w, LPARAM l)
{
	int iCurTab = GetCurFocus();
	int iCount = (int)obArray.GetCount();

	int iOldPos = iCtrl;//save old pos

	//increment or deincrement
	if(w == 0){iCtrl++;}//down button
	else if(w == 1){iCtrl--;}//up button

	//overflow check
	if(iCtrl >= iCount){iCtrl = iCount-1;}
	if(iCtrl < 0){iCtrl = 0;}

	if(w == 0)//down button
	{
		//-------------------------------
		for(int i=iCtrl; i<iCount; i++)//spin until we get to our current tab
		{
			ITEM* pItem = (ITEM*)obArray[i];
			if(pItem->iTab == iCurTab)//scroll through current tab controls only
			{
				if(pItem->bTabStop)
				{
					pItem->wnd->SetFocus(); 
					iCtrl = i;
					return 0;
				}			
			}	
		}
		//-------------------------------
	}
	else
	{
		//-------------------------------
		for(int i=iCtrl; i>=0; i--)//spin until we get to our current tab
		{
			ITEM* pItem = (ITEM*)obArray[i];
			if(pItem->iTab == iCurTab)//scroll through current tab controls only
			{
				if(pItem->bTabStop)
				{
					pItem->wnd->SetFocus(); 
					iCtrl = i;
					return 0;
				}			
			}	
		}
		//-------------------------------
	}
	iCtrl = iOldPos; //no control was found to set focus to, we are end the end of the control set, backup

	return 0;
}

LRESULT CMyTabCtrl::ButtonPressed(WPARAM w, LPARAM l)
{
	int nID = 0;
	HWND h = (HWND)w;
	BOOL bKeyPress = (BOOL)l;
	if(h == NULL){return 0;}
	CPoint cur;
	CRect rc;
	::GetCursorPos(&cur);	
	::GetWindowRect(h,rc);
	CWnd* pWnd = CWnd::FromHandle(h);
	//make sure mouse is inside of button when released
	if((cur.x > rc.left && cur.x < rc.right && cur.y > rc.top && cur.y < rc.bottom) || bKeyPress)
	{		
		//do we have a normal push button?
		DWORD dwStyle = WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_AUTOCHECKBOX|BS_AUTORADIOBUTTON|WS_TABSTOP|WS_GROUP;
		DWORD dw = pWnd->GetStyle();
		dw &= ~(dwStyle);//remove all the styles from dw
		if(dw <= 1)//regular pushbutton
		{			
			nID = pWnd->GetDlgCtrlID();
			GetParent()->PostMessage(WM_BUTTONPRESSED,nID,0);
		}
	}
	return 0;
}