// PropertyList.cpp : 实现文件
//

#include "stdafx.h"

#include "PropertyList.h"
#include ".\propertylist.h"


// CPropertyList

IMPLEMENT_DYNAMIC(CPropertyList, CListBox)
CPropertyList::CPropertyList()
{
}

CPropertyList::~CPropertyList()
{
	DeleteAllItem();
}


BEGIN_MESSAGE_MAP(CPropertyList, CListBox)
	//{{AFX_MSG_MAP(CPropertyList)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnLbnSelchange)
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_CBN_KILLFOCUS(IDC_PROPCMBBOX, OnKillfocusCmbBox)
	ON_CBN_SELCHANGE(IDC_PROPCMBBOX, OnSelchangeCmbBox)
	ON_EN_KILLFOCUS(IDC_PROPEDITBOX, OnKillfocusEditBox)
	ON_EN_CHANGE(IDC_PROPEDITBOX, OnChangeEditBox)
	ON_BN_CLICKED(IDC_PROPBTNCTRL, OnButton)
END_MESSAGE_MAP()

int CPropertyList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;


	m_bDivIsSet = FALSE;
	m_nDivider = 0;
	m_bTracking = FALSE;

	m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	m_SSerif8Font.CreatePointFont(80,_T("MS Sans Serif"));

	return 0;
}

void CPropertyList::OnLbnSelchange()
{
	CRect rect;
	CString lBoxSelText;

	GetItemRect(m_curSel,rect);
	rect.left = m_nDivider;

	CPropertyItem* pItem = (CPropertyItem*) GetItemDataPtr(m_curSel);
  
	if (m_btnCtrl)
		m_btnCtrl.ShowWindow(SW_HIDE);

	if (pItem->m_nItemType==PIT_COMBO)
	{

		m_nLastBox = 0;
		if (m_cmbBox)
			m_cmbBox.MoveWindow(rect);
		else
		{	
			rect.bottom += 200;
			m_cmbBox.Create(CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_BORDER,
				rect,this,IDC_PROPCMBBOX);
			m_cmbBox.SetFont(&m_SSerif8Font);
		}

		CString cmbItems = pItem->m_cmbItems;
		lBoxSelText = pItem->m_curValue;

		m_cmbBox.ResetContent();
		//m_cmbBox.AddString("");		
		int i,i2;
		i=0;
		while ((i2=cmbItems.Find(_T('|'),i)) != -1)
		{
			m_cmbBox.AddString(cmbItems.Mid(i,i2-i));
			i=i2+1;
		}

		m_cmbBox.ShowWindow(SW_SHOW);
		m_cmbBox.SetFocus();

		int j = m_cmbBox.FindStringExact(0,lBoxSelText);
		if (j != CB_ERR)
			m_cmbBox.SetCurSel(j);
		else
			m_cmbBox.SetCurSel(0);
	}
	else if (pItem->m_nItemType==PIT_EDIT)
	{
		m_nLastBox = 1;
		m_prevSel = m_curSel;
		rect.bottom -= 3;
		if (m_editBox)
			m_editBox.MoveWindow(rect);
		else
		{	
			m_editBox.Create(ES_LEFT | ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD | WS_BORDER,
				rect,this,IDC_PROPEDITBOX);
			m_editBox.SetFont(&m_SSerif8Font);
		}

		lBoxSelText = pItem->m_curValue;

		m_editBox.ShowWindow(SW_SHOW);
		m_editBox.SetFocus();
		m_editBox.SetWindowText(lBoxSelText);
	}
	else
		DisplayButton(rect);
}

void CPropertyList::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		m_bTracking = FALSE;
		if (GetCapture()==this)
			::ReleaseCapture();

		::ClipCursor(NULL);

		CClientDC dc(this);
		InvertLine(&dc,CPoint(point.x,m_nDivTop),CPoint(point.x,m_nDivBtm));
		m_nDivider = point.x;
		Invalidate();
	}
	else
	{
		BOOL loc;
		int i = ItemFromPoint(point,loc);
		m_curSel = i;
		CListBox::OnLButtonUp(nFlags, point);
	}
}

void CPropertyList::OnKillFocus(CWnd* pNewWnd)
{
	CListBox::OnKillFocus(pNewWnd);

}

void CPropertyList::OnLButtonDown(UINT nFlags, CPoint point)
{	
	if ((point.x>=m_nDivider-5) && (point.x<=m_nDivider+5))
	{
		::SetCursor(m_hCursorSize);

		CRect windowRect;
		GetWindowRect(windowRect);
		windowRect.left += 10; windowRect.right -= 10;
		::ClipCursor(windowRect);

		if (m_cmbBox)
			m_cmbBox.ShowWindow(SW_HIDE);
		if (m_editBox)
			m_editBox.ShowWindow(SW_HIDE);

		CRect clientRect;
		GetClientRect(clientRect);

		m_bTracking = TRUE;
		m_nDivTop = clientRect.top;
		m_nDivBtm = clientRect.bottom;
		m_nOldDivX = point.x;

		CClientDC dc(this);
		InvertLine(&dc,CPoint(m_nOldDivX,m_nDivTop),CPoint(m_nOldDivX,m_nDivBtm));
		SetCapture();
	}
	else
	{
		m_bTracking = FALSE;
		CListBox::OnLButtonDown(nFlags, point);
	}
}

