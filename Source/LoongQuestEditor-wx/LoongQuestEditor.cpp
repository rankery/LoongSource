/////////////////////////////////////////////////////////////////////////////
// Program:     loong任务编辑器
// Name:        LoongQuestEditor.cpp
// Created:     2008-11-03
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/wx.h"
#include "wx/panel.h"
#include "wx/treectrl.h"
#include "wx/app.h"
#include "wx/log.h"
#include "wx/frame.h"
#include "wx/menu.h"

#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/listbox.h"
#include "wx/statbox.h"
#include "wx/stattext.h"
#include "wx/textctrl.h"
#include "wx/msgdlg.h"

#include "wx/sysopt.h"
#include "wx/bookctrl.h"
#include "wx/sizer.h"
#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/textdlg.h"

#include "LoongQuestEditor.h"
#include "..\WorldDefine\Questdef.h"
#include "QuestMgr.h"

#include "addbookm.xpm"
#include "wxwin32x32.xpm"
#include "sample.xpm"

class QuestTreeCtrl;

// control ids
enum
{
	QuestEditor_QuestTree = 100,
	QuestEditor_Book,
	QuestEditor_Menu_New,
	QuestEditor_Menu_Save,
	QuestEditor_Menu_Save_All,
	QuestEditor_Menu_Set_Path,
	QuestEditor_Menu_Find_Quest,
	QuestEditor_Menu_Save_Accept_Quest_Relation,
	QuestEditor_Menu_Save_Complete_Quest_Relation,
	QuestEditor_Menu_About,
	QuestEditor_ItemMenu_Delete,
	QuestEditor_Find_Quest_Type,
};

//程序入口
class MyApp : public wxApp
{
public:
	// 初始化App
	virtual bool OnInit();
	~MyApp();
};

//程序框架
class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title);
	
	virtual ~MyFrame();
	
	//更新树形控件
	void UpdateTree();
	
	//更新book
	void UpdateBook(tagQuestProto *pQuestProto);

	//New Quest
	void NewQuest(wxCommandEvent& event);

	//Save Quest
	void SaveQuest(wxCommandEvent& event);

	//Save all quest
	void SaveAllQuest(wxCommandEvent& event);

	//Delete Quest
	void DeleteQuest(wxCommandEvent& event);

	//quit
	void OnExit(wxCommandEvent& event);
	
	void OnAbout(wxCommandEvent& event);

	void SetPath(wxCommandEvent& event);

	void FindQuest(wxCommandEvent& event);

	void SaveAcceptQuestRelation(wxCommandEvent& event);

	void SaveCompleteQuestRelation(wxCommandEvent& event);

	//读取配置文件
	void ReadConfigFile();
	//保存配置文件
	void WriteConfigFile();
	//设置配置文件
	void SetConfigFile();

	wxString m_strQuestFilePath;

private:
	wxPanel *m_panel;
	wxBookCtrlBase *m_book;
	QuestTreeCtrl *m_treeCtrl;
	DECLARE_EVENT_TABLE()
};

class FindQuestDialog : public wxDialog
{
public:
	FindQuestDialog(const wxString& title);
	void OnOk(wxCommandEvent& event); 
	void OnClose(wxCloseEvent& event);
	UINT16 GetQuestID() { return quest_id_; }
	bool   IsFindQuest() { return is_found_; }
private:
	wxComboBox *type_;  
	wxTextCtrl *content_;
	wxButton   *search_;
	UINT16      quest_id_;
	bool        is_found_;
	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(FindQuestDialog, wxDialog)
	EVT_BUTTON(wxID_OK, FindQuestDialog::OnOk)
	EVT_CLOSE(FindQuestDialog::OnClose)
END_EVENT_TABLE()

FindQuestDialog::FindQuestDialog(const wxString &title) : quest_id_(0), is_found_(false),
						wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(300,150))
{
	wxSizer *sizeAll = new wxBoxSizer(wxVERTICAL);
	
	//type
	type_ = new  wxComboBox(this, QuestEditor_Find_Quest_Type, wxEmptyString, 
						wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	type_->Append(wxT("任务名称"));
	type_->Append(wxT("任务ID"));
	//content
	content_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);

	//search button 
	search_ = new wxButton(this, wxID_OK, wxT("Search..."));

	sizeAll->Add(type_, 1, wxGROW|wxALL, 5);
	sizeAll->Add(content_, 1, wxGROW|wxALL, 5);
	sizeAll->Add(search_, 1, wxGROW|wxALL, 5);

	SetSizer(sizeAll);

}

