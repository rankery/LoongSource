//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: LoongDBLoad.cpp
// author: Sxg
// actor:
// data: 2008-05-12
// last:
// brief: 数据库操作应用层实现 -- 模版方法
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "LoongDB.h"


//-------------------------------------------------------------------------------------------------------
// 读取数据库信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::Load(OUT LPVOID &pData, OUT INT32 *pNum, DWORD dwAccountID, 
					DWORD dwRoleID, Fun_pFormat3 pFunFormat, Fun_pProcRes pFunProcRes)
{
	BOOL bRet = TRUE;

	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	ASSERT(P_VALID(pStream));

	// 清空流
	pStream->Clear();

	// 格式化查询的角色数据
	(this->*pFunFormat)(pStream, dwAccountID, dwRoleID);

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);
	if(!P_VALID(pResult))
	{
		bRet = FALSE;
		goto Exit;
	}

	// 检验并保存查询结果
	if (pResult->GetRowCount() > 0)
	{
		(this->*pFunProcRes)(pData, pNum, pResult);
	}
	else
	{
		if (P_VALID(pNum))
		{
			*pNum = 0;
		}
		bRet = FALSE;
	}

Exit:
#ifdef _DEBUG
	IMSG(_T("%s\r\n"), m_pUtil->AnsiToUnicode(pStream->GetBuf()));
	IMSG(_T("Results Row Count: %d\n"), P_VALID(pResult) ? pResult->GetRowCount() : 0);
#endif
	
	++m_dwReadTimes;

	// 释放流和结果
	m_pDBLoong->ReturnStream(pStream);
	m_pDBLoong->FreeQueryResult(pResult);

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 插入，删除，更新等数据库操作模版方法(无结果集,且不需要数据库联接)
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::Update(DWORD dwRoleID, LPVOID pData, INT32 nNum, INT32 nSzOne, 
					  Fun_pFormat3_1 pFunFormat, OUT LPVOID *ppOutPointer/* = NULL*/)
{
	if(nNum <= 0)
	{
		return TRUE;
	}
	
	BOOL bRet = TRUE;
	
	// 获取流
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	ASSERT(P_VALID(pStream));

	// 清空流
	pStream->Clear();

	CHAR *pCurData = (CHAR *)pData;
	for(INT32 i=0; i<nNum; ++i)
	{
		// 格式化数据库操作语句,并执行
		(this->*pFunFormat)(pStream, dwRoleID, pCurData);

#ifdef _DEBUG
		IMSG(_T("%s\r\n"), m_pUtil->AnsiToUnicode(pStream->GetBuf()));
#endif

		if(!m_pDBLoong->Execute(pStream))
		{
			bRet = FALSE;
		}

		// 重置数据指针
		pCurData += nSzOne;

		++m_dwWriteTimes;
	}

	if(ppOutPointer != NULL)
	{
		*ppOutPointer = pCurData;
	}
	
	// 释放流
	m_pDBLoong->ReturnStream(pStream);

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 插入，删除，更新等数据库操作模版方法(无结果集,且不需要数据库联接)
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::UpdateLog(LPCSTR szTablePostfix, LPVOID pData, Fun_pFormat3_2 pFunFormat)
{
	// 获取流
	Beton::MyStream* pStream = m_pDBLog->GetStream();
	ASSERT(P_VALID(pStream));

	// 清空流
	pStream->Clear();

	// 格式化数据库操作语句,并执行
	(this->*pFunFormat)(pStream, szTablePostfix, pData);

#ifdef _DEBUG
	IMSG(_T("%s\r\n"), m_pUtil->AnsiToUnicode(pStream->GetBuf()));
#endif

	m_pDBLog->AddQuery(pStream);

	//++m_dwWriteTimes;

	// 释放流 -- 不能在此处释放
}

////-------------------------------------------------------------------------------------------------------
//// 插入，删除，更新等数据库操作模版方法(无结果集,且不需要数据库联接)
////-------------------------------------------------------------------------------------------------------
//VOID CLoongDB::UpdateLog(DWORD dwRoleID, LPVOID pData, INT32 nNum, INT32 nSzOne, Fun_pFormat3_1 pFunFormat)
//{
//	if(nNum <= 0)
//	{
//		return;
//	}
//
//	CHAR *pCurData = (CHAR *)pData;
//	for(INT32 i=0; i<nNum; ++i)
//	{
//		// 获取流
//		Beton::MyStream* pStream = m_pDBLog->GetStream();
//		ASSERT(P_VALID(pStream));
//		
//		// 格式化数据库操作语句,并执行
//		(this->*pFunFormat)(pStream, dwRoleID, pCurData);
//
//#ifdef _DEBUG
//		IMSG(_T("%s\r\n"), m_pUtil->Unicode8ToUnicode(pStream->GetBuf()));
//#endif
//
//		m_pDBLog->AddQuery(pStream);
//
//		// 重置数据指针
//		pCurData += nSzOne;
//
//		++m_dwWriteTimes;
//	}
//
//	// 释放流 -- 不能在此处释放
//}