//-----------------------------------------------------------------------------
//!\file db.cpp
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 数据库类
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../ServerDefine/msg_rt_errorcode.h"
#include "db.h"
#include "player.h"
#include "login_server.h"
#include "../ServerDefine/fatigue_define.h"
#include "../WorldDefine/time.h"
//-------------------------------------------------------------------------------
// 初始化函数
//-------------------------------------------------------------------------------
BOOL DB::Init()
{
	// 从配置文件初始化数据库引擎
	TObjRef<VarContainer> pVar = "LoginServerConfig";

	// 初始化数据库
	BOOL bRet = m_DB.Init(pVar->GetString(_T("ip database")), 
							pVar->GetString(_T("user database")),
							pVar->GetString(_T("psd database")), 
							pVar->GetString(_T("name database")),
							(INT)pVar->GetDword(_T("port database")));

	//世界状态db更新时间（毫秒）
	m_dwWorldStateUpdateTime = pVar->GetDword(_T("update_time world_state"));

	//世界状态log插入间隔时间（毫秒）
	m_dwWorldStateInsertTime = pVar->GetDword(_T("insert_time world_state"));

	// 设置报警回调函数
	m_DB.SetWarningCallBack((WARNINGCALLBACK)DBCallBack);

	return bRet;
}

//-------------------------------------------------------------------------------
// 销毁函数
//-------------------------------------------------------------------------------
VOID DB::Destroy()
{
}

//--------------------------------------------------------------------------------
// 玩家登入
//--------------------------------------------------------------------------------
VOID DB::PlayerLogin(DWORD dwAccountID, DWORD dwWorldNameCrc)
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return;

	pStream->SetUpdate("account");
	pStream->FillString("login_status=") << EPLS_Loging;
	pStream->FillString(",worldname_crc=") << dwWorldNameCrc;
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;

	m_DB.Execute(pStream);

	m_DB.ReturnStream(pStream);
}

//---------------------------------------------------------------------------------
// 玩家登入
//---------------------------------------------------------------------------------
VOID DB::PlayerLogout(DWORD dwAccountID)
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return;

	pStream->SetUpdate("account");
	pStream->FillString("login_status=") << EPLS_OffLine;
	pStream->FillString(",worldname_crc=") << (DWORD)GT_INVALID;
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;
	pStream->FillString(" and login_status=") << EPLS_Loging;

	m_DB.Execute(pStream);

	m_DB.ReturnStream(pStream);
}

//---------------------------------------------------------------------------------
// 玩家登入游戏世界
//---------------------------------------------------------------------------------
VOID DB::PlayerEnterWorld(DWORD dwAccountID, DWORD dwWorldNameCrc)
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return;

	pStream->SetUpdate("account");
	pStream->FillString("login_status=") << EPLS_Online;
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;
	pStream->FillString(" and worldname_crc=") << dwWorldNameCrc;

	m_DB.Execute(pStream);

	m_DB.ReturnStream(pStream);
}

//---------------------------------------------------------------------------------
// 玩家登出游戏世界
//---------------------------------------------------------------------------------
VOID DB::PlayerOutWorld(DWORD dwAccountID, DWORD dwWorldNameCrc)
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return;

	pStream->SetUpdate("account");
	pStream->FillString("login_status=") << EPLS_OffLine;
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;
	pStream->FillString(" and worldname_crc=") << dwWorldNameCrc;

	m_DB.Execute(pStream);

	m_DB.ReturnStream(pStream);
}

//---------------------------------------------------------------------------------
// 修正玩家的登入标志
//---------------------------------------------------------------------------------
VOID DB::FixPlayerLoginStatus( DWORD dwAccountID, EPlayerLoginStatus eDest )
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return;

	pStream->SetUpdate("account");
	pStream->FillString("login_status=") << eDest;
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;

	m_DB.Execute(pStream);

	m_DB.ReturnStream(pStream);
}

