// CPackView.h : interface of the CCPackView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPACKVIEW_H__F3B07995_79D3_4B93_BF20_BC2C060CECA5__INCLUDED_)
#define AFX_CPACKVIEW_H__F3B07995_79D3_4B93_BF20_BC2C060CECA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCPackView;
class CMyOleSource : public COleDataSource
{
	CCPackView* m_pView;
	BOOL		m_bReDoDrop;

public:
	CMyOleSource(CCPackView* pView)
	{
		m_pView = pView;
		m_bReDoDrop = TRUE;
	}

	void SetView(CCPackView* pView)
	{
		m_pView = pView;
	}

	virtual BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc,HGLOBAL* phGlobal );
};


class CDragSource : public COleDropSource
{
	CCPackView* m_pView;
	DWORD m_dwLastEffect;

public:
	CDragSource(CCPackView* pView) { m_pView = pView; }

	SCODE GiveFeedback(DWORD dwEffect);
	SCODE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
};


class CCPackView : public CListView
{
	INT		m_nDirCounter;
	INT		m_nFileCounter;
	INT		m_nSelFileNum;
	DWORD	m_dwAllPackSize;
	DWORD	m_dwAllOriginSize;

	static INT		m_iSortColumn;
	static BOOL	    m_bAsc;
	
public:
	VOID	DoRealDrop();
	HGLOBAL GetSelectedFileNameForDrop();
	void	DropFiles(HDROP hDropInfo) { OnDropFiles(hDropInfo); }

protected: // create from serialization only
	CCPackView();
	DECLARE_DYNCREATE(CCPackView)

// Attributes
public:
	CCPackDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCPackView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCPackView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CCPackView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditPaste();
	afx_msg void OnEditCopy();
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg LRESULT	OnAddItem( WPARAM wParam, LPARAM lparam );
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
};

#ifndef _DEBUG  // debug version in CPackView.cpp
inline CCPackDoc* CCPackView::GetDocument()
   { return (CCPackDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPACKVIEW_H__F3B07995_79D3_4B93_BF20_BC2C060CECA5__INCLUDED_)
