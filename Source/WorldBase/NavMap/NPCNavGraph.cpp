#include "StdAfx.h"
#include "NPCNavGraph.h"
#include "NavCollider.h"
#include "PathPointOctree.h"
#include "PathPointDistMap.h"
#include "PathPointOctreeBuilder.h"
#include "NavCollider_NPCMove.h"

namespace WorldBase
{
	//--class NPCNavGraph::Member----------------------------------------------------------
	class NPCNavGraph::Member
	{
	public:
		PathPointOctree			m_octree;
		PathPointDistMap		m_distMap;
		vector<tagPathPointRT>	m_pathPoints;
		NavCollider_NPCMove		m_moveCollider;

		void Clone(NPCNavGraph::Member* pTarget)
		{
			m_octree.Clone(&pTarget->m_octree);
			m_distMap.Clone(&pTarget->m_distMap);
			pTarget->m_pathPoints=m_pathPoints;
		}
	};


	//--class NPCNavGraph------------------------------------------------------------------
	NPCNavGraph::NPCNavGraph(void)
	{
		m_p=new Member;
	}

	NPCNavGraph::~NPCNavGraph(void)
	{
		delete m_p;
	}

	void WorldBase::NPCNavGraph::LoadFromFile( const TCHAR* szMapName,IFS* pFS )
	{
		//--加载八叉树，有些地图没有NPC导航点，加载失败则返回
		if(!m_p->m_octree.LoadFromFile(szMapName,pFS))
			return;

		//--加载DistMap
		TCHAR szPath[512];
		_stprintf(szPath,_T("data\\map\\%s\\nav\\npcpathpoints.data"),szMapName);

		DWORD hFile=pFS->Open(szPath);
		THROW_NULL(hFile,_T("load npcpathpoints.data fail"));

		m_p->m_distMap.ReadFile(pFS,hFile);

		//--加载PathPoints
		int size;
		FReadValue(pFS,hFile,size);
		m_p->m_pathPoints.resize(size);
		for(int i=0;i<(int)m_p->m_pathPoints.size();i++)
		{
			FReadValue(pFS,hFile,m_p->m_pathPoints[i].pos);
			FReadValVector(pFS,hFile,m_p->m_pathPoints[i].nearPoints);
			FReadValVector(pFS,hFile,m_p->m_pathPoints[i].nearDists);
		}

		pFS->Close(hFile);
	}

	bool NPCNavGraph::Save( const TCHAR* szMapName,const vector<tagPathPointRT>& pathPoints,PathPointDistMap* pDistMap )
	{
		TCHAR szPath[512];

		//--生成八叉树
		PathPointOctreeBuilder pathPointBuilder;
		pathPointBuilder.Begin();
		for(int i=0;i<(int)pathPoints.size();i++)
		{
			const tagPathPointRT& pathPoint=pathPoints[i];
			pathPointBuilder.AddPoint(pathPoint.pos);
		}
		pathPointBuilder.End();

		_stprintf(szPath,_T("data\\map\\%s\\nav\\npcpathpoints.oct"),szMapName);
		pathPointBuilder.Save(szPath);
		
		//--保存DistMap
		_stprintf(szPath,_T("data\\map\\%s\\nav\\npcpathpoints.data"),szMapName);

		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());

		FILE* fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
			return false;

		pDistMap->WriteFile(fp);

		//--保存PathPoints
		int size=(int)pathPoints.size();
		FWriteValue(fp,size);
		for(int i=0;i<(int)pathPoints.size();i++)
		{
			FWriteValue(fp,pathPoints[i].pos);
			FWriteValVector(fp,pathPoints[i].nearPoints);
			FWriteValVector(fp,pathPoints[i].nearDists);
		}

