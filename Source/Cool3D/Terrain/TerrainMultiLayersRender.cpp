#include "StdAfx.h"
#include ".\TerrainMultiLayersRender.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IIndexBuffer.h"
#include "..\Device\IVertShader.h"
#include "..\Device\IDevice.h"
#include "TPatchTriangulationStatic.h"
#include "TerrainPatchRes.h"
#include "TerrainPatch.h"
#include "TerrainMtlMultiLayers.h"
#include "..\Kernel.h"
#include "..\Cool3DOption.h"
#include "Heightmap.h"
#include "..\Math\Transform.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\RenderSys\RenderSys.h"
#include "..\RenderSys\MtlRenderCache.h"
#include "..\Util\SpeedAnalyser.h"
#include "..\RenderSys\HardwareBufferPool.h"
#include "TerrainPatchRender.h"
#include "..\SceneGraph\SGLightNode.h"
#include "..\math\Frustum.h"

namespace Cool3D
{

	class SortByZValFun
	{
	public:
		Matrix4 *m_pView;

		SortByZValFun(Matrix4 *pView):m_pView(pView)
		{}

		bool operator()(const TerrainPatch* pPatch1,const TerrainPatch* pPatch2)
		{
			AABBox box1;
			AABBox box2;
			pPatch1->GetBox(box1);
			pPatch2->GetBox(box2);
			Vector3 c1=(box1.max+box1.min)*0.5f;
			Vector3 c2=(box2.max+box2.min)*0.5f;
			Vector4 t1,t2;
			D3DXVec3Transform(&t1,&c1,m_pView);
			D3DXVec3Transform(&t2,&c2,m_pView);
			return t1.z<t2.z;
		}
	};

	void _SortByZVal(vector<TerrainPatch*>& array,Matrix4* pView)
	{
		SortByZValFun sortObj(pView);
		sort(array.begin(),array.end(),sortObj);
	}

	//--class TerrainMultiLayersRender::Member------------------------
	class TerrainMultiLayersRender::Member
	{
	public:
		vector<TerrainPatch*>	m_trnArray;
	public:
		Member(){}
		~Member(){}

		/** 创建资源
		*/
		void Create()
		{
		}

		/** 渲染所有地块
		*/
		void Render(SceneGraph* pSG)
		{
			if(m_trnArray.empty())
				return;

			IDevice* pDev=Device();
			RenderSys* pRenderSys=Kernel::Inst()->GetRenderSys();

			//--
			_SortByZVal(m_trnArray,pSG->GetCamera()->GetView());

			//--
			StateBlock block;
			pDev->MatLoad(EMS_World,&MathConst::IdentityMat);
			pDev->FFlushMatrix(true,false,false,false);
			pDev->SetStreamDefine(pRenderSys->GetStreamDefine(EVType_Trn4));

			//--
			for(size_t i=0;i<m_trnArray.size();i++)
			{
				TerrainPatch* pPatch=m_trnArray[i];

				RenderNode* pNode=pPatch->GetRenderNode(0);
				if(pNode==NULL)
					continue;

				ASSERT(pNode->IS_STATIC_CLASS(TerrainPatchRender));
				TerrainPatchRender* pRender=(TerrainPatchRender*)pNode;
				pRender->Draw(pRenderSys,pPatch);
			}
		}
	};

	//--class TerrainMultiLayersRender--------------------------------
	TerrainMultiLayersRender::TerrainMultiLayersRender(void)
	{
		m_bCreated=false;
		m_p=new Member;
	}

	TerrainMultiLayersRender::~TerrainMultiLayersRender(void)
	{
		delete m_p;
	}

	void TerrainMultiLayersRender::Create()
	{
		if(!IsHWSurpport())
			return;

		m_p->Create();
		m_bCreated=true;
	}

	void TerrainMultiLayersRender::Destroy()
	{
	}

	void TerrainMultiLayersRender::Begin()
	{
		if(!m_bCreated)
			return;

		m_p->m_trnArray.clear();
	}

	bool TerrainMultiLayersRender::AttachPatch(TerrainPatch* pPatch)
	{
		if(!m_bCreated)
			return false;

		if(pPatch->GetLod()>3)		
			return false;

		m_p->m_trnArray.push_back(pPatch);
		return true;
	}

	void TerrainMultiLayersRender::End(SceneGraph* pSG)
	{
		if(!m_bCreated)
			return;

		m_p->Render(pSG);
	}

	vector<TerrainPatch*>& TerrainMultiLayersRender::GetTrnArray()
	{
		return m_p->m_trnArray;
	}

	TerrainMultiLayersRender* TerrainMultiLayersRender::Inst()
	{
		static TerrainMultiLayersRender Inst;
		return &Inst;
	}

	bool TerrainMultiLayersRender::IsHWSurpport()
	{
		return Device()->CapsPixelShaderVersion()>=D3DPS_VERSION(1,4);
	}

}//namespace Cool3D