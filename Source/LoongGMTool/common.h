#ifndef _COMMON_H_
#define _COMMON_H_

//控件边界
#define CONTROL_BORDER 5

//创建控件辅助函数
static wxSizer *CreateSizerWithText(wxWindow *parent, wxControl *control, wxWindowID id = wxID_ANY,wxTextCtrl **ppText = NULL, 
								bool bPwd = false, wxTextValidator* pVal=NULL, int style = wxHORIZONTAL, const wxSize& size = wxDefaultSize, wxControl *control2 = NULL)
{
	wxSizer *sizeRow = new wxBoxSizer(style);
	wxTextCtrl *text = NULL;
	if (bPwd == true)
	{
		//密码
		text = new wxTextCtrl(parent, id, wxEmptyString, wxDefaultPosition, size, wxTE_PASSWORD);
	}
	else
	{
		if( P_VALID(pVal) )
			text = new wxTextCtrl(parent, id, wxEmptyString, wxDefaultPosition, size, 0, *pVal);
		else
			text = new wxTextCtrl(parent, id, wxEmptyString, wxDefaultPosition, size, wxTE_PROCESS_ENTER);
	}
	if (control != NULL)
	{
		sizeRow->Add(control, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	}

	sizeRow->Add(text, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

	if (control2 != NULL)
	{
		sizeRow->Add(control2, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	}

	if(ppText)
		*ppText = text;

	return sizeRow;
}

static wxSizer *CreateSizerWithTextAndLabel(wxWindow *parent, const wxString& label, wxWindowID id = wxID_ANY,wxTextCtrl **ppText = NULL, 
									 bool bPwd = false, wxTextValidator* pVal=NULL, int style = wxHORIZONTAL, const wxSize& size = wxDefaultSize)
{
	return CreateSizerWithText(parent, new wxStaticText(parent, wxID_ANY,label),id, ppText, bPwd, pVal, style, size);
}

static wxSizer *CreateSizerWithTextAndLabels(wxWindow *parent, const wxString& label, const wxString& label2, wxWindowID id = wxID_ANY,wxTextCtrl **ppText = NULL, 
									 bool bPwd = false, wxTextValidator* pVal=NULL, int style = wxHORIZONTAL, const wxSize& size = wxDefaultSize)
{
	return CreateSizerWithText(parent, new wxStaticText(parent, wxID_ANY,label),id, ppText, bPwd, pVal, style, size, new wxStaticText(parent, wxID_ANY,label2));
}

static wxCheckBox* CreateCheckBoxAndAddToSizer(wxWindow *parent, wxSizer *sizer, const wxString& label, wxWindowID id)
{
	wxCheckBox *checkbox = new wxCheckBox(parent, id, label);
	sizer->Add(checkbox, 0, wxLEFT | wxRIGHT, 5);
	sizer->Add(0, 2, 0, wxGROW); // spacer

	return checkbox;
}
// create a sizer containing a button and a text ctrl
static wxSizer* CreateSizerWithTextAndButton(wxWindow *parent, wxWindowID idBtn, const wxString& label, wxWindowID id, wxTextCtrl **ppText, wxTextValidator* pVal=NULL)
{
	return CreateSizerWithText(parent, new wxButton(parent, idBtn, label), id, ppText, false, pVal);
}

//
static wxSizer* CreateSizerWithTextAndComboBox(wxWindow *parent, wxWindowID idComboBox, const wxString& label, wxComboBox **ppComboBox, int n=0, const wxString choices[] = NULL)
{
	wxSizer* sizeRow = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* staticText = new wxStaticText(parent, wxID_ANY, label);
	*ppComboBox = new wxComboBox(parent, idComboBox, wxEmptyString, wxDefaultPosition, wxDefaultSize, n, choices, 0);
	sizeRow->Add(staticText, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	sizeRow->Add(*ppComboBox, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	return sizeRow;
}

#endif /* _COMMON_H_ */