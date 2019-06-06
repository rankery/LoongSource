/////////////////////////////////////////////////////////////////////////////
// Program:     LoongGMTool
// Name:        MyListCtrl.cpp
// Created:     2009-02-09
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mylistctrl.h"
#include "wx/menu.h"

//¿Ø¼þID
enum 
{
	LIST_DELETE = wxID_DELETE,
};

BEGIN_EVENT_TABLE(MyListCtrl, wxListCtrl)
	EVT_MENU(LIST_DELETE, MyListCtrl::OnDelete)
	EVT_RIGHT_DOWN(MyListCtrl::OnRightClick)
END_EVENT_TABLE()

void MyListCtrl::ShowContentMenu(const wxPoint& pos)
{
	wxMenu menu;

	menu.Append(LIST_DELETE, g_StrTable[_T("Delete")]);

	PopupMenu(&menu, pos.x, pos.y);
}

void MyListCtrl::OnRightClick(wxMouseEvent& event)
{
	int flags;
	long subitem;
	long item_hit = HitTest(event.GetPosition(), flags, &subitem);
	long item_selected = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (flags == wxLIST_HITTEST_ONITEMLABEL && item_selected == item_hit)
	{
		ShowContentMenu(event.GetPosition());
	}	
}

void MyListCtrl::OnDelete(wxCommandEvent& WXUNUSED(event))
{
	long item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (item != -1)
	{
		DeleteItem(item);
	}
}