void FindQuestDialog::OnOk(wxCommandEvent& event)
{
	//任务查找
	QuestMap& quest = sQuestMgr.GetQuestMap();
	int selection = type_->GetSelection();
	
	//任务名称
	if (selection == 0)
	{
		//得到任务名称
		wxString need_find_content = content_->GetValue();
		if (need_find_content.IsEmpty() == true)
		{
			is_found_ = false;
			wxMessageBox(wxT("任务名称为空"));
			//EndModal(wxID_CANCEL);
			return;
		}
		//遍历任务列表
		QuestMap::iterator it =  quest.begin();
		for (; it != quest.end(); ++it)
		{
			tagQuest* pQuest = it->second;
			if (pQuest == NULL)
				continue;
			if(pQuest->pQuestProto == NULL)
				continue;

			//得到任务名称
			wxString tmp = pQuest->pQuestProto->name;
			if(tmp.find(need_find_content) == -1)
			{
				//没找到,继续遍历
				continue;
			}

			//找到了
			quest_id_ = pQuest->pQuestProto->id;
			is_found_ = true;
			EndModal(wxID_OK);
			return;
		}

		//没找到
		wxMessageBox(wxT("没找到相应任务! T_T"));
	}
	else if (selection == 1)
	{
		UINT16 id;
		//任务ID
		id = wxAtoi(content_->GetValue());
		QuestMap::iterator it = quest.find(id);
		if ( it == quest.end() )
		{
			//没有找到
			is_found_ = false;
			wxMessageBox(wxT("任务ID不存在! T_T"));
			//EndModal(wxID_CANCEL);
			return;
		}
		else
		{
			//找到了
			quest_id_ = id;
			is_found_ = true;
			EndModal(wxID_OK);
			return;
		}
	}
}

void FindQuestDialog::OnClose(wxCloseEvent &event)
{
	EndModal(wxID_CANCEL);
	Destroy();
}

class QuestTreeItemData : public wxTreeItemData
{
public:
	QuestTreeItemData(const wxString& desc, tagQuest* pQuest) : m_desc(desc),m_pQuest(pQuest) { }
	wxString m_desc;
	tagQuest* m_pQuest;
};

class QuestTreeCtrl : public wxTreeCtrl
{
public:
	QuestTreeCtrl() { }
	QuestTreeCtrl(wxWindow *parent, const wxWindowID id) : wxTreeCtrl(parent, id)
	{

	}
	virtual ~QuestTreeCtrl() { }

	 void OnSelChanged(wxTreeEvent& event);
	 void OnItemMenu(wxTreeEvent& event);
	 void OnItemRClick(wxTreeEvent& event);

	 bool FindAndSelectItem(const wxTreeItemId& root, UINT16 quest_id);
private:
	DECLARE_DYNAMIC_CLASS(QuestTreeCtrl)
	DECLARE_EVENT_TABLE()
};

DECLARE_APP(MyApp)


WX_DEFINE_ARRAY_PTR(MyPage*, ArrayMyPage);

