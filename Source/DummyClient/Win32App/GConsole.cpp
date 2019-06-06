#include "..\StdAfx.h"
#include ".\gconsole.h"
#include "time.h"
#include "..\..\Cool3D\Device\DrawX.h"
#include "..\..\Cool3D\Util\DebugMisc.h"
using namespace Cool3D;


GConsole::GConsole(void)
{
	m_bactive=true;
	memset(m_showbuf,0,sizeof(m_showbuf));
	memset(m_inputbuf,0,sizeof(m_inputbuf));
	m_inputbuf[0]=_T('_');
	m_rect.left=0;
	m_rect.top=16;//Cool3D状态显示在第一行
	m_rect.right=800;
	m_rect.bottom=360;
	m_nCur = -1;
}

GConsole::~GConsole(void)
{
}

void GConsole::Init(RECT rect)
{
	m_rect=rect;
}

void GConsole::RegCommand(void(*fun)(const AgvList& agvList),LPCTSTR keyword,LPCTSTR help)
{
	tagConCmd cmd;
	cmd.fun=fun;
	::_tcscpy_s(cmd.szKeyword,sizeof(cmd.szKeyword)/sizeof(TCHAR),keyword);
	::_tcscpy_s(cmd.szHelp,sizeof(cmd.szHelp)/sizeof(TCHAR),help);
	m_cmds.push_back(cmd);
}

void GConsole::Print(LPCTSTR format,...)
{
	static TCHAR buf[512];
	va_list argptr;
	int cnt;
	va_start(argptr, format);
	cnt = _vstprintf_s(buf, 512, format, argptr);
	va_end(argptr);

	::memmove(m_showbuf[0],m_showbuf[1],sizeof(m_showbuf)-sizeof(m_inputbuf));
	::_tcscpy_s(m_showbuf[CONSOLE_MAX_LINE-1],99,buf);
	//自动折行,但只折一行
	int len=::_tcslen(buf);
	if(len-99>0)
	{
		::memmove(m_showbuf[0],m_showbuf[1],sizeof(m_showbuf)-sizeof(m_inputbuf));
		::_tcscpy_s(m_showbuf[CONSOLE_MAX_LINE-1],99,buf+99);
	}
}

void GConsole::OnChar(TCHAR c)
{
	if(c==_T('`'))
	{
		m_bactive=!m_bactive;

		if(m_bactive)
			m_nCur = vecStrHis.size() - 1;
		else
			m_nCur = -1;
		return;
	}
	if(!m_bactive)
		return;
	switch(c)
	{
	case VK_ESCAPE:
		memset(m_inputbuf,0,sizeof(m_inputbuf));
		m_inputbuf[0]=_T('_');
		break;
	case VK_RETURN:
		{
			int len=::_tcslen(m_inputbuf);
			ASSERT(len>=1);
			m_inputbuf[len-1]=0;
			Print(_T(">%s"),m_inputbuf);
			
			if(0 == vecStrHis.size() && len > 1)
			{
				vecStrHis.push_back(tstring(m_inputbuf));
				m_nCur = 0;
			}
			else if(vecStrHis.size()>0 && _tcsicmp(m_inputbuf,vecStrHis[vecStrHis.size()-1].c_str()) && len > 1)
			{
				vecStrHis.push_back(tstring(m_inputbuf));
				m_nCur = vecStrHis.size()-1;
			}

			RunCommand();
			memset(m_inputbuf,0,sizeof(m_inputbuf));
			m_inputbuf[0]=_T('_');
			break;
		}
	case VK_BACK:
		{
			int len=::_tcslen(m_inputbuf);
			ASSERT(len<sizeof(m_inputbuf));
			if(len>=2)
			{
				m_inputbuf[len-1]=0;
				m_inputbuf[len-2]=_T('_');
			}
			break;
		}
	case VK_UP:
		{
			if(m_nCur < 0)
				return;	

			::memset(m_inputbuf,0,sizeof(m_inputbuf));
			_tcscpy_s(m_inputbuf,sizeof(m_inputbuf)/sizeof(TCHAR),vecStrHis[m_nCur].c_str());
			const TCHAR* chT = _T("_");
			_tcscat_s(m_inputbuf,sizeof(m_inputbuf)/sizeof(TCHAR),chT);
			if(0 == m_nCur--)
				m_nCur = vecStrHis.size() - 1;

			break;
		}
	default:
		{
			int len=::_tcslen(m_inputbuf);
			ASSERT(len>=1);//至少有'_'
			if(len < sizeof(m_inputbuf)-1)
			{
				m_inputbuf[len-1]=c;
				m_inputbuf[len]=_T('_');
			}
		}
	}//endof switch
}

