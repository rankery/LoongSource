#pragma once


// CPropertyList


enum EPIT_TYPE
{
	PIT_COMBO  = 0,
	PIT_EDIT   = 1,
	PIT_COLOR   = 2,
	PIT_FONT   = 3,
	PIT_FILE   = 4,
	
	PIT_NUM    = 32
};


#define IDC_PROPCMBBOX   712
#define IDC_PROPEDITBOX  713
#define IDC_PROPBTNCTRL  714


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
class CPropertyList : public CListBox
{
	DECLARE_DYNAMIC(CPropertyList)

public:
	CPropertyList();
	virtual ~CPropertyList();

	int AddItem(CString txt);
	int AddPropItem(CPropertyItem* pItem);
	void DeleteAllItem(void);

	int GetListSize() { return m_pItemList.size();}


protected:

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

	vector<CPropertyItem*> m_pItemList;
	
public:
	//{{AFX_MSG(CPropertyList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLbnSelchange();
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
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
public:
	static COLORREF Str2Color(const TCHAR* szRGBStr);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
public:
};


