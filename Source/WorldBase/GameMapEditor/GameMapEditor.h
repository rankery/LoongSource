#pragma once
#include "..\GameMap\GameMap.h"
#include "MapObjEdit.h"
#include "..\..\Cool3D\SceneGraph\SGSkyBox.h"
#include "CreatureDataReader.h"
using namespace Cool3D;

#define DISTENCE_WATPOINT_PICK 50   //waypoint's pick distance
#define DISTENCE_TRIGGER_PICK 50
#define HEIGHT_SHOW_BOX 100    //触发器区域 拾取触发器和导航点范围盒子的高度 chopfield盒子高度
#define DISTENCE_SOUNDBOX_PICK 100  
#define DISTENCE_CHOPFIELD_TREEZONE 64
#define DISTENCE_CHOPFIELD_WHEATZONE 32
#define DISTENCE_MAZEWALL_ZONE 20
#define DISTENCE_POINTLIGHT_PICK 100	//点光源
#define DIS_PATHPOINT_Y 50
#define DIS_PATHPOINT_XZ 30
#define DIS_SPAWNPOINT_Y 30
#define DIS_SPAWNPOINT_XZ 20
#define DIS_NPCNAVSTARTPOINT_PICK 50
#undef VFS_READ
namespace WorldBase
{
	class MapObjEdit;
	class WORLD_BASE_API GameMapEditor : public GameMap
	{
	public:
		GameMapEditor(void);
		virtual ~GameMapEditor(void);

		//--GameMap
		virtual void Destroy();

		//--
		void Create(TerrainEditor *pTerrain,NullSceneGraph *pSG,NavNullSceneGraph *pNavSG);
		void LoadAttDataBase();

		bool MousePick(UINT cursorX,UINT cursorY,Vector3& out);

		bool WriteEditFile(const TCHAR* szFileName);
		bool LoadFromEditFile(const TCHAR* szFileName);

		bool SelectObjByMouse(UINT cursorX,UINT cursorY);
		bool SelectObjByMouseEx(UINT cursorX,UINT cursorY,MapObjEdit::EType type);
		bool SelectObjByID(WORD mapID);

		void AddMapObjEdit(MapObjEdit* pEdit);

		bool RemoveMapObjEdit(MapObjEdit* pEdit);
		MapObjEdit* RemoveMapObjEdit(MapObjEdit::EType Type, DWORD mapID);

		const list<MapObjEdit*>& GetMapObjEditList();
		const list<tagNPCNavMapBuildStartPoint*>& GetNPCNavMapBuildStartPoint() { return m_NPCNavStartPointList; }
		const list<tagPlayerNavMapBuildStartPoint*>& GetPlayerNavMapBuildStartPoint() { return m_PlayerNavStartPointList; }

		void ClearEditList(bool bAll=true);

		MapObjEdit *GetSelectedObj() const			{	return m_pSelectedObj;}

		CreatureDataReader* GetAttDataBase()		{	return &m_createData;}
		
		SceneGraph*  GetSceneGraph()				{	return m_pSceneGraph;}
		NavNullSceneGraph* GetNavSG()				{	return m_pNavSG;	 }

		void SetSelectObj(MapObjEdit* pSelected)	{   m_pSelectedObj = pSelected;}

		//GNavMap* GetNav()						    {	return &m_navMap;}
		void DrawEdit();
		void DrawWayPoint();		//画导航点相关
		void DrawTrigger();			//画触发器相关
		void DrawSound();			//画音效相关
		void DrawPointLight();		//画点光源
		void DrawMapRect();			//画地图区域
		void DrawPathPoint();		//画寻路导航点
		void DrawSpawnPoint();		//画刷怪点
		void DrawNPCNavStartPoint(); //画NPC格子导航图生成起点
		void DrawPlayerNavStartPoint();//画玩家格子导航图生成起点

		
		void SetSunLight(const tagLight& sunlight) {	m_SunLight = sunlight; }
		void SetFog(const tagDistFog& fog)	{	m_Fog = fog;}
		void SetSkyCol(const Color4f& skyCol) { m_SkyCol = skyCol; }
		void SetRenderSkyShade(const bool bRenderskyShade);
		void SetSkyShadeCol(const DWORD dwSkyShadeCol);
		void SetDynamicDiffCol(const DWORD dwCol) { m_dwDynamicDiffCol = dwCol; }
		void SetDynamicAmbCol(const DWORD dwCol) { m_dwDynamicAmbCol = dwCol; }
		void SetDynamicSpecCol(const DWORD dwCol) { m_dwDynamicSpecCol = dwCol; }

