#include "stdafx.h"
#include "NavOctreeCollider.h"
#include "NavOctreeNode.h"

namespace WorldBase
{
	NavOctreeCollider::NavOctreeCollider()
	{
		m_pRoot=NULL;
	}

	NavOctreeCollider::~NavOctreeCollider()
	{
		SAFE_DELETE(m_pRoot);
	}

	void NavOctreeCollider::ReadFile( IFS *pFS,DWORD hFile )
	{
		ASSERT(pFS!=NULL);

		FReadValVector(pFS,hFile,m_boxes);
		if(m_boxes.size()>0)
			m_pBoxes=&m_boxes[0];
		else
			m_pBoxes=NULL;

		SAFE_DELETE(m_pRoot);
		m_pRoot=new NavOctreeNode;
		m_pRoot->ReadFile(pFS,hFile);
	}

	bool NavOctreeCollider::AABBoxCollideBoxTopHighest( const AABBox& box,float& intersectY )
	{
		if(!m_pRoot->GetBox().Intersects(box))
			return false;
		return m_pRoot->AABBoxCollideBoxTopHighest(box,intersectY,this);
	}

	bool NavOctreeCollider::AABBoxCollideBox( const AABBox& box )
	{
		if(!m_pRoot->GetBox().Intersects(box))
			return false;
		return m_pRoot->AABBoxCollideBox(box,this);
	}

	TResult NavOctreeCollider::RayCollideBox( const Ray& ray )
	{
		return m_pRoot->RayCollideBox(ray,this);
	}

	bool NavOctreeCollider::RayCollideBoxOnce(const Ray& ray)
	{
		return m_pRoot->RayCollideBoxOnce(ray,this);
	}

	const AABBox& NavOctreeCollider::GetBigBox()
	{
		return m_pRoot->GetBox();
	}
}//namespace WorldBase