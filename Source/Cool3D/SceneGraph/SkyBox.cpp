#include "StdAfx.h"
#include ".\skybox.h"

namespace F3D
{
	SkyBox::SkyBox(void)
	{
	}

	SkyBox::~SkyBox(void)
	{
	}

	bool IsInFrustum(const Frustum *pFrumstum) const
	{
		return true;
	}

	void LoadRes(const char* szName)
	{}

UINT GetNumRenderNode() const
{}
RenderNode *GetRenderNode(UINT i) const
{}
const Matrix4& GetWorldMat() const
{}
void SetWorldMat(const Matrix4 *pMat);
}//namespace F3D