#include "BintreeNode.h"
#include "FileUtil.h"

namespace Cool3D
{
	int BintreeNode::maxContent=0;
	int BintreeNode::boxCount=0;
	int BintreeNode::boxTotal=0;
	int BintreeNode::maxLevel=0;
	int BintreeNode::noLeafCount=0;

	BintreeNode::BintreeNode()
	{
		m_box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		m_box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
		m_lv=-1;
		for(int i=0;i<8;i++)
		{
			m_pChildren[i]=NULL;
			m_childrenID[i]=false;
		}
	}

	BintreeNode::~BintreeNode()
	{
		for(int i=0;i<8;i++)
		{
			delete m_pChildren[i];
			m_pChildren[i]=NULL;
		}
	}

	void BintreeNode::WriteFile( FILE *fp )
	{
		FWriteValue(fp,m_box.min);
		FWriteValue(fp,m_box.max);

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
		if(m_content.size()>maxContent)
		{
			maxContent=m_content.size();
		}
		if(m_lv>maxLevel)
		{
			maxLevel=m_lv;
		}

		for(int i=0;i<8;i++)
		{
			FWriteValue(fp,m_childrenID[i]);
			if(m_childrenID[i])
				m_pChildren[i]->WriteFile(fp);
		}
	}
}//namespace Cool3D