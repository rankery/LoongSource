#pragma once
#include "..\WorldBase.h"
#include "PathFinderByMap.h"
#include "PathFinderByPatch.h"
#include "PathFinderByTile.h"

namespace WorldBase
{
	/** \class PlayerNavMap
		\brief 玩家自动寻路导航图
	*/
	class WORLD_BASE_API PlayerNavMap
	{
	public:
		enum EResult		//寻路返回值
		{
			ER_Failed,		//失败
			ER_Arrive,		//到达终点
			ER_GetNearPos,	//到达附近点
			ER_EnterRoad,	//进入道路区域
		};
	public:
		PlayerNavMap(void);
		~PlayerNavMap(void);

		/** 加载数据
		*/
		void LoadFromFile(const TCHAR* szMapName,IFS* pMapFS,IFS* pSysFS);

		/** 判断是否可通过
		*/
		bool IfCanGo(int x,int z,bool bRoadMap);
		bool IfCanGo(float x,float z,bool bRoadMap);
		/** 按地图寻路
		*/
		bool FindPathByMap(const TCHAR* szSrcMap,const TCHAR* szDestMap,Vector3& switchPos);
		/** 按地块寻路
		*/
		EResult FindPathByPatch(const Vector3& startPos,const Vector3& endPos,bool bRoadMap,list<Point>& path);
		/** 按格子寻路
		*/
		EResult FindPathByTile(const Vector3& startPos,const Point& destTile,const Vector3& roleSize,bool bRoadMap,bool bRoadCheck,bool bHalfPath,bool bOnLand,NavCollider* pCollider,list<Vector3>& path);
		EResult FindPathByTile(const Vector3& startPos,const Vector3& destPos,const Vector3& roleSize,bool bRoadMap,bool bRoadCheck,bool bHalfPath,bool bOnLand,NavCollider* pCollider,list<Vector3>& path);

	private:
		PathFinderByMap		m_pathFinderByMap;
		PathFinderByPatch	m_pathFinderByPatch_Road;
		PathFinderByPatch	m_pathFinderByPatch_Auto;
		PathFinderByTile	m_pathFinderByTile;
		BitMap				m_cangoMap_Road;
		BitMap				m_cangoMap_Auto;	
	};
}//namespace WorldBase
