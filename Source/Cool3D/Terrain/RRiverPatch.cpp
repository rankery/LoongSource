#include "stdafx.h"
#include ".\RRiverPatch.h"
#include "..\Device\IDevice.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IIndexBuffer.h"
#include "..\Kernel.h"
#include "..\Math\Transform.h"
#include "..\Math\CameraBase.h"
#include ".\SGRiverPatch.h"
#include ".\ResRiverPatch.h"
#include ".\ResRiver.h"
#include ".\TRiverPatchTriangulationDynamic.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\Device\IPixelShader.h"
#include "..\SceneGraph\SceneGraph.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(RRiverPatch,RenderNode);

	RRiverPatch::RRiverPatch()
	{
		//
	}

	RRiverPatch::~RRiverPatch()
	{
		//
	}

	void RRiverPatch::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod/* =0 */,
		RenderItem* pItem/* =NULL */, SceneGraph* pSG/* =NULL */)
	{
		ASSERT(pSGNode->IS_STATIC_CLASS(SGRiverPatch));

		SGRiverPatch *pPatch=(SGRiverPatch *)pSGNode;
		ResRiverPatch *pPatchRes=pPatch->GetPatchRes();
		ResRiver *pRiverRes=pPatch->GetRiverRes();
		if( pPatchRes==NULL
			|| !pPatchRes->IsCreated()
			|| pRiverRes==NULL
			|| !pRiverRes->IsCreated())
			return;

		IDevice* pDev=Device();
		pDev->SetVB(0,pPatchRes->GetPosTexVB());
		pDev->SetVB(1,pRiverRes->GetNormalAlphaVB());
		nLod=pSG->CalcDetailLevel(*pPatchRes->GetAABBox(),&pSGNode->GetWorldMat());

		int numVerts=0;
		int numFaces=0;
		int lod=0;
		if(nLod<=4)
			lod=0;
		else if(nLod<=6)
			lod=1;
		else
			lod=2;
		IIndexBuffer* pIB=TRiverPatchTriangulationDynamic::Inst()->GetIB_HW(lod,numVerts,numFaces);
		pDev->SetIB(pIB);

		IMaterial *pMtl=pPatch->GetMtl(0);
		if(pMtl==NULL 
			|| !pMtl->IsResourceReady())
			return;

		//--draw
		int numPass=RenderNode::BeginMtl(pSys,pMtl);
		for(int i=0;i<numPass;i++)
		{
			RenderNode::BeginMtlPass(i);

			pDev->DrawIndexed(EDraw_TriangleList,0,numVerts,numFaces);

			RenderNode::EndMtlPass(true);
		}

		RenderNode::EndMtl();
	}

	EMtlTrait RRiverPatch::GetMtlTrait( SceneNode *pSGNode ) const
	{
		return RenderNode::GetMtlTrait(0,pSGNode);
	}

	DWORD RRiverPatch::GetMtlTrait1( SceneNode *pSGNode ) const
	{
		return RenderNode::GetMtlTrait1(0,pSGNode);
	}

	DWORD RRiverPatch::GetMtlTrait2( SceneNode *pSGNode ) const
	{
		return RenderNode::GetMtlTrait2(0,pSGNode);
	}

	const AABBox& RRiverPatch::GetAABBox() const
	{
		return MathConst::SmallBox;
	}

	void RRiverPatch::DrawMaxLodWithOutMtl(RenderSys *pSys,SceneNode *pSGNode,RenderItem* pItem,SceneGraph* pSG)
	{
		ASSERT(pSGNode->IS_STATIC_CLASS(SGRiverPatch));

		SGRiverPatch *pPatch=(SGRiverPatch *)pSGNode;
		ResRiverPatch *pPatchRes=pPatch->GetPatchRes();
		ResRiver *pRiverRes=pPatch->GetRiverRes();
		if( pPatchRes==NULL
			|| !pPatchRes->IsCreated()
			|| pRiverRes==NULL
			|| !pRiverRes->IsCreated())
			return;

		IDevice* pDev=Device();
		pDev->SetVB(0,pPatchRes->GetPosTexVB());
		pDev->SetVB(1,pRiverRes->GetNormalAlphaVB());

		int numVerts=0;
		int numFaces=0;
		IIndexBuffer* pIB=TRiverPatchTriangulationDynamic::Inst()->GetIB_HW(2,numVerts,numFaces);
		pDev->SetIB(pIB);

		pDev->DrawIndexed(EDraw_TriangleList,0,numVerts,numFaces);
	}
}//namespace Cool3D