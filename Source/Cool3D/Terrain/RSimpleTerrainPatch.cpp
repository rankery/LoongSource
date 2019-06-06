#include "StdAfx.h"
#include ".\RSimpleTerrainPatch.h"
#include "..\Device\IDevice.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IIndexBuffer.h"
#include "..\Kernel.h"
#include ".\TPatchLodTable.h"
#include ".\TPatchTriangulationDynamic.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(RSimpleTerrainPatch,RenderNode);

	RSimpleTerrainPatch::RSimpleTerrainPatch()
	{
		m_pVB=NULL;
	}

	RSimpleTerrainPatch::~RSimpleTerrainPatch(void)
	{
		SAFE_DELETE(m_pVB);
	}

	void RSimpleTerrainPatch::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)	
	{
		IDevice* pDev=Device();

		int myLod		=TPatchLodTable::Inst_SimpleTerrain()->SafeGetLod(m_patchX,m_patchZ);
		int leftLod		=TPatchLodTable::Inst_SimpleTerrain()->SafeGetLod(m_patchX-1,m_patchZ);
		int rightLod	=TPatchLodTable::Inst_SimpleTerrain()->SafeGetLod(m_patchX+1,m_patchZ);
		int topLod		=TPatchLodTable::Inst_SimpleTerrain()->SafeGetLod(m_patchX,m_patchZ-1);
		int bottomLod	=TPatchLodTable::Inst_SimpleTerrain()->SafeGetLod(m_patchX,m_patchZ+1);

		int numVerts,numFaces;
		IIndexBuffer* pIB=TPatchTriangulationDynamic::Inst()->GetIB_HW(myLod,leftLod,topLod,rightLod,bottomLod,numVerts,numFaces);
		
		pDev->SetVB(0,m_pVB);
		pDev->SetIB(pIB);
		pDev->DrawIndexed(EDraw_TriangleList,0,numVerts,numFaces);
	}

	EMtlTrait RSimpleTerrainPatch::GetMtlTrait(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait(0,pSGNode);
	}

	DWORD RSimpleTerrainPatch::GetMtlTrait1(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait1(0,pSGNode);
	}

	DWORD RSimpleTerrainPatch::GetMtlTrait2(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait2(0,pSGNode);
	}

	const AABBox& RSimpleTerrainPatch::GetAABBox() const
	{
		return m_box;
	}

	void RSimpleTerrainPatch::CreateGeom(VertStream& vertStream)
	{
		ASSERT(m_pVB==NULL);//·ÀÖ¹ÖØ¸´´´½¨
		m_pVB=Device()->NewVB();
		m_pVB->Create(vertStream.GetBufferSize(),EB_StaticWriteOnly,0);
		m_pVB->CopyFromVertStream(&vertStream);
	}

	void RSimpleTerrainPatch::SetPatchPos(int patchX,int patchZ)
	{
		m_patchX=patchX;
		m_patchZ=patchZ;
	}
	
}//namespace Cool3D