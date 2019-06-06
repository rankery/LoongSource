#include "StdAfx.h"
#include ".\csvfile.h"
#include "Exception.h"

namespace Cool3D
{
	CSVFile::CSVFile(void)
	{
		m_szCSVBuffer=NULL;
		m_nCharNum=0;
		m_nCharIndex=0;
	}

	CSVFile::~CSVFile(void)
	{
		CloseFile();
	}

	bool CSVFile::OpenFile(IFS *pFS,const TCHAR* szFileName)
	{
		if(!pFS)//出现这种错误可以让它挂了
		{
			ASSERT(0 && _T("Invalid File System!"));
			return false;
		}
		
		if(!szFileName)
			return false;

		int nBufSize = pFS->GetSize(szFileName);
		if(nBufSize<=2)//unicode文本文件包含2字节的文件头
		{
			LOG(_T("CSV file open failed,%s\r\n"),szFileName);
			return false;
		}
		nBufSize-=2;
		DWORD dwFileHandle=pFS->Open(szFileName);
		if(dwFileHandle==0
			|| dwFileHandle==-1)
		{
			LOG(_T("CSV file open failed,%s\r\n"),szFileName);
			return false;
		}

		nBufSize+=sizeof(TCHAR);//留下一个结尾0的空间
		m_szCSVBuffer=(TCHAR*)malloc(nBufSize);
		memset(m_szCSVBuffer,0,nBufSize);

		pFS->Seek(dwFileHandle,2,SEEK_SET);
		pFS->Read(m_szCSVBuffer,nBufSize-sizeof(TCHAR),dwFileHandle);
		pFS->Close(dwFileHandle);

		m_nCharNum=nBufSize/sizeof(TCHAR)-1;
		m_nCharIndex=0;

		return true;
	}

	void CSVFile::BeginReadLine()
	{
		m_nCharIndex=0;
	}

	vector<tstring> CSVFile::GetNextLine()
	{
		vector<tstring> ret;
		if(m_nCharIndex >= m_nCharNum)
			return ret;

		const TCHAR* seps = _T("\r\n");
		TCHAR* next_token;
		TCHAR* token = _tcstok_s(m_szCSVBuffer+m_nCharIndex,seps,&next_token);
		if(token)
		{
			m_nCharIndex += _tcslen(token)+1;

			if( m_nCharIndex < m_nCharNum )
			{
				if(m_szCSVBuffer[m_nCharIndex]==_T('\n'))//跳过\n
					m_nCharIndex++;
			}

			seps = _T(",");
			ret.push_back(tstring(_tcstok_s(token,seps,&next_token)));

			while((token = _tcstok_s(NULL,seps,&next_token)) != NULL)
				ret.push_back(tstring(token));
		}

		return ret;
	}

	bool CSVFile::IsEnd() const
	{
		return m_nCharIndex >= m_nCharNum;
	}

	void CSVFile::CloseFile()
	{
		SAFE_FREE(m_szCSVBuffer);
		m_nCharNum=0;
	}
}//namespace Cool3D