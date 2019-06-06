#include "StdAfx.h"
#include ".\configfile.h"
#include "..\VFS\IFS.h"
#include "..\Kernel.h"
#include "Exception.h"

namespace Cool3D
{
	ConfigFile::ConfigFile(const TCHAR* szFileName)
	{
		m_szFileName=szFileName;
	}

	ConfigFile::~ConfigFile(void)
	{
	}
	
	double ConfigFile::GetNumber(const TCHAR* szVarName) const
	{
		tstring str=GetString(szVarName);
		return _tstof(str.c_str());
	}

	bool ConfigFile::GetBool(const TCHAR* szVarName) const
	{
		return GetInt(szVarName)!=0;
	}

	int	ConfigFile::GetInt(const TCHAR* szVarName) const
	{
		tstring str=GetString(szVarName);
		return _ttoi(str.c_str());
	}

	tstring ConfigFile::GetString(const TCHAR* szVarName) const
	{
		SetCurrentDirectory(Kernel::Inst()->GetWorkPath());

		TCHAR szBuf[100];
		GetPrivateProfileString(_T("coolengine"),szVarName,_T("NULL"),szBuf,sizeof(szBuf),m_szFileName.c_str());
		return tstring(szBuf);
	}

}//namespace Cool3D	