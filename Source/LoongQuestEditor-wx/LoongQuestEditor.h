/////////////////////////////////////////////////////////////////////////////
// Program:     loong任务编辑器
// Name:        LoongQuestEditor.h
// Created:     2008-11-03
/////////////////////////////////////////////////////////////////////////////

#ifndef LOONGQUESTEDITOR_H_
#define LOONGQUESTEDITOR_H_

class MyPageInfo;
struct tagQuestProto;

#pragma warning(disable:4800)

// ----------------------------------------------------------------------------
// 页面容器
// ----------------------------------------------------------------------------
class MyPage : public wxPanel
{
public:
	MyPage(wxBookCtrlBase *book);

	//更新数据
	virtual void Update(tagQuestProto *pQuestProto) = 0;
	//保存数据
	virtual void Save(tagQuestProto *pQuestProto) = 0;

	virtual INT16 GetQuestID() { return 0; }

	wxSizer *CreateSizerWithText(wxControl *control, wxWindowID id = wxID_ANY,wxTextCtrl **ppText = NULL, int style = wxHORIZONTAL, const wxSize& size = wxDefaultSize);
	wxSizer *CreateSizerWithTextAndLabel(const wxString& label, wxWindowID id = wxID_ANY,wxTextCtrl **ppText = NULL, int style = wxHORIZONTAL, const wxSize& size = wxDefaultSize);
	wxCheckBox *CreateCheckBoxAndAddToSizer(wxSizer *sizer,const wxString& label,wxWindowID id = wxID_ANY);

	// create an info text contorl
	wxTextCtrl *CreateInfoText(wxWindowID id = wxID_ANY);
	wxRadioButton *CreateInfoRadio(const wxString& label,int style = 0, wxWindowID id = wxID_ANY);
	// create a horz sizer holding a static text and this text control
	wxSizer *CreateTextWithLabelSizer(const wxString& label,wxTextCtrl *text,const wxString& label2 = wxEmptyString,wxTextCtrl *text2 = NULL,const wxString& label3 = wxEmptyString,wxTextCtrl *text3 = NULL);

	wxSizer *CreateTextWithLabelSizer(const wxString& label,wxTextCtrl *text,wxTextCtrl *text2 = NULL,wxTextCtrl *text3= NULL,const wxString& label4 = wxEmptyString,wxTextCtrl *text4  = NULL);

	wxSizer *CreateRadioSizer(wxRadioButton *radio,const wxString& label,wxTextCtrl *text);
public:
	static MyPageInfo *ms_myPages;
};

// ----------------------------------------------------------------------------
// 动态创建页面
// ----------------------------------------------------------------------------
class MyPageInfo
{
public:
	typedef MyPage *(*Constructor)(wxBookCtrlBase *book);

	MyPageInfo(Constructor ctor, const wxChar *label);

	Constructor GetCtor() const { return m_ctor; }
	const wxString& GetLabel() const { return m_label; }
	MyPageInfo *GetNext() const { return m_next; }
	void Setnext(MyPageInfo *next) { m_next = next; }

private:
	wxString m_label;
	Constructor m_ctor;
	MyPageInfo *m_next;
};

#define SET_TEXT_CTRL_VAL(ctrl,val)		\
	ctrl->Clear();			\
	*(ctrl) << pQuestProto->val;

#define SET_TEXT_CTRL_VAL_FLOAT(ctrl,val)		\
	ctrl->Clear();			\
	*(ctrl) << wxString::Format(_T("%6.4f"),pQuestProto->val);

#define SET_TEXT_CTRL_VAL_DWORD(ctrl,val)		\
	ctrl->Clear();			\
	*(ctrl) << wxString::Format(_T("%lu"),pQuestProto->val);

#define SET_TEXT_CTRL_TIME(ctrl,val)		\
	ctrl->Clear();			\
	*(ctrl) << val;

#define GET_TEXT_CTRL_VAL_INT(ctrl,val)		\
	pQuestProto->val = wxAtoi(ctrl->GetValue());

#define GET_TEXT_CTRL_VAL_DWORD(ctrl,val)		\
	 ctrl->GetValue().ToULong(&pQuestProto->val);

#define GET_TEXT_CTRL_VAL_ENUM(ctrl,val,type)		\
	pQuestProto->val = (type)wxAtoi(ctrl->GetValue());

#define GET_TEXT_CTRL_VAL_BOOL(ctrl,val)		\
	pQuestProto->val = (BOOL)ctrl->GetValue();

#define GET_TEXT_CTRL_VAL_FLOAT(ctrl,val,num)		\
	double num;		\
	ctrl->GetValue().ToDouble(&num);	\
	pQuestProto->val = (float)num;

#define GET_RADIO_CTRL_VAL(ctrl,val)		\
	pQuestProto->val = ctrl->GetSelection();

#endif /*LOONGQUESTEDITOR_H_*/