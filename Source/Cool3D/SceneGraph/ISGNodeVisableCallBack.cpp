#include "StdAfx.h"
#include ".\isgnodevisablecallback.h"
#include <set>
using namespace std;

namespace Cool3D
{
	class SGNodeVisableSet::Member
	{
	public:
		Member()	{}
		~Member()	{}

		set<SceneNode*>	m_visSet;
	};

	SGNodeVisableSet::SGNodeVisableSet()
	{
		m_p=new Member;
	}

	SGNodeVisableSet::~SGNodeVisableSet()
	{
		delete m_p;
	}

	void SGNodeVisableSet::Begin()
	{
		m_p->m_visSet.clear();
	}

	void SGNodeVisableSet::CallBack(SceneNode *pVisNode)
	{
		m_p->m_visSet.insert(pVisNode);
	}

	void SGNodeVisableSet::End()
	{}


	bool SGNodeVisableSet::IsNodeVisable(SceneNode *pNode) const
	{
		return m_p->m_visSet.end() != m_p->m_visSet.find(pNode);
	}
}//namespace Cool3D