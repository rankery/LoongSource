#pragma once
#include "rendernode.h"

namespace Cool3D
{
	class StaticMeshIndexed;
	class LodSubMesh;
	class MStatic;
	/**	\class RStatic 
		\brief ¾²Ì¬Ä£ÐÍäÖÈ¾Æ÷
	*/
	class RStatic :public RenderNode
	{
	public:
		RStatic(void);
		virtual ~RStatic(void);

		//--RenderNode
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;
		virtual int GetMtlID() const;

		//--
		void Create(SharedVB pVB,EVertType vt,const LodSubMesh* pSubMesh);
	private:
		void SetShader(MStatic* pMtl,SceneNode* pSGNode,const RenderItem* pItem,SceneGraph* pSG);

	private:
		StaticMeshIndexed*	m_pMesh;

		DECL_RTTI_DYNC(RStatic);
	};
}//namespace Cool3D