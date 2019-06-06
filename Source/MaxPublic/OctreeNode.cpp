#include "OctreeNode.h"
#include "FileUtil.h"

namespace Cool3D
{
	int OctreeNode::maxContent=0;
	int OctreeNode::boxCount=0;
	int OctreeNode::boxTotal=0;
	int OctreeNode::maxLevel=0;
	int OctreeNode::noLeafCount=0;
	OctreeNode::OctreeNode()
	{
		m_id=-1;
		m_lv=0;
		m_box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		m_box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
		memset(m_pChildren,0,sizeof(m_pChildren));
		for(int i=0;i<2;i++)
			m_childrenID[i]=false;
	}

	OctreeNode::~OctreeNode()
	{
		for(int i=0;i<2;i++)
		{
			if(m_pChildren[i]!=NULL)
			{
				delete m_pChildren[i];
				m_pChildren[i]=NULL;
			}
		}
	}

	void OctreeNode::WriteFile( FILE *fp )
	{
		FWriteValue(fp,m_box.max);
		FWriteValue(fp,m_box.min);
		FWriteValVector(fp,m_content);

		if(m_content.size()>0)
		{
			boxCount++;
			boxTotal+=m_content.size();
		}
		else
		{
			noLeafCount++;
		}
		if(m_content.size()>(UINT)maxContent)
		{
			maxContent=m_content.size();
		}
		if(m_lv>maxLevel)
		{
			maxLevel=m_lv;
		}

		for(UINT i=0;i<2;i++)
		{
			FWriteValue(fp,m_childrenID[i]);
			if(m_childrenID[i])
				m_pChildren[i]->WriteFile(fp);
		}
	}
}//namespace Cool3D