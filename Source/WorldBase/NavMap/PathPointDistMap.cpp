#include "StdAfx.h"
#include "PathPointDistMap.h"

namespace WorldBase
{
	PathPointDistMap::PathPointDistMap(void)
		: m_numPoints(0)
	{}

	PathPointDistMap::~PathPointDistMap(void)
	{}

	void PathPointDistMap::Create( int numPoints )
	{
		m_numPoints=numPoints;
		m_data.resize(numPoints*numPoints);
	}

	void PathPointDistMap::Clone( PathPointDistMap* pTarget )
	{
		pTarget->m_numPoints=m_numPoints;
		pTarget->m_data=m_data;
	}

	void PathPointDistMap::WriteFile( FILE* fp )
	{
		FWriteValue(fp,m_numPoints);
		FWriteValVector(fp,m_data);
	}

	void PathPointDistMap::ReadFile( IFS* pFS,DWORD hFile )
	{
		FReadValue(pFS,hFile,m_numPoints);
		FReadValVector(pFS,hFile,m_data);
	}

}//namespace WorldBase
