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

#include "wx/wx.h"
#include "servermergetoolapp.h"
#include "Loong_OutputCtrl.h"
#include "Loong_LoginDBCtrl.h"
#include "../WorldDefine/time.h"


//-------------------------------------------------------------------------------
// 初始化函数
//-------------------------------------------------------------------------------
BOOL CLoongLoginDBCtrl::Init(tagConfigDetail stConfig[EM_DB_TYPE_END])
{

	//初始化LoginDB
	for(int iLoop=EM_DB_SRC_ONE;iLoop<EM_DB_TYPE_END;iLoop++)
	{
		BOOL bRet = m_LoginDB[iLoop].Init(stConfig[iLoop].strIp.c_str(),
			stConfig[iLoop].strUser.c_str(),
			stConfig[iLoop].strPsd.c_str(),
			stConfig[iLoop].strDBName.c_str(),
			stConfig[iLoop].dwPort);
		if(bRet == FALSE)
		{
			tstring strErr;
			strErr += stConfig[iLoop].strIp;
			strErr += _T("		");
			strErr += stConfig[iLoop].strUser;
			strErr += _T("		");
			strErr += stConfig[iLoop].strPsd;
			strErr += _T("		");
			strErr += stConfig[iLoop].strDBName;
			strErr += _T("		");
			strErr += stConfig[iLoop].dwPort;
			strErr += _T("初始化失败！");

			ILOG->Write(strErr.c_str());
			return FALSE;
		}
	}

	// 设置报警回调函数
	for(int iLoop=EM_DB_SRC_ONE;iLoop<EM_DB_TYPE_END;iLoop++)
	{
		m_LoginDB[iLoop].SetWarningCallBack((WARNINGCALLBACK)LoginDBCallBack);
	}
	return TRUE;
}

//-------------------------------------------------------------------------------
// 销毁函数
//-------------------------------------------------------------------------------
VOID CLoongLoginDBCtrl::Destroy()
{
	for(int iLoop=EM_DB_SRC_ONE;iLoop<EM_DB_TYPE_END;iLoop++)
	{
		m_LoginDB[iLoop].ShutDown();
	}
}
//-------------------------------------------------------------------------------
//得到account表最大序列号
//-------------------------------------------------------------------------------
DWORD CLoongLoginDBCtrl::GetAccountMaxNum(EDBType eDBType)
{
	MyStream* pStream = m_LoginDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->SetSelect("account","max(id)");
	// 查询
	QueryResult* pResult = m_LoginDB[eDBType].Query(pStream);

	// 查询结果为空
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return GT_INVALID;

	return (*pResult)[0].GetDword();
}
//-------------------------------------------------------------------------
// 插入玩家封停记录
//-------------------------------------------------------------------------
BOOL CLoongLoginDBCtrl::LoginDB_Accout_Insert(std::vector <tagTWrap_Longin_WorldForbid*> &vecLoginWorldForbid,EDBType eDBType)
{
	MyStream* pStream = m_LoginDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	vector <tagTWrap_Longin_WorldForbid *>::iterator itLoop = vecLoginWorldForbid.begin();
	for(; itLoop != vecLoginWorldForbid.end(); ++itLoop)
	{
		pStream->SetInsert("world_forbid");
		pStream->FillString("accountid=")<<(*itLoop)->dwAccountID;
		pStream->FillString(",worldname_crc=")<<(*itLoop)->dwWorldNameCrc;

		// 查询
		if(!m_LoginDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}

		pStream->Clear();
	}
	// 释放流
	m_LoginDB[eDBType].ReturnStream(pStream);

	return TRUE;
}

