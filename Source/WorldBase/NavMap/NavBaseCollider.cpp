#include "stdafx.h"
#include "NavBaseCollider.h"

namespace WorldBase
{
	NavBaseCollider::NavBaseCollider()
		: m_pBoxes(0)
	{

	}

	NavBaseCollider::~NavBaseCollider()
	{
		m_pBoxes = NULL;
	}

	void NavBaseCollider::DrawBox()
	{
		for(size_t i=0;i<m_boxes.size();i++)
		{
			DrawX::DrawAABBox(m_boxes[i]);
		}
	}
}//namespace WorldBase