BEGIN_EVENT_TABLE(QuestTreeCtrl, wxTreeCtrl)
	EVT_TREE_SEL_CHANGED(QuestEditor_QuestTree, QuestTreeCtrl::OnSelChanged)
	EVT_TREE_ITEM_MENU(QuestEditor_QuestTree, QuestTreeCtrl::OnItemMenu)
	EVT_TREE_ITEM_RIGHT_CLICK(QuestEditor_QuestTree, QuestTreeCtrl::OnItemRClick)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(QuestEditor_Menu_New, MyFrame::NewQuest)
	EVT_MENU(QuestEditor_Menu_Save, MyFrame::SaveQuest)
	EVT_MENU(QuestEditor_Menu_Save_All, MyFrame::SaveAllQuest)
	EVT_MENU(QuestEditor_Menu_Set_Path, MyFrame::SetPath)
	EVT_MENU(QuestEditor_Menu_Find_Quest, MyFrame::FindQuest)
	EVT_MENU(QuestEditor_Menu_Save_Accept_Quest_Relation, MyFrame::SaveAcceptQuestRelation)
	EVT_MENU(QuestEditor_Menu_Save_Complete_Quest_Relation, MyFrame::SaveCompleteQuestRelation)
	EVT_MENU(wxID_EXIT, MyFrame::OnExit)
	EVT_MENU(QuestEditor_ItemMenu_Delete, MyFrame::DeleteQuest)
	EVT_MENU(QuestEditor_Menu_About, MyFrame::OnAbout)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)
// ----------------------------------------------------------------------------
// app class
// ----------------------------------------------------------------------------
bool MyApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	MyFrame* frame = new MyFrame(wxT("LoongQuestEditor"));
	if (frame->m_strQuestFilePath.IsEmpty())
	{
		return false;
	}
	frame->Show(true);

	return true;
}

MyApp::~MyApp()
{
}

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

MyFrame::MyFrame(const wxString& title) : 
			wxFrame(NULL, wxID_ANY, title,
				wxPoint(50,0), wxDefaultSize, wxDEFAULT_FRAME_STYLE |
				wxNO_FULL_REPAINT_ON_RESIZE |
				wxCLIP_CHILDREN |
				wxTAB_TRAVERSAL)
{
	// Give it an icon
	SetIcon(wxICON(sample));
	//创建Menu
	wxMenuBar *mbar = new wxMenuBar;
	wxMenu *menuQuest = new wxMenu;
	menuQuest->Append(QuestEditor_Menu_New, _T("&New Quest...\tCtrl-N"));
	menuQuest->Append(QuestEditor_Menu_Save, _T("&Save Quest...\tCtrl-S"));
	menuQuest->Append(QuestEditor_Menu_Save_All, _T("Save &All Quest...\t"));
	menuQuest->AppendSeparator();
	menuQuest->Append(QuestEditor_Menu_Find_Quest, _T("&Find Quest...\tCtrl-F"));
	menuQuest->AppendSeparator();
	menuQuest->Append(QuestEditor_Menu_Set_Path, _T("Set &Path...\tCtrl-P"));
	menuQuest->AppendSeparator();
	menuQuest->Append(QuestEditor_Menu_Save_Accept_Quest_Relation, _T("Save Accep&t Quest Relation...\tCtrl-T"));
	menuQuest->Append(QuestEditor_Menu_Save_Complete_Quest_Relation, _T("Save Complet&e Quest Relation...\tCtrl-E"));
	menuQuest->AppendSeparator();
	menuQuest->Append(QuestEditor_Menu_About, _T("&About..."));
	menuQuest->AppendSeparator();
	menuQuest->Append(wxID_EXIT, _T("&Quit...\tCtrl-Q"));
	

	mbar->Append(menuQuest, _T("&Quest"));
	SetMenuBar(mbar);

	m_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);

	wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

	int style = wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN;
	m_book = new wxBookCtrl(m_panel,QuestEditor_Book, wxDefaultPosition, wxDefaultSize, style);

	ArrayMyPage pages;
	wxArrayString labels;

	//读取配置文件
	ReadConfigFile();

	if (!m_strQuestFilePath.IsEmpty())
	{
		//初始化 QuestMgr
		new QuestMgr;

		sQuestMgr.LoadQuest(m_strQuestFilePath.c_str());
	}

	MyPageInfo *info = MyPage::ms_myPages;
	while (info)
	{
		MyPage *page = (*info->GetCtor())(m_book);
		pages.Add(page);

		labels.Add(info->GetLabel());
		info = info->GetNext();
	}

	m_book->AddPage(pages[1], labels[1], false);
	m_book->AddPage(pages[0], labels[0], false);
	m_book->AddPage(pages[2], labels[2], false);
	m_book->AddPage(pages[3], labels[3], false);

	wxSizer *sizerLeft = new wxBoxSizer(wxHORIZONTAL);


		m_treeCtrl = new QuestTreeCtrl(m_panel, QuestEditor_QuestTree);
		UpdateTree();

		sizerLeft->Add(m_treeCtrl,1,wxGROW|wxALL, 5);
		sizerLeft->SetMinSize(170,540);
		sizerTop->Add(sizerLeft,  1,  wxGROW | wxALL, 5);
		sizerTop->Add(m_book, 1, wxGROW | wxALL, 5);	
		//sizerTop->SetMinSize(800,800);
		m_panel->SetSizer(sizerTop);
		sizerTop->Fit(this);
		//sizerTop->SetSizeHints(this);
}

