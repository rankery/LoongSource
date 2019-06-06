#ifndef PAGEASSIST_H_
#define PAGEASSIST_H_

#include "../ServerDefine/rt_define.h"

class OpenAndCloseServerPage;
struct UserInfo;
//开关服务器页面数据
struct OpenAndCloseServerPageData;
struct SectionInfo;

class PageAssist;
typedef void (OpenAndCloseServerPage::* OACSUPDATEFUNC)(PageAssist*, BOOL is_static);

class RTServer;

//为了使界面和数据分离
class PageAssist
{
public:

	PageAssist();
	~PageAssist();
	//得到登录用户信息
	void SetUserInfo(const UserInfo* user_info);
	UserInfo* GetUserInfo() { return user_info_; }

	//得到OpenAndCloseServerPageDataList
	std::list<OpenAndCloseServerPageData*>& GetOpenAndCloseServerPageDtatList() { return openandcloseserver_page_data_list_; }
	std::list<OpenAndCloseServerPageData*>& GetAbnormalServerPageDtatList() { return page_data_list_abnormal; }
	void AddOpenAndCloseServerPageDtatList(int index, EServerType type, wxString section_name,  DWORD section_id, 
														wxString world_name,   DWORD world_id, wxString ip);
	void UpdateOpenAndCloseServerPageDtatList(SectionInfo* section_info);
	void ClearOpenAndCloseServerPageDtatList() 
	{ 
		openandcloseserver_page_data_list_.clear();
		page_data_list_abnormal.clear();
		temp_moving_data_list.clear();
	}

	void SetOpenAndCloseServerPageFunc(OpenAndCloseServerPage *page, OACSUPDATEFUNC fn ) { page_ = page; OpenAndCloseServerPage_fn = fn; }
	void UpdateOpenAndCloseServerPage( BOOL is_static);

	void SendStartServerMessage(OpenAndCloseServerPageData* data, BOOL alarm);
	void SendCloseServerMessage(BOOL is_force, OpenAndCloseServerPageData* data, BOOL alarm);

	void SwitchDatalist(OpenAndCloseServerPageData* server_data);
	void ResortDatalist();

	void SendSystemCmdMessage(OpenAndCloseServerPageData* data, BOOL alarm, LPCTSTR szCmd);

	BOOL SendChangeMaxMessage(OpenAndCloseServerPageData* data, BOOL alarm, LPCTSTR szCmd);
	
	BOOL UpdateMall(OpenAndCloseServerPageData* data, BOOL alarm);

	//GM管理页面功能
	void SendCreateGMMessage(LPCTSTR szName, LPCTSTR szPwd, EPrivilege ePrivilege);
	void SendModifyGMMessage(LPCTSTR szName, DWORD dwIP);
	void SendDeleteGMMessage(LPCTSTR szName);
	void SendQueryGMMessage();

private:
	OACSUPDATEFUNC OpenAndCloseServerPage_fn;
	OpenAndCloseServerPage *page_;
	std::list<OpenAndCloseServerPageData*> openandcloseserver_page_data_list_;
	std::list<OpenAndCloseServerPageData*> page_data_list_abnormal;
	UserInfo *user_info_;
	RTServer* server_;

	BOOL bListChange;
	std::list<OpenAndCloseServerPageData*> temp_moving_data_list;
};


#endif  /* PAGEASSIST_H_ */