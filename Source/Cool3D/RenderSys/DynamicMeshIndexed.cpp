#include "StdAfx.h"
#include ".\dynamicmeshindexed.h"
#include "RenderSys.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\RenderMtl\MPreCombineBitmap.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(DynamicMeshIndexed,RenderNode);

	DynamicMeshIndexed::DynamicMeshIndexed(void)
	{
		m_pVertStream=NULL;
	}

	DynamicMeshIndexed::~DynamicMeshIndexed(void)
	{
		SAFE_DELETE(m_pVertStream);
	}

	void DynamicMeshIndexed::Create(const LodSubMesh* pSubMesh)
	{
		m_subMesh.CreateFromLodSubMesh(pSubMesh);
	}

	void DynamicMeshIndexed::CreateDirect(EDrawType drawType,WORD baseIndex,int mtlID,WORD numVert,UINT indexBufSize/*in bytes*/,WORD *pIndices)
	{
		m_subMesh.CreateDirect(drawType,baseIndex,mtlID,numVert,indexBufSize,pIndices);
	}

	bool DynamicMeshIndexed::SupportLod()
	{
		return true;
	}

	EMtlTrait DynamicMeshIndexed::GetMtlTrait(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait(m_subMesh.GetMtlID(),pSGNode);
	}

	DWORD DynamicMeshIndexed::GetMtlTrait1(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait1(m_subMesh.GetMtlID(),pSGNode);
	}

	DWORD DynamicMeshIndexed::GetMtlTrait2(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait2(m_subMesh.GetMtlID(),pSGNode);
	}

	void DynamicMeshIndexed::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)
	{
		IDevice *pDev=Device();
		
		IVertBuffer *pPoolVB=NULL;
		if(m_pVertStream)
		{
			IStreamDefine *pDef=pSys->GetStreamDefine(m_pVertStream->GetType());
			pDev->SetStreamDefine(pDef);

			pPoolVB=pSys->AllocVB(m_pVertStream->GetBufferSize());
			pPoolVB->CopyFromVertStream(m_pVertStream);
			pDev->SetVB(0,pPoolVB);
		}
		else
		{
			ASSERT(0 && "DynamicMeshIndexed unknown scene graph node");
		}

		IMaterial *pMtl=pSGNode->GetMtl(m_subMesh.GetMtlID());
		m_subMesh.Draw(nLod,pSys,this,pMtl);

		if(pPoolVB)
			pSys->FreeVB(pPoolVB);
	}

	const AABBox&  DynamicMeshIndexed::GetAABBox() const
	{
		return m_subMesh.GetAABBox();
	}

	void DynamicMeshIndexed::AttachVertStream(VertStream *pVStream)
	{
		SAFE_DELETE(m_pVertStream);//free old
		m_pVertStream=pVStream;
	}
}//namespace Cool3D