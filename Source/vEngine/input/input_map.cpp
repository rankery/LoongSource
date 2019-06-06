//-----------------------------------------------------------------------------
//!\file input_map.cpp
//!\author Lyp
//!
//!\date 2008-02-27
//! last 2008-02-27
//!
//!\brief 键盘命令注册
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "input_map.h"

// 使用其它部件
#include "input_dx8.h"
#include "..\console\console_gui.h"
#include "..\gui\gui_static.h"
#include "..\gui\gui_window.h"
#include "..\gui\gui_button.h"
#include "..\gui\gui_listbox.h"
#include "..\gui\gui_event_define.h"

namespace vEngine {
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
InputMap::InputMap():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndCaption = NULL;
	m_pWndList = NULL;
	m_pWndOK = NULL;
	m_dwCurrentSet = GT_INVALID;
	m_dwTempKey = 0;
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
InputMap::~InputMap()
{
	tagKeyMapInfo* pInfo = 0;
	m_mapKeyFunction.ResetIterator();
	while(m_mapKeyFunction.PeekNext(pInfo))
		SAFE_DEL(pInfo);

	m_mapKeyFunction.Clear();
}



//-----------------------------------------------------------------------------
// load gui
//-----------------------------------------------------------------------------
BOOL InputMap::LoadGUI(LPCSTR szVFS, LPCTSTR szWindowName, tagPoint& ptPos)
{
	// 创建窗口
	tstring strPath = g_strLocalPath + _T("\\ui\\key_map.xml");
	m_pWnd = (GUIWindow*)m_pGUI->CreateFromFile(szVFS, strPath.c_str());

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&vEngine::InputMap::EventHandler));

	m_pWndCaption = (GUIStatic*)m_pWnd->GetChild(_T("caption"));
	m_pWndList = (GUIListBox*)m_pWnd->GetChild(_T("list"));
	m_pWndOK = (GUIButton*)m_pWnd->GetChild(_T("ok"));

	m_pWndCaption->SetText(szWindowName);
	m_pWnd->Move(ptPos);

	this->ShowKeys();
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy GUI
//-----------------------------------------------------------------------------
VOID InputMap::UnloadGUI()
{
	m_pGUI->DestroyWnd(m_pWnd);
	m_pWnd = NULL;
}


//-----------------------------------------------------------------------------
// show keys
//-----------------------------------------------------------------------------
VOID InputMap::ShowKeys()
{
	m_pWndList->Clear();
	m_pWndList->SetColNum(2,300);

	// 显示所有的注册按键
	m_listKeyDesc.ResetIterator();
	DWORD dwDesc = 0;
	INT nIndex = 0;
	while(m_listKeyDesc.PeekNext(dwDesc))
	{
		tagKeyMapInfo* pInfo = m_mapKeyFunction.Peek(dwDesc);
		if( pInfo->bShow )
		{
			m_pWndList->SetText(nIndex, 0, pInfo->strDesc.c_str());

			tstringstream stream;
			if( pInfo->dwKey & 0xffff )
				stream << this->GetKeyName((DIK)pInfo->dwKey);
			if( pInfo->dwKey & 0xffff0000 )
				stream << _T(" + ") << this->GetKeyName((DIK)(pInfo->dwKey >> 16));

			m_pWndList->SetText(nIndex, 1, stream.str().c_str());
			nIndex++;
		}
	}

}


