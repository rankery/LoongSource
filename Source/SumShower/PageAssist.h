#ifndef PAGEASSIST_H_
#define PAGEASSIST_H_

#include "../ServerDefine/rt_define.h"

class ShowPage;
struct UserInfo;
//开关服务器页面数据
struct ShowPageData;
struct SectionInfo;

class PageAssist;
typedef void (ShowPage::* OACSUPDATEFUNC)(PageAssist*, BOOL is_static);

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

	//得到ShowPageDataList
	std::list<ShowPageData*>& GetShowPageDtatList() { return openandcloseserver_page_data_list_; }
	std::list<ShowPageData*>& GetAbnormalServerPageDtatList() { return page_data_list_abnormal; }
	void AddShowPageDtatList(int index, EServerType type, wxString section_name,  DWORD section_id, 
														wxString world_name,   DWORD world_id, wxString ip);
	void UpdateShowPageDtatList(SectionInfo* section_info);
	void ClearShowPageDtatList() 
	{ 
		openandcloseserver_page_data_list_.clear();
		page_data_list_abnormal.clear();
		temp_moving_data_list.clear();
	}

	void SetShowPageFunc(ShowPage *page, OACSUPDATEFUNC fn ) { page_ = page; ShowPage_fn = fn; }
	void UpdateShowPage( BOOL is_static);

	//void SendStartServerMessage(ShowPageData* data, BOOL alarm);
	//void SendCloseServerMessage(BOOL is_force, ShowPageData* data, BOOL alarm);

	void SwitchDatalist(ShowPageData* server_data);
	void ResortDatalist();

	/*void SendSystemCmdMessage(ShowPageData* data, BOOL alarm, LPCTSTR szCmd);

	BOOL SendChangeMaxMessage(ShowPageData* data, BOOL alarm, LPCTSTR szCmd);
	
	BOOL UpdateMall(ShowPageData* data, BOOL alarm);*/

	//GM管理页面功能
	/*void SendCreateGMMessage(LPCTSTR szName, LPCTSTR szPwd, EPrivilege ePrivilege);
	void SendModifyGMMessage(LPCTSTR szName, DWORD dwIP);
	void SendDeleteGMMessage(LPCTSTR szName);
	void SendQueryGMMessage();*/

private:
	OACSUPDATEFUNC ShowPage_fn;
	ShowPage *page_;
	std::list<ShowPageData*> openandcloseserver_page_data_list_;
	std::list<ShowPageData*> page_data_list_abnormal;
	UserInfo *user_info_;
	RTServer* server_;

	BOOL bListChange;
	std::list<ShowPageData*> temp_moving_data_list;
};


#endif  /* PAGEASSIST_H_ */