MyFrame::~MyFrame()
{
	//销毁QuestMgr
	if (sQuestMgr.getSingletonPtr() != NULL)
		delete sQuestMgr.getSingletonPtr();
}

void MyFrame::UpdateTree()
{
	//清空TreeCtrl
	m_treeCtrl->DeleteAllItems();

	//用QuestMgr的数据更新
	QuestMap& quest = sQuestMgr.GetQuestMap();
	QuestMap::iterator it = quest.begin();
	wxTreeItemId rootID = m_treeCtrl->AddRoot(wxT("Quest"),-1,-1, new QuestTreeItemData(wxT("Quest"), NULL));
	//插入子项
	list<wstring>& list_dir = sQuestMgr.GetQuestDir();
	// LxC -->
	bool has_list_dir = list_dir.size() > 0;
	// <--
	list<wstring>::iterator it_dir = list_dir.begin();
	for (; it_dir != list_dir.end(); ++it_dir)
	{
		m_treeCtrl->AppendItem(rootID, *it_dir, -1, -1,NULL );
	}

	for (; it != quest.end(); ++it)
	{
		tagQuest* pQuest = it->second;
		if (pQuest == NULL)
			continue;
		if(pQuest->pQuestProto == NULL)
			continue;
		
		wxTreeItemIdValue cookie;
		wxTreeItemId id = m_treeCtrl->GetFirstChild(rootID,cookie);
		// LxC -->
		if (!has_list_dir) {
			m_treeCtrl->AppendItem(rootID, it->second->file_name, -1, -1,new QuestTreeItemData(pQuest->pQuestProto->name, pQuest) );
			continue;
		}
		// <--
		while (id.IsOk())
		{
			wxString text = m_treeCtrl->GetItemText(id);
			if (text == it->second->file_dir.c_str())
			{
				m_treeCtrl->AppendItem(id, it->second->file_name, -1, -1,new QuestTreeItemData(pQuest->pQuestProto->name, pQuest) );
			}
			id = m_treeCtrl->GetNextChild(id, cookie);
		}
	}
}

void MyFrame::UpdateBook(tagQuestProto *pQuestProto)
{
	size_t count = m_book->GetPageCount();
	for (size_t i = 0; i < count; ++i)
	{
		MyPage *pPage = (MyPage*)m_book->GetPage(i);
		pPage->Update(pQuestProto);
	}
}

void MyFrame::NewQuest(wxCommandEvent& event)
{
	//查看是否有没有保存的，提示保存
	if (m_treeCtrl->GetSelection().IsOk() == true)
	{
		m_treeCtrl->Unselect();
	}

	sQuestMgr.m_pQuestTmp = new tagQuest;
	sQuestMgr.m_pQuestTmp->pQuestProto = new tagQuestProto;
	ZeroMemory(sQuestMgr.m_pQuestTmp->pQuestProto, sizeof(tagQuestProto));

	//清空四个Page
	size_t count = m_book->GetPageCount();
	for (size_t i = 0; i < count; ++i)
	{
		MyPage *pPage = (MyPage*)m_book->GetPage(i);
		pPage->Update(sQuestMgr.m_pQuestTmp->pQuestProto);
	}
	
}