//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID InputMap::Update()
{
	if( P_VALID(m_pWnd) && GT_VALID(m_dwCurrentSet) )	// 设置状态
		return UpdateSetup();

	tagKeyMapInfo* pInfo = NULL;
	m_mapKeyFunction.ResetIterator();
	while( m_mapKeyFunction.PeekNext(pInfo) )
	{
		if( !pInfo->bEnable )
			continue;

		if( !pInfo->bEnableInEdit && m_pGUI->GetActive()->IsEditable() )
			continue;

		BOOL bCall = FALSE, bDown = FALSE;
		DIK k1 = (DIK)(pInfo->dwKey), k2 = (DIK)(pInfo->dwKey>>16);
		if( k2 )	// 双键还是单键
		{
			if( (m_pInput->IsKeyDownOnce(k1) && m_pInput->IsKeyDown(k2))
				|| (m_pInput->IsKeyDown(k1) && m_pInput->IsKeyDownOnce(k2)) )
				bCall = bDown = TRUE;	// 按下

			if( (m_pInput->IsKeyUpOnce(k1) && m_pInput->IsKeyDown(k2))
				|| (m_pInput->IsKeyDown(k1) && m_pInput->IsKeyUpOnce(k2)) )
				bCall = TRUE, bDown = FALSE;	// 弹起
		}
		else	// 单键
		{
			if( k1 != DIK_LSHIFT && k1 != DIK_RSHIFT 
				&& k1 != DIK_LMENU && k1 != DIK_RMENU 
				&& k1 != DIK_LCONTROL && k1 != DIK_LCONTROL 
				&& k1 != DIK_LWIN && k1 != DIK_RWIN )
			{	// 对于各种状态键需要特殊处理，各种状态键按下时，各种单键不起作用
				if( m_pInput->IsKeyDown(DIK_LSHIFT)
					|| m_pInput->IsKeyDown(DIK_RSHIFT) 
					|| m_pInput->IsKeyDown(DIK_LMENU) 
					|| m_pInput->IsKeyDown(DIK_RMENU) 
					|| m_pInput->IsKeyDown(DIK_LCONTROL) 
					|| m_pInput->IsKeyDown(DIK_LCONTROL) 
					|| m_pInput->IsKeyDown(DIK_LWIN) 
					|| m_pInput->IsKeyDown(DIK_LWIN) )
				continue;
			}

			if( m_pInput->IsKeyDownOnce(k1) )	// 按下
				bCall = bDown = TRUE;

			if( m_pInput->IsKeyUpOnce(k1) )	// 弹起
				bCall = TRUE, bDown = FALSE;
		}

		if( bCall )
		{
			(pInfo->pfn)(pInfo->dwID, bDown);
		}
	}// while

}


//-----------------------------------------------------------------------------
// 设置状态时的更新
//-----------------------------------------------------------------------------
VOID InputMap::UpdateSetup()
{
	std::vector<DIK> vec;
	std::vector<DIK>::iterator it;
	BOOL bResult = m_pInput->GetKeyDown(vec);

	LPCTSTR szText = m_pWndList->GetText(m_dwCurrentSet, 0);
	if( !P_VALID(szText) )
		return;

	DWORD dwID = m_pUtil->Crc32(szText);
	tagKeyMapInfo* pInfo = m_mapKeyFunction.Peek(dwID);
	if( !P_VALID(pInfo) )
		return;

	if( !bResult && m_dwTempKey != 0 )	// 按键全部松开
	{
		m_dwCurrentSet = GT_INVALID;	// 退出设置状态
		pInfo->dwKey = m_dwTempKey;
		this->ShowKeys();
		if( pInfo->pfnChange )
		{
			// 通知按键更改
			(pInfo->pfnChange)(pInfo->dwID, (DIK)pInfo->dwKey, (DIK)(pInfo->dwKey>>16));
		}
		return;
	}

	if( vec.size() > 0 )
	{
		m_dwTempKey = vec[0];
		if( vec.size() > 1 )	// 同时按下俩键
		{
			m_dwCurrentSet = GT_INVALID;	// 退出设置状态
			m_dwTempKey |= (vec[1] << 16);
			pInfo->dwKey = m_dwTempKey;
			if( pInfo->pfnChange )
			{
				// 通知按键更改
				(pInfo->pfnChange)(pInfo->dwID, (DIK)pInfo->dwKey, (DIK)(pInfo->dwKey>>16));
			}

		}
		this->ShowKeys();
	}
}


