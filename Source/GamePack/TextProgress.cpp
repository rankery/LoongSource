#include "StdAfx.h"
#include ".\textprogress.h"

CTextProgress::CTextProgress(void)
{
	m_nPos = 0;
	m_nStepSize = 1;
	m_nMax = 100;
	m_nMin = 0;
	m_bShowText = TRUE;
	m_strText.Empty();
	m_colFore = ::GetSysColor(COLOR_HIGHLIGHT);
	m_colBk = ::GetSysColor(COLOR_WINDOW);
	m_colTextFore = ::GetSysColor(COLOR_HIGHLIGHT);
	m_colTextBk = ::GetSysColor(COLOR_WINDOW);

	m_nBarWidth = -1;

}

CTextProgress::~CTextProgress(void)
{
}


BEGIN_MESSAGE_MAP(CTextProgress, CProgressCtrl)
	//{{AFX_MSG_MAP(CTextProgressCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CTextProgress::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CTextProgress::OnSize(UINT nType, int cx, int cy)
{

}


void CTextProgress::OnPaint() 
{
	if (m_nMin >= m_nMax) 
		return;

	CRect LeftRect, RightRect, ClientRect;
	GetClientRect(ClientRect);

	double Fraction = (double)(GetPos() - m_nMin) / ((double)(m_nMax - m_nMin));

	CPaintDC PaintDC(this); // device context for painting
	CMemDC dc(&PaintDC);
	//CPaintDC dc(this); // device context for painting (if not double buffering)

	LeftRect = RightRect = ClientRect;

	LeftRect.right = LeftRect.left + (int)((LeftRect.right - LeftRect.left)*Fraction);
	dc.FillSolidRect(LeftRect, m_colFore);

	RightRect.left = LeftRect.right;
	dc.FillSolidRect(RightRect, m_colBk);

	if (m_bShowText)
	{
		CString str;
		if (m_strText.GetLength())
			str = m_strText;
		else
			str.Format(_T("%d%%"), (int)(Fraction*100.0));

		dc.SetBkMode(TRANSPARENT);

		CRgn rgn;
		rgn.CreateRectRgn(LeftRect.left, LeftRect.top, LeftRect.right, LeftRect.bottom);
		dc.SelectClipRgn(&rgn);
		dc.SetTextColor(m_colTextBk);

		dc.DrawText(str, ClientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		rgn.DeleteObject();
		rgn.CreateRectRgn(RightRect.left, RightRect.top, RightRect.right, RightRect.bottom);
		dc.SelectClipRgn(&rgn);
		dc.SetTextColor(m_colTextFore);

		dc.DrawText(str, ClientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

