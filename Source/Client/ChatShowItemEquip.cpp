#include "stdafx.h"
#include "ChatShowItemEquip.h"
#include "..\WorldDefine\ItemDefine.h"

#define MAX_CHATSHOW_ITEMEQUIP 10
#define MAX_CHATSHOW_ITEMEQUIP_TIME 30
ChatShowItemEquip::ChatShowItemEquip( void )
{
	m_dwItemIndex = 0;
	m_dwEquipIndex = 0;
	m_dwCurShowItemIndex = 0;
	m_dwCurShowEquipIndex = 0;
}

ChatShowItemEquip::~ChatShowItemEquip( void )
{

}

ChatShowItemEquip g_ChatShowItemEquip;
ChatShowItemEquip* ChatShowItemEquip::Inst()
{
	return &g_ChatShowItemEquip;
}

bool ChatShowItemEquip::AddItemData( const TCHAR* szKey, DWORD data )
{
	if(szKey==NULL)
		return false;

	//30秒内不能展示同一样物品
	map<tstring, float>::iterator it;
	it = m_mapItemTime.find(szKey);
	if(it!=m_mapItemTime.end()
		&& Kernel::Inst()->GetAccumTime()-it->second<MAX_CHATSHOW_ITEMEQUIP_TIME)
		return false;

	if(m_mapItem.size() > MAX_CHATSHOW_ITEMEQUIP)
	{
		tstringstream ss;
		ss << _T("tagItem") << m_dwItemIndex++ ;
		DelItemData(ss.str().c_str());
		DelItemTime(ss.str().c_str());
	}

	//保存数据
	m_mapItem[szKey] = data;
	m_mapItemTime[szKey] = Kernel::Inst()->GetAccumTime();
	return true;
}

bool ChatShowItemEquip::AddEquipData( const TCHAR* szKey, const tagEquip& data )
{
	if(szKey==NULL)
		return false;

	//30秒内不能展示同一样装备
	map<tstring, float>::iterator it;
	it = m_mapEquipTime.find(szKey);
	if(it!=m_mapEquipTime.end()
		&& Kernel::Inst()->GetAccumTime()-it->second<MAX_CHATSHOW_ITEMEQUIP_TIME)
		return false;

	if(m_mapEquip.size() > MAX_CHATSHOW_ITEMEQUIP)
	{
		tstringstream ss;
		ss << _T("tagEquip") << m_dwEquipIndex++ ;
		DelEquipData(ss.str().c_str());
		DelEquipTime(ss.str().c_str());
	}

	//保存数据
	m_mapEquip[szKey] = data;
	m_mapEquipTime[szKey] = Kernel::Inst()->GetAccumTime();
	return true;
}

void ChatShowItemEquip::DelItemData( const TCHAR* szKey )
{
	m_mapItem.erase(szKey);
}

void ChatShowItemEquip::DelEquipData( const TCHAR* szKey )
{
	m_mapEquip.erase(szKey);
}

void ChatShowItemEquip::DelItemTime(const TCHAR* szKey)
{
	m_mapItemTime.erase(szKey);
}

void ChatShowItemEquip::DelEquipTime(const TCHAR* szKey)
{
	m_mapEquipTime.erase(szKey);
}

DWORD ChatShowItemEquip::FindItemData( const TCHAR* szKey )
{
	map<tstring, DWORD>::const_iterator iter = m_mapItem.find(szKey);

	if( iter!=m_mapItem.end() )
		return iter->second;
	else
		return GT_INVALID;
}

const tagEquip* ChatShowItemEquip::FindEquipData( const TCHAR* szKey )
{
	map<tstring, tagEquip>::const_iterator iter = m_mapEquip.find(szKey);

	if( iter!=m_mapEquip.end() )
		return &iter->second;
	else
		return NULL;
}

EItemConType ChatShowItemEquip::DragType2ContainerType(EDragType eType)
{
	switch(eType)
	{
	case EDT_Item:
		return EICT_Bag;
		break;
	case EDT_ItemEquip:
		return EICT_Equip;
		break;
	case EDT_ItemWare:
		return EICT_RoleWare;
		break;
	default:
		return EICT_Bag;
		break;
	}	
}

tstring ChatShowItemEquip::GetColorByQuality( BYTE byQuality )
{
	switch(byQuality)
	{
	case 0:
		return _T("<color=(224,247,255)>");
		break;
	case 1:
		return _T("<color=(23,247,255)>");
		break;
	case 2:
		return _T("<color=(0,255,5)>");
		break;
	case 3:
		return _T("<color=(255,162,0)>");
		break;
	case 4:
		return _T("<color=(0,120,255)>");
		break;
	case 5:
		return _T("<color=(255,0,255)>");
	default:
		return _T("<color=(141,141,141)>");
		break;
	}
}