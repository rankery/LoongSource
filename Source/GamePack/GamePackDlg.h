// GamePackDlg.h : 头文件
//

#pragma once

#include "textprogress.h"
#include "sfp_trunk.h"
#include "afxwin.h"

// 定义线程函数类型
typedef UINT (WINAPI* THREADPROC)(LPVOID);
class CPK;
class CSelfExtract;
class CMD5;
//-----------------------------------------------------------
// CGamePackDlg 对话框
class CGamePackDlg : public CDialog 
{
// 构造
public:
	CGamePackDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GAMEPACK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	CTextProgress				m_Progress;		//进度条
	TSFPTrunk<CGamePackDlg>		m_Trunk;
	HANDLE						m_hGamePackThread;	//打包线程句柄
	CSelfExtract*				m_pCpk;
	CPK*						m_pNewCpk;
	CPK*						m_pOldCpk;
	CMD5*						m_pMD5;
	FLOAT						m_fIntePos;			//
	INT							m_nFileNum;			//比对的文件数
	DWORD						m_dwTick;
	volatile	BOOL			m_bTerminateGamePack;	//结束线程标志
	volatile	BOOL			m_bPacking;			//是否正在打包

	CEdit*						m_pETOld;		//老版本目录
	CEdit*						m_pETNew;		//新版本目录
	CEdit*						m_pETSave;		//升级包存储目录
	CStatic*					m_pStatic;		
	CStatic*					m_pSTimer;		//显示花费时间的静态框
public:
	//版本打包线程
	UINT	GamePackThread(LPVOID);
	BOOL	GamePack(TCHAR *szNewPath, TCHAR *szOldPath);
protected:
	//检测该文件扩展名是不是.cpk。如果要真实判断，还需要判断其文件内容
	BOOL	IsCPKFile(TCHAR *szFileName);
	//比较两个MD5是否相等
	BOOL	IsEqualForMD5(BYTE digest1[16], BYTE digest2[16]);
	//比对新旧两个版本的cpk文件
	BOOL	CompareCPK(TCHAR *szNewCPK, TCHAR *szOldCPK);
	// -->> 将指定键值写入注册表中指定路径,如果指定路径没有,则创建并写入
	BOOL	WriteReg(TCHAR* KEY_ROOT/*注册表中的路径*/, TCHAR* KEY_NAME, LPCSTR sz);
	// -->> 读取注册表中指定路径的键值
	BOOL	ReadReg(TCHAR *KEY_ROOT, TCHAR* KEY_NAME, DWORD& key_value);
	BOOL	ReadReg(TCHAR* KEY_ROOT, TCHAR* KEY_NAME, TCHAR outstr[]);
	
	//返回False表示退出
	BOOL	AddFileAndDir( LPCTSTR lpFullName, DWORD dwFatherCrc = 0 );
// 实现
protected:
	HICON m_hIcon;
	
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtNewdir();
	afx_msg void OnBnClickedBtOlddir();
	afx_msg void OnBnClickedBtSavedir();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtSavedir2();
	afx_msg void OnBnClickedCheckUpdate();
	afx_msg void OnStnClickedStFile();
	afx_msg void OnEnChangeEditVersionMin();
	afx_msg void OnEnChangeEditVersionMax();
	
	BOOL m_bAddexe;
	CString m_strVersionSrc;
	CString m_strVersionDest;

	CEdit m_EditVersionSrc;
	CEdit m_EditVersionDest;
	CEdit m_pExePath;
	afx_msg void OnEnChangeEtSavedir();

	CString		m_strVersion;
};



