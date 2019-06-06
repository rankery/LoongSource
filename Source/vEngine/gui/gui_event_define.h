//-----------------------------------------------------------------------------
//!\file gui_event_define.h
//!\author Lyp
//!
//!\date 2005-04-29
//! last 2005-04-29
//!
//!\brief 界面系统框架事件定义
//-----------------------------------------------------------------------------
#pragma once

namespace vEngine {
//-----------------------------------------------------------------------------
// 事件枚举
//-----------------------------------------------------------------------------
enum EGUIEvent
{
	EGUIE_Null=0,
	EGUIE_Create,					// 控件刚被创建
	EGUIE_Destroy,					// 控件被销毁
	EGUIE_Click,					// 控件被一次完整的点击
	EGUIE_PushButtonDown,			// PushButton按下
	EGUIE_EditBoxEnter,				// 用户在edit box内按下回车
	EGUIE_UserChangeListContext,	// 用户改变List内容
	EGUIE_ItemClick,				// 项目被点击
	EGUIE_ItemDblClick,				// 项目被双击
	EGUIE_Scroll,					// 滚动条之类的滚动, dwParam1=当前值, Param2=总值
	EGUIE_Active,					// 被激活
	EGUIE_Deactive,					// 不再被激活
	EGUIE_FrameMutex,				// frame内部的互斥事件dwParam = 0或者 1.
	EGUIE_RichLinkClick,			// richeditbox中超链接被点击(双击或者单击看参数)
	EGUIE_DragStart,				// 拖动开始
	EGUIE_DragEnd,					// 拖动停止
	EGUIE_DragOver,					// 拖动经过
	EGUIE_Drag,						// 移动事件
	EGUIE_Resize,					// 改变大小事件
	EGUIE_KeyDown,					// VIRTUAL_KEY_DOWN事件
	EGUIE_CursorMove,				// 光标移动(EditBoxEx)
	EGUIE_MoveIntoCtrl,				// 鼠标移入该控件	
	EGUIE_MoveOutOfCtrl,			// 鼠标移出该控件
	EGUIE_StaticExLinkClick,		// StaticEx中超链接被点击(双击或者单击看参数)

	EGUIE_User = 0x80000000,		// 客户自定义事件
	EGUIE_End  = 0xffffffff,
};



//-----------------------------------------------------------------------------
// 事件数据结构
//-----------------------------------------------------------------------------
struct tagGUIEvent
{
	DWORD			dwWndID;		// 实际发生事件的控件
	EGUIEvent		eEvent;			// 事件类型
	DWORD			dwParam1;		// 事件参数
	DWORD			dwParam2;
	DWORD			dwParam3;
	DWORD			dwParam4;
	tagGUIInputMsg	msg;			// 引发事件的消息
	DWORD			dwSendWndID;	// 发送此事件的控件


	tagGUIEvent(GUIWnd* pGUIWnd, EGUIEvent event, tagGUIInputMsg* pM=NULL, 
		DWORD dw1=0, DWORD dw2=0, DWORD dw3=0, DWORD dw4=0)
		:dwWndID(GT_INVALID), eEvent(event), dwParam1(dw1), dwParam2(dw2), dwParam3(dw3), dwParam4(dw4)
	{ 
		if( P_VALID(pGUIWnd) )
			dwWndID = pGUIWnd->GetID();

		if(pM) 
			memcpy(&msg, pM, sizeof(msg)); 
	}
};

}	// namespace vEngine {
