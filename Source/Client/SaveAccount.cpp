#include "stdafx.h"
#include "SaveAccount.h"

SaveAccount::SaveAccount( void )
{
	m_nMusic		= 0;
}

SaveAccount::~SaveAccount( void )
{

}

SaveAccount g_SaveAccount;
SaveAccount* SaveAccount::Inst()
{
	return &g_SaveAccount;
}

void SaveAccount::SetAccount( const TCHAR* szAccount )
{
	m_account = szAccount;
}