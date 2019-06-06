//定义程序所需要的数据结构.
//2009-04-09

#ifndef DATA_DEFINE_H_ 
#define DATA_DEFINE_H_

#include "../ServerDefine/login_define.h"
#include "../ServerDefine/rt_define.h"

class PageAssist;

//登录用户信息
struct UserInfo
{
	wxString name;
	wxString password;
	int privilege;
};

struct GameWorldInfo
{
	//游戏世界名称,world name
	wxString world_name;
	//world id
	DWORD id;
	//world ip
	wxString world_ip;
	//db ip
	wxString db_ip;
	//world 状态
	EWorldStatus	world_status;
	//DB 状态
	EWorldStatus	db_status;
	// 在线人数
	INT					nPlayerOnline;
	// 总人数上限
	INT					nMaxPlayerNum;	
};

//大区信息
struct SectionInfo
{
	//大区名字
	wxString section_name;
	//名字Crc
	DWORD  id;
	//Login ip
	wxString login_ip;
	//Login 状态
	EWorldStatus	login_status;
	//Login连接World的人数信息
	INT nPlayerOnline;
	INT nMaxPlayerNum;

	std::map<DWORD, GameWorldInfo> game_world_infos;
};

//开关服务器页面数据
struct OpenAndCloseServerPageData 
{
	//服务器类型  world db Login
	EServerType type;
	//大区名字
	wxString section_name;
	DWORD section_id;

	//游戏世界名称
	wxString world_name;
	DWORD world_id;

	//服务器ip
	wxString ip;
	//服务器状态
	EWorldStatus status;
	//在线人数
	int online_number;
	//人数上限
	int max_online;
	//操作名称
	wxString operate_name;
	//是否被选中
	bool is_check;
	//PageAssist指针
	PageAssist *page_assist;
	//列表中顺序标识
	int index;

	OpenAndCloseServerPageData()
	{
		page_assist = NULL;
		section_id = 0;
		world_id = 0;
		section_name = wxT("");
		world_name = wxT("");
		ip = wxT("");
		world_name = wxT("");
		status = EWS_InitNotDone;
		online_number = 0;
		max_online = 0;
		operate_name = wxT("");
		is_check = false;
		index = 0;
	}
};

//仿函数类
class CmpData{   
public:   
	CmpData() {};   
	bool operator()(OpenAndCloseServerPageData*& x, OpenAndCloseServerPageData*& y) const
	{   
		return x->index < y->index; 
	}
};  

#endif /* DATA_DEFINE_H_ */