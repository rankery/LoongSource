#pragma once
#include "Singleton.h"
#include "UserEvent.h"
//--------------------------------------------------------------------
// 注册用户窗口事件
//--------------------------------------------------------------------
class UserFrame;



typedef DWORD (WINAPI* FRAMEEVENTPROC)(tagUserEvent* pEvent);

class UserFrameMgr : public Singleton<UserFrameMgr>
{
public:
	UserFrameMgr();
	~UserFrameMgr();

	BOOL Init();
	BOOL Destroy();

	VOID SendEvent(tagUserEvent* pEvent);

	// 注册Frame事件处理函数
	BOOL RegisterEventHandle(LPCTSTR szEvent, FRAMEEVENTPROC fp);
	BOOL UnRegisterEventHandler(LPCTSTR szEvent, FRAMEEVENTPROC fp);

private:

	struct tagEventHandle	// 事件处理结构定义
	{
		tstring					strName;
		FRAMEEVENTPROC			fp;
	};

	std::map<DWORD, tagEventHandle*>	m_mapHandle;	// 事件处理者
};

#define sUserFrameMgrPgr UserFrameMgr::GetSingletonPtr()