//---------------------------------------------------------------------------------
// 重启时重置所有玩家的登入标志
//---------------------------------------------------------------------------------
VOID DB::ResetAllPlayerLoginStatus(EPlayerLoginStatus eSrc, EPlayerLoginStatus eDest)
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return;

	// 将登陆中状态全部改变成已登入状态，这样就可以修正因为login当机引发的异常
	pStream->SetUpdate("account");
	pStream->FillString("login_status=") << eDest;
	pStream->SetWhere();
	pStream->FillString("login_status=") << eSrc;

	m_DB.Execute(pStream);

	m_DB.ReturnStream(pStream);
}
//---------------------------------------------------------------------------------
// 改变指定游戏世界玩家的登录状态
//---------------------------------------------------------------------------------
VOID DB::FixOneWorldPlayerLoginStatus( DWORD dwWorldNameCrc, EPlayerLoginStatus eSrc, EPlayerLoginStatus eDest )
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return;

	pStream->SetUpdate("account");
	pStream->FillString("login_status=") << eDest;
	pStream->SetWhere();
	pStream->FillString("worldname_crc=")<<dwWorldNameCrc;
	if (eSrc != EPLS_Null)
	{
		pStream->FillString(" and login_status=") << eSrc;
	}

	m_DB.Execute(pStream);

	m_DB.ReturnStream(pStream);
}
//---------------------------------------------------------------------------------
// 得到某个玩家的密保
//---------------------------------------------------------------------------------
BOOL DB::GetPlayerMibao(DWORD dwAccountID, CHAR szMibao[MIBAO_LEN])
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 构建sql
	pStream->SetSelect("account", "mibao");
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;

	QueryResult* pResult = m_DB.Query(pStream);

	m_DB.ReturnStream(pStream);

	// 查询结果为空
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return FALSE;

	BOOL bRet = (*pResult)[0].GetBlob(szMibao, MIBAO_LEN);
	m_DB.FreeQueryResult(pResult);
	return bRet;
}
//-------------------------------------------------------------------------
// 插入世界状态记录
//-------------------------------------------------------------------------
BOOL DB::InsertWorldState(DWORD dwWorldID)
{

	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	//查询用户名,密码, 登录状态
	Connection* pCon = m_DB.GetFreeConnection();

	char szDate[X_DATATIME_LEN + 1];
	ZeroMemory(szDate, sizeof(szDate));
	DwordTime2DataTime(szDate, X_DATATIME_LEN + 1, GetCurrentDWORDTime());


	pStream->SetInsert("world_state");
	pStream->FillString("worldid=")<<dwWorldID;
	pStream->FillString(",rolenum=")<<0;
	pStream->FillString(",worldstate=")<<-1;
	pStream->FillString(",time='").FillString(szDate, pCon).FillString("'");

	m_DB.ReturnConnection(pCon);
	// 查询
	BOOL bRet = m_DB.Execute(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	return bRet;
}
//----------------------------------------------------------------------------
// 插入世界状态log
//----------------------------------------------------------------------------
BOOL DB::InsertWorldStateLog(DWORD dwWorldID,INT iRoleNum, SHORT sState)
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	//查询用户名,密码, 登录状态
	Connection* pCon = m_DB.GetFreeConnection();

	char szDate[X_DATATIME_LEN + 1];
	ZeroMemory(szDate, sizeof(szDate));
	DwordTime2DataTime(szDate, X_DATATIME_LEN + 1, GetCurrentDWORDTime());

	pStream->SetInsert("world_state_log");
	pStream->FillString("worldid=")<<dwWorldID;
	pStream->FillString(",rolenum=")<<iRoleNum;
	pStream->FillString(",worldstate=")<<sState;
	pStream->FillString(",time='").FillString(szDate, pCon).FillString("'");


	m_DB.ReturnConnection(pCon);

	// 查询
	BOOL bRet = m_DB.Execute(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	return bRet;
}
//----------------------------------------------------------------------------
// 清空世界状态表
//----------------------------------------------------------------------------
BOOL DB::ClearWorldStateTable()
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->Clear();
	pStream->FillString("TRUNCATE TABLE world_state");

	// 查询
	BOOL bRet = m_DB.Execute(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	return bRet;

}
//----------------------------------------------------------------------------
// 更新世界状态
//----------------------------------------------------------------------------
BOOL DB::UpdateWorldState(DWORD dwWorldID,INT iRoleNum, SHORT sState)
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	//查询用户名,密码, 登录状态
	Connection* pCon = m_DB.GetFreeConnection();

	char szDate[X_DATATIME_LEN + 1];
	ZeroMemory(szDate, sizeof(szDate));
	DwordTime2DataTime(szDate, X_DATATIME_LEN + 1, GetCurrentDWORDTime());

	pStream->SetUpdate("world_state");
	pStream->FillString("rolenum=")<<iRoleNum;
	pStream->FillString(",worldstate=")<<sState;
	pStream->FillString(",time='").FillString(szDate, pCon).FillString("'");
	pStream->SetWhere();
	pStream->FillString("worldid=")<<dwWorldID;


	m_DB.ReturnConnection(pCon);

	// 查询
	BOOL bRet = m_DB.Execute(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	return bRet;
}
////-------------------------------------------------------------------------------
//// 从数据库中查询玩家数据
////-------------------------------------------------------------------------------
//BOOL DB::QueryPlayerData(Player* pPlayer)
//{
//	if (!P_VALID(pPlayer))
//		return FALSE;
//	tagPlayerData* pData = pPlayer->GetPlayerData();
//	if(!P_VALID(pData))
//		return FALSE;
//
//	// 获取流
//	MyStream* pStream = m_DB->GetStream();
//	if (!P_VALID(pStream))
//		return FALSE;
//
//	//查询用户名,密码, 登录状态
//	Connection* pCon = m_DB->GetFreeConnection();
//
//	pStream->SetSelect( "account", "id, password_crc, disabled, privilege,login_status, logout, online_time, guard, guard_accolsec, guard_acculsec, seal_time, seal_code" );
//	pStream->SetWhere();
//	pStream->FillString("name='").FillString(pData->szAccountName, pCon).FillString("'");
//
//	m_DB->ReturnConnection(pCon);
//
//	QueryResult* pResult = m_DB->Query(pStream);
//
//	// 释放流
//	m_DB->ReturnStream(pStream);
//
//	//查询结果为空
//	if(!P_VALID(pResult))
//		return FALSE;
//	
//	//用数据库查询数据初始化 player
//	pData->dwAccountID	= (*pResult)[0].GetDword();
//	pData->dwPasswordCrc = (*pResult)[1].GetDword();
//	pData->bDisabled	= (*pResult)[2].GetBool();
//	pData->byPrivilege	= (*pResult)[3].GetByte();
//	pData->eLoginStatus = (EPlayerLoginStatus)(*pResult)[4].GetInt();
//	pData->dwLastLogoutTime = (*pResult)[5].GetDword();
//	pData->dwOnlineTime = (*pResult)[6].GetDword();
//	pData->bGuard		= (*pResult)[7].GetBool();
//	pData->nAccOLSec	= (*pResult)[8].GetDword();
//	pData->dwAccULSec	= (*pResult)[9].GetDword();
//	pData->dwtSealTime	= (*pResult)[10].GetDword();
//	pData->u16SealCode	= (*pResult)[11].GetShort();
//
//	//释放 Result
//	m_DB->FreeQueryResult( pResult );
//	return TRUE;
//}
//
////-------------------------------------------------------------------------------
//// 更新数据库中玩家数据
////-------------------------------------------------------------------------------
//BOOL DB::UpdatePlayerData(Player* pPlayer)
//{
//	if (!P_VALID(pPlayer))
//		return FALSE;
//	tagPlayerData* pData = pPlayer->GetPlayerData();
//	if(!P_VALID(pData))
//		return FALSE;
//
//	MyStream* pStream = m_DB->GetStream();
//
//	if (!P_VALID(pStream))
//		return FALSE;
//
//	// 格式化SQL语句
//	Connection* pCon = m_DB->GetFreeConnection();
//		
//	pStream->SetUpdate("account");
//	pStream->FillString("login_status=") << pData->eLoginStatus;
//	pStream->FillString(",worldname_crc=") << pData->dwWorldNameCrc;
//	pStream->FillString(",ip='").FillString( pData->szIP ).FillString("'");
//	pStream->FillString(",ip='").FillString(pData->szIP, pCon).FillString("',");
//	pStream->FillString("guard_accolsec=") << pData->dwTotalGuardTime;
//	pStream->SetWhere();
//	pStream->FillString("name='").FillString(pData->szAccountName, pCon).FillString("'");
//
//	m_DB->ReturnConnection(pCon);
//
//	// 执行
//	BOOL bRet = m_DB->Execute(pStream);
//	m_DB->ReturnStream(pStream);
//	return bRet;
//}
//

//-------------------------------------------------------------------------------
// 更新account表本次登录的时间和ip
//-------------------------------------------------------------------------------
VOID DB::UpdateAccountLoginInfo( DWORD dwAccountID, DWORD &dwIP)
{
	MyStream* pStream = m_DB.GetStream();

	if (!P_VALID(pStream))
		return;

	//格式化SQL语句
	Connection* pCon = m_DB.GetFreeConnection();

	TObjRef<StreamTransport> pTrans;
	pTrans = "StreamTransport";

	char szDate[X_DATATIME_LEN + 1];
	ZeroMemory(szDate, sizeof(szDate));
	DwordTime2DataTime(szDate, X_DATATIME_LEN + 1, GetCurrentDWORDTime());

	//
	pStream->SetUpdate("account");
	pStream->FillString("ip='").FillString(pTrans->IP2String(dwIP), pCon).FillString("',");
	pStream->FillString("time='").FillString(szDate, pCon).FillString("'");
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;


	BOOL bRet = m_DB.Execute( pStream );

	m_DB.ReturnConnection(pCon);	 
	 // 释放流
 	m_DB.ReturnStream(pStream);

}

//-------------------------------------------------------------------------------
// 将Log插入数据库
//-------------------------------------------------------------------------------
VOID DB::LogPlayerAction( DWORD dwAccountID, LPCSTR szAccountName, DWORD dwIP, LPCSTR szAction )
{
	MyStream* pStream = m_DB.GetStream();

	if (!P_VALID(pStream))
		return;

	//格式化SQL语句
	Connection* pCon = m_DB.GetFreeConnection();

	TObjRef<StreamTransport> pTrans;
	pTrans = "StreamTransport";

	char szDate[X_DATATIME_LEN + 1];
	ZeroMemory(szDate, sizeof(szDate));
	DwordTime2DataTime(szDate, X_DATATIME_LEN + 1, GetCurrentDWORDTime());

	pStream->SetInsert("login_log");
	pStream->FillString("accountID=") << dwAccountID;
	pStream->FillString(",accountName='").FillString(szAccountName, pCon).FillString("'");
	pStream->FillString(",ip='").FillString(pTrans->IP2String(dwIP), pCon).FillString("'");
	pStream->FillString(",action='").FillString(szAction, pCon).FillString("'");
	pStream->FillString(",time='").FillString(szDate, pCon).FillString("'");

	m_DB.ReturnConnection(pCon);
	m_DB.AddQuery(pStream);
}


//-------------------------------------------------------------------------------
// 将KickLog插入数据库
//-------------------------------------------------------------------------------
VOID DB::InsertKickLog( const CHAR* pAccountName, DWORD dwAccountID, DWORD dwTime, UINT16 u16ErrCode, BOOL bSeal )
{
	MyStream* pStream = m_DB.GetStream();
	
	if (!P_VALID(pStream))
		return;

	//格式化SQL语句
	Connection* pCon = m_DB.GetFreeConnection();

	if (!P_VALID(pCon))
	{
		return;
	}

	char szDate[X_DATATIME_LEN + 1];
	ZeroMemory(szDate, sizeof(szDate));
	DwordTime2DataTime(szDate, X_DATATIME_LEN + 1, dwTime);

	pStream->SetInsert("game_guarder_log");
	pStream->FillString(" account_id=") << dwAccountID;
	pStream->FillString(", name='").FillString(pAccountName, pCon).FillString("'");
	pStream->FillString(" ,kick_time='").FillString(szDate, pCon).FillString("'");
	pStream->FillString(" ,seal=") << bSeal;
	pStream->FillString(" ,error_code=") << u16ErrCode;

	m_DB.ReturnConnection(pCon);
	m_DB.AddQuery(pStream);
}
//-------------------------------------------------------------------------------
// 看指定游戏世界是否被封停
//-------------------------------------------------------------------------------
BOOL DB::IfWorldForbid(DWORD dwAccountID,DWORD dwWorldNameCrc)
{
	MyStream* pStream = m_DB.GetStream();

	if (!P_VALID(pStream))
		return FALSE;	

	//检查指定游戏世界是否被封了
	pStream->SetSelect("world_forbid","*");
	pStream->SetWhere();
	pStream->FillString("accountid=") << dwAccountID;
	pStream->FillString(" AND worldname_crc=") << dwWorldNameCrc;

	QueryResult* pResult = m_DB.Query( pStream );
	//查询结果为空
	if(!P_VALID(pResult) || pResult->GetRowCount() <= 0)
	{
		m_DB.ReturnStream(pStream);
		return FALSE;
	}

	m_DB.ReturnStream(pStream);
	return TRUE;
}
//-------------------------------------------------------------------------------
// 看指定账号是否存在被封停游戏世界
//-------------------------------------------------------------------------------
BOOL DB::IfHaveWorldForbid(DWORD dwAccountID)
{
	MyStream* pStream = m_DB.GetStream();

	if (!P_VALID(pStream))
		return FALSE;	

	//检查指定游戏世界是否被封了
	pStream->SetSelect("world_forbid","*");
	pStream->SetWhere();
	pStream->FillString("accountid=") << dwAccountID;

	QueryResult* pResult = m_DB.Query( pStream );
	//查询结果为空
	if(!P_VALID(pResult) || pResult->GetRowCount() <= 0)
	{
		m_DB.FreeQueryResult( pResult );
		m_DB.ReturnStream(pStream);
		return FALSE;
	}
	m_DB.FreeQueryResult( pResult );
	m_DB.ReturnStream(pStream);
	return TRUE;
}
//-------------------------------------------------------------------------------
// 看指定ip是不是被封了
//-------------------------------------------------------------------------------
BOOL DB::IfIPForbid(DWORD dwIP)
{
	MyStream* pStream = m_DB.GetStream();

	if (!P_VALID(pStream))
		return FALSE;

	TObjRef<StreamTransport> pTrans;
	pTrans = "StreamTransport";
	QueryResult* pResult = NULL;

	__try{

		char szIP[X_IP_LEN] = {_T('\0')};
		strcpy(szIP,pTrans->IP2String(dwIP));

		//查询
		pStream->SetSelect("black_list","*");
		pStream->SetWhere();
		pStream->FillString("ip = '").FillString(szIP).FillString("'");

		pResult = m_DB.Query(pStream);
	}
	__except(1)//continue
	{		
		ILOG->Write(_T("the DB::IfIPForbid method have exception when query ip:%s\r\n",szIP));

		m_DB.FreeQueryResult( pResult );
		m_DB.ReturnStream(pStream);
		return FALSE;
	}	

	//查询结果为空
	if(!P_VALID(pResult) || pResult->GetRowCount() <= 0)
	{
		m_DB.FreeQueryResult( pResult );
		m_DB.ReturnStream(pStream);
		return FALSE;
	}

	m_DB.FreeQueryResult( pResult );
	m_DB.ReturnStream(pStream);
	return TRUE;
}

//-------------------------------------------------------------------------------
// 解封
//-------------------------------------------------------------------------------
DWORD DB::RemoveAccountForbid( LPCTSTR szAccountName, DWORD dwForbidM,DWORD dwWorldNameCrc)
{
	MyStream* pStream = m_DB.GetStream();

	if (!P_VALID(pStream))
		return E_RT_Error;

	//检查逻辑
	pStream->SetSelect("account","id,forbid_mask");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szAccountName).FillString("'");

	QueryResult* pResult = m_DB.Query( pStream );
	//查询结果为空
	if(!P_VALID(pResult) || pResult->GetRowCount() <= 0)
	{
		m_DB.ReturnStream(pStream);
		return E_Login_GMServer_NoAccount;
	}
	//账号id
	DWORD dwAccountID = (*pResult)[0].GetDword();
	//要保证原来的标志位为1
	if(  ( (*pResult)[1].GetShort() & dwForbidM ) != dwForbidM )
	{
		//如果是gm
		if(dwForbidM & EPLM_GMTool)
		{
			//如果对应游戏世界没有被封停
			if(!IfWorldForbid(dwAccountID,dwWorldNameCrc))
			{
				m_DB.FreeQueryResult( pResult );
				m_DB.ReturnStream(pStream);
				return E_Login_GMServer_Freed;
			}
		}
		else
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_Login_GMServer_Freed;
		}
	}
	//如果是GM工具封号 需要更新world_forbid表
	if(dwForbidM & EPLM_GMTool)
	{
		pStream->SetDelete("world_forbid");
		pStream->SetWhere();
		pStream->FillString("accountid=")<<dwAccountID;
		pStream->FillString(" AND");
		pStream->FillString(" worldname_crc=")<<dwWorldNameCrc;

		BOOL bRet = m_DB.Execute(pStream);
		if(bRet == false)
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_RT_Error;
		}
		//如果是gm工具封停指定游戏世界 只要当该账号所有游戏世界都被解封 才异或标志位

		//还存在被封停的游戏世界
		if(IfHaveWorldForbid(dwAccountID))
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_RT_Success;
		}
	}
	//异或一下
	pStream->SetUpdate("account");
	pStream->FillString("forbid_mask = ") << dwForbidM;
	pStream->FillString("^forbid_mask");
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;

	BOOL bRet = m_DB.Execute(pStream);
	if(bRet == FALSE)
	{
		m_DB.FreeQueryResult( pResult );
		m_DB.ReturnStream(pStream);
		return E_RT_Error;
	}
	m_DB.FreeQueryResult( pResult );
	m_DB.ReturnStream(pStream);

	//记个log
	sBeton.InsertKickLog("EPLM_RemoveForbid", dwAccountID, GetCurrentDWORDTime(), dwForbidM, FALSE);

	return E_RT_Success;
}
DWORD DB::RemoveAccountForbid( DWORD dwAccountID, DWORD dwForbidM,DWORD dwWorldNameCrc)
{
	MyStream* pStream = m_DB.GetStream();

	if (!P_VALID(pStream))
		return E_RT_Error;

	//检查逻辑
	pStream->SetSelect("account","forbid_mask");
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;

	QueryResult* pResult = m_DB.Query( pStream );
	//查询结果为空
	if(!P_VALID(pResult) || pResult->GetRowCount() <= 0)
	{
		m_DB.ReturnStream(pStream);
		return E_Login_GMServer_NoAccount;
	}
	//要保证原来的标志位为1
	if(  ( (*pResult)[0].GetShort() & dwForbidM ) != dwForbidM )
	{
		//如果是gm
		if(dwForbidM & EPLM_GMTool)
		{
			//如果对应游戏世界没有被封停
			if(!IfWorldForbid(dwAccountID,dwWorldNameCrc))
			{
				m_DB.FreeQueryResult( pResult );
				m_DB.ReturnStream(pStream);
				return E_Login_GMServer_Freed;
			}
		}
		else
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_Login_GMServer_Freed;
		}
	}
	//如果是GM工具封号 需要更新world_forbid表
	if(dwForbidM & EPLM_GMTool)
	{
		pStream->SetDelete("world_forbid");
		pStream->SetWhere();
		pStream->FillString("accountid=")<<dwAccountID;
		pStream->FillString(" AND");
		pStream->FillString(" worldname_crc=")<<dwWorldNameCrc;

		BOOL bRet = m_DB.Execute(pStream);
		if(bRet == false)
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_RT_Error;
		}
		//还存在被封停的游戏世界
		if(IfHaveWorldForbid(dwAccountID))
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_RT_Success;
		}
	}
	//异或一下
	pStream->SetUpdate("account");
	pStream->FillString("forbid_mask = ") << dwForbidM;
	pStream->FillString("^forbid_mask");
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;

	BOOL bRet = m_DB.Execute(pStream);
	if(bRet == FALSE)
	{
		m_DB.FreeQueryResult( pResult );
		m_DB.ReturnStream(pStream);
		return E_RT_Error;
	}



	m_DB.FreeQueryResult( pResult );
	m_DB.ReturnStream(pStream);

	//记个log
	sBeton.InsertKickLog("EPLM_RemoveForbid", dwAccountID, GetCurrentDWORDTime(), dwForbidM, FALSE);

	return E_RT_Success;
}

