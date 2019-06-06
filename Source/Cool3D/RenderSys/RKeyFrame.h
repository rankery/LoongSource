#pragma once
#include "rendernode.h"
#include "RenderSubMesh.h"

namespace Cool3D
{
	class LodSubMesh;
	class MKeyFrame;
	struct tagMtlDesc;
	/**	\class RKeyFrame 
		\brief ¹Ø¼üÖ¡Ä£ÐÍäÖÈ¾Æ÷
		\todo bounding box
	*/
	class RKeyFrame :public RenderNode
	{
	public:
		RKeyFrame(void);
		virtual ~RKeyFrame(void);

		void Create(const LodSubMesh* pSubMesh);
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;
		virtual int GetMtlID() const;
	private:
		void SetShader(MKeyFrame* pMtl,SceneNode* pSGNode,const RenderItem* pItem,SceneGraph* pSG);

	private:
		RenderSubMesh	m_subMesh;

		DECL_RTTI_DYNC(RKeyFrame);
	};
}//namespace Cool3D