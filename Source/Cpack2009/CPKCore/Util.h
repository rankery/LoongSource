//-----------------------------------------------------------------------------
// File: Util.h
// Desc: Game System Util 2.0
// Auth: CTCRST
//
// Copyright (c) 2002 CTCRST Corporation All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_UTIL_H_
#define	_GT_UTIL_H_
#pragma once

#define GT_INVALID	-1
#define Msg			CUtil::MsgBox
#define DbgMsg		CUtil::DebugMsgBox
#define SAFE_DEL(p)	{ if(p) { delete p; p = NULL; } }

class CUtil
{
public:
	static VOID MsgBox(LPCTSTR lpFormat, ...);
	static INT  MsgBox(UINT uType, LPCTSTR lpFormat, ...);
	static VOID ErrMsg(UINT uType, LPCTSTR lpFormat, ...);
	static VOID DebugMsgBox(LPCTSTR lpFormat, ...);

	static DWORD GetFreeID();

	CUtil();
	~CUtil();

private:
	
	static DWORD m_dwIDHolder;	// 记录ID使用情况
	
};


//
// 异常处理类
//

#define GT_MAX_EXCPTSTR_LEN	256
class CExcept
{
public:
	CExcept( char* sMsg, char* sFileName, int nLineNum);
	char* GetString();

protected:
	char	m_sMsg[GT_MAX_EXCPTSTR_LEN];
	char	m_sDetail[GT_MAX_EXCPTSTR_LEN];
	char	m_sFileName[GT_MAX_EXCPTSTR_LEN];
	int		m_nLineNum;
};




#endif	// _GT_UTIL_H_