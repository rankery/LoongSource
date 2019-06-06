#include "StdAfx.h"
#include "PathFinderByPatchDataGen.h"

namespace WorldBase
{
	PathFinderByPatchDataGen::PathFinderByPatchDataGen(void)
	{
		m_pCangoMap=NULL;
	}

	PathFinderByPatchDataGen::~PathFinderByPatchDataGen(void)
	{
	}

	void PathFinderByPatchDataGen::BeginBuild( BitMap* pCangoMap )
	{
		m_pCangoMap=pCangoMap;
	}

	bool PathFinderByPatchDataGen::EndBuild( const TCHAR* szSavePath,const TCHAR* szMapName,const TCHAR* szFileName )
	{
		int numPatchesX=m_pCangoMap->Width()/EPatchSize;
		int numPatchesZ=m_pCangoMap->Height()/EPatchSize;
		if(m_pCangoMap->Width()%EPatchSize!=0)
			numPatchesX++;
		if(m_pCangoMap->Height()%EPatchSize!=0)
			numPatchesZ++;

		PathFinderByPatch::tagPatch* pPatches=new PathFinderByPatch::tagPatch[numPatchesX*numPatchesZ];

		//--build areamap of patch
		int x,z;
		for(z=0;z<numPatchesZ;z++)
		{
			for(x=0;x<numPatchesX;x++)
			{
				PathFinderByPatch::tagPatch* pPatch=&pPatches[x+z*numPatchesX];

				HalfByteMap halfByteMap;
				BuildAreaMap(pPatch,x*EPatchSize,z*EPatchSize,&halfByteMap);
				pPatch->areaMap.Create(&halfByteMap);
			}
		}

		//--build linkpoints of patch
		for(z=0;z<numPatchesZ;z++)
		{
			for(x=0;x<numPatchesX;x++)
			{
				//left
				if(x>0)
					BuildLeftLinkPoints(
						&pPatches[x+numPatchesX*z],
						&pPatches[x-1+numPatchesX*z]);

				//right
				if(x<numPatchesX-1)
					BuildRightLinkPoints(
						&pPatches[x+numPatchesX*z],
						&pPatches[x+1+numPatchesX*z]);

				//top
				if(z>0)
					BuildTopLinkPoints(
						&pPatches[x+numPatchesX*z],
						&pPatches[x+numPatchesX*(z-1)]);

				//bottom
				if(z<numPatchesZ-1)
					BuildBottomLinkPoints(
						&pPatches[x+numPatchesX*z],
						&pPatches[x+numPatchesX*(z+1)]);

				//left top
				if(x>0&&z>0)
					BuildLeftTopLinkPoints(
						&pPatches[x+numPatchesX*z],
						&pPatches[x-1+numPatchesX*(z-1)],
						&pPatches[x-1+numPatchesX*z],
						&pPatches[x+numPatchesX*(z-1)]);

				//left bottom
				if(x>0&&z<numPatchesZ-1)
					BuildLeftBottomLinkPoints(
						&pPatches[x+numPatchesX*z],
						&pPatches[x-1+numPatchesX*(z+1)],
						&pPatches[x-1+numPatchesX*z],
						&pPatches[x+numPatchesX*(z+1)]);

				//right top
				if(x<numPatchesX-1&&z>0)
					BuildRightTopLinkPoints(
						&pPatches[x+numPatchesX*z],
						&pPatches[x+1+numPatchesX*(z-1)],
						&pPatches[x+1+numPatchesX*z],
						&pPatches[x+numPatchesX*(z-1)]);

				//right bottom
				if(x<numPatchesX-1&&z<numPatchesZ-1)
					BuildRightBottomLinkPoints(
						&pPatches[x+numPatchesX*z],
						&pPatches[x+1+numPatchesX*(z+1)],
						&pPatches[x+1+numPatchesX*z],
						&pPatches[x+numPatchesX*(z+1)]);
			}
		}

#ifdef _DEBUG
		Image img;
		img.Create(m_pCangoMap->Width(),m_pCangoMap->Height(),EPF_R8G8B8);

		//--画可行走区域
		tagBGR* pPixel=(tagBGR*)img.GetRawBuffer();
		for(z=0;z<(int)m_pCangoMap->Height();z++)
		{
			for(x=0;x<(int)m_pCangoMap->Width();x++)
			{
				if(m_pCangoMap->GetValue(x,z))
				{
					pPixel->B=255;
					pPixel->G=255;
					pPixel->R=255;
				}
				else
				{
					pPixel->B=0;
					pPixel->G=0;
					pPixel->R=0;
				}
				pPixel++;
			}
		}

		//--画地块边缘
		pPixel=(tagBGR*)img.GetRawBuffer();
		for(z=0;z<(int)m_pCangoMap->Height();z+=EPatchSize)
		{
			for(x=0;x<(int)m_pCangoMap->Width();x++)
			{
				tagBGR& pixel=pPixel[x+z*img.Width()];
				pixel.G=255;
				pixel.R=0;
				pixel.B=0;
			}
		}
		for(x=0;x<(int)m_pCangoMap->Width();x+=EPatchSize)
		{
			for(z=0;z<(int)m_pCangoMap->Height();z++)
			{
				tagBGR& pixel=pPixel[x+z*img.Width()];
				pixel.G=255;
				pixel.R=0;
				pixel.B=0;
			}
		}

		//--画连接点
		for(z=0;z<numPatchesZ;z++)
		{
			for(x=0;x<numPatchesX;x++)
			{
				PathFinderByPatch::tagPatch* pPatch=&pPatches[x+z*numPatchesX];
				vector<PathFinderByPatch::tagLinkPoint>& linkPoints=pPatch->linkPoints;
				for(int i=0;i<(int)linkPoints.size();i++)
				{
					PathFinderByPatch::tagLinkPoint& linkPoint=linkPoints[i];
					int imgx=x*EPatchSize+linkPoint.xInPatch;
					int imgz=z*EPatchSize+linkPoint.zInPatch;
					tagBGR& pixel=pPixel[imgx+imgz*img.Width()];
					pixel.B=0;
					pixel.G=0;
					pixel.R=255;
				}
			}
		}

		img.WriteToFile(_T("d:\\playerpathfinder.tga"));
#endif

		//--save to file
		if(!PathFinderByPatch::Save(pPatches,numPatchesX,numPatchesZ,szSavePath,szFileName))
		{
			SAFE_DELETEA(pPatches);
			return false;
		}

		SAFE_DELETEA(pPatches);
		return true;
	}

