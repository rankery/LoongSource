#pragma once

/***************************
\!brief 怪物说话的表
***************************/

class MonsterSayTable
{
public:
	static MonsterSayTable *Inst();
	~MonsterSayTable(){}
	BOOL Init();
	tstring GetSay(DWORD dwID);

private:
	std::map<DWORD, tstring> m_mapSays;
	MonsterSayTable(){}
};