DWORD DB::SetAccountChenMi( DWORD dwAccountID, DWORD dwChenMi,DWORD dwWorldNameCrc)
{
	MyStream* pStream = m_DB.GetStream();

	if (!P_VALID(pStream))
		return E_RT_Error;

	pStream->SetUpdate("account");
	pStream->FillString("guard=") << (BYTE)dwChenMi;
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;

	BOOL bRtv = m_DB.Execute(pStream);
	m_DB.ReturnStream(pStream);

	return bRtv ? E_RT_Success : E_RT_Error;
}

//-------------------------------------------------------------------------------
// 封号
//-------------------------------------------------------------------------------
DWORD DB::ForbidAccount( LPCTSTR szAccountName, DWORD dwForbidM,DWORD &dwAccountID,DWORD dwWorldNameCrc)
{
	MyStream* pStream = m_DB.GetStream();
	dwAccountID = GT_INVALID;

	if (!P_VALID(pStream))
		return E_RT_Error;

	//检查逻辑
	pStream->SetSelect("account","id,forbid_mask");
	pStream->SetWhere();
	pStream->FillString("name='").FillString(szAccountName).FillString("'");

	QueryResult* pResult = m_DB.Query( pStream );
	//查询结果为空
	if(!P_VALID(pResult) || pResult->GetRowCount() <= 0)
	{
		m_DB.ReturnStream(pStream);
		return E_Login_GMServer_NoAccount;
	}
	//账号id
	dwAccountID = (*pResult)[0].GetDword();
	SHORT sForbidMask = (*pResult)[1].GetShort();

	//要保证原来的对应标志位为0
	if(  ( sForbidMask & dwForbidM ) != 0 )
	{
		//如果是gm
		if(dwForbidM & EPLM_GMTool)
		{
			//如果对应游戏世界已经被封停
			if(IfWorldForbid(dwAccountID,dwWorldNameCrc))
			{
				m_DB.FreeQueryResult( pResult );
				m_DB.ReturnStream(pStream);
				return E_Login_GMServer_Forbidded;
			}
		}
		else
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_Login_GMServer_Forbidded;
		}
	}


	//如果是GM工具封号 需要更新world_forbid表
	if(dwForbidM & EPLM_GMTool)
	{
		pStream->SetInsert("world_forbid");
		pStream->FillString("accountid=")<<dwAccountID;
		pStream->FillString(",worldname_crc=")<<dwWorldNameCrc;

		BOOL bRet = m_DB.Execute(pStream);
		if(bRet == FALSE)
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_RT_Error;
		}
		//如果之前标志位已经被置上了
		if(sForbidMask & EPLM_GMTool)
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_RT_Success;
		}
	}
	//异或一下
	pStream->SetUpdate("account");
	pStream->FillString("forbid_mask = ") << dwForbidM;
	pStream->FillString("^forbid_mask");
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;

	BOOL bRet = m_DB.Execute(pStream);
	if(bRet == FALSE)
	{
		m_DB.FreeQueryResult( pResult );
		m_DB.ReturnStream(pStream);
		return E_RT_Error;
	}


	m_DB.FreeQueryResult( pResult );
	m_DB.ReturnStream(pStream);

	//记个log
	sBeton.InsertKickLog("EPLM_Forbid", dwAccountID, GetCurrentDWORDTime(), dwForbidM, TRUE);

	return E_RT_Success;
}