	void PathFinderByPatchDataGen::BuildAreaMap( PathFinderByPatch::tagPatch* pPatch,int baseX,int baseZ,HalfByteMap* pOut )
	{
		int areaCount=1;

		pOut->Create(EPatchSize,EPatchSize,0);
		bool visitFlag[EPatchSize][EPatchSize]={false};

		deque<POINT> q;
		int x,z;
		POINT p,top;
		for(z=0;z<EPatchSize;z++)
		{
			for(x=0;x<EPatchSize;x++)
			{
				if(!visitFlag[z][x] && m_pCangoMap->GetValue(x+baseX,z+baseZ))
				{
					visitFlag[z][x]=true;
					pOut->SetValue(x,z,areaCount);

					q.clear();
					p.x=x;
					p.y=z;
					q.push_back(p);

					while(!q.empty())
					{
						top=q.front();
						q.pop_front();

						//left
						if(top.x-1>=0 
							&&!visitFlag[top.y][top.x-1] 
							&&m_pCangoMap->GetValue(top.x-1+baseX,top.y+baseZ))
						{
							visitFlag[top.y][top.x-1]=true;
							pOut->SetValue(top.x-1,top.y,areaCount);
							p.x=top.x-1;
							p.y=top.y;
							q.push_back(p);
						}
						//right
						if(top.x+1<EPatchSize 
							&&!visitFlag[top.y][top.x+1] 
							&&m_pCangoMap->GetValue(top.x+1+baseX,top.y+baseZ))
						{
							visitFlag[top.y][top.x+1]=true;
							pOut->SetValue(top.x+1,top.y,areaCount);
							p.x=top.x+1;
							p.y=top.y;
							q.push_back(p);
						}
						//top
						if(top.y-1>=0
							&&!visitFlag[top.y-1][top.x]
							&&m_pCangoMap->GetValue(top.x+baseX,top.y-1+baseZ))
						{
							visitFlag[top.y-1][top.x]=true;
							pOut->SetValue(top.x,top.y-1,areaCount);
							p.x=top.x;
							p.y=top.y-1;
							q.push_back(p);
						}
						//bottom
						if(top.y+1<EPatchSize
							&&!visitFlag[top.y+1][top.x]
							&&m_pCangoMap->GetValue(top.x+baseX,top.y+1+baseZ))
						{
							visitFlag[top.y+1][top.x]=true;
							pOut->SetValue(top.x,top.y+1,areaCount);
							p.x=top.x;
							p.y=top.y+1;
							q.push_back(p);
						}
					}

					areaCount++;
					if(areaCount>=10)
						return;
				}//end if
			}//end for
		}//end for
	}

