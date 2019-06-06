///-----------------------------------------------------------------------------
/// File: vEngine
/// Desc: 所有引擎对外接口
/// Auth: Lyp
/// Date: 2004/07/22
///
///-----------------------------------------------------------------------------
#pragma once
#include "stdafx.h"

#ifdef _DEBUG
	#pragma comment(lib,"..\\vsout\\vengine\\debug\\vengine.lib")
#else
	#pragma comment(lib,"..\\vsout\\vengine\\release\\vengine.lib")
#endif


// 基本逻辑模型
#include "frame\binder.h"
#include "frame\auto_factory.h"
#include "frame\sfp_trunk.h"
#include "frame\simple_task.h"
#include "frame\safe_list.h"
#include "frame\safe_map.h"
#include "frame\simple_map.h"
#include "frame\simple_list.h"
#include "frame\msg_queue_ts.h"
#include "frame\msg_queue.h"


/// 对外标准接口
#include "debug\debug.h"
#include "debug\log.h"
#include "debug\mini_dump.h"

#include "console\console_command.h"
#include "console\console_gui.h"

#include "input\input_dx8.h"
#include "input\input_map.h"

#include "memory\new_protect.h"	
#include "memory\mem_pool.h"
#include "memory\safe_mem_pool.h"

#include "network\ping.h"
#include "network\stream_server.h"
#include "network\stream_transport.h"
#include "network\broadcast.h"
#include "network\session.h"
#include "network\net_cmd_mgr.h"
#include "network\x_server.h"
#include "network\x_client.h"

#include "render\render.h"
#include "render\render.h"
#include "render\d3d_interface.h"
#include "render\gdi_interface.h"

#include "util\fastcode.h"
#include "util\util.h"

#include "system\disciomgr.h"
#include "system\sysinfo.h"
#include "system\window_wrap.h"
#include "system\thread.h"
#include "system\cpu_usage.h"
#include "system\process.h"
#include "system\system_tray.h"

#include "variable\ini_obj.h"
#include "variable\var_container.h"
#include "variable\var_mgr.h"

#include "audio\audio.h"
#include "filter\filtermgr.h"
#include "image\image_obj.h"
#include "script\lua_manager.h"
#include "vfs\vfs.h"
#include "gui\gui.h"

