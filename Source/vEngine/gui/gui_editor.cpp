//-----------------------------------------------------------------------------
//!\file gui_editor.cpp
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2007-12-31
//!
//!\brief 界面系统 编辑器
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "..\system\window_wrap.h"
#include "..\system\DiscIOMgr.h"
#include "..\variable\var_container.h"
#include "..\resource\resource.h"
#include "gui_event_define.h"

// 所有控件
#include "gui_static.h"
#include "gui_button.h"
#include "gui_pushbutton.h"
#include "gui_editbox.h"
#include "gui_listbox.h"
#include "gui_scrollbar.h"
#include "gui_combobox.h"
#include "gui_tree.h"
#include "gui_window.h"

#include "gui_editor.h"
#include "Commdlg.h"


namespace vEngine {
//-----------------------------------------------------------------------------
// construction
//-----------------------------------------------------------------------------
GUIEditor::GUIEditor():m_Trunk(this)
{
	tstring strName = g_strLocalPath + _T("\\ui\\newfile.xml");
	lstrcpy(m_szFileName, strName.c_str());
	m_dwActiveWndID = GT_INVALID;
	m_pActiveWnd = NULL;
	m_pEditor = NULL;
	m_bChanges = FALSE;
}

GUIEditor::~GUIEditor()
{
	this->Destroy();	
}

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIEditor::Init(GUISystem* pSystem, BOOL bInGame)
{
	m_pSystem = pSystem;
	m_bInGame = bInGame;
	
	this->CreateControls();
	this->SetActiveWnd(m_pSystem->GetDesktop());

	m_hWnd = (HWND)TObjRef<VarContainer>()->GetDword(_T("HWND"));
	m_bResize = FALSE;
	m_bInResize = FALSE;

	if( !m_bInGame )
	{
		m_pWindow->SetCaption(m_szFileName);
		// 读取默认文件
		this->Load(m_szFileName, true);
	}
	else
	{
		m_pEditor->SetInvisible(TRUE);
	}

	// 注册事件处理函数
	m_pSystem->RegisterEventHandler(_T("\\desktop"), m_Trunk.sfp1(&vEngine::GUIEditor::EventHandler));

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID GUIEditor::Destroy()
{
	while( !m_listUndoAction.empty() )
	{
		SAFE_DEL(m_listUndoAction.back());
		m_listUndoAction.pop_back();
	}
}



//-----------------------------------------------------------------------------
// Render
//-----------------------------------------------------------------------------
VOID GUIEditor::Render()
{
	if( m_pActiveWnd != m_pSystem->GetWnd(m_dwActiveWndID) )
		this->SetActiveWnd(m_pSystem->GetDesktop());

	if( !m_pEditor->IsInvisible() )
	{
		// Active边框
		FLOAT fColor = abs(255.0f*sinf((FLOAT)timeGetTime()/200.0f));
		DWORD dwColorTemp = ((DWORD)fColor) | (((DWORD)fColor) << 8);
		DWORD dwColor = 0xff000000 | dwColorTemp;
		tagGUIImage image;
		image.rc.SetLTWH(m_pActiveWnd->GetView(), m_pActiveWnd->GetSize());
		image.rc.left -= 2;
		image.rc.top -= 2;
		image.rc.right += 1;
		image.rc.bottom += 1;
		m_pSystem->GetRender()->Line(&image, dwColor, TRUE);
	}

	m_rcActiveIn.SetLTWH(m_pActiveWnd->GetView().x-5, m_pActiveWnd->GetView().y-5,5,5);
	m_rcActiveIn = m_rcActiveIn + m_pActiveWnd->GetSize();
}



//-----------------------------------------------------------------------------
// 外部通过此函数向XUI Editor系统传窗口消息
//-----------------------------------------------------------------------------
BOOL GUIEditor::OnWindowsMsg( DWORD dwMsg, DWORD dwParam1, DWORD dwParam2 )
{
	tagPoint pt;
	if( dwMsg >= WM_MOUSEFIRST && dwMsg <= WM_MOUSELAST )
	{
		pt.Set(LOWORD(dwParam2), HIWORD(dwParam2));
		m_pSystem->GetRender()->ClientToView(pt);
	}

	INT wmId = LOWORD(dwParam1), wmEvent = HIWORD(dwParam1);
	switch( dwMsg )
	{
	case WM_COMMAND:
		switch (wmId)	// 菜单命令
		{
		case IDM_FILE_NEW: Clear(); break;
		case IDM_FILE_OPEN: Open(); break;
		case IDM_FILE_ADD: Add(); break;
		case IDM_FILE_SAVE: Save(m_szFileName); break;
		case IDM_FILE_SAVEAS: SaveAs(); break;
		case IDM_FILE_EXIT: if( Clear() ) {DestroyWindow(m_hWnd);} break;

		case IDM_EDIT_UNDO: Undo(); break;
		case IDM_EDIT_REDO: Redo(); break;
		case IDM_EDIT_CUT: Copy(); Delete(); break;
		case IDM_EDIT_COPY: Copy(); break;
		case IDM_EDIT_PASTE: Paste(); break;
		case IDM_EDIT_DELETE: Delete(); break;

		case IDM_VIEW_ATTRIBUTEWINDOW: m_pEditor->SetInvisible(FALSE); break;
		case IDM_VIEW_ARRAGECONTROLS: while(m_pSystem->GetDesktop()->Align()); break;
		case IDM_HELP_ABOUT: MessageBox(m_hWnd, _T("VUI Editor Ver 2007\nBy ctcrst"), _T("About"), MB_OK); break;
		}
		break;
	case WM_MOUSEMOVE:	// 处理鼠标拖动控件大小
		m_bInResize = m_rcActiveIn.IsContain(pt);
		break;
	case WM_KEYDOWN:
		if( m_pSystem->GetActive() == m_pActiveWnd )
		{
			switch(dwParam1)
			{
			case VK_UP:		{ tagPoint pt(0, -1);	m_pActiveWnd->Move(pt); }	return TRUE;; // 方向按钮移动控件
			case VK_DOWN:	{ tagPoint pt(0, 1);	m_pActiveWnd->Move(pt); }	return TRUE;;		
			case VK_LEFT:	{ tagPoint pt(-1, 0);	m_pActiveWnd->Move(pt); }	return TRUE;;
			case VK_RIGHT:	{ tagPoint pt(1, 0);	m_pActiveWnd->Move(pt); }	return TRUE;;	
			case VK_DELETE:	Delete(); return TRUE;;
			}
		}
		break;
	case WM_CHAR:	// 快捷键
		switch( dwParam1 )
		{
		case ('s' & 0x1F):	Save(m_szFileName); return TRUE;
		case ('z' & 0x1F):	{ GetKeyState(VK_SHIFT) < 0 ? Redo() : Undo(); return TRUE; }
		case ('x' & 0x1F):	Copy(); Delete(); return TRUE;
		case ('c' & 0x1F):	Copy();	return TRUE;
		case ('v' & 0x1F):	Paste(); return TRUE;
		}

	}	// switch( dwMsg )

	return FALSE;
}



//-----------------------------------------------------------------------------
// 接受并处理按钮消息
//-----------------------------------------------------------------------------
BOOL GUIEditor::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pSystem->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Active:	// 被激活:
		{
			if( !m_pSystem->IsChildOf(m_pEditor, pWnd) && pWnd != m_pEditor )
				this->SetActiveWnd(pWnd);
		}
		break;

	case EGUIE_DragStart:
		if( m_pActiveWnd == pWnd && m_pActiveWnd != m_pSystem->GetDesktop() && !m_bInGame )
		{
			if( !m_pActiveWnd->IsMovable() && (pEvent->msg.dwParam1&MK_CONTROL) )
			{
				m_pActiveWnd->SetMovable(TRUE);// 按住Ctrl可以拖动不可移动的控件
				m_pActiveWnd->SetUserData(2,1);
			}
			if( !m_pActiveWnd->IsResizable() && (pEvent->msg.dwParam1&MK_CONTROL) )
			{
				m_pActiveWnd->SetResizable(TRUE);// 按住Ctrl可以调整不可调整大小的控件
				m_pActiveWnd->SetUserData(3,1);
			}
			m_DragElement.ClearThis();
			Record(&m_DragElement, pWnd);
		}
		break;

	case EGUIE_DragEnd:
		if( m_pActiveWnd == pWnd && pEvent->dwParam1 && !m_bInGame
			&& m_pActiveWnd != m_pSystem->GetDesktop() )	// 拖动成功
		{
			if( m_pActiveWnd->GetUserData(2) )
			{
				m_pActiveWnd->SetMovable(FALSE);
				m_pActiveWnd->SetUserData(2,0);
			}
			if( m_pActiveWnd->GetUserData(3) )
			{
				m_pActiveWnd->SetResizable(FALSE);
				m_pActiveWnd->SetUserData(3,0);
			}
			RecordAction(&m_DragElement, pWnd->GetFather()->GetFullName().c_str(), 
				pWnd->GetFullName().c_str());
		}
		break;

	case EGUIE_Drag:
		if( m_pActiveWnd == pWnd )
		{
			// 动态更新显示的控件逻辑坐标
			TCHAR mess[X_LONG_STRING];
			GUIListBox* pListBox = (GUIListBox*)m_pSystem->GetWnd(_T("\\desktop\\editor\\attrlist"));
			wsprintf(mess, _T("%d"), m_pActiveWnd->GetClient().x);
			pListBox->SetText(9, 1, mess);
			wsprintf(mess, _T("%d"), m_pActiveWnd->GetClient().y);
			pListBox->SetText(10, 1, mess);
		}
		break;

	case EGUIE_Resize:
		if( m_pActiveWnd == pWnd && !m_bInGame )
		{
			// 动态更新显示的控件大小
			TCHAR mess[X_LONG_STRING];
			GUIListBox* pListBox = (GUIListBox*)m_pSystem->GetWnd(_T("\\desktop\\editor\\attrlist"));
			wsprintf(mess, _T("%d"), m_pActiveWnd->GetSize().x);
			pListBox->SetText(11, 1, mess);
			wsprintf(mess, _T("%d"), m_pActiveWnd->GetSize().y);
			pListBox->SetText(12, 1, mess);
		}
		break;

	case EGUIE_ItemDblClick:	// 双击创建
		if( !m_bInGame )
		{
			GUIListBox* pList = (GUIListBox*)m_pSystem->GetWnd(_T("\\desktop\\editor\\classlist"));
			if( pWnd == pList )
			{
				XmlElement ele;
				ele.SetAttribute("Name", "new");
				INT nRow = pList->GetCurSelectedRow();
				ele.SetAttribute("ClassName", m_pUtil->UnicodeToUnicode8(pList->GetText(nRow, 0)));
				GUIWnd* pNew = m_pSystem->CreateWnd(m_pActiveWnd->GetFullName().c_str(), &ele);
				this->RecordAction(NULL, _T(""), pNew->GetFullName().c_str());
				this->RefreshControlsTree(pNew);	// 刷新控件树
			}
		}
		break;

	case EGUIE_ItemClick:
		{
			GUITree* pTree = (GUITree*)m_pSystem->GetWnd(_T("\\desktop\\editor\\tree"));
			if( pWnd == pTree )
			{
				tagGUITreeItem* pTreeItem = pTree->GetItem(pEvent->dwParam1);
				this->SetActiveWnd(m_pSystem->GetWnd(pTreeItem->strName.c_str()));
				m_pSystem->SetActive(m_pActiveWnd);
			}
			GUIListBox* pListBox = (GUIListBox*)m_pSystem->GetWnd(_T("\\desktop\\editor\\attrlist"));
			if( pWnd == pListBox )	// 记录控件当前属性窗口位置
			{
				m_pActiveWnd->SetUserData(0, pListBox->GetCurrentTextRow());
				m_pActiveWnd->SetUserData(1, pListBox->GetCurSelectedRow());
			}
		}
		break;

	case EGUIE_UserChangeListContext:	// 用户改变List内容
		if( !m_bInGame )
		{
			GUIListBox* pListBox = (GUIListBox*)m_pSystem->GetWnd(_T("\\desktop\\editor\\attrlist"));
			if( pListBox == m_pSystem->GetActive() )// 记录控件当前属性窗口位置
			{
				m_pActiveWnd->SetUserData(0, pListBox->GetCurrentTextRow());
				m_pActiveWnd->SetUserData(1, pListBox->GetCurSelectedRow());
			}
			Rebuild(m_pActiveWnd, pListBox->GetText(pEvent->dwParam1, 0), pListBox->GetText(pEvent->dwParam1, 1));
		}
		break;

	default: //(_T("%s : %d"), pWnd->GetName().c_str(), (INT)pEvent->eEvent);
		break;
	}
	return FALSE;
}



