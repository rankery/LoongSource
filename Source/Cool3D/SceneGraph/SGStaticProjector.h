#pragma once
#include "sceneprojector.h"

namespace Cool3D
{
	class VertStream;
	/**	\class SGStaticProjector
		\brief 静态projector,其texture和vert buffer都不改变
	*/
	class Cool3D_API SGStaticProjector : public SceneProjector
	{
	public:
		SGStaticProjector(void);
		virtual ~SGStaticProjector(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--scene node
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		/**	没有实现*/
		virtual void LoadRes(const TCHAR* szName);
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual IMaterial*	GetMtl(int mtlID);
		virtual void GetBox(AABBox& out) const;

		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);

		//--
		void Create(VertStream *pVertStream,const TCHAR *szTexPath);
	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(SGStaticProjector);
	};
}//namespace Cool3D