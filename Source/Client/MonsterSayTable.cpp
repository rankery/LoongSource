#include "stdafx.h"
#include "MonsterSayTable.h"


BOOL MonsterSayTable::Init()
{
	tstring strPath = g_strLocalPath + _T("\\monsterSay_table.xml");
	
	list<tstring> sayList;
	TObjRef<VarContainer> con = CreateObj( "MonsterSay", "VarContainer" );
	if(!con->Load("VFS_System", strPath.c_str(), "name", &sayList))
	{
		IMSG(_T("Load file monsterSay_table.xml failed\r\n"));
		return FALSE;
	}
	list<tstring>::iterator end = sayList.end();
	for (list<tstring>::iterator iter = sayList.begin(); iter != end; ++iter)
	{
		DWORD dwID = con->GetDword(_T("name"), iter->c_str(), 0);
		tstring strSay = con->GetString(_T("value"), iter->c_str(), _T(""));
		m_mapSays.insert(std::make_pair(dwID, strSay));
	}
	KillObj("MonsterSay");
	return TRUE;
}

tstring MonsterSayTable::GetSay(DWORD dwID)
{
	std::map<DWORD, tstring>::const_iterator iter = m_mapSays.find(dwID);
	if (iter != m_mapSays.end())
		return iter->second;
	return _T("");

}

MonsterSayTable * MonsterSayTable::Inst()
{
	static MonsterSayTable inst;
	return &inst;
}