DWORD DB::ForbidAccount( DWORD dwAccountID, DWORD dwForbidM,DWORD dwWorldNameCrc)
{
	MyStream* pStream = m_DB.GetStream();

	if (!P_VALID(pStream))
		return E_RT_Error;

	//检查逻辑
	pStream->SetSelect("account","forbid_mask");
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;

	QueryResult* pResult = m_DB.Query( pStream );
	//查询结果为空
	if(!P_VALID(pResult) || pResult->GetRowCount() <= 0)
	{
		m_DB.ReturnStream(pStream);
		return E_Login_GMServer_NoAccount;
	}
	SHORT sForbidMask = (*pResult)[0].GetShort();
	//要保证原来的对应标志位为0
	if(  ( sForbidMask & dwForbidM ) != 0 )
	{
		//如果是gm
		if(dwForbidM & EPLM_GMTool)
		{
			//如果对应游戏世界已经被封停
			if(IfWorldForbid(dwAccountID,dwWorldNameCrc))
			{
				m_DB.FreeQueryResult( pResult );
				m_DB.ReturnStream(pStream);
				return E_Login_GMServer_Forbidded;
			}
		}
		else
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_Login_GMServer_Forbidded;
		}
	}
	//如果是GM工具封号 需要更新world_forbid表
	if(dwForbidM & EPLM_GMTool)
	{
		pStream->SetInsert("world_forbid");
		pStream->FillString("accountid=")<<dwAccountID;
		pStream->FillString(",worldname_crc=")<<dwWorldNameCrc;

		BOOL bRet = m_DB.Execute(pStream);
		if(bRet == FALSE)
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_RT_Error;
		}
		//如果之前标志位已经被置上了
		if(sForbidMask & EPLM_GMTool)
		{
			m_DB.FreeQueryResult( pResult );
			m_DB.ReturnStream(pStream);
			return E_RT_Success;
		}
	}
	//异或一下
	pStream->SetUpdate("account");
	pStream->FillString("forbid_mask = ") << dwForbidM;
	pStream->FillString("^forbid_mask");
	pStream->SetWhere();
	pStream->FillString("id=") << dwAccountID;

	BOOL bRet = m_DB.Execute(pStream);
	if(bRet == FALSE)
	{
		m_DB.FreeQueryResult( pResult );
		m_DB.ReturnStream(pStream);
		return E_RT_Error;
	}
	m_DB.FreeQueryResult( pResult );
	m_DB.ReturnStream(pStream);

	//记个log
	sBeton.InsertKickLog("EPLM_Forbid", dwAccountID, GetCurrentDWORDTime(), dwForbidM, TRUE);

	return E_RT_Success;
}
//-------------------------------------------------------------------------------
// 封IP
//-------------------------------------------------------------------------------
DWORD DB::ForbidIP(DWORD dwIP)
{
	MyStream* pStream = m_DB.GetStream();

	if (!P_VALID(pStream))
		return E_RT_Error;

	TObjRef<StreamTransport> pTrans;
	pTrans = "StreamTransport";

	char szIP[X_IP_LEN];
	strcpy(szIP,pTrans->IP2String(dwIP));

	//往里插
	pStream->SetInsert("black_list");
	pStream->FillString("ip = '").FillString(szIP).FillString("'");

	BOOL bRet = m_DB.Execute(pStream);
	if(bRet == FALSE)
	{
		m_DB.ReturnStream(pStream);
		return E_RT_Error;
	}

	return E_RT_Success;
}
//-------------------------------------------------------------------------------
// 解封IP
//-------------------------------------------------------------------------------
DWORD DB::RemoveIPForbid(DWORD dwIP)
{
	MyStream* pStream = m_DB.GetStream();

	if (!P_VALID(pStream))
		return E_RT_Error;

	TObjRef<StreamTransport> pTrans;
	pTrans = "StreamTransport";

	char szIP[X_IP_LEN];
	strcpy(szIP,pTrans->IP2String(dwIP));

	//往删除
	pStream->SetDelete("black_list");
	pStream->SetWhere();
	pStream->FillString("ip = '").FillString(szIP).FillString("'");

	BOOL bRet = m_DB.Execute(pStream);
	if(bRet == FALSE)
	{
		m_DB.ReturnStream(pStream);
		return E_RT_Error;
	}
	return E_RT_Success;
}

