//-----------------------------------------------------------------------------
//!\file gui_define.h
//!\author Lyp
//!
//!\date 2004-12-09
//! last 2005-03-29
//!
//!\brief 界面系统框架数据结构定义
//-----------------------------------------------------------------------------
#pragma once

namespace vEngine {

// 各种类的预申明
class IGUIRender;
class GUISystem;
class GUIWnd;
class GUIStatic;
class GUIStaticEx;
class GUIButton;
class GUIPushButton;
class GUIEditBox;
class GUIEditBoxEx;
class GUIListBox;
class GUIScrollBar;
class GUIProgress;
class GUIComboBox;
class GUIPatch;
class GUIFrameMutex;
class GUIVirtual;
class GUITab;
class GUIWindow;
class GUIHttpPic;
class GUIEditor;
class GUIEventMgr;

struct tagGUIEvent;
typedef DWORD (WINAPI* VUIEVENTHANDLE)(DWORD);


enum EGUIButtonState	// 按钮状态
{
	EGUIBS_PopUp=0,			// 正常弹起状态
	EGUIBS_PushDown,		// 按下状态
	EGUIBS_HighLight,		// 高亮状态，一般表示鼠标正指向
};

enum EGUIAlign	// 对齐方式
{
	EGUIA_Null=0,			// 不自动对齐
	EGUIA_Center,			// 内部居中
	EGUIA_InnerLeftTop,		// 内部左上
	EGUIA_InnerLeft,		// 内部左中
	EGUIA_InnerLeftBottom,	// 内部左下
	EGUIA_InnerRightTop,	// 内部右上
	EGUIA_InnerRight,		// 内部右中
	EGUIA_InnerRightBottom,	// 内部右下
	EGUIA_InnerTop,			// 内部上中
	EGUIA_InnerBottom,		// 内部下中
	EGUIA_OuterLeftTop,		// 外部左上
	EGUIA_OuterLeft,		// 外部左中
	EGUIA_OuterLeftBottom,	// 外部左下
	EGUIA_OuterRightTop,	// 外部右上
	EGUIA_OuterRight,		// 外部右中
	EGUIA_OuterRightBottom,	// 外部右下
	EGUIA_OuterTopLeft,		// 外部上左
	EGUIA_OuterTop,			// 外部上中
	EGUIA_OuterTopRight,	// 外部上右
	EGUIA_OuterBottomLeft,	// 外部下左
	EGUIA_OuterBottom,		// 外部下中
	EGUIA_OuterBottomRight,	// 外部下右
};


//-----------------------------------------------------------------------------
// 内部使用的输入消息
//-----------------------------------------------------------------------------
struct tagGUIInputMsg	
{
	DWORD			dwType;		// 消息类别
	DWORD			dwParam1;	// win32 msg的两个参数
	DWORD			dwParam2;	// win32 msg的两个参数
	tagPoint		pt;			// 事件发生的鼠标位置

	VOID Set(DWORD dw, DWORD dw1, DWORD dw2) 
	{ dwType = dw; dwParam1 = dw1; dwParam2 = dw2; pt.Set(LOWORD(dw2), HIWORD(dw2)); }
};


typedef BOOL (WINAPI* VUIMSGHANDLE)(tagGUIInputMsg*);

}	// namespace vEngine {


#include "gui_render.h"
#include "gui_wnd.h"
#include "gui_event_define.h"
#include "gui_system.h"

