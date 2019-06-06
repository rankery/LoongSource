#include "StdAfx.h"
#include ".\sglensflare.h"
#include "..\Math\GeomDefine.h"
#include "..\RenderMtl\IMaterial.h"

namespace Cool3D
{
	SGLensFlare::SGLensFlare(void)
	{
	}

	SGLensFlare::~SGLensFlare(void)
	{
	}

	bool SGLensFlare::IsInFrustum(const Frustum *pFrumstum) const
	{
		return true;
	}

	void SGLensFlare::LoadRes(const TCHAR* szName)
	{}

	UINT SGLensFlare::GetNumRenderNode() const
	{
		return 0;
	}

	RenderNode *SGLensFlare::GetRenderNode(UINT i) const
	{
		return NULL;
	}

	const Matrix4& SGLensFlare::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGLensFlare::SetWorldMat(const Matrix4 *pMat)
	{}

	void SGLensFlare::GetBox(AABBox& out) const
	{}

	IMaterial*	SGLensFlare::GetMtl(int mtlID)
	{
		return NULL;
	}

	void SGLensFlare::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{}
}//namespace Cool3D