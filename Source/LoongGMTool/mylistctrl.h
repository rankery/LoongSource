/////////////////////////////////////////////////////////////////////////////
// Program:     LoongGMTool
// Name:        MyListCtrl.h
// Created:     2009-02-09
/////////////////////////////////////////////////////////////////////////////

#ifndef MYLISTCTRL_H_
#define MYLISTCTRL_H_

#include "wx/listctrl.h"

class MyListCtrl : public wxListCtrl
{
public:
	MyListCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style) 
		: wxListCtrl(parent, id, pos, size, style)
	{

	}

	void OnRightClick(wxMouseEvent& event);
	void OnDelete(wxCommandEvent& event);
private:
	void ShowContentMenu(const wxPoint& pos);

	wxListItemAttr m_attr;
	DECLARE_NO_COPY_CLASS(MyListCtrl)
	DECLARE_EVENT_TABLE()
};


#endif /* MYLISTCTRL_H_ */