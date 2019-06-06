#include "StdAfx.h"
#include ".\strtable.h"
#include "txtfile.h"
#include "..\Cool3D\kernel.h"

using namespace Cool3D;

StrTable::StrTable(void)
{
}

StrTable::~StrTable(void)
{
}

void StrTable::Load(const TCHAR *filename)
{
	TxtFile file;
	file.Load(ResMgr::Inst()->SafeGetFS(ResMgr::EFST_System),filename);
	file.OpenSection(_T("StrTable"));

	tstring key;
	while(file.NextToken())
	{
		if(_tcsicmp(file.m_token,_T("end"))==0)
			break;
		else
		{
			key=file.m_token;
			file.NextValueToken();
			tstring strText=file.m_token;
			
			size_t index = strText.find(_T("\\n"));
			while(index != tstring::npos)
			{
				strText.erase(index, 2);
				strText.insert(index, _T("\n"));
				index = strText.find(_T("\\n"));
			}

			m_data.insert(str2str::value_type(key,strText));
		}
	}//endof while
}

const TCHAR* StrTable::operator [](const TCHAR *key)
{
	str2str::iterator f=m_data.find(key);
	if(f!=m_data.end())
		return (*f).second.c_str();
	else
	{
		//LOG("String table find failed,key=%s\r\n",key);
		return _T("NULL");
	}
}
