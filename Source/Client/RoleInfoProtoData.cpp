#include "StdAfx.h"
#include "RoleInfoProtoData.h"

RoleInfoProtoData::RoleInfoProtoData(void)
{
}

RoleInfoProtoData::~RoleInfoProtoData(void)
{
}

RoleInfoProtoData m_InfoProto;
RoleInfoProtoData * RoleInfoProtoData::inst()
{
	return &m_InfoProto;
}

VOID RoleInfoProtoData::LoadFromFile()
{
	tstring strConstellationPath = g_strLocalPath + _T("\\attdata\\Constellation.xml");
	tstring strChineseZodiacPath = g_strLocalPath + _T("\\attdata\\ChineseZodiac.xml");
	tstring strAreaPath = g_strLocalPath + _T("\\attdata\\area.xml");
	tstring strProvince = g_strLocalPath + _T("\\attdata\\province.xml");

	list<tstring> lstConstellationField;
	list<tstring> lstChineseZodiacField;
	list<tstring> lstAreaField;
	list<tstring> lstProvinceField;

	TObjRef<VarContainer> varConstellationData =
		CreateObj("ConstellationData", "VarContainer");
	if(!varConstellationData->Load("VFS_System",
		strConstellationPath.c_str(), "id", &lstConstellationField))
		IMSG(_T("Load file Constellation.xml failed"));

	TObjRef<VarContainer> varChineseZodiacData =
		CreateObj("ChineseZodiacData", "VarContainer");
	if(!varChineseZodiacData->Load("VFS_System",
		strChineseZodiacPath.c_str(), "id", &lstChineseZodiacField))
		IMSG(_T("Load file ChineseZodiac.xml failed"));

	TObjRef<VarContainer> varAreaData =
		CreateObj("AreaData", "VarContainer");
	if(!varAreaData->Load("VFS_System",
		strAreaPath.c_str(), "id", &lstAreaField))
		IMSG(_T("Load file area.xml failed"));

	TObjRef<VarContainer> varProvinceData =
		CreateObj("ProvinceData", "VarContainer");
	if(!varProvinceData->Load("VFS_System",
		strProvince.c_str(), "id", &lstProvinceField))
		IMSG(_T("Load file province.xml failed"));

	for (list<tstring>::const_iterator citer = lstConstellationField.begin();
		citer != lstConstellationField.end(); ++citer)
	{
		tagConstellationProto proto;
		proto.dwID =
			varConstellationData->GetDword(_T("id"), (*citer).c_str(), GT_INVALID);
		proto.strName =
			varConstellationData->GetString(_T("name"), (*citer).c_str(), _T(""));

		m_mapConstellation.insert(make_pair(proto.dwID, proto));
	}
	KillObj("ConstellationData");

	for (list<tstring>::const_iterator citer = lstChineseZodiacField.begin();
		citer != lstChineseZodiacField.end(); ++citer)
	{
		tagChinesZodiacProto proto;
		proto.dwID =
			varChineseZodiacData->GetDword(_T("id"), (*citer).c_str(), GT_INVALID);
		proto.strName =
			varChineseZodiacData->GetString(_T("name"), (*citer).c_str(), _T(""));

		m_mapChineseZodiac.insert(make_pair(proto.dwID, proto));
	}
	KillObj("ChineseZodiacData");

	for (list<tstring>::const_iterator citer = lstAreaField.begin();
		citer != lstAreaField.end(); ++citer)
	{
		tagAreaProto proto;
		proto.dwID = varAreaData->GetDword(_T("id"), (*citer).c_str(), GT_INVALID);
		proto.strName = varAreaData->GetString(_T("name"), (*citer).c_str(), _T(""));
		proto.mapProvince.clear();

		m_mapArea.insert(make_pair(proto.dwID, proto));
	}
	KillObj("AreaData");

	for (list<tstring>::const_iterator citer = lstProvinceField.begin();
		citer != lstProvinceField.end(); ++citer)
	{
		tagProvinceProto proto;
		proto.dwID =
			varProvinceData->GetDword(_T("id"), (*citer).c_str(), GT_INVALID);
		proto.strName =
			varProvinceData->GetString(_T("name"), (*citer).c_str(), _T(""));
		DWORD dwAreaID =
			varProvinceData->GetDword(_T("areaid"), (*citer).c_str(), GT_INVALID);

		map<DWORD, tagAreaProto>::iterator iter = m_mapArea.find(dwAreaID);
		if (iter != m_mapArea.end())
			iter->second.mapProvince.insert(make_pair(proto.dwID, proto));

	}
	KillObj("ProvinceData");

}