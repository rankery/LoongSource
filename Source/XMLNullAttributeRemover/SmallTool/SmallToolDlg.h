// SmallToolDlg.h : 头文件
//

#pragma once



#include "tinystr.h"
#include "tinyxml.h"
#include <string>
#include <list>
#include "afxcmn.h"



// CSmallToolDlg 对话框
class CSmallToolDlg : public CDialog
{
// 构造
public:
	CSmallToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SMALLTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CMenu m_menu;
	CProgressCtrl m_ProgressBar;
	CString m_szInfo;
	BOOL m_bBackUp;
	std::list<CString> m_listFilePaths;
	BOOL m_bNotifyThreadEnd;
	HANDLE m_hThread;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
	void HandleOneXmlFile(const char *filename);
	void RemoveNullAttribute(TiXmlElement *child);
	void SetInfo(const char *filename, ULONGLONG oldSize, ULONGLONG newSize);
	void SearchAndAddToFileLists(CString szFile);
	static DWORD WINAPI ThreadFunc(PVOID param);
public:
	afx_msg LRESULT OnUpdateDataMessage(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnThreadEnd(WPARAM wparam, LPARAM lparam);
	afx_msg void OnFileOpensinglefile();
	afx_msg void OnFileFolder();
	afx_msg void OnBnClickedCheckBackup();
	afx_msg void OnAbout();
	afx_msg void OnClose();
};