//-----------------------------------------------------------------------------
// 初始化的时候创建各种控件 
//-----------------------------------------------------------------------------
VOID GUIEditor::CreateControls()
{
	{
		XmlElement ele;
		ele.SetAttribute("Name", "editor");
		ele.SetAttribute("Movable", TRUE);
		ele.SetAttribute("AlignType", "InnerRight");
		ele.SetAttribute("CanMoveOutOfFather", TRUE);
		ele.SetAttribute("TopMost", TRUE);
		ele.SetAttribute("ClassName", "CGUIWindow");
		ele.SetAttribute("PicColor", 0x1);
		ele.SetAttribute("CloseButtonName", "closebutton");
		ele.SetAttribute("Size_x", 266);
		ele.SetAttribute("Size_y", 661);
		ele.SetAttribute("LogicPos_x", 740);
		ele.SetAttribute("LogicPos_y", 25);
		ele.SetAttribute("Font", "Arial");
		ele.SetAttribute("FontWidth", "0");
		ele.SetAttribute("FontHeight", "16");
		ele.SetAttribute("FontWeight", "400");
		m_pEditor = m_pSystem->CreateWnd(_T("\\desktop"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "caption");
		ele.SetAttribute("ClassName", "CGUIStatic");
		ele.SetAttribute("MoveFather", TRUE);
		ele.SetAttribute("Size_x", 250);
		ele.SetAttribute("Size_y", 16);
		ele.SetAttribute("PicColor", 0xff00008f);
		ele.SetAttribute("Text", "Editor");
		ele.SetAttribute("TextColor", 0xffffffff);
		ele.SetAttribute("TextRect_left", 3);
		ele.SetAttribute("TextRect_top", 1);
		ele.SetAttribute("TextRect_right", 250);
		ele.SetAttribute("TextRect_bottom", 16);
		ele.SetAttribute("Font", "Arial");
		ele.SetAttribute("FontWidth", "0");
		ele.SetAttribute("FontHeight", "16");
		ele.SetAttribute("FontWeight", "400");
		m_pSystem->CreateWnd(_T("\\desktop\\editor"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "closebutton");
		ele.SetAttribute("ClassName", "CGUIButton");
		ele.SetAttribute("PicColor", 0xff808080);
		ele.SetAttribute("PushPicColor", 0xff606060);
		ele.SetAttribute("MovePicColor", 0xffA0A0A0);
		ele.SetAttribute("Size_x", 16);
		ele.SetAttribute("Size_y", 16);
		ele.SetAttribute("LogicPos_x", 250);
		ele.SetAttribute("LogicPos_y", 0);
		ele.SetAttribute("Font", "arial");
		ele.SetAttribute("FontHeight", "16");
		ele.SetAttribute("FontWeight", "800");
		ele.SetAttribute("Text", "X");
		ele.SetAttribute("TextColor", 0xffffffff);
		ele.SetAttribute("TextRect_left", 3);
		ele.SetAttribute("TextRect_top", 0);
		ele.SetAttribute("TextRect_right", 16);
		ele.SetAttribute("TextRect_bottom", 16);
		m_pSystem->CreateWnd(_T("\\desktop\\editor"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "classlist");
		ele.SetAttribute("ClassName", "CGUIListBox");
		ele.SetAttribute("Size_x", 250);
		ele.SetAttribute("Size_y", 128);
		ele.SetAttribute("PicColor", 0xff404040);
		ele.SetAttribute("LogicPos_y", 17);
		ele.SetAttribute("RowLineColor", 0xff000000);
		ele.SetAttribute("TextRect_left", 4);
		ele.SetAttribute("TextRect_top", 1);
		ele.SetAttribute("TextRect_right", 246);
		ele.SetAttribute("TextRect_bottom", 126);
		ele.SetAttribute("Font", "Arial");
		ele.SetAttribute("FontWidth", "0");
		ele.SetAttribute("FontHeight", "16");
		ele.SetAttribute("FontWeight", "400");
		ele.SetAttribute("SelectShadeColor", 0);
		m_pSystem->CreateWnd(_T("\\desktop\\editor"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "scrollbar");
		ele.SetAttribute("ClassName", "CGUIScrollBar");
		ele.SetAttribute("Size_x", 16);
		ele.SetAttribute("Size_y", 96);
		ele.SetAttribute("PicColor", 0xff808080);
		ele.SetAttribute("LogicPos_x", 250);
		ele.SetAttribute("LogicPos_y", 16);
		ele.SetAttribute("TotalValue", 100);
		ele.SetAttribute("VisibleValue", 50);
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\classlist"), &ele);
		{
			XmlElement ele;
			ele.SetAttribute("Name", "button1");
			ele.SetAttribute("ClassName", "CGUIButton");
			ele.SetAttribute("PicColor", 0xff808080);
			ele.SetAttribute("PushPicColor", 0xff606060);
			ele.SetAttribute("MovePicColor", 0xffA0A0A0);
			ele.SetAttribute("Size_x", 16);
			ele.SetAttribute("Size_y", 16);
			ele.SetAttribute("LogicPos_y", -16);
			m_pSystem->CreateWnd(_T("\\desktop\\editor\\classlist\\scrollbar"), &ele);
			ele.SetAttribute("Name", "button2");
			ele.SetAttribute("LogicPos_y", 96);
			m_pSystem->CreateWnd(_T("\\desktop\\editor\\classlist\\scrollbar"), &ele);

			GUIListBox* pListBox = (GUIListBox*)m_pSystem->GetWnd(_T("\\desktop\\editor\\classlist"));
			pListBox->Clear();
			pListBox->SetColNum(2, 100);

			INT n=0; 
			std::list<tstring> list1 = m_pSystem->EnumControlType();
			std::list<tstring> list2 = m_pSystem->EnumControlDesc();
			std::list<tstring>::iterator it1, it2;
			for(it1=list1.begin(), it2=list2.begin(); it1!=list1.end(); it1++, it2++, n++)
			{
				pListBox->SetText(n,0,(*it1).c_str());
				pListBox->SetText(n,1,(*it2).c_str());
			}
		}
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "tree");
		ele.SetAttribute("ClassName", "CGUITree");
		ele.SetAttribute("Size_x", 250);
		ele.SetAttribute("Size_y", 192);
		ele.SetAttribute("PicColor", 0xff404040);
		ele.SetAttribute("LogicPos_y", 146);
		ele.SetAttribute("ShadeColor", 0xff000001);
		ele.SetAttribute("TextRect_left", 4);
		ele.SetAttribute("TextRect_top", 1);
		ele.SetAttribute("TextRect_right", 250 - 4);
		ele.SetAttribute("TextRect_bottom", 192 - 2);
		ele.SetAttribute("PlusPicFileName", "data\\ui\\system\\plus.bmp");
		ele.SetAttribute("MinusPicFileName", "data\\ui\\system\\minus.bmp");
		ele.SetAttribute("BagPicFileName", "data\\ui\\system\\bag.bmp");
		ele.SetAttribute("Font", "Arial");
		ele.SetAttribute("FontWidth", "0");
		ele.SetAttribute("FontHeight", "16");
		ele.SetAttribute("FontWeight", "400");
		m_pSystem->CreateWnd(_T("\\desktop\\editor"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "scrollbar");
		ele.SetAttribute("ClassName", "CGUIScrollBar");
		ele.SetAttribute("Size_x", 16);
		ele.SetAttribute("Size_y", 192-32);
		ele.SetAttribute("PicColor", 0xff808080);
		ele.SetAttribute("LogicPos_x", 250);
		ele.SetAttribute("LogicPos_y", 16);
		ele.SetAttribute("TotalValue", 100);
		ele.SetAttribute("VisibleValue", 50);
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\tree"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "button1");
		ele.SetAttribute("ClassName", "CGUIButton");
		ele.SetAttribute("PicColor", 0xff808080);
		ele.SetAttribute("PushPicColor", 0xff606060);
		ele.SetAttribute("MovePicColor", 0xffA0A0A0);
		ele.SetAttribute("Size_x", 16);
		ele.SetAttribute("Size_y", 16);
		ele.SetAttribute("LogicPos_y", -16);
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\tree\\scrollbar"), &ele);
		ele.SetAttribute("Name", "button2");
		ele.SetAttribute("LogicPos_y", 192-32);
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\tree\\scrollbar"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "fullname");
		ele.SetAttribute("ClassName", "CGUIEditBox");
		ele.SetAttribute("Size_x", 266);
		ele.SetAttribute("Size_y", 16);
		ele.SetAttribute("PicColor", 0xff404040);
		ele.SetAttribute("TextColor", 0xffffffff);
		ele.SetAttribute("LogicPos_y", 339);
		ele.SetAttribute("TextRect_left", 2);
		ele.SetAttribute("TextRect_top", 0);
		ele.SetAttribute("TextRect_right", 266 - 2);
		ele.SetAttribute("TextRect_bottom", 16);
		ele.SetAttribute("Font", "Arial");
		ele.SetAttribute("FontWidth", "0");
		ele.SetAttribute("FontHeight", "16");
		ele.SetAttribute("FontWeight", "400");
		ele.SetAttribute("ShadeColor", 0);
		ele.SetAttribute("ReadOnly", 1);
		m_pSystem->CreateWnd(_T("\\desktop\\editor"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "attrlist");
		ele.SetAttribute("ClassName", "CGUIListBox");
		ele.SetAttribute("Size_x", 250);
		ele.SetAttribute("Size_y", 304);
		ele.SetAttribute("PicColor", 0xff404040);
		ele.SetAttribute("LogicPos_y", 356);
		ele.SetAttribute("RowLineColor", 0xff000000);
		ele.SetAttribute("TextRect_left", 3);
		ele.SetAttribute("TextRect_top", 1);
		ele.SetAttribute("TextRect_right", 250 - 4);
		ele.SetAttribute("TextRect_bottom", 300 - 2);
		ele.SetAttribute("OpenFileDialog", "1");
		ele.SetAttribute("OpenColorDialog", "1");
		ele.SetAttribute("Font", "Arial");
		ele.SetAttribute("FontWidth", "0");
		ele.SetAttribute("FontHeight", "16");
		ele.SetAttribute("FontWeight", "400");
		m_pSystem->CreateWnd(_T("\\desktop\\editor"), &ele);

		GUIListBox* pListBox = (GUIListBox*)m_pSystem->GetWnd(_T("\\desktop\\editor\\attrlist"));
		pListBox->SetColNum(2, 112);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "editbox");
		ele.SetAttribute("ClassName", "CGUIEditBox");
		ele.SetAttribute("Size_x", 192);
		ele.SetAttribute("Size_y", 16);
		ele.SetAttribute("TextRect_left", 2);
		ele.SetAttribute("TextRect_top", 0);
		ele.SetAttribute("TextRect_right", 192 - 2);
		ele.SetAttribute("TextRect_bottom", 16);
		ele.SetAttribute("Font", "Arial");
		ele.SetAttribute("FontWidth", "0");
		ele.SetAttribute("FontHeight", "16");
		ele.SetAttribute("FontWeight", "400");
		ele.SetAttribute("ShadeColor", 0);
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\attrlist"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "combo");
		ele.SetAttribute("ClassName", "CGUIComboBox");
		ele.SetAttribute("Size_x", 192);
		ele.SetAttribute("Size_y", 21);
		ele.SetAttribute("AutoResize", 1);
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\attrlist"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "editbox");
		ele.SetAttribute("ReadOnly", 1);
		ele.SetAttribute("ClassName", "CGUIEditBox");
		ele.SetAttribute("Size_x", 192);
		ele.SetAttribute("Size_y", 21);
		ele.SetAttribute("TextRect_left", 2);
		ele.SetAttribute("TextRect_top", 0);
		ele.SetAttribute("TextRect_right", 192 - 2);
		ele.SetAttribute("TextRect_bottom", 21);
		ele.SetAttribute("Font", "Arial");
		ele.SetAttribute("FontWidth", "0");
		ele.SetAttribute("FontHeight", "16");
		ele.SetAttribute("FontWeight", "400");
		ele.SetAttribute("ShadeColor", 0);
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\attrlist\\combo"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "button");
		ele.SetAttribute("ClassName", "CGUIButton");
		ele.SetAttribute("PicColor", 0xff808080);
		ele.SetAttribute("PushPicColor", 0xff606060);
		ele.SetAttribute("MovePicColor", 0xffA0A0A0);
		ele.SetAttribute("Size_x", 21);
		ele.SetAttribute("Size_y", 21);
		ele.SetAttribute("LogicPos_x", 192);
		ele.SetAttribute("LogicPos_y", 0);
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\attrlist\\combo"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "listbox");
		ele.SetAttribute("CanMoveOutOfFather", 1);
		ele.SetAttribute("ClassName", "CGUIListBox");
		ele.SetAttribute("Size_x", 192);
		ele.SetAttribute("Size_y", 352);
		ele.SetAttribute("LogicPos_y", 21);
		ele.SetAttribute("PicColor", 0xff707070);
		ele.SetAttribute("MoveSelect", TRUE);
		ele.SetAttribute("RowLineColor", 0xff000000);
		ele.SetAttribute("SelectStickColor", 0xff0000ff);
		ele.SetAttribute("SelectStickActiveColor", 0xff00007f);
		ele.SetAttribute("SelectTextColor", 0xffffffff);
		ele.SetAttribute("TextRect_left", 3);
		ele.SetAttribute("TextRect_top", 1);
		ele.SetAttribute("TextRect_right", 250 - 4);
		ele.SetAttribute("TextRect_bottom", 300 - 2);
		ele.SetAttribute("Font", "Arial");
		ele.SetAttribute("FontWidth", "0");
		ele.SetAttribute("FontHeight", "16");
		ele.SetAttribute("FontWeight", "400");
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\attrlist\\combo"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "scrollbar");
		ele.SetAttribute("ClassName", "CGUIScrollBar");
		ele.SetAttribute("Size_x", 16);
		ele.SetAttribute("Size_y", 304-32);
		ele.SetAttribute("PicColor", 0xff808080);
		ele.SetAttribute("LogicPos_x", 250);
		ele.SetAttribute("LogicPos_y", 16);
		ele.SetAttribute("TotalValue", 100);
		ele.SetAttribute("VisibleValue", 50);
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\attrlist"), &ele);
	}
	{
		XmlElement ele;
		ele.SetAttribute("Name", "button1");
		ele.SetAttribute("ClassName", "CGUIButton");
		ele.SetAttribute("PicColor", 0xff808080);
		ele.SetAttribute("PushPicColor", 0xff606060);
		ele.SetAttribute("MovePicColor", 0xffA0A0A0);
		ele.SetAttribute("Size_x", 16);
		ele.SetAttribute("Size_y", 16);
		ele.SetAttribute("LogicPos_y", -16);
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\attrlist\\scrollbar"), &ele);
		ele.SetAttribute("Name", "button2");
		ele.SetAttribute("LogicPos_y", 304-32);
		m_pSystem->CreateWnd(_T("\\desktop\\editor\\attrlist\\scrollbar"), &ele);
	}

	GUITree* pTree = (GUITree*)m_pSystem->GetWnd(_T("\\desktop\\editor\\tree"));
	GUIWnd* pWnd = m_pSystem->GetWnd(_T("\\desktop"));
	this->AddControlsToTree(pTree, NULL, pWnd);

}


//-----------------------------------------------------------------------------
// 将pWnd以及其下所有控件名字加入Tree控件的指定Name下
//-----------------------------------------------------------------------------
VOID GUIEditor::AddControlsToTree(GUITree* pTree, LPCTSTR szName, GUIWnd* pWnd)
{
	if( m_pSystem->IsChildOf(m_pEditor, pWnd) || m_pEditor == pWnd )
		return;

	tstringstream ssShow;
	ssShow << pWnd->GetName() << _T("(") << pWnd->GetClassName() << _T(")");
	pTree->InsterItem(szName, pWnd->GetName().c_str(), ssShow.str().c_str());

	std::list<GUIWnd*> list = pWnd->GetAllChild();
	std::list<GUIWnd*>::iterator it;
	INT nIndex = 0;
	std::list<GUIWnd*>::iterator itMin;
	while( !list.empty() )
	{
		INT nIndexMin = INT_MAX;
		for(it=list.begin(); it!=list.end(); ++it)
		{
			INT nThisIndex = (INT)(*it)->GetUserData(5);
			if( nThisIndex <= nIndexMin )
			{
				nIndexMin = nThisIndex;
				itMin = it;
			}
		}

		AddControlsToTree(pTree, pWnd->GetFullName().c_str(), (*itMin));
		(*itMin)->SetUserData(5, (DWORD)nIndex++);
		list.erase(itMin);
	}
}


//-----------------------------------------------------------------------------
// 重构指定控件及其子控件
//-----------------------------------------------------------------------------
VOID GUIEditor::Rebuild(GUIWnd* pWnd, LPCTSTR szKey, LPCTSTR szValue)
{
	if( pWnd == m_pSystem->GetDesktop() )	// 不修改桌面的属性
		return;

	XmlElement eleOrigin, eleNew;
	this->Record(&eleOrigin, pWnd);
	this->Record(&eleNew, pWnd);

	std::vector<DWORD> vecUserData = pWnd->GetUserData();	// 保存用户变量
	tstring m_strFatherName = pWnd->GetFather()->GetFullName();
	m_pSystem->DestroyWnd(pWnd);

	// 修改变量
	std::string strKey(m_pUtil->UnicodeToUnicode8(szKey));
	std::string strValue(m_pUtil->UnicodeToUnicode8(szValue));
	eleNew.SetAttribute(strKey, strValue);

	m_pActiveWnd = m_pSystem->CreateWnd(m_strFatherName.c_str(), &eleNew);
	m_pActiveWnd->GetUserData() = vecUserData;	// 恢复用户变量

	// 记录undo数据
	this->RecordAction(&eleOrigin, m_strFatherName.c_str(), m_pActiveWnd->GetFullName().c_str());

	// 刷新属性编辑界面
	this->RefreshControlsTree(m_pActiveWnd);

}


//-----------------------------------------------------------------------------
// 记录指定控件及其子控件
//-----------------------------------------------------------------------------
BOOL GUIEditor::Record(XmlElement* pXmlElement, GUIWnd* pWnd)
{
	if( m_bInGame )
		return FALSE;

	if( pWnd == m_pEditor )
		return FALSE;

	if( pWnd == m_pSystem->GetDesktop() )
	{
		pXmlElement->SetValue("desktop");
	}
	else
	{
		pXmlElement->SetValue("UIControl");
		pWnd->GetVar()->Save(pXmlElement, TRUE);
	}

	std::list<GUIWnd*>& listWnd = pWnd->GetAllChild();
	std::list<GUIWnd*>::iterator it;
	for(it=listWnd.begin(); it!=listWnd.end(); ++it)
	{
		XmlElement* pNewXmlElement = new XmlElement;
		if( Record(pNewXmlElement, (*it)) )
			pXmlElement->LinkEndChild(pNewXmlElement);
		else
			delete(pNewXmlElement);
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// 设置当前激活控件
//-----------------------------------------------------------------------------
VOID GUIEditor::SetActiveWnd(GUIWnd* pWnd)
{
	m_pActiveWnd = pWnd;
	m_dwActiveWndID = pWnd->GetID();

	// 定位控件树状图
	DWORD dwID = m_pUtil->Crc32(pWnd->GetFullName().c_str());
	GUITree* pTree = (GUITree*)m_pSystem->GetWnd(_T("\\desktop\\editor\\tree"));
	if( pTree->GetCurrentSelItem() != dwID )
		pTree->SetCurrentSelItem(dwID, FALSE);

	pWnd->FlipToTop();


	// 显示控件全名
	GUIEditBox* pNameBox = (GUIEditBox*)m_pSystem->GetWnd(_T("\\desktop\\editor\\fullname"));
	tstring str = m_pActiveWnd->GetFullName();
	INT nFind = 0;
	while( (nFind = str.find(_T("\\"), nFind)) != str.npos )	// 先将"\"换成"\r"
		str.replace(nFind, 1, _T("\r"));
	nFind = 0;
	while( (nFind = str.find(_T("\r"), nFind)) != str.npos )	// 再将"\r"换成两个"\"
		str.replace(nFind, 1, _T("\\\\"));
	pNameBox->SetText(str.c_str());

	// 显示控件属性
	GUIListBox* pListBox = (GUIListBox*)m_pSystem->GetWnd(_T("\\desktop\\editor\\attrlist"));
	pListBox->Clear();

	XmlElement ele;
	m_pActiveWnd->GetVar()->Save(&ele);

	XmlAttribute* pAttrib = ele.FirstAttribute();
	for(INT n=0; pAttrib; n++)
	{
		pListBox->SetText(n, 0, m_pUtil->Unicode8ToUnicode(pAttrib->Name()));
		pListBox->SetItemEditable(n, 0, FALSE);
		pListBox->SetText(n, 1, m_pUtil->Unicode8ToUnicode(pAttrib->Value()));
		pListBox->SetItemEditable(n, 1, true);

		std::vector<tstring>* pVec = m_pActiveWnd->GetVar()->GetEnumText
			(m_pUtil->Unicode8ToUnicode(pAttrib->Name()));
		if( P_VALID(pVec) && !pVec->empty() )
		{
			std::list<tstring> list;
			for(INT m=0; m<(INT)pVec->size(); m++)
				list.push_back((*pVec)[m]);

			pListBox->SetItemOptionText(n, 1, list);
		}

		pAttrib = pAttrib->Next();
	}

	// 设置属性窗口的当前显示位置
	pListBox->SetCurrentTextRow(m_pActiveWnd->GetUserData(0));
	pListBox->SetCurSelectedRow(m_pActiveWnd->GetUserData(1));
}


//-----------------------------------------------------------------------------
// 刷新控件树,同时设置当前激活控件
//-----------------------------------------------------------------------------
VOID GUIEditor::RefreshControlsTree(GUIWnd* pActiveWnd)
{
	// 刷新控件树
	GUITree* pTree = (GUITree*)m_pSystem->GetWnd(_T("\\desktop\\editor\\tree"));
	pTree->RemoveItem(_T("\\desktop"));
	this->AddControlsToTree(pTree, NULL, m_pSystem->GetWnd(_T("\\desktop")));

	// 是否需要刷新系统当前Active控件
	GUIWnd* pSystemActive = m_pSystem->GetActive();
	if( !m_pSystem->IsChildOf(m_pEditor, pSystemActive) && pSystemActive != m_pEditor )
		m_pSystem->SetActive(pActiveWnd);

	// 刷新Editor当前active控件
	SetActiveWnd(pActiveWnd);
}


//-----------------------------------------------------------------------------
// 保存到磁盘
//-----------------------------------------------------------------------------
BOOL GUIEditor::Save(LPCTSTR szFileName)
{
	XmlElement ele;
	this->Record(&ele, m_pSystem->GetDesktop());
	XmlDocument doc;
	XmlDeclaration declaration("1.0", "UTF-8", "");
	doc.InsertEndChild(declaration);
	doc.InsertEndChild(ele);
	doc.SaveFile(szFileName);
	m_bChanges = FALSE;		// 是否需要存盘
	return TRUE;
}


//-----------------------------------------------------------------------------
// 从磁盘读取
//-----------------------------------------------------------------------------
BOOL GUIEditor::Load(LPCTSTR szFileName, bool bClear)
{
	XmlDocument doc;
	if( !doc.LoadFile(NULL, szFileName) )
		return FALSE;

	XmlHandle docHandle( &doc );
	XmlHandle eleHandle = docHandle.FirstChildElement("desktop").FirstChildElement();
	XmlElement* pElement = eleHandle.Element();
	if( !pElement )	// 是否符合CGUI存盘格式
		return FALSE;

	if( bClear )
	{
		if( !Clear() )
			return FALSE;
		
		_tcsncpy(m_szFileName, szFileName, MAX_PATH);
	}

	while( pElement )
	{
		m_pSystem->CreateWnd(_T("\\desktop"), pElement);
		pElement = pElement->NextSiblingElement();
	}

	// 刷新控件树
	this->RefreshControlsTree(m_pSystem->GetDesktop());
	return TRUE;
}


//-----------------------------------------------------------------------------
// 清除所有控件
//-----------------------------------------------------------------------------
BOOL GUIEditor::Clear()
{
	if( m_bChanges && !m_bInGame )
	{
		tstringstream stream;
		stream << _T("Save Changes to ") << m_szFileName << " ?";
		switch( ::MessageBox(m_hWnd, stream.str().c_str(), _T("warning"), MB_ICONWARNING|MB_YESNOCANCEL))
		{
		case IDYES:
			this->Save(m_szFileName);
			break;
		case IDNO: 
			break;
		case IDCANCEL: 
			return FALSE;
		}
	}

	std::list<GUIWnd*> listWnd = m_pSystem->GetDesktop()->GetAllChild();
	std::list<GUIWnd*>::iterator it;
	for(it=listWnd.begin(); it!=listWnd.end(); ++it)
	{
		if( *it != m_pEditor )
			m_pSystem->DestroyWnd(*it);
	}
	// 刷新控件树
	this->RefreshControlsTree(m_pSystem->GetDesktop());

	// 清空undo/redo列表
	while( !m_listUndoAction.empty() )
	{
		SAFE_DEL(m_listUndoAction.back());
		m_listUndoAction.pop_back();
	}
	while( !m_listRedoAction.empty() )
	{
		SAFE_DEL(m_listRedoAction.back());
		m_listRedoAction.pop_back();
	}
	m_DragElement.ClearThis();

	m_bChanges = FALSE;
	return TRUE;
}


//-----------------------------------------------------------------------------
// 记录动作行为
//-----------------------------------------------------------------------------
VOID GUIEditor::RecordAction(XmlElement* pOriginEle, LPCTSTR szFather, LPCTSTR szNew)
{
	if( m_bInGame )
		return;

	tagGUIEditorAction* pAction = new tagGUIEditorAction;
	pAction->strFather = szFather;
	pAction->strNew = szNew;
	if( pOriginEle )
		pAction->OriginEle = *pOriginEle;

	m_listUndoAction.push_back(pAction);

	// 清空redo列表
	while( !m_listRedoAction.empty() )
	{
		SAFE_DEL(m_listRedoAction.back());
		m_listRedoAction.pop_back();
	}

	m_bChanges = TRUE;		// 是否需要存盘
}


//-----------------------------------------------------------------------------
// undo行为
//-----------------------------------------------------------------------------
VOID GUIEditor::Undo()
{
	if( m_listUndoAction.empty() )
		return;

	XmlElement ele;
	tstring strFather;

	tagGUIEditorAction* pAtcion = m_listUndoAction.back();
	GUIWnd* pWnd = m_pSystem->GetWnd(pAtcion->strNew.c_str());
	if( P_VALID(pWnd) )	// 是否需要删除新窗口
	{
		strFather = pWnd->GetFather()->GetFullName();
		this->Record(&ele, pWnd);	// 记录以便redo
		m_pSystem->DestroyWnd(pWnd);	
	}

	GUIWnd* pNewWnd = NULL;
	if( !pAtcion->strFather.empty() )	// 是否需要恢复老窗口
	{
		pNewWnd = m_pSystem->CreateWnd(pAtcion->strFather.c_str(), &pAtcion->OriginEle);
		pAtcion->strNew = pNewWnd->GetFullName();	// 记录以便redo
		this->RefreshControlsTree(pNewWnd);
	}
	else
		this->RefreshControlsTree(m_pSystem->GetDesktop());

	m_listUndoAction.pop_back();
	m_listRedoAction.push_back(pAtcion);	// 记录redo
	pAtcion->OriginEle = ele;				// 记录以便redo
	pAtcion->strFather = strFather;			// 记录以便redo

	if( m_listUndoAction.empty() )
		m_bChanges = FALSE;	// 是否需要存盘
}


//-----------------------------------------------------------------------------
// redo行为
//-----------------------------------------------------------------------------
VOID GUIEditor::Redo()
{
	if( m_listRedoAction.empty() )
		return;

	XmlElement ele;
	tstring strFather;

	tagGUIEditorAction* pAtcion = m_listRedoAction.back();
	GUIWnd* pWnd = m_pSystem->GetWnd(pAtcion->strNew.c_str());
	if( P_VALID(pWnd) )	// 是否需要删除新窗口
	{
		strFather = pWnd->GetFather()->GetFullName();
		this->Record(&ele, pWnd);	// 记录以便undo
		m_pSystem->DestroyWnd(pWnd);	
	}

	GUIWnd* pNewWnd = NULL;
	if( !pAtcion->strFather.empty() )	// 是否需要恢复老窗口
	{
		pNewWnd = m_pSystem->CreateWnd(pAtcion->strFather.c_str(), &pAtcion->OriginEle);
		pAtcion->strNew = pNewWnd->GetFullName();	// 记录以便redo
		this->RefreshControlsTree(pNewWnd);
	}
	else
		this->RefreshControlsTree(m_pSystem->GetDesktop());

	m_listRedoAction.pop_back();
	m_listUndoAction.push_back(pAtcion);	// 记录undo
	pAtcion->OriginEle = ele;				// 记录以便undo
	pAtcion->strFather = strFather;			// 记录以便undo
}


//-----------------------------------------------------------------------------
// copy
//-----------------------------------------------------------------------------
VOID GUIEditor::Copy()
{
	if( m_pActiveWnd == m_pSystem->GetActive() 
		&& m_pActiveWnd != m_pSystem->GetDesktop() )
	{
		m_CopyElement.ClearThis();
		this->Record(&m_CopyElement, m_pActiveWnd);
	}
}


//-----------------------------------------------------------------------------
// paste
//-----------------------------------------------------------------------------
VOID GUIEditor::Paste()
{
	if( m_pActiveWnd == m_pSystem->GetActive() )
	{
		GUIWnd* pWnd = m_pSystem->CreateWnd(m_pActiveWnd->GetFullName().c_str(), &m_CopyElement);
		if( P_VALID(pWnd) )
		{
			this->RecordAction(NULL, _T(""), pWnd->GetFullName().c_str());
			this->RefreshControlsTree(m_pActiveWnd);	// 刷新控件树
		}
	}
}


//-----------------------------------------------------------------------------
// delete
//-----------------------------------------------------------------------------
VOID GUIEditor::Delete()
{
	if( m_pActiveWnd == m_pSystem->GetActive() 
		&& m_pActiveWnd != m_pSystem->GetDesktop() )
	{
		XmlElement OriginEle;
		this->Record(&OriginEle, m_pActiveWnd);
		this->RecordAction(&OriginEle, m_pActiveWnd->GetFather()->GetFullName().c_str(), _T(""));
		m_pSystem->DestroyWnd(m_pActiveWnd);
		this->RefreshControlsTree(m_pSystem->GetDesktop());	// 刷新控件树
	}
}


//-----------------------------------------------------------------------------
// open
//-----------------------------------------------------------------------------
VOID GUIEditor::Open()
{
	TCHAR szFileName[MAX_PATH];
	_tcsncpy(szFileName, m_szFileName, MAX_PATH);
	OPENFILENAME file;
	ZeroMemory(&file, sizeof(file));
	file.lStructSize = sizeof(file);
	file.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	file.hwndOwner = m_hWnd;
	file.lpstrFilter = _T("vEngine GUI files\0*.xml\0\0");
	file.lpstrFile = szFileName;
	file.nMaxFile = MAX_PATH;
	if( ::GetOpenFileName(&file) )
	{
		SetDefaultDir();
		if(	Load(file.lpstrFile, true) )
			m_pWindow->SetCaption(m_szFileName);
	}
}


//-----------------------------------------------------------------------------
// add
//-----------------------------------------------------------------------------
VOID GUIEditor::Add()
{
	TCHAR szFileName[MAX_PATH];
	_tcsncpy(szFileName, m_szFileName, MAX_PATH);
	OPENFILENAME file;
	ZeroMemory(&file, sizeof(file));
	file.lStructSize = sizeof(file);
	file.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	file.hwndOwner = m_hWnd;
	file.lpstrFilter = _T("vEngine GUI files\0*.xml\0\0");
	file.lpstrFile = szFileName;
	file.nMaxFile = MAX_PATH;
	if( ::GetOpenFileName(&file) )
	{
		SetDefaultDir();
		Load(file.lpstrFile, false);
	}
}


//-----------------------------------------------------------------------------
// save as
//-----------------------------------------------------------------------------
VOID GUIEditor::SaveAs()
{
	OPENFILENAME file;
	ZeroMemory(&file, sizeof(file));
	file.lStructSize = sizeof(file);
	file.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY;
	file.hwndOwner = m_hWnd;
	file.lpstrFilter = _T("vEngine GUI files\0*.xml\0\0");
	file.lpstrFile = m_szFileName;
	file.nMaxFile = MAX_PATH;
	if( ::GetSaveFileName(&file) )
	{
		SetDefaultDir();
		if( Save(file.lpstrFile) )
			m_pWindow->SetCaption(m_szFileName);
	}
}


//-----------------------------------------------------------------------------
// 显示开关
//-----------------------------------------------------------------------------
DWORD GUIEditor::Switch(DWORD dwID, BOOL bDown)
{
	if( bDown )
	{
		m_pEditor->SetInvisible(!m_pEditor->IsInvisible()); 
		this->RefreshControlsTree(m_pSystem->GetDesktop());
	}
	return 0;
}
}