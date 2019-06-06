#include "stdafx.h"
#include "NavGenNode.h"

namespace WorldBase
{
	int NavGenNode::maxContent=0;
	int NavGenNode::boxCount=0;
	int NavGenNode::boxTotal=0;
	int NavGenNode::maxLevel=0;
	int NavGenNode::noLeafCount=0;

	NavGenNode::NavGenNode()
	{
		m_id=-1;
		m_lv=0;
		m_box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		m_box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
		memset(m_pChildren,0,sizeof(m_pChildren));
		for(int i=0;i<EChildNum;i++)
			m_childrenID[i]=false;
	}

	NavGenNode::~NavGenNode()
	{
		for(int i=0;i<EChildNum;i++)
		{
			if(m_pChildren[i]!=NULL)
			{
				delete m_pChildren[i];
				m_pChildren[i]=NULL;
			}
		}
	}

	void NavGenNode::WriteFile( FILE *fp )
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
		if((int)m_content.size()>maxContent)
		{
			maxContent=m_content.size();
		}
		if(m_lv>maxLevel)
		{
			maxLevel=m_lv;
		}

		for(UINT i=0;i<EChildNum;i++)
		{
			FWriteValue(fp,m_childrenID[i]);
			if(m_childrenID[i])
				m_pChildren[i]->WriteFile(fp);
		}
	}
}// WorldBase