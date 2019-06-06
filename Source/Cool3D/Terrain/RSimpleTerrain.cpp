#include "StdAfx.h"
#include ".\RSimpleTerrain.h"
#include "..\Device\IDevice.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IIndexBuffer.h"
#include "..\RenderSys\RenderSys.h"
#include "..\Kernel.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\SceneGraph\HugeQuadTreeSG.h"
#include "..\Util\FMemory.h"
#include ".\SGSimpleTerrain.h"
#include "..\Device\StateBlock.h"
#include "..\Util\SpeedAnalyser.h"
#include "..\Device\IVertShader.h"
#include "..\Device\IPixelShader.h"
#include "..\Math\Frustum.h"
#include "..\EditorSupport\TerrainEditor.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(RSimpleTerrain,RenderNode);

	RSimpleTerrain::RSimpleTerrain()
	{
		SetPriority(EPri_Low);//先画近处地形
	}

	RSimpleTerrain::~RSimpleTerrain(void)
	{
	}

	void RSimpleTerrain::BuildRenderItems(SGSimpleTerrain* pSimpleTerrain,vector<RenderNode*>& renderNodes)
	{
		const vector<SceneNode*>& sceneNodes=pSimpleTerrain->GetVisPatchNodes();
		for(int i=0;i<(int)sceneNodes.size();i++)
		{
			SceneNode* pSGNode=sceneNodes[i];
			if(pSGNode->GetNumRenderNode()>0)
			{
				RenderNode* pRenderNode=pSGNode->GetRenderNode(0);
				if(pRenderNode!=NULL)
				{
					renderNodes.push_back(pRenderNode);
				}
			}
		}
	}

	class SortByZValFun
	{
	public:
		Matrix4 *m_pView;

		SortByZValFun(Matrix4 *pView):m_pView(pView)
		{}

		bool operator()(const RenderNode* pNode1,const RenderNode* pNode2)
		{
			const AABBox& box1=pNode1->GetAABBox();
			const AABBox& box2=pNode2->GetAABBox();
			Vector3 c1=(box1.max+box1.min)*0.5f;
			Vector3 c2=(box2.max+box2.min)*0.5f;
			Vector4 t1,t2;
			D3DXVec3Transform(&t1,&c1,m_pView);
			D3DXVec3Transform(&t2,&c2,m_pView);
			return t1.z<t2.z;
		}
	};

	void RSimpleTerrain::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)	
	{
		ASSERT(pSGNode->IS_STATIC_CLASS(SGSimpleTerrain));
		SGSimpleTerrain* pSimpleTerrain=(SGSimpleTerrain*)pSGNode;

		ASSERT(pSG->IS_STATIC_CLASS(HugeQuadTreeSG));
		HugeQuadTreeSG* pQuadTreeSG=(HugeQuadTreeSG*)pSG;

		//--Get simple terrain patch render nodes;
		vector<RenderNode*> renderNodes;
		BuildRenderItems(pSimpleTerrain,renderNodes);

		//ScreenPrinter::Inst()->Print(_T("simple terrain patch:%d"),renderNodes.size());

		//--Sort by z
		CameraBase *pCamera=pSG->GetCamera();
		SortByZValFun sortObj(pCamera->GetView());
		sort(renderNodes.begin(),renderNodes.end(),sortObj);

		//--Set project matrix
		IDevice* pDev=Device();
		pDev->MatPush(EMS_Prj);
		pDev->MatPush(EMS_View);
		pDev->MatPush(EMS_World);
		Matrix4 matPrj;
		float zFar=pQuadTreeSG->GetViewZoneSize(HugeQuadTreeSG::EVZT_SimpleTerrain).z;
		pDev->MatLoad(EMS_Prj,pCamera->GetPrj(zFar,matPrj));
		pDev->MatLoad(EMS_View,pCamera->GetView());
		pDev->FFlushMatrix(true,true,true,false);

		//--Set viewport
		tagViewport oldViewport;
		pDev->GetViewport(oldViewport);
		tagViewport newViewport=oldViewport;
		newViewport.minZ=0.5f;
		newViewport.maxZ=1.0f;
		pDev->SetViewport(&newViewport);

		//--设置裁剪面
		Frustum frustum;
		float zNear=pQuadTreeSG->GetViewZoneSize(HugeQuadTreeSG::EVZT_BigNode).z-TerrainEditor::ESimplePatchSize*50.0f;
		frustum.Extract(pCamera->GetView(),pCamera->GetPrj(zNear,zFar,matPrj));
		Plane oldPlane;
		bool hasClipPlane=false;
		hasClipPlane=pDev->GetClipPlane(&oldPlane);
		pDev->SetClipPlane(&frustum.GetNearPlane());

		//--Set vert stream define
		IStreamDefine *pDef=pSys->GetStreamDefine(EVType_P);
		pDev->SetStreamDefine(pDef);

		//--Draw
		IMaterial* pMtl=pSGNode->GetMtl(0);
		int numPass=RenderNode::BeginMtl(pSys,pMtl);
		for(int p=0;p<numPass;p++)
		{
			RenderNode::BeginMtlPass(p);
			for(int i=0;i<(int)renderNodes.size();i++)
			{
				RenderNode* pNode=renderNodes[i];
				pNode->Draw(pSys,pSGNode,nLod,pItem,pSG);
			}
			RenderNode::EndMtlPass();
		}
		RenderNode::EndMtl();

		//--恢复裁剪面
		if(hasClipPlane)
			pDev->SetClipPlane(&oldPlane);
		else
			pDev->SetClipPlane(NULL);

		//--Restore viewport
		pDev->SetViewport(&oldViewport);

		//--Restore project matrix
		pDev->MatPop(EMS_Prj);
		pDev->MatPop(EMS_View);
		pDev->MatPop(EMS_World);
		pDev->FFlushMatrix(true,true,true,false);
	}

	EMtlTrait RSimpleTerrain::GetMtlTrait(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait(0,pSGNode);
	}

	DWORD RSimpleTerrain::GetMtlTrait1(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait1(0,pSGNode);
	}

	DWORD RSimpleTerrain::GetMtlTrait2(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait2(0,pSGNode);
	}

	const AABBox& RSimpleTerrain::GetAABBox() const
	{
		return MathConst::BigBox;
	}

	
}//namespace Cool3D