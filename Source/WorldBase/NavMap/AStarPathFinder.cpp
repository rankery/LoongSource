#include "StdAfx.h"
#include "AStarPathFinder.h"

namespace WorldBase
{
	AStarPathFinder::AStarPathFinder(void)
		: m_pCallBack(NULL)
	{}

	AStarPathFinder::~AStarPathFinder(void)
	{
	}

	AStarPathFinder* AStarPathFinder::Inst()
	{
		static AStarPathFinder Inst;
		return &Inst;
	}

	void AStarPathFinder::SetCallBack( IAStarPathFinderCallBack* pCallBack )
	{
		m_pCallBack=pCallBack;
	}

	AStarPathFinder::EResult AStarPathFinder::FindPath( int sx,int sz,int sa,int dx,int dz,int da,list<tagPathPoint>& path )
	{
		ASSERT(sx>=0&&sz>=0&&dx>=0&&dz>=0);
		ASSERT(sa<EMaxArea&&da<EMaxArea);
		ASSERT(m_pCallBack!=NULL);

		path.clear();
		if(sx==dx&&sz==dz&&sa==da)
			return ER_Failed;

		//--计算局部map左上角位置
		int cx=abs(sx-dx);
		int cz=abs(sz-dz);
		if(cx>ELocalMapSize||cz>ELocalMapSize)
			return ER_Failed;
		int offsetx=(ELocalMapSize-cx)/2;
		int offsetz=(ELocalMapSize-cz)/2;
		int minx=min(sx,dx);
		int minz=min(sz,dz);
		m_localMapStartPos.x=minx-offsetx;
		m_localMapStartPos.y=minz-offsetz;

		//--计算起点、终点局部坐标
		m_startPos.x=GlobalX2LocalX(sx);
		m_startPos.y=GlobalZ2LocalZ(sz);
		m_targetPos.x=GlobalX2LocalX(dx);
		m_targetPos.y=GlobalZ2LocalZ(dz);
		m_startArea=sa;
		m_targetArea=da;

		return FindPathCore(path);
	}

	inline float _Distance(int x1,int z1,int x2,int z2)
	{
		float sq=float(abs((x2-x1)*(x2-x1))+abs((z2-z1)*(z2-z1)));
		return sqrtf(sq);
	}

	AStarPathFinder::EResult AStarPathFinder::FindPathCore( list<tagPathPoint>& path )
	{
		bool bFind=false;
		const float SQRT2=1.4142135f;//2的平方根

		//--
		ZeroMemory(m_visitFlag,sizeof(m_visitFlag));
		m_visitHeap.clear();

		//--将起点放入待访问列表 
		m_visitFlag[m_startPos.y][m_startPos.x][m_startArea]=true;
		m_visitMap[m_startPos.y][m_startPos.x][m_startArea].dist2Target=_Distance(m_startPos.x,m_startPos.y,m_targetPos.x,m_targetPos.y);
		m_visitHeap.push(tagVisitNode(m_startPos.x,m_startPos.y,m_startArea,0,0));

		//--
		int ways[EMaxArea]={0};
		int i;
		while(m_visitHeap.size()>0)
		{
			//--
			tagVisitNode top;
			m_visitHeap.pop(top);

			//--到达终点？
			if(top.x==m_targetPos.x 
				&& top.z==m_targetPos.y 
				&& top.area==m_targetArea)
			{
				bFind=true;
				break;
			}			

			//--left
			int numWay=m_pCallBack->GetWay(LocalX2GlobalX(top.x),
				LocalZ2GlobalZ(top.z),
				top.area,
				IAStarPathFinderCallBack::ED_Left,
				LocalX2GlobalX(top.x)-1,
				LocalZ2GlobalZ(top.z),
				ways);
			for(i=0;i<numWay;i++)
				if(!Visit(top.x-1,top.z,ways[i],top.x,top.z,top.area,top.dist2Start+1))
					return ER_Failed;

			//--right
			numWay=m_pCallBack->GetWay(LocalX2GlobalX(top.x),
				LocalZ2GlobalZ(top.z),
				top.area,
				IAStarPathFinderCallBack::ED_Right,
				LocalX2GlobalX(top.x)+1,
				LocalZ2GlobalZ(top.z),
				ways);
			for(i=0;i<numWay;i++)
				if(!Visit(top.x+1,top.z,ways[i],top.x,top.z,top.area,top.dist2Start+1))
					return ER_Failed;

			//--top
			numWay=m_pCallBack->GetWay(LocalX2GlobalX(top.x),
				LocalZ2GlobalZ(top.z),
				top.area,
				IAStarPathFinderCallBack::ED_Top,
				LocalX2GlobalX(top.x),
				LocalZ2GlobalZ(top.z)-1,
				ways);
			for(i=0;i<numWay;i++)
				if(!Visit(top.x,top.z-1,ways[i],top.x,top.z,top.area,top.dist2Start+1))
					return ER_Failed;

			//--bottom
			numWay=m_pCallBack->GetWay(LocalX2GlobalX(top.x),
				LocalZ2GlobalZ(top.z),
				top.area,
				IAStarPathFinderCallBack::ED_Bottom,
				LocalX2GlobalX(top.x),
				LocalZ2GlobalZ(top.z)+1,
				ways);
			for(i=0;i<numWay;i++)
				if(!Visit(top.x,top.z+1,ways[i],top.x,top.z,top.area,top.dist2Start+1))
					return ER_Failed;

			//--left-top
			numWay=m_pCallBack->GetWay(LocalX2GlobalX(top.x),
				LocalZ2GlobalZ(top.z),
				top.area,
				IAStarPathFinderCallBack::ED_LeftTop,
				LocalX2GlobalX(top.x)-1,
				LocalZ2GlobalZ(top.z)-1,
				ways);
			for(i=0;i<numWay;i++)
				if(!Visit(top.x-1,top.z-1,ways[i],top.x,top.z,top.area,top.dist2Start+SQRT2))
					return ER_Failed;

			//--left-bottom
			numWay=m_pCallBack->GetWay(LocalX2GlobalX(top.x),
				LocalZ2GlobalZ(top.z),
				top.area,
				IAStarPathFinderCallBack::ED_LeftBottom,
				LocalX2GlobalX(top.x)-1,
				LocalZ2GlobalZ(top.z)+1,
				ways);
			for(i=0;i<numWay;i++)
				if(!Visit(top.x-1,top.z+1,ways[i],top.x,top.z,top.area,top.dist2Start+SQRT2))
					return ER_Failed;

			//--right-top
			numWay=m_pCallBack->GetWay(LocalX2GlobalX(top.x),
				LocalZ2GlobalZ(top.z),
				top.area,
				IAStarPathFinderCallBack::ED_RightTop,
				LocalX2GlobalX(top.x)+1,
				LocalZ2GlobalZ(top.z)-1,
				ways);
			for(i=0;i<numWay;i++)
				if(!Visit(top.x+1,top.z-1,ways[i],top.x,top.z,top.area,top.dist2Start+SQRT2))
					return ER_Failed;

			//--right-bottom
			numWay=m_pCallBack->GetWay(LocalX2GlobalX(top.x),
				LocalZ2GlobalZ(top.z),
				top.area,
				IAStarPathFinderCallBack::ED_RightBottom,
				LocalX2GlobalX(top.x)+1,
				LocalZ2GlobalZ(top.z)+1,
				ways);
			for(i=0;i<numWay;i++)
				if(!Visit(top.x+1,top.z+1,ways[i],top.x,top.z,top.area,top.dist2Start+SQRT2))
					return ER_Failed;
		}

		//--串连路径
		if(bFind)
		{
			BuildPath(path,m_targetPos.x,m_targetPos.y,m_targetArea);
			return ER_Arrive;
		}
		else
		{
			int nearX,nearZ,nearArea;
			if(!FindNearPos(nearX,nearZ,nearArea))
				return ER_Failed;
			if(nearX==m_startPos.x&&nearZ==m_startPos.y)
				return ER_Failed;

			BuildPath(path,nearX,nearZ,nearArea);
			return ER_GetNearPos;
		}
	}