	void PathFinderByPatchDataGen::BuildLeftLinkPoints( 
		PathFinderByPatch::tagPatch* pPatch,
		PathFinderByPatch::tagPatch* pLeftPatch )
	{
		map<int,vector<int>> linkPoints;

		for(int y=0;y<EPatchSize;y++)
		{
			int srcArea=pPatch->areaMap.GetValue(0,y);
			int dstArea=pLeftPatch->areaMap.GetValue(PathFinderByPatch::EPatchSize-1,y);
			if(srcArea!=0&&dstArea!=0)
			{
				int key=MAKELONG(srcArea,dstArea);
				linkPoints[key].push_back(y);
			}
		}

		for(map<int,vector<int>>::iterator iter=linkPoints.begin();
			iter!=linkPoints.end();iter++)
		{
			PathFinderByPatch::tagLinkPoint linkPoint;
			linkPoint.dir		= IAStarPathFinderCallBack::ED_Left;
			linkPoint.myArea	= (BYTE)LOWORD(iter->first);
			linkPoint.destArea	= (BYTE)HIWORD(iter->first);
			linkPoint.xInPatch	= 0;
			linkPoint.zInPatch	= iter->second[iter->second.size()/2];

			pPatch->linkPoints.push_back(linkPoint);
		}
	}

	void PathFinderByPatchDataGen::BuildRightLinkPoints( 
		PathFinderByPatch::tagPatch* pPatch,
		PathFinderByPatch::tagPatch* pRightPatch )
	{
		map<int,vector<int>> linkPoints;

		for(int y=0;y<EPatchSize;y++)
		{
			int srcArea=pPatch->areaMap.GetValue(EPatchSize-1,y);
			int dstArea=pRightPatch->areaMap.GetValue(0,y);
			if(srcArea!=0&&dstArea!=0)
			{
				int key=MAKELONG(srcArea,dstArea);
				linkPoints[key].push_back(y);
			}
		}

		for(map<int,vector<int>>::iterator iter=linkPoints.begin();
			iter!=linkPoints.end();iter++)
		{
			PathFinderByPatch::tagLinkPoint linkPoint;
			linkPoint.dir		= IAStarPathFinderCallBack::ED_Right;
			linkPoint.myArea	= (BYTE)LOWORD(iter->first);
			linkPoint.destArea	= (BYTE)HIWORD(iter->first);
			linkPoint.xInPatch	= EPatchSize-1;
			linkPoint.zInPatch	= iter->second[iter->second.size()/2];

			pPatch->linkPoints.push_back(linkPoint);
		}
	}

