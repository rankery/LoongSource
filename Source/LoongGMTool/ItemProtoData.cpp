#include "stdafx.h"
#include "ItemProtoData.h"

ItemProtoData::ItemProtoData()
{
}

ItemProtoData::~ItemProtoData()
{
}

ItemProtoData g_itemproto;
ItemProtoData* ItemProtoData::Inst()
{
	return &g_itemproto;
}

void ItemProtoData::LoadFromFile()
{
	LPCTSTR szItemDisplay = _T("item_name.xml");
	LPCTSTR szEquipDisplay = _T("equip_name.xml");

	std::list<tstring> ItemProtoFieldList;
	std::list<tstring> ItemDisplayFieldList;
	std::list<tstring> EquipProtoFieldList;
	std::list<tstring> EquipDisplayFieldList;
	std::list<tstring>::iterator iter;

	VarContainer VarItemDisplay;
	if(!VarItemDisplay.Load("VFS_System", szItemDisplay, "id", &ItemDisplayFieldList))
		return;
	for(iter = ItemDisplayFieldList.begin(); iter != ItemDisplayFieldList.end(); ++iter)
	{

		tagItem2GM item;
		item.dwTypeID = VarItemDisplay.GetDword( _T("id"),			(*iter).c_str(), GT_INVALID );
		_tcsncpy( item.szName, VarItemDisplay.GetString(_T("name"),  (*iter).c_str(), _T("")), sizeof(item.szName)/sizeof(TCHAR));
		m_mapItemProto.insert(std::make_pair(item.dwTypeID, item));
	}	

	VarContainer VarEquipDisplay;
	if(!VarEquipDisplay.Load("VFS_System", szEquipDisplay, "id", &EquipDisplayFieldList))
		return;

	for(iter = EquipDisplayFieldList.begin(); iter != EquipDisplayFieldList.end(); ++iter)
	{
		tagItem2GM item;
		item.dwTypeID = VarEquipDisplay.GetDword( _T("id"),			(*iter).c_str(), GT_INVALID );
		_tcsncpy( item.szName, VarEquipDisplay.GetString(_T("name"),  (*iter).c_str(), _T("")), sizeof(item.szName)/sizeof(TCHAR));
		m_mapItemProto.insert(std::make_pair(item.dwTypeID, item));
	}

}