//-------------------------------------------------------------------------
// 黑名单
//-------------------------------------------------------------------------
BOOL CLoongLoginDBCtrl::LoginDB_Accout_Insert(std::vector <tagTWrap_Longin_BlackList*> &vecLoginBlackList,EDBType eDBType)
{
	MyStream* pStream = m_LoginDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	vector <tagTWrap_Longin_BlackList *>::iterator itLoop = vecLoginBlackList.begin();
	for(; itLoop != vecLoginBlackList.end(); ++itLoop)
	{
		pStream->SetInsert("black_list");
		pStream->FillString("ip='").FillString((*itLoop)->szIP).FillString("'");

		// 查询
		if(!m_LoginDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}

		pStream->Clear();
	}
	// 释放流
	m_LoginDB[eDBType].ReturnStream(pStream);

	return TRUE;
}
//-------------------------------------------------------------------------
// 防沉迷
//-------------------------------------------------------------------------
BOOL CLoongLoginDBCtrl::LoginDB_Accout_Insert(std::vector <tagTWrap_Longin_FatigueTime*> &vecLoginFatigueTime,EDBType eDBType)
{
	MyStream* pStream = m_LoginDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	vector <tagTWrap_Longin_FatigueTime *>::iterator itLoop = vecLoginFatigueTime.begin();
	for(; itLoop != vecLoginFatigueTime.end(); ++itLoop)
	{
		pStream->SetInsert("fatigue_time");
		pStream->FillString("accountname_crc=")<<(*itLoop)->dwAccountNameCrc;
		pStream->FillString(",acc_online_time=")<<(*itLoop)->dwOnlineTime;
		pStream->FillString(",acc_offline_time=")<<(*itLoop)->dwOfflineTime;

		// 查询
		if(!m_LoginDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}

		pStream->Clear();
	}
	// 释放流
	m_LoginDB[eDBType].ReturnStream(pStream);

	return TRUE;
}
//-------------------------------------------------------------------------
// 插入玩家记录
//-------------------------------------------------------------------------
BOOL CLoongLoginDBCtrl::LoginDB_Accout_Insert(std::vector <tagTWrap_Longin_Account*> &vecLoginAccount,EDBType eDBType)
{
	MyStream* pStream = m_LoginDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 申请连接
	Connection* pCon = m_LoginDB[eDBType].GetFreeConnection();

	vector <tagTWrap_Longin_Account *>::iterator itLoop = vecLoginAccount.begin();
	for(; itLoop != vecLoginAccount.end(); ++itLoop)
	{
		pStream->SetInsert("account");
		pStream->FillString("id=")<<(*itLoop)->stOthers.dwAccountID;
		pStream->FillString(",name='").FillString((*itLoop)->szName, pCon).FillString("'");
		pStream->FillString(",psd='").FillString((*itLoop)->szPsd, pCon).FillString("'");
		pStream->FillString(",privilege=") << (*itLoop)->stOthers.byPrivilege;
		pStream->FillString(",login_status=") << (*itLoop)->stOthers.eLoginStatus;
		pStream->FillString(",worldname_crc=") << (*itLoop)->stOthers.dwWorldNameCrc;
		pStream->FillString(",forbid_mask=") << (*itLoop)->stOthers.dwFrobidMask;
		pStream->FillString(",guard=") << (*itLoop)->stOthers.bGuard;

		pStream->FillString(",mibao='").FillString((*itLoop)->szMibao, pCon).FillString("'");

		// 查询
		if(!m_LoginDB[eDBType].Execute(pStream))
		{
			return FALSE;
		}
		
		pStream->Clear();
	}
	// 释放连接
	m_LoginDB[eDBType].ReturnConnection(pCon);


	// 释放流
	m_LoginDB[eDBType].ReturnStream(pStream);

	return TRUE;
}
//-------------------------------------------------------------------------
// 查询所有用户封停记录
//-------------------------------------------------------------------------
BOOL CLoongLoginDBCtrl::LoginDB_Accout_Query(std::vector <tagTWrap_Longin_WorldForbid*> &vecLoginWorldForbid,EDBType eDBType)
{
	MyStream* pStream = m_LoginDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->SetSelect("world_forbid", "accountid,worldname_crc");

	QueryResult* pResult = m_LoginDB[eDBType].Query(pStream);

	// 释放流
	m_LoginDB[eDBType].ReturnStream(pStream);

	// 查询结果为空
	if( !P_VALID(pResult)) return FALSE;

	if(pResult->GetRowCount() == 0) return TRUE;

	do{
		tagTWrap_Longin_WorldForbid *pWorldForbid = new tagTWrap_Longin_WorldForbid;
		// 设置结果
		pWorldForbid->dwAccountID		=	(*pResult)[0].GetDword();
		pWorldForbid->dwWorldNameCrc	=	(*pResult)[1].GetDword();

		vecLoginWorldForbid.push_back(pWorldForbid);

	}while(pResult->NextRow());

	// 归还结果集
	m_LoginDB[eDBType].FreeQueryResult(pResult);

	return TRUE;
}
//-------------------------------------------------------------------------
// 黑名单
//-------------------------------------------------------------------------
BOOL CLoongLoginDBCtrl::LoginDB_Accout_Query(std::vector <tagTWrap_Longin_BlackList*> &vecLoginBlackList,EDBType eDBType)
{
	MyStream* pStream = m_LoginDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->SetSelect("black_list", "ip");

	QueryResult* pResult = m_LoginDB[eDBType].Query(pStream);

	// 释放流
	m_LoginDB[eDBType].ReturnStream(pStream);

	// 查询结果为空
	if( !P_VALID(pResult)) return FALSE;

	if(pResult->GetRowCount() == 0) return TRUE;

	do{
		tagTWrap_Longin_BlackList *pBlackList = new tagTWrap_Longin_BlackList;
		// 设置结果
		memcpy(pBlackList->szIP,(*pResult)[0].GetString(),(*pResult)[0].GetLen()+1);

		vecLoginBlackList.push_back(pBlackList);

	}while(pResult->NextRow());

	// 归还结果集
	m_LoginDB[eDBType].FreeQueryResult(pResult);

	return TRUE;
}
//-------------------------------------------------------------------------
// 防沉迷
//-------------------------------------------------------------------------
BOOL CLoongLoginDBCtrl::LoginDB_Accout_Query(std::vector <tagTWrap_Longin_FatigueTime*> &vecLoginFatigueTime,EDBType eDBType)
{
	MyStream* pStream = m_LoginDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->SetSelect("fatigue_time", "accountname_crc,acc_online_time,acc_offline_time");

	QueryResult* pResult = m_LoginDB[eDBType].Query(pStream);

	// 释放流
	m_LoginDB[eDBType].ReturnStream(pStream);

	// 查询结果为空
	if( !P_VALID(pResult)) return FALSE;

	if(pResult->GetRowCount() == 0) return TRUE;

	do{
		tagTWrap_Longin_FatigueTime *pFatigueTime = new tagTWrap_Longin_FatigueTime;
		// 设置结果
		pFatigueTime->dwAccountNameCrc		=	(*pResult)[0].GetDword();
		pFatigueTime->dwOnlineTime		=	(*pResult)[0].GetDword();
		pFatigueTime->dwOfflineTime		=	(*pResult)[0].GetDword();

		vecLoginFatigueTime.push_back(pFatigueTime);

	}while(pResult->NextRow());

	// 归还结果集
	m_LoginDB[eDBType].FreeQueryResult(pResult);

	return TRUE;
}
//-------------------------------------------------------------------------
// 查询所有用户
//-------------------------------------------------------------------------
BOOL CLoongLoginDBCtrl::LoginDB_Accout_Query(std::vector <tagTWrap_Longin_Account*> &vecLoginAccount,EDBType eDBType)
{
	MyStream* pStream = m_LoginDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->SetSelect("account", "id,name,psd,privilege,login_status,worldname_crc,forbid_mask,guard,mibao");

	QueryResult* pResult = m_LoginDB[eDBType].Query(pStream);

	// 释放流
	m_LoginDB[eDBType].ReturnStream(pStream);

	// 查询结果为空
	if( !P_VALID(pResult)) return FALSE;

	if(pResult->GetRowCount() == 0) return TRUE;


	do{
		tagTWrap_Longin_Account *pAccount = new tagTWrap_Longin_Account;
		// 设置结果
		pAccount->stOthers.dwAccountID		=	(*pResult)[0].GetDword();
		
		strncpy(pAccount->szName,(*pResult)[1].GetString(),(*pResult)[1].GetLen()+1);
		strncpy(pAccount->szPsd,(*pResult)[2].GetString(),(*pResult)[2].GetLen()+1);
		pAccount->stOthers.byPrivilege		=	(*pResult)[3].GetByte();
		pAccount->stOthers.eLoginStatus		=	(EPlayerLoginStatus)(*pResult)[4].GetInt();
		pAccount->stOthers.dwWorldNameCrc	=	(*pResult)[5].GetDword();
		pAccount->stOthers.dwFrobidMask		=	(*pResult)[6].GetDword();
		pAccount->stOthers.bGuard			=	(*pResult)[7].GetBool();
		pAccount->stOthers.nGuardAccuTime	=	0;
		
		if((*pResult)[8].GetLen() > 0 )
		strncpy(pAccount->szMibao,(*pResult)[8].GetString(),(*pResult)[8].GetLen()+1);

		// 记录密保
		CHAR szMibao[MIBAO_LEN] = {'\0'};
		BOOL bRet = (*pResult)[8].GetBlob(szMibao, MIBAO_LEN);
		if( '\0' == szMibao[0] )
		{
			pAccount->stOthers.bNeedMibao = FALSE;
		}
		else
		{
			pAccount->stOthers.bNeedMibao = TRUE;
		}


		vecLoginAccount.push_back(pAccount);

	}while(pResult->NextRow());

	// 归还结果集
	m_LoginDB[eDBType].FreeQueryResult(pResult);

	return TRUE;
}