void CPropertyList::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		CClientDC dc(this);
		InvertLine(&dc,CPoint(m_nOldDivX,m_nDivTop),CPoint(m_nOldDivX,m_nDivBtm));
		InvertLine(&dc,CPoint(point.x,m_nDivTop),CPoint(point.x,m_nDivBtm));
		m_nOldDivX = point.x;
	}
	else if ((point.x >= m_nDivider-5) && (point.x <= m_nDivider+5))
		::SetCursor(m_hCursorSize);
	else
		CListBox::OnMouseMove(nFlags, point);
}

void CPropertyList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 20; //pixels
}

void CPropertyList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	CRect rectFull = lpDrawItemStruct->rcItem;
	CRect rect = rectFull;
	if (m_nDivider==0)
		m_nDivider = rect.Width() / 2;
	rect.left = m_nDivider;
	CRect rect2 = rectFull;
	rect2.right = rect.left - 1;
	UINT nIndex = lpDrawItemStruct->itemID;

	if (nIndex != (UINT) -1)
	{
		dc.FillSolidRect(rect2,RGB(255,255,255));
		dc.DrawEdge(rect2,EDGE_SUNKEN,BF_BOTTOMRIGHT);
		dc.DrawEdge(rect,EDGE_SUNKEN,BF_BOTTOM);

		CPropertyItem* pItem = (CPropertyItem*) GetItemDataPtr(nIndex);
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(pItem->m_propName,CRect(rect2.left+3,rect2.top+3,
			rect2.right-3,rect2.bottom+3),
			DT_LEFT | DT_SINGLELINE);

		CRect rc2=CRect(rect.left+3,rect.top+3,
			rect.right-3,rect.bottom-3);
		if(pItem->m_nItemType==PIT_COLOR)
		{
			rc2.right-=32;//留下按钮的宽度
			dc.FillSolidRect(&rc2,Str2Color(pItem->m_curValue));
		}
		else
		{
			dc.DrawText(pItem->m_curValue,CRect(rect.left+3,rect.top+3,
				rect.right+3,rect.bottom+3),
				DT_LEFT | DT_SINGLELINE);
		}
	}
	dc.Detach();
	
}

BOOL CPropertyList::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CListBox::PreCreateWindow(cs))
		return false;

	cs.style &= ~(LBS_OWNERDRAWVARIABLE | LBS_SORT);
	cs.style |= LBS_OWNERDRAWFIXED;

	m_bTracking = FALSE;
	m_nDivider = 0;
	m_bDivIsSet = FALSE;

	return true;
}

void CPropertyList::PreSubclassWindow()
{
	m_bDivIsSet = FALSE;
	m_nDivider = 0;
	m_bTracking = FALSE;
	m_curSel = 1;

	m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	m_SSerif8Font.CreatePointFont(80,_T("MS Sans Serif"));
}


void CPropertyList::InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo)
{
	int nOldMode = pDC->SetROP2(R2_NOT);

	pDC->MoveTo(ptFrom);
	pDC->LineTo(ptTo);

	pDC->SetROP2(nOldMode);
}


void CPropertyList::OnKillfocusCmbBox() 
{
	m_cmbBox.ShowWindow(SW_HIDE);

	Invalidate();
}

void CPropertyList::OnKillfocusEditBox()
{
	CString newStr;
	m_editBox.ShowWindow(SW_HIDE);

	Invalidate();
}

void CPropertyList::OnSelchangeCmbBox()
{
	CString selStr;
	if (m_cmbBox)
	{
		m_cmbBox.GetLBText(m_cmbBox.GetCurSel(),selStr);
		CPropertyItem* pItem = (CPropertyItem*) GetItemDataPtr(m_curSel);
		pItem->m_curValue = selStr;
	}
}

void CPropertyList::OnChangeEditBox()
{
	CString newStr;
	m_editBox.GetWindowText(newStr);

	CPropertyItem* pItem = (CPropertyItem*) GetItemDataPtr(m_curSel);
	pItem->m_curValue = newStr;
}

