#pragma once

// 系统命令个数
const int g_nSystemCmdNum = 3;

// 系统命令表的实现
class CmdTable
{
public:

	//BOOL Init();
	//VOID Destroy();
	CmdTable();
	~CmdTable();

	LPCTSTR operator[](LPCTSTR szKey); 
	LPCTSTR operator[](INT nIndex); 
	VOID InsertCmd();

private:
	std::map<LPCTSTR, LPCTSTR>	m_mapCmd;
	std::map<INT, LPCTSTR>		m_mapCmdIndex;

};

CmdTable::CmdTable()
{
	TCHAR *szBuff = new TCHAR[X_SHORT_STRING];
	TCHAR *szCmd = new TCHAR[X_LONG_STRING];
	ZeroMemory(szBuff, X_SHORT_STRING);
	ZeroMemory(szCmd, X_LONG_STRING);

	_sntprintf( szBuff, X_SHORT_STRING, _T("Game_Open_Inner"));
	_sntprintf( szCmd, X_LONG_STRING, _T("netsh ipsec static set policy name=\"IP Security Policy\" assign=y"));
	m_mapCmdIndex.insert(std::pair<INT, LPCTSTR>(0, szBuff));
	m_mapCmd.insert(std::pair<LPCTSTR, LPCTSTR>(szBuff, szCmd));

	//----------------------------------------------------------------
	szBuff = new TCHAR[X_SHORT_STRING];
	szCmd = new TCHAR[X_LONG_STRING];
	ZeroMemory(szBuff, X_SHORT_STRING);
	ZeroMemory(szCmd, X_LONG_STRING);

	_sntprintf( szBuff, X_SHORT_STRING, _T("Game_Open_Outer"));
	_sntprintf( szCmd, X_LONG_STRING, _T("netsh ipsec static set policy name=\"IP Security Policy\" assign=n"));
	m_mapCmdIndex.insert(std::pair<INT, LPCTSTR>(1, szBuff));
	m_mapCmd.insert(std::pair<LPCTSTR, LPCTSTR>(szBuff, szCmd));

	//----------------------------------------------------------------
	szBuff = new TCHAR[X_SHORT_STRING];
	szCmd = new TCHAR[X_LONG_STRING];
	ZeroMemory(szBuff, X_SHORT_STRING);
	ZeroMemory(szCmd, X_LONG_STRING);

	_sntprintf( szBuff, X_SHORT_STRING, _T("Update_Rtservice"));
	_sntprintf( szCmd, X_LONG_STRING, _T("d:\game\rtservices-restart.bat"));
	m_mapCmdIndex.insert(std::pair<INT, LPCTSTR>(2, szBuff));
	m_mapCmd.insert(std::pair<LPCTSTR, LPCTSTR>(szBuff, szCmd));

}

CmdTable::~CmdTable()
{
	std::map<INT, LPCTSTR>::iterator it_1 = m_mapCmdIndex.begin();
	std::map<LPCTSTR, LPCTSTR>::iterator it_2 = m_mapCmd.begin();
	
	for (;it_2 != m_mapCmd.end(); it_2++)
	{
		delete it_2->second;
	}
	
	for (;it_1 != m_mapCmdIndex.end(); it_1++)
	{
		delete it_1->second;
	}

	m_mapCmd.clear();
	m_mapCmdIndex.clear();
}

LPCTSTR CmdTable::operator[](LPCTSTR szKey) 
{ 
	std::map<LPCTSTR, LPCTSTR>::iterator it = m_mapCmd.begin();
	
	for (;it != m_mapCmd.end();it++)
	{
		if(_tcscmp(it->first, szKey) == 0)
			break;
	}

	LPCTSTR temp;
	if(it != m_mapCmd.end())
		temp = it->second;

	return temp;
}

LPCTSTR CmdTable::operator[](INT nIndex) 
{ 
	std::map<INT, LPCTSTR>::iterator it;
	it = m_mapCmdIndex.find(nIndex);

	LPCTSTR temp;

	if( it == m_mapCmdIndex.end() )
		temp = NULL;
	else
		temp = it->second;

	return temp;
}

CmdTable g_CmdTable;
