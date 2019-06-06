#pragma once
#include "..\rendersys\rendernode.h"

namespace Cool3D
{
	class SGSimpleTerrain;
	/**	\class RSimpleTerrain 
		\brief ‘∂¥¶µÿ–Œ‰÷»æ∆˜
	*/
	class RSimpleTerrain :	public RenderNode
	{
	public:
		RSimpleTerrain();
		virtual ~RSimpleTerrain(void);

		//--RenderNode
		virtual bool SupportLod()	{ return true; }
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);	
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;

	private:
		void BuildRenderItems(SGSimpleTerrain* pSimpleTerrain,vector<RenderNode*>& renderNodes);

		DECL_RTTI_DYNC(RSimpleTerrain);
	};
}//namespace Cool3D