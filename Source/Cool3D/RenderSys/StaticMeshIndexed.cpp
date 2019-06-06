#include "StdAfx.h"
#include ".\staticMeshIndexed.h"
#include "..\Util\Exception.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Device\IIndexBuffer.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IStreamDefine.h"
#include "RenderSys.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\RenderMtl\IMaterial.h"

#define VFS_READ
#include "..\Public\SubMesh.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(StaticMeshIndexed,RenderNode);

	StaticMeshIndexed::StaticMeshIndexed(void)
	{
	}

	StaticMeshIndexed::~StaticMeshIndexed(void)
	{
	}

	void StaticMeshIndexed::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)
	{
		IDevice *pDev=Device();
		pDev->SetVB(0,m_pVB.GetMem());

		Device()->SetStreamDefine(pSys->GetStreamDefine(m_vertType));

		IMaterial *pMtl=pSGNode->GetMtl(m_subMesh.GetMtlID());
		m_subMesh.Draw(nLod,pSys,this,pMtl);
	}

	void StaticMeshIndexed::CreateDirect(WORD baseIndex,int mtlID,VertStream *pVertStream,UINT indexBufSize/*in bytes*/,WORD *pIndices)
	{
		//没有创建过
		ASSERT(m_pVB.GetMem()==NULL);
		
		ASSERT(pVertStream!=NULL);

		//--创建vert buffer
		SharedVB pVB(Device()->NewVB());
		pVB->Create(pVertStream->GetBufferSize(),EB_StaticWriteOnly,pVertStream->GetStride());
		pVB->CopyFromVertStream(pVertStream);
		m_vertType=pVertStream->GetType();
		m_pVB=pVB;

		//--创建sub mesh
		m_subMesh.CreateDirect(EDraw_TriangleList,
			baseIndex,mtlID,pVertStream->GetNumVert(),indexBufSize,pIndices);
	}

	void StaticMeshIndexed::Create(SharedVB pVB,EVertType vt,const LodSubMesh* pSubMesh)
	{
		ASSERT(pVB.GetMem()!=NULL);
		ASSERT(pSubMesh!=NULL);

		m_pVB=pVB;
		m_vertType=vt;

		m_subMesh.CreateFromLodSubMesh(pSubMesh);
	}

	EMtlTrait StaticMeshIndexed::GetMtlTrait(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait(m_subMesh.GetMtlID(),pSGNode);
	}

	DWORD StaticMeshIndexed::GetMtlTrait1(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait1(m_subMesh.GetMtlID(),pSGNode);
	}

	DWORD StaticMeshIndexed::GetMtlTrait2(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait2(m_subMesh.GetMtlID(),pSGNode);
	}

	const AABBox& StaticMeshIndexed::GetAABBox() const
	{
		return m_subMesh.GetAABBox();
	}
}//namespace Cool3D