VOID DB::ResetPlayersLoginStatus( DWORD* pdwAccountIDs, INT nNum, EPlayerLoginStatus eDest )
{
	for (INT i=0; i<nNum; ++i)
	{
		FixPlayerLoginStatus(pdwAccountIDs[i], eDest);
	}
}

BOOL DB::LoadAllFatigueInfo( TMap<DWORD, tagFatigueInfo*> &mapFatigueInfo )
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 构建sql
	pStream->SetSelect("fatigue_time", 
						"accountname_crc,"			//0
						"acc_online_time,"		//1
						"acc_offline_time"	//2
						);

	QueryResult* pResult = m_DB.Query(pStream);
	m_DB.ReturnStream(pStream);

	// 查询结果为空
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return FALSE;

	tagFatigueInfo* tmp = NULL;
	INT nCount = pResult->GetRowCount();
	for (INT i=0; i<nCount; ++i)
	{
		DWORD dwAccountNameCrc = (*pResult)[0].GetDword();
		tmp = mapFatigueInfo.Peek(dwAccountNameCrc);
		if (!P_VALID(tmp))
		{
			tmp = new tagFatigueInfo;
			mapFatigueInfo.Add(dwAccountNameCrc, tmp);
		}

		tmp->dwAccountNameCrc = dwAccountNameCrc;
		tmp->nAccOnLineTimeMin = (*pResult)[1].GetInt();
		tmp->nAccOffLineTimeMin = (*pResult)[2].GetInt();

		pResult->NextRow();
	}

	m_DB.FreeQueryResult(pResult);
	return TRUE;
}

