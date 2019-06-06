#include "StdAfx.h"
#include "PathFinderByPatch.h"
#include "AStarPathFinder.h"

namespace WorldBase
{
	PathFinderByPatch::PathFinderByPatch(void)
		: m_pPatches(NULL)
	{}

	PathFinderByPatch::~PathFinderByPatch(void)
	{
		Destroy();
	}

	void PathFinderByPatch::Destroy()
	{
		SAFE_DELETEA(m_pPatches);
	}

	void PathFinderByPatch::LoadFromFile( const TCHAR* szMapName,const TCHAR* szFileName,IFS* pFS )
	{
		Destroy();

		TCHAR szPath[512];
		_stprintf(szPath,_T("data\\map\\%s\\nav\\%s.data"),szMapName,szFileName);

		DWORD hFile=pFS->Open(szPath);
		THROW_NULL(hFile,_T("load nav player path find data fail!"));

		FReadValue(pFS,hFile,m_numPatchesX);
		FReadValue(pFS,hFile,m_numPatchesZ);
		m_pPatches=new tagPatch[m_numPatchesX*m_numPatchesZ];
		tagPatch* pPatch=&m_pPatches[0];
		for(int i=0;i<m_numPatchesX*m_numPatchesZ;i++)
		{
			pPatch->areaMap.ReadFile(pFS,hFile);
			FReadValVector(pFS,hFile,pPatch->linkPoints);
			pPatch++;
		}

		pFS->Close(hFile);
	}

	bool PathFinderByPatch::Save( tagPatch* pPatches,int numPatchesX,int numPatchesZ,const TCHAR* szMapPath,const TCHAR* szFileName )
	{
		TCHAR szPath[512];
		_stprintf(szPath,_T("%snav\\%s.data"),szMapPath,szFileName);
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());

		FILE* fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
			return false;

		FWriteValue(fp,numPatchesX);
		FWriteValue(fp,numPatchesZ);

		tagPatch* pPatch=pPatches;
		for(int i=0;i<numPatchesX*numPatchesZ;i++)
		{
			pPatch->areaMap.WriteFile(fp);
			FWriteValVector(fp,pPatch->linkPoints);
			pPatch++;
		}

		fclose(fp);

