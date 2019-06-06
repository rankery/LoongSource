// LoongQuestEditorDlg.h : header file
//

#pragma once
#include "afxcmn.h"

#include "MyTabCtrl.h"
#include "afxwin.h"

struct tagQuestProto;

// CLoongQuestEditorDlg dialog
class CLoongQuestEditorDlg : public CDialog
{
// Construction
public:
	CLoongQuestEditorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LOONGQUESTEDITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CMyTabCtrl m_ctrlTab;
	CTreeCtrl m_ctrlQuestTree;
	CButton m_ctrlNewQuest;
	CButton m_ctrlSaveQuest;
	CButton m_ctrlSaveAsQuest;
	CButton m_ctrlDeleteQuest;
	CButton m_ctrlQuestPath;
	CButton m_ctrlRefreshQuestTree;
	CButton m_ctrlExportQuestAcceptRelation;
	CButton m_ctrlExportQuestCompleteRelation;
private:
	void InitTabCtrl();
	void SaveQuest( tagQuestProto* pQuest, LPCTSTR szFileName );
	
	//从注册表读出任务路径
	BOOL ReadQuestPathFromReg();
	//将任务路径写入注册表
	BOOL WriteQuestPathToReg();

	//加载任务文件
	BOOL LoadQuest();

	//遍历任务路径下的所有文件夹和文件
	void FindAllQuestXMLFile( LPCTSTR szQuestPath );

	//更新 Tab 控件数据
	void UpdateTabCtrl( tagQuestProto* pQuest );

	//更新 Tree 控件数据
	void UpdateTreeCtrl();

	//
	void LoadQuestDataFromXML();

	//接取任务NPC和任务对照文件
	BOOL SaveAcceptQuestNPCXml( LPCTSTR szFileName );

	//完成任务NPC和任务对照文件
	BOOL SaveCompleteQuestNPCXml( LPCTSTR szFileName );

public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnSetQuestPath();
	
	LRESULT ButtonPressed(WPARAM w, LPARAM l);

private:
	//任务路径
	wstring m_wstrQuestPath;

	//对话框同时只能显示一份任务数据，所以需要保存当前任务的一些信息
	//当前任务文件名
	wstring m_wstrCurrentQuestFileName;
	//当前任务指针
	tagQuestProto* m_pCurrentQuest;

	//目录集合
	vector< wstring > m_vecPath;
	//文件集合
	vector< wstring > m_vecQuestXMLFile;

	//任务文件名， 目录名存	( RXS_001.xml  d:\quest\新手 )
	map< wstring, wstring > m_mapQuestXMLFile;

	//
	map< wstring, tagQuestProto* > m_mapQuestData;

	list< tagQuestProto* > m_listDeleteQuest;

	list< wstring > m_listDeleteQuestFile;

public:
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNewQuest();
	afx_msg void OnDeleteQuest();
	afx_msg void OnOk();
	afx_msg void OnRefreshQuesTree();
	
	afx_msg void OnBtnExportQuestAcceptRelation();
	
	afx_msg void OnBtnExportQuestCompleteRelation();
};
