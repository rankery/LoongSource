
#pragma once
#include "DummyClient.h"

class SysData
{
public:
	SysData();
	~SysData();

	static SysData *Inst();
	
	//--
	void Init(bool bUseCPK);
	void OpenMapFS(const TCHAR* szName);

	bool IsFileExist(const TCHAR* szName);
	
	//--
	IFS *GetMapFS() const	{	return m_pMapFS;}
	IFS *GetSysFS() const	{	return m_pSysFS;}

private:
	bool	m_bUseCPK;
	IFS		*m_pSysFS;
	IFS		*m_pMapFS;
};