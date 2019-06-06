#pragma once
#include "scenenode.h"

namespace Cool3D
{
	/**	\class SGSkyNode 
		\brief 所有天空的基类
		\remarsk 因为天空节点在Render时需要特殊处理，所以在SG中也使用特殊接口
		\see SceneGraph::SetSkyNode()
	*/
	class Cool3D_API SGSkyNode : public SceneNode
	{
	public:
		SGSkyNode(void);
		virtual ~SGSkyNode(void);

		//--这些SceneNode在这里都没用啦
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		virtual void LoadRes(const TCHAR* szName);
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual void GetBox(AABBox& out) const;

	private:
		DECL_RTTI(SGSkyNode);
	};
}//namespace Cool3D