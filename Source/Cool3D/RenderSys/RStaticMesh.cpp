#include "StdAfx.h"
#include ".\rstaticmesh.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\StateBlock.h"
#include "..\Kernel.h"
#include "RenderSys.h"
#include "..\SceneGraph\SceneNode.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(RStaticMesh,RenderNode);

	RStaticMesh::RStaticMesh(void)
	{
		m_pVB=NULL;
		m_drawType=EDraw_TriangleList;
		m_mtlID=-1;
		m_numPrimitive=0;
	}

	RStaticMesh::~RStaticMesh(void)
	{
		Destroy();
	}

	bool RStaticMesh::SupportLod()
	{
		return false;
	}

	void RStaticMesh::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)
	{
		if(m_pVB==NULL)
			return;

		IDevice *pDev=Device();
		//--vert stream define
		IStreamDefine *pDef=pSys->GetStreamDefine(m_vertType);
		pDev->SetStreamDefine(pDef);
		pDev->SetVB(0,m_pVB);
	
		//--mtl & draw
		int numPass=RenderNode::BeginMtl(pSys,pSGNode->GetMtl(m_mtlID));
		for(int i=0;i<numPass;i++)
		{
			RenderNode::BeginMtlPass(i);
			pDev->Draw(m_drawType,m_numPrimitive);
			RenderNode::EndMtlPass();
		}
		RenderNode::EndMtl();
	}

	EMtlTrait RStaticMesh::GetMtlTrait(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait(m_mtlID,pSGNode);
	}

	DWORD RStaticMesh::GetMtlTrait1(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait1(m_mtlID,pSGNode);
	}

	DWORD RStaticMesh::GetMtlTrait2(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait2(m_mtlID,pSGNode);
	}

	const AABBox& RStaticMesh::GetAABBox() const
	{
		return m_box;
	}

	extern int CalNumPrimitiveByNumVert(EDrawType drawType,int numVert);
	void RStaticMesh::Create(int mtlID,EDrawType drawType,VertStream *pVertStream)
	{
		m_mtlID=mtlID;
		m_drawType=drawType;
		m_numPrimitive=CalNumPrimitiveByNumVert(drawType,pVertStream->GetNumVert());

		ASSERT(m_pVB==NULL);
		m_pVB=Device()->NewVB();
		m_pVB->Create(pVertStream->GetBufferSize(),EB_StaticWriteOnly,pVertStream->GetStride());
		m_pVB->CopyFromVertStream(pVertStream);

		m_vertType=pVertStream->GetType();
	}

	void RStaticMesh::Destroy()
	{
		SAFE_DELETE(m_pVB);
	}
}//namespace Cool3D