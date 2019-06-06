/////////////////////////////////////////////////////////////////////////////
// Name:        YxjPage.h
// Created:     2009-02-04
// Info:		右下角公告页面
/////////////////////////////////////////////////////////////////////////////

#ifndef YXJPAGE_H_
#define YXJPAGE_H_
#include "UserFrame.h"

class MyListCtrl;
class YxjPage : public wxPanel, public UserFrame
{
public:
	enum EYxjListCol
	{
		EYLC_NULL	= -1,
		EYLC_0		= 0,
		EYLC_1,
		EYLC_2,
		EYLC_3,
		EYLC_4,
		EYLC_5,
		EYLC_6,
	};
	struct tagYxj
	{
		DWORD						dwNetCrc;
		tstring						strTitle;
		tstring						strContent;
		tstring						strLink;		
		tstring						strYear;
		tstring						strMonth; 
		tstring						strDay;
		tstring						strHour; 
		tstring						strMin; 
		tstring						strSec; 
		bool						bLoginHint; 
		std::vector<tstring>		vecWorld;
		tagYxj()
		{
			
		}
	};

public:
	YxjPage(wxBookCtrlBase *book, const wxString& label);
	void ClearTextCtrl();

	const wxString& GetLabel() { return m_label; }

	void InsertShowItem(tstring szWorldName, tstring szTester, tstring szTitle, tstring szContent, tagDWORDTime dwStartTime);
	void ClearShowItem();
	void YxjOperationStatus(BOOL);

	void IssueYxjPage(wxCommandEvent& event);
	void CancelYxj(wxCommandEvent& event);

	void SetMyFrame(MyFrame* frame) { m_pMyFrame = frame; }

	//DWORD OnYxjPageEvent(tagUserEvent* pGameEvent);

private:
	// 控件
	wxString		m_label;
	wxTextCtrl*		m_text_title;
	wxTextCtrl*		m_text_link;
	wxTextCtrl*		m_text_year;
	wxTextCtrl*		m_text_month;
	wxTextCtrl*		m_text_day;
	wxTextCtrl*		m_text_hour;
	wxTextCtrl*		m_text_min;
	//wxTextCtrl*		m_text_sec;
	//wxCheckBox*     m_chkOnlinePrompt;
	wxTextCtrl*		m_text_yxj;					//右下角公告内容
	wxButton*		m_btnAdd_yxj;				//添加右下角公告
	wxButton*		m_btnAdd_cancelyxj;			//取消右下角公告
	MyListCtrl*     m_lctrl_yxj;				//右下角公告状态

	MyFrame*		m_pMyFrame;

	std::map<DWORD, tagYxj>		m_mapYxj;		//缓存

	DECLARE_EVENT_TABLE()
};



#endif /* YXJPAGE_H_ */