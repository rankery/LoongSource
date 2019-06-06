#include "StdAfx.h"
#include "WaterRefractMapRender.h"
#include "..\Util\DebugMisc.h"
#include "..\Kernel.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\Device\IRenderTexture.h"
#include "..\Device\IDevice.h"
#include "..\Cool3DOption.h"
#include "..\Terrain\TerrainMultiLayersRender.h"
#include "..\Terrain\TerrainPatchRender.h"
#include "..\Terrain\TerrainPatch.h"
#include "..\Terrain\TerrainPatchRes.h"
#include "..\Terrain\RRiverPatch.h"
#include "..\ResSys\ResVertShader.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\Device\IVertShader.h"

using namespace std;

namespace Cool3D
{
	//--class Member-----------------------------------------------------------------
	class WaterRefractMapRender::Member
	{
	public:
		IRenderTexture*  m_pRefractRTex;
		ResVertShader*   m_pHeightBasedFogVS;
		ResPixelShader*  m_pHeightBasedFogPS;
		ResPixelShader*  m_pRefractMaskPS;
	public:
		Member() : 
		m_pRefractRTex( NULL ),
		m_pHeightBasedFogVS( NULL ),
		m_pHeightBasedFogPS( NULL ),
		m_pRefractMaskPS( NULL )
		{
		}

		void Init()
		{
			IDevice* pDev = Device();
			m_pRefractRTex = pDev->NewRenderTexture();
			m_pRefractRTex->Create( pDev->GetClientWidth(), pDev->GetClientHeight(), EPF_A8R8G8B8, false, false, 1, true, true );

			m_pHeightBasedFogVS = ResMgr::Inst()->NewVS(_T("data\\system\\shader\\h_fog.vs"),MAKELONG(2,0));
			m_pHeightBasedFogPS = ResMgr::Inst()->NewPS(_T("data\\system\\shader\\h_fog.ps"),MAKELONG(1,4));

			m_pRefractMaskPS    = ResMgr::Inst()->NewPS(_T("data\\system\\shader\\water_rr_mask.ps"),MAKELONG(1,4));
		}

		void Destroy()
		{
			RES_MGR_RELEASE_VS( m_pHeightBasedFogVS );
			RES_MGR_RELEASE_PS( m_pHeightBasedFogPS );
			RES_MGR_RELEASE_PS( m_pRefractMaskPS );

			SAFE_DELETE( m_pRefractRTex );
		}
	};


	//--class WaterRefractMapRender-----------------------------------------------------
	WaterRefractMapRender::WaterRefractMapRender(void)
	{
		m_p=new Member;
	}

	WaterRefractMapRender::~WaterRefractMapRender(void)
	{
		delete m_p;
	}

	void WaterRefractMapRender::Init()
	{
		if( !IsHWSurpport() )
			return;

		m_p->Init();
	}

	void WaterRefractMapRender::Destroy()
	{
		if( !IsHWSurpport() )
			return;

		m_p->Destroy();
	}

	WaterRefractMapRender* WaterRefractMapRender::Inst()
	{
		static WaterRefractMapRender Inst;
		return &Inst;
	}

	bool WaterRefractMapRender::IsHWSurpport()
	{
		IDevice* pDev=Device();
		if( pDev!=NULL )
		{
			return pDev->CapsMaxTextureStages() >= 4 && 
				   pDev->FCapsMaxSimultaneousTextures() >= 4 && 
				   pDev->CapsVertexShaderVersion() >= D3DVS_VERSION(2,0) &&
				   pDev->CapsPixelShaderVersion() >= D3DPS_VERSION(2,0) &&
				   pDev->CapsColorWriteEnable();
		}
		return false;
	}

	void WaterRefractMapRender::RenderTerrainPatchFog( TerrainPatch* pPatch )
	{
		if( NULL == pPatch || NULL == pPatch->GetTerrainPatchRes() || -1 == pPatch->GetTerrainPatchRes()->GetRiverID() )
			return;

		IDevice* pDev = Device();
		RenderSys* pRenderSys=Kernel::Inst()->GetRenderSys();

		float   fStart = pPatch->GetTerrainPatchRes()->GetUnderWaterFogStart();
		float   fEnd   = pPatch->GetTerrainPatchRes()->GetUnderWaterFogEnd();
		Vector3 vColor = pPatch->GetTerrainPatchRes()->GetUnderWaterFogColor();

		m_p->m_pHeightBasedFogVS->GetVS()->SetFloat( ("g_fStart"), fStart );
		m_p->m_pHeightBasedFogVS->GetVS()->SetFloat( ("g_fEnd"), fEnd );
		m_p->m_pHeightBasedFogVS->GetVS()->SetFloatArray("g_Color",(float*)&vColor,3);

		Plane clipplane(Vector3(0,fStart,0),-MathConst::AxisY);
		pDev->SetClipPlane( &clipplane );

		RenderNode* pNode=pPatch->GetRenderNode(0);
		if( NULL == pNode || !pNode->IS_STATIC_CLASS(TerrainPatchRender) )
			return;

		TerrainPatchRender* pRender=(TerrainPatchRender*)pNode;
		pRender->Draw(pRenderSys,pPatch);
		
		pDev->SetClipPlane( NULL );
	}

	void WaterRefractMapRender::RenderUnderWaterFog( vector<RenderItem>& trnArray, SceneGraph* pSG )
	{
		if( NULL == m_p->m_pHeightBasedFogVS || NULL == m_p->m_pHeightBasedFogPS )
			return;
		if( !m_p->m_pHeightBasedFogVS->IsCreated() || !m_p->m_pHeightBasedFogPS->IsCreated() )
			return;
		if( NULL == m_p->m_pHeightBasedFogVS->GetVS() || NULL == m_p->m_pHeightBasedFogPS->GetPS() )
			return;

		// 设置渲染状态
		IDevice* pDev = Device();
		StateBlock block;
		block.SaveState( ERS_AlphaBlendEnable );
		pDev->EnableAlphaBlend( true );
		pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);