//-----------------------------------------------------------------------------
//! 注册按键需求
//-----------------------------------------------------------------------------
BOOL InputMap::Register(DWORD dwID, LPCTSTR szDesc, INPUTMAPEVENTHANDLE fpFunction, 
						DWORD key, BOOL bShow, BOOL bEnableInEdit, INT nUserData,
						INPUTMAPCHANGEEVENTHANDLE fpChangeFunction)
{
	DWORD dwDesc = m_pUtil->Crc32(szDesc);
	tagKeyMapInfo* pInfo = m_mapKeyFunction.Peek(dwDesc);
	if( P_VALID(pInfo) )
	{
		IMSG(_T("re register keys:%s"), szDesc);
		pInfo->bEnable = TRUE;	// 重新设置为有效
		pInfo->pfn = fpFunction;
		pInfo->dwOriginKey = key;
		pInfo->pfnChange = fpChangeFunction;
		return FALSE;
	}

	pInfo = new tagKeyMapInfo;
	if( !P_VALID(pInfo) )
		return FALSE;

	pInfo->dwID = dwID;
	pInfo->dwKey = key;
	pInfo->dwOriginKey = key;
	pInfo->dwDefaultKey = key;
	pInfo->strDesc = szDesc;
	pInfo->bEnable = TRUE;
	pInfo->bShow = bShow;
	pInfo->bEnableInEdit = bEnableInEdit;
	pInfo->nUserData = nUserData;
	
	pInfo->pfn = fpFunction;
	pInfo->pfnChange = fpChangeFunction;

	m_mapKeyFunction.Add(dwDesc, pInfo);
	m_listKeyDesc.PushBack(dwDesc);
	return TRUE;
}


//-----------------------------------------------------------------------------
//! 注销按键需求
//-----------------------------------------------------------------------------
BOOL InputMap::Unregister(LPCTSTR szDesc)
{
	ASSERT(0); // 从游戏保留键位功能上考虑不允许调用此函数

	DWORD dwDesc = m_pUtil->Crc32(szDesc);
	tagKeyMapInfo* pInfo = m_mapKeyFunction.Peek(dwDesc);
	if( !P_VALID(pInfo) )
	{
		ASSERT(0);	// 未注册
		return FALSE;
	}

	SAFE_DEL(pInfo);
	m_mapKeyFunction.Erase(dwDesc);
	return TRUE;
}

//-----------------------------------------------------------------------------
//! 设置按键是否有效
//-----------------------------------------------------------------------------
BOOL InputMap::SetEnable(LPCTSTR szDesc, BOOL bEnable)
{
	DWORD dwDesc = m_pUtil->Crc32(szDesc);
	tagKeyMapInfo* pInfo = m_mapKeyFunction.Peek(dwDesc);
	if( !P_VALID(pInfo) )
	{
		ASSERT(0);	//未注册
		return FALSE;
	}

	pInfo->bEnable = bEnable;
	return TRUE;
}


//-----------------------------------------------------------------------------
//! 事件处理
//! 
//-----------------------------------------------------------------------------
BOOL InputMap::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		if( pWnd == m_pWndOK )
		{
			this->UnloadGUI();
		}
		break;
	case EGUIE_ItemClick:
		if( pWnd == m_pWndList )
		{
			LPCTSTR szText = m_pWndList->GetText(pEvent->dwParam1, pEvent->dwParam2);
			if( !P_VALID(szText) )
			{
				m_dwCurrentSet = GT_INVALID;
				break;
			}

			m_dwCurrentSet = pEvent->dwParam1;	// 进入设置状态
			m_dwTempKey = 0;
		}
		break;
	}

	return FALSE;
}


