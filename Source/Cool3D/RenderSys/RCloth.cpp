#include "stdafx.h"
#include "RCloth.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\SceneGraph\SGClothNode.h"
#include "..\Cool3DOption.h"
#include "..\SceneGraph\SceneGraph.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(RCloth,RenderNode);

	RCloth::RCloth(void)
	{
		m_vertType=EVType_None;
		m_subMeshID=0;
	}

	RCloth::~RCloth(void)
	{
	}

	void RCloth::Create(const LodSubMesh* pSubMesh,EVertType vertType,int subMeshID)
	{
		m_vertType=vertType;
		m_subMeshID=subMeshID;
		m_subMesh.CreateFromLodSubMesh(pSubMesh);
	}

	void RCloth::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)
	{
		ASSERT(pSGNode->IS_STATIC_CLASS(SGClothNode));
		SGClothNode *pSGCloth=(SGClothNode*)pSGNode;

		IDevice* pDev=Device();
		if(!pSGCloth->IsCreated())
			return;

		IStreamDefine *pDef=pSys->GetStreamDefine(pSGCloth->GetVertType());
		pDev->SetStreamDefine(pDef);
		pDev->SetVB(0,pSGCloth->GetVB());

		IMaterial *pMtl=pSGNode->GetMtl(m_subMesh.GetMtlID());
		m_subMesh.Draw(pSGCloth->GetLod(),pSys,this,pMtl);
	}

	EMtlTrait RCloth::GetMtlTrait(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait(m_subMesh.GetMtlID(),pSGNode);
	}

	DWORD RCloth::GetMtlTrait1(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait1(m_subMesh.GetMtlID(),pSGNode);
	}

	DWORD RCloth::GetMtlTrait2(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait2(m_subMesh.GetMtlID(),pSGNode);
	}

	const AABBox& RCloth::GetAABBox() const
	{
		return m_subMesh.GetAABBox();
	}
}//namespace Cool3D