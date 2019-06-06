#pragma once


// DownloadInfo dialog

class DownloadInfo : public CDialog
{
	DECLARE_DYNAMIC(DownloadInfo)

public:
	DownloadInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~DownloadInfo();

	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_DOWNLOADINFO };
	CEdit	m_ctrlEdit;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
