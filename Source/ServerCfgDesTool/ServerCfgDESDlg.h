// ServerCfgDESDlg.h : 头文件
//

#pragma once


// CServerCfgDESDlg 对话框
class CServerCfgDESDlg : public CDialog
{
// 构造
public:
	CServerCfgDESDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CServerCfgDESDlg();

// 对话框数据
	enum { IDD = IDD_ServerCfgDES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


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
	afx_msg void OnBnClickedOk();

private:
	//根据被加密文件名生成加密文件名(将后缀改为.dat)
	CString GetSaveFileName() const;
	//随机生成加密号
	INT GenDesCode();

	//读取被加密文件，并加密后保存
	BOOL EncryptAndSave();
	//读取加密文件，并解密后保存(测试用)
	BOOL DecryptAndSave();

public:
	afx_msg void OnBnClickedButtonBrowse();

private:
	TObjRef<Util>	m_pUtil;

	CString m_strFileName;
};
