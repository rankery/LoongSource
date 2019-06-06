#include "StdAfx.h"
#include ".\terrainpatchrender.h"
#include ".\TerrainPatch.h"
#include ".\TerrainRes.h"
#include ".\TerrainPatchRes.h"
#include ".\TerrainPatchRenderGroupsRes.h"
#include "..\Device\IDevice.h"
#include "..\Kernel.h"
#include "..\EditorSupport\TerrainEditor.h"
#include ".\TPatchTriangulationDynamic.h"
#include ".\TPatchLodTable.h"

namespace Cool3D
{
	TerrainPatchRender::ERenderMode	TerrainPatchRender::m_renderMode=ERM_PreCombine;

	IMP_RTTI_DYNC(TerrainPatchRender,RenderNode);

	TerrainPatchRender::TerrainPatchRender()
	{
		m_pTerrainPatchRes=NULL;
	}

	TerrainPatchRender::~TerrainPatchRender(void)
	{
	}

	bool TerrainPatchRender::RenderMultiLayers(RenderSys *pSys,TerrainPatch *pSGNode)
	{
		TerrainPatchRenderGroupsRes* pRenderGroupsRes=pSGNode->GetTerrainPatchRenderGroupsRes();
		if(pRenderGroupsRes==NULL
			||!pRenderGroupsRes->IsCreated())
			return false;

		TerrainRes* pTerrainRes=pSGNode->GetTerrainRes();
		if(pTerrainRes==NULL
			||!pTerrainRes->IsCreated())
			return false;

		IDevice *pDev=Device();

		//vb
		pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_Trn4));
		pDev->SetVB(0,m_pTerrainPatchRes->GetPosNormalVB());
		pDev->SetVB(1,pTerrainRes->GetTexCoord4VB());

		//ib
		pDev->SetIB(pRenderGroupsRes->GetIB());

		//draw
		int numDrawGroup=0;
		for(int i=0;i<pRenderGroupsRes->GetNumRenderGroups();i++)
		{
			const TerrainPatchRenderGroupsRes::tagRenderGroup& renderGroup=pRenderGroupsRes->GetRenderGroup(i);

			IMaterial* pMtl=pSGNode->GetRenderGroupMtl(i);
			int numPass=RenderNode::BeginMtl(pSys,pMtl);
			for(int j=0;j<numPass;j++)
			{
				RenderNode::BeginMtlPass(j);
				pDev->DrawIndexed(EDraw_TriangleList,0,TerrainEditor::EPatchVerts,renderGroup.numFace,renderGroup.startIndex);
				RenderNode::EndMtlPass(false);
			}
			RenderNode::EndMtl();

			numDrawGroup++;
		}

		//ScreenPrinter::Inst()->Print("group:%d/%d",numDrawGroup,m_renderGroups.size());
		return true;
	}

	bool TerrainPatchRender::RenderTwoLayers(RenderSys *pSys,TerrainPatch *pSGNode)
	{
		TerrainRes* pTerrainRes=pSGNode->GetTerrainRes();
		if(pTerrainRes==NULL
			||!pTerrainRes->IsCreated())
			return false;

		//计算周围地块LOD
		int myLod,leftLod,topLod,rightLod,bottomLod;
		GetAroundLod(myLod,leftLod,topLod,rightLod,bottomLod);

		IDevice *pDev=Device();
		int numVerts=0;
		int numFaces=0;

		//vb
		pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_Trn2));
		pDev->SetVB(0,m_pTerrainPatchRes->GetPosNormalVB());
		pDev->SetVB(1,pTerrainRes->GetTexCoord2VB());

		//ib
		IIndexBuffer* pIB=TPatchTriangulationDynamic::Inst()->GetIB_HW(
			myLod,leftLod,topLod,rightLod,bottomLod,numVerts,numFaces);
		pDev->SetIB(pIB);

		//draw
		IMaterial *pMtl=pSGNode->GetMtl(TerrainPatch::EMType_TwoLayers);
		int numPass=RenderNode::BeginMtl(pSys,pMtl);
		for(int i=0;i<numPass;i++)
		{
			RenderNode::BeginMtlPass(i);
			pDev->DrawIndexed(EDraw_TriangleList,0,numVerts,numFaces);
			RenderNode::EndMtlPass();
		}
		RenderNode::EndMtl();

		return true;
	}

	bool TerrainPatchRender::RenderPreCombine(RenderSys *pSys,TerrainPatch *pSGNode)
	{
		TerrainRes* pTerrainRes=pSGNode->GetTerrainRes();
		if(pTerrainRes==NULL
			||!pTerrainRes->IsCreated())
			return false;

		//计算周围地块LOD
		int myLod,leftLod,topLod,rightLod,bottomLod;
		GetAroundLod(myLod,leftLod,topLod,rightLod,bottomLod);

		IDevice *pDev=Device();
		int numVerts=0;
		int numFaces=0;

		//vb
		pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_Trn2));
		pDev->SetVB(0,m_pTerrainPatchRes->GetPosNormalVB());
		pDev->SetVB(1,pTerrainRes->GetTexCoord2VB());

		//ib
		IIndexBuffer* pIB=TPatchTriangulationDynamic::Inst()->GetIB_HW(
			myLod,leftLod,topLod,rightLod,bottomLod,numVerts,numFaces);
		pDev->SetIB(pIB);

		//draw
		IMaterial *pMtl=pSGNode->GetMtl(TerrainPatch::EMType_PreCombine);
		int numPass=RenderNode::BeginMtl(pSys,pMtl);
		for(int i=0;i<numPass;i++)
		{
			RenderNode::BeginMtlPass(i);
			pDev->DrawIndexed(EDraw_TriangleList,0,numVerts,numFaces);
			RenderNode::EndMtlPass();
		}
		RenderNode::EndMtl();

		return true;
	}

	bool TerrainPatchRender::RenderEditor(RenderSys *pSys,TerrainPatch *pSGNode)
	{
		TerrainRes* pTerrainRes=pSGNode->GetTerrainRes();
		if(pTerrainRes==NULL
			||!pTerrainRes->IsCreated())
			return false;

		//计算周围地块LOD
		IDevice *pDev=Device();
		int numVerts=0;
		int numFaces=0;

		//vb
		pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_Trn2));
		pDev->SetVB(0,m_pTerrainPatchRes->GetPosNormalVB());
		pDev->SetVB(1,pTerrainRes->GetTexCoord2VB());

		//ib
		IIndexBuffer* pIB=TPatchTriangulationDynamic::Inst()->GetIB_HW(
			0,0,0,0,0,numVerts,numFaces);
		pDev->SetIB(pIB);

		//draw
		IMaterial *pMtl=pSGNode->GetMtl(TerrainPatch::EMType_Editor);
		int numPass=RenderNode::BeginMtl(pSys,pMtl);
		for(int i=0;i<numPass;i++)
		{
			RenderNode::BeginMtlPass(i);
			pDev->DrawIndexed(EDraw_TriangleList,0,numVerts,numFaces);
			RenderNode::EndMtlPass();
		}
		RenderNode::EndMtl();

		return true;
	}

	bool TerrainPatchRender::RenderReflectMap(RenderSys *pSys,TerrainPatch *pSGNode)
	{
		TerrainRes* pTerrainRes=pSGNode->GetTerrainRes();
		if(pTerrainRes==NULL
			||!pTerrainRes->IsCreated())
			return false;

		//计算周围地块LOD
		int myLod,leftLod,topLod,rightLod,bottomLod;
		GetAroundLod(myLod,leftLod,topLod,rightLod,bottomLod);

		IDevice *pDev=Device();
		int numVerts=0;
		int numFaces=0;

		//vb
		pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_Trn2));
		pDev->SetVB(0,m_pTerrainPatchRes->GetPosNormalVB());
		pDev->SetVB(1,pTerrainRes->GetTexCoord2VB());

		//ib
		IIndexBuffer* pIB=TPatchTriangulationDynamic::Inst()->GetIB_HW(
			myLod,leftLod,topLod,rightLod,bottomLod,numVerts,numFaces);
		pDev->SetIB(pIB);

		//draw
		IMaterial *pMtl=pSGNode->GetMtl(TerrainPatch::EMType_PreCombine);
		int numPass=RenderNode::BeginMtl(pSys,pMtl);
		for(int i=0;i<numPass;i++)
		{
			RenderNode::BeginMtlPass(i);
			pDev->DrawIndexed(EDraw_TriangleList,0,numVerts,numFaces);
			RenderNode::EndMtlPass();
		}
		RenderNode::EndMtl();

		return true;
	}

	bool TerrainPatchRender::RenderWithOutMtl(RenderSys *pSys,TerrainPatch *pSGNode)
	{
		TerrainRes* pTerrainRes=pSGNode->GetTerrainRes();
		if(pTerrainRes==NULL || !pTerrainRes->IsCreated() )
			return false;

		//计算周围地块LOD
		int myLod,leftLod,topLod,rightLod,bottomLod;
		GetAroundLod(myLod,leftLod,topLod,rightLod,bottomLod);

		IDevice *pDev=Device();
		int numVerts=0;
		int numFaces=0;

		pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_PN));
		pDev->SetVB(0,m_pTerrainPatchRes->GetPosNormalVB());

		IIndexBuffer* pIB=TPatchTriangulationDynamic::Inst()->GetIB_HW(myLod,leftLod,topLod,rightLod,bottomLod,numVerts,numFaces);
		pDev->SetIB(pIB);

		pDev->DrawIndexed(EDraw_TriangleList,0,numVerts,numFaces);

		return true;
	}

	void TerrainPatchRender::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)
	{
		ASSERT(pSGNode->IS_STATIC_CLASS(TerrainPatch));
		TerrainPatch* pPatch=(TerrainPatch*)pSGNode;

		if(m_pTerrainPatchRes==NULL
			||!m_pTerrainPatchRes->IsCreated())
			return;

		switch(m_renderMode)
		{
		case ERM_MultiLayers:
			{
				if(!RenderMultiLayers(pSys,pPatch))
					RenderPreCombine(pSys,pPatch);
			}
			break;
		case ERM_ReflectMap:
			RenderReflectMap(pSys,pPatch);
			break;
		case ERM_PreCombine:
			RenderPreCombine(pSys,pPatch);
			break;
		case ERM_Editor:
			RenderEditor(pSys,pPatch);
			break;
		case ERM_TwoLayers:
			RenderTwoLayers(pSys,pPatch);
			break;
		case ERM_WithOutMtl:
			RenderWithOutMtl(pSys,pPatch);
			break;
		default:
			ASSERT(0);
		}
	}

	void TerrainPatchRender::GetAroundLod(int& myLod,int& leftLod,int& topLod,int& rightLod,int& bottomLod)
	{
		int x=m_pTerrainPatchRes->GetPatchX();
		int z=m_pTerrainPatchRes->GetPatchZ();
		myLod		= TPatchLodTable::Inst()->SafeGetLod(x,		z);
		leftLod		= TPatchLodTable::Inst()->SafeGetLod(x-1,	z);
		rightLod	= TPatchLodTable::Inst()->SafeGetLod(x+1,	z);
		topLod		= TPatchLodTable::Inst()->SafeGetLod(x,		z-1);
		bottomLod	= TPatchLodTable::Inst()->SafeGetLod(x,		z+1);
	}

	EMtlTrait TerrainPatchRender::GetMtlTrait(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait(0,pSGNode);
	}

	DWORD TerrainPatchRender::GetMtlTrait1(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait1(0,pSGNode);
	}

	DWORD TerrainPatchRender::GetMtlTrait2(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait2(0,pSGNode);
	}

	const AABBox& TerrainPatchRender::GetAABBox() const
	{
		if(m_pTerrainPatchRes==NULL
			||!m_pTerrainPatchRes->IsCreated())
			return MathConst::SmallBox;
		return *m_pTerrainPatchRes->GetAABBox();
	}

}//namespace Cool3D