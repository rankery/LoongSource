#include "StdAfx.h"
#include "PathFinderByTile.h"
#include "NavCollider.h"
#include "NavCollider_Move.h"
#include "NavCollider_Swim.h"

namespace WorldBase
{
	const float g_tileSize=50.0f;

	PathFinderByTile::PathFinderByTile(void)
		: m_pCangoMap(NULL)
		, m_pCollider(NULL)
	{}

	PathFinderByTile::~PathFinderByTile(void)
	{}

	inline Point _World2Tile(const Vector3& wpt)
	{
		return Point(int(wpt.x/g_tileSize),int(wpt.z/g_tileSize));
	}

	inline Vector3 _Tile2World(int x,int z)
	{
		return Vector3((float)x*g_tileSize,0,(float)z*g_tileSize);
	}

	PathFinderByTile::EResult PathFinderByTile::FindPath( const Vector3& startPos,const Point& destTile,const Vector3& roleSize,bool bHalfPath,list<AStarPathFinder::tagPathPoint>& path )
	{
		ASSERT(m_pCollider!=NULL);
		ASSERT(m_pCangoMap!=NULL);

		path.clear();
		Point srcTile=_World2Tile(startPos);

		//--A*寻路
		AStarPathFinder::Inst()->SetCallBack(this);
		AStarPathFinder::EResult result=AStarPathFinder::Inst()->FindPath(srcTile.x,srcTile.y,0,destTile.x,destTile.y,0,path);
		if(result==AStarPathFinder::ER_Failed)
		{
			return ER_Failed;
		}

		if(path.size()==0)
			return ER_Failed;

		//--截取一半路径
		if(bHalfPath
			&&path.size()>1)
		{
			int halfSize=int(path.size()/2);
			int i=0;
			for(list<AStarPathFinder::tagPathPoint>::iterator iter=path.begin();
				iter!=path.end();iter++)
			{
				if(i>=halfSize)
				{
					path.erase(iter,path.end());
					break;
				}
				i++;
			}
		}

		if(result==AStarPathFinder::ER_Arrive)
			return ER_Arrive;
		else if(result==AStarPathFinder::ER_GetNearPos)
			return ER_GetNearPos;
		else
			return ER_Failed;
	}

	int PathFinderByTile::GetWay( int sx,int sz,int sa,EDir dir,int dx,int dz,int* pda )
	{
		if(m_pCangoMap->IfIndex(dx,dz) 
			&&m_pCangoMap->GetValue(dx,dz))
		{
			pda[0]=0;
			return 1;
		}
		return 0;
	}

	void PathFinderByTile::SmoothPath( const Vector3& startPos,const Vector3& roleSize,bool bOnLand,list<AStarPathFinder::tagPathPoint>& tilePath,list<Vector3>& out )
	{
		out.clear();

		if(tilePath.size()==0)
			return;

		if(tilePath.size()==1)
		{
			out.push_back(_Tile2World(tilePath.front().x,tilePath.front().z));
			return;
		}
		
		Vector3 start=startPos;
		NavCollider_Move moveCollider;
		NavCollider_Swim swimCollider;
		list<AStarPathFinder::tagPathPoint>::iterator iterPre=tilePath.begin();
		list<AStarPathFinder::tagPathPoint>::iterator iterCur=tilePath.begin();
		++iterCur;
		while(iterCur!=tilePath.end())
		{
			Vector3 end=_Tile2World(iterCur->x,iterCur->z);
			if(bOnLand)
			{
				if(moveCollider.IfCanGo(m_pCollider,start,end,roleSize,NULL,10000,true,false)!=ENR_Arrive)
				{
					out.push_back(_Tile2World(iterPre->x,iterPre->z));
					start=out.back();
				}
			}
			else
			{
				if(swimCollider.IfCanGo(m_pCollider,start,end,roleSize,NULL,10000)!=ENR_Arrive)
				{
					out.push_back(_Tile2World(iterPre->x,iterPre->z));
					start=out.back();
				}
			}
			++iterPre;
			++iterCur;
		}
		out.push_back(_Tile2World(iterPre->x,iterPre->z));
	}

}//namespace WorldBase
