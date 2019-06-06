#pragma once

namespace WorldBase
{
	class NavEntityNode;
	/** \class NavColliderDataGen
		\brief 碰撞检测数据生成器
	*/
	class NavColliderDataGen
	{
		enum EConst
		{
			ETerrainPatchSize=64,			//地形块有多少*多少个格子
		};
	public:
		NavColliderDataGen(void);
		~NavColliderDataGen(void);

		void BeginBuild(Heightmap* pHMap,BitMap* pManualCangoMap,Heightmap* pGroundType);
		bool EndBuild(const TCHAR* szSavePath,const TCHAR* szMapName);

	private:
		void LoadEntityNodes(const TCHAR* szSavePath,const TCHAR* szMapName,vector<NavEntityNode*>& out);
		bool BuildQuadTreeSG(const TCHAR* szSavePath,const TCHAR* szMapName);
		bool BuildModelMap(const TCHAR* szSavePath,const TCHAR* szMapName);
		bool BuildCanGoMap(const TCHAR* szSavePath);
		bool BuildGroundTypeMap(const TCHAR* szSavePath,const TCHAR* szMapName);

	private:
		Heightmap*		m_pHMap;
		BitMap*			m_pManualCangoMap;
		Heightmap*		m_pGroundType;
	};

}//namespace WorldBase
