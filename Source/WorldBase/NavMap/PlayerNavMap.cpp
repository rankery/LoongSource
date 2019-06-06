#include "StdAfx.h"
#include "PlayerNavMap.h"
#include "NavCollider.h"

namespace WorldBase
{
	const float g_tileSize=50.0f;

	PlayerNavMap::PlayerNavMap(void)
	{}

	PlayerNavMap::~PlayerNavMap(void)
	{}

	void PlayerNavMap::LoadFromFile( const TCHAR* szMapName,IFS* pMapFS,IFS* pSysFS )
	{
		//--程序自动生成的导航图
		TCHAR szPath[512];
		_stprintf(szPath,_T("data\\map\\%s\\nav\\playercango.bmap"),szMapName);
		DWORD hFile=pMapFS->Open(szPath);
		THROW_NULL(hFile,_T("load nav playercango.bmap fail"));
		
		m_cangoMap_Auto.ReadFile(pMapFS,hFile);
		pMapFS->Close(hFile);

		//--道路区域图
		_stprintf(szPath,_T("data\\map\\%s\\nav\\playerroadmap.bmap"),szMapName);
		hFile=pMapFS->Open(szPath);
		THROW_NULL(hFile,_T("load nav playerroadmap.bmap fail"));

		m_cangoMap_Road.ReadFile(pMapFS,hFile);
		pMapFS->Close(hFile);

		//--程序自动生成的地块寻路数据
		m_pathFinderByPatch_Auto.LoadFromFile(szMapName,_T("playerpatchpathdata"),pMapFS);

		//--道路区域地块寻路数据
		m_pathFinderByPatch_Road.LoadFromFile(szMapName,_T("playerroadpatchpathdata"),pMapFS);

		//--地图寻路数据
		m_pathFinderByMap.LoadFromFile(pSysFS);

		//test
		//m_cangoMap_Road.ExportImage(_T("d:\\test.tga"));
	}

	inline Point _World2Tile(const Vector3& wpt)
	{
		return Point(int(wpt.x/g_tileSize),int(wpt.z/g_tileSize));
	}

	bool PlayerNavMap::FindPathByMap( const TCHAR* szSrcMap,const TCHAR* szDestMap,Vector3& switchPos )
	{
		return m_pathFinderByMap.GetSwitchPos(szSrcMap,szDestMap,switchPos);
	}

	PlayerNavMap::EResult PlayerNavMap::FindPathByPatch( const Vector3& startPos,const Vector3& endPos,bool bRoadMap,list<Point>& path )
	{
		Point s=_World2Tile(startPos);
		Point d=_World2Tile(endPos);

		PathFinderByPatch::EResult result;
		if(bRoadMap)
			result=m_pathFinderByPatch_Road.FindPath(s.x,s.y,d.x,d.y,path);
		else
			result=m_pathFinderByPatch_Auto.FindPath(s.x,s.y,d.x,d.y,path);

		if(result==PathFinderByPatch::ER_Arrive)
			return ER_Arrive;
		else if(result==PathFinderByPatch::ER_GetNearPos)
			return ER_GetNearPos;
		else
			return ER_Failed;
	}

	PlayerNavMap::EResult PlayerNavMap::FindPathByTile( const Vector3& startPos,const Point& destTile,const Vector3& roleSize,bool bRoadMap,bool bRoadCheck,bool bHalfPath,bool bOnLand,NavCollider* pCollider,list<Vector3>& path )
	{
		if(bRoadMap)
			m_pathFinderByTile.SetCangoMap(&m_cangoMap_Road);
		else
			m_pathFinderByTile.SetCangoMap(&m_cangoMap_Auto);
		m_pathFinderByTile.SetCollider(pCollider);

		//--寻路
		list<AStarPathFinder::tagPathPoint> tilePath;
		PathFinderByTile::EResult result=m_pathFinderByTile.FindPath(startPos,destTile,roleSize,bHalfPath,tilePath);
		if(result==PathFinderByTile::ER_Failed)
			return ER_Failed;

		//--检查是否进入道路区域
		if(bRoadCheck)
		{
			bool bEnterRoad=false;
			for(list<AStarPathFinder::tagPathPoint>::iterator iter=tilePath.begin();
				iter!=tilePath.end();++iter)
			{
				if(IfCanGo(iter->x,iter->z,true))
				{
					++iter;
					tilePath.erase(iter,tilePath.end());
					bEnterRoad=true;
					break;
				}
			}
			if(bEnterRoad)
			{
				m_pathFinderByTile.SmoothPath(startPos,roleSize,bOnLand,tilePath,path);
				return ER_EnterRoad;
			}
		}

		//--
		m_pathFinderByTile.SmoothPath(startPos,roleSize,bOnLand,tilePath,path);
		if(result==PathFinderByTile::ER_Arrive)
			return ER_Arrive;
		else
			return ER_GetNearPos;
	}

	PlayerNavMap::EResult PlayerNavMap::FindPathByTile( const Vector3& startPos,const Vector3& destPos,const Vector3& roleSize,bool bRoadMap,bool bRoadCheck,bool bHalfPath,bool bOnLand,NavCollider* pCollider,list<Vector3>& path )
	{
		Point destTile=_World2Tile(destPos);
		return FindPathByTile(startPos,destTile,roleSize,bRoadMap,bRoadCheck,bHalfPath,bOnLand,pCollider,path);
	}

	bool PlayerNavMap::IfCanGo( int x,int z,bool bRoadMap )
	{
		if(bRoadMap)
			return m_cangoMap_Road.SafeGetValue(x,z);
		else
			return m_cangoMap_Auto.SafeGetValue(x,z);
	}

	bool PlayerNavMap::IfCanGo( float x,float z,bool bRoadMap )
	{
		return IfCanGo(int(x/g_tileSize),int(z/g_tileSize),bRoadMap);
	}
}//namespace WorldBase
