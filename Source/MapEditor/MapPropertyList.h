#pragma once
#include "propertylist.h"

class CMapPropertyList :public CPropertyList
{
public:
	CMapPropertyList(void);
	virtual ~CMapPropertyList(void);

	//--显示物体属性

	void DisplayNPC(const tagMapNPC* pNPC);
	void DisplayStaticObj(const tagStaticMapObj* pStaticMapObj);
	void DisplayDynamicBlock(const tagMapDynamicBlock* pDynamicBlock);
	void DisplayWayPointObj(const tagMapWayPoint* pWayPointObj);
	void DisplayTriggerObj(const tagMapTrigger* pTriggerObj);
	void DisplaySoundObj(const tagMapSound* pSoundObj);
	void DisplayPointLightObj(const tagMapPointLight* pPointLight);
	void DisplayMapRectObj(const tagMapArea* pMapRect);
	void DisplayPathPoint(const tagMapPathPoint* pPathPoint);
	void DisplaySpawnPoint(const tagMapSpawnPoint* pSpawnPoint);
	void DisplayEventTrigger(const tagMapEventTrigger* pEventTrigger);
	void DisplayNPCNav(const tagNPCNavMapBuildStartPoint* pNPCNav);
	void DisplayPlayerNav(const tagPlayerNavMapBuildStartPoint* pPlayerNav);
	void DisplayMapDoor(const tagMapDoor* pDoor);
	void DisplayMapCarrier(const tagMapCarrier* carrier);
	void DisplayMapTriggerEffect(const tagMapTriggerEffect* pTEffect);

	//--获得修改的导航点数据
	bool GetModifiedWayPointProp(tagMapWayPoint& WayPoint);
	//--获得修改的触发器数据
	bool GetModifiedTriggerProp(tagMapTrigger& Trigger);
	//--得到修改NPC数据
	bool GetModifiedNPCObjProp(tagMapNPC* p,Vector3& pos,float &fYaw,bool &bIsLoadMDL);
	//--得到修改静态物体数据
	bool GetModifiedStaticObjProp(tagStaticMapObj* p,Vector3& pos,bool &bIsLoadMDL);
	//--得到修改的动态障碍物数据
	bool GetModifyedDynamicBlockProp(tagMapDynamicBlock* p, Vector3& pos, bool &bIsLoadMDL);
	//--得到修改的音效数据
	bool GetModifiedSoundProp(tagMapSound* pSound);
	//--得到修改的点光源数据
	bool GetModifiedPointLightProp(tagMapPointLight* pPointLight);
	//--得到修改的地图区域数据
	bool GetModifyedMapRectProp(tagMapArea* pMapRect);
	//--得到修改的寻路导航点
	bool GetModifyedPathPointProp(tagMapPathPoint* pPathPoint);
	//--得到修改的刷怪点
	bool GetModifyedSpawnPointProp(tagMapSpawnPoint* pSpawnPoint);
	//--得到修改的事件触发器
	bool GetModifyedEventTrigger(tagMapEventTrigger* pEventTrigger);
	//--
	bool GetModifyedNPCNav(tagNPCNavMapBuildStartPoint* pNPCNav);
	//--
	bool GetModifyedPlayerNav(tagPlayerNavMapBuildStartPoint* pPlayerNav);
	//--
	bool GetModifiedMapDoor(tagMapDoor* p, Vector3& pos, bool &bIsLoadMDL);
	//--
	bool GetModifiedMapCarrier(tagMapCarrier* p, bool &bIsLoadMDL);
	//--得到修改的触发特效
	bool GetModifyedMapTriggerEffect(tagMapTriggerEffect* p, Vector3& pos, bool &bIsLoadMDL);
private:
	int		m_nTriggerNum ;
	int		m_nMapAreaNum;
};
