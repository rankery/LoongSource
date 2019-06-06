#pragma once
#include "..\NavMap\PathPointDistMap.h"
#include "..\NavMap\NPCNavGraph.h"
#include "..\..\WorldDefine\MapAttDefine.h"
#include "..\NavMap\NavCollider_NPCMove.h"

namespace WorldBase
{
	/** \class NPCNavGraphGen
		\brief NPC路点导航图生成器
	*/
	class NPCNavGraphGen
	{
	public:
		NPCNavGraphGen(void);
		~NPCNavGraphGen(void);

		void BeginBuild(vector<tagMapPathPoint>& pathPoints,NavCollider* pCollider,float fPassHeight);
		bool EndBuild(const TCHAR* szSavePath,const TCHAR* szMapName);

	private:
		/** 计算相邻导航点距离，不相邻返回-1
		*/
		float CalcNearPathPointDist(int curPoint,int nearPoint,NavCollider* pCollider);
		/** 计算一个导航点到其它所有导航点的最短距离
		*/
		void CalcPointShortestDist(int srcPoint,vector<NPCNavGraph::tagPathPointRT>& points,PathPointDistMap& distMap);

	private:
		vector<tagMapPathPoint>     m_pathPoints;
		NavCollider*				m_pCollider;
		NavCollider_NPCMove			m_moveCollider;
		float                       m_fPassHeight;
	};

}//namespace WorldBase
