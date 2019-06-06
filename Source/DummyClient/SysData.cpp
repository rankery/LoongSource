
#include "stdafx.h"
#include "SysData.h"

SysData::SysData()
{
	m_pSysFS = NULL;
	m_pMapFS = NULL;

	m_bUseCPK = false;
}

SysData::~SysData()
{}

SysData g_sysData;
SysData *SysData::Inst()
{
	return &g_sysData;
}

//--
void SysData::Init(bool bUseCPK)
{
	m_bUseCPK = bUseCPK;
	if(bUseCPK)
	{
		////--打包文件系统
		//xEngine::CloneObj("Sys_CPK","vfs");
		//m_pSysFS=(IFS*)xEngine::GetInterface("Sys_CPK");
		//BOOL bOK=m_pSysFS->SetWorkingPackage("system.cpk");

		//xEngine::CloneObj("Map_CPK","vfs");
		//m_pMapFS=(IFS*)xEngine::GetInterface("Map_CPK");
	}
	else
	{
		//--use default
		if(m_pSysFS == NULL)
			m_pSysFS = Kernel::Inst()->DefaultFS();
		m_pMapFS = Kernel::Inst()->DefaultFS();
	}

	Cool3D::ResMgr::Inst()->SetFS(m_pSysFS,m_pMapFS);
}

void SysData::OpenMapFS(const TCHAR* szName)
{
	if(m_bUseCPK)
	{
		ASSERT(m_pMapFS!=NULL);
		TCHAR szCpk[MAX_PATH];
		_stprintf_s(szCpk,MAX_PATH,_T("..\\data\\Map\\%s.cpk"),szName);
		m_pMapFS->SetWorkingPackage(szCpk);
	}
}

bool SysData::IsFileExist(const TCHAR* szName)
{
	if(!m_bUseCPK)
	{
		return IsFileExist(szName);
	}
	else
	{
		DWORD testSize = m_pSysFS->GetSize(szName);
		if(testSize!=0
			&& testSize!=-1)
			return true;

		testSize = m_pMapFS->GetSize(szName);
		if(testSize!=0
			&& testSize!=-1)
			return true;

		return false;
	}
}