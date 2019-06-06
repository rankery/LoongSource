#pragma once
#include "afxwin.h"


// CMainDlg dialog

class CMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainDlg();

// Dialog Data
	enum { IDD = IDD_MAINDLG };

	void GetBrushRadius(int& inner,int& outer);
	int  GetBrushShape();
	BOOL GetCango(){ return m_bCango; }
	BOOL GetPlayerNav(){ return m_bPlayerNav; }
	BOOL GetNpcNav(){ return m_bNpcNav; }
	BOOL GetPlayerRoad(){ return m_bPlayerRoad;}
	BOOL GetNPCWayPoint(){ return m_bNPCWayPoint;}
	BOOL IsEditCango(){ return m_bEditCango;}
	BOOL IsEditPlayerRoad(){ return m_bEditPlayerRoad;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedClear();
	afx_msg void OnEnChangeShowdist();
	afx_msg void OnBnClickedPlayerNavLoad();
	afx_msg void OnBnClickedNpcNavLoad();
	afx_msg void OnBnClickedButtonNpcNavlook();
	afx_msg void OnBnClickedButtonPlayerNavLook();
	afx_msg void OnBnClickedClearPlayerRoad();
	afx_msg void OnBnClickedButtonNpcwaypointload();

	CComboBox m_shapeTypeList;
	CString m_szInner;
	CString m_szOuter;
	BOOL m_bCango;
	BOOL m_bNpcNav;
	BOOL m_bPlayerNav;
	BOOL m_bPlayerRoad;
	BOOL m_bNPCWayPoint;
	BOOL m_bEditCango;
	BOOL m_bEditPlayerRoad;
	afx_msg void OnBnClickedButtonCangolook();
	afx_msg void OnBnClickedButtonRoadlook();
};
