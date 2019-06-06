#include "StdAfx.h"
#include "OutlinePathFinder.h"

namespace WorldBase
{
	const int g_maxBlocks=10;			//最多可以绕过多少个障碍物
	const int g_maxRoundTiles=200;		//绕行障碍物时最多允许遍历多少个格子

#define CAN_GO(x,z)				(m_pCangoMap->GetValue(x,z))

#define CAN_GO_LT_CW()			(bCangoLeftTop		&&bCangoLeft	&&!bCangoTop		)
#define CAN_GO_RT_CW()			(bCangoRightTop		&&bCangoTop		&&!bCangoRight		)
#define CAN_GO_LB_CW()			(bCangoLeftBottom	&&bCangoBottom	&&!bCangoLeft		)
#define CAN_GO_RB_CW()			(bCangoRightBottom	&&bCangoRight	&&!bCangoBottom		)
#define CAN_GO_L_CW()			(bCangoLeft			&&!bCangoTop	&&!bCangoLeftTop	)
#define CAN_GO_R_CW()			(bCangoRight		&&!bCangoBottom	&&!bCangoRightBottom)
#define CAN_GO_T_CW()			(bCangoTop			&&!bCangoRight	&&!bCangoRightTop	)
#define CAN_GO_B_CW()			(bCangoBottom		&&!bCangoLeft	&&!bCangoLeftBottom	)
#define CAN_GO_L_RELAX_CW()		(bCangoLeft			&&!bCangoLeftTop					)
#define CAN_GO_R_RELAX_CW()		(bCangoRight		&&!bCangoRightBottom				)
#define CAN_GO_T_RELAX_CW()		(bCangoTop			&&!bCangoRightTop					)
#define CAN_GO_B_RELAX_CW()		(bCangoBottom		&&!bCangoLeftBottom					)

#define CAN_GO_LT_CCW()			(bCangoLeftTop		&&bCangoTop		&&!bCangoLeft		)
#define CAN_GO_RT_CCW()			(bCangoRightTop		&&bCangoRight	&&!bCangoTop		)
#define CAN_GO_LB_CCW()			(bCangoLeftBottom	&&bCangoLeft	&&!bCangoBottom		)
#define CAN_GO_RB_CCW()			(bCangoRightBottom	&&bCangoBottom	&&!bCangoRight		)
#define CAN_GO_L_CCW()			(bCangoLeft			&&!bCangoBottom	&&!bCangoLeftBottom	)
#define CAN_GO_R_CCW()			(bCangoRight		&&!bCangoTop	&&!bCangoRightTop	)
#define CAN_GO_T_CCW()			(bCangoTop			&&!bCangoLeft	&&!bCangoLeftTop	)
#define CAN_GO_B_CCW()			(bCangoBottom		&&!bCangoRight	&&!bCangoRightBottom)
#define CAN_GO_L_RELAX_CCW()	(bCangoLeft			&&!bCangoLeftBottom					)
#define CAN_GO_R_RELAX_CCW()	(bCangoRight		&&!bCangoRightTop					)
#define CAN_GO_T_RELAX_CCW()	(bCangoTop			&&!bCangoLeftTop					)
#define CAN_GO_B_RELAX_CCW()	(bCangoBottom		&&!bCangoRightBottom				)


	OutlinePathFinder::OutlinePathFinder(void):m_pCangoMap(NULL),m_maxRoundTiles(100)
	{}

	OutlinePathFinder::~OutlinePathFinder(void)
	{}

	void OutlinePathFinder::SetCangoMap( BitMap* pCangoMap )
	{
		m_pCangoMap=pCangoMap;
	}

	void OutlinePathFinder::SetMaxRoundTiles( int numTiles )
	{
		ASSERT(numTiles>0 && numTiles<=g_maxRoundTiles);
		m_maxRoundTiles=min(numTiles,g_maxRoundTiles);
	}