	bool AStarPathFinder::FindNearPos(int& nearX,int& nearZ,int& nearArea)
	{
		bool bFind=false;
		float minDist=-1;

		int z,x,area;
		for(z=0;z<ELocalMapSize;z++)
		{
			for(x=0;x<ELocalMapSize;x++)
			{
				for(area=0;area<EMaxArea;area++)
				{
					if(m_visitFlag[z][x][area])
					{
						if(minDist==-1
							||m_visitMap[z][x][area].dist2Target<minDist)
						{
							minDist=m_visitMap[z][x][area].dist2Target;
							nearX=x;
							nearZ=z;
							nearArea=area;

							bFind=true;
						}
					}
				}
			}
		}

		return bFind;
	}

	void AStarPathFinder::BuildPath(list<tagPathPoint>& path,int targetX,int targetZ,int targetArea)
	{
		tagPathPoint point;
		int x=targetX;
		int z=targetZ;
		int area=targetArea;
		while(x!=m_startPos.x||z!=m_startPos.y||area!=m_startArea)
		{
			point.x=LocalX2GlobalX(x);
			point.z=LocalZ2GlobalZ(z);
			point.area=area;
			path.push_front(point);

			int parentX=m_visitMap[z][x][area].parentX;
			int parentZ=m_visitMap[z][x][area].parentZ;
			int parentArea=m_visitMap[z][x][area].parentArea;

			x=parentX;
			z=parentZ;
			area=parentArea;
		}
	}

	bool AStarPathFinder::Visit(int x,int z,int area,int parentX,int parentZ,int parentArea,float dist2Start)
	{
		if(x<0||x>=ELocalMapSize)
			return true;
		if(z<0||z>=ELocalMapSize)
			return true;
		ASSERT(area>=0&&area<EMaxArea);

		if(!m_visitFlag[z][x][area])
		{
			m_visitFlag[z][x][area]=true;

			float dist2Target=_Distance(x,z,m_targetPos.x,m_targetPos.y);
			float value=dist2Start+dist2Target;

			if(!m_visitHeap.push(tagVisitNode(x,z,area,value,dist2Start)))
				return false;

			m_visitMap[z][x][area].parentX=parentX;
			m_visitMap[z][x][area].parentZ=parentZ;
			m_visitMap[z][x][area].parentArea=parentArea;
			m_visitMap[z][x][area].dist2Start=dist2Start;
			m_visitMap[z][x][area].dist2Target=dist2Target;
		}
		else
		{
			if(dist2Start<m_visitMap[z][x][area].dist2Start)
			{
				float value=m_visitMap[z][x][area].dist2Target+dist2Start;

				if(!m_visitHeap.push(tagVisitNode(x,z,area,value,dist2Start)))
					return false;

				m_visitMap[z][x][area].parentX=parentX;
				m_visitMap[z][x][area].parentZ=parentZ;
				m_visitMap[z][x][area].parentArea=parentArea;
				m_visitMap[z][x][area].dist2Start=dist2Start;
			}
		}

		return true;
	}

	void AStarPathFinder::TraceHeap()
	{
		TRACE(_T("\nAStar heap:"));
		for(int i=1;i<=m_visitHeap.size();i++)
		{
			TRACE(_T("%f,"),m_visitHeap[i].value);
		}
		TRACE(_T("\n"));
	}
}//namespace WorldBase
