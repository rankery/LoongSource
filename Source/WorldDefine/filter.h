//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: filter.h
// author: 
// actor:
// data: 2008-12-17
// last:
// brief: 名称过滤
//-------------------------------------------------------------------------------
#pragma once

#include <wtypes.h>
#include <vector>
using namespace std;

#ifdef _UNICODE
typedef std::wstring		tstring;
typedef std::wstringstream	tstringstream;
typedef std::wstreambuf		tstreambuf;
#else
typedef std::string			tstring;
typedef std::stringstream	tstringstream;
typedef std::streambuf		tstreambuf;
#endif

//-------------------------------------------------------------------------------
// 方法
//-------------------------------------------------------------------------------
namespace Filter
{
	DWORD	CheckName(LPCTSTR strName, INT nMax=7, INT nMin=1, vector<tstring>* pFilter=NULL);
	DWORD	IsNameLegal(LPCTSTR strName, INT nMax=7, INT nMin=1);
	DWORD	IsNameInNameFilterFile(LPCTSTR szName, vector<tstring>* pFilter=NULL);
	DWORD	WordsFilterForChat(tstring& strText, vector<tstring>* pFilter=NULL);
}