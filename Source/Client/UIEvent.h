#pragma once

/** \设置地图标志事件
*/
struct tagSetFlagEvent : public tagGameEvent
{
	float x;
	float z;

	tagSetFlagEvent():tagGameEvent(_T("tagSetFlagEvent"),NULL){}
	tagSetFlagEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame){}
};

/** \打开/关闭地图窗口事件
*/
struct tagOpenCloseMapWndEvent : public tagGameEvent
{
	bool bWorldMap;

	tagOpenCloseMapWndEvent():tagGameEvent(_T("tagOpenCloseMapWndEvent"),NULL), bWorldMap( false ){}
	tagOpenCloseMapWndEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame){}
};

/** \打开地图窗口事件
*/
struct tagOpenMapWndEvent : public tagGameEvent
{
	bool bWorldMap;

	tagOpenMapWndEvent():tagGameEvent(_T("tagOpenMapWndEvent"),NULL), bWorldMap( false ){}
	tagOpenMapWndEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame){}
};