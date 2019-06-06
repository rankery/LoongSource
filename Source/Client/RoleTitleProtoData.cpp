#include "StdAfx.h"
#include "RoleTitleProtoData.h"

RoleTitleProtoData::RoleTitleProtoData(void)
{
}

RoleTitleProtoData::~RoleTitleProtoData(void)
{
}

RoleTitleProtoData m_titleProto;
RoleTitleProtoData* RoleTitleProtoData::Inst()
{
	return &m_titleProto;
}

VOID RoleTitleProtoData::LoadFromFile()
{
	tstring strTitleProtoPath = _T("data\\system\\attdata\\role_title_proto.xml");
	tstring strTitleNamePath = g_strLocalPath + _T("\\attdata\\role_title_name.xml");
	//tstring strTitleTypeNamePath = g_strLocalPath + _T("\\attdata\\role_title_type.xml");

	list<tstring> lstTitleProtoField;
	list<tstring> lstTitleNameField;
	//list<tstring> lstTitleTypeNameField;

	TObjRef<VarContainer> VarTitleProto = CreateObj("TitleProto", "VarContainer");
	if(!VarTitleProto->Load("VFS_System", strTitleProtoPath.c_str(), "id", &lstTitleProtoField))
		IMSG(_T("Load file role_title_proto.xml failed"));
	

	TObjRef<VarContainer> VarTitleName = CreateObj("TitleName", "VarContainer");
	if(!VarTitleName->Load("VFS_System", strTitleNamePath.c_str(), "id", &lstTitleNameField))
		IMSG(_T("Load file role_title_name.xml failed"));
	

	//TObjRef<VarContainer> VarTitleTypeName = CreateObj("TitleTypeName", "VarContainer");
	//if(!VarTitleTypeName->Load("VFS_System", strTitleTypeNamePath.c_str(), "typeid", &lstTitleTypeNameField))
	//	IMSG(_T("Load file role_title_name.xml failed"));
	

	//构建typename map，方便查找
	tagRoleTitleTypeInfo rtti[ETT_FIN];

	rtti[ETT_RenWu].strName = g_StrTable[_T("RoleTitleType1")];
	rtti[ETT_RenWu].nTitlesCount = 0;
	m_mapTypename.insert(
		make_pair((DWORD)ETT_RenWu, rtti[ETT_RenWu])
		);

	rtti[ETT_ShiZu].strName = g_StrTable[_T("RoleTitleType2")];
	rtti[ETT_ShiZu].nTitlesCount = 0;
	m_mapTypename.insert(
		make_pair((DWORD)ETT_ShiZu, rtti[ETT_ShiZu])
		);

	rtti[ETT_ChuanQi].strName = g_StrTable[_T("RoleTitleType3")];
	rtti[ETT_ChuanQi].nTitlesCount = 0;
	m_mapTypename.insert(
		make_pair((DWORD)ETT_ChuanQi, rtti[ETT_ChuanQi])
		);

	rtti[ETT_TeShu].strName = g_StrTable[_T("RoleTitleType4")];
	rtti[ETT_TeShu].nTitlesCount = 0;
	m_mapTypename.insert(
		make_pair((DWORD)ETT_TeShu, rtti[ETT_TeShu])
		);

	rtti[ETT_HuoDong].strName = g_StrTable[_T("RoleTitleType5")];
	rtti[ETT_HuoDong].nTitlesCount = 0;
	m_mapTypename.insert(
		make_pair((DWORD)ETT_HuoDong, rtti[ETT_HuoDong])
		);

	//for (list<tstring>::iterator iter = lstTitleTypeNameField.begin();
	//	iter != lstTitleTypeNameField.end(); ++iter)
	//{
	//	tagRoleTitleTypeInfo rtti;
	//	rtti.strName = VarTitleTypeName->GetString(_T("typename"), (*iter).c_str(), _T(""));
	//	rtti.nTitlesCount = 0;

	//	m_mapTypename.insert(
	//		make_pair(
	//		VarTitleTypeName->GetDword(_T("typeid"), (*iter).c_str(), GT_INVALID),
	//		rtti)
	//		);
	//}

	//KillObj("TitleTypeName");


	//读取原型数据
	for (list<tstring>::iterator iter = lstTitleProtoField.begin();
		iter != lstTitleProtoField.end(); ++iter)
	{
		tagRoleTitleProto titleProto;
		titleProto.m_dwTitleID
			= VarTitleProto->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		titleProto.m_dwTypeID
			= VarTitleProto->GetDword(_T("typeid"), (*iter).c_str(), GT_INVALID);
		titleProto.m_eColor
			= (EnumTitleColor)VarTitleProto->GetInt(_T("colorid"), (*iter).c_str(), 1);
		titleProto.m_dwColor = GetColor(titleProto.m_eColor);

		tagRoleTitleTypeInfo* pTypeInfo
			= FindInMap(m_mapTypename, titleProto.m_dwTypeID);
		if (P_VALID(pTypeInfo))
		{
			_tcsncpy(titleProto.m_szTypeName,
				pTypeInfo->strName.c_str(),
				sizeof(titleProto.m_szTypeName)/sizeof(TCHAR));
			pTypeInfo->nTitlesCount++;
		}

		titleProto.m_dwBuffID
			= VarTitleProto->GetDword(_T("buffid"), (*iter).c_str(), GT_INVALID);

		m_mapTitleProto.insert(make_pair(titleProto.m_dwTitleID, titleProto));
	}
	KillObj("TitleProto");

	//读取名字和描述
	for (list<tstring>::iterator iter = lstTitleNameField.begin();
		iter != lstTitleNameField.end(); ++iter)
	{
		tagRoleTitleProto* pTitleProto = FindInMap(m_mapTitleProto,
			VarTitleName->GetDword(_T("id"), (*iter).c_str(), GT_INVALID));
		if (P_VALID(pTitleProto))
		{
			_tcsncpy(pTitleProto->m_szTitleName,
				VarTitleName->GetString(_T("name"), (*iter).c_str(), _T("")),
				sizeof(pTitleProto->m_szTypeName)/sizeof(TCHAR));
			pTitleProto->m_strDescription
				= VarTitleName->GetString(_T("desc"), (*iter).c_str(), _T(""));
		}
	}
	KillObj("TitleName");

}