		//设置天空盒
		void SetSkyBox(const TCHAR* szTopTex, const TCHAR* szFrontRightTex, const TCHAR* szBackLeftTex, 
			const Vector3& drawSize, const Vector3& offSize);
		//设置太阳光增强系数
		void SetSunModulus(FLOAT fVal) { m_fSunModulus = fVal; }
		//设置天空盒旋转角度
		void SetSkyYaw(FLOAT fVal);

		//得到地形中一点所对应的格子坐标，且格子z坐标已经变换
		void GetTerrainTileXZ(Vector3 &pos,POINT &tileXZ);
		//得到地形
		TerrainEditor * GetHeightMap()              {   return m_pTerrain;  }
		//查找物体
		MapObjEdit* FindMapObjEdit(MapObjEdit::EType Type, DWORD ObjID);

		//处理导航点
		//添加一个导航点，并返回同一组导航点中的位置索引
		void AddWayPoint(tagMapWayPoint *pWayPoint,int &nIndex);
		//根据索引得到寻路导航点
		tagMapWayPoint* FindWayPoint(DWORD dwIndex);
		//得到导航点列表
		void GetWayPointList(list<tagMapWayPoint*>& out)      {  out = m_WayPointList;  }
		//清空导航点
		void ClearWayPointList();
		//set current draw waypoint
		void SetCurPosForWayPoint(Vector3 pos,bool bIsCurDrawWayPoint)  
		{ 
			m_bCurDrawWayPoint=bIsCurDrawWayPoint;
			if(bIsCurDrawWayPoint)
			   m_vCurPosForWayPoint=pos;
		}
		//set pick way point
		void SetCurPosForPickWayPoint(Vector3 pos)            {  m_vCurPickWayPoint=pos;  }
		void SetDrawPickFlagForWayPoint(bool bIsPickWayPoint) {  m_bPickWayPoint=bIsPickWayPoint;  }
		//找到一个导航点数据，如果没找到返回NULL
        tagMapWayPoint* GetWayPointData(DWORD nObjId);
		tagMapWayPoint* GetWayPointData(const TCHAR* szName, BYTE byIndex);
		//查询是否已经存在同名的导航点组
		bool IsExistWayPointWithSameName(TCHAR* szName);
		//pick way point
		void  PickWayPoint(UINT x, UINT y);
		//get current picked way point
		tagMapWayPoint* GetCurPickedWayPoint()                {  return m_pSelectWayPoint;  }
		//set current picked way point 
		void SetCurPickedWayPoint(tagMapWayPoint* pWayPoint)  {  m_pSelectWayPoint=pWayPoint;  }
		//delete a waypoint
		void DeleteWayPoint(DWORD dwObjID);
		//直接修改特定导航点属性
		void ModifyWayPointProp(tagMapWayPoint WayPoint);
        //将当前导航点属性完全改变
		void ModifyWayPointProp(DWORD dwOriID, TCHAR * szOldName, int nOldIndex,tagMapWayPoint WayPoint);
		//设置/获得 是否会制导航点
		void SetIsDrawWayPoint(bool bVal) { m_bDrawWayPoint = bVal; }
		bool GetIsDrawWayPoint()const { return m_bDrawWayPoint; }

