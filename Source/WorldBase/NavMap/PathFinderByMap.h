#pragma once

namespace WorldBase
{
	/** \class PathFinderByMap
		\brief 基于地图的寻路器
	*/
	class PathFinderByMap
	{
		struct tagMapLinkInfo					//地图连接信息
		{
			tstring			szMapName;			//地图名
			vector<tstring>	nextMapNames;		//可切换到达的地图名字列表
			vector<int>		nextMapIndices;		//可切换到达的地图索引列表
			vector<Vector3>	nextMapSwitchPos;	//可切换到达的地图对应的切换点
			vector<int>		prevMapIndices;		//可切换到此地图的地图索引列表
			int				dist;				//与目的地图距离，寻路时使用
		};
	public:
		PathFinderByMap(void);
		~PathFinderByMap(void);

		void LoadFromFile(IFS* pSysFS);
		void Destroy();

		bool GetSwitchPos(const TCHAR* szSrcMap,const TCHAR* szDestMap,Vector3& switchPos);
	
	private:
		tagMapLinkInfo* FindMapLinkInfo(const TCHAR* szMapName);
		int FindMapIndex(const TCHAR* szMapName);

	private:
		vector<tagMapLinkInfo> m_mapLinkInfo;
	};
}//namespace WorldBase
