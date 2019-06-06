#pragma once

#define WIN32_LEAN_AND_MEAN

#pragma warning(disable:4311)
#pragma warning(disable:4996)
#pragma warning(disable:4273)
#include <windows.h>
#include <ctype.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <process.h>	// for _beginthreadex
#include <tchar.h>
#include <mmsystem.h> //timeGetTime
#include <assert.h>

#include "wx/wx.h"
#include "wx/busyinfo.h"
#include "wx/panel.h"
#include "wx/bookctrl.h"
#include "wx/listbox.h"
#include "wx/checklst.h"
#include "wx/dataview.h"
#include "wx/renderer.h"
#include "wx/timer.h"

#include "common.h"

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <list>

#include "net_command_manager.h"
#include "stream_transport.h"

//tinyxml
#include "tinyxml.h"

#include "../WorldDefine/time.h"

//log4cplus
//#include "log4cplus/logger.h"
//#include "log4cplus/fileappender.h"
//using namespace log4cplus;

//#ifdef _DEBUG
//#pragma comment(lib,"log4cplusUSD.lib")
//#else
//#pragma comment(lib,"log4cplusUS.lib")
//#endif

//logºê
//#define LOG(x) LOG4CPLUS_DEBUG(Logger::getRoot(), x);
#define LOG(x)	0;

#ifdef _DEBUG
#pragma comment(lib,"wxbase29ud.lib")
#pragma comment(lib,"wxmsw29ud_core.lib")
#pragma comment(lib,"wxmsw29ud_adv.lib")
#else
#pragma comment(lib,"wxbase29u.lib")
#pragma comment(lib,"wxmsw29u_core.lib")
#pragma comment(lib,"wxmsw29u_adv.lib")
#endif

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"rpcrt4.lib")
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"oleacc.lib")
#pragma comment(lib,"Ws2_32.lib")

