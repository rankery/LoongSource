#pragma once
#include "AreaMap.h"
#include "AStarPathFinder.h"

namespace WorldBase
{
	/** \class PathFinderByPatch
		\brief 基于地形块的寻路器
	*/
	class PathFinderByPatch : public IAStarPathFinderCallBack
	{
	public:
		enum EConst
		{
			EPatchSize=32
		};

		enum EResult
		{
			ER_Failed,		//寻路失败
			ER_Arrive,		//到达终点
			ER_GetNearPos,	//到达附近位置
		};

		struct tagLinkPoint//连接点
		{
			BYTE dir;
			BYTE myArea;
			BYTE destArea;
			BYTE xInPatch;
			BYTE zInPatch;
		};

		struct tagPatch//地形块
		{
			vector<tagLinkPoint> linkPoints;
			AreaMap areaMap;
		};

	public:
		PathFinderByPatch(void);
		virtual ~PathFinderByPatch(void);

		//--
		void LoadFromFile(const TCHAR* szMapName,const TCHAR* szFileName,IFS* pFS);
		void Destroy();

		EResult FindPath(int sx,int sz,int dx,int dz,list<Point>& path);

		//--IAStarPathFinderCallBack
		virtual int GetWay(int sx,int sz,int sa,EDir dir,int dx,int dz,int* pda);

		//--Editor support
		static bool Save(tagPatch* pPatches,int numPatchesX,int numPatchesZ,const TCHAR* szMapPath,const TCHAR* szFileName);

	private:
		inline int Tile2Patch(int tile){ return tile/EPatchSize;}
		int GetArea(int x,int z);
		void BuildPathPoint(int spx,int spz,int sa,int dpx,int dpz,int da,Point& tile);
		bool FindNearestTile(int px,int pz,int area,int dx,int dz,Point& out);

	private:
		tagPatch* m_pPatches;
		int m_numPatchesX;
		int m_numPatchesZ;
	};

}//namespace WorldBase
