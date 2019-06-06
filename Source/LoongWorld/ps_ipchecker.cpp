//-----------------------------------------------------------------------------
//!\file ps_ipchecker.cpp
//!\author xlguo
//!
//!\date 2009-04-21
//! last 2009-04-23
//!
//!\brief ·´Ë½·þip¼ì²â
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ps_ipchecker.h"

PSIpDict g_ipDict;

LPCTSTR	tszInfPath = _T("server_data/information_position.ins");

BOOL PSIpDict::Init()
{

	CreateObj("TmpCont", "VarContainer");
	CreateObj("TmpUtil", "Util");

	TObjRef<VarContainer> pVar = "TmpCont";
	TObjRef<Util> pUtil = "TmpUtil";

	pVar->SetSaveSupport(TRUE);

	if (!pVar->Load("VirtualFileSys", tszInfPath))
	{
		return FALSE;
	}

	std::list<tstring>& rNameList = pVar->GetVarName();
	for (std::list<tstring>::iterator itr = rNameList.begin(); itr != rNameList.end(); ++itr)
	{
		LPSTR pIP = (LPSTR)pUtil->UnicodeToAnsi(pVar->GetString((*itr).c_str()));
		DWORD dwIP = m_pTrans->StringIP2IP(pIP);
		Add(dwIP);
	}

	KillObj("TmpCont");
	KillObj("TmpUtil");

	return TRUE;
}

BOOL PSIpDict::LookUp( LPCSTR pIP )
{
	DWORD dwIP = m_pTrans->StringIP2IP((LPSTR)pIP);

	return LookUp(dwIP);
}

BOOL PSIpDict::LookUp( DWORD dwIP )
{
	return m_vecIpDict.end() != std::find(m_vecIpDict.begin(), m_vecIpDict.end(), m_pUtil->Crc32(LPBYTE(&dwIP), sizeof(DWORD)));
}

BOOL PSIpDict::Add( LPCSTR pIP )
{
	DWORD dwIP = m_pTrans->StringIP2IP((LPSTR)pIP);

	return Add(dwIP);
}

BOOL PSIpDict::Add( DWORD dwIP )
{
	if (LookUp(dwIP))
	{
		return FALSE;
	}
	m_vecIpDict.push_back(m_pUtil->Crc32(LPBYTE(&dwIP), sizeof(DWORD)));
	return TRUE;
}

BOOL PSIpDict::Test()
{
	PSIpDict ipDict;

	ASSERT( ipDict.Init() );

	ASSERT( ipDict.LookUp("172.17.1.166") );


	return TRUE;

}