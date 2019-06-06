#pragma once
#include "..\WorldDefine\chat.h"
class ChatShowPop
{
public:
	ChatShowPop(void);
	~ChatShowPop(void);
	static ChatShowPop* Inst();


	//玩家发言时调用，如果找到则创建创建窗口
	void PushChatInfo(DWORD dwRoleID,const TCHAR* szText);
	
	//接受聊天信息
	void NetRecvRoleChat(tagNS_RoleChat* pMsg);

	void DrawInfo();

	void Destroy();

private:
	//删除某人的喊话信息保存,同时销毁窗口
	void DeleteChatShow(DWORD dwRoleID);
	
private:
	GUIWnd*						m_pWnd;
	
	map<DWORD,float>			m_mapChatShow;			//发内容保存在对应的xui文件中，这里还需要保存喊话的时间
};