// VOID DB::SaveFatigueInfo( tagFatigueInfo* pToSave )
// {
// 	MyStream* pStream = m_DB.GetStream();
// 	if( !P_VALID(pStream) ) return ;
// 
// 	pStream->SetUpdate("fatigue_time");
// 	pStream->FillString("acc_online_time=")<<pToSave->nAccOnLineTimeMin;
// 	pStream->FillString(",acc_offline_time=")<<pToSave->nAccOffLineTimeMin;
// 	pStream->SetWhere();
// 	pStream->FillString("account_id=")<<pToSave->dwAccountID;
// 
// 
// 	m_DB.Execute(pStream);
// 
// 	m_DB.ReturnStream(pStream);
// }

// BOOL DB::LoadLoginStatus( TMap<DWORD, tagFatigueInfo*> &mapFatigueInfo )
// {
// 	MyStream* pStream = m_DB.GetStream();
// 	if( !P_VALID(pStream) ) return FALSE;
// 
// 	// 构建sql
// 	pStream->SetSelect("account", 
// 		"id, "			//0
// 		"login_status, "		//1
// 		"worldname_crc, "		//2
// 		"guard");				//3
// 
// 	QueryResult* pResult = m_DB.Query(pStream);
// 	m_DB.ReturnStream(pStream);
// 
// 	// 查询结果为空
// 	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return FALSE;
// 
// 	tagFatigueInfo* tmp = NULL;
// 	INT nCount = pResult->GetRowCount();
// 	for (INT i=0; i<nCount; ++i)
// 	{
// 		DWORD dwAccountID = (*pResult)[0].GetDword();
// 		tmp = mapFatigueInfo.Peek(dwAccountID);
// 		if (!P_VALID(tmp))
// 		{
// 			tmp = new tagFatigueInfo;
// 			mapFatigueInfo.Add(dwAccountID, tmp);
// 			tmp->dwAccountID = dwAccountID;
// 		}
// 		tmp->ePls = EPlayerLoginStatus((*pResult)[1].GetInt());
// 		tmp->dwWorldNameCrc = (*pResult)[2].GetDword();
// 		tmp->bGuard = (*pResult)[3].GetBool();
// 		pResult->NextRow();
// 	}
// 
// 	m_DB.FreeQueryResult(pResult);
// 	return TRUE;
// }

