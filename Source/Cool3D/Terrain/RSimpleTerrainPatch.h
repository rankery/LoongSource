#pragma once
#include "..\rendersys\rendernode.h"
#include "..\Util\fast_vector.h"

namespace Cool3D
{
	class IVertBuffer;
	class IIndexBuffer;
	/**	\class RSimpleTerrainPatch 
		\brief Ô¶´¦µØÐÎ¿éäÖÈ¾Æ÷
	*/
	class RSimpleTerrainPatch :	public RenderNode
	{
	public:
		RSimpleTerrainPatch();
		virtual ~RSimpleTerrainPatch(void);

		//--RenderNode
		virtual bool SupportLod()	{ return true; }
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);	
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;

		//--
		void SetAABBox(AABBox& box){ m_box=box;}
		void SetPatchPos(int patchX,int patchZ);
		void CreateGeom(VertStream& vertStream);

	private:
		AABBox m_box;
		IVertBuffer* m_pVB;
		int m_patchX;
		int m_patchZ;

		DECL_RTTI_DYNC(RSimpleTerrainPatch);
	};
}//namespace Cool3D