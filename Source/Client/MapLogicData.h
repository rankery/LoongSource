#pragma once

#include "..\WorldDefine\MapAttDefine.h"
#include ".\BaseData.h"

/** 地图显示区域逻辑数据
*/
struct tagMapShowAreaData
{
	DWORD       dwObjID;
	tstring     szTitle;			// 区域显示的名称
	AABBox      box;                // 包围盒
	DWORD		dwMiniMapSize;      // 缩略图大小
};

/** 地图逻辑数据
*/
struct tagMapLogicData
{
	DWORD						dwID;				//地图ID，根据szName生成的CRC
	tstring						szName;				//程序内部名称
	tstring						szShowName;			//显示名称
	Vector2						vSize;				//大小
	EMapType					eMapType;			//地图类型
	vector<tagMapShowAreaData>  vecShowAreaData;    //显示区域数据列表
	//more
};

/** \class MapLogicData
	\brief 地图逻辑数据
*/
class MapLogicData
	: public BaseData
{
public:
	MapLogicData(void);
	~MapLogicData(void);

	static MapLogicData* Inst();

	/** 载入属性文件
	*/
	virtual void LoadFromFile();
	/** 获取地图逻辑数据
	*/
	const tagMapLogicData* FindMapLogicData(DWORD dwMapID)
	{
		return FindInMap(m_mapMapLogicData, dwMapID);
	}
	/** 获取地图显示名称
	*/
	LPCTSTR FindMapShowName(DWORD dwMapID);
	/** 获取地图显示区域数据
	*/
	bool GetMapShowAreaList(DWORD dwMapID,vector<tagMapShowAreaData>& vecData);
	const tagMapShowAreaData* FindMapShowAreaData(DWORD dwMapID, DWORD dwAreaObjID);
	/** 获取数据容器
	*/
	const map<DWORD, tagMapLogicData>& GetDataMap() { return m_mapMapLogicData; }
private:
	map<DWORD, tagMapLogicData>		m_mapMapLogicData;		// 地图逻辑数据

};