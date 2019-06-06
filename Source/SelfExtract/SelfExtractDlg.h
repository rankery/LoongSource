// SelfExtractDlg.h : 头文件
//

#pragma once


class CExtractHanle;
class CPKVfs;
#include "afxwin.h"
#include "afxcmn.h"

// CSelfExtractDlg 对话框
class CSelfExtractDlg : public CDialog
{
// 构造
public:
	CSelfExtractDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SELFEXTRACT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	//获得压缩包最大需要空间
	DWORD	GetUnPackSize( CPKVfs* pCpK );



// 实现
protected:
	HICON m_hIcon;

	static DWORD ThreadUnPack( LPVOID* pParam );

	BOOL UnPackCpk(DWORD dwFatherCRC, DWORD& dwUnPackNum, CPKVfs *pReadCpk, CPKVfs *pWriteCpk);
	BOOL	CreateDirToDisc(LPCTSTR szPath);
	DWORD	CreateFileToDisc(LPCTSTR szFileName);
	VOID	SetProgressPos(INT nPos);
	BOOL	UnPackCPKFromCPK(DWORD dwDirCRC, DWORD& dwUnPackNum, CPKVfs *pReadCPK, CPKVfs *pWriteCPK);
	BOOL	UnPackSimpleFile(DWORD dwCRC, DWORD& dwUnPackNum, CPKVfs *pReadCPK);
	BOOL	RefreshVersion(const std::string& strVersion);

	BOOL	SelectPath();

	BOOL	ReadReg(TCHAR* KEY_ROOT, TCHAR* KEY_NAME, TCHAR outstr[]);

	VOID	Check();


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	
	BOOL		m_bTerminateThread;
	DWORD		m_dwProgress;
	DWORD		m_dwTotalFileNum;
	HANDLE		m_Mutex;
	CString		m_strAppPath;

public:
	CStatic m_staticFile;
	afx_msg void OnBnClickedUpdate();
	CProgressCtrl m_Progress;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CStatic Title;
	CButton Update;
	CButton Cancel;
};	
