//-----------------------------------------------------------------------------
//!\file GameFrameMgr.h
//!\author Lyp
//!
//!\date 2008-02-17
//! last 2008-04-15
//!
//!\brief game frame mgr
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

class GameFrame;
struct tagGameEvent
{
	DWORD		dwNameCrc;
	tstring		strEventName;
	GameFrame*	pSender;	// 发送此事件的Frame

	tagGameEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
	{
		strEventName = szEventName;
		dwNameCrc = TObjRef<Util>()->Crc32(szEventName);
		pSender = pSenderFrame;
	}
};


typedef DWORD (WINAPI* FRAMEEVENTPROC)(tagGameEvent* pEvent);
//-----------------------------------------------------------------------------
//!\brief game frame mgr
//!
//!
//-----------------------------------------------------------------------------
class GameFrameMgr
{
public:
	GameFrameMgr();
	~GameFrameMgr();

	BOOL Init();
	BOOL Destroy();
	VOID Update();
	VOID Render3D();

	VOID SendEvent(tagGameEvent* pEvent);

	GameFrame* CreateFrame(LPCTSTR szFatherName, LPCTSTR szName, LPCTSTR szClass, DWORD dwParam);
	GameFrame* GetFrame(LPCTSTR szName) { return m_mapChild.Peek(m_pUtil->Crc32(szName)); }
	TMap<DWORD, GameFrame*>& GetMap() { return m_mapChild; }

	VOID DestroyFrame(GameFrame* pFrame);	// 立即删除
	BOOL AddToDestroyList(GameFrame* pWnd);	// 下一祯删除


	// 注册frame类型
	template<typename T>VOID Register(TWrap<T> wrap, LPCTSTR szName);

	// 注册Frame事件处理函数
	BOOL RegisterEventHandle(LPCTSTR szEvent, FRAMEEVENTPROC fp);
	BOOL UnRegisterEventHandler(LPCTSTR szEvent, FRAMEEVENTPROC fp);

private:
	TObjRef<Util>				m_pUtil;
	TAutoFactory<GameFrame>		m_Factory;

	std::list<tstring>			m_listWndType;	// 所有控件类型
	GameFrame*					m_pRoot;		// 根frame
	TMap<DWORD, GameFrame*>		m_mapChild;		// 所有frame

	TList<GameFrame*>			m_listDestroy;	// 删除列表

	struct tagEventHandle	// 事件处理结构定义
	{
		tstring					strName;
		DWORD					dwCount;	// 记录曾经发生过多少次此事件
		TList<FRAMEEVENTPROC>	listFP;
	};
	TMap<DWORD, tagEventHandle*>	m_mapHandle;	// 事件处理者

};




//-----------------------------------------------------------------------------
// 注册frame
//-----------------------------------------------------------------------------
template<typename T>VOID GameFrameMgr::Register(TWrap<T> wrap, LPCTSTR szName)
{ 
	if( m_Factory.Register(wrap, m_pUtil->Crc32(szName)) )
		m_listWndType.push_back(tstring(szName));
}