//------------------------------------------------------------------------------------
// 数据库预警回调函数
//------------------------------------------------------------------------------------
VOID LoginDBCallBack(DataBase* pDB, INT nReason, INT nParam)
{
	TCHAR cTemp[200];

	if (nReason == DataBase::EDBE_System)
	{
		_stprintf(cTemp,_T("Beton Warning CallBack: Reason  %s , nParam = %u\r\n"), _T("EDBE_System"), nParam);
		g_clsOutput.Output(cTemp);
	}
	else if(nReason == DataBase::EDBE_QueueFull)
	{
		_stprintf(cTemp,_T("Beton Warning CallBack: Reason  %s , nParam = %u\r\n"), _T("EDBE_QueueFull"), nParam);
		g_clsOutput.Output(cTemp);
	}
	else if(nReason == DataBase::EDBE_PoolFull)
	{
		_stprintf(cTemp,_T("Beton Warning CallBack: Reason  %s , nParam = %u\r\n"), _T("EDBE_PoolFull"), nParam);
		g_clsOutput.Output(cTemp);
	}
	else
	{	
		_stprintf(cTemp,_T("Beton Warning CallBack: Reason Unknow,nReason = %u, nParam = %u\r\n"), nReason, nParam);
		g_clsOutput.Output(cTemp);
	}

}