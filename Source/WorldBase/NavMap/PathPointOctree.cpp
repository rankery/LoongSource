#include "StdAfx.h"
#include "PathPointOctree.h"
#include "PathPointOctreeNode.h"

namespace WorldBase
{
	PathPointOctree::PathPointOctree(void)
	{
		m_pRoot=NULL;
	}

	PathPointOctree::~PathPointOctree(void)
	{
		SAFE_DELETE(m_pRoot);
	}

	bool PathPointOctree::LoadFromFile(const TCHAR* szMapName,IFS* pFS)
	{
		TCHAR szPath[512];
		_stprintf(szPath,_T("data\\map\\%s\\nav\\npcpathpoints.oct"),szMapName);

		DWORD hFile=pFS->Open(szPath);
		if(NULL==hFile)
		{
			LOG(_T("load %s failed.\n"),szPath);
			return false;
		}
		
		SAFE_DELETE(m_pRoot);
		m_pRoot=new PathPointOctreeNode;
		m_pRoot->ReadFile(pFS,hFile);
		pFS->Close(hFile);

		return true;
	}

	void PathPointOctree::Clone(PathPointOctree* pTarget)
	{
		ASSERT(pTarget->m_pRoot==NULL);
		ASSERT(m_pRoot!=NULL);
		pTarget->m_pRoot=new PathPointOctreeNode;
		m_pRoot->Clone(pTarget->m_pRoot);
	}

	void PathPointOctree::QueryPathPoints(const AABBox& box,const vector<NPCNavGraph::tagPathPointRT>& points,list<pair<int,float> >& out)
	{
		ASSERT(m_pRoot!=NULL);
		m_pRoot->QueryPathPoints(box,points,out);
	}

};//namespace WorldBase