void GConsole::RunCommand()
{
	if(_tcslen(m_inputbuf)==0)
		return;
	const TCHAR seps[]   = _T(" ,");
	TCHAR* next_token = NULL;
	TCHAR *token=_tcstok_s( m_inputbuf, seps, &next_token );

	//得到命令和参数列表
	AgvList	argList;//由input buffer中解析出来的命令参数列表
	tstring cmd;
	if(token==NULL)
	{
		cmd=m_inputbuf;
	}
	else
	{
		cmd=token;
		token = _tcstok_s( NULL, seps, &next_token );
		while(token!=NULL)
		{
			argList.push_back(tstring(token));
			token = _tcstok_s( NULL, seps, &next_token );
		}
	}

	//--
	if(_tcsicmp(cmd.c_str(),_T("help"))==0)
	{
		CmdHelp();
		return;
	}
	for(vector<tagConCmd>::iterator iter=m_cmds.begin();
		iter!=m_cmds.end();++iter)
	{
		if(_tcsicmp(cmd.c_str(),iter->szKeyword)==0)
		{
			iter->fun(argList);
			return;
		}
	}

	//--
	Print(_T("Console error:无法识别的控制台命令."));
}

void GConsole::CmdHelp()
{
	Print(_T("--Console cmd list:"));
	for(vector<tagConCmd>::iterator iter=m_cmds.begin();
		iter!=m_cmds.end();++iter)
	{
		Print(_T("%s:%s"),iter->szKeyword,iter->szHelp);
	}
}

void GConsole::Draw()
{
	if(!m_bactive)
		return;

	DrawX::Draw2DQuad(0,0,
		m_rect.right,m_rect.bottom,false,Color4ub(200,5,0,5));
	DrawX::Draw2DQuad(m_rect.left,m_rect.bottom-16,
		m_rect.right-m_rect.left,16,false,Color4ub(200,5,50,55));
	DrawX::DrawDebugString(m_inputbuf,m_rect.left+4,m_rect.bottom-17);
	int i;
	for(i=0;i<CONSOLE_MAX_LINE;i++)
	{
		if(::_tcslen(m_showbuf[i])==0)
			continue;
		DrawX::DrawDebugString(m_showbuf[i],m_rect.left+4,m_rect.top+i*16);
	}
}
void GConsole::LogToFile(const TCHAR* fileName)
{
	FILE* fp = NULL;
	if(_tfopen_s(&fp,fileName,_T("wt"))==0)
	{
		TCHAR buf[128];
		::GetModuleFileName(NULL,buf,sizeof(buf));
		_ftprintf(fp,_T("%s Log at "),buf);
		
		_tstrdate_s(buf,128);
		_ftprintf(fp,buf);
		_ftprintf(fp,_T("\t"));

		_tstrtime_s(buf,128);
		_ftprintf(fp,buf);
		_ftprintf(fp,_T("\n"));

		for(int i=0;i<CONSOLE_MAX_LINE;i++)
		{
			if(_tcslen(m_showbuf[i])>0)
			{
				_ftprintf(fp,m_showbuf[i]);
				_ftprintf(fp,_T("\n"));
			}
		}
	}
}
