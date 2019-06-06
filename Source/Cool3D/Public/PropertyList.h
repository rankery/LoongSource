#if !defined(AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_)
#define AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyList.h : header file
//
#include "..\Math\Color.h"
using namespace Cool3D;

#define PIT_COMBO	0  //PIT = property item type
#define PIT_EDIT	1
#define PIT_COLOR	2
#define PIT_FONT	3
#define PIT_FILE	4

#define IDC_PROPCMBBOX   712
#define IDC_PROPEDITBOX  713
#define IDC_PROPBTNCTRL  714


/////////////////////////////////////////////////////////////////////////////
//CPropertyList Items
class CPropertyItem
{
// Attributes
public:
	CString m_propName;
	CString m_curValue;
	int m_nItemType;
	CString m_cmbItems;

public:
	CPropertyItem(CString propName, CString curValue,
				  int nItemType, CString cmbItems)
	{
		m_propName = propName;
		m_curValue = curValue;
		m_nItemType = nItemType;
		m_cmbItems = cmbItems;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CPropertyList window

class CPropertyList : public CListBox
{
// Construction
public:
	CPropertyList();

// Attributes
public:

// Operations
public:
	int AddItem(CString txt);
	int AddPropItem(CPropertyItem* pItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyList)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyList();

	void SetFileFilter(const TCHAR* szFilter)	{	m_szFileFilter=szFilter;}
	static COLORREF Str2Color(const TCHAR* szRGBStr);

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchange();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnKillfocusCmbBox();
	afx_msg void OnSelchangeCmbBox();
	afx_msg void OnKillfocusEditBox();
	afx_msg void OnChangeEditBox();
	afx_msg void OnButton();

	DECLARE_MESSAGE_MAP()

	void InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo);
	void DisplayButton(CRect region);

	CComboBox m_cmbBox;
	CEdit m_editBox;
	CButton m_btnCtrl;
	CFont m_SSerif8Font;
	
	int m_curSel,m_prevSel;
	int m_nDivider;
	int m_nDivTop;
	int m_nDivBtm;
	int m_nOldDivX;
	int m_nLastBox;
	BOOL m_bTracking;
	BOOL m_bDivIsSet;
	HCURSOR m_hCursorArrow;
	HCURSOR m_hCursorSize;

	CString	m_szFileFilter;

public:
	//--updage×åº¯Êý,add by yanliang
	void UpdateVal(const TCHAR* szName,float& v,int index,bool bSave);
	void UpdateVal(const TCHAR* szName,int& v,int index,bool bSave);
	void UpdateVal(const TCHAR* szName,BYTE& v,int index,bool bSave);
	void UpdateVal(const TCHAR* szName,TCHAR* szBuf,int bufSize,int index,bool bSave,bool bFileName);
	void UpdateVal(const TCHAR* szName,bool& v,int index,bool bSave);
	void UpdateVal(const TCHAR* szName,Color4f& v,int index,bool bSave);//RGB only,not alpha

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_)