// VOID DB::UpdateFatigueTimeTable()
// {
// 	MyStream* pStream = m_DB.GetStream();
// 	if( !P_VALID(pStream) ) return ;
// 
// 	pStream->FillString("INSERT INTO fatigue_time(account_id)  (SELECT id FROM account ) on DUPLICATE key update account_id=account_id");
// 
// 	m_DB.Execute(pStream);
// 
// 	m_DB.ReturnStream(pStream);
// }

BOOL DB::LoadOnLineGuardAccountIDs( TMap<DWORD, DWORD> &mapAccounts )
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 构建sql
	pStream->SetSelect("account", "id, worldname_crc");
	pStream->SetWhere();
	pStream->FillString("login_status=1 and guard=1");

	QueryResult* pResult = m_DB.Query(pStream);
	m_DB.ReturnStream(pStream);

	// 查询结果为空
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return FALSE;

	INT nCount = pResult->GetRowCount();

	mapAccounts.Clear();
	for (INT i=0; i<nCount; ++i)
	{
		DWORD dwAccountID = (*pResult)[0].GetDword();
		DWORD dwWorldNameCrc = (*pResult)[1].GetDword();
		
		if (!GT_VALID(mapAccounts.Peek(dwAccountID)))
		{
			mapAccounts.Add(dwAccountID, dwWorldNameCrc);
		}

		pResult->NextRow();
	}

	m_DB.FreeQueryResult(pResult);
	return TRUE;
}

BOOL DB::LoadCacheAccountData( TSafeMap<DWORD, tagAccountData*> &mapAccountData, TSafeMap<DWORD, DWORD> &mapNameCrc2AccountID)
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	TObjRef<Util> pUtil;
	pUtil = "Util";
	TObjRef<StreamTransport> pTrans;
	pTrans = "StreamTransport";

	// 申请连接
	Connection* pCon = m_DB.GetFreeConnection();

	pStream->SetSelect("account", "id, name,guard,ip");

	// 释放连接
	m_DB.ReturnConnection(pCon);

	QueryResult* pResult = m_DB.Query(pStream);

	// 释放流
	m_DB.ReturnStream(pStream);

	// 查询结果为空
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return FALSE;

	DWORD dwAccountID = GT_INVALID;
	DWORD dwAccountNameCrc = GT_INVALID;
	tagAccountData* pAccountData = NULL;

	mapAccountData.Clear();
	mapNameCrc2AccountID.Clear();

	BOOL bRtv = TRUE;

	for (INT i=0; i<pResult->GetRowCount(); ++i)
	{
		// 设置结果
		pAccountData = new tagAccountData;
		if (!P_VALID(pAccountData))
		{
			bRtv = FALSE;
			break;
		}

		dwAccountID = (*pResult)[0].GetDword();

		memcpy(pAccountData->szAccountName, (*pResult)[1].GetString(), sizeof(CHAR)*16);
		pAccountData->bGuard			=	(*pResult)[2].GetBool();

		//上次登录的ip
		char szIP[X_IP_LEN] = "";
		memcpy(szIP,(*pResult)[3].GetString(),(*pResult)[3].GetLen());
		pAccountData->dwIp = pTrans->StringIP2IP(szIP);

		dwAccountNameCrc = pUtil->Crc32(pAccountData->szAccountName);

		if (!GT_VALID(mapAccountData.Peek(dwAccountID)))
		{
			mapAccountData.Add(dwAccountID, pAccountData);
		}
		if (!GT_VALID(mapNameCrc2AccountID.Peek(dwAccountNameCrc)))
		{
			mapNameCrc2AccountID.Add(dwAccountNameCrc, dwAccountID);
		}

		pResult->NextRow();
	}

	// 归还结果集
	m_DB.FreeQueryResult(pResult);

	return bRtv;

}

VOID DB::SaveFatigueInfo( tagFatigueInfo* pFatigueInfo )
{
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return ;

	pStream->SetReplace("fatigue_time");
	pStream->FillString("acc_online_time=")<<pFatigueInfo->nAccOnLineTimeMin;
	pStream->FillString(",acc_offline_time=")<<pFatigueInfo->nAccOffLineTimeMin;
	pStream->FillString(",accountname_crc=")<<pFatigueInfo->dwAccountNameCrc;

	m_DB.Execute(pStream);

	m_DB.ReturnStream(pStream);
}

