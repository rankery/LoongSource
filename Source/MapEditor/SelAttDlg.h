#pragma once
#include "afxcmn.h"
using namespace WorldBase;

// CSelAttDlg 对话框

class CSelAttDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelAttDlg)

public:
	CSelAttDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelAttDlg();

// 对话框数据
	enum { IDD = IDD_SelAtt };


	void MyInit(const TCHAR* szTitle,MapObjEdit::EType objType)
	{
		m_szTitle=szTitle;
		m_objType=objType;
	}

	//set current npc is a monster
	bool IsMonsterAboutNPC(bool IsMonster)
	{
		if(m_objType==MapObjEdit::NPC)
		{
            m_bISMonster=IsMonster;
			return true;
		}
		return false;
	}

	DWORD GetAttSel()	{	return m_sel;}


	void SetLastUsedNpcAtt(DWORD dwAttID)
	{
		m_dwLastNPCAttID=dwAttID;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CString	m_szTitle;
	MapObjEdit::EType	m_objType;

	DWORD	m_sel;

	bool    m_bISMonster;

	DWORD m_dwLastNPCAttID;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListCtrl m_hList;

private:
	void readstr(FILE *f,TCHAR *string);

public:
	afx_msg void OnLvnItemchangedAttlist(NMHDR *pNMHDR, LRESULT *pResult);
};