//-----------------------------------------------------------------------------
//! 由按键得到名字
//-----------------------------------------------------------------------------
LPCTSTR InputMap::GetKeyName(DIK key)
{
	switch(key)
	{
	case DIK_ESCAPE:		return _T("ESC");
	case DIK_1:				return _T("1");        
	case DIK_2:				return _T("2");               
	case DIK_3:				return _T("3");               
	case DIK_4:				return _T("4");               
	case DIK_5:				return _T("5");               
	case DIK_6:				return _T("6");               
	case DIK_7:				return _T("7");               
	case DIK_8:				return _T("8");               
	case DIK_9:				return _T("9");               
	case DIK_0:				return _T("0");               
	case DIK_MINUS:			return _T("-");
	case DIK_EQUALS:		return _T("=");
	case DIK_BACK:			return _T("BackSpace");
	case DIK_TAB:			return _T("TAB");             
	case DIK_Q:				return _T("Q");               
	case DIK_W:				return _T("W");               
	case DIK_E:				return _T("E");
	case DIK_R:				return _T("R");
	case DIK_T:				return _T("T");              
	case DIK_Y:				return _T("Y");
	case DIK_U:				return _T("U");
	case DIK_I:				return _T("I");
	case DIK_O:				return _T("O");
	case DIK_P:				return _T("P");
	case DIK_LBRACKET:		return _T("[");
	case DIK_RBRACKET:		return _T("]");
	case DIK_RETURN:		return _T("Enter");
	case DIK_LCONTROL:		return _T("L-Control");
	case DIK_A:				return _T("A");
	case DIK_S:				return _T("S");
	case DIK_D:				return _T("D");
	case DIK_F:				return _T("F");
	case DIK_G:				return _T("G");
	case DIK_H:				return _T("H");
	case DIK_J:				return _T("J");
	case DIK_K:				return _T("K");
	case DIK_L:				return _T("L");
	case DIK_SEMICOLON:		return _T(";");
	case DIK_APOSTROPHE:	return _T("'");
	case DIK_GRAVE:			return _T("`");					/* accent grave */
	case DIK_LSHIFT:		return _T("L-Shift");
	case DIK_BACKSLASH:		return _T("\\");       
	case DIK_Z:				return _T("Z");               
	case DIK_X:				return _T("X");               
	case DIK_C:				return _T("C");               
	case DIK_V:				return _T("V");               
	case DIK_B:				return _T("B");                              
	case DIK_N:				return _T("N");                              
	case DIK_M:				return _T("M");                              
	case DIK_COMMA:			return _T(",");                  
	case DIK_PERIOD:		return _T(".");					/* . on main keyboard */
	case DIK_SLASH:			return _T("/");					/* / on main keyboard */
	case DIK_RSHIFT:		return _T("R-Shift");                         
	case DIK_MULTIPLY:		return _T("NumPad*");           /* * on numeric keypad */
	case DIK_LMENU:			return _T("L-Alt");             /* left Alt */
	case DIK_SPACE:			return _T("Space");                          
	case DIK_CAPITAL:		return _T("CapsLock");                        
	case DIK_F1:			return _T("F1");                             
	case DIK_F2:			return _T("F2");                                           
	case DIK_F3:			return _T("F3");                                           
	case DIK_F4:			return _T("F4");                                           
	case DIK_F5:			return _T("F5");                                           
	case DIK_F6:			return _T("F6");                                           
	case DIK_F7:			return _T("F7");                                           
	case DIK_F8:			return _T("F8");                                           
	case DIK_F9:			return _T("F9");                                           
	case DIK_F10:			return _T("F10");                                          
	case DIK_NUMLOCK :		return _T("NumLock");        
	case DIK_SCROLL:		return _T("ScrollLock");        /* Scroll Lock */
	case DIK_NUMPAD7:		return _T("NumPad7");         
	case DIK_NUMPAD8:		return _T("NumPad8");         
	case DIK_NUMPAD9:		return _T("NumPad9");         
	case DIK_SUBTRACT:		return _T("NumPad-");           /* - on numeric keypad */
	case DIK_NUMPAD4:		return _T("NumPad4");         
	case DIK_NUMPAD5:		return _T("NumPad5");         
	case DIK_NUMPAD6:		return _T("NumPad6");         
	case DIK_ADD:			return _T("NumPad+");           /* + on numeric keypad */
	case DIK_NUMPAD1:		return _T("NumPad1");         
	case DIK_NUMPAD2:		return _T("NumPad2");         
	case DIK_NUMPAD3:		return _T("NumPad3");         
	case DIK_NUMPAD0:		return _T("NumPad0");         
	case DIK_DECIMAL:		return _T("NumPad.");           /* . on numeric keypad */
	case DIK_OEM_102:		return _T("OEM_102");           /* <> or \| on RT 102-key keyboard (Non-U.S.) */
	case DIK_F11:			return _T("F11");             
	case DIK_F12:			return _T("F12");             
	case DIK_F13:			return _T("F13");               /*                     (NEC PC98) */
	case DIK_F14:			return _T("F14");               /*                     (NEC PC98) */
	case DIK_F15:			return _T("F15");               /*                     (NEC PC98) */
	case DIK_KANA:			return _T("Kana");              /* (Japanese keyboard)            */
	case DIK_ABNT_C1:		return _T("ABNT_C1");           /* /? on Brazilian keyboard */
	case DIK_CONVERT:		return _T("Convert");           /* (Japanese keyboard)            */
	case DIK_NOCONVERT:		return _T("NoConvert");         /* (Japanese keyboard)            */
	case DIK_YEN:			return _T("YEN");               /* (Japanese keyboard)            */
	case DIK_ABNT_C2:		return _T("ABNT_C2");           /* Numpad . on Brazilian keyboard */
	case DIK_NUMPADEQUALS:	return _T("NumPad=");		    /* = on numeric keypad (NEC PC98) */
	case DIK_PREVTRACK:		return _T("PreTrack");			/* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
	case DIK_AT:			return _T("AT");                /*                     (NEC PC98) */
	case DIK_COLON:			return _T("Colon");             /*                     (NEC PC98) */
	case DIK_UNDERLINE:		return _T("UnderLine");         /*                     (NEC PC98) */
	case DIK_KANJI:			return _T("Kanji");             /* (Japanese keyboard)            */
	case DIK_STOP:			return _T("Stop");              /*                     (NEC PC98) */
	case DIK_AX:			return _T("AX");                /*                     (Japan AX) */
	case DIK_UNLABELED:		return _T("UnLabeled");         /*                        (J3100) */
	case DIK_NEXTTRACK:		return _T("NextTrack");         /* Next Track */
	case DIK_NUMPADENTER:	return _T("NumPadEnter");       /* Enter on numeric keypad */
	case DIK_RCONTROL:		return _T("R-Control");        
	case DIK_MUTE:			return _T("Mute");              /* Mute */
	case DIK_CALCULATOR:	return _T("Calculator");        /* Calculator */
	case DIK_PLAYPAUSE:		return _T("Play");              /* Play / Pause */
	case DIK_MEDIASTOP:		return _T("MediaStop");         /* Media Stop */
	case DIK_VOLUMEDOWN:	return _T("Volume-");           /* Volume - */
	case DIK_VOLUMEUP:		return _T("Volume+");			/* Volume + */
	case DIK_WEBHOME:		return _T("WebHome");           /* Web home */
	case DIK_NUMPADCOMMA:	return _T("NumPadComma");       /* , on numeric keypad (NEC PC98) */
	case DIK_DIVIDE:		return _T("NumPad/");           /* / on numeric keypad */
	case DIK_SYSRQ:			return _T("PrintScreen");           
	case DIK_RMENU:			return _T("R-Alt");             /* right Alt */
	case DIK_PAUSE:			return _T("Pause");             /* Pause */
	case DIK_HOME:			return _T("Home");              /* Home on arrow keypad */
	case DIK_UP:			return _T("Up");                /* UpArrow on arrow keypad */
	case DIK_PRIOR:			return _T("PageUp");            /* PgUp on arrow keypad */
	case DIK_LEFT:			return _T("Left");              /* LeftArrow on arrow keypad */
	case DIK_RIGHT:			return _T("Right");             /* RightArrow on arrow keypad */
	case DIK_END:			return _T("End");               /* End on arrow keypad */
	case DIK_DOWN:			return _T("Down");              /* DownArrow on arrow keypad */
	case DIK_NEXT:			return _T("PageDown");          /* PgDn on arrow keypad */
	case DIK_INSERT:		return _T("Insert");            /* Insert on arrow keypad */
	case DIK_DELETE:		return _T("Delete");            /* Delete on arrow keypad */
	case DIK_LWIN:			return _T("L-Windows");         /* Left Windows key */
	case DIK_RWIN:			return _T("R-Windows");         /* Right Windows key */
	case DIK_APPS:			return _T("AppMenu");           /* AppMenu key */
	case DIK_POWER:			return _T("Power");             /* System Power */
	case DIK_SLEEP:			return _T("Sleep");             /* System Sleep */
	case DIK_WAKE:			return _T("Wake");              /* System Wake */
	case DIK_WEBSEARCH:		return _T("WebSearch");         /* Web Search */
	case DIK_WEBFAVORITES:	return _T("WebFavorites");      /* Web Favorites */
	case DIK_WEBREFRESH:	return _T("WebRefresh");        /* Web Refresh */
	case DIK_WEBSTOP:		return _T("WebStop");           /* Web Stop */
	case DIK_WEBFORWARD:	return _T("WebForward");        /* Web Forward */
	case DIK_WEBBACK:		return _T("WebBack");           /* Web Back */
	case DIK_MYCOMPUTER:	return _T("MyComputer");        /* My Computer */
	case DIK_MAIL:			return _T("Mail");              /* Mail */
	case DIK_MEDIASELECT:	return _T("MediaSelect");       /* Media Select */
	default:				return _T("Unknow");
	}
}



} // namespace vEngine {
