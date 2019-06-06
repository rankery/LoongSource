#include "stdafx.h"
#include "MapLogicData.h"

MapLogicData::MapLogicData()
{
}

MapLogicData::~MapLogicData()
{
}

MapLogicData g_mapLogicData;
MapLogicData* MapLogicData::Inst()
{
	return &g_mapLogicData;
}

void MapLogicData::LoadFromFile()
{
	LPCTSTR szMapProto = _T("data\\system\\attdata\\map_logic_proto.xml");
	tstring strPath = g_strLocalPath + _T("\\attdata\\map_logic_name.xml");
	LPCTSTR szMapDisplay = strPath.c_str();

	list<tstring> MapProtoFieldList;
	list<tstring> MapDisplayFieldList;
	list<tstring>::iterator iter;

	TObjRef<Util> pUtil;

	//读取map_logic_proto
	TObjRef<VarContainer> VarMapProto = CreateObj("MapProto", "VarContainer");
	if(!VarMapProto->Load("VFS_System", szMapProto, "id", &MapProtoFieldList))
		IMSG(_T("Load file map_logic_name.xml failed\r\n"));

	for(iter = MapProtoFieldList.begin(); iter != MapProtoFieldList.end(); ++iter)
	{
		tagMapLogicData data;
		data.szName = VarMapProto->GetString(_T("id"), (*iter).c_str());
		data.dwID = pUtil->Crc32(data.szName.c_str());
		data.vSize.x = VarMapProto->GetFloat(_T("width"), (*iter).c_str());
		data.vSize.y = VarMapProto->GetFloat(_T("height"), (*iter).c_str());
		data.eMapType = (EMapType)(VarMapProto->GetInt(_T("type"), (*iter).c_str()));		// 地图类型
		//more

		m_mapMapLogicData.insert(make_pair(data.dwID, data));
	}

	KillObj("MapProto");


	//读取map_logic_name
	TObjRef<VarContainer> VarMapDisplay = CreateObj("MapDisplay", "VarContainer");
	if(!VarMapDisplay->Load("VFS_System", szMapDisplay, "id", &MapDisplayFieldList))
		IMSG(_T("Load file map_logic_name.xml failed\r\n"));

	for(iter = MapDisplayFieldList.begin(); iter != MapDisplayFieldList.end(); ++iter)
	{
		DWORD dwID = pUtil->Crc32(VarMapDisplay->GetString(_T("id"), (*iter).c_str()));
		tagMapLogicData* pData = FindInMap(m_mapMapLogicData, dwID);
		if(P_VALID(pData))
		{
			pData->szShowName = VarMapDisplay->GetString(_T("name"), (*iter).c_str());
			
			//more
		}
	}
	KillObj("MapDisplay");


	//读取MapShowAreaData
	TObjRef<VarContainer> VarMapShowAreaData = CreateObj("MapShowAreaData", "VarContainer");
	for(map<DWORD, tagMapLogicData>::iterator iterLogic = m_mapMapLogicData.begin(); iterLogic != m_mapMapLogicData.end(); iterLogic++ )
	{
		tagMapLogicData& data = iterLogic->second;
		tstring strFileName = g_strLocalPath + _T("\\map\\");
		strFileName += data.szName;
		strFileName += _T("\\show_area.xml");

		list<tstring> MapShowAreaList;
		if(!VarMapShowAreaData->Load("VFS_System", strFileName.c_str(), "id", &MapShowAreaList))
		{
			IMSG(_T("Load file %s failed\r\n"), strFileName.c_str() );
			continue;
		}

		for(iter = MapShowAreaList.begin(); iter != MapShowAreaList.end(); ++iter)
		{
			tagMapShowAreaData areaData;
			areaData.dwObjID = VarMapShowAreaData->GetDword( _T("id"),	(*iter).c_str(), GT_INVALID );
			areaData.szTitle = VarMapShowAreaData->GetString( _T("title"), (*iter).c_str(), _T("") );
			areaData.box.min.x = VarMapShowAreaData->GetFloat( _T("box_min_x"), (*iter).c_str(), 0.0f );
			areaData.box.min.y = VarMapShowAreaData->GetFloat( _T("box_min_y"), (*iter).c_str(), 0.0f );
			areaData.box.min.z = VarMapShowAreaData->GetFloat( _T("box_min_z"), (*iter).c_str(), 0.0f );
			areaData.box.max.x = VarMapShowAreaData->GetFloat( _T("box_max_x"), (*iter).c_str(), 0.0f );
			areaData.box.max.y = VarMapShowAreaData->GetFloat( _T("box_max_y"), (*iter).c_str(), 0.0f );
			areaData.box.max.z = VarMapShowAreaData->GetFloat( _T("box_max_z"), (*iter).c_str(), 0.0f );
			areaData.dwMiniMapSize = VarMapShowAreaData->GetDword( _T("minimap_size"),	(*iter).c_str(), 0 );
			//more

			data.vecShowAreaData.push_back( areaData );
		}
		VarMapShowAreaData->Clear();
	}
	KillObj("MapShowAreaData");
}

LPCTSTR MapLogicData::FindMapShowName( DWORD dwMapID )
{
	const tagMapLogicData* pData = FindMapLogicData(dwMapID);
	if( !P_VALID(pData) )
		return _T("");
	return pData->szShowName.c_str();
}

bool MapLogicData::GetMapShowAreaList(DWORD dwMapID,vector<tagMapShowAreaData>& vecData)
{
	vecData.clear();
	const tagMapLogicData* pData = FindMapLogicData(dwMapID);
	if( !P_VALID(pData) )
		return false;

	for( size_t i = 0; i < pData->vecShowAreaData.size(); i++ )
	{
		const tagMapShowAreaData& area = pData->vecShowAreaData[i];
		if( 0 == area.dwMiniMapSize )
			continue;
		vecData.push_back( area );
	}
	return true;
}


const tagMapShowAreaData* MapLogicData::FindMapShowAreaData(DWORD dwMapID, DWORD dwAreaObjID)
{
	const tagMapLogicData* pData = FindMapLogicData(dwMapID);
	if( !P_VALID(pData) )
		return NULL;

	for( size_t i = 0; i < pData->vecShowAreaData.size(); i++ )
	{
		if( pData->vecShowAreaData[i].dwObjID == dwAreaObjID )
			return &pData->vecShowAreaData[i];
	}
	return NULL;
}