void CPropertyList::OnButton()
{
	CPropertyItem* pItem = (CPropertyItem*) GetItemDataPtr(m_curSel);

	if(m_pItemList.empty()) return;

	if (pItem->m_nItemType == PIT_COLOR)
	{
		COLORREF initClr;
		CString currClr = pItem->m_curValue;
		if (currClr.Find(_T("RGB")) > -1)
		{
			int j = currClr.Find(_T(','),3);
			CString bufr = currClr.Mid(4,j-4);
			int RVal = _ttoi(bufr);
			int j2 = currClr.Find(_T(','),j+1);
			bufr = currClr.Mid(j+1,j2-(j+1));
			int GVal = _ttoi(bufr);
			int j3 = currClr.Find(_T(')'),j2+1);
			bufr = currClr.Mid(j2+1,j3-(j2+1));
			int BVal = _ttoi(bufr);
			initClr = RGB(RVal,GVal,BVal);
		}
		else
			initClr = 0;

		CColorDialog ClrDlg(initClr, 0, this);

		if (IDOK == ClrDlg.DoModal())
		{
			COLORREF selClr = ClrDlg.GetColor();
			CString clrStr;
			clrStr.Format(_T("RGB(%d,%d,%d)"),GetRValue(selClr),
				GetGValue(selClr),GetBValue(selClr));
			m_btnCtrl.ShowWindow(SW_HIDE);

			pItem->m_curValue = clrStr;
			Invalidate();
		}
		else
		{
			m_btnCtrl.ShowWindow(SW_HIDE);
			Invalidate();
		}
		SetFocus();
	}
	else if (pItem->m_nItemType == PIT_FILE)
	{
		CString SelectedFile; 
		CString Filter("All Files (*.*)|*.*||");

		CFileDialog FileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
			Filter);

		CString currPath = pItem->m_curValue;
		FileDlg.m_ofn.lpstrTitle = _T("Select file");
		if (currPath.GetLength() > 0)
			FileDlg.m_ofn.lpstrInitialDir = currPath.Left(
			currPath.GetLength() - currPath.ReverseFind(_T('\\')));

		if(IDOK == FileDlg.DoModal())
		{
			SelectedFile = FileDlg.GetPathName();
			
			TCHAR szWorkDir[MAX_PATH];
			_tcscpy( szWorkDir, Kernel::Inst()->GetWorkPath() );
			//--文件相对路径名
			TCHAR* find = ::_tcsrchr(szWorkDir, _T('\\'));
			if(find != NULL)
				*(find+1) = 0;
			_tcslwr(szWorkDir);
			SelectedFile.Delete(0, _tcslen(szWorkDir));

			m_btnCtrl.ShowWindow(SW_HIDE);

			pItem->m_curValue = SelectedFile;
			Invalidate();
		}
		else
		{	
			m_btnCtrl.ShowWindow(SW_HIDE);
			Invalidate();
		}
	}
	else if (pItem->m_nItemType == PIT_FONT)
	{	
		CFontDialog FontDlg(NULL,CF_EFFECTS | CF_SCREENFONTS,NULL,this);

		if(IDOK == FontDlg.DoModal())
		{
			CString faceName = FontDlg.GetFaceName();

			m_btnCtrl.ShowWindow(SW_HIDE);

			pItem->m_curValue = faceName;
			Invalidate();
		}
	}
}


int CPropertyList::AddPropItem(CPropertyItem* pItem)
{
	int nIndex = AddString(_T(""));
	SetItemDataPtr(nIndex,pItem);
	m_pItemList.push_back(pItem);
	return nIndex;
}

void CPropertyList:: DeleteAllItem()
{
	int i = 0;
	vector<CPropertyItem*>::iterator pIter = m_pItemList.begin();
	for(; pIter != m_pItemList.end(); ++pIter)
	{	
		CPropertyItem* pItem = *pIter;
		delete pItem;
	}
	m_pItemList.clear();
	if(m_btnCtrl)
		m_btnCtrl.ShowWindow(SW_HIDE);
	if(m_hWnd!=NULL)
		ResetContent();	
}


void CPropertyList:: DisplayButton(CRect region)
{
	m_nLastBox = 2;
	m_prevSel = m_curSel;

	if (region.Width() > 25)
		region.left = region.right - 25;
	region.bottom -= 3;

	if (m_btnCtrl)
		m_btnCtrl.MoveWindow(region);
	else
	{	
		m_btnCtrl.Create(_T("..."),BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD,
			region,this,IDC_PROPBTNCTRL);
		m_btnCtrl.SetFont(&m_SSerif8Font);
	}

	m_btnCtrl.ShowWindow(SW_SHOW);
	m_btnCtrl.SetFocus();
}

COLORREF CPropertyList::Str2Color(const TCHAR* szRGBStr)
{
	COLORREF initClr;
	CString currClr =szRGBStr;
	//parse the property's current color value
	if (currClr.Find(_T("RGB")) > -1)
	{
		int j = currClr.Find(_T(','),3);
		CString bufr = currClr.Mid(4,j-4);
		int RVal = _ttoi(bufr);
		int j2 = currClr.Find(_T(','),j+1);
		bufr = currClr.Mid(j+1,j2-(j+1));
		int GVal = _ttoi(bufr);
		int j3 = currClr.Find(_T(')'),j2+1);
		bufr = currClr.Mid(j2+1,j3-(j2+1));
		int BVal = _ttoi(bufr);
		initClr = RGB(RVal,GVal,BVal);
	}
	else
		initClr = 0;

	return initClr;
}
