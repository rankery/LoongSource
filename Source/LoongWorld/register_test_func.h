//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: register_test_func.h
// author: Sxg
// actor:
// data: 2008-12-5
// last:
// brief: 注册测试函数
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "create_db_record.h"
#include "loong_db_test.h"
//-----------------------------------------------------------------------------
VOID RegisterTestCmd(TObjRef<Console> pConsole)
{
	// 注册服务器控制台命令(测试专用:正式版本需注释掉) // need del
	pConsole->Register(_T("createitem"), CreateItem, _T("createitem nItemNum dwRoleIDStart"), 2);
	pConsole->Register(_T("createequip"), CreateEquip, _T("createequip nEquipNum dwRoleIDStart"), 2);
	pConsole->Register(_T("createrole"), CreateRole, _T("createrole nameprefix postfix num"), 3);

	pConsole->Register(_T("loadrole"), TestLoadRole, _T("loadrole namepostfix num testnum"), 3);
	pConsole->Register(_T("updaterole"), TestUpdateRole, _T("updaterole namepostfix num testnum"), 3);
	pConsole->Register(_T("loaditem"), TestLoadItem, _T("loaditem namepostfix num testnum"), 3);
	pConsole->Register(_T("updateitem"), TestUpdateItem, _T("updateitem serialstart num testnum typeid"), 4);
	pConsole->Register(_T("updateequip"), TestUpdateEquip, _T("updateequip serialstart num testnum typeid"), 4);
}