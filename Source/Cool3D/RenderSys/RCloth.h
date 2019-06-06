#pragma once
#include "rendernode.h"
#include "RenderSubMesh.h"
#include "..\ResSys\ResSkin.h"

namespace Cool3D
{
	class MCloth;
	/**	\class RCloth 
		\brief ≤º¡œ‰÷»æ∆˜
	*/
	class RCloth :public RenderNode
	{
	public:
		RCloth(void);
		virtual ~RCloth(void);

		//--RenderNode
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;

		//--
		void Create(const LodSubMesh* pSubMesh,EVertType vertType,int subMeshID);
		int  GetSubMeshID() { return m_subMeshID; }

	private:
		int				m_subMeshID;
		RenderSubMesh	m_subMesh;
		EVertType		m_vertType;

		DECL_RTTI_DYNC(RCloth);
	};
}//namespace Cool3D