		//触发器相关
		//新加触发器
		void  AddTrigger(tagMapTrigger *pTrigger);
		//modify trigger
		void ModifyTriggerProp(tagMapTrigger Trigger);
		//根据ID删除一个触发器
		void DeleteTrigger(DWORD dwObjID);
		//根据索引删除一个触发器
		void DeleteTrigger(int nIndex);
		//get a trigger list
		void GetTriggerList(list<tagMapTrigger*>& out)        {  out = m_TriggerList;  }
		//clear trigger list
		void ClearTriggerList();
		//set current draw trigger
		void SetCurPosForTrigger(Vector3 pos,bool bIsCurDrawTrigger)  
		{ 
			m_bCurDrawTrigger=bIsCurDrawTrigger;
			if(m_bCurDrawTrigger)
				m_vCurPosForTrigger=pos;
		}
		void GetCurPosForTrigger(Vector3 &out)                {  out=m_vCurPosForTrigger;   }

		void SetPosForTrigger(Vector3& pos, int curPos)	
		{	 
			if(curPos>=(int)m_vTriggerPos.size())
				m_vTriggerPos.push_back(pos);
			else
				m_vTriggerPos[curPos] = pos; 
		}
		void SetPosForTrigger(Vector3& pos)		
		{	 
			m_vTriggerPos.push_back(pos); 
			m_nCurTriggerNum++;
		}
		void GetPosForTrigger(Vector3&out, int curPos)		  {  out = m_vTriggerPos[curPos];}
		void GetPosForTrigger(vector<Vector3>& vec)			  {	 vec = m_vTriggerPos; }
		//set pick trigger
		void SetCurPosForPickTrigger(Vector3 pos)             {  m_vCurPickTrigger=pos;     }
		void SetDrawPickFlagForTrigger(bool bIsPickTrigger)   {  m_bPickTrigger=bIsPickTrigger;  }
		//pick trigger
		void  PickTrigger(UINT x, UINT y);
		//拾取触发器的某一个点
		void PickTriggerPoint(UINT x, UINT y, bool& bHave);
		//get current trigger
		tagMapTrigger* GetCurPickedTrigger()                  {  return m_pSelectTrigger;  }
		//set current trigger
		void SetCurPickedTrigger(tagMapTrigger* pTrigger)     {  m_pSelectTrigger=pTrigger;  }
		//通过ID查找触发器
		tagMapTrigger* GetTriggerData(DWORD dwObjID);
		//获得当前建立触发器多边形区域到哪个点
		int GetCurTriggerNum()								  { return m_nCurTriggerNum; }
		void ClearTriggerNum()								  { m_nCurTriggerNum = 0; }
		//计算触发器的AABBOX
		AABBox CalTriggerAABBox();
		//重新计算触发器的AABBOX
		void ReCalTriggerAABBox(tagMapTrigger* pTri);
		//
		void ClearTriggerPos();
		//得到拾取触发器某个点是第几个点
		int GetTriggerNum()								{	return m_nTriggerNum; }
		void ClearPickTriggerPoint()					{	m_bPickTriggerPoint = false; m_nTriggerNum = 0; }
		//设置/获得 是否会制触发器
		void SetIsDrawTrigger(bool bVal) { m_bDrawTrigger = bVal; }
		bool GetIsDrawTrigger()const { return m_bDrawTrigger; }
		

		//音效相关
		//新增一个音效
		void AddSound(tagMapSound *pSound);
		//删除一个音效
		void DeleteSound(tagMapSound *pSound);
		//删除一个音效通过索引
		void DeleteSound(DWORD nIndex);
		//得到音效列表
		void GetSoundList(list<tagMapSound*>& out)            {  out = m_SoundList;  }
		//清空音效列表
		void ClearSoundList();
		//拾取音效
		void PickSoundBox(UINT x,UINT y);
		//设置当前选择的音效
		void SetSelectedSound(tagMapSound *pSound)            {  m_pSelectSound=pSound;  }
		//得到当前选择的音效
		tagMapSound *GetSelectedSound()                       {  return m_pSelectSound;  }
		//根据索引得到音效
		tagMapSound *FindSound(DWORD nIndex);
		//绘制音效标识
		void SetDrawSoundFlag(bool bVal)	{ m_bPickSound = bVal; }
		//音效标识位置
		void SetPosForSound(Vector3& pos)	{ m_vCurPosSound = pos; }

