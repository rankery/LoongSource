#include "StdAfx.h"
#include ".\plugin.h"
#include "DebugMisc.h"

namespace Cool3D
{
	PlugIn::PlugIn(void)
	{
		m_hDll=NULL;
		m_funGetClassName=NULL;
		m_funNewObject=NULL;
		m_funDelObject=NULL;
		m_funGetNumClass=NULL;
		m_funDestroy=NULL;
		m_funGetObj=NULL;
	}

	PlugIn::~PlugIn(void)
	{
		Close();
	}

	bool PlugIn::LoadFromDll(const TCHAR* szPath)
	{
		m_hDll=LoadLibrary(szPath);
		if(m_hDll==NULL)
			return false;
		Plug_Init *pFunInit=(Plug_Init*)GetProcAddress(m_hDll,"Plug_Init");
		if(pFunInit)
		{
			if(!(pFunInit)())
				return false;
		}

		m_funGetClassName=(Plug_GetClassName*)GetProcAddress(m_hDll,"Plug_GetClassName");
		m_funNewObject=(Plug_NewObject*)GetProcAddress(m_hDll,"Plug_NewObject");
		m_funDelObject=(Plug_DelObject*)GetProcAddress(m_hDll,"Plug_DelObject");
		m_funGetNumClass=(Plug_GetNumClass*)GetProcAddress(m_hDll,"Plug_GetNumClass");
		m_funDestroy=(Plug_Destroy*)GetProcAddress(m_hDll,"Plug_Destroy");
		m_funGetObj=(Plug_GetObject*)GetProcAddress(m_hDll,"Plug_GetObject");

		return m_funGetClassName!=NULL
			&& m_funNewObject!=NULL
			&& m_funDelObject!=NULL
			&& m_funGetNumClass!=NULL;
	}

	void PlugIn::Close()
	{
		if(m_funDestroy)
		{
			(m_funDestroy)();
		}
		if(m_hDll!=NULL)
			FreeLibrary(m_hDll);
		m_hDll=NULL;
		m_funGetClassName=NULL;
		m_funNewObject=NULL;
		m_funDelObject=NULL;
		m_funGetNumClass=NULL;
		m_funDestroy=NULL;
		m_funGetObj=NULL;
	}

	const TCHAR*	PlugIn::GetClassName(int i)
	{
		ASSERT(m_funGetClassName != NULL);
		return (m_funGetClassName)(i);
	}
	
	void* PlugIn::NewObject(const TCHAR* szClassName,DWORD param1,const TCHAR* param2)
	{
		ASSERT(m_funNewObject != NULL);
		return (m_funNewObject)(szClassName,param1,param2);
	}

	void PlugIn::DelObject(const TCHAR* szClassName,void *pObj,const TCHAR* param2)
	{
		ASSERT(m_funDelObject != NULL);
		(m_funDelObject)(szClassName,pObj,param2);
	}
	int	PlugIn::GetNumClass()
	{
		ASSERT(m_funGetNumClass != NULL);
		return (m_funGetNumClass)();
	}
	void* PlugIn::GetPlugObject(const TCHAR* szName)
	{
		ASSERT(m_funGetObj != NULL);
		return (m_funGetObj)(szName);
	}
}