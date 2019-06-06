#pragma once
#include "GameClientDefine.h"
#include "..\WorldDefine\container_define.h"
struct tagItem;
struct tagEquip;

class ChatShowItemEquip :
	public GameFrame
{
public:
	ChatShowItemEquip(void);
	~ChatShowItemEquip(void);

	static ChatShowItemEquip* Inst();

	//--添加
	bool AddItemData(const TCHAR* szKey, DWORD data);
	bool AddEquipData(const TCHAR* szKey, const tagEquip& data);

	//删除
	void DelItemData(const TCHAR* szKey);
	void DelEquipData(const TCHAR* szKey);

	//--查找
	DWORD FindItemData(const TCHAR* szKey);
	const tagEquip* FindEquipData(const TCHAR* szKey);

	//拖动类型转换成容器类型
	EItemConType DragType2ContainerType(EDragType eType);

	//根据品质获得颜色
	tstring GetColorByQuality(BYTE byQuality);

	//--删除保存的物品或装备开始时间
	void DelItemTime(const TCHAR* szKey);
	void DelEquipTime(const TCHAR* szKey);

	//--
	DWORD GetCurrentShowItemIndex() { return m_dwCurShowItemIndex++; }
	DWORD GetCurrentShowEquipIndex() { return m_dwCurShowEquipIndex++; }

private:
	map<tstring, DWORD>				m_mapItem;
	map<tstring, tagEquip>			m_mapEquip;
	map<tstring, float>				m_mapItemTime;
	map<tstring, float>				m_mapEquipTime;

	DWORD							m_dwItemIndex;
	DWORD							m_dwEquipIndex;

	DWORD							m_dwCurShowItemIndex;
	DWORD							m_dwCurShowEquipIndex;
};