		//点光源相关
		//新增一个点光源
		void AddPointLight(MapObjEdit* pPointLight);
		//删除一个点光源
		void DeletePointLight(tagMapPointLight *pPointLight);
		//删除一个点光源通过索引
		void DeletePointLight(DWORD nIndex);
		//得到点光源列表
		void GetPointLightList(list<MapObjEdit*>& out)	{ out = m_PointLightList;}
		//清空点光源列表
		void ClearPointLightList();
		//拾取点光源
		void PickPointLight(UINT x, UINT y);
		//设置当前选择的点光源
		void SetSelectedPointLight(MapObjEdit *pPointLight) { m_pSelectPointLight = pPointLight;}
		//得到当前选择的点光源
		MapObjEdit* GetSelectedPointLight()	{ return m_pSelectPointLight;}
		//根据索引得到点光源
		MapObjEdit* FindPointLight(DWORD nIndex);
		//绘制点光源标识
		void SetDrawPointLightFlag(bool bVal)		{ m_bPickPointLight = bVal;}
		//点光源标识位置
		void SetPosForPointLight(Vector3& pos)		{ m_vCurPosPointLight = pos;}

		//--地图区域相关
		//新增加一个地图区域
		void AddMapRect(MapObjEdit* pMapRect);
		//删除一个地图区域
		void DeleteMapRect(tagMapArea * pMapRect);
		//通过索引删除一个地图区域
		void DeleteMapRect(DWORD dwIndex);
		//得到点地图区域列表
		void GetMapRectList(list<MapObjEdit*>& out)	{	out = m_pMapRectList; }
		//清空地图区域列表
		void ClearMapRectList();
		//设置当前选择的地图区域
		void SetSelectedMapRect(MapObjEdit* pMapRect)	{ m_pSelectMapRect = pMapRect;}
		//得到当前选择的地图区域
		MapObjEdit* GetSelectedMapRect()	{return m_pSelectMapRect;}
		//根据索引得到地图区域
		MapObjEdit* FindMapRect(DWORD dwIndex);

		void SetCurPosForMapRect(Vector3 pos,bool bIsCurDrawMapRect)
		{
			m_bDrawMapRect = bIsCurDrawMapRect;
			if(m_bDrawMapRect)
				m_vCurPosForMapRect = pos;
		}
		
		void SetCurPosForPickMapRect(Vector3 pos)             {  m_vCurPickMapRect = pos;     }
		void GetCurPosForMapRect(Vector3 &out)                {  out = m_vCurPosForMapRect; }

		void SetPosForMapArea(Vector3& pos, int curPos)
		{ 
			if(curPos>=(int)m_vMapAreaPos.size())
				m_vMapAreaPos.push_back(pos);
			else
				m_vMapAreaPos[curPos] = pos; 
		}
		void SetPosForMapArea(Vector3& pos)					  
		{ 
			m_vMapAreaPos.push_back(pos); 
			m_nCurMapAreaNum++;
		}
		void GetPosForMapArea(Vector3&out, int curPos)		  { out = m_vMapAreaPos[curPos]; }
		void GetPosForMapArea(vector<Vector3>& vec)			  {	vec = m_vMapAreaPos; }
			
		void SetDrawPickFlagForMapRect(bool bIsPickMapRect)   {  m_bPickMapRect = bIsPickMapRect;  }
		//得到拾取的地图区域
		void PickMapRect(UINT x, UINT y);
		//拾取地图区域的某一个点
		void PickMapAreaPoint(UINT x, UINT y, bool& bHave);
		//设置/获得 是否会制地图区域AABBox
		void SetIsDrawMapRect(bool bVal) { m_bIsDrawMapRect = bVal; }
		bool GetIsDrawMapRect()const { return m_bIsDrawMapRect; }

