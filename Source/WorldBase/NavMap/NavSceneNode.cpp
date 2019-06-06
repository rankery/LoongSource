#include "stdafx.h"
#include "NavSceneNode.h"

namespace WorldBase
{
	IMP_RTTI(NavSceneNode,RTTIObj);

	NavSceneNode::NavSceneNode():m_dwFlags(0),m_bEnableCollide(true),m_dwObjID(0)
	{
	}

	NavSceneNode::~NavSceneNode()
	{
	}

	void NavSceneNode::Serialize( FArchive& ar )
	{
		ar.ValueWrite(m_dwFlags);
		ar.ValueWrite(m_dwObjID);
	}

	void NavSceneNode::Deserialize( FArchive& ar )
	{
		ar.ValueRead(m_dwFlags);
		ar.ValueRead(m_dwObjID);
	}

}//namespace WorldBase