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
#include "stdafx.h"
#include "StringTable.h"



//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL StringTable::Init(LPCSTR szVFS, LPCTSTR szFileName)
{
	m_bLoaded = FALSE;
	m_pVarContainer = new VarContainer;
	if( !m_pVarContainer )
		return FALSE;

	if( FALSE == m_pVarContainer->Load(szVFS, szFileName) )
		return FALSE;

	m_bLoaded = TRUE;
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID StringTable::Destroy()
{
	SAFE_DEL(m_pVarContainer);
	m_bLoaded = FALSE;
}

