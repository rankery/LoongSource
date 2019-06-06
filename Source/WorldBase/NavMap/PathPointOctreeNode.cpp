#include "StdAfx.h"
#include "PathPointOctreeNode.h"

namespace WorldBase
{
	PathPointOctreeNode::PathPointOctreeNode(void)
	{
		memset(m_pChildren,0,sizeof(m_pChildren));
		m_box=MathConst::SmallBox;
	}

	PathPointOctreeNode::~PathPointOctreeNode(void)
	{
		for(int i=0;i<8;i++)
			SAFE_DELETE(m_pChildren[i]);
	}

	void PathPointOctreeNode::ReadFile(IFS* pFS,DWORD hFile)
	{
		ASSERT(pFS!=NULL);

		FReadValue(pFS,hFile,m_box.max);
		FReadValue(pFS,hFile,m_box.min);
		FReadValVector(pFS,hFile,m_content);

		bool bChild=false;
		for(int i=0;i<8;i++)
		{
			FReadValue(pFS,hFile,bChild);
			if(bChild)
			{
				PathPointOctreeNode *pNode=new PathPointOctreeNode;
				pNode->ReadFile(pFS,hFile);
				m_pChildren[i]=pNode;
			}
		}

	}

	void PathPointOctreeNode::Clone(PathPointOctreeNode* pTarget)
	{
		pTarget->m_box=m_box;
		pTarget->m_content=m_content;

		//--clone children node
		int i;
		for(i=0;i<8;i++)
		{
			ASSERT(pTarget->m_pChildren[i]==NULL);
			if(m_pChildren[i]!=NULL)
			{
				PathPointOctreeNode* pNewNode=new PathPointOctreeNode;
				m_pChildren[i]->Clone(pNewNode);
				pTarget->m_pChildren[i]=pNewNode;
			}
		}
	}

	void PathPointOctreeNode::QueryPathPoints(const AABBox& box,const vector<NPCNavGraph::tagPathPointRT>& points,list<pair<int,float> >& out)
	{
		if(!m_box.Intersects(box))
			return;

		//--
		int i;
		for(i=0;i<(int)m_content.size();i++)
		{
			int index=m_content[i];
			const NPCNavGraph::tagPathPointRT& point=points[index];
			if(m_box.Contain(point.pos))
				out.push_back(make_pair(index,0.0f));
		}

		//--
		for(i=0;i<8;i++)
		{
			if(m_pChildren[i]!=NULL)
				m_pChildren[i]->QueryPathPoints(box,points,out);
		}
	}
};//namespace WorldBase
