#include "StdAfx.h"
#include "NPCNavGraphGen.h"
#include "NavCollider.h"

namespace WorldBase
{
	NPCNavGraphGen::NPCNavGraphGen(void)
	{}

	NPCNavGraphGen::~NPCNavGraphGen(void)
	{}

	void NPCNavGraphGen::BeginBuild( vector<tagMapPathPoint>& pathPoints,NavCollider* pCollider,float fPassHeight )
	{
		m_pathPoints=pathPoints;
		m_pCollider=pCollider;
		m_fPassHeight=fPassHeight;
	}

	bool NPCNavGraphGen::EndBuild( const TCHAR* szSavePath,const TCHAR* szMapName )
	{
		vector<NPCNavGraph::tagPathPointRT> pathPoints;
		PathPointDistMap distMap;

		pathPoints.resize(m_pathPoints.size());
		distMap.Create(m_pathPoints.size());

		//--计算每个导航点的相邻导航点和距离
		int i,j;
		for(i=0;i<(int)m_pathPoints.size();i++)
		{
			tagMapPathPoint& point=m_pathPoints[i];
			pathPoints[i].pos=point.vPos;

			for(j=0;j<(int)m_pathPoints.size();j++)
			{
				float dist=CalcNearPathPointDist(i,j,m_pCollider);
				distMap(i,j)=dist;
				if(i!=j&&dist>0)
				{
					pathPoints[i].nearPoints.push_back(j);
					pathPoints[i].nearDists.push_back(dist);
				}
			}
		}

		//--计算每两个导航点之间的最短路径距离
		for(i=0;i<(int)pathPoints.size();i++)
		{
			CalcPointShortestDist(i,pathPoints,distMap);
		}

		//--保存pathPoints和distMap
		if(!NPCNavGraph::Save(szMapName,pathPoints,&distMap))
			return false;

		return true;
	}

	float NPCNavGraphGen::CalcNearPathPointDist( int curPoint,int nearPoint,NavCollider* pCollider )
	{
		//--到自已的距离为0
		if(curPoint==nearPoint)
			return 0;

		//--碰撞计算距离
		tagMapPathPoint& curPt=m_pathPoints[curPoint];
		tagMapPathPoint& nearPt=m_pathPoints[nearPoint];

		float dist=0.0f;
		const Vector3 roleSize=Vector3(50.0f,175.0f,50.0f);
		if(m_moveCollider.IfCanGo(pCollider,curPt.vPos,nearPt.vPos,roleSize,NULL,&dist,true,1000,false,m_fPassHeight)!=ENR_Arrive)
			return -1;

		return dist;
	}

	void NPCNavGraphGen::CalcPointShortestDist( int srcPoint,vector<NPCNavGraph::tagPathPointRT>& points,PathPointDistMap& distMap )
	{
		list<int> visitNearList;//待访问导航点列表
		visitNearList.push_back(srcPoint);

		vector<bool> visitFlag;//已访问标志
		visitFlag.resize(points.size(),false);
		visitFlag[srcPoint]=true;

		while(!visitNearList.empty())
		{
			int curPoint=visitNearList.front();
			visitNearList.pop_front();

			float dist2Cur=distMap(srcPoint,curPoint);

			//--visit near points
			vector<int>& nearPoints=points[curPoint].nearPoints;
			vector<float>& nearDists=points[curPoint].nearDists;
			for(size_t i=0;i<nearPoints.size();++i)
			{
				int nearPoint=nearPoints[i];
				float nearDist=nearDists[i];

				float& dist=distMap(srcPoint,nearPoint);
				float newDist=dist2Cur+nearDist;

				if(!visitFlag[nearPoint]
					||newDist<dist)
				{
					visitFlag[nearPoint]=true;
					dist=newDist;
					visitNearList.push_back(nearPoint);
				}
			}
		}
	}

}//namespace WorldBase

