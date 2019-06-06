#pragma once
#include "afxcmn.h"
#define WM_DELRG_DLG WM_USER+102
#define WM_NEWOBJ	WM_USER+103
#define WM_SHOWMODEL WM_USER+105
// ResourceGather 对话框
struct tagResGather
{
	TCHAR szObj[MAX_PATH];
	enum GMAPOBJ_TYPE eType;
};

class ResourceGather : public CDialog
{
	DECLARE_DYNAMIC(ResourceGather)

public:
	ResourceGather(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ResourceGather();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnDestroy();

// 对话框数据
	enum { IDD = IDD_ResourceGather };
	const tagResGather& GetRG()const{return m_tagRG;}
	const CString& GetModelPath()const {return m_szMdlPath;}
	bool GetIsShow()const {return m_bShow;}
	void UpdateList();
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedNewadd();
	CListCtrl					m_ListCtrl;
	//CString						m_sel;
	tagResGather				m_tagRG;				//增加一个地图对象时返回的地图对象
	CWnd*						m_pParent;
	CString						m_szMdlPath;
	bool						m_bShow;
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedRead();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnHdnItemdblclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
};
