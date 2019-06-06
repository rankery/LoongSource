#pragma once
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;

// CActMsgTableDlg 对话框

class CActMsgTableDlg : public CDialog
{
	DECLARE_DYNAMIC(CActMsgTableDlg)

public:
	CActMsgTableDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CActMsgTableDlg();

// 对话框数据
	enum { IDD = IDD_ActMsgTable };

	//--
public:
    void InitMsgTable(const TCHAR* szResName,AnimationCtrl *pAniCtrl);
	void ClearAll();
private:
	AnimationCtrl			*m_pAniCtrl;
	tstring		m_szResName;

	CListBox m_trackList;
	CListCtrl m_msgList;
	int m_nItem;
	int m_nSubItem;
	CEdit m_edit;
	AniMsgTable* m_pAniMsgTable;
	MultiMsgMap	 m_mapAniMsg;
	//vector<tagAniMsg> m_vecAniMsg;
	void SaveCurrentTrackMsg();
	bool m_bIsMsgTableInitialized;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();

	void BuildTrackList();
	void BuildMsgList();
public:
	afx_msg void OnLbnSelchangeTracklist();
	afx_msg void OnNMClickMsglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnNMDblclkMsglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDelete();
};
