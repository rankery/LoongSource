#include "stdafx.h"
#include "FilterData.h"
#include "..\WorldDefine\msg_common_errorcode.h"

using namespace Filter;

FilterData::FilterData( void )
{

}

FilterData::~FilterData( void )
{

}

FilterData g_FilterData;
FilterData* FilterData::Inst()
{
	return &g_FilterData;
}

void FilterData::LoadFromFile()
{
	tstring strPath1 = g_strLocalPath + _T("\\attdata\\name_filter.xml");
	const TCHAR* szNameFilter = strPath1.c_str();
	tstring strPath2 = g_strLocalPath + _T("\\attdata\\chat_filter.xml");
	const TCHAR* szChatFilter = strPath2.c_str();

	list<tstring> nameList;
	list<tstring> chatList;
	list<tstring>::iterator iter;

	//加载姓名过滤词表
	TObjRef<VarContainer> varNameFilter = CreateObj( "NameFilter", "VarContainer" );
	if(!varNameFilter->Load("VFS_System", szNameFilter, "id", &nameList))
		IMSG(_T("Load file name_filter.xml failed\r\n"));

	for( iter=nameList.begin(); iter!=nameList.end(); ++iter )
	{
		tstring strNameFilter;
		strNameFilter = varNameFilter->GetString(_T("name"), (*iter).c_str(), _T(""));
		m_vecNameFilter.push_back(strNameFilter);
	}

	KillObj("NameFilter");

	//加载聊天过滤词表
	TObjRef<VarContainer> varChatFilter = CreateObj( "ChatFilter", "VarContainer" );
	if(!varChatFilter->Load("VFS_System", szChatFilter, "id", &chatList))
		IMSG(_T("Load file chat_filter.xml failed\r\n"));

	for( iter=chatList.begin(); iter!=chatList.end(); ++iter )
	{
		tstring strChatFilter;
		strChatFilter = varChatFilter->GetString(_T("name"), (*iter).c_str(), _T(""));
		m_vecChatFilter.push_back(strChatFilter);
	}

	KillObj("ChatFilter");
}

DWORD FilterData::IsValidName(LPCTSTR name, INT nMax/*=7*/, INT nMin/*=1*/)
{
	DWORD dwError;
	dwError = CheckName(name, nMax, nMin, &m_vecNameFilter);
	if( dwError!=E_Success )
		return dwError;

	return E_Success;
}

DWORD FilterData::WordsFilteror( tstring& strText, INT nMax/*=200*/ )
{
	DWORD dwError;
	dwError = WordsFilterForChat( strText, &m_vecChatFilter );
	if( dwError!=E_Success )
		return dwError;

	if(strText.size() > (DWORD)nMax)
		return E_Filter_Text_TooLong;

	return E_Success;
}