#include "stdafx.h"
#include "PlayerActionData.h"

PlayerActionData::PlayerActionData()
{
}

PlayerActionData::~PlayerActionData()
{
}

PlayerActionData g_PlayerActionData;
PlayerActionData* PlayerActionData::Inst()
{
	return &g_PlayerActionData;
}

void PlayerActionData::LoadFromFile()
{
	TObjRef<Util> pUtil;

	list<tstring> PlayerActionList;
	TObjRef<VarContainer> VarMdlData = CreateObj("SFXData", "VarContainer");
	tstring strSFXDataPath = g_strLocalPath + _T("\\attdata\\player_act_data.xml");
	if(!VarMdlData->Load("VFS_System", strSFXDataPath.c_str(), "id", &PlayerActionList))
		IMSG(_T("Load file player_act_data.xml failed\r\n"));

	for(list<tstring>::iterator iter = PlayerActionList.begin(); iter != PlayerActionList.end(); ++iter)
	{
		tagPlayerActionData data;
		data.dwAttackTime = VarMdlData->GetDword( _T("atk_time"), (*iter).c_str(), 0 );

		transform( (*iter).begin(), (*iter).end(), (*iter).begin(), tolower );
		const DWORD dwID = TObjRef<Util>()->Crc32( (*iter).c_str() );
		m_mapPlayerActionData.insert( make_pair( dwID, data ) );
	}
	KillObj("SFXData");
}

const tagPlayerActionData* PlayerActionData::FindData(const TCHAR* szActName)
{
	if( NULL == szActName || _T('\0') == szActName[0] )
		return NULL;

	tstring strActName = szActName;
	transform( strActName.begin(), strActName.end(), strActName.begin(), tolower );

	const DWORD dwID = TObjRef<Util>()->Crc32(strActName.c_str());
	map<DWORD,tagPlayerActionData>::iterator itr = m_mapPlayerActionData.find( dwID );
	if( itr != m_mapPlayerActionData.end() )
		return &itr->second;

	return NULL;
}