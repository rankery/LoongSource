#include "StdAfx.h"
#include "CreatureDataReader.h"
namespace WorldBase
{
CreatureDataReader::CreatureDataReader(void)
{
}
CreatureDataReader::~CreatureDataReader(void)
{
}

void CreatureDataReader::LoadFromFile()
{
	XMLReader varLanguage;

	varLanguage.Load(NULL, _T("config\\local.xml"));
	tstring strPath = varLanguage.GetString(_T("local_path"));
	tstring strPath1 = strPath + _T("\\attdata\\creature_name.xml");

	const TCHAR* const szCreatureName = strPath1.c_str();
	const TCHAR* const szCreature = _T("data\\system\\attdata\\creature_proto.xml");

	list<tstring> CreNameFieldList;
	list<tstring> CreFieldList;
	list<tstring>::iterator iter;

	
	XMLReader VarCre;
	if(!VarCre.Load(NULL, szCreature, "id", &CreFieldList))
		return;
	
	for(iter = CreFieldList.begin(); iter != CreFieldList.end(); ++iter)
	{
		tagCreatureProto att;
		att.dwTypeID = VarCre.GetDword(_T("id"), (*iter).c_str(),-1);
		att.eType = (ECreatureType)VarCre.GetDword(_T("type"), (*iter).c_str(),0);
		
		m_creatureMap.insert(make_pair(att.dwTypeID, att));
	}
	
	XMLReader VarCreName;
	if(!VarCreName.Load(NULL, szCreatureName, "id", &CreNameFieldList))
		return;

	for(iter = CreNameFieldList.begin(); iter != CreNameFieldList.end(); ++iter)
	{
		DWORD dwID = VarCreName.GetDword(_T("id"), (*iter).c_str());
		tagCreatureProto* pAtt = FindInMap(m_creatureMap, dwID);
		if( pAtt )
		{
			_tcscpy( pAtt->szName, VarCreName.GetString(_T("name"), (*iter).c_str(), _T("")) );
			_tcscpy ( pAtt->szModelName, VarCreName.GetString(_T("model"), (*iter).c_str(), _T("")) );

			pAtt->eAnimationType = (EAniType)VarCreName.GetDword(_T("anitype"), (*iter).c_str(), 1);
		}

	}
}

tstring CreatureDataReader::GetCreatureMdlPath(const tagCreatureProto *pNpcAtt)
{
	TCHAR szPath[MAX_PATH] = {0};

	switch( pNpcAtt->eAnimationType )
	{
	case EAT_None:
		_stprintf( szPath, _T("data\\animation\\npc\\%s\\%s.fsm"), pNpcAtt->szModelName,  pNpcAtt->szModelName );
		break;

	case EAT_KeyFrame:
		_stprintf( szPath, _T("data\\animation\\npc\\%s\\%s.fak"), pNpcAtt->szModelName,  pNpcAtt->szModelName );
		break;

	case EAT_Skeleton:
		_stprintf( szPath, _T("data\\animation\\npc\\%s\\std.fskel"), pNpcAtt->szModelName );
		break;
	}

	return tstring(szPath);
}

tstring CreatureDataReader::GetCreatureSkinPath( const tagCreatureProto *pNpcAtt, int index )
{
	TCHAR szPath[MAX_PATH] = {0};

	if( pNpcAtt->eAnimationType == EAT_Skeleton )
	{
		_stprintf( szPath, _T("data\\animation\\npc\\%s\\%s_obj%d.fskin"), 
			pNpcAtt->szModelName, 
			pNpcAtt->szModelName,
			index );
	}

	return tstring(szPath);
}
} // namespace WorldBase

