//-----------------------------------------------------------------------------
//!\file StringTable.h
//!\author Lyp
//!
//!\date 2008-05-27
//! last 2008-05-27
//!
//!\brief string table
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
//!\brief string table
//!
//!
//-----------------------------------------------------------------------------
class StringTable
{
public:

	BOOL Init(LPCSTR szVFS, LPCTSTR szFileName);
	VOID Destroy();

	LPCTSTR operator[](LPCTSTR szKey) { return m_pVarContainer->GetString(szKey); }
	BOOL IsLoaded() { return m_bLoaded; }

private:
	VarContainer*	m_pVarContainer;
	BOOL			m_bLoaded;

};
