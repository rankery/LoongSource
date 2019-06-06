#include "StdAfx.h"
#include ".\sgskynode.h"

namespace Cool3D
{
	IMP_RTTI(SGSkyNode,SceneNode);

	SGSkyNode::SGSkyNode(void)
	{
	}

	SGSkyNode::~SGSkyNode(void)
	{
	}

	bool SGSkyNode::IsInFrustum(const Frustum *pFrumstum) const
	{
		return true;
	}

	void SGSkyNode::LoadRes(const TCHAR* szName)
	{
		ASSERT(0 && _T("sky node have no resource"));
	}

	const Matrix4& SGSkyNode::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGSkyNode::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT(0 && _T("SGSkyNode cant set world mat"));
	}

	void SGSkyNode::GetBox(AABBox& out) const
	{}

}//namespace Cool3D