// CPackDoc.h : interface of the CCPackDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPACKDOC_H__209591D4_3C9B_4E33_AC59_203E3AFC8945__INCLUDED_)
#define AFX_CPACKDOC_H__209591D4_3C9B_4E33_AC59_203E3AFC8945__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CPK;
class CFastCPK;

class CCPackDoc : public CDocument
{
protected: // create from serialization only
	CCPackDoc();
	DECLARE_DYNCREATE(CCPackDoc)

private:
	CPK*		m_pCpk;
	CImageList	m_IconList;
	CImageList	m_BigIconList;

// Attributes
public:


// Operations
public:

	CPK*		GetCpk() { return m_pCpk; }
	CImageList*	GetIconList() { return &m_IconList; }
	CImageList*	GetBigIconList() { return &m_BigIconList; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCPackDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCPackDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
public:
	//{{AFX_MSG(CCPackDoc)
	afx_msg void OnAdd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPACKDOC_H__209591D4_3C9B_4E33_AC59_203E3AFC8945__INCLUDED_)
