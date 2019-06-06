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

	LPCTSTR operator[](LPCTSTR szKey) 
	{ 
		LPCTSTR szValue = m_pVarContainer->GetString(szKey);
		if( P_VALID(szValue) )
			return szValue; 

		m_strError = szKey;
		m_strError.append(_T(":NOT found in StringTable!"));
		return m_strError.c_str();
	}

private:
	TObjRef<VarContainer>	m_pVarContainer;
	tstring					m_strError;

};