		//获得当前建立地图区域多边形区域到哪个点
		int GetCurMapAreaNum()								  { return m_nCurMapAreaNum; }
		void ClearMapAreaNum()								  { m_nCurMapAreaNum = 0; }
		//计算触发器的AABBOX
		AABBox CalMapAreaAABBox();
		//重新计算触发器的AABBOX
		void ReCalMapAreaAABBox(tagMapArea* pArea);
		//
		void ClearMapAreaPos();
		//得到拾取触发器某个点是第几个点
		int GetMapAreaNum()								{	return m_nMapAreaNum; }
		void ClearPickMapAreaPoint()					{	m_bPickMapAreaPoint = false; m_nMapAreaNum = 0; }


		//--寻路导航点相关
		//新增加一个寻路导航点
		void AddPathPoint(MapObjEdit* pEdit);
		//删除一个寻路导航点
		void DeletePathPoint(tagMapPathPoint * pPathPoint);
		//通过索引删除一个寻路导航点
		void DeletePathPoint(DWORD dwIndex);
		//得到点寻路导航点列表
		void GetPathPointList(list<MapObjEdit*>& out)		{ out = m_pPathPointList; }
		void GetPathPointVector(vector<tagMapPathPoint>& out)
		{
			vector<tagMapPathPoint> vecResult;
			tagMapPathPoint pathpoint;
			list<MapObjEdit*>::iterator it = m_pPathPointList.begin();
			for( ; it!=m_pPathPointList.end(); ++it )
			{
				if( (*it)->GetType() == MapObjEdit::PathPoint )
				{
					(*it)->GetData(pathpoint);
					vecResult.push_back(pathpoint);
				}
			}
			out = vecResult;
		}

		//清空寻路导航点列表
		void ClearPathPointList();
		//设置当前选择的寻路导航点
		void SetSelectedPathPoint(MapObjEdit* pPathPoint)	{ m_pSelectPathPoint = pPathPoint;}
		//得到当前选择的寻路导航点
		MapObjEdit* GetSelectedPathPoint()					{ return m_pSelectPathPoint;}
		//根据索引得到寻路导航点
		MapObjEdit* FindPathPoint(DWORD dwIndex);
		//拾取寻路导航点
		void PickPathPoint(UINT x, UINT y);
		//设置画建立寻路导航点标识
		void SetDrawPathPoint(bool bVal)					{ m_bDrawPathPoint = bVal; }
		//设置画建立寻路导航点位置
		void SetCurPosForPathPoint(Vector3 pos)				{ m_vCurPosForPathPoint = pos; }

		//--刷怪点相关
		//添加一个刷怪点
		void AddSpawnPoint(MapObjEdit* pEdit);
		//删除一个刷怪点
		void DeleteSpawnPoint(tagMapSpawnPoint* pSpawnPoint);
		//通过索引删除一个刷怪点
		void DeleteSpawnPoint(DWORD dwIndex);
		//得到刷怪点列表
		void GetSpawnPointList(list<MapObjEdit*>& out)		{ out = m_pSpawnPointList;}
		//清空刷怪点列表
		void ClearSpawnPointList();
		//设置当前选择的刷怪点
		void SetSelectedSpawnPoint(MapObjEdit* pSpawnPoint)	{ m_pSelectSpawnPoint = pSpawnPoint; }
		//得到当前选择的刷怪点
		MapObjEdit* GetSelectedSpawnPoint()					{ return m_pSelectSpawnPoint; }
		//通过索引得到刷怪点
		MapObjEdit* FindSpawnPoint(DWORD dwIndex);
		//拾取刷怪点
		void PickSpawnPoint(UINT x, UINT y);
		//设置画建立刷怪点标识
		void SetDrawSpawnPoint(bool bVal)					{ m_bDrawSpawnPoint = bVal; }
		//设置画建立刷点的位置
		void SetCurPosForSpawnPoint(Vector3 pos)			{ m_vCurPosForSpawnPoint = pos; }

