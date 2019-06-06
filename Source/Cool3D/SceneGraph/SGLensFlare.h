#pragma once
#include "scenenode.h"

namespace Cool3D
{
	class SGLensFlare :	public SceneNode
	{
	public:
		SGLensFlare(void);
		virtual ~SGLensFlare(void);

		//-- SceneNode
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		virtual void LoadRes(const TCHAR* szName);
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual void GetBox(AABBox& out) const;
		virtual IMaterial*	GetMtl(int mtlID);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);

	};
}//namespace Cool3D