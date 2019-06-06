#pragma once
#include "AStarPathFinder.h"

namespace WorldBase
{
	class NavCollider;
	/** \class PathFinderByTile
		\brief 基于格子的寻路器
	*/
	class PathFinderByTile : public IAStarPathFinderCallBack
	{
	public:
		enum EResult
		{
			ER_Failed,		//寻路失败
			ER_Arrive,		//到达终点
			ER_GetNearPos,	//到达附近位置
		};

	public:
		PathFinderByTile(void);
		virtual ~PathFinderByTile(void);

		//--
		void SetCangoMap(BitMap* pCangoMap)		{ m_pCangoMap=pCangoMap;}	
		void SetCollider(NavCollider* pCollider){ m_pCollider=pCollider;}

		/** 寻路
		*/
		EResult FindPath(const Vector3& startPos,const Point& destTile,const Vector3& roleSize,bool bHalfPath,list<AStarPathFinder::tagPathPoint>& path);
		/** 平滑路径
		*/
		void SmoothPath(const Vector3& startPos,const Vector3& roleSize,bool bOnLand,list<AStarPathFinder::tagPathPoint>& tilePath,list<Vector3>& out);

		//--IAStarPathFinderCallBack
		virtual int GetWay(int sx,int sz,int sa,EDir dir,int dx,int dz,int* pda);

	private:
		BitMap*			m_pCangoMap;
		NavCollider*	m_pCollider;
	};
}//namespace WorldBase