	void PathFinderByPatchDataGen::BuildTopLinkPoints( 
		PathFinderByPatch::tagPatch* pPatch,
		PathFinderByPatch::tagPatch* pTopPatch )
	{
		map<int,vector<int>> linkPoints;

		for(int x=0;x<EPatchSize;x++)
		{
			int srcArea=pPatch->areaMap.GetValue(x,0);
			int dstArea=pTopPatch->areaMap.GetValue(x,EPatchSize-1);
			if(srcArea!=0&&dstArea!=0)
			{
				int key=MAKELONG(srcArea,dstArea);
				linkPoints[key].push_back(x);
			}
		}

		for(map<int,vector<int>>::iterator iter=linkPoints.begin();
			iter!=linkPoints.end();iter++)
		{
			PathFinderByPatch::tagLinkPoint linkPoint;
			linkPoint.dir		= IAStarPathFinderCallBack::ED_Top;
			linkPoint.myArea	= (BYTE)LOWORD(iter->first);
			linkPoint.destArea	= (BYTE)HIWORD(iter->first);
			linkPoint.xInPatch	= iter->second[iter->second.size()/2];
			linkPoint.zInPatch	= 0;

			pPatch->linkPoints.push_back(linkPoint);
		}
	}

	void PathFinderByPatchDataGen::BuildBottomLinkPoints( 
		PathFinderByPatch::tagPatch* pPatch,
		PathFinderByPatch::tagPatch* pBottomPatch )
	{
		map<int,vector<int>> linkPoints;

		for(int x=0;x<EPatchSize;x++)
		{
			int srcArea=pPatch->areaMap.GetValue(x,EPatchSize-1);
			int dstArea=pBottomPatch->areaMap.GetValue(x,0);
			if(srcArea!=0&&dstArea!=0)
			{
				int key=MAKELONG(srcArea,dstArea);
				linkPoints[key].push_back(x);
			}
		}

		for(map<int,vector<int>>::iterator iter=linkPoints.begin();
			iter!=linkPoints.end();iter++)
		{
			PathFinderByPatch::tagLinkPoint linkPoint;
			linkPoint.dir		= IAStarPathFinderCallBack::ED_Bottom;
			linkPoint.myArea	= (BYTE)LOWORD(iter->first);
			linkPoint.destArea	= (BYTE)HIWORD(iter->first);
			linkPoint.xInPatch	= iter->second[iter->second.size()/2];
			linkPoint.zInPatch	= EPatchSize-1;

			pPatch->linkPoints.push_back(linkPoint);
		}
	}

	void PathFinderByPatchDataGen::BuildLeftTopLinkPoints( 
		PathFinderByPatch::tagPatch* pPatch, 
		PathFinderByPatch::tagPatch* pLeftTopPatch, 
		PathFinderByPatch::tagPatch* pLeftPatch, 
		PathFinderByPatch::tagPatch* pTopPatch )
	{
		if(pPatch->areaMap.GetValue(0,0)!=0
			&&pLeftTopPatch->areaMap.GetValue(EPatchSize-1,EPatchSize-1)!=0)
		{
			if(pLeftPatch->areaMap.GetValue(EPatchSize-1,0)!=0
				||pTopPatch->areaMap.GetValue(0,EPatchSize-1)!=0)
			{
				PathFinderByPatch::tagLinkPoint linkPoint;
				linkPoint.dir		= IAStarPathFinderCallBack::ED_LeftTop;
				linkPoint.myArea	= pPatch->areaMap.GetValue(0,0);
				linkPoint.destArea	= pLeftTopPatch->areaMap.GetValue(EPatchSize-1,EPatchSize-1);
				linkPoint.xInPatch	= 0;
				linkPoint.zInPatch	= 0;

				pPatch->linkPoints.push_back(linkPoint);
			}
		}
	}

