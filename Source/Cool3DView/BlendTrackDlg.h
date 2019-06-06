#pragma once


// CBlendTrackDlg dialog

class CBlendTrackDlg : public CDialog
{
	DECLARE_DYNAMIC(CBlendTrackDlg)

public:
	CBlendTrackDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBlendTrackDlg();

// Dialog Data
	enum { IDD = IDD_BlendTrack };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_szUpperSkeAniName;
	CString m_szLowerSkeAniName;
	CString m_szAllSkeAniName;
	float m_fUpperWeight;
	float m_LowerWeight;
	float m_fAllWeight;
};
