#include "stdafx.h"
#include "NavKernel.h"
#include "NavResMgr.h"

namespace WorldBase
{
	class NavKernel::Member
	{
	public:
		UINT				m_navPoolBytes;
		TCHAR				m_szAppPath[512];
		TCHAR				m_szWorkPath[512];

		Member()
		{	
			m_navPoolBytes=8*1024*1024;

			//--
			GetModuleFileName(NULL,m_szAppPath,sizeof(m_szAppPath));
			TCHAR *find=::_tcsrchr(m_szAppPath,_T('\\'));
			if(find!=NULL)
				*(find+1)=0;
			_tcslwr_s(m_szAppPath,sizeof(m_szAppPath)/sizeof(TCHAR));

			//--
			GetModuleFileName(NULL,m_szWorkPath,sizeof(m_szWorkPath)/sizeof(TCHAR));
			find=::_tcsrchr(m_szWorkPath,_T('\\'));
			if(find!=NULL)
			{
				*find=0;
				find=::_tcsrchr(m_szWorkPath,_T('\\'));
				if(find!=NULL)
				{
					*(find+1)=0;
					_tcslwr_s(m_szWorkPath,sizeof(m_szWorkPath)/sizeof(TCHAR));
				}
			}
		}
		~Member()
		{}
	};

	NavKernel::NavKernel(void)
	{
		m_p=new Member;
	}

	NavKernel::~NavKernel(void)
	{
		delete m_p;
	}

	static NavKernel g_kernelInst;
	NavKernel *NavKernel::Inst()
	{
		return &g_kernelInst;
	}

	void NavKernel::Init( UINT poolBytes,bool bPoolThreadSync )
	{
		m_p->m_navPoolBytes=poolBytes;
		NavResMgr::Inst()->Init(m_p->m_navPoolBytes,bPoolThreadSync);
	}

	void NavKernel::Destroy()
	{
		NavResMgr::Inst()->Destroy();
	}

	const TCHAR* NavKernel::GetAppPath() const
	{
		return m_p->m_szAppPath;
	}

	const TCHAR* NavKernel::GetWorkPath() const
	{
		return m_p->m_szWorkPath;
	}
}//namespace WorldBase