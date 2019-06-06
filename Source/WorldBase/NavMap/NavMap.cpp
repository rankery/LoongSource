#include "StdAfx.h"
#include "NavMap.h"
#include "NavResHeightmap.h"

namespace WorldBase
{
	NavMap::NavMap(void)
	{
		m_loadOptions=0;
	}

	NavMap::~NavMap(void)
	{
	}

	void NavMap::LoadFromFile( const TCHAR* szMapName,IFS* pMapFS,IFS* pSysFS,DWORD loadOptions )
	{
		m_loadOptions=loadOptions;

		if(loadOptions&ELO_Collider)
			m_collider.LoadFromFile(szMapName,pMapFS,(loadOptions&ELO_GroundType)!=0);
		if(loadOptions&ELO_NPCNavMap)
			m_npcNavMap.LoadFromFile(szMapName,pMapFS,(loadOptions&ELO_NPCNavMapDoors)!=0);
		if(loadOptions&ELO_PlayerNavMap)
			m_playerNavMap.LoadFromFile(szMapName,pMapFS,pSysFS);
		if(loadOptions&ELO_NPCNavGraph)
			m_npcNavGraph.LoadFromFile(szMapName,pMapFS);
	}

	void NavMap::Destroy()
	{
		m_collider.Destroy();
		m_npcNavMap.Destroy();
	}

	void NavMap::SetViewZoneSize( const Vector3& size )
	{
		m_collider.SetViewZoneSize(size);
	}

	void NavMap::InvalidViewZone( const Vector3& zoneCenter )
	{
		m_collider.InvalidViewZone(zoneCenter);
	}

	void NavMap::Clone(NavMap* pTarget)
	{
		if(m_loadOptions&ELO_Collider)
			m_collider.Clone(&pTarget->m_collider);
		if(m_loadOptions&ELO_NPCNavMap)
			m_npcNavMap.Clone(&pTarget->m_npcNavMap);
		if(m_loadOptions&ELO_NPCNavGraph)
			m_npcNavGraph.Clone(&pTarget->m_npcNavGraph);
	}


}//namespace WorldBase
