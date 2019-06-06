// ActMsgTableDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Cool3DView.h"
#include "ActMsgTableDlg.h"
#include "..\Cool3D\ResSys\animsgtable.h"
#include "Cool3DViewWnd.h"
#include "Cool3DViewDoc.h"
#include ".\actmsgtabledlg.h"


// CActMsgTableDlg 对话框

IMPLEMENT_DYNAMIC(CActMsgTableDlg, CDialog)
CActMsgTableDlg::CActMsgTableDlg(CWnd* pParent /*=NULL*/)
: CDialog(CActMsgTableDlg::IDD, pParent)
{
	m_pAniCtrl=NULL;
	m_nItem = m_nSubItem = 0;
	m_pAniMsgTable = NULL;
	m_bIsMsgTableInitialized = false;
}

CActMsgTableDlg::~CActMsgTableDlg()
{
}

void CActMsgTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MsgList, m_msgList);
	DDX_Control(pDX, IDC_TrackList, m_trackList);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
}


BEGIN_MESSAGE_MAP(CActMsgTableDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_TrackList, OnLbnSelchangeTracklist)
	ON_NOTIFY(NM_CLICK, IDC_MsgList, OnNMClickMsglist)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_EN_KILLFOCUS(IDC_EDIT1, OnEnKillfocusEdit1)
	ON_NOTIFY(NM_DBLCLK, IDC_MsgList, OnNMDblclkMsglist)
	ON_BN_CLICKED(IDDELETE, OnBnClickedDelete)
END_MESSAGE_MAP()


// CActMsgTableDlg 消息处理程序

void CActMsgTableDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
	MultiMsgMap& msgMap( m_pAniMsgTable->GetMsgMap() );
	msgMap.clear();
	msgMap = m_mapAniMsg;

	/*vector<tagAniMsg>& vec(m_pAniMsgTable->GetMsgArray());
	vec.clear();
	vec = m_vecAniMsg;*/
	m_pAniMsgTable->SaveToFile(g_viewWnd->GetDocument()->GetCSVFilePath());
}

void CActMsgTableDlg::InitMsgTable(const TCHAR* szResName,AnimationCtrl *pAniCtrl)
{
	ASSERT(szResName!=NULL);
	m_szResName=szResName;
	ASSERT(pAniCtrl!=NULL);
	m_pAniCtrl=pAniCtrl;

	if(&m_edit)
		m_edit.ShowWindow(SW_HIDE);

	if(!m_bIsMsgTableInitialized)
	{
		//--Load Msg配置文件,并显示到list ctrl
		DWORD dwStyle = m_msgList.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_TRACKSELECT;
		m_msgList.SetExtendedStyle(dwStyle);
		m_msgList.InsertColumn(0, _T("index"), LVCFMT_LEFT, 40);
		m_msgList.InsertColumn(1, _T("trackName"), LVCFMT_LEFT, 70);
		m_msgList.InsertColumn(2, _T("time"), LVCFMT_LEFT, 65);
		m_msgList.InsertColumn(3, _T("msgCode"), LVCFMT_LEFT, 70);
		for(int i = 0; i < 1024; ++i)
		{
			TCHAR c[10] = { 0 };
			_stprintf(c, _T("%d"), i);
			m_msgList.InsertItem(i, c);
		}
		m_bIsMsgTableInitialized = true;
	}
	m_msgList.ShowWindow(SW_HIDE);

	//--显示track list
	BuildTrackList();
	BuildMsgList();

	m_msgList.ShowWindow(SW_SHOW);
}

void CActMsgTableDlg::BuildTrackList()
{
	//--build ani track list
	m_trackList.ResetContent();
	m_pAniMsgTable = g_viewWnd->GetDocument()->GetAniMsgTable();
	if(!m_pAniMsgTable)
		return;

	m_mapAniMsg = m_pAniMsgTable->GetMsgMap();
	//m_vecAniMsg = m_pAniMsgTable->GetMsgArray();

	vector<tstring> tracks;
	m_pAniCtrl->GetTrackSet(tracks);
	sort( tracks.begin(), tracks.end() );
	size_t n = tracks.size();
	for(vector<tstring>::iterator iter=tracks.begin();
		iter!=tracks.end();iter++)
		m_trackList.AddString(iter->c_str());
}

void CActMsgTableDlg::BuildMsgList()
{
	if(!m_mapAniMsg.empty())
	{
		int i = 0;
		TCHAR c[10] = { 0 };
		for( MultiMsgMap::iterator it = m_mapAniMsg.begin(); it != m_mapAniMsg.end(); it++ )
		{
			m_msgList.SetItemText(i, 1, _FourCC2Str(it->first).c_str() );
			_stprintf( c, _T("%f"), it->second.time );
			m_msgList.SetItemText(i, 2, c);
			m_msgList.SetItemText(i, 3, _FourCC2Str(it->second.msgCode).c_str());

			i++;
		}
		/*
		size_t n = m_vecAniMsg.size();
		for(size_t i = 0; i < n; ++i)
		{
			m_msgList.SetItemText(i, 1, _FourCC2Str(m_vecAniMsg[i].trackName).c_str());
			char c[10] = { 0 };
			sprintf(c, "%f", m_vecAniMsg[i].time);
			m_msgList.SetItemText(i, 2, c);
			m_msgList.SetItemText(i, 3, _FourCC2Str(m_vecAniMsg[i].msgCode).c_str());
		}
		*/
	}
}

