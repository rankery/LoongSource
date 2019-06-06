#pragma once
#include "..\RenderSys\RenderNode.h"

namespace Cool3D
{
	class CameraBase;
	class VertStream;

	/**	\class RRiverPatch
		\brief ºÓÁ÷¿éäÖÈ¾Æ÷
	*/
	class RRiverPatch : public RenderNode
	{
	public:
		RRiverPatch();
		virtual ~RRiverPatch();

		//--RenderNode
		virtual bool SupportLod()	{ return true; }
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,
			SceneGraph* pSG=NULL);	
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;

		void DrawMaxLodWithOutMtl(RenderSys *pSys,SceneNode *pSGNode,RenderItem* pItem,SceneGraph* pSG);
	private:

		DECL_RTTI_DYNC(RRiverPatch);
	};
}//namespace Cool3D