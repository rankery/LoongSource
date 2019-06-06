//-----------------------------------------------------------------------------
//!\file db.h
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 数据库类 跨大区合并时Login数据库控制
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Singleton.h"
#include "Loong_CommonDef.h"

//-----------------------------------------------------------------------------
// 数据库类
//-----------------------------------------------------------------------------
class CLoongLoginDBCtrl : public Singleton<CLoongLoginDBCtrl>
{
public:
    //-------------------------------------------------------------------------
	// CONSTURCT
	//-------------------------------------------------------------------------
	CLoongLoginDBCtrl() { }
	~CLoongLoginDBCtrl() { Destroy(); }

    //-------------------------------------------------------------------------
	// 初始化和销毁
	//-------------------------------------------------------------------------
    BOOL	Init(tagConfigDetail stConfig[EM_DB_TYPE_END]);
    VOID	Destroy();
   
	//-------------------------------------------------------------------------
	// 各种数据库操作函数
	//-------------------------------------------------------------------------	
	BOOL LoginDB_Accout_Insert(std::vector <tagTWrap_Longin_Account*> &vecLoginAccount,EDBType eDBType);
	BOOL LoginDB_Accout_Insert(std::vector <tagTWrap_Longin_WorldForbid*> &vecLoginWorldForbid,EDBType eDBType);
	BOOL LoginDB_Accout_Insert(std::vector <tagTWrap_Longin_BlackList*> &vecLoginWorldForbid,EDBType eDBType);
	BOOL LoginDB_Accout_Insert(std::vector <tagTWrap_Longin_FatigueTime*> &vecLoginWorldForbid,EDBType eDBType);

	BOOL LoginDB_Accout_Query(std::vector <tagTWrap_Longin_Account*> &vecLoginAccount,EDBType eDBType);
	BOOL LoginDB_Accout_Query(std::vector <tagTWrap_Longin_WorldForbid*> &vecLoginAccount,EDBType eDBType);
	BOOL LoginDB_Accout_Query(std::vector <tagTWrap_Longin_BlackList*> &vecLoginAccount,EDBType eDBType);
	BOOL LoginDB_Accout_Query(std::vector <tagTWrap_Longin_FatigueTime*> &vecLoginAccount,EDBType eDBType);
public:
	//得到account表最大序列号
	DWORD GetAccountMaxNum(EDBType eDBType);

private:

	DataBase		m_LoginDB[EM_DB_TYPE_END];	// Beton数据库

};

//Beton回调函数
VOID LoginDBCallBack(DataBase* pDB, INT nReason, INT nParam);

#define sLoginBeton CLoongLoginDBCtrl::getSingleton()







