#include "stdafx.h"
#include "DakData.h"
#include "MapLogicData.h"

DakData::DakData( void )
{

}

DakData::~DakData( void )
{

}

DakData g_DakData;
DakData* DakData::Inst()
{
	return &g_DakData;
}

void DakData::LoadFromFile()
{
	const TCHAR* szDakProto = _T("data\\system\\attdata\\map_switch_proto.xml");
	tstring strPath = g_strLocalPath + _T("\\attdata\\map_switch_name.xml");
	const TCHAR* szDakName = strPath.c_str();

	list<tstring> dakList;
	list<tstring> dakNameList;
	list<tstring>::iterator iter;
	TObjRef<Util> pUtil;

	//∂¡»°Ê‰’æ
	TObjRef<VarContainer> varDak = CreateObj( "DakProto", "VarContainer" );
	if(!varDak->Load("VFS_System", szDakProto, "id", &dakList))
		IMSG(_T("Load file map_switch_proto.xml failed\r\n"));

	TCHAR szBuff[X_LONG_NAME];
	for(iter=dakList.begin(); iter!=dakList.end(); ++iter)
	{
		tagDakProtoEx dak;
		dak.dwID = varDak->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		for(int i=0; i<MAX_DAK_SITE_NUM; i++)
		{
			_stprintf(szBuff, _T("map%d"), i+1);
			dak.dakSite[i].dwMapID = pUtil->Crc32(varDak->GetString(szBuff, (*iter).c_str(), _T("")));
			dak.dakSite[i].strName = MapLogicData::Inst()->FindMapShowName(dak.dakSite[i].dwMapID);
			_stprintf(szBuff, _T("x_map%d"), i+1);
			dak.dakSite[i].fX = (FLOAT)varDak->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf(szBuff, _T("y_map%d"), i+1);
			dak.dakSite[i].fY = (FLOAT)varDak->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf(szBuff, _T("z_map%d"), i+1);
			dak.dakSite[i].fZ = (FLOAT)varDak->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf(szBuff, _T("cost_type_map%d"), i+1);
			dak.dakSite[i].eCostType = (ECurCostType)varDak->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf(szBuff, _T("cost_num_map%d"), i+1);
			dak.dakSite[i].nCostVal = (INT32)varDak->GetDword(szBuff, (*iter).c_str(), 0);
		}
		m_pDakProto.insert(make_pair(dak.dwID, dak));
	}

	KillObj("DakProto");

	TObjRef<VarContainer> varDakName = CreateObj( "DakName", "VarContainer" );
	if(!varDakName->Load("VFS_System", szDakName, "id", &dakNameList))
		IMSG(_T("Load file map_switch_proto.xml failed\r\n"));

	for(iter=dakNameList.begin(); iter!=dakNameList.end(); ++iter)
	{
		DWORD dwID = varDakName->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		tagDakProtoEx* pDak = FindDakData(dwID);
		if( P_VALID(pDak) )
		{
			for(int i=0; i<MAX_DAK_SITE_NUM; i++)
			{
				_stprintf(szBuff, _T("name%d"), i+1);
				 pDak->dakSite[i].strDesc = varDakName->GetString(szBuff, (*iter).c_str(), _T("")) ;
			}
		}
	}

	KillObj("DakName");
}