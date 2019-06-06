#include "stdafx.h"
#include "SFXData.h"

SFXData::SFXData()
{
}

SFXData::~SFXData()
{
}

SFXData g_SFXData;
SFXData* SFXData::Inst()
{
	return &g_SFXData;
}

void SFXData::LoadFromFile()
{
	TObjRef<Util> pUtil;

	list<tstring> SFXList;
	TObjRef<VarContainer> VarMdlData = CreateObj("SFXData", "VarContainer");
	tstring strSFXDataPath = g_strLocalPath + _T("\\attdata\\sfx_data.xml");
	if(!VarMdlData->Load("VFS_System", strSFXDataPath.c_str(), "id", &SFXList))
		IMSG(_T("Load file sfx_data.xml failed\r\n"));

	for(list<tstring>::iterator iter = SFXList.begin(); iter != SFXList.end(); ++iter)
	{
		tagSFXData data;
		data.bCanMove = VarMdlData->GetInt( _T("can_move"), (*iter).c_str(), TRUE ) == TRUE;

		transform( (*iter).begin(), (*iter).end(), (*iter).begin(), tolower );
		const DWORD dwID = TObjRef<Util>()->Crc32( (*iter).c_str() );
		m_mapSFXData.insert( make_pair( dwID, data ) );
	}
	KillObj("SFXData");
}

const tagSFXData* SFXData::FindData(const TCHAR* szSFXName)
{
	if( NULL == szSFXName || _T('\0') == szSFXName[0] )
		return NULL;

	tstring strSFXName = szSFXName;
	transform( strSFXName.begin(), strSFXName.end(), strSFXName.begin(), tolower );

	const DWORD dwID = TObjRef<Util>()->Crc32(strSFXName.c_str());
	map<DWORD,tagSFXData>::iterator itr = m_mapSFXData.find( dwID );
	if( itr != m_mapSFXData.end() )
		return &itr->second;

	return NULL;
}