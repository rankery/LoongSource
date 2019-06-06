/////////////////////////////////////////////////////////////////////////////
// Program:     loong任务编辑器
// Name:        NoticePage.h
// Created:     2009-02-04
// Info:		公告页面
/////////////////////////////////////////////////////////////////////////////


#ifndef NOTICEPAGE_H_
#define NOTICEPAGE_H_

class MyListCtrl;

//----------------------------------------------------------------------------
// 公告
//----------------------------------------------------------------------------
class NoticePage : public wxPanel
{
public:
	NoticePage(wxBookCtrlBase *book, const wxString& label);
	const wxString& GetLabel() { return m_label; }

private:
	wxString		m_label;
	wxButton*		m_btnAdd_pmd;		  //添加跑马灯公告
	wxComboBox*		m_combobox_color_pmd; //跑马灯公告颜色
	wxTextCtrl*		m_text_pmd;			  //跑马灯公告内容
	wxButton*		m_btnAdd_zdxh;		  //添加自动循环公告
	wxComboBox*		m_combobox_color_zdxh; //自动循环公告颜色
	MyListCtrl*     m_lctrl_zdxh;		   //自动循环公告状态
	wxTextCtrl*		m_text_interval;
	wxTextCtrl*		m_text_loop;
	wxTextCtrl*		m_text_title;
	wxTextCtrl*		m_text_link;
	wxTextCtrl*		m_text_year;
	wxTextCtrl*		m_text_month;
	wxTextCtrl*		m_text_day;
	wxTextCtrl*		m_text_hour;
	wxTextCtrl*		m_text_min;
	wxTextCtrl*		m_text_sec;
	wxCheckBox*     m_chkOnlinePrompt;
	wxTextCtrl*		m_text_yxj;				//右下角公告内容
	wxButton*		m_btnAdd_yxj;			//添加右下角公告
	MyListCtrl*     m_lctrl_yxj;		   //右下角公告状态

	DECLARE_EVENT_TABLE()
};

#endif /* NOTICEPAGE_H_ */