	bool OutlinePathFinder::FindPath( int x1,int z1,int x2,int z2,list<POINT>& path )
	{
		if(x1==x2&&z1==z2)
			return false;

		if(!m_pCangoMap->IfIndex(x1,z1)
			||!m_pCangoMap->IfIndex(x2,z2))
			return false;

		if(!CAN_GO(x1,z1)
			||!CAN_GO(x2,z2))
			return false;

		path.clear();

		//--画线找出所有障碍物的接入点和接出点
		tagBlock blocks[g_maxBlocks];
		int numBlocks=0;
		if(!FindBlocks(x1,z1,x2,z2,blocks,numBlocks))
			return false;

		//--遍历所有障碍物
		POINT tilesCW[g_maxRoundTiles];
		POINT tilesCCW[g_maxRoundTiles];
		int numTilesCW;
		int numTilesCCW;
		int distCW;
		int distCCW;
		for(int i=0;i<numBlocks;i++)
		{
			//--顺时钟绕行
			distCW=RoundBlock_CW(&blocks[i],tilesCW,numTilesCW);

			//--逆时钟绕行
			distCCW=RoundBlock_CCW(&blocks[i],tilesCCW,numTilesCCW);

			//--选择最近的绕行方向，添加到路径
			if(distCW>0 && distCCW>0)
			{
				if(distCW<distCCW)
					AddPathPoints(x1,z1,tilesCW,numTilesCW,path);
				else
					AddPathPoints(x1,z1,tilesCCW,numTilesCCW,path);
			}
			else if(distCW>0)
			{
				AddPathPoints(x1,z1,tilesCW,numTilesCW,path);
			}
			else if(distCCW>0)
			{
				AddPathPoints(x1,z1,tilesCCW,numTilesCCW,path);
			}
			else
				return false;
		}

		//--将终点添加到路径
		AddPathPoint(x1,z1,path,x2,z2);
		
		return true;
	}

	bool OutlinePathFinder::FindBlocks( int x1,int z1,int x2,int z2,tagBlock* pBlocks,int& numBlocks )
	{
		numBlocks = 0;

		INT dx = x2 >= x1 ? x2 - x1 : x1 - x2;
		INT s1 = x2 >= x1 ? 1 : -1;
		INT dy = z2 >= z1 ? z2 - z1 : z1 - z2;
		INT s2 = z2 >= z1 ? 1 : -1;

		INT interchange = dy > dx ? 1 : 0;

		if( dy > dx ) 
			std::swap(dx, dy);

		INT f = dy + dy - dx;

		BYTE by = 0;

		bool bBlocking = false;
		bool bBlock;
		INT oldx;
		INT oldz;
		for( INT i=1; i<=dx; i++ )
		{
			oldx = x1;
			oldz = z1;

			if( f >= 0 )  
			{
				if( interchange ) 
					x1 += s1;
				else 
					z1 += s2;

				f -= dx + dx;
			}

			if( interchange ) 
				z1 += s2;
			else 
				x1 += s1;

			f += dy + dy;

			bBlock = !CAN_GO(x1, z1);
			if( bBlock != bBlocking )
			{
				bBlocking = bBlock;

				if( bBlock )
				{
					if(numBlocks >= g_maxBlocks)
						return false;

					pBlocks[numBlocks].inPt.x = oldx;
					pBlocks[numBlocks].inPt.y = oldz;
				}
				else
				{
					pBlocks[numBlocks].outPt.x = x1;
					pBlocks[numBlocks].outPt.y = z1;

					numBlocks++;
				}
			}
		}

		return true;
	}

	void OutlinePathFinder::AddPathPoint( int x1,int z1,list<POINT>& path,int pointX,int pointZ )
	{
		POINT newPoint={pointX,pointZ};

		if(!path.empty())
		{
			POINT& lastPoint=path.back();

			if(lastPoint.x!=newPoint.x
				||lastPoint.y!=newPoint.y)
				path.push_back(newPoint);
		}
		else
		{
			if(x1!=newPoint.x||z1!=newPoint.y)
				path.push_back(newPoint);
		}
	}