BOOL DB::LoadFatigueInfo(DWORD dwAccountNameCrc, tagFatigueInfo* pFatigueInfo )
{
	if (!P_VALID(pFatigueInfo)) return FALSE;
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 构建sql
	pStream->SetSelect("fatigue_time", "acc_online_time, acc_offline_time");
	pStream->SetWhere();
	pStream->FillString("accountname_crc=") << dwAccountNameCrc;

	QueryResult* pResult = m_DB.Query(pStream);
	m_DB.ReturnStream(pStream);

	// 查询结果为空
	if( !P_VALID(pResult) || pResult->GetRowCount() != 1) return FALSE;

	pFatigueInfo->dwAccountNameCrc = dwAccountNameCrc;
	pFatigueInfo->nAccOnLineTimeMin = (*pResult)[0].GetInt();
	pFatigueInfo->nAccOffLineTimeMin = (*pResult)[1].GetInt();

	m_DB.FreeQueryResult(pResult);
	return TRUE;	
}
//
////-------------------------------------------------------------------------------
//// 当玩家成功登录游戏世界，更新数据库玩家数据
////-------------------------------------------------------------------------------
//BOOL DB::UpdatePlayerIntoWorld(EPlayerLoginStatus status, DWORD dwLoginTime, DWORD dwAccULSec, DWORD dwAccOLSec, DWORD dwAccountNameCrc)
//{
//	MyStream* pStream = m_DB->GetStream();
//
//	if (!P_VALID(pStream))
//		return FALSE;
//
//	//格式化SQL语句
//	pStream->SetUpdate("account");
//	pStream->FillString("login_status=") << status;
//	pStream->FillString(",login=") << dwLoginTime;
//	pStream->FillString(",guard_acculsec=") << dwAccULSec;
//	pStream->FillString(",guard_accolsec=") << dwAccOLSec;
//	pStream->SetWhere();
//	pStream->FillString("id='") <<dwAccountNameCrc ;
//	pStream->FillString( "'" );
//	// 执行
//	BOOL bRet = m_DB->Execute(pStream);
//	m_DB->ReturnStream(pStream);
//	return bRet;
//}
////-------------------------------------------------------------------------------
//// 当玩家离开游戏世界，更新数据库玩家数据
////-------------------------------------------------------------------------------
//BOOL DB::UpdatePlayerOutWorld(EPlayerLoginStatus status, DWORD dwLogoutTime, DWORD dwOnlineTime, DWORD dwAccOLSec, DWORD dwAccountNameCrc)
//{
//	MyStream* pStream = m_DB->GetStream();
//
//	if (!P_VALID(pStream))
//		return FALSE;
//
//	//格式化SQL语句
//	pStream->SetUpdate("account");
//	pStream->FillString("login_status=") << status;
//	pStream->FillString(",logout=") << dwLogoutTime;
//	pStream->FillString(",online_time=") << dwOnlineTime;
//	pStream->FillString(",guard_accolsec=") << dwAccOLSec;
//	pStream->SetWhere();
//	pStream->FillString("id='")<<dwAccountNameCrc ;
//	pStream->FillString("'");
//
//	// 执行
//	BOOL bRet = m_DB->Execute(pStream);
//	m_DB->ReturnStream(pStream);
//	return bRet;
//}
//
////-------------------------------------------------------------------------------
//// 当LoongWorld当掉,清空该服务器上登录的玩家
////-------------------------------------------------------------------------------
//VOID DB::UpdatePlayer_LoongWorld(EPlayerLoginStatus status, DWORD dwWorldNameCrc, DWORD dwLogoutTime)
//{
//	MyStream* pStream = m_DB->GetStream();
//
//	if (!P_VALID(pStream))
//		return;
//
//	//格式化SQL语句
//	pStream->SetUpdate("account");
//	pStream->FillString("login_status=") << status;
//	pStream->FillString(",logout=") << dwLogoutTime;
//	pStream->SetWhere();
//	pStream->FillString("worldname_crc='")<<dwWorldNameCrc ;
//	pStream->FillString( "'" );
//
//	// 执行
//	m_DB->Execute(pStream);
//	m_DB->ReturnStream(pStream);
//}
//
//
//-------------------------------------------------------------------------------
// 从数据库中查询玩家帐号名称
//-------------------------------------------------------------------------------
// tstring DB::GetAccountName(DWORD dwAccountNameCrc)
// {
// 	tstring name;
// 	// 获取流
// 	MyStream* pStream = m_DB.GetStream();
// 	if (!P_VALID(pStream))
// 		return name;
// 
// 	//查询用户名,密码, 登录状态
// 	pStream->SetSelect( "account", "name" );
// 	pStream->SetWhere().FillString("id =") << dwAccountNameCrc;
// 
// 	QueryResult* pResult = m_DB.Query( pStream );
// 
// 	// 释放流
// 	m_DB.ReturnStream(pStream);
// 
// 	//查询结果为空
// 	if(!P_VALID(pResult) || pResult->GetRowCount() <= 0)
// 		return name;
// 
// 	//用数据库查询数据初始化 player
// 	name = (*pResult)[0].GetUnicodeString();
// 
// 	//释放 Result
// 	m_DB.FreeQueryResult( pResult );
// 	return name;
// }

//------------------------------------------------------------------------------------
// 数据库预警回调函数
//------------------------------------------------------------------------------------
VOID DBCallBack(DataBase* pDB, INT nReason, INT nParam)
{
	if (nReason == DataBase::EDBE_System)
	{
		ILOG->Write(_T("Beton Warning CallBack: Reason  %s , nParam = %u\r\n"), _T("EDBE_System"), nParam);
	}
	else if(nReason == DataBase::EDBE_QueueFull)
	{
		ILOG->Write(_T("Beton Warning CallBack: Reason  %s , nParam = %u\r\n"), _T("EDBE_QueueFull"), nParam);
	}
	else if(nReason == DataBase::EDBE_PoolFull)
	{
		ILOG->Write(_T("Beton Warning CallBack: Reason  %s , nParam = %u\r\n"), _T("EDBE_PoolFull"), nParam);
	}
	else
	{
		ILOG->Write(_T("Beton Warning CallBack: Reason Unknow,nReason = %u, nParam = %u\r\n"), nReason, nParam);
	}
}