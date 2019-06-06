#pragma once
#include "..\WorldBase.h"

namespace WorldBase
{
	class NavResHeightmap;
	class OutlinePathFinder;
	class NPCNavMapDoors;
	/** \class NPCNavMap
		\brief NPC格子导航图
	*/
	class WORLD_BASE_API NPCNavMap
	{
	public:
		NPCNavMap(void);
		~NPCNavMap(void);

		/** 加载数据
		*/
		void LoadFromFile(const TCHAR* szMapName,IFS* pFS,bool bLoadDoors=false);
		/** 释放数据
		*/
		void Destroy();
		/** 克隆对象
		*/
		void Clone(NPCNavMap* pTarget);

		/** 获取指定格子的高度，xz坐标会自动截断
		*/
		float SafeGetHeight(float x,float z);
		/** 获取指定格子的高度，会与相邻格子高度插值，xz坐标会自动截断
		*/
		float SafeGetHeightSlerp(float x,float z);

		/** 判断指定格子是否可通过
		*/
		bool IfCanGo(float x,float z);
		bool IfCanGo(int x,int z);

		/** 使用画线算法判断是否可直线到达
			\return 到达一个最近点或终点会返回true
		*/
		bool IfCanDirectGo(float x1,float z1,float x2,float z2,POINT* pNearPos=NULL);
		bool IfCanDirectGo(int x1,int z1,int x2,int z2,POINT* pNearPos=NULL);

		/** 使用“边缘跟踪算法”基于格子寻路
		*/
		bool FindPath(float x1,float z1,float x2,float z2,int maxRoundTiles,list<POINT>& path);
		bool FindPath(int x1,int z1,int x2,int z2,int maxRoundTiles,list<POINT>& path);

		/** 平滑路径
		*/
		void SmoothPath(float curx,float curz,list<POINT>& path);
		
		inline void Tile2World(const POINT& tile,float& x,float& z);
		inline POINT World2Tile(float x,float z);
		
		BitMap* GetCanGoMap()	{ return m_pCangoMap;}

		/** 打开/关闭指定的门
		*/
		void OpenCloseDoor(DWORD dwObjID,bool bOpen);
		/** 打开/关闭所有门
		*/
		void OpenCloseAllDoors(bool bOpen);

	private:
		BitMap*				m_pCangoMap;
		NavResHeightmap*	m_pHMap;	
		OutlinePathFinder*	m_pPathFinder;
		NPCNavMapDoors*		m_pDoors;
	};

}//namespace WorldBase
