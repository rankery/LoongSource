//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: filter.h
// author: 
// actor:
// data: 2008-12-17
// last:
// brief: 名称过滤
//-------------------------------------------------------------------------------
#include "filter.h"
#include "msg_common_errorcode.h"
#include <tchar.h>
#include <algorithm>

#define JUDGE_ACSII_1(c)	{ if( c>=32 && c<=47 )			return E_Filter_Name_Illegal; }
#define JUDGE_ACSII_2(c)	{ if( c>=58 && c<=64 )			return E_Filter_Name_Illegal; }
#define JUDGE_ACSII_3(c)	{ if( c>=91 && c<=96 )			return E_Filter_Name_Illegal; }
#define JUDGE_ACSII_4(c)	{ if( c>=123 && c<=127 )		return E_Filter_Name_Illegal; }
#define JUDEG_ASCII_Tab(c)	{ if( c==9 )					return E_Filter_Name_Illegal; }
#define JUDGE_UNICODE_1(c)	{ if( c>=0x3000 && c<=0x301F )	return  E_Filter_Name_Illegal; }
#define JUDGE_UNICODE_2(c)	{ if( c>=0x2000 && c<=0x2050 )	return  E_Filter_Name_Illegal; }
#define JUDGE_UNICODE_3(c)	{ if( c>=0xFF00 && c<=0xFF0F )	return  E_Filter_Name_Illegal; }
#define JUDGE_UNICODE_4(c)	{ if( c>=0xFF1A && c<=0xFF20 )	return  E_Filter_Name_Illegal; }
#define JUDGE_UNICODE_5(c)	{ if( c>=0xFF3B && c<=0xFF40 )	return  E_Filter_Name_Illegal; }
#define JUDGE_UNICODE_6(c)	{ if( c>=0xFF5B && c<=0xFF65 )	return  E_Filter_Name_Illegal; }
//-------------------------------------------------------------------------------
namespace Filter
{
	//-------------------------------------------------------------------------------
	// 字符合法性及是否在过滤词表中
	//-------------------------------------------------------------------------------
	DWORD CheckName(LPCTSTR strName, INT nMax/*=7*/, INT nMin/*=1*/, vector<tstring>* pFilter/*=NULL*/)
	{
		// 检查字符合法性
		DWORD dwErrorCode = Filter::IsNameLegal(strName, nMax, nMin);
		if(dwErrorCode != E_Success)
		{
			return dwErrorCode;
		}

		return IsNameInNameFilterFile(strName, pFilter);
	}
	
	//-------------------------------------------------------------------------------
	// 字符合法性
	//-------------------------------------------------------------------------------
	DWORD IsNameLegal(LPCTSTR strName, INT nMax/*=7*/, INT nMin/*=1*/)
	{
		INT nLen = (INT)_tcslen(strName);
		if(  strName==NULL || nLen==0  )
			return E_Filter_Name_Null;

		if( nLen>nMax )
			return E_Filter_Name_TooLong;

		for( int i=0; i<nLen; i++ )
		{
			JUDGE_ACSII_1(strName[i]);
			JUDGE_ACSII_2(strName[i]);
			JUDGE_ACSII_3(strName[i]);
			JUDGE_ACSII_4(strName[i]);
			JUDEG_ASCII_Tab(strName[i]);
			JUDGE_UNICODE_1(strName[i]);
			JUDGE_UNICODE_2(strName[i]);
			JUDGE_UNICODE_3(strName[i]);
			JUDGE_UNICODE_4(strName[i]);
			JUDGE_UNICODE_5(strName[i]);
			JUDGE_UNICODE_6(strName[i]);
		}

		return E_Success;
	}

	//-------------------------------------------------------------------------------
	// 是否在过滤词表中
	//-------------------------------------------------------------------------------
	DWORD IsNameInNameFilterFile( LPCTSTR szName, vector<tstring>* pFilter/*=NULL*/ )
	{
		if( pFilter==NULL )
			return E_Success;

		size_t index = 0;
		tstring	strName = szName;
		transform(strName.begin(), strName.end(), strName.begin(), tolower);
		size_t	nameLen = strName.size();	//待检测的字符串的长度
		size_t fLen = pFilter->size();		//表的大小

		for(size_t i=0; i<fLen; ++i)
		{
			index = 0;
			size_t length = pFilter->at(i).size();
			if(nameLen < length)	
				continue;
			tstring strFind = pFilter->at(i);
			transform(strFind.begin(), strFind.end(), strFind.begin(), tolower);
			index  = strName.find(strFind);
			if(index != tstring::npos)
			{	
				return E_Filter_Name_InFilterFile;
			}	
		}
		return E_Success;
	}

	//-------------------------------------------------------------------------------
	// 聊天内容过滤,将过滤词转换为"*"
	//-------------------------------------------------------------------------------
	DWORD WordsFilterForChat( tstring& strText, vector<tstring>* pFilter/*=NULL*/ )
	{
		size_t textLen = strText.size();	//待检测的字符串的长度
		if( textLen == 0 )
			return E_Filter_Chat_NoContent;

		tstring strTemp;
		strTemp.assign(strText);
		transform(strText.begin(), strText.end(), strText.begin(), tolower);
		size_t index = 0;
		size_t fLen = pFilter->size();		//表的大小

		for( size_t i=0; i<fLen; ++i )
		{
			index = 0;
			size_t length = pFilter->at(i).size();
			if(textLen < length)
				continue;
			
			tstring strFind = pFilter->at(i);
			transform(strFind.begin(), strFind.end(), strFind.begin(), tolower);
			index = strText.find(strFind, index);
			while( index != string::npos )
			{
				strText.replace(index, length, length, _T('*'));
				index += length;
				if(index > textLen-1)
					break;
				index = strText.find(pFilter->at(i), index);
				if( index+length > textLen-1 )
					break;
			}
		}

		for( int i=0; i<(int)strText.size(); i++ )
		{
			if( strText[i]>='a' && strText[i]<='z' )
			{
				strText.replace(i, 1, 1, strTemp[i]);
			}
		}

		return E_Success;
	}


} // namespace Filter