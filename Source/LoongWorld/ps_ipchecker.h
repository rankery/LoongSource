//-----------------------------------------------------------------------------
//!\file ps_ipchecker.h
//!\author xlguo
//!
//!\date 2009-04-21
//! last 2009-04-23
//!
//!\brief ·´Ë½·þip¼ì²â
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

class PSIpDict
{
	typedef std::vector<DWORD>	VecIp;

public:
	BOOL	Init();

	BOOL	LookUp(LPCSTR pIP);
	BOOL	LookUp(DWORD dwIP);

public:
	BOOL	Add(LPCSTR pIP);
	BOOL	Add(DWORD dwIP);
	
	static BOOL Test();

private:
	TObjRef<Util>				m_pUtil;
	TObjRef<StreamTransport>	m_pTrans;	

private:
	VecIp				m_vecIpDict;
};

extern PSIpDict	g_ipDict;