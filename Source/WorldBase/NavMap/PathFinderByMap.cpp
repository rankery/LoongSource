#include "StdAfx.h"
#include "PathFinderByMap.h"

namespace WorldBase
{
	PathFinderByMap::PathFinderByMap( void )
	{}

	PathFinderByMap::~PathFinderByMap( void )
	{
		Destroy();
	}

	void PathFinderByMap::LoadFromFile( IFS* pSysFS )
	{
		Destroy();
		
		XMLReader xmlReader;
		list<tstring> fieldList;
		if(!xmlReader.Load(pSysFS,_T("data\\system\\pathfinder\\map_link.xml"),"id",&fieldList))
			THROW_NULL(NULL,_T("load map_link.xml fail"));
		
		for(list<tstring>::iterator iter=fieldList.begin();
			iter!=fieldList.end();++iter)
		{
			tstring szMapName=xmlReader.GetString(_T("name"),iter->c_str(),_T(""));
			tstring szNextMap=xmlReader.GetString(_T("linkmap"),iter->c_str(),_T(""));
			Vector3 switchPos;
			switchPos.x=xmlReader.GetFloat(_T("posx"),iter->c_str(),0.0f);
			switchPos.y=xmlReader.GetFloat(_T("posy"),iter->c_str(),0.0f);
			switchPos.z=xmlReader.GetFloat(_T("posz"),iter->c_str(),0.0f);

			tagMapLinkInfo* pMapLinkInfo=FindMapLinkInfo(szMapName.c_str());
			if(pMapLinkInfo==NULL)
			{
				tagMapLinkInfo newInfo;
				newInfo.szMapName=szMapName.c_str();
				m_mapLinkInfo.push_back(newInfo);
				pMapLinkInfo=&m_mapLinkInfo.back();
			}

			pMapLinkInfo->nextMapNames.push_back(szNextMap);
			pMapLinkInfo->nextMapSwitchPos.push_back(switchPos);

			pMapLinkInfo=FindMapLinkInfo(szNextMap.c_str());
			if(pMapLinkInfo==NULL)
			{
				tagMapLinkInfo newInfo;
				newInfo.szMapName=szNextMap.c_str();
				m_mapLinkInfo.push_back(newInfo);
			}
		}

		//--计算nextMapIndices与prevMapIndices
		for(int i=0;i<(int)m_mapLinkInfo.size();++i)
		{
			tagMapLinkInfo& mapLinkInfo=m_mapLinkInfo[i];

			for(int j=0;j<(int)mapLinkInfo.nextMapNames.size();++j)
			{
				tstring& szNextMapName=mapLinkInfo.nextMapNames[j];

				int index=FindMapIndex(szNextMapName.c_str());
				ASSERT(index!=-1);

				mapLinkInfo.nextMapIndices.push_back(index);
				m_mapLinkInfo[index].prevMapIndices.push_back(i);
			}
		}

	}

	void PathFinderByMap::Destroy()
	{
		m_mapLinkInfo.clear();
	}

	bool PathFinderByMap::GetSwitchPos( const TCHAR* szSrcMap,const TCHAR* szDestMap,Vector3& switchPos )
	{
		int srcMapIndex=FindMapIndex(szSrcMap);
		if(srcMapIndex==-1)
			return false;
		int destMapIndex=FindMapIndex(szDestMap);
		if(destMapIndex==-1)
			return false;

		//--同一张地图？
		if(srcMapIndex==destMapIndex)
			return false;

		//--所有map距离设成-1，表示未访问过
		size_t i;
		for(i=0;i<m_mapLinkInfo.size();++i)
			m_mapLinkInfo[i].dist=-1;

		//--计算每个地图离目的地图的最短距离
		deque<int> q;
		m_mapLinkInfo[destMapIndex].dist=0;
		q.push_back(destMapIndex);
		while(!q.empty())
		{
			int topIndex=q.front();
			q.pop_front();

			int dist=m_mapLinkInfo[topIndex].dist;
			for(i=0;i<m_mapLinkInfo[topIndex].prevMapIndices.size();++i)
			{
				int index=m_mapLinkInfo[topIndex].prevMapIndices[i];
				if(m_mapLinkInfo[index].dist==-1
					||dist+1<m_mapLinkInfo[index].dist)
				{
					m_mapLinkInfo[index].dist=dist+1;
					q.push_back(index);
				}
			}
		}

		//--从源地图选一个离目的地图最近的地图，作为下一个切换地图
		bool bFind=false;
		int minDist=-1;
		tagMapLinkInfo& srcMap=m_mapLinkInfo[srcMapIndex];
		for(i=0;i<srcMap.nextMapIndices.size();++i)
		{
			int nextMapIndex=srcMap.nextMapIndices[i];
			if(m_mapLinkInfo[nextMapIndex].dist!=-1)
			{
				if(minDist==-1
					||m_mapLinkInfo[nextMapIndex].dist<minDist)
				{
					minDist=m_mapLinkInfo[nextMapIndex].dist;
					switchPos=srcMap.nextMapSwitchPos[i];
					bFind=true;
				}
			}
		}
		
		return bFind;
	}

	PathFinderByMap::tagMapLinkInfo* PathFinderByMap::FindMapLinkInfo( const TCHAR* szMapName )
	{
		for(size_t i=0;i<m_mapLinkInfo.size();++i)
		{
			if(_tcsicmp(szMapName,m_mapLinkInfo[i].szMapName.c_str())==0)
				return &m_mapLinkInfo[i];
		}
		return NULL;
	}

	int PathFinderByMap::FindMapIndex( const TCHAR* szMapName )
	{
		for(int i=0;i<(int)m_mapLinkInfo.size();++i)
		{
			if(_tcsicmp(szMapName,m_mapLinkInfo[i].szMapName.c_str())==0)
				return i;
		}
		return -1;
	}
}//namespace WorldBase