		//--事件触发器相关
		//添加一个事件触发器
		void AddEventTrigger(MapObjEdit* pEdit);
		//删除一个事件触发器
		void DeleteEventTrigger(tagMapEventTrigger* pEventTrigger);
		//通过索引删除一个事件触发器
		void DeleteEventTrigger(DWORD dwIndex);
		//得到事件触发器列表
		void GetEventTriggerList(list<MapObjEdit*>& out)	{ out = m_pEventTriggerList; }
		//清空事件触发器
		void ClearEventTrigger();
		//通过索引得到事件触发器
		MapObjEdit* FindEventTrigger(DWORD dwIndex);

		//--NPC格子导航图生成起点
		void AddNPCNavStartPoint(tagNPCNavMapBuildStartPoint* pNPCNav);
		void DeleteNPCNavStartPoint(tagNPCNavMapBuildStartPoint* pNPCNav);
		void DeleteNPCNavStartPoint(DWORD dwIndex);
		void ClearNPCNavStartPoint();
		void SetSelectedNPCNavStartPoint(tagNPCNavMapBuildStartPoint* pNPCNav)  { m_pSelectNPCNavStartPoint = pNPCNav;}
		tagNPCNavMapBuildStartPoint* GetSelectedNPCNavStartPoint()				{ return m_pSelectNPCNavStartPoint; }
		tagNPCNavMapBuildStartPoint* FindNPCNavStartPoint(DWORD dwIndex);
		void PickNPCNavStartPoint(UINT x, UINT y);
		void SetDrawNPCNavStartPoint(bool bVal)									{ m_bDrawNPCNavStartPoint = bVal; }
		void SetCurPosForNPCNavStartPoint(Vector3 pos)							{ m_vCurPosForNPCNav = pos; }

		//--玩家寻路导航图生成起点
		void AddPlayerNavStartPoint(tagPlayerNavMapBuildStartPoint* pPlayerNav);
		void DeletePlayerNavStartPoint(tagPlayerNavMapBuildStartPoint* pPlayerNav);
		void DeletePlayerNavStartPoint(DWORD dwIndex);
		void ClearPlayerNavStartPoint();
		void SetSelectedPlayerNavStartPoint(tagPlayerNavMapBuildStartPoint* pPlayerNav)  { m_pSelectPlayerNavStartPoint = pPlayerNav;}
		tagPlayerNavMapBuildStartPoint* GetSelectedPlayerNavStartPoint()				{ return m_pSelectPlayerNavStartPoint; }
		tagPlayerNavMapBuildStartPoint* FindPlayerNavStartPoint(DWORD dwIndex);
		void PickPlayerNavStartPoint(UINT x, UINT y);
		void SetDrawPlayerNavStartPoint(bool bVal)									{ m_bDrawPlayerNavStartPoint = bVal; }
		void SetCurPosForPlayerNavStartPoint(Vector3 pos)							{ m_vCurPosForPlayerNav = pos; }



		//!	找到第一个引用了这个way point的角色
		MapObjEdit *FindRoleByWayPoint(const TCHAR* szWayName);

		//--
		DWORD GetGlobalID()const			{ return m_dwGlobalID; }
		void SetGlobalID(DWORD id)			{ m_dwGlobalID = id; }

	private:
		void GetTransList(IFS *pFS, LPCTSTR szFileName, vector<tstring>& fsmList);
		void SaveShowAreaData(LPCTSTR szFileName);
	protected:
		list<MapObjEdit*>			m_objList;
		MapObjEdit*					m_pSelectedObj;
		CreatureDataReader			m_createData;

		NullSceneGraph				*m_pSceneGraph;
		NavNullSceneGraph			*m_pNavSG;

		TerrainEditor				*m_pTerrain;
		CameraBase					*m_pCamera;

		//way point list
		list<tagMapWayPoint*>        m_WayPointList;
        //current selected way point
		tagMapWayPoint*              m_pSelectWayPoint;
		//deal with drawing waypoint
		bool                         m_bCurDrawWayPoint;
		Vector3                      m_vCurPosForWayPoint;
        //deal with picking waypoint
		bool                         m_bPickWayPoint;
		Vector3                      m_vCurPickWayPoint;
		//
		bool						 m_bDrawWayPoint;