void CActMsgTableDlg::OnLbnSelchangeTracklist()
{
}

void CActMsgTableDlg::OnNMClickMsglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	SaveCurrentTrackMsg();
	ClearAll();
	BuildMsgList();
	// TODO: 在此添加控件通知处理程序代码
	if (&m_edit)	m_edit.ShowWindow(SW_HIDE);
	Invalidate();
	*pResult = 0;
	LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE) pNMHDR;
	//get the row number
	m_nItem = temp->iItem;
	//get the column number
	m_nSubItem = temp->iSubItem;

	if(m_nSubItem == 0 || m_nSubItem == -1 || m_nItem == -1 )
		return ;

	//Retrieve the text of the selected subItem from the list
	CString str(m_msgList.GetItemText(m_nItem ,m_nSubItem));	

	CRect rect;
	m_msgList.GetSubItemRect(temp->iItem,temp->iSubItem,LVIR_BOUNDS,rect);

	m_msgList.ClientToScreen(rect);
	ScreenToClient(rect);
	CDC* dc =GetDC();
	dc->Rectangle(rect);

	if(!(&m_edit))
	{		
		m_edit.Create(ES_LEFT | ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD | WS_BORDER,
			rect,this,IDC_EDIT1);		
	}
	m_edit.MoveWindow(rect);	
	m_edit.ShowWindow(SW_SHOW);
	m_edit.SetFocus();

	m_edit.SetWindowText(str.GetBuffer());
}

void CActMsgTableDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString cstr;
	m_edit.GetWindowText(cstr);
	m_msgList.SetItemText(m_nItem, m_nSubItem, cstr.GetBuffer());
}

void CActMsgTableDlg::OnEnKillfocusEdit1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_edit.ShowWindow(SW_HIDE);
	Invalidate();
}


void CActMsgTableDlg::OnNMDblclkMsglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	if (&m_edit)	m_edit.ShowWindow(SW_HIDE);
	Invalidate();
	LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE) pNMHDR;
	//get the row number
	m_nItem = temp->iItem;
	//get the column number
	m_nSubItem = temp->iSubItem;
	SaveCurrentTrackMsg();
	ClearAll();
	BuildMsgList();
}

void CActMsgTableDlg::ClearAll()
{
	for(int i = 0; i < 10; ++i)
		for(int j = 1; j < 4; ++j)
			m_msgList.SetItemText(i, j, _T(""));
}

void CActMsgTableDlg::SaveCurrentTrackMsg()
{
	m_mapAniMsg.clear();

	for(int i = 0; i < 1024; ++i)
	{
		if(!m_msgList.GetItemText(i, 1).IsEmpty())
		{
			DWORD dwTrackName = _MakeFourCC(tstring(m_msgList.GetItemText(i, 1)).c_str());
			tagAniMsg tag;
			tag.time = (float)_tstof(tstring(m_msgList.GetItemText(i, 2)).c_str());
			tag.msgCode = _MakeFourCC(tstring(m_msgList.GetItemText(i, 3)).c_str());
			m_mapAniMsg.insert( make_pair( dwTrackName, tag ) );			
		}
	}

	/*
	m_vecAniMsg.clear();
	for(int i = 0; i < 1024; ++i)
	{
		if(!m_msgList.GetItemText(i, 1).IsEmpty())
		{
			tagAniMsg tag;
			tag.trackName = _MakeFourCC(string(m_msgList.GetItemText(i, 1)).c_str());
			tag.time = (float)_tstof(string(m_msgList.GetItemText(i, 2)).c_str());
			tag.msgCode = _MakeFourCC(string(m_msgList.GetItemText(i, 3)).c_str());
			m_vecAniMsg.push_back(tag);
		}
	}
	*/
}

void CActMsgTableDlg::OnBnClickedDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_nItem == -1)
		return;
	CString cstrText(m_msgList.GetItemText(m_nItem, 1));
	if(cstrText == _T(""))
		return;
	//m_msgList.SetItemText(m_nItem, 1, "");
	//m_msgList.SetItemText(m_nItem, 2, "");
	//m_msgList.SetItemText(m_nItem, 3, "");
	m_msgList.DeleteItem(m_nItem);
	SaveCurrentTrackMsg();
	//ClearAll();
	m_msgList.DeleteAllItems();
	for(int i = 0; i < 1024; ++i)
	{
		TCHAR c[10] = { 0 };
		_stprintf(c, _T("%d"), i);
		m_msgList.InsertItem(i, c);
	}
	BuildMsgList();
}