	int OutlinePathFinder::RoundBlock_CW( const tagBlock* pBlock,POINT* pTiles,int& numOutTiles )
	{
		numOutTiles=0;

		int x=pBlock->inPt.x;
		int z=pBlock->inPt.y;
		int oldx=x;
		int oldz=z;

		bool bCangoTop;
		bool bCangoLeft;
		bool bCangoRight;
		bool bCangoBottom;
		bool bCangoLeftTop;
		bool bCangoRightTop;
		bool bCangoLeftBottom;
		bool bCangoRightBottom;
		int numVisitTiles=0;
		EWay way=EW_None;
		EWay newWay=EW_None;
		for(;;)
		{
			//遍历的格子数达到上限？
			if(numVisitTiles>=m_maxRoundTiles)
				return 0;

			//到达接出点？
			if(abs(x-pBlock->outPt.x)<2
				&&abs(z-pBlock->outPt.y)<2)
			{
				//放入接出点
				ASSERT(numOutTiles<g_maxRoundTiles);
				pTiles[numOutTiles]=pBlock->outPt;
				numOutTiles++;
				break;
			}

			//防止越界
			if(x<10
				||z<10
				||x+10>(int)m_pCangoMap->Width()
				||z+10>(int)m_pCangoMap->Height())
				return 0;

			bCangoTop=CAN_GO(x,z-1);
			bCangoLeft=CAN_GO(x-1,z);
			bCangoRight=CAN_GO(x+1,z);
			bCangoBottom=CAN_GO(x,z+1);
			bCangoLeftTop=CAN_GO(x-1,z-1);
			bCangoRightTop=CAN_GO(x+1,z-1);
			bCangoLeftBottom=CAN_GO(x-1,z+1);
			bCangoRightBottom=CAN_GO(x+1,z+1);

			oldx=x;
			oldz=z;

			switch(way)
			{
			case EW_None:
				{
					//left-top
					if(CAN_GO_LT_CW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left
					else if(CAN_GO_L_RELAX_CW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_RELAX_CW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_RELAX_CW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_RELAX_CW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_Left:
				{
					//left-top
					if(CAN_GO_LT_CW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left
					else if(CAN_GO_L_CW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_CW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_Right:
				{
					//left-top
					if(CAN_GO_LT_CW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//right
					else if(CAN_GO_R_CW())
					{
						x++;
						newWay=EW_Right;
					}
					//left
					else if(CAN_GO_L_CW())
					{
						x--;
						newWay=EW_Left;
					}
					//top
					else if(CAN_GO_T_CW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_Top:
				{
					//left-top
					if(CAN_GO_LT_CW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//top
					else if(CAN_GO_T_CW())
					{
						z--;
						newWay=EW_Top;
					}
					//left
					else if(CAN_GO_L_CW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CW())
					{
						x++;
						newWay=EW_Right;
					}
					//bottom
					else if(CAN_GO_B_CW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_Bottom:
				{
					//left-top
					if(CAN_GO_LT_CW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//bottom
					else if(CAN_GO_B_CW())
					{
						z++;
						newWay=EW_Bottom;
					}
					//right
					else if(CAN_GO_R_CW())
					{
						x++;
						newWay=EW_Right;
					}
					//left
					else if(CAN_GO_L_CW())
					{
						x--;
						newWay=EW_Left;
					}
					//top
					else if(CAN_GO_T_CW())
					{
						z--;
						newWay=EW_Top;
					}
					else
						return 0;
				}
				break;
			case EW_LeftTop:
				{
					//left-top
					if(CAN_GO_LT_CW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left
					else if(CAN_GO_L_CW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_CW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_RightTop:
				{
					//right-top
					if(CAN_GO_RT_CW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-top
					else if(CAN_GO_LT_CW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left
					else if(CAN_GO_L_CW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_CW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_LeftBottom:
				{
					//left-bottom
					if(CAN_GO_LB_CW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//left-top
					else if(CAN_GO_LT_CW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//right-bottom
					else if(CAN_GO_RB_CW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left
					else if(CAN_GO_L_CW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_CW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_RightBottom:
				{
					//right-bottom
					if(CAN_GO_RB_CW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left-top
					else if(CAN_GO_LT_CW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//left
					else if(CAN_GO_L_CW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_CW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			default:
				return 0;
			}

			if(way!=newWay)
			{
				ASSERT(numOutTiles<g_maxRoundTiles);
				pTiles[numOutTiles].x=oldx;
				pTiles[numOutTiles].y=oldz;
				numOutTiles++;
			}
			way=newWay;

			//TRACE(_T("%d,%d\n"),x,z);

			++numVisitTiles;
		}

		return numVisitTiles;
	}

	int OutlinePathFinder::RoundBlock_CCW( const tagBlock* pBlock,POINT* pTiles,int& numOutTiles )
	{
		numOutTiles=0;

		int x=pBlock->inPt.x;
		int z=pBlock->inPt.y;
		int oldx=x;
		int oldz=z;

		bool bCangoTop;
		bool bCangoLeft;
		bool bCangoRight;
		bool bCangoBottom;
		bool bCangoLeftTop;
		bool bCangoRightTop;
		bool bCangoLeftBottom;
		bool bCangoRightBottom;
		int numVisitTiles=0;
		EWay way=EW_None;
		EWay newWay=EW_None;
		for(;;)
		{
			//遍历的格子数达到上限？
			if(numVisitTiles>=m_maxRoundTiles)
				return 0;

			//到达接出点？
			if(abs(x-pBlock->outPt.x)<2
				&&abs(z-pBlock->outPt.y)<2)
			{
				//放入接出点
				ASSERT(numOutTiles<g_maxRoundTiles);
				pTiles[numOutTiles]=pBlock->outPt;
				numOutTiles++;
				break;
			}

			//防止越界
			if(x<10
				||z<10
				||x+10>(int)m_pCangoMap->Width()
				||z+10>(int)m_pCangoMap->Height())
				return 0;

			bCangoTop=CAN_GO(x,z-1);
			bCangoLeft=CAN_GO(x-1,z);
			bCangoRight=CAN_GO(x+1,z);
			bCangoBottom=CAN_GO(x,z+1);
			bCangoLeftTop=CAN_GO(x-1,z-1);
			bCangoRightTop=CAN_GO(x+1,z-1);
			bCangoLeftBottom=CAN_GO(x-1,z+1);
			bCangoRightBottom=CAN_GO(x+1,z+1);

			oldx=x;
			oldz=z;

			switch(way)
			{
			case EW_None:
				{
					//left-top
					if(CAN_GO_LT_CCW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CCW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CCW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CCW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left
					else if(CAN_GO_L_RELAX_CCW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_RELAX_CCW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_RELAX_CCW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_RELAX_CCW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_Left:
				{
					//left-top
					if(CAN_GO_LT_CCW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CCW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CCW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CCW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left
					else if(CAN_GO_L_CCW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CCW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_CCW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CCW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_Right:
				{
					//left-top
					if(CAN_GO_LT_CCW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CCW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CCW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CCW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//right
					else if(CAN_GO_R_CCW())
					{
						x++;
						newWay=EW_Right;
					}
					//left
					else if(CAN_GO_L_CCW())
					{
						x--;
						newWay=EW_Left;
					}
					//top
					else if(CAN_GO_T_CCW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CCW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_Top:
				{
					//left-top
					if(CAN_GO_LT_CCW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CCW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CCW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CCW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//top
					else if(CAN_GO_T_CCW())
					{
						z--;
						newWay=EW_Top;
					}
					//left
					else if(CAN_GO_L_CCW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CCW())
					{
						x++;
						newWay=EW_Right;
					}
					//bottom
					else if(CAN_GO_B_CCW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_Bottom:
				{
					//left-top
					if(CAN_GO_LT_CCW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CCW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CCW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CCW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//bottom
					else if(CAN_GO_B_CCW())
					{
						z++;
						newWay=EW_Bottom;
					}
					//left
					else if(CAN_GO_L_CCW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CCW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_CCW())
					{
						z--;
						newWay=EW_Top;
					}
					else
						return 0;
				}
				break;
			case EW_LeftTop:
				{
					//left-top
					if(CAN_GO_LT_CCW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CCW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CCW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CCW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left
					else if(CAN_GO_L_CCW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CCW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_CCW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CCW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_RightTop:
				{
					//right-top
					if(CAN_GO_RT_CCW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-top
					else if(CAN_GO_LT_CCW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CCW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//right-bottom
					else if(CAN_GO_RB_CCW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left
					else if(CAN_GO_L_CCW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CCW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_CCW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CCW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_LeftBottom:
				{
					//left-bottom
					if(CAN_GO_LB_CCW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//left-top
					else if(CAN_GO_LT_CCW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CCW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//right-bottom
					else if(CAN_GO_RB_CCW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left
					else if(CAN_GO_L_CCW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CCW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_CCW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CCW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			case EW_RightBottom:
				{
					//right-bottom
					if(CAN_GO_RB_CCW())
					{
						x++;z++;
						newWay=EW_RightBottom;
					}
					//left-top
					else if(CAN_GO_LT_CCW())
					{
						x--;z--;
						newWay=EW_LeftTop;
					}
					//right-top
					else if(CAN_GO_RT_CCW())
					{
						x++;z--;
						newWay=EW_RightTop;
					}
					//left-bottom
					else if(CAN_GO_LB_CCW())
					{
						x--;z++;
						newWay=EW_LeftBottom;
					}
					//left
					else if(CAN_GO_L_CCW())
					{
						x--;
						newWay=EW_Left;
					}
					//right
					else if(CAN_GO_R_CCW())
					{
						x++;
						newWay=EW_Right;
					}
					//top
					else if(CAN_GO_T_CCW())
					{
						z--;
						newWay=EW_Top;
					}
					//bottom
					else if(CAN_GO_B_CCW())
					{
						z++;
						newWay=EW_Bottom;
					}
					else
						return 0;
				}
				break;
			default:
				return 0;
			}

			if(way!=newWay)
			{
				ASSERT(numOutTiles<g_maxRoundTiles);
				pTiles[numOutTiles].x=oldx;
				pTiles[numOutTiles].y=oldz;
				numOutTiles++;
			}
			way=newWay;

			++numVisitTiles;
		}

		return numVisitTiles;
	}

	void OutlinePathFinder::AddPathPoints( int x1,int z1,const POINT* pPoints,int numPoints,list<POINT>& path )
	{
		for(int i=0;i<numPoints;i++)
			AddPathPoint(x1,z1,path,pPoints[i].x,pPoints[i].y);
	}

}//namespace WorldBase