	void PathFinderByPatchDataGen::BuildLeftBottomLinkPoints( 
		PathFinderByPatch::tagPatch* pPatch, 
		PathFinderByPatch::tagPatch* pLeftBottomPatch, 
		PathFinderByPatch::tagPatch* pLeftPatch, 
		PathFinderByPatch::tagPatch* pBottomPatch )
	{
		if(pPatch->areaMap.GetValue(0,EPatchSize-1)!=0
			&&pLeftBottomPatch->areaMap.GetValue(EPatchSize-1,0)!=0)
		{
			if(pLeftPatch->areaMap.GetValue(EPatchSize-1,PathFinderByPatch::EPatchSize-1)!=0
				||pBottomPatch->areaMap.GetValue(0,0)!=0)
			{
				PathFinderByPatch::tagLinkPoint linkPoint;
				linkPoint.dir		= IAStarPathFinderCallBack::ED_LeftBottom;
				linkPoint.myArea	= pPatch->areaMap.GetValue(0,EPatchSize-1);
				linkPoint.destArea	= pLeftBottomPatch->areaMap.GetValue(EPatchSize-1,0);
				linkPoint.xInPatch	= 0;
				linkPoint.zInPatch	= EPatchSize-1;

				pPatch->linkPoints.push_back(linkPoint);
			}
		}
	}

	void PathFinderByPatchDataGen::BuildRightTopLinkPoints( 
		PathFinderByPatch::tagPatch* pPatch, 
		PathFinderByPatch::tagPatch* pRightTopPatch, 
		PathFinderByPatch::tagPatch* pRightPatch, 
		PathFinderByPatch::tagPatch* pTopPatch )
	{
		if(pPatch->areaMap.GetValue(EPatchSize-1,0)!=0
			&&pRightTopPatch->areaMap.GetValue(0,EPatchSize-1)!=0)
		{
			if(pRightPatch->areaMap.GetValue(0,0)!=0
				||pTopPatch->areaMap.GetValue(EPatchSize-1,EPatchSize-1)!=0)
			{
				PathFinderByPatch::tagLinkPoint linkPoint;
				linkPoint.dir		= IAStarPathFinderCallBack::ED_RightTop;
				linkPoint.myArea	= pPatch->areaMap.GetValue(EPatchSize-1,0);
				linkPoint.destArea	= pRightTopPatch->areaMap.GetValue(0,EPatchSize-1);
				linkPoint.xInPatch	= EPatchSize-1;
				linkPoint.zInPatch	= 0;

				pPatch->linkPoints.push_back(linkPoint);
			}
		}
	}

	void PathFinderByPatchDataGen::BuildRightBottomLinkPoints( 
		PathFinderByPatch::tagPatch* pPatch, 
		PathFinderByPatch::tagPatch* pRightBottomPatch, 
		PathFinderByPatch::tagPatch* pRightPatch, 
		PathFinderByPatch::tagPatch* pBottomPatch )
	{
		if(pPatch->areaMap.GetValue(EPatchSize-1,EPatchSize-1)!=0
			&&pRightBottomPatch->areaMap.GetValue(0,0)!=0)
		{
			if(pRightPatch->areaMap.GetValue(0,EPatchSize-1)!=0
				||pBottomPatch->areaMap.GetValue(EPatchSize-1,0)!=0)
			{
				PathFinderByPatch::tagLinkPoint linkPoint;
				linkPoint.dir		= IAStarPathFinderCallBack::ED_RightBottom;
				linkPoint.myArea	= pPatch->areaMap.GetValue(EPatchSize-1,EPatchSize-1);
				linkPoint.destArea	= pRightBottomPatch->areaMap.GetValue(0,0);
				linkPoint.xInPatch	= EPatchSize-1;
				linkPoint.zInPatch	= EPatchSize-1;

				pPatch->linkPoints.push_back(linkPoint);
			}
		}
	}
}//namespace WorldBase
