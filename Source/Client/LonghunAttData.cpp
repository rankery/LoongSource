#include "StdAfx.h"
#include "LonghunAttData.h"
#include "..\WorldDefine\ItemDefine.h"
#include "..\WorldDefine\longhun_define.h"

LonghunAttData::LonghunAttData(void)
{
}

LonghunAttData::~LonghunAttData(void)
{
}

LonghunAttData g_Longhun;
LonghunAttData* LonghunAttData::Inst()
{
	return &g_Longhun;
}

void LonghunAttData::LoadFromFile()
{
	const TCHAR* szLonghunProto = _T("data\\system\\attdata\\longhun_proto.xml");
	tstring strPath = g_strLocalPath + _T("\\attdata\\longhun_name.xml");
	LPCTSTR szLonghunName = strPath.c_str(); 

	list<tstring> protoList;
	list<tstring> nameList;
	list<tstring>::iterator iter;

	//∂¡»°longhun_proto
	TObjRef<VarContainer> varProto = CreateObj( "LonghunProto", "VarContainer" );
	if(!varProto->Load("VFS_System", szLonghunProto, "id", &protoList))
		IMSG(_T("Load file longhun_proto.xml failed\r\n"));

	for(iter=protoList.begin(); iter!=protoList.end(); ++iter)
	{
		tagLongHunProtoClient longhun;
		longhun.dwID						= varProto->GetDword(_T("id"),			(*iter).c_str(), GT_INVALID);
		longhun.eType						= (ELongHunType)varProto->GetInt(_T("Type"),		(*iter).c_str(), NULL);
		longhun.eTypeEx						= (ELongHunTypeEx)varProto->GetInt(_T("TypeEx"),	(*iter).c_str(), ELHTE_Null);
		longhun.byWuXingActive[EWX_Metal]	= (BYTE)varProto->GetInt(_T("Metal"),	(*iter).c_str(), NULL);
		longhun.byWuXingActive[EWX_Wood]	= (BYTE)varProto->GetInt(_T("Wood"),	(*iter).c_str(), NULL);
		longhun.byWuXingActive[EWX_Water]	= (BYTE)varProto->GetInt(_T("Water"),	(*iter).c_str(), NULL);
		longhun.byWuXingActive[EWX_Fire]	= (BYTE)varProto->GetInt(_T("Fire"),	(*iter).c_str(), NULL);
		longhun.byWuXingActive[EWX_Soil]	= (BYTE)varProto->GetInt(_T("Soil"),	(*iter).c_str(), NULL);
		
		m_mapLunghun.insert(make_pair(longhun.dwID, longhun));
	}

	KillObj("LonghunProto");

	//∂¡»°longhun_name
	TObjRef<VarContainer> varName = CreateObj( "LonghunName", "VarContainer" );
	if(!varName->Load("VFS_System", szLonghunName, "id", &nameList))
		IMSG(_T("Load file longhun_name.xml failed\r\n"));

	for(iter=nameList.begin(); iter!=nameList.end(); ++iter)
	{
		DWORD dwID = varName->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		tagLongHunProtoClient* pUnit = FindInMap(m_mapLunghun, dwID);
		if (P_VALID(pUnit))
		{
			_tcsncpy( pUnit->szName,	varName->GetString( _T("name"),	(*iter).c_str(), _T("") ), sizeof(pUnit->szName)/sizeof(TCHAR));
			_tcsncpy( pUnit->szDesc,	varName->GetString( _T("desc"),	(*iter).c_str(), _T("") ), sizeof(pUnit->szDesc)/sizeof(TCHAR));
		}
	}

	KillObj("LonghunName");
}

const tagLongHunProtoClient* LonghunAttData::FindLonghunData( DWORD dwID )
{
	return FindInMap(m_mapLunghun, dwID);
}