void MyFrame::SaveQuest(wxCommandEvent& event)
{
	//保存当前编辑的任务文件
	wxTreeItemId item = m_treeCtrl->GetSelection();
	if (item.IsOk() == false && sQuestMgr.m_pQuestTmp == NULL)
		return;
	if (item.IsOk() == false && sQuestMgr.m_pQuestTmp != NULL)
	{
		//有新建任务，保存新建任务
		//得到任务ID
		MyPage *pPage = (MyPage*)m_book->GetPage(0);
		INT16 id = pPage->GetQuestID();
		//查看新建任务ID是否已存在
		if (sQuestMgr.IsExist(id) == TRUE)
		{
			//任务ID已经存在
			wxMessageBox(wxT("任务ID已存在,请重新更改ID"), wxT("Warning....."), wxOK | wxICON_EXCLAMATION, this);
			return;
		}

		//调用文件保存对话框
		wxFileDialog dialog(this,
			_T("保存任务文件"),
			wxEmptyString,
			_T("quest.xml"),
			_T("Quest files (*.xml)|*.xml"),
			wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

		dialog.SetFilterIndex(1);

		if (dialog.ShowModal() == wxID_OK)
		{
			//wxLogMessage(_T("%s, filter %d"),
				//dialog.GetPath().c_str(), dialog.GetFilterIndex());
			//插入任务集合中
			tagQuestProto *pQuestProto = sQuestMgr.m_pQuestTmp->pQuestProto;
			sQuestMgr.m_pQuestTmp->file_path = dialog.GetPath();
			sQuestMgr.m_pQuestTmp->file_name = dialog.GetFilename();
			wxString dir = dialog.GetDirectory();
			size_t pos = dir.rfind(_T("\\"));
			wxString tmp = dir.substr(pos + 1);
			sQuestMgr.m_pQuestTmp->file_dir = tmp.c_str();
			pQuestProto->id = id;
			sQuestMgr.AddQuestToMap();
			//更新TreeCtrl
			UpdateTree();
			size_t count = m_book->GetPageCount();
			for (size_t i = 0; i < count; ++i)
			{
				MyPage *pPage = (MyPage*)m_book->GetPage(i);
				pPage->Save(pQuestProto);
			}

			sQuestMgr.SaveQuest(id);
			return;
		}
		else
		 return;
	}

	QuestTreeItemData* data = (QuestTreeItemData*)m_treeCtrl->GetItemData(item);
	if(data == NULL)
		return;

	tagQuest *pQuest = data->m_pQuest;
	if(pQuest == NULL)
		return;
	if(pQuest->pQuestProto == NULL)
		return;

	size_t count = m_book->GetPageCount();
	for (size_t i = 0; i < count; ++i)
	{
		MyPage *pPage = (MyPage*)m_book->GetPage(i);
		pPage->Save(pQuest->pQuestProto);
	}
	
	sQuestMgr.SaveQuest(data->m_pQuest->pQuestProto->id);
}

void MyFrame::DeleteQuest(wxCommandEvent& event)
{
	wxTreeItemId itemID = m_treeCtrl->GetSelection();
	if (itemID.IsOk() == false)
		return;

	QuestTreeItemData* data = (QuestTreeItemData*)m_treeCtrl->GetItemData(itemID);
	if(data == NULL)
		return;

	sQuestMgr.DeleteQuest(data->m_pQuest->pQuestProto->id);

	m_treeCtrl->Delete(itemID);

	//如果没有Item了，则清空page
	if (m_treeCtrl->GetCount() == 1)
	{
		tagQuestProto questProto = {0};
		size_t count = m_book->GetPageCount();
		for (size_t i = 0; i < count; ++i)
		{
			MyPage *pPage = (MyPage*)m_book->GetPage(i);
			pPage->Update(&questProto);
		}
	}
	

}

void MyFrame::SaveAllQuest(wxCommandEvent& event)
{
	QuestMap& quest = sQuestMgr.GetQuestMap();
	QuestMap::iterator it = quest.begin();
	for (; it != quest.end(); ++it)
	{
		tagQuestProto* pQuestProto = it->second->pQuestProto;
		if (pQuestProto == NULL)
			continue;
		sQuestMgr.SaveQuest(pQuestProto->id);
	}
}

void MyFrame::SetPath(wxCommandEvent& event)
{
	SetConfigFile();
}

void MyFrame::FindQuest(wxCommandEvent& event)
{
	FindQuestDialog dialog(wxT("Search"));
	if (dialog.ShowModal() == wxID_OK)
	{
		if (dialog.IsFindQuest() == true)
		{
			//找到任务
			UINT16 id = dialog.GetQuestID();

			//在TreeCtrl选中
			wxTreeItemId root = m_treeCtrl->GetRootItem();
			
			m_treeCtrl->FindAndSelectItem(root,id);
		}
	}
}

void MyFrame::SaveAcceptQuestRelation(wxCommandEvent& event)
{
	sQuestMgr.SaveAcceptQuestRelation(m_strQuestFilePath.c_str());
}

void MyFrame::SaveCompleteQuestRelation(wxCommandEvent& event)
{
	sQuestMgr.SaveCompleteQuestRelation(m_strQuestFilePath.c_str());
	sQuestMgr.SaveQuestNPCRelation(m_strQuestFilePath.c_str());
}

void MyFrame::OnExit(wxCommandEvent& event)
{
	Close();
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageDialog dialog(this, _T("2009-12-4 修改by ~~~~~~文维奇~~~~~~"),
		_T("关于任务编辑器的那些事"), wxOK);

	dialog.ShowModal();
}

MyPageInfo* MyPage::ms_myPages = NULL;

MyPageInfo::MyPageInfo(Constructor ctor, const wxChar *label) : m_label(label)
{
	m_ctor = ctor;
	m_next = NULL;
	if (MyPage::ms_myPages)
	{
		MyPageInfo *node_prev = MyPage::ms_myPages;
		MyPageInfo *node_next;
		do 
		{
			node_next = node_prev->GetNext();
			if (node_next)
				node_prev = node_next;
			else
			{
				node_prev->Setnext(this);
				m_next = node_next;
			}
			node_prev = node_next;
		} while (node_next);
	}
	else
	{
		MyPage::ms_myPages = this;
	}
}

MyPage::MyPage(wxBookCtrlBase *book) : wxPanel(book, wxID_ANY, wxDefaultPosition, wxDefaultSize,
											   wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{

}

wxSizer *MyPage::CreateSizerWithText(wxControl *control, wxWindowID id /* = wxID_ANY */,wxTextCtrl **ppText /* = NULL */,int style /*= wxHORIZONTAL*/,const wxSize& size /*= wxDefaultSize*/)
{
	wxSizer *sizeRow = new wxBoxSizer(style);
	wxTextCtrl *text = new wxTextCtrl(this, id, wxEmptyString, wxDefaultPosition, size, wxTE_PROCESS_ENTER);

	sizeRow->Add(control, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	sizeRow->Add(text, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

	if(ppText)
		*ppText = text;

	return sizeRow;
}

wxSizer *MyPage::CreateSizerWithTextAndLabel(const wxString& label, wxWindowID id /* = wxID_ANY */,wxTextCtrl **ppText /* = NULL */,int style /*= wxHORIZONTAL*/, const wxSize& size /*= wxDefaultSize*/)
{
	return CreateSizerWithText(new wxStaticText(this, wxID_ANY,label),id, ppText, style, size);
}

wxCheckBox *MyPage::CreateCheckBoxAndAddToSizer(wxSizer *sizer,const wxString& label,wxWindowID id /* = wxID_ANY */)
{
	wxCheckBox *checkbox = new wxCheckBox(this, id, label);
	sizer->Add(checkbox, 0, wxLEFT | wxRIGHT, 5);
	sizer->Add(0, 2, 0, wxGROW); // spacer

	return checkbox;
}

wxTextCtrl *MyPage::CreateInfoText(wxWindowID id /*= wxID_ANY*/)
{
	static int s_maxWidth = 0;
	if ( !s_maxWidth )
	{
		// calc it once only
		GetTextExtent(_T("9999999"), &s_maxWidth, NULL);
	}

	wxTextCtrl *text = new wxTextCtrl(this, id, wxEmptyString,
		wxDefaultPosition,
		wxSize(s_maxWidth, wxDefaultCoord),
		wxTE_PROCESS_ENTER);
	return text;
}

wxSizer *MyPage::CreateTextWithLabelSizer(const wxString& label,wxTextCtrl *text,const wxString& label2 /* = wxEmptyString */,wxTextCtrl *text2 /* = NULL */,const wxString& label3 /* = wxEmptyString */,wxTextCtrl *text3 /* = NULL */)
{
	wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
	sizerRow->Add(new wxStaticText(this, wxID_ANY, label), 0,
		wxALIGN_CENTRE_VERTICAL | wxRIGHT, 5);
	sizerRow->Add(text, 1, wxALIGN_CENTRE_VERTICAL);
	if ( text2 )
	{
		sizerRow->Add(new wxStaticText(this, wxID_ANY, label2), 0,
			wxALIGN_CENTRE_VERTICAL | wxLEFT | wxRIGHT, 5);
		sizerRow->Add(text2, 1, wxALIGN_CENTRE_VERTICAL);
	}
	
	if ( text3 )
	{
		sizerRow->Add(new wxStaticText(this, wxID_ANY, label3), 0,
			wxALIGN_CENTRE_VERTICAL | wxLEFT | wxRIGHT, 5);
		sizerRow->Add(text3, 0, wxALIGN_CENTRE_VERTICAL);
	}
	return sizerRow;
}

wxSizer *MyPage::CreateTextWithLabelSizer(const wxString& label, wxTextCtrl *text, wxTextCtrl *text2 /*= NULL*/,wxTextCtrl *text3/*= NULL*/,const wxString& label4 /*= wxEmptyString*/,wxTextCtrl *text4 /*= NULL*/)
{
	wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
	sizerRow->Add(new wxStaticText(this, wxID_ANY, label), 0,
		wxALIGN_CENTRE_VERTICAL | wxRIGHT, 5);
	sizerRow->Add(text, 1, wxALIGN_CENTRE_VERTICAL);
	if ( text2 && text3 && text4 )
	{
		sizerRow->Add(5, 0, 0, wxGROW); // spacer
		sizerRow->Add(text2, 1,wxALIGN_CENTRE_VERTICAL);
		sizerRow->Add(5, 0, 0, wxGROW); // spacer
		sizerRow->Add(text3, 1, wxALIGN_CENTRE_VERTICAL);
		sizerRow->Add(5, 0, 0, wxGROW); // spacer
		sizerRow->Add(new wxStaticText(this, wxID_ANY, label4), 0,
			wxALIGN_CENTRE_VERTICAL | wxLEFT | wxRIGHT, 5);
		sizerRow->Add(text4, 1, wxALIGN_CENTRE_VERTICAL);
	}
	return sizerRow;
}

wxRadioButton *MyPage::CreateInfoRadio(const wxString& label, int style /*=0*/,wxWindowID id /*= wxID_ANY*/)
{
	wxRadioButton *radio = new wxRadioButton(this, id, label,
		wxDefaultPosition,
		wxDefaultSize,
		style);
	return radio;
}

wxSizer *MyPage::CreateRadioSizer(wxRadioButton *radio,const wxString& label,wxTextCtrl *text)
{
	wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
	sizerRow->Add(radio, 0,wxALIGN_CENTRE_VERTICAL);
	sizerRow->Add(new wxStaticText(this, wxID_ANY, label), 0,
		wxALIGN_CENTRE_VERTICAL | wxRIGHT, 5);
	sizerRow->Add(text, 1, wxALIGN_CENTRE_VERTICAL);

	return sizerRow;

}

IMPLEMENT_DYNAMIC_CLASS(QuestTreeCtrl, wxTreeCtrl)

void QuestTreeCtrl::OnSelChanged(wxTreeEvent& event)
{
	wxTreeItemId treeID = event.GetItem();
	if (treeID.IsOk() == false)
		return;

	//根节点不处理
	if (event.GetItem() != GetRootItem())
	{
		wxWindow *pParent = GetParent();
		MyFrame* pFrame = (MyFrame*)pParent->GetParent();
		QuestTreeItemData* pData = (QuestTreeItemData*)GetItemData(event.GetItem());
		if (pData == NULL)
			return;

		pFrame->UpdateBook(pData->m_pQuest->pQuestProto);
		//设置标题
		wxString title = _T("LoongQuestEditor    ") + pData->m_pQuest->file_path ;
		pFrame->SetTitle(title);
	}
}

void QuestTreeCtrl::OnItemMenu(wxTreeEvent& event)
{
	wxTreeItemId itemID = event.GetItem();
	if (itemID.IsOk() == false)
		return;
	if (event.GetItem() != GetRootItem())
	{
		QuestTreeItemData* pData = (QuestTreeItemData*)GetItemData(itemID);
		if (pData == NULL)
			return;
		wxMenu menu;
		menu.Append(QuestEditor_ItemMenu_Delete, _T("&Delete Quest"));
		PopupMenu(&menu,event.GetPoint());
		event.Skip();
	}
}

void QuestTreeCtrl::OnItemRClick(wxTreeEvent& event)
{
	event.Skip();
}

bool QuestTreeCtrl::FindAndSelectItem(const wxTreeItemId& root, UINT16 quest_id)
{
	//遍历TreeCtrl找到符合任务ID的子项
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(root, cookie);
	
	while ( child.IsOk() )
	{
		// depth first traversal
		if ( !FindAndSelectItem(child, quest_id) )
			return false;

		QuestTreeItemData* pData = (QuestTreeItemData*)GetItemData(child);
		if (pData != NULL)
		{
			if (pData->m_pQuest->pQuestProto->id == quest_id)
			{
				SelectItem(child);
				return false;
			}
		}
		child = GetNextChild(root, cookie);
	}

	return true;
}

void MyFrame::ReadConfigFile()
{
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL, path, MAX_PATH);
	//得到路径
	wstring strPtah = path;
	size_t pos = strPtah.rfind(_T('\\'));
	if(pos != wstring::npos)
	{
		wstring str = strPtah.substr(0, pos + 1);
		wstring strConfigPath = str + _T("config.xml");
		//打开配置文件
		string strFile = QuestMgr::ToAnsi(strConfigPath);
		TiXmlDocument doc(strFile.c_str());
		if (doc.LoadFile() == false) 
		{
			//配置文件不存在，请用户设置配置文件
			SetConfigFile();
		}
		
		TiXmlHandle hDoc(&doc);
		TiXmlHandle hRoot(0);
		TiXmlElement *pElem = hDoc.FirstChildElement("config").Element();
		if (!pElem) 
			return;
		hRoot = TiXmlHandle( pElem );
		TiXmlElement *pNode = hRoot.FirstChild().Element();
		if (pNode != NULL)
		{
			m_strQuestFilePath = wxString(pNode->GetText(), wxConvUTF8);
			pNode = pNode->NextSiblingElement();
		}
	}
}

void MyFrame::SetConfigFile()
{
	wxString dirHome;
	wxGetHomeDir(&dirHome);

	wxDirDialog dialog(this, _T("设置任务文件加载路径"), dirHome, wxDD_DEFAULT_STYLE);

	if (dialog.ShowModal() == wxID_OK)
	{
		m_strQuestFilePath = dialog.GetPath();
		//保存路径到xml
		TiXmlDocument doc;
		TiXmlDeclaration* doc_decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
		doc.LinkEndChild( doc_decl );

		TiXmlElement* root = new TiXmlElement( "config" );
		doc.LinkEndChild( root );
		{
			TiXmlElement *quest = new TiXmlElement("quest");
			quest->LinkEndChild(new TiXmlText(m_strQuestFilePath.mb_str(wxConvUTF8)));
			root->LinkEndChild(quest);
		}

		//得到当前路径
		TCHAR path[MAX_PATH];
		::GetModuleFileName(NULL, path, MAX_PATH);
		//得到路径
		wstring strPtah = path;
		size_t pos = strPtah.rfind(_T('\\'));
		if(pos != wstring::npos)
		{
			wstring str = strPtah.substr(0, pos + 1);
			wstring strConfigPath = str + _T("config.xml");
			string strFile = QuestMgr::ToAnsi(strConfigPath);
			doc.SaveFile( strFile.c_str() );	
		}
	}
	else
	{
		m_strQuestFilePath = wxEmptyString;
	}
}

void MyFrame::WriteConfigFile()
{

}