#include "stdafx.h"
#include "ZoneMapData.h"

ZoneMapData::ZoneMapData()
{
}

ZoneMapData::~ZoneMapData()
{
}

ZoneMapData g_ZoneMapData;
ZoneMapData* ZoneMapData::Inst()
{
	return &g_ZoneMapData;
}

void ZoneMapData::LoadFromFile()
{
	TCHAR szFileName[MAX_PATH];
	_stprintf( szFileName, _T("%s\\map\\mark_type.xml"), g_strLocalPath.c_str() );

	list<tstring> MarkTypeList;

	TObjRef<Util> pUtil;

	// 读取MarkTypeList
	TObjRef<VarContainer> VarMarkTypeList = CreateObj("MapMarkTypeList", "VarContainer");
	if( !VarMarkTypeList->Load("VFS_System", szFileName, "id", &MarkTypeList) )
		IMSG( _T("Load file %s failed\r\n"), szFileName );

	TObjRef<VarContainer> VarMarkDataList = CreateObj("MapMarkDataList", "VarContainer");
	for(list<tstring>::iterator iter = MarkTypeList.begin(); iter != MarkTypeList.end(); ++iter)
	{
		tagZoneMapData data;
		tstring strMapName = VarMarkTypeList->GetString(_T("id"), (*iter).c_str());
		data.dwID = pUtil->Crc32( strMapName.c_str() );
		TCHAR szTypeKey[32];
		for( int nType = 1; ; nType++ )
		{
			_stprintf( szTypeKey, _T("type%d"), nType );
			tstring strName = VarMarkTypeList->GetString( szTypeKey, (*iter).c_str(), _T("") );
			if( strName.empty() )
				break;

			tagMapMarkType typeData;
			typeData.strType = strName;
			data.vecMarkTypes.push_back( typeData );
		}

		// 加载标记数据列表
		tstring strFileName = g_strLocalPath + _T("\\map\\");
		strFileName += strMapName;
		strFileName += _T("\\mark_list.xml");
		list<tstring> MarkDataList;
		if(!VarMarkDataList->Load("VFS_System", strFileName.c_str(), "id", &MarkDataList))
		{
			IMSG(_T("Load file %s failed\r\n"), strFileName.c_str() );
			continue;
		}

		for(list<tstring>::iterator itrData = MarkDataList.begin(); itrData != MarkDataList.end(); ++itrData)
		{
			const int nType = (int)VarMarkDataList->GetDword( _T("type"), (*itrData).c_str(), 0 ) - 1;
			if( nType < 0 || nType > (int)data.vecMarkTypes.size()-1 )
				continue;

			tagMapMarkData markData;
			markData.strName = VarMarkDataList->GetString( _T("show_name"), (*itrData).c_str(), _T("empty") );
			markData.vPos.x  = VarMarkDataList->GetFloat( _T("pos_x"), (*itrData).c_str(), 0.0f );
			markData.vPos.y  = VarMarkDataList->GetFloat( _T("pos_y"), (*itrData).c_str(), 0.0f );
			markData.vPos.z  = VarMarkDataList->GetFloat( _T("pos_z"), (*itrData).c_str(), 0.0f );
			data.vecMarkTypes[nType].vecMarks.push_back( markData );
		}
		VarMarkDataList->Clear();
		m_mapZoneMapData[data.dwID] = data;
	}
	KillObj("MapMarkDataList");
	KillObj("MapMarkTypeList");
}
