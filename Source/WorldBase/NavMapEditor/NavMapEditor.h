#pragma once
#include "..\WorldBase.h"
#include "..\NavMap\NavNullSceneGraph.h"
#include "..\..\WorldDefine\MapAttDefine.h"

namespace WorldBase
{
	class NavCollider;
	/**	\class NavMapEditor
		\brief 导航图编辑器核心类
	*/
	class WORLD_BASE_API NavMapEditor
	{
	public:
		struct tagBuildOptions
		{
			bool bColliderData;//碰撞数据
			bool bNPCNavMap;//NPC格子导航图
			bool bNPCNavGraph;//NPC路点导航图
			bool bPlayerNavMap;//玩家自动寻路导航图
			float fNPCPassHeight;//NPC通过高度
			float fNPCNavGraphPassHeight; //NPC路点导航图可通过高度
		};
	public:
		NavMapEditor();
		virtual ~NavMapEditor();

		void BuildSetHeightmap(Heightmap *pHMap);
		void BuildSetManualCangoMap(BitMap* pManualCangoMap);
		void BuildSetPlayerRoadMap(BitMap* pPlayerRoadMap);
		void BuildSetPathPoints(const vector<tagMapPathPoint>& points);
		void BuildSetNPCNavMapBuildStartPoints(const list<tagNPCNavMapBuildStartPoint*>& points);
		void BuildSetPlayerNavMapBuildStartPoints(const list<tagPlayerNavMapBuildStartPoint*>& points);
		void BuildSetWaterArea(BitMap* pBitmap);
		void BuildSetWaterHeight(Heightmap* pHMap);
		void BuildSetGroundType(Heightmap* pGroundType);
		void BuildSetDoors(const vector<tagMapDoor>& doors);
		bool Build(const TCHAR* szSavePath,const TCHAR* szMapName,IUICallBack* pUI);

		bool Save(const TCHAR *szMapName,BitMap *pManualCangoMap,BitMap *pPlayerRoadMap);
		bool Load(const TCHAR *szMapName,BitMap *pManualCangoMap,BitMap *pPlayerRoadMap);

		bool LoadNPCNavMap(const TCHAR *szMapName, BitMap *pNPCNavBMap, Heightmap* pNPCNavHMap);
		bool LoadPlayerNavMap(const TCHAR *szMapName, BitMap *pPlayerBMap, Heightmap* pPlayerHMap);


		NavNullSceneGraph* GetSceneGraph(){ return &m_sg;}
		tagBuildOptions& GetBuildOptions(){ return m_buildOptions;}
	
	private:
		bool SaveWaterData(const TCHAR* szSavePath,const TCHAR* szMapName);

	private:
		Heightmap				*m_pTerrainHMap;		//地形高度图
		BitMap					*m_pManualCangoMap;		//手工编辑的不可行走区域
		BitMap					*m_pPlayerRoadMap;		//手工编辑的玩家寻路道路区域
		vector<tagMapPathPoint> m_pathPoints;			//外部传进来的寻路导航点
		BitMap					*m_pWaterArea;			//水面区域
		Heightmap				*m_pWaterHeight;		//水面高度
		Heightmap				*m_pGroundType;			//地表类型

		list<tagNPCNavMapBuildStartPoint*>		m_npcNavMapBuildStartPoints;
		list<tagPlayerNavMapBuildStartPoint*>	m_playerNavMapBuildStartPoints;

		vector<tagMapDoor>		m_doors;

		NavNullSceneGraph		m_sg;					//用于碰撞检测的简易场景管理器
		tagBuildOptions			m_buildOptions;			//Build选项

	};
}//namespace WorldBase