		return true;
	}

	int PathFinderByPatch::GetWay( int sx,int sz,int sa,EDir dir,int dx,int dz,int* pda )
	{
		int areaCount=0;

		tagPatch* pPatch=&m_pPatches[sx+sz*m_numPatchesX];
		for(size_t i=0;i<pPatch->linkPoints.size();i++)
		{
			tagLinkPoint& linkPt=pPatch->linkPoints[i];
			if(linkPt.dir==dir
				&&linkPt.myArea==sa)
			{
				pda[areaCount++]=linkPt.destArea;
				if(areaCount>=AStarPathFinder::EMaxArea)
					break;
			}
		}

		return areaCount;
	}

	PathFinderByPatch::EResult PathFinderByPatch::FindPath( int sx,int sz,int dx,int dz,list<Point>& path )
	{
		path.clear();

		//--判断起点与终点是否相同
		if(sx==dx&&sz==dz)
			return ER_Failed;

		int spx=Tile2Patch(sx);
		int spz=Tile2Patch(sz);
		int dpx=Tile2Patch(dx);
		int dpz=Tile2Patch(dz);
		int sa=GetArea(sx,sz);
		int da=GetArea(dx,dz);
		
		//--如果位于同一地块的同一区域，直接返回终点作为路径
		if(spx==dpx&&spz==dpz&&sa==da)
		{
			path.push_back(Point(dx,dz));
			return ER_Arrive;
		}

		//--A*寻路
		list<AStarPathFinder::tagPathPoint> patchPath;
		AStarPathFinder::Inst()->SetCallBack(this);
		AStarPathFinder::EResult result=AStarPathFinder::Inst()->FindPath(spx,spz,sa,dpx,dpz,da,patchPath);
		if(result==AStarPathFinder::ER_Failed)
		{
			Point point;
			if(!FindNearestTile(spx,spz,sa,dx,dz,point))
				return ER_Failed;

			if(point.x==sx
				&&point.y==sz)
				return ER_Failed;

			path.push_back(point);
			return ER_GetNearPos;
		}

		//--生成基于tile的路径
		ASSERT(patchPath.size()>0);
		int px=spx;
		int pz=spz;
		int area=sa;
		for(list<AStarPathFinder::tagPathPoint>::iterator iter=patchPath.begin();
			iter!=patchPath.end();iter++)
		{
			Point point;
			BuildPathPoint(px,pz,area,iter->x,iter->z,iter->area,point);
			path.push_back(point);

			px=iter->x;
			pz=iter->z;
			area=iter->area;
		}

		//--生成终点tile坐标
		if(patchPath.back().x==dpx
			&&patchPath.back().z==dpz
			&&patchPath.back().area==da)
		{
			path.push_back(Point(dx,dz));
			return ER_Arrive;
		}
		else
		{
			Point point;
			if(!FindNearestTile(patchPath.back().x,patchPath.back().z,patchPath.back().area,dx,dz,point))
				return ER_Failed;

			path.push_back(point);
			return ER_GetNearPos;
		}
	}

	int PathFinderByPatch::GetArea( int x,int z )
	{
		int px=Tile2Patch(x);
		int pz=Tile2Patch(z);
		tagPatch* pPatch=&m_pPatches[px+pz*m_numPatchesX];
		int lx=x%EPatchSize;
		int lz=z%EPatchSize;
		return pPatch->areaMap.SafeGetValue(lx,lz);
	}

	void PathFinderByPatch::BuildPathPoint(int spx,int spz,int sa,int dpx,int dpz,int da,Point& tile)
	{
		tile.x=0;
		tile.y=0;

		//--判断方向
		EDir dir=ED_Left;
		if(dpx==spx-1 && dpz==spz)
			dir=ED_Left;
		else if(dpx==spx+1 && dpz==spz)
			dir=ED_Right;
		else if(dpx==spx && dpz==spz-1)
			dir=ED_Top;
		else if(dpx==spx && dpz==spz+1)
			dir=ED_Bottom;
		else if(dpx==spx-1 && dpz==spz-1)
			dir=ED_LeftTop;
		else if(dpx==spx-1 && dpz==spz+1)
			dir=ED_LeftBottom;
		else if(dpx==spx+1 && dpz==spz-1)
			dir=ED_RightTop;
		else if(dpx==spx+1 && dpz==spz+1)
			dir=ED_RightBottom;
		else
		{
			ASSERT(0);
			return;
		}

		//--
		tagPatch* pPatch=&m_pPatches[spx+spz*m_numPatchesX];
		for(int i=0;i<(int)pPatch->linkPoints.size();i++)
		{
			tagLinkPoint& linkPt=pPatch->linkPoints[i];
			if(linkPt.dir==dir
				&&linkPt.myArea==sa
				&&linkPt.destArea==da)
			{
				tile.x=linkPt.xInPatch+spx*EPatchSize;
				tile.y=linkPt.zInPatch+spz*EPatchSize;
				return;
			}
		}

		ASSERT(0);
	}

	inline float _DistanceSq(int x1,int y1,int x2,int y2)
	{
		float sq=float(abs((x2-x1)*(x2-x1))+abs((y2-y1)*(y2-y1)));
		return sq;
	}

	bool PathFinderByPatch::FindNearestTile(int px,int pz,int area,int dx,int dz,Point& out)
	{
		ASSERT(px>=0 && px<m_numPatchesX);
		ASSERT(pz>=0 && pz<m_numPatchesZ);

		bool bFind=false;
		float minDist=-1;

		tagPatch& patch=m_pPatches[px+pz*m_numPatchesX];

		int x,z;
		int baseX=px*EPatchSize;
		int baseZ=pz*EPatchSize;
		for(z=0;z<EPatchSize;z++)
		{
			for(x=0;x<EPatchSize;x++)
			{
				if(patch.areaMap.SafeGetValue(x,z)==area)
				{
					float dist=_DistanceSq(baseX+x,baseZ+z,dx,dz);
					if(minDist==-1
						||dist<minDist)
					{
						bFind=true;
						minDist=dist;
						out.x=baseX+x;
						out.y=baseZ+z;
					}
				}
			}
		}

		return bFind;
	}


}//namespace WorldBase
