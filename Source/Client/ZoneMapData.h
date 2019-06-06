#pragma once

#include ".\BaseData.h"

/** \地图标记数据
*/
struct tagMapMarkData
{
	tstring             strName;
	Vector3             vPos;
};

/** \地图标记分类
*/
struct tagMapMarkType
{
	tstring					strType;
	vector<tagMapMarkData>	vecMarks;
};


/** \区域地图数据
*/
struct tagZoneMapData
{
	DWORD						dwID;			// 地图ID，根据szName生成的CRC
	vector<tagMapMarkType>		vecMarkTypes;	// 地图标记分类列表
};

/** \class ZoneMapData
	\brief 区域地图数据
*/
class ZoneMapData
	: public BaseData
{
public:
	ZoneMapData(void);
	~ZoneMapData(void);

	static ZoneMapData* Inst();
	/** \载入属性文件
	*/
	virtual void LoadFromFile();
	/** \查找区域地图数据
	*/
	const tagZoneMapData* FindZoneMapData(DWORD dwMapID)
	{
		return FindInMap(m_mapZoneMapData, dwMapID);
	}
private:
	map<DWORD, tagZoneMapData>		m_mapZoneMapData;
};