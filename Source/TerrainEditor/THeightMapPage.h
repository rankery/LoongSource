#pragma once
#include "afxwin.h"

const int HEditToolsNum=5;
const TCHAR* const HEditToolsName[]=
{
	_T("Brush"),
	_T("Flatten"),
	_T("Smooth"),
	_T("Noise"),
	_T("Apply")
};

const TCHAR* const HEditToolsInfo[]=
{
	_T("Brush:\t用鼠标在主窗口点击或者拖动,来向高度图应用笔刷;鼠标左键为笔刷正应用,鼠标右键为反应用"),
	_T("Flatten:\t使得笔刷所确定的区域变得平坦"),
	_T("SMooth:\t使得笔刷所确定的区域变得平滑"),
	_T("Noise:\t在笔刷所确定的区域随机的增加高低变化"),
	_T("Apply:\t设置笔刷所确定的区域为指定值")
};

// THeightMapPage 对话框

class THeightMapPage : public CPropertyPage
{
	DECLARE_DYNAMIC(THeightMapPage)

public:
	THeightMapPage();
	virtual ~THeightMapPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_HEIGHTMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedImporthightmap();
	afx_msg void OnBnClickedCreate();
	CListBox m_editTools;
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeEdittools();

	CString m_szEditToolInfo;
	BOOL m_bWireFrame;
protected:
	void SetDrawOption();
public:
	afx_msg void OnBnClickedWireframe();
	CString m_szName;
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	CString m_szBuildPath;
	afx_msg void OnBnClickedBuild();
	CButton m_btnBuild;
	CButton m_btnShowEdge;
	CString m_szShowDist;
	CString m_szEditDist;
	afx_msg void OnEnChangeShowdist();
	afx_msg void OnEnChangeEditdist();
	afx_msg void OnBnClickedMod();
	afx_msg void OnBnClickedEdituvset();
	afx_msg void OnBnClickedExporthightmap();
	afx_msg void OnBnClickedUndo();
	afx_msg void OnBnClickedRedo();
};
