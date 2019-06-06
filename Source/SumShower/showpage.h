/////////////////////////////////////////////////////////////////////////////
// Program:     RTClient
// Name:        ShowPage.h
// Created:     2009-03-20
// Info:		开启和关闭服务器
/////////////////////////////////////////////////////////////////////////////

#ifndef OPENAANDCLOSESERVERPAGE_H_
#define OPENAANDCLOSESERVERPAGE_H_

#include "PageAssist.h"

//class MyButtonCustomRenderer :public wxDataViewCustomRenderer
//{
//public:
//	MyButtonCustomRenderer(wxDataViewCellMode mode, int alignment) : 
//	  wxDataViewCustomRenderer(wxString("void*"), mode, alignment)
//	{
//		name_ = wxT("");
//		data_ = NULL;
//	}
//
//	virtual bool GetValue( wxVariant &WXUNUSED(value) ) const { return true; }
//
//	bool SetValue( const wxVariant &value );
//
//	wxSize GetSize() const
//	{
//		return wxSize(100,20);
//	}
//
//	bool Render(wxRect rect, wxDC *dc, int WXUNUSED(state));
//
//	bool Activate( wxRect WXUNUSED(cell), wxDataViewModel *model, const wxDataViewItem & item, unsigned int col );
//
//private:
//	wxString name_;
//	ShowPageData* data_;
//};

//WX_DEFINE_ARRAY(Server*, wxArrayOfServer);
class MyListModel: public wxDataViewVirtualListModel
{
public:
	MyListModel();
	~MyListModel();

	void AddData(std::list<ShowPageData*>& data_list, PageAssist* page_assist, DWORD &dwSum);

	bool HasContainerColumns(const wxDataViewItem& WXUNUSED(item)) const
	{ return true; }
	// implementation of base class virtuals to define model


	virtual unsigned int GetColumnCount() const
	{
		return 8;
	}

	virtual wxString GetColumnType( unsigned int col ) const
	{
		/*if (col == 1)
			return wxT("wxDataViewIconText");

		return wxT("string");*/
		if (col == 6 || col == 7 || col == 8)
		{
			return wxT("void*");
		}
		return wxT("string");
	}

	virtual unsigned int GetRowCount()
	{
		return data_vector_.size();
	}

	virtual void GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const;
	virtual bool GetAttrByRow( unsigned int row, unsigned int col, wxDataViewItemAttr &attr );
	virtual bool SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col );

	std::vector<ShowPageData*>& GetDatas() { return data_vector_; }

private:
	std::vector<ShowPageData*> data_vector_;
	PageAssist * page_assist_;
};

class ShowPage : public wxPanel
{
public:
	ShowPage(wxBookCtrlBase* book, const wxString label);
	//void InsertSum(DWORD, PageAssist*);
	const wxString& GetLabel() { return label_; }
	void ChangeLabel(wxString label) { label_ = label; }

	//用数据更新界面,回调函数
	void update_ui(PageAssist *page_assist, BOOL is_static);
	
	DWORD GetSum() { return dwSum_; }

	//void OpenMultiRegionService(wxCommandEvent& event);
	//void CloseMultiRegionService(wxCommandEvent& event);
	//void SelectAllServer(wxCommandEvent& event);
	//void UnSelectAllServer(wxCommandEvent& event);
	//void SelectAllBadServer(wxCommandEvent& event);
	//void UnSelectAllBadServer(wxCommandEvent& event);
	//void SendSystemCmd(wxCommandEvent& event);
	//void ChangeMaxOnline(wxCommandEvent& event);
	//void ReloadMall(wxCommandEvent& event);

	//void ShowButtons(bool bShow_1, bool bShow_2, bool bShow_3, bool bShow_4);

private:
	//页面标题
	wxString label_;

	//正常状态服务器
	wxDataViewCtrl	*servers_;
	MyListModel     *model_;
	
	////全选，取消全选
	//wxButton *server_all_select;
	//wxButton *server_all_unselect;

	////异常状态服务器
	//wxDataViewCtrl	*servers_abnormal;
	//MyListModel		*model_abnormal;

	////全选，取消全选
	//wxButton *badserver_all_select;
	//wxButton *badserver_all_unselect;

	////多区开服
	//wxButton *multi_region_open_service;
	////多区关服
	//wxButton *multi_region_close_service;
	
	PageAssist *page_assist_;

	////设置系统指令
	////wxTextCtrl*	m_cmd;
	//wxButton* m_set_cmd_ok;
	//wxComboBox*	m_combobox_system_cmd; 

	//// 修改最大在线人数
	//wxTextCtrl*	m_MaxNum;
	//wxButton* m_set_MaxNum_ok;

	//// 更新商城
	//wxButton* m_Reload_Mall_ok;

	DWORD dwSum_;		// 统计获得的总人数

	DECLARE_EVENT_TABLE()
};

#endif /* OPENAANDCLOSESERVERPAGE_H_ */

