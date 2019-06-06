#include "stdafx.h"
#include "NavBintreeCollider.h"
#include "NavBintreeNode.h"

namespace WorldBase
{
	NavBintreeCollider::NavBintreeCollider()
		: m_pRoot(0)
	{
	}

	NavBintreeCollider::~NavBintreeCollider()
	{
		SAFE_DELETE(m_pRoot);
	}

	void NavBintreeCollider::ReadFile( IFS *pFS,DWORD hFile )
	{
		ASSERT(pFS!=NULL);

		FReadValVector(pFS,hFile,m_boxes);
		if(m_boxes.size()>0)
			m_pBoxes=&m_boxes[0];
		else
			m_pBoxes=NULL;

		SAFE_DELETE(m_pRoot);
		m_pRoot=new NavBintreeNode;
		m_pRoot->ReadFile(pFS,hFile);
	}

	const AABBox& NavBintreeCollider::GetBigBox()
	{
		return m_pRoot->GetBox();
	}

	bool NavBintreeCollider::AABBoxCollideBox( const AABBox& box )
	{
		if(!m_pRoot->GetBox().Intersects(box))
			return false;
		return m_pRoot->AABBoxCollideBox(box,this);
	}

	bool NavBintreeCollider::AABBoxCollideBoxTopHighest( const AABBox& box,float& intersectY )
	{
		if(!m_pRoot->GetBox().Intersects(box))
			return false;
		return m_pRoot->AABBoxCollideBoxTopHighest(box,intersectY,this);
	}

	Cool3D::TResult NavBintreeCollider::RayCollideBox( const Ray& ray )
	{
		return m_pRoot->RayCollideBox(ray,this);
	}

	bool NavBintreeCollider::RayCollideBoxOnce( const Ray& ray )
	{
		return m_pRoot->RayCollideBoxOnce(ray,this);
	}
}//namespace WorldBase