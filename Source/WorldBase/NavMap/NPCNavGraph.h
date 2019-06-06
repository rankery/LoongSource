#pragma once
#include "..\WorldBase.h"

namespace WorldBase
{
	class NavCollider;
	class PathPointDistMap;
	/** \class NPCNavGraph
		\brief NPC路点导航图
	*/
	class WORLD_BASE_API NPCNavGraph
	{
	public:
		struct tagPathPointRT 
		{
			Vector3 pos;				
			vector<int> nearPoints;
			vector<float> nearDists;
		};
	public:
		NPCNavGraph(void);
		~NPCNavGraph(void);

		//--
		void LoadFromFile(const TCHAR* szMapName,IFS* pFS);
		void Clone(NPCNavGraph* pTarget);

		/** 导航点寻路
			\param searchRange 搜索导航点的范围
		*/
		bool FindPath(const Vector3& startPos,const Vector3& endPos,const Vector3& roleSize,float searchRange,NavCollider* pCollider,list<Vector3>& out);
		
		//--Editor support
		const vector<tagPathPointRT>& GetPathPoints();
		static bool Save(const TCHAR* szMapName,const vector<tagPathPointRT>& pathPoints,PathPointDistMap* pDistMap);
		
	private:
		/** 寻找起始导航点，找不到返回-1
		*/
		int FindStartPoint(const Vector3& startPos,const Vector3& roleSize,int tarPoint,float searchRange,NavCollider* pCollider);
		/** 寻找结束导航点，找不到返回-1
		*/
		int FindTargetPoint(const Vector3& targetPos,const Vector3& roleSize,float searchRange,NavCollider* pCollider);
		/** 寻找两个导航点之间的最短路径
		*/
		bool FindPointPath(int srcPoint,int tarPoint,list<Vector3>& out);
		/** 寻找下一个寻航点，找不到返回-1
		*/
		int FindNextPoint(int curPoint,int tarPoint);
	private:
		class Member;
		Member* m_p;
	};
}//namespace WorldBase
