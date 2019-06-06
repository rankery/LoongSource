#include "stdafx.h"
#include "TxtFile.h"
#include <assert.h>

TxtFile::TxtFile()
{
	m_buf=NULL;
	m_sectionstart=NULL;
	m_curpos=NULL;
	memset(m_token,0,sizeof(m_token));
}

TxtFile::~TxtFile()
{
	Destroy();
}

void TxtFile::Load(IFS *pFS,LPCTSTR filename)
{
	Destroy();
	//--
	DWORD hFile=pFS->Open(filename);
	if(hFile == NULL)
	{
		//LOG("TxtFile:%s\r\n",filename);
	}
	//THROW_NULL(hFile,"TxtFile load failed");
	assert( hFile != NULL );
	int size=pFS->GetSize(filename);
	assert(size>0);
		
	m_buf=(TCHAR*)malloc(size+1);
	memset(m_buf,0,size+1);
	pFS->Read(m_buf,size,hFile);
	m_sectionstart=m_buf;

	pFS->Close(hFile);
}



void TxtFile::Destroy()
{
	if(m_buf!=NULL)
	{
		free(m_buf);
		m_buf=NULL;
	}
	m_sectionstart=NULL;
	m_curpos=NULL;
}

void TxtFile::OpenSection(LPCTSTR name)
{
	assert(m_buf!=NULL);
	m_curpos=m_buf;
	BOOL findflag=FALSE;
	while(NextToken())
	{
		if(findflag)
		{
			if(_tcsicmp(m_token,name)==0)
			{
				m_sectionstart=m_curpos;
				return;
			}
			findflag=FALSE;
		}
		else
		{
			if(_tcsicmp(m_token,_T("Begin"))==0)
				findflag=TRUE;
		}
	}
	assert(0 && "Txt section failed");//section not find
	//LOG("TxtFile OpenSection failed!sec=%s\r\n",name);
}

BOOL TxtFile::NextToken()
{
	assert(m_curpos!=NULL);
	memset(m_token,0,sizeof(m_token));
	int i=0;
	
	int state=0;//0:start,1:注释,2:正常输入,3:跳过值定义,255:end
	while(*m_curpos!=0)
	{
		switch(state)
		{
		case 0:
			if(*m_curpos==_T('{'))
				state=3;
			else if(*m_curpos==_T(';'))
				state=1;
			else
			{
				if(*m_curpos!=_T(' ') && *m_curpos!='\t' && *m_curpos!=_T(',')
					&& *m_curpos!=_T('\r') && *m_curpos!=_T('\n'))
				{
					assert(i< sizeof(m_token)-1);
					m_token[i]=*m_curpos;
					i++;
					state=2;
				}
			}
			break;
		case 1:
			if(*m_curpos==_T('\n'))
				state=0;
			break;
		case 3:
			if(*m_curpos==_T('}'))
				state=0;
			break;
		case 2:
			if(*m_curpos==_T(';'))
				state=1;
			else if(*m_curpos==_T('\n') || *m_curpos==_T(' ') || *m_curpos==_T('\t') || *m_curpos==_T(','))//分割符
				state=255;
			else if(*m_curpos!=_T('\r'))
			{
				assert(i< sizeof(m_token)-1);
				m_token[i]=*m_curpos;
				i++;
			}
			break;
		case 255:
			return TRUE;
		}
		m_curpos++;
	}
	return FALSE;
}

void TxtFile::NextValueToken()
{
	assert(m_curpos!=NULL);
	memset(m_token,0,sizeof(m_token));
	int i=0;
	int state=0;//0:start,1:注释,2:正常输入,255:end
	while(*m_curpos!=0)
	{
		switch(state)
		{
		case 0:
			if(*m_curpos==_T(';'))
				state=1;
			else if(*m_curpos==_T('{'))
				state=2;
#ifdef _DEBUG
			else if(*m_curpos!=' ' && *m_curpos!='\t' && *m_curpos!=','
					&& *m_curpos!='\r' && *m_curpos!='\n')
			{
				//遇到下一个关键字token,即上一个关键字没有Value
				assert(0 && "Txt Token error");
			}
#endif
			break;
		case 1:
			if(*m_curpos==_T('\n'))
				state=0;
			break;
		case 2:
			if(*m_curpos==_T('}'))
			{
				state=255;
			}
			else
			{
				assert(i< sizeof(m_token)-1);
				m_token[i]=*m_curpos;
				i++;
			}
			break;
		case 255:
			return;
		}
		m_curpos++;
	}
}


void TxtFile::RewindSection()
{
	assert(m_sectionstart!=NULL);
	m_curpos=m_sectionstart;
}




#define SECTION_END		"End"
#define MAX_SEARCH		1024
//-----------------------------------------------------------------------------
// 得到指定token的浮点数值，使用此函数后，将会把偏移量移动到section开始
//-----------------------------------------------------------------------------
float TxtFile::GetTokenValue(LPCTSTR name)	// added by lyp
{
	BOOL bOK = FALSE;

	assert(name);
	assert(m_sectionstart);
	assert(m_curpos);

	this->RewindSection();
	
	for (int n=0; n<MAX_SEARCH; n++)
	{

		bOK = this->NextToken();
		if (FALSE == bOK)
		{
			assert(0);
			return 0.0f;
		}
		
		if (_tcscmp(m_token, name) == 0)	// found
		{
			this->NextValueToken();
			return (float)_tstof(m_token);
		}

		if (_tcscmp(m_token, _T(SECTION_END)) == 0)
			return 0.0f;
	}

	assert(0);
	return 0.0f;
}






bool TxtFile::GetTokenValue_String(LPCTSTR name, TCHAR *buffer, int bufferlen)
{
	BOOL bOK = FALSE;

	assert(name);
	assert(m_sectionstart);
	assert(m_curpos);

	this->RewindSection();
	
	for (int n=0; n<MAX_SEARCH; n++)
	{

		bOK = this->NextToken();
		if (FALSE == bOK)
		{
			return false;
		}
		
		if (_tcsicmp(m_token, name) == 0)	// found
		{
			this->NextValueToken();
			_tcsncpy(buffer,m_token,bufferlen-1);
			return true;
		}

		if (_tcsicmp(m_token, _T(SECTION_END)) == 0)
			return false;
	}

	return false;
}