		// 设置矩阵
		Matrix4 matWorldViewPrj = *( pSG->GetCamera()->GetView() );
		Matrix4 prjZbias;
		pSG->GetCamera()->GetPrjZBias(-0.0001f,prjZbias);
		matWorldViewPrj *= prjZbias;		
		m_p->m_pHeightBasedFogVS->GetVS()->SetConstValue("g_matWorldViewPrj",(void*)&matWorldViewPrj,sizeof(Matrix4));

		pDev->SetVertShader( m_p->m_pHeightBasedFogVS->GetVS() );
		pDev->SetPixelShader( m_p->m_pHeightBasedFogPS->GetPS() );

		TerrainPatchRender::ERenderMode eOldRenderMode = TerrainPatchRender::m_renderMode;
		TerrainPatchRender::m_renderMode = TerrainPatchRender::ERM_WithOutMtl;

		// 使用地形渲染水下高度雾
		size_t size = 0;
		if( TerrainMultiLayersRender::Inst()->IsCreated() )
		{
			vector<TerrainPatch*>& trnPatchArray = TerrainMultiLayersRender::Inst()->GetTrnArray();
			size = trnPatchArray.size();
			for( size_t i = 0; i < size; i++ )
				RenderTerrainPatchFog( trnPatchArray[i] );
		}

		size = trnArray.size();
		for( size_t i = 0; i < size; i++ )
		{
			RenderItem& item = trnArray[i];
			if( item.pSGNode->IS_STATIC_CLASS( TerrainPatch ) )
			{
				TerrainPatch* pPatch = (TerrainPatch*)item.pSGNode;
				RenderTerrainPatchFog( pPatch );
			}
		}

		TerrainPatchRender::m_renderMode = eOldRenderMode;

		pDev->SetVertShader( NULL );
		pDev->SetPixelShader( NULL );
		block.Apply();
	}

	void WaterRefractMapRender::RenderRefractMask( map<DWORD,vector<RenderItem>>& mapRiver, SceneGraph* pSG )
	{
		if( NULL == m_p->m_pRefractMaskPS || !m_p->m_pRefractMaskPS->IsCreated() )
			return;
		if( NULL == m_p->m_pRefractMaskPS->GetPS() )
			return;
		
		// 设置渲染状态
		IDevice* pDev = Device();
		RenderSys* pRenderSys=Kernel::Inst()->GetRenderSys();
		StateBlock block;
		block.SaveState( ERS_AlphaBlendEnable ); pDev->EnableAlphaBlend( false );
		block.SaveState( ERS_AlphaTestEnable );  pDev->EnableAlphaTest( false );
		block.SaveState( ERS_CullMode );		 pDev->SetCullMode( ECull_None );
		block.SaveState( ERS_ColorWriteEnable ); pDev->SetRenderState( ERS_ColorWriteEnable, D3DCOLORWRITEENABLE_ALPHA );

		// 使用最大LOD渲染河流RefractMask
		pDev->SetStreamDefine(pRenderSys->GetStreamDefine(EVType_Water));
		pDev->SetPixelShader( m_p->m_pRefractMaskPS->GetPS() );
		for(map<DWORD,vector<RenderItem>>::iterator iter = mapRiver.begin(); iter != mapRiver.end(); iter++ )
		{
			for(size_t i=0;i<iter->second.size();i++)
			{
				RenderItem& item=iter->second[i];
				ASSERT( item.pNode->IS_STATIC_CLASS(RRiverPatch) );
				RRiverPatch* pRNode = (RRiverPatch*)item.pNode;
				pRNode->DrawMaxLodWithOutMtl(pRenderSys,item.pSGNode,&item,pSG);
			}
		}

		block.Apply();
		pDev->SetPixelShader( NULL );
	}

	void WaterRefractMapRender::RenderRefractMap( map<DWORD,vector<RenderItem>>& mapRiver, vector<RenderItem>& trnArray, SceneGraph* pSG, const tagViewport& viewPort )
	{
		if( mapRiver.empty() )
			return;

		if( NULL == m_p->m_pRefractRTex )
			return;

		IDevice* pDev = Device();
		pDev->CopyFromBackbuffer( m_p->m_pRefractRTex, NULL, NULL );

		if( m_p->m_pRefractRTex->Begin() )
		{
			pDev->MatPush(EMS_World);
			pDev->MatLoad(EMS_World,&MathConst::IdentityMat);
			pDev->FFlushMatrix(true,false,false,false);

			pDev->SetViewport( &viewPort ); // 更改了RenderTarget 重新设置view port

			StateBlock block;
			block.SaveState( ERS_DepthTestEnable );  pDev->EnableDepthTest( true );
			block.SaveState( ERS_DepthWriteEnable ); pDev->EnableDepthWrite( false );

			RenderUnderWaterFog( trnArray, pSG );

			RenderRefractMask( mapRiver, pSG );

			m_p->m_pRefractRTex->End();

			block.Apply();
			pDev->SetViewport( &viewPort ); // 更改了RenderTarget 重新设置view port

			pDev->MatPop(EMS_World);
			pDev->FFlushMatrix(true,false,false,false);//flush world matrix only
		}

	}

	IRenderTexture* WaterRefractMapRender::GetRefractRTex()
	{
		return m_p->m_pRefractRTex;
	}
}//end namespace Cool3D