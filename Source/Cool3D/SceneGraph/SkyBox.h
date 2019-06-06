#pragma once
#include "scenenode.h"

namespace F3D
{
	class SkyBox :	public SceneNode
	{
	public:
		SkyBox(void);
		virtual ~SkyBox(void);

		//--scene node
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		virtual void LoadRes(const char* szName);
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);

	private:
		class Member;
		Member *m_;
	};
}//namespace F3D