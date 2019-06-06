/////////////////////////////////////////////////////////////////////////////
// Program:     loong任务编辑器
// Name:        NoticePage.cpp
// Created:     2009-02-04
// Info:		公告页面
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "wx/wx.h"
#include "wx/busyinfo.h"
#include "wx/panel.h"
#include "wx/bookctrl.h"
#include "wx/combobox.h"
#include "LoongGMTool.h"
#include "common.h"
#include "NoticePage.h"
#include "mylistctrl.h"


//控件ID
enum
{
	NoticePage_BTN_PMD = 100,
	NoticePage_Color_PMD,
	NoticePage_BTN_ZDXH,
	NoticePage_Color_ZDXH,
	NoticePage_List_ZDXH,
	NoticePage_OnlinePrompt,
	NoticePage_List_YXJ,
};

BEGIN_EVENT_TABLE(NoticePage, wxPanel)
END_EVENT_TABLE()

NoticePage::NoticePage(wxBookCtrlBase *book, const wxString& label) : m_label(label),wxPanel(book, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
														  wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
	//巨大的构造函数
	wxSizer* sizerAll = new wxBoxSizer(wxHORIZONTAL);
	
	//布局-左
	wxSizer* sizerLeft = new wxBoxSizer(wxVERTICAL);
	wxStaticBox *box1 = new wxStaticBox(this, wxID_ANY, wxT("跑马灯"));
	wxSizer* sizerBox_pmd = new wxStaticBoxSizer(box1, wxVERTICAL);

	wxSizer* sizeRow = NULL;
	sizeRow = CreateSizerWithTextAndLabel(this, wxT("公告内容"), wxID_ANY, &m_text_pmd);
	sizerBox_pmd->Add(sizeRow, 0, wxGROW | wxALL, CONTROL_BORDER);

	wxSizer* sizerTmp = new wxBoxSizer(wxHORIZONTAL);
	sizerTmp->Add(CreateSizerWithTextAndComboBox(this, NoticePage_Color_PMD, wxT("颜色"), &m_combobox_color_pmd), 0, wxGROW | wxALL, CONTROL_BORDER);
	m_btnAdd_pmd = new wxButton(this, NoticePage_BTN_PMD, wxT("发布"));
	sizerTmp->Add(m_btnAdd_pmd, 0, wxGROW | wxALL, CONTROL_BORDER);

	sizerBox_pmd->Add(sizerTmp, 0, wxGROW | wxALL, CONTROL_BORDER);

	sizerLeft->Add(sizerBox_pmd, 0, wxGROW | wxALL, CONTROL_BORDER);


	//布局-中
	wxSizer* sizerMiddle = new wxBoxSizer(wxVERTICAL);
	
	wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, wxT("自动循环公告"));
	wxSizer* sizerBox_zdxh = new wxStaticBoxSizer(box2, wxVERTICAL);

	sizerBox_zdxh->Add(CreateSizerWithTextAndLabels(this, wxT("间隔时间"), wxT("分钟"), wxID_ANY, &m_text_interval), 
			0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_zdxh->Add(CreateSizerWithTextAndLabels(this, wxT("循环时间"), wxT("分钟"), wxID_ANY, &m_text_loop), 
			0, wxGROW | wxALL, CONTROL_BORDER);

	wxSizer* sizerTmp1 = new wxBoxSizer(wxHORIZONTAL);
	sizerTmp1->Add(CreateSizerWithTextAndComboBox(this, NoticePage_BTN_ZDXH, wxT("颜色"), &m_combobox_color_pmd), 0, wxGROW | wxALL, CONTROL_BORDER);
	m_btnAdd_zdxh = new wxButton(this, NoticePage_Color_ZDXH, wxT("添加"));
	sizerTmp1->Add(m_btnAdd_zdxh, 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_zdxh->Add(sizerTmp1, 0, wxGROW | wxALL, CONTROL_BORDER);
	
	m_lctrl_zdxh = new MyListCtrl(this, NoticePage_List_ZDXH, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS);
	wxListItem itemCol;
	itemCol.SetText(wxT("公告内容"));
	m_lctrl_zdxh->InsertColumn(0, itemCol);

	itemCol.SetText(wxT("字体颜色"));
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_zdxh->InsertColumn(1, itemCol);

	itemCol.SetText(wxT("循环结束时间"));
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_zdxh->InsertColumn(2, itemCol);
	sizerBox_zdxh->Add(m_lctrl_zdxh, 1, wxGROW | wxALL, CONTROL_BORDER);

	sizerMiddle->Add(sizerBox_zdxh, 1, wxGROW | wxALL, CONTROL_BORDER);

	//布局-右
	wxSizer* sizerRight = new wxBoxSizer(wxVERTICAL);
	wxStaticBox *box3 = new wxStaticBox(this, wxID_ANY, wxT("右下角公告"));
	wxSizer* sizerBox_yxj = new wxStaticBoxSizer(box3, wxVERTICAL);
	
	sizerBox_yxj->Add(CreateSizerWithTextAndLabel(this, wxT("标题"), wxID_ANY, &m_text_title),
				0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_yxj->Add(CreateSizerWithTextAndLabel(this, wxT("连接"), wxID_ANY, &m_text_link),
				0, wxGROW | wxALL, CONTROL_BORDER);
	
	sizerBox_yxj->Add(CreateSizerWithTextAndLabel(this, wxT("公告内容"), wxID_ANY, &m_text_yxj),
				0, wxGROW | wxALL, CONTROL_BORDER);

	m_chkOnlinePrompt = CreateCheckBoxAndAddToSizer(this, sizerBox_yxj, wxT("上线提示"), NoticePage_OnlinePrompt);

	wxSizer* sizerTmp5 = new wxBoxSizer(wxHORIZONTAL);
	sizerTmp5->Add(CreateSizerWithTextAndLabel(this, wxT("年"), wxID_ANY, &m_text_year), 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerTmp5->Add(CreateSizerWithTextAndLabel(this, wxT("月"), wxID_ANY, &m_text_month), 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_yxj->Add(sizerTmp5, 0, wxGROW | wxALL, CONTROL_BORDER);

	wxSizer* sizerTmp6 = new wxBoxSizer(wxHORIZONTAL);
	sizerTmp6->Add(CreateSizerWithTextAndLabel(this, wxT("日"), wxID_ANY, &m_text_day), 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerTmp6->Add(CreateSizerWithTextAndLabel(this, wxT("小时"), wxID_ANY, &m_text_hour), 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_yxj->Add(sizerTmp6, 0, wxGROW | wxALL, CONTROL_BORDER);

	wxSizer* sizerTmp7 = new wxBoxSizer(wxHORIZONTAL);
	sizerTmp7->Add(CreateSizerWithTextAndLabel(this, wxT("分"), wxID_ANY, &m_text_min), 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerTmp7->Add(CreateSizerWithTextAndLabel(this, wxT("秒"), wxID_ANY, &m_text_sec), 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_yxj->Add(sizerTmp7, 0, wxGROW | wxALL, CONTROL_BORDER);

	m_lctrl_yxj = new MyListCtrl(this, NoticePage_List_YXJ, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS);
	itemCol.SetText(wxT("标题"));
	m_lctrl_yxj->InsertColumn(0, itemCol);

	itemCol.SetText(wxT("内容"));
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_yxj->InsertColumn(1, itemCol);

	itemCol.SetText(wxT("时间"));
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_yxj->InsertColumn(2, itemCol);

	itemCol.SetText(wxT("服务器"));
	itemCol.SetAlign(wxLIST_FORMAT_CENTER); 
	m_lctrl_yxj->InsertColumn(3, itemCol);

	itemCol.SetText(wxT("上线提示"));
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_yxj->InsertColumn(4, itemCol);
	sizerBox_yxj->Add(m_lctrl_yxj, 1, wxGROW | wxALL, CONTROL_BORDER);

	sizerRight->Add(sizerBox_yxj, 1, wxGROW | wxALL, CONTROL_BORDER);

	sizerAll->Add(sizerLeft, 0, wxGROW|wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerMiddle, 0, wxGROW|wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerRight, 1, wxGROW|wxALL, CONTROL_BORDER);

	SetSizer(sizerAll);
	sizerAll->Fit(this);
}