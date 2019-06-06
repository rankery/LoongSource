#include "StdAfx.h"
#include ".\QuadTreeNode.h"
#include "..\Util\DebugMisc.h"
#include "..\Math\Frustum.h"

namespace F3D
{
	QuadTreeNode::QuadTreeNode(QuadTreeNode *pFather):m_pFather(pFather)
	{
	}

	QuadTreeNode::~QuadTreeNode(void)
	{
	}

	QuadTreeNode *QuadTreeNode::GetChild(UINT i) const
	{
		ASSERT(i<4);
		return m_pChildren[i];
	}

	bool QuadTreeNode::IsInFrustum(const Frustum *pFrumstum) const
	{
		return pFrumstum->BoxInFrustum(m_nodeBox);
	}
}//namespace F3D