#include "StdAfx.h"
#include "NPCNavMap.h"
#include "NavHeightmap.h"
#include "NavResHeightmap.h"
#include "NavResMgr.h"
#include "OutlinePathFinder.h"
#include "NPCNavMapDoors.h"

namespace WorldBase
{
	const float g_tileSize=50.0f;				//高度图格子大小

	NPCNavMap::NPCNavMap(void)
	{
		m_pCangoMap=NULL;
		m_pHMap=NULL;
		m_pPathFinder=new OutlinePathFinder;
		m_pDoors=NULL;
	}

	NPCNavMap::~NPCNavMap(void)
	{
		Destroy();
		SAFE_DELETE(m_pPathFinder);
	}

	void NPCNavMap::LoadFromFile( const TCHAR* szMapName,IFS* pFS,bool bLoadDoors )
	{
		Destroy();

		//--加载高度图资源，使用阻塞模式
		TCHAR szPath[512];
		_stprintf(szPath,_T("data\\map\\%s\\nav\\terrain.hmap"),szMapName);

		bool bAsync=NavResMgr::Inst()->IsIOAsync();
		NavResMgr::Inst()->EnableIOAsync(false);
		m_pHMap=(NavResHeightmap*)NavResMgr::Inst()->NewRes(szPath);
		NavResMgr::Inst()->EnableIOAsync(bAsync);

		//--加载可行走区域位图
		m_pCangoMap=new BitMap;

		_stprintf(szPath,_T("data\\map\\%s\\nav\\npccango.bmap"),szMapName);
		DWORD hFile=pFS->Open(szPath);
		THROW_NULL(hFile,_T("load nav npc.bmap fail"));

		m_pCangoMap->ReadFile(pFS,hFile);
		pFS->Close(hFile);

		//--加载门数据
		if(bLoadDoors)
		{
			m_pDoors=new NPCNavMapDoors;
			m_pDoors->LoadFromFile(szMapName,pFS);
		}
	}

	void NPCNavMap::Destroy()
	{
		SAFE_DELETE(m_pCangoMap);
		NAV_RES_MGR_RELEASE_RES(m_pHMap);
		SAFE_DELETE(m_pDoors);
	}

	void NPCNavMap::Clone( NPCNavMap* pTarget )
	{
		//--cango map
		ASSERT(pTarget->m_pCangoMap==NULL);
		pTarget->m_pCangoMap=new BitMap;
		m_pCangoMap->Clone(pTarget->m_pCangoMap);

		//--height map
		ASSERT(pTarget->m_pHMap==NULL);
		pTarget->m_pHMap=(NavResHeightmap*)NavResMgr::Inst()->NewRes(m_pHMap->GetName());

		//--doors data
		ASSERT(pTarget->m_pDoors==NULL);
		if(m_pDoors!=NULL)
		{
			pTarget->m_pDoors=new NPCNavMapDoors;
			m_pDoors->Clone(pTarget->m_pDoors);
		}
	}

	float NPCNavMap::SafeGetHeight( float x,float z )
	{
		ASSERT(m_pHMap->IsCreated());
		return m_pHMap->GetHMap()->SafeGetGroundHeight(x,z);
	}

	float NPCNavMap::SafeGetHeightSlerp( float x,float z )
	{
		ASSERT(m_pHMap->IsCreated());
		return m_pHMap->GetHMap()->SafeGetGroundHeightSlerp(x,z);
	}

	bool NPCNavMap::IfCanGo( float x,float z )
	{
		ASSERT(m_pCangoMap!=NULL);
		POINT tile=World2Tile(x,z);
		return IfCanGo(tile.x,tile.y);
	}

	bool NPCNavMap::IfCanGo( int x,int z )
	{
		if(!m_pCangoMap->IfIndex(x,z))
			return false;
		return m_pCangoMap->GetValue(x,z);
	}

	bool NPCNavMap::IfCanDirectGo( float x1,float z1,float x2,float z2,POINT* pNearPos )
	{
		POINT s=World2Tile(x1,z1);
		POINT d=World2Tile(x2,z2);
		return IfCanDirectGo(s.x,s.y,d.x,d.y,pNearPos);
	}

	bool NPCNavMap::IfCanDirectGo( int x1,int z1,int x2,int z2,POINT* pNearPos )
	{
		if(pNearPos)
		{
			pNearPos->x=x1;
			pNearPos->y=z1;
		}

		if(x1==x2 && z1==z2)
		{
			return true;
		}

		INT dx = x2 >= x1 ? x2 - x1 : x1 - x2;
		INT s1 = x2 >= x1 ? 1 : -1;
		INT dy = z2 >= z1 ? z2 - z1 : z1 - z2;
		INT s2 = z2 >= z1 ? 1 : -1;

		INT interchange = dy > dx ? 1 : 0;

		if( dy > dx ) 
			std::swap(dx, dy);

		INT f = dy + dy - dx;

		BYTE by = 0;

		//TRACE("IfCanDirectGoTile:\n");
		for( INT i=1; i<=dx; i++ )
		{
			//TRACE("%d,%d\n",x1,z1);

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

			if( !m_pCangoMap->GetValue(x1,z1) )
				return false;

			if(pNearPos)
			{
				pNearPos->x=x1;
				pNearPos->y=z1;
			}
		}

		//由于画线算法可能会漏掉终点，所以最后再判断一下终点是否可通过
		if(!m_pCangoMap->GetValue(x2,z2))
		{
			return false;
		}

		return true;
	}

	void NPCNavMap::Tile2World( const POINT& tile,float& x,float& z)
	{
		x=(float)tile.x*g_tileSize;
		z=(float)tile.y*g_tileSize;
	}

	POINT NPCNavMap::World2Tile( float x,float z )
	{
		POINT tile;
		tile.x=long(x/g_tileSize);
		tile.y=long(z/g_tileSize);
		return tile;
	}

	bool NPCNavMap::FindPath( float x1,float z1,float x2,float z2,int maxRoundTiles,list<POINT>& path )
	{
		POINT s=World2Tile(x1,z1);
		POINT d=World2Tile(x2,z2);
		return FindPath(s.x,s.y,d.x,d.y,maxRoundTiles,path);
	}

	bool NPCNavMap::FindPath( int x1,int z1,int x2,int z2,int maxRoundTiles,list<POINT>& path )
	{
		ASSERT(m_pCangoMap!=NULL);
		m_pPathFinder->SetCangoMap(m_pCangoMap);
		m_pPathFinder->SetMaxRoundTiles(maxRoundTiles);
		return m_pPathFinder->FindPath(x1,z1,x2,z2,path);
	}

	void NPCNavMap::SmoothPath( float curx,float curz,list<POINT>& path )
	{
		if(path.empty())
			return;

		POINT start=World2Tile(curx,curz);
		list<POINT>::iterator iter=path.end();
		iter--;
		for(;iter!=path.begin();iter--)
		{
			if(IfCanDirectGo(start.x,start.y,iter->x,iter->y))
			{
				path.erase(path.begin(),iter);
				break;
			}
		}
	}

	void NPCNavMap::OpenCloseDoor( DWORD dwObjID,bool bOpen )
	{
		if(m_pDoors==NULL)
			return;

		m_pDoors->OpenCloseDoor(dwObjID,bOpen,m_pCangoMap);
	}

	void NPCNavMap::OpenCloseAllDoors( bool bOpen )
	{
		if(m_pDoors==NULL)
			return;

		m_pDoors->OpenCloseAllDoors(bOpen,m_pCangoMap);
	}
}//namespace WorldBase