		//Trigger list
		list<tagMapTrigger*>         m_TriggerList;
		//current selected trigger
		tagMapTrigger*               m_pSelectTrigger;
		//拾取触发器的某个点是第几个点
		int							 m_nTriggerNum;
		bool						 m_bPickTriggerPoint;
		//触发器多边形区域已经建立到哪个点
		int							 m_nCurTriggerNum;
		//触发器多边形区域的点位置
		vector<Vector3>				 m_vTriggerPos;
		//deal with drawing Trigger
		bool                         m_bCurDrawTrigger;
		//Vector3                      m_vStartPosForTrigger;
		Vector3                      m_vCurPosForTrigger;
		//deal with picking trigger
		bool                         m_bPickTrigger;
		Vector3                      m_vCurPickTrigger;
		//
		bool						 m_bDrawTrigger;

		//sound list
		list<tagMapSound*>           m_SoundList;
		//current selected sound
		tagMapSound*                 m_pSelectSound;
		//画音效标识
		bool						 m_bPickSound;
		//音效标识位置
		Vector3						 m_vCurPosSound;

		//点光源
		list<MapObjEdit*>			 m_PointLightList;
		//当前选择的点光源
		MapObjEdit*					 m_pSelectPointLight;
		//画点光源标识
		bool						 m_bPickPointLight;
		//点光源标识位置
		Vector3						 m_vCurPosPointLight;

		//天空盒SceneGraph结点	
		SGSkyBox*					 m_pSGSkyBoxNode;	

		//--地图区域
		list<MapObjEdit*>			 m_pMapRectList;
		//当前选择的地图区域
		MapObjEdit*					 m_pSelectMapRect;
		//拾取地图区域的某个点是第几个点
		int							 m_nMapAreaNum;
		bool						 m_bPickMapAreaPoint;
		//地图区域多边形区域已经建立到哪个点
		int							 m_nCurMapAreaNum;
		//地图区域多边形区域的点位置
		vector<Vector3>				 m_vMapAreaPos;
		//处理画地图区域
		bool						 m_bDrawMapRect;
		Vector3						 m_vStartPosForMapRect;
		Vector3						 m_vCurPosForMapRect;
		//处理拾取地图区域
		bool						 m_bPickMapRect;
		Vector3						 m_vCurPickMapRect;
		//处理是否绘制地图区域
		bool						 m_bIsDrawMapRect;

		//--寻路导航点			by add xtian 2008-8-6
		list<MapObjEdit*>			m_pPathPointList;
		//当前选择的寻路导航点
		MapObjEdit*					m_pSelectPathPoint;
		//处理画寻路寻航点
		bool						m_bDrawPathPoint;
		Vector3						m_vCurPosForPathPoint;

		//--刷怪点				by add xtian 2008-8-11
		list<MapObjEdit*>			m_pSpawnPointList;
		//当前选择的刷怪点
		MapObjEdit*					m_pSelectSpawnPoint;
		//处理画刷怪点
		bool						m_bDrawSpawnPoint;
		Vector3						m_vCurPosForSpawnPoint;

		//--事件触发器			by add xtian 2008-9-10
		list<MapObjEdit*>			m_pEventTriggerList;

		//全局唯一ID
		DWORD						m_dwGlobalID;

		//--NPC格子导航图生成起点	 by add xtian 2008-10-20
		list<tagNPCNavMapBuildStartPoint*>	m_NPCNavStartPointList;
		tagNPCNavMapBuildStartPoint*		m_pSelectNPCNavStartPoint;
		bool								m_bDrawNPCNavStartPoint;
		Vector3								m_vCurPosForNPCNav;

		//--玩家寻路导航图生成起点
		list<tagPlayerNavMapBuildStartPoint*>	m_PlayerNavStartPointList;
		tagPlayerNavMapBuildStartPoint*			m_pSelectPlayerNavStartPoint;
		bool									m_bDrawPlayerNavStartPoint;
		Vector3									m_vCurPosForPlayerNav;

                		
	};
}//namespace WorldBase