		fclose(fp);
		return true;
	}

	void WorldBase::NPCNavGraph::Clone( NPCNavGraph* pTarget )
	{
		m_p->Clone(pTarget->m_p);
	}	
	
	bool NPCNavGraph::FindPath( const Vector3& srcPos,const Vector3& tarPos,const Vector3& roleSize,float searchRange,NavCollider* pCollider,list<Vector3>& out )
	{
		out.clear();

		//--找出离目标最近的导航点
		int tarPoint=FindTargetPoint(tarPos,roleSize,searchRange,pCollider);
		if(-1==tarPoint)
			return false;

		//--找出追击者附近离目标最近的导航点
		int srcPoint=FindStartPoint(srcPos,roleSize,tarPoint,searchRange,pCollider);
		if(-1==srcPoint)
			return false;

		//--导航点之间寻路
		if( !FindPointPath(srcPoint,tarPoint,out) )
			return false;

		out.push_back(tarPos);
		return true;
	}

	bool SortFun(const pair<int,float>& p1,const pair<int,float>& p2)
	{
		return p1.second<p2.second;
	}

	int NPCNavGraph::FindStartPoint( const Vector3& srcPos,const Vector3& roleSize,int tarPoint,float searchRange,NavCollider* pCollider )
	{
		//--找出起点附近的导航点
		AABBox box;
		box.min.x=srcPos.x-searchRange;
		box.min.y=srcPos.y-searchRange;
		box.min.z=srcPos.z-searchRange;
		box.max.x=srcPos.x+searchRange;
		box.max.y=srcPos.y+searchRange;
		box.max.z=srcPos.z+searchRange;

		list<pair<int,float> > nearPoints;
		m_p->m_octree.QueryPathPoints(box,m_p->m_pathPoints,nearPoints);
		if(nearPoints.size()==0)
			return -1;

		//--计算到目标导航点的距离并排序
		for(list<pair<int,float> >::iterator iter=nearPoints.begin();
			iter!=nearPoints.end();++iter)
		{
			iter->second=m_p->m_distMap(tarPoint,iter->first);
		}
		nearPoints.sort(SortFun);

		//--检测连通性
		for(list<pair<int,float> >::iterator iter=nearPoints.begin();
			iter!=nearPoints.end();++iter)
		{
			int index=iter->first;

			if(ENR_Arrive==m_p->m_moveCollider.IfCanGo(pCollider,srcPos,m_p->m_pathPoints[index].pos,roleSize,NULL))
				return index;
		}

		return -1;
	}

	int NPCNavGraph::FindTargetPoint( const Vector3& tarPos,const Vector3& roleSize,float searchRange,NavCollider* pCollider )
	{
		//--找出终点附近的导航点
		AABBox box;
		box.min.x=tarPos.x-searchRange;
		box.min.y=tarPos.y-searchRange;
		box.min.z=tarPos.z-searchRange;
		box.max.x=tarPos.x+searchRange;
		box.max.y=tarPos.y+searchRange;
		box.max.z=tarPos.z+searchRange;

		list<pair<int,float> > nearPoints;
		m_p->m_octree.QueryPathPoints(box,m_p->m_pathPoints,nearPoints);
		if(nearPoints.size()==0)
			return -1;

		//--计算到终点的距离并排序
		for(list<pair<int,float> >::iterator iter=nearPoints.begin();
			iter!=nearPoints.end();++iter)
		{
			iter->second=Vec3DistSq(tarPos,m_p->m_pathPoints[iter->first].pos);
		}
		nearPoints.sort(SortFun);

		//--检测连通性
		for(list<pair<int,float> >::iterator iter=nearPoints.begin();
			iter!=nearPoints.end();++iter)
		{
			int index=iter->first;

			if(ENR_Arrive==m_p->m_moveCollider.IfCanGo(pCollider,tarPos,m_p->m_pathPoints[index].pos,roleSize,NULL))
				return index;
		}

		return -1;
	}

	bool NPCNavGraph::FindPointPath( int srcPoint,int tarPoint,list<Vector3>& out )
	{
		//起点和目标点相同
		if( srcPoint == tarPoint )
		{
			out.push_back(m_p->m_pathPoints[srcPoint].pos);
			return true;
		}

		//不连通
		if(m_p->m_distMap(srcPoint,tarPoint)==-1)
			return false;

		int curPoint=srcPoint;
		out.push_back(m_p->m_pathPoints[srcPoint].pos);

		for(int i=0;i<20;i++)//最多搜索20层
		{
			curPoint=FindNextPoint(curPoint,tarPoint);//寻找下一个导航点
			if(curPoint==-1)
				return false;

			out.push_back(m_p->m_pathPoints[curPoint].pos);

			if(curPoint==tarPoint)//到达目标导航点？
				return true;
		}

		return false;
	}

	int NPCNavGraph::FindNextPoint( int curPoint,int tarPoint )
	{
		vector<int>& nearPoints=m_p->m_pathPoints[curPoint].nearPoints;
		vector<float>& nearDists=m_p->m_pathPoints[curPoint].nearDists;
		ASSERT(nearPoints.size()==nearDists.size());

		int bestPoint=-1;
		float minDist=FLOAT_MAX;

		for(size_t i=0;i<nearPoints.size();i++)
		{
			int nearPoint=nearPoints[i];
			float nearDist=nearDists[i];

			float dist=nearDist+m_p->m_distMap(nearPoint,tarPoint);
			if(bestPoint==-1
				||dist<minDist)
			{
				minDist=dist;
				bestPoint=nearPoint;
			}
		}

		return bestPoint;
	}

	const vector<NPCNavGraph::tagPathPointRT>& NPCNavGraph::GetPathPoints()
	{
		return m_p->m_pathPoints;
	}
}//namespace WorldBase
