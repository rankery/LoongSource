#include "StdAfx.h"
#include ".\rendersysfixed.h"
#include "..\Math\CameraBase.h"
#include "..\Util\Exception.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\SceneGraph\EntityNode.h"
#include "..\SceneGraph\AvatarNode.h"
#include "..\SceneGraph\SGSkinNode.h"
#include "..\SceneGraph\SceneProjector.h"
#include "..\SceneGraph\SGStaticProjector.h"
#include "..\SceneGraph\SGSkyNode.h"
#include "..\SceneGraph\SGBladeGlow.h"
#include "..\SceneGraph\SGRoleGlow.h"
#include "..\Terrain\TerrainPatch.h"
#include "RenderNode.h"
#include "StaticMeshIndexed.h"
#include "RKeyFrame.h"
#include "DynamicMesh.h"
#include "RStaticMesh.h"
#include "..\Kernel.h"
#include "..\Cool3DOption.h"
#include "..\Device\IDeviceTex.h"
#include "..\Device\StateBlock.h"
#include "..\Device\IStreamDefine.h"
#include "..\Device\FogDefine.h"
#include "..\Device\IRenderTexture.h"
#include "..\Device\IRenderCubeTexture.h"
#include "..\Animation\AnimationCtrl.h"
#include "..\Terrain\TerrainPatchRender.h"
#include "..\Terrain\RSimpleTerrain.h"
#include "..\Terrain\GrassPatch.h"
#include "MtlRenderCacheFixed.h"
#include "..\RenderMtl\IMaterial.h"
#include "DevPixelStageFixed.h"
#include "..\SpecialEffect\SGEffectNode.h"
#include "..\SceneGraph\SGSunNode.h"
#include "..\SceneGraph\SGSunFlare.h"
#include "..\Device\DX9Device\DX9Device.h"
#include "..\Device\DX9Device\DX9RenderTex.h"
#include "..\Math\Frustum.h"
#include "..\PostProcess\PostProcess.h"
#include "..\PostProcess\PostProcessBlur.h"
#include "..\PostProcess\PostProcessHeatHaze.h"
#include "..\Device\DrawX.h"
#include "..\Util\SpeedAnalyser.h"
#include "..\Util\FMemory.h"
#include ".\SortByMtlAndZVal.h"
#include ".\IRenderCallBack.h"
#include "..\Terrain\SGRiverPatch.h"
#include "..\Device\IVertShader.h"

#ifdef USE_SPEEDTREE
#include "..\SpeedTreeRT\RSpeedTreeNode.h"
#include "..\SpeedTreeRT\SpeedTreeRender.h"
#endif

#include "..\Terrain\TerrainMultiLayersRender.h"
#include "..\RenderSys\WaterRender.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(RenderSysFixed,RenderSys);

	/**	\class RTChain 
	\brief 包含两个RenderTexture的交换链,用于后期处理
	*/
	class RenderSysFixed::RTChain
	{
		IRenderTexture*		m_pRenderTex[2];	// 交换用的两个RenderTexture
		int					m_nIndex;			// 当前RenderTexture索引

	public:
		RTChain()
		{
			m_pRenderTex[0] = NULL;
			m_pRenderTex[1] = NULL;
			m_nIndex = 0;
		}

		~RTChain()
		{
			Destroy();
		}

		//!创建RenderTexture
		void Create()
		{
			for( int i=0; i<2; i++ )
			{
				if( m_pRenderTex[i] == NULL )
				{
					m_pRenderTex[i] = Device()->NewRenderTexture();
					m_pRenderTex[i]->Create( Device()->GetClientWidth(), Device()->GetClientHeight(), EPF_A8R8G8B8, false );
				}
			}
		}

		//!释放RenderTexture
		void Destroy()
		{
			SAFE_DELETE(m_pRenderTex[0]);
			SAFE_DELETE(m_pRenderTex[1]);
		}

		//!交换
		void Flip() { m_nIndex = 1 - m_nIndex; }

		//!返回RenderTexture
		IRenderTexture* GetCurrRT() { return m_pRenderTex[m_nIndex]; }
		IRenderTexture* GetNextRT() { return m_pRenderTex[1-m_nIndex]; }
	};

	RenderSysFixed::RenderSysFixed(void)
	{
		m_renderCount=0;

		memset(m_streamDef,0,sizeof(m_streamDef));
		
		m_bLightEnable=true;

		m_pRTChain = new RTChain;
	}

	RenderSysFixed::~RenderSysFixed(void)
	{
		for(int i=0;i<EVType_Num;i++)
		{
			SAFE_DELETE(m_streamDef[i]);
		}
		SAFE_DELETE( m_pRTChain );
	}

	void RenderSysFixed::EnableFog(bool bEnable)
	{
		Device()->FEnableFog(bEnable);
	}
	//--
	void RenderSysFixed::InitDevice()
	{
		RenderSys::InitDevice();
		//--
		IDevice *pDev=Device();
		pDev->FEnableLight(m_bLightEnable);
		pDev->FEnableSpecular(false);
		pDev->FSetRenderColor(&RenderSys::WhiteRenderColor);//设置默认的材质颜色

#ifdef USE_SPEEDTREE
		SpeedTreeRender::Init();
#endif

		tagTexStage defaultStage;
		for(int i=0;i<pDev->CapsMaxTextureStages();i++)
		{
			DevPixelStageFixed::SetStage2Device(i,defaultStage);
		}

		// 创建和初始化热气后处理
		if( NULL == m_pPostProcessHeatHaze && PostProcessHeatHaze::IsHWSupport() )
		{
			m_pPostProcessHeatHaze = new PostProcessHeatHaze;
			if( m_pPostProcessHeatHaze->Init() )
				AddPostProcess( m_pPostProcessHeatHaze );
			else
				SAFE_DELETE( m_pPostProcessHeatHaze )
		}
	}

	int RenderSysFixed::PrepMtl(IMaterial *pMtl)
	{
		if(pMtl==NULL)
			return 0;
		if(pMtl->GetRenderCache()==NULL)
		{
			if(!pMtl->IsResourceReady())//等待所需resource load完成
				return 0;
			MtlRenderCacheFixed *pNewCache=new MtlRenderCacheFixed;
			pNewCache->CompileMaterial(pMtl);
			pMtl->AttachRenderCache(pNewCache);
		}
		return pMtl->GetRenderCache()->GetNumPass();
	}

	bool RenderSysFixed::GetCapacity(ERenderSysCap cap)
	{
		return false;//不具备任何能力
	}
	
	void RenderSysFixed::Begin(SceneGraph *pSG)
	{
		RenderSys::Begin(pSG);

		ASSERT(pSG!=NULL);
		m_trnArray.clear();
		m_grassArray.clear();
		m_opaqueArray.clear();
		m_alphaTestArray.clear();
		m_alphaTestAndBlendArray.clear();
		m_alphaBlendArray.clear();
		m_alphaBlendDisableZArray.clear();
		m_alphaBlendDisableZWriteArray.clear();
	#ifdef USE_SPEEDTREE
		SpeedTreeRender::Inst()->Clear();
	#endif

		//--保存所有Matrix
		IDevice *pDev=Device();
		pDev->MatPush(EMS_Prj);
		pDev->MatPush(EMS_View);
		pDev->MatPush(EMS_World);
		pDev->MatPush(EMS_Tex0);
		pDev->MatPush(EMS_Tex1);
		//--setup camera
		CameraBase *pCamera=pSG->GetCamera();
		if(pCamera!=NULL)
		{
			pDev->MatLoad(EMS_Prj,pCamera->GetPrj());
			pDev->MatLoad(EMS_View,pCamera->GetView());
			pDev->FFlushMatrix(false,true,true,false);
		}

		//--setup sun light
		const tagLight& sunLgt=pSG->GetSunLight();
		pDev->FSetLight(0,&sunLgt);
		const tagDistFog& fog=pSG->GetSceneFog();
		pDev->FSetFog(fog.color,fog.start,fog.end,fog.density);

		//--
		TerrainMultiLayersRender::Inst()->Begin();
		WaterRender::Inst()->Begin();
	}

	void RenderSysFixed::RenderSceneNode(CameraBase *pCamera,const SceneNode *pNode,IRenderTexture *pTarget,bool clearTarget,int lod,DWORD targetColor,IRenderCallBack* pCallBack,SceneGraph* pSG,const tagViewport* pNewViewport/*=NULL*/)
	{
		IDevice *pDev=Device();
		pDev->MatPush(EMS_Prj);
		pDev->MatPush(EMS_View);
		pDev->MatPush(EMS_World);
		pDev->MatPush(EMS_Tex0);
		pDev->MatPush(EMS_Tex1);
		if(pCamera!=NULL)
		{
			pDev->MatLoad(EMS_Prj,pCamera->GetPrj());
			pDev->MatLoad(EMS_View,pCamera->GetView());
			pDev->FFlushMatrix(false,true,true,false);
		}

		if(pTarget)
		{
			if(!pTarget->Begin())
				return;//render target失败
			if(clearTarget)
			{
				bool bDepth=pTarget->HaveDepthBuffer();
				bool bStencil=bDepth&&pTarget->HaveStencilBuffer();
				pDev->Clear(true,bDepth,bStencil,targetColor,1.0f,0);
			}
			if(NULL!=pNewViewport)
				pDev->SetViewport(pNewViewport);
		}

		RenderSGNodeR(pNode,pCamera,lod,pCallBack,pSG);

		if(pTarget)
			pTarget->End();

		pDev->MatPop(EMS_Prj);
		pDev->MatPop(EMS_View);
		pDev->MatPop(EMS_World);
		pDev->MatPop(EMS_Tex0);
		pDev->MatPop(EMS_Tex1);
		pDev->FFlushMatrix();//flush all matrix

		pDev->SetVertShader(NULL);
		pDev->SetPixelShader(NULL);
	}

	void RenderSysFixed::RenderSGNodeR(const SceneNode *pNode,CameraBase *pCamera,int lod,IRenderCallBack* pCallBack,SceneGraph* pSG)
	{
		//有些scene不能渲染
		if( pNode->IsStaticClass(_T("SGDynamicProjector")) || pNode->IsKindOf(_T("SGEffectNode")) )
			return;

		AnimationCtrl *pAni=GetAniCtrlFromSGNode(pNode);
		RenderItem tmp;
		memset(&tmp,0,sizeof(tmp));
		tmp.pAni=pAni;
		tmp.pSGNode=const_cast<SceneNode*>(pNode);
		tmp.nLod=lod;
		UINT numRenderNodes=pNode->GetNumRenderNode();
		for(UINT i=0;i<numRenderNodes;i++)
		{
			RenderNode *pRender=pNode->GetRenderNode(i);
			tmp.pNode=const_cast<RenderNode *>(pRender);
		#ifdef USE_SPEEDTREE
			if(pRender->IS_STATIC_CLASS(RSpeedTreeNode))
			{
				SpeedTreeRender::Inst()->Clear();
				SpeedTreeRender::Inst()->AttachTree(tmp);
				SpeedTreeRender::Inst()->RenderAllTrees(pCamera);
				continue;
			}
		#endif

			if(pCallBack!=NULL)
				pCallBack->OnRenderBefore(tmp.pSGNode,tmp.pNode);

			RenderOne(tmp,pSG);
		}

		//--render all children
		for(int i=0;i<pNode->GetNumChildren();i++)
		{
			RenderSGNodeR(pNode->GetChild(i),pCamera,lod,pCallBack,pSG);
		}
	}

	void RenderSysFixed::RenderRenderNode( CameraBase *pCamera,const SceneNode *pSceneNode,const RenderNode *pRenderNode,IRenderTexture *pTarget,bool clearTarget,int lod,DWORD targetColor/*=0xFFFFFFFF*/,IRenderCallBack* pCallBack/*NULL*/,SceneGraph *pSG/*=NULL*/ )
	{
		IDevice *pDev=Device();
		pDev->MatPush(EMS_Prj);
		pDev->MatPush(EMS_View);
		pDev->MatPush(EMS_World);
		pDev->MatPush(EMS_Tex0);
		pDev->MatPush(EMS_Tex1);
		if(pCamera!=NULL)
		{
			pDev->MatLoad(EMS_Prj,pCamera->GetPrj());
			pDev->MatLoad(EMS_View,pCamera->GetView());
			pDev->FFlushMatrix(false,true,true,false);
		}

		if(pTarget)
		{
			if(!pTarget->Begin())
				return;//render target失败
			if(clearTarget)
			{
				bool bDepth=pTarget->HaveDepthBuffer();
				bool bStencil=bDepth&&pTarget->HaveStencilBuffer();
				pDev->Clear(true,bDepth,bStencil,targetColor,1.0f,0);
			}
		}

		AnimationCtrl *pAni=GetAniCtrlFromSGNode(pSceneNode);
		RenderItem tmp;
		memset(&tmp,0,sizeof(tmp));
		tmp.pAni=pAni;
		tmp.pSGNode=const_cast<SceneNode*>(pSceneNode);
		tmp.nLod=lod;
		tmp.pNode=const_cast<RenderNode*>(pRenderNode);

		if(pCallBack!=NULL)
			pCallBack->OnRenderBefore(tmp.pSGNode,tmp.pNode);

		RenderOne(tmp,pSG);

		if(pTarget)
			pTarget->End();

		pDev->MatPop(EMS_Prj);
		pDev->MatPop(EMS_View);
		pDev->MatPop(EMS_World);
		pDev->MatPop(EMS_Tex0);
		pDev->MatPop(EMS_Tex1);
		pDev->FFlushMatrix();//flush all matrix

		pDev->SetVertShader(NULL);
		pDev->SetPixelShader(NULL);
	}

	void RenderSysFixed::AddRenderItem(RenderItem& newItem)
	{
		RenderNode *pNode=newItem.pNode;

	#ifdef USE_SPEEDTREE
		if(pNode->IS_STATIC_CLASS(RSpeedTreeNode))
		{
			SpeedTreeRender::Inst()->AttachTree(newItem);
		}
		else
	#endif
		if(newItem.pSGNode->IS_STATIC_CLASS(TerrainPatch))
		{
			if(!TerrainMultiLayersRender::Inst()->AttachPatch((TerrainPatch*)newItem.pSGNode))
				m_trnArray.push_back(newItem);
		}
		else if(newItem.pNode->IS_STATIC_CLASS(RSimpleTerrain))
		{
			m_trnArray.push_back(newItem);
		}
		else if(newItem.pSGNode->IS_STATIC_CLASS(GrassPatch))
		{
			m_grassArray.push_back(newItem);
		}
		else if(newItem.pSGNode->IS_STATIC_CLASS(SGRiverPatch))
		{
			WaterRender::Inst()->AttachNode(newItem);
		}
		else
		{
			switch(pNode->GetMtlTrait(newItem.pSGNode))
			{
			case EMT_Opacity:
				m_opaqueArray.push_back(newItem);
				break;
			case EMT_AlphaTest:
				m_alphaTestArray.push_back(newItem);
				break;
			case EMT_AlphaBlend:
				m_alphaBlendArray.push_back(newItem);
				break;
			case EMT_AlphaBlendDisableZ:
				m_alphaBlendDisableZArray.push_back(newItem);
				break;
			case EMT_AlphaBlendDisableZWrite:
				m_alphaBlendDisableZWriteArray.push_back(newItem);
				break;
			case EMT_AlphaTestAndBlend:
				m_alphaTestAndBlendArray.push_back(newItem);
				break;
			case EMT_Unknow:
				break;
			default:
				ASSERT(0 && "Unknown trans type!");
			}
		}
	}

	void RenderSysFixed::End(SceneGraph *pSG,bool bZScale)
	{
		ASSERT(pSG!=NULL);

		RenderSys::BuildRenderItems(pSG);

		IDevice *pDev=Device();
		pDev->SetVertShader(NULL);
		pDev->SetPixelShader(NULL);
		pDev->EnableAlphaBlend(false);

		size_t i,size;

		//--渲染天空
		if( Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainDetail)!=Cool3DOption::ETD_Editor
			&& pSG->GetSkyNode()!=NULL)
			RenderSky(pSG);

		//--修改Viewport
		tagViewport oldViewport;
		pDev->GetViewport(oldViewport);
		tagViewport newViewport=oldViewport;
		if(bZScale)
		{
			newViewport.minZ=0;
			newViewport.maxZ=0.5f;
			pDev->SetViewport(&newViewport);
		}

		m_matView=*(Device()->GetMatrix(EMS_View));
		m_matPrj=*(Device()->GetMatrix(EMS_Prj));

		//--渲染地形
		SortByZVal(m_trnArray,true);

		pDev->MatLoad(EMS_World,&MathConst::IdentityMat);
		pDev->FFlushMatrix(true,false,false,false);

		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainDetail)==Cool3DOption::ETD_Editor)//编辑器模式
		{
			TerrainPatchRender::m_renderMode=TerrainPatchRender::ERM_Editor;
			size=m_trnArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=m_trnArray[i];
				RenderTerrainPatch(item,pSG);
			}
		}
		else if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainDetail)==Cool3DOption::ETD_Precombine)//预混合贴图
		{
			TerrainPatchRender::m_renderMode=TerrainPatchRender::ERM_PreCombine;
			size=m_trnArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=m_trnArray[i];
				RenderTerrainPatch(item,pSG);
			}
		}
		else if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainDetail)==Cool3DOption::ETD_TwoLayers)//双层贴图
		{
			TerrainPatchRender::m_renderMode=TerrainPatchRender::ERM_TwoLayers;
			size=m_trnArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=m_trnArray[i];
				RenderTerrainPatch(item,pSG);
			}
		}
		else
		{
			if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainSpecular)==Cool3DOption::ETS_Open)
				pDev->FEnableSpecular(true);

			TerrainPatchRender::m_renderMode=TerrainPatchRender::ERM_MultiLayers;
			TerrainMultiLayersRender::Inst()->End(pSG);

			TerrainPatchRender::m_renderMode=TerrainPatchRender::ERM_PreCombine;
			size=m_trnArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=m_trnArray[i];
				RenderTerrainPatch(item,pSG);
			}

			pDev->FEnableSpecular(false);
		}

		//--渲染m_opaqueArray
		SortByMtlAndZVal(m_opaqueArray);
		size=m_opaqueArray.size();
		for(i=0;i<size;i++)
		{
			RenderItem& item=m_opaqueArray[i];
			RenderOne(item,pSG);
		}

		StateBlock block;
		block.SaveState(ERS_CullMode);
		block.SaveState(ERS_DepthTestEnable);
		block.SaveState(ERS_DepthWriteEnable);
		block.SaveState(ERS_AlphaBlendEnable);
		block.SaveState(ERS_SrcBlend);
		block.SaveState(ERS_DstBlend);
		block.SaveState(ERS_AlphaTestEnable);
		block.SaveState(ERS_AlphaTestFunc);
		block.SaveState(ERS_AlphaRef);
		block.SaveState(ERS_FFogEnable);
		block.SaveState(ERS_FLightEnable);

		//--渲染草
		SortByMtlAndZVal(m_grassArray);
		pDev->EnableAlphaTest(true);
		pDev->EnableAlphaBlend(false);
		pDev->SetAlphaTestFunc(ECMP_Greater);
		pDev->SetCullMode(ECull_None);
		pDev->FEnableLight(false);
		size=m_grassArray.size();
		for(i=0;i<size;i++)
		{
			RenderItem& item=m_grassArray[i];
			RenderOne(item,pSG);
		}
		pDev->SetCullMode(ECull_CW);
		pDev->FEnableLight(true);

		//--渲染m_alphaTestAndBlendArray
		//--2pass半透明处理
		//--先渲染不透明部分
		SortByMtlAndZVal(m_alphaTestAndBlendArray);
		pDev->EnableAlphaBlend(false);
		pDev->EnableAlphaTest(true);
		pDev->SetAlphaTestFunc(ECMP_Greater);
		pDev->SetAlphaRef(240);
		size=m_alphaTestAndBlendArray.size();
		for(i=0;i<size;i++)
		{
			RenderItem& item=m_alphaTestAndBlendArray[i];
			RenderOne(item,pSG);
		}
		//--后渲染透明部分
		pDev->EnableDepthWrite(false);
		pDev->EnableAlphaBlend(true);
		pDev->SetAlphaTestFunc(ECMP_LessEqual);
		pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);
		size=m_alphaTestAndBlendArray.size();
		for(i=0;i<size;i++)
		{
			RenderItem& item=m_alphaTestAndBlendArray[i];
			RenderOne(item,pSG);
		}
		pDev->EnableDepthWrite(true);
		pDev->EnableAlphaBlend(false);

		//--渲染m_alphaTestArray
		pDev->EnableAlphaTest(true);
		pDev->SetAlphaRef(0);
		pDev->SetAlphaTestFunc(ECMP_Greater);
		SortByMtlAndZVal(m_alphaTestArray);
		size=m_alphaTestArray.size();
		for(i=0;i<size;i++)
		{
			RenderItem& item=m_alphaTestArray[i];
			RenderOne(item,pSG);
		}
		pDev->EnableAlphaTest(false);

		//--渲染河流
		pDev->EnableDepthWrite(true);
		pDev->EnableAlphaTest(true);
		pDev->SetAlphaRef(0);
		pDev->SetAlphaTestFunc(ECMP_Greater);
		WaterRender::Inst()->End(pSG,m_trnArray,newViewport);
		pDev->EnableAlphaTest(false);

		//--渲染m_alphaBlendArray
		pDev->EnableAlphaBlend(true);
		pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);
		SortByZVal(m_alphaBlendArray,false);
		size=m_alphaBlendArray.size();
		for(i=0;i<size;i++)
		{
			RenderItem& item=m_alphaBlendArray[i];
			RenderOne(item,pSG);
		}

		//--渲染m_alphaBlendDisableZArray，主要是特效对象
		SortByZVal(m_alphaBlendDisableZArray,false);
		pDev->SetCullMode(ECull_None);
		pDev->EnableDepthTest(false);
		pDev->EnableDepthWrite(false);
		pDev->FEnableLight(false);
		pDev->FEnableFog(false);
		size=m_alphaBlendDisableZArray.size();
		for(i=0;i<size;i++)
		{
			RenderItem& item=m_alphaBlendDisableZArray[i];
			RenderOne(item,pSG);
		}

		//--渲染m_alphaBlendDisableZWriteArray，主要是特效对象
		SortByZVal(m_alphaBlendDisableZWriteArray,false);
		pDev->SetCullMode(ECull_None);
		pDev->EnableDepthTest(true);
		pDev->EnableDepthWrite(false);
		pDev->FEnableLight(false);
		pDev->FEnableFog(false);
		size=m_alphaBlendDisableZWriteArray.size();
		for(i=0;i<size;i++)
		{
			RenderItem& item=m_alphaBlendDisableZWriteArray[i];
			RenderOne(item,pSG);
		}

		//--渲染太阳光晕
		RenderSunFlare(pSG);

		// 后期处理
		if( m_bEnablePostProcess )
		{
			if( !m_PostProcessArray.empty() )
			{
				// 确保已创建RenderTexture
				m_pRTChain->Create();

				// BackBuffer -> CurrRT
				Device()->CopyFromBackbuffer(m_pRTChain->GetCurrRT(),NULL,NULL);

				for( i=0; i<m_PostProcessArray.size()-1; i++ )
				{
					// CurrRT -> NextRT
					if( m_PostProcessArray[i]->Start( m_pRTChain->GetCurrRT(), m_pRTChain->GetNextRT(), this, false, pSG ) )
					{
						// CurrRT <-> NextRT
						m_pRTChain->Flip();
					}
				}

				// CurrRT -> BackBuffer
				m_PostProcessArray[i]->Start( m_pRTChain->GetCurrRT(), m_pRTChain->GetNextRT(), this, true, pSG );
			}
			else
			{
				// 释放RenderTexture
				m_pRTChain->Destroy();
			}
		}

		//--恢复Viewport
		if(bZScale)
			pDev->SetViewport(&oldViewport);

		block.Apply();

		pDev->MatPop(EMS_Prj);
		pDev->MatPop(EMS_View);
		pDev->MatPop(EMS_World);
		pDev->MatPop(EMS_Tex0);
		pDev->MatPop(EMS_Tex1);
		pDev->FFlushMatrix();//flush all matrix
		
		pDev->SetVertShader(NULL);
		pDev->SetPixelShader(NULL);

		RenderSys::End(pSG);
	}

	class SortByMtlFun
	{
	public:
		bool operator()(const RenderItem &pNode1,const RenderItem &pNode2)
		{
			RenderNode *p1=pNode1.pNode;
			RenderNode *p2=pNode2.pNode;
			if(p1->GetPriority() == p2->GetPriority())
			{
				if(p1->GetMtlTrait1(pNode1.pSGNode) == p2->GetMtlTrait1(pNode2.pSGNode))
					return p1->GetMtlTrait2(pNode1.pSGNode) > p2->GetMtlTrait2(pNode2.pSGNode);
				else
					return p1->GetMtlTrait1(pNode1.pSGNode) > p2->GetMtlTrait1(pNode2.pSGNode);
			}
			else
				return p1->GetPriority() > p2->GetPriority();
		}
	};

	class SortByZValFun
	{
	public:
		Matrix4 *m_pView;
		bool m_bNear2Far;
		
		SortByZValFun(Matrix4 *pView,bool bNear2Far):m_pView(pView),m_bNear2Far(bNear2Far)
		{}

		bool operator()(const RenderItem &pNode1,const RenderItem &pNode2)
		{
			RenderNode *p1=pNode1.pNode;
			RenderNode *p2=pNode2.pNode;
			if(p1->GetPriority()==p2->GetPriority())
			{
				const AABBox& box1=pNode1.pNode->GetAABBox();
				const AABBox& box2=pNode2.pNode->GetAABBox();

				Vector3 c1=(box1.max+box1.min)*0.5f;
				Vector3 c2=(box2.max+box2.min)*0.5f;
				Vector4 w1,w2,v1,v2;
				D3DXVec3Transform(&w1,&c1,&pNode1.pSGNode->GetWorldMat());
				D3DXVec4Transform(&v1,&w1,m_pView);

				D3DXVec3Transform(&w2,&c2,&pNode2.pSGNode->GetWorldMat());
				D3DXVec4Transform(&v2,&w2,m_pView);

				if(m_bNear2Far)
				{
					return v1.z<v2.z;
				}
				else
				{
					return v1.z>v2.z;
				}
			}
			else
			{
				return p1->GetPriority() > p2->GetPriority();
			}
		}
	};

	class SortByMtlAndZValFun
	{
	public:
		Matrix4 *m_pView;

		SortByMtlAndZValFun(Matrix4 *pView):m_pView(pView)
		{}

		bool operator()(const RenderItem &pNode1,const RenderItem &pNode2)
		{
			RenderNode *p1=pNode1.pNode;
			RenderNode *p2=pNode2.pNode;
			if(p1->GetPriority()==p2->GetPriority())
			{
				if(p1->GetMtlTrait1(pNode1.pSGNode) == p2->GetMtlTrait1(pNode2.pSGNode))
				{
					if(p1->GetMtlTrait2(pNode1.pSGNode) == p2->GetMtlTrait2(pNode2.pSGNode))
					{
						const AABBox& box1=pNode1.pNode->GetAABBox();
						const AABBox& box2=pNode2.pNode->GetAABBox();

						Vector3 c1=(box1.max+box1.min)*0.5f;
						Vector3 c2=(box2.max+box2.min)*0.5f;
						Vector4 w1,w2,v1,v2;
						D3DXVec3Transform(&w1,&c1,&pNode1.pSGNode->GetWorldMat());
						D3DXVec4Transform(&v1,&w1,m_pView);

						D3DXVec3Transform(&w2,&c2,&pNode2.pSGNode->GetWorldMat());
						D3DXVec4Transform(&v2,&w2,m_pView);

						return v1.z<v2.z;
					}
					else
					{
						return p1->GetMtlTrait2(pNode1.pSGNode) > p2->GetMtlTrait2(pNode2.pSGNode);
					}
				}
				else
				{
					return p1->GetMtlTrait1(pNode1.pSGNode) > p2->GetMtlTrait1(pNode2.pSGNode);
				}
			}
			else
			{
				return p1->GetPriority() > p2->GetPriority();
			}
		}
	};

	void RenderSysFixed::SortByMtl(vector<RenderItem>& array)
	{
		SortByMtlFun sortObj;
		sort(array.begin(),array.end(),sortObj);
	}

	void RenderSysFixed::SortByZVal(vector<RenderItem>& array,bool bNear2Far)
	{
		SortByZValFun sortObj(&m_matView,bNear2Far);
		sort(array.begin(),array.end(),sortObj);
	}

	void RenderSysFixed::SortByMtlAndZVal(vector<RenderItem>& array)
	{
		class SortByMtlAndZVal sortObj;
		sortObj.Sort(array,m_matView);
	}

	void RenderSysFixed::RenderOne(RenderItem& item,SceneGraph* pSG)
	{
		IDevice *pDev=Device();

		//set world matrix
		pDev->MatLoad(EMS_World,&(item.pSGNode->GetWorldMat()));//upload world matrix of this node
		pDev->FFlushMatrix(true,false,false,false);//flush world matrix only

		//set lights
		int i;
		int numLgt=0;
		if(!m_bIgnoreSGLights)
		{
			for(i=0;i<RMax_Light;i++)
			{
				//第0个light留给sun light
				if(item.pLights[i]==NULL)
					break;
				numLgt++;
				pDev->FSetLight(i+1,item.pLights[i]);
			}
		}//endof if(light)

		if(item.pNode->IS_STATIC_CLASS(StaticMeshIndexed))
			RenderStaticNode(item,pSG);
		else if(item.pNode->IS_STATIC_CLASS(RKeyFrame))
			RenderKeyFrameNode(item,pSG);
		else if(item.pNode->IS_STATIC_CLASS(TerrainPatchRender))
			RenderTerrainPatch(item,pSG);
		else
		{
			item.pNode->Draw(this,item.pSGNode,item.nLod,&item,pSG);
		}

		//--close lights
		for(i=0;i<numLgt;i++)
			pDev->FSetLight(i+1,NULL);
	}

	void RenderSysFixed::RenderStaticNode(RenderItem& item,SceneGraph* pSG)
	{
		item.pNode->Draw(this,item.pSGNode,item.nLod,&item,pSG);
	}

	void RenderSysFixed::RenderKeyFrameNode(RenderItem& item,SceneGraph* pSG)
	{
		ASSERT(item.pAni!=NULL);
		ASSERT(item.pAni->IS_STATIC_CLASS(VertexAnimationCtrl));
		VertexAnimationCtrl *pCtrl=static_cast<VertexAnimationCtrl *>(item.pAni);

		const VertStream *pStream=pCtrl->GetFrameVertStream();
		if(pStream == NULL)
			return;

		IVertBuffer *pVB=RenderSys::AllocVB(pStream->GetBufferSize());
		ASSERT(pVB!=NULL);
		pVB->CopyFromVertStream(pStream);
		IDevice *pDev=Device();
		pDev->SetVB(0,pVB);
		IStreamDefine *pDef=GetStreamDefine(pStream->GetType());
		pDev->SetStreamDefine(pDef);

		item.pNode->Draw(this,item.pSGNode,item.nLod,&item,pSG);

		RenderSys::FreeVB(pVB);
	}

	void RenderSysFixed::RenderTerrainPatch(RenderItem& item,SceneGraph* pSG)
	{
		if(item.pNode->IS_STATIC_CLASS(TerrainPatchRender))
		{
			item.pNode->Draw(this,item.pSGNode,item.nLod,&item,pSG);
		}
		else if(item.pNode->IS_STATIC_CLASS(RSimpleTerrain))
		{
			item.pNode->Draw(this,item.pSGNode,item.nLod,&item,pSG);
		}
	}

	IStreamDefine* RenderSysFixed::GetStreamDefine(EVertType vertType)
	{
		ASSERT(vertType>EVType_None && vertType<EVType_Num);
		if(m_streamDef[vertType]==NULL)
		{
			IStreamDefine *pDef=Device()->NewStreamDefine();
			switch(vertType)
			{
			case EVType_PNT:
				pDef->Begin();
				pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
				pDef->AddElement(0,		4*3,	VET_FLOAT3,		VES_NORMAL,		0);
				pDef->AddElement(0,		4*6,	VET_FLOAT2,		VES_TEXCOORD,	0);
				pDef->End();
				break;
			case EVType_PN:
				pDef->Begin();
				pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
				pDef->AddElement(0,		4*3,	VET_FLOAT3,		VES_NORMAL,		0);
				pDef->End();
				break;
			case EVType_PT2:
				pDef->Begin();
				pDef->AddElement(0,	0,					VET_FLOAT3,	VES_POSITION,	0);
				pDef->AddElement(0,	sizeof(float)*3,	VET_FLOAT2,	VES_TEXCOORD,	0);
				pDef->AddElement(0,	sizeof(float)*5,	VET_FLOAT2,	VES_TEXCOORD,	1);
				pDef->End();
				break;
			case EVType_PNT2:
				pDef->Begin();
				pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
				pDef->AddElement(0,		4*3,	VET_FLOAT3,		VES_NORMAL,		0);
				pDef->AddElement(0,		4*6,	VET_FLOAT2,		VES_TEXCOORD,	0);
				pDef->AddElement(0,		4*8,	VET_FLOAT2,		VES_TEXCOORD,	1);
				pDef->End();
				break;
			case EVType_Trn4:
				{
					pDef->Begin();
					pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
					pDef->AddElement(0,		4*3,	VET_FLOAT3,		VES_NORMAL,		0);
					for(int i=0;i<TRN_MAX_UV_SET;i++)
						pDef->AddElement(1,	4*2*i,	VET_FLOAT2,		VES_TEXCOORD,	i);
					pDef->End();
				}
				break;
			case EVType_Trn2:
				{
					pDef->Begin();
					pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
					pDef->AddElement(0,		4*3,	VET_FLOAT3,		VES_NORMAL,		0);
					pDef->AddElement(1,		0,		VET_FLOAT2,		VES_TEXCOORD,	0);
					pDef->AddElement(1,		4*2,	VET_FLOAT2,		VES_TEXCOORD,	1);
					pDef->End();
				}
				break;
			case EVType_PD:
				pDef->Begin();
				pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
				pDef->AddElement(0,		4*3,	VET_COLOUR,		VES_DIFFUSE,	0);
				pDef->End();
				break;
			case EVType_PT:
				pDef->Begin();
				pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
				pDef->AddElement(0,		4*3,	VET_FLOAT2,		VES_TEXCOORD,	0);
				pDef->End();
				break;
			case EVType_PTp:
				pDef->Begin();
				pDef->AddElement(0,	0,					VET_FLOAT3,	VES_POSITION,	0);
				pDef->AddElement(0,	sizeof(float)*3,	VET_FLOAT3,	VES_TEXCOORD,	0);
				pDef->End();
				break;
			case EVType_PTpD:
				pDef->Begin();
				pDef->AddElement(0,	0,					VET_FLOAT3,	VES_POSITION,	0);
				pDef->AddElement(0,	sizeof(float)*3,	VET_COLOUR,	VES_DIFFUSE,	0);
				pDef->AddElement(0,	sizeof(float)*4,	VET_FLOAT3,	VES_TEXCOORD,	0);
				pDef->End();
				break;
			case EVType_PTD:
				pDef->Begin();
				pDef->AddElement(0,	0,					VET_FLOAT3,	VES_POSITION,	0);
				pDef->AddElement(0,	sizeof(float)*3,	VET_COLOUR,	VES_DIFFUSE,	0);
				pDef->AddElement(0,	sizeof(float)*4,	VET_FLOAT2,	VES_TEXCOORD,	0);
				pDef->End();
				break;
			case EVType_Water:
				pDef->Begin();
				pDef->AddElement(0,	0,					VET_FLOAT3,	VES_POSITION,	0);
				pDef->AddElement(0,	sizeof(float)*3,	VET_FLOAT2,	VES_TEXCOORD,	0);
				pDef->AddElement(1, 0,					VET_FLOAT3,	VES_NORMAL,		0);
				pDef->AddElement(1,	sizeof(float)*3,	VET_FLOAT2,	VES_TEXCOORD,	1);
				pDef->End();
				break;
			case EVType_PSprite:
				pDef->Begin();
				pDef->AddElement(0,	0,					VET_FLOAT3,	VES_POSITION,	0);
				pDef->AddElement(0,	sizeof(float)*3,	VET_FLOAT1,	VES_PSIZE,		0);
				pDef->AddElement(0,	sizeof(float)*4,	VET_COLOUR,	VES_DIFFUSE,	0);
				pDef->End();
				break;
			case EVType_PtT:
				pDef->Begin();
				pDef->AddElement(0,	0,					VET_FLOAT4,	VES_POSITION_TRANSED,	0);
				pDef->AddElement(0,	sizeof(float)*4,	VET_FLOAT2,	VES_TEXCOORD,	0);
				pDef->End();
				break;
			case EVType_PtD:
				pDef->Begin();
				pDef->AddElement(0,	0,					VET_FLOAT4,	VES_POSITION_TRANSED,	0);
				pDef->AddElement(0,	sizeof(float)*4,	VET_COLOUR,	VES_DIFFUSE,	0);
				pDef->End();
				break;
			case EVType_PtT4:
				pDef->Begin();
				pDef->AddElement(0,	0,					VET_FLOAT4,	VES_POSITION_TRANSED,	0);
				pDef->AddElement(0,	sizeof(float)*4,	VET_FLOAT2,	VES_TEXCOORD,	0);
				pDef->AddElement(0,	sizeof(float)*6,	VET_FLOAT2,	VES_TEXCOORD,	1);
				pDef->AddElement(0,	sizeof(float)*8,	VET_FLOAT2,	VES_TEXCOORD,	2);
				pDef->AddElement(0,	sizeof(float)*10,	VET_FLOAT2,	VES_TEXCOORD,	3);
				pDef->End();
				break;
			case EVType_PtDT:
				pDef->Begin();
				pDef->AddElement(0,	0,					VET_FLOAT4,	VES_POSITION_TRANSED,	0);
				pDef->AddElement(0,	sizeof(float)*4,	VET_COLOUR,	VES_DIFFUSE,	0);
				pDef->AddElement(0,	sizeof(float)*5,	VET_FLOAT2,	VES_TEXCOORD,	0);
				pDef->End();
				break;
			case EVType_PtDT2:
				pDef->Begin();
				pDef->AddElement(0,	0,					VET_FLOAT4,	VES_POSITION_TRANSED,	0);
				pDef->AddElement(0,	sizeof(float)*4,	VET_COLOUR,	VES_DIFFUSE,	0);
				pDef->AddElement(0,	sizeof(float)*5,	VET_FLOAT2,	VES_TEXCOORD,	0);
				pDef->AddElement(0,	sizeof(float)*7,	VET_FLOAT2,	VES_TEXCOORD,	1);
				pDef->End();
				break;
			case EVType_P:
				pDef->Begin();
				pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
				pDef->End();
				break;
			case EVType_PNDT:
				pDef->Begin();
				pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
				pDef->AddElement(0,		4*3,	VET_FLOAT3,		VES_NORMAL,		0);
				pDef->AddElement(0,		4*6,	VET_COLOUR,		VES_DIFFUSE,	0);
				pDef->AddElement(0,		4*7,	VET_FLOAT2,		VES_TEXCOORD,	0);
				pDef->End();
				break;
			case EVType_PNGT:
				pDef->Begin();
				pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
				pDef->AddElement(0,		4*3,	VET_FLOAT3,		VES_NORMAL,		0);
				pDef->AddElement(0,		4*6,	VET_FLOAT3,		VES_TEXCOORD,	1);
				pDef->AddElement(0,		4*9,	VET_FLOAT2,		VES_TEXCOORD,	0);
				pDef->End();
				break;
			case EVType_PNWIT:
				pDef->Begin();
				pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
				pDef->AddElement(0,		4*3,	VET_FLOAT3,		VES_NORMAL,		0);
				pDef->AddElement(0,		4*6,	VET_FLOAT4,		VES_BLEND_WEIGHT,0);
				pDef->AddElement(0,		4*10,	VET_COLOUR,		VES_BLEND_INDEX,0);
				pDef->AddElement(0,		4*11,	VET_FLOAT2,		VES_TEXCOORD,	0);
				pDef->End();
				break;
			case EVType_PNGWIT:
				pDef->Begin();
				pDef->AddElement(0,		0,		VET_FLOAT3,		VES_POSITION,	0);
				pDef->AddElement(0,		4*3,	VET_FLOAT3,		VES_NORMAL,		0);
				pDef->AddElement(0,		4*6,	VET_FLOAT3,		VES_TANGENT,	0);
				pDef->AddElement(0,		4*9,	VET_FLOAT4,		VES_BLEND_WEIGHT,0);
				pDef->AddElement(0,		4*13,	VET_COLOUR,		VES_BLEND_INDEX,0);
				pDef->AddElement(0,		4*14,	VET_FLOAT2,		VES_TEXCOORD,	0);
				pDef->End();
				break;
			default:
				ASSERT(0 && "unsupported key frame vertex type");
			}
			m_streamDef[vertType]=pDef;
		}
		return m_streamDef[vertType];
	}

	void RenderSysFixed::RenderSky(SceneGraph* pSG)
	{
		SGSkyNode* pSkyNode = pSG->GetSkyNode();
		if( NULL != pSkyNode )
		{
			for(UINT i=0;i<pSkyNode->GetNumRenderNode();i++)
			{
				RenderNode *pRNode=pSkyNode->GetRenderNode(i);
				pRNode->Draw(this,pSkyNode,0,NULL,pSG);
			}
		}
		RenderSun( pSG );
	}

	void RenderSysFixed::RenderSun(SceneGraph* pSG)
	{
		IDevice *pDev=Device();
		
		SGSunNode* pSunNode = pSG->GetSunNode();
		if( NULL == pSunNode )
			return;

		UINT nNumRenderNode = pSunNode->GetNumRenderNode();
		for(UINT i=0;i<nNumRenderNode;i++)
		{
			RenderNode *pRNode=pSunNode->GetRenderNode(i);
			pRNode->Draw(this,pSunNode,0,NULL,pSG);
		}
	}

	void RenderSysFixed::RenderSunFlare(SceneGraph* pSG)
	{
		SGSunNode* pSunNode = pSG->GetSunNode();
		if( NULL == pSunNode || !pSunNode->IsEnableFlare() )
			return;

		if( !pSunNode->IsInFrustum() )
			return;

		IDevice *pDev=Device();

		// 渲染光晕
		const int nNumChildren = pSunNode->GetNumChildren();
		for( int i = 0; i < nNumChildren; i++ )
		{
			const SceneNode* pNode = pSunNode->GetChild( i );
			if( pNode->IS_STATIC_CLASS(SGSunFlare) )
			{
				SGSunFlare* pSunFlare = (SGSunFlare*)pNode;

				// 遮挡查询
				if( pSunFlare->BeginVizQuery() )
				{
					StateBlock block;
					block.SaveState( ERS_ColorWriteEnable ); 
					pDev->SetRenderState( ERS_ColorWriteEnable, 0 );
					RenderSun( pSG );
					block.Apply();
					pSunFlare->EndVizQuery();
				}

				for(UINT j=0;j<pSunFlare->GetNumRenderNode();j++)
				{
					RenderNode *pRNode=pSunFlare->GetRenderNode(j);
					pRNode->Draw(this,pSunFlare,0,NULL,pSG);
				}
			}
		}
	}

	void RenderSysFixed::RenderReflectMap(SceneGraph *pSG,const vector<SceneNode*>& sceneNodes,IRenderTexture* pRT,float surfaceHeight,bool bZScale)
	{
		ASSERT(pSG);

		CameraBase* pCamera=pSG->GetCamera();
		IDevice* pDev=Device();

		//--设置矩阵
		pDev->MatPush(EMS_Prj);
		pDev->MatPush(EMS_View);
		pDev->MatPush(EMS_World);
		pDev->MatLoad(EMS_Prj,pCamera->GetPrj());
		pDev->MatLoad(EMS_View,pCamera->GetView());		
		pDev->FFlushMatrix(false,true,true,false);

		//--
		if(pRT->Begin())
		{
			pDev->Clear(true,true,true,0,1.0f,0);

			//--
			pDev->SetVertShader(NULL);
			pDev->SetPixelShader(NULL);

			//--分类
			vector<RenderItem> trnArray;
			vector<RenderItem> opaqueArray;
			vector<RenderItem> alphaTestArray;
			vector<RenderItem> alphaTestAndBlendArray;
			BuildRenderItems(pSG,sceneNodes,trnArray,opaqueArray,alphaTestArray,&alphaTestAndBlendArray);
	
			//--天空
			RenderSky(pSG);

			//--修改Viewport
			tagViewport oldViewport;
			if(bZScale)
			{
				pDev->GetViewport(oldViewport);
				tagViewport newViewport=oldViewport;
				newViewport.minZ=0;
				newViewport.maxZ=0.5f;
				pDev->SetViewport(&newViewport);
			}

			//--设置裁剪面
			Plane clipPlane(Vector3(0,surfaceHeight,0),MathConst::AxisY);
			pDev->SetClipPlane(&clipPlane);

			size_t size,i;

			m_matView=*(Device()->GetMatrix(EMS_View));
			m_matPrj=*(Device()->GetMatrix(EMS_Prj));

			//--地形
			pDev->MatLoad(EMS_World,&MathConst::IdentityMat);
			pDev->FFlushMatrix(true,false,false,false);

			TerrainPatchRender::m_renderMode=TerrainPatchRender::ERM_ReflectMap;
			SortByZVal(trnArray,true);
			size=trnArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=trnArray[i];
				RenderTerrainPatch(item,pSG);
			}

			//--不透明
			SortByMtlAndZVal(opaqueArray);
			size=opaqueArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=opaqueArray[i];
				RenderOne(item,pSG);
			}

			StateBlock block;
			block.SaveState(ERS_DepthTestEnable);
			block.SaveState(ERS_DepthWriteEnable);
			block.SaveState(ERS_AlphaBlendEnable);
			block.SaveState(ERS_SrcBlend);
			block.SaveState(ERS_DstBlend);
			block.SaveState(ERS_AlphaTestEnable);
			block.SaveState(ERS_AlphaTestFunc);
			block.SaveState(ERS_AlphaRef);

			//--渲染m_alphaTestAndBlendArray
			//--2pass半透明处理
			//--先渲染不透明部分
			SortByMtlAndZVal(alphaTestAndBlendArray);
			pDev->EnableAlphaBlend(false);
			pDev->EnableAlphaTest(true);
			pDev->SetAlphaTestFunc(ECMP_Greater);
			pDev->SetAlphaRef(240);
			size=alphaTestAndBlendArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=alphaTestAndBlendArray[i];
				RenderOne(item,pSG);
			}
			//--后渲染透明部分
			pDev->EnableDepthWrite(false);
			pDev->EnableAlphaBlend(true);
			pDev->SetAlphaTestFunc(ECMP_LessEqual);
			pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);
			size=alphaTestAndBlendArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=alphaTestAndBlendArray[i];
				RenderOne(item,pSG);
			}
			pDev->EnableDepthWrite(true);
			pDev->EnableAlphaBlend(false);


			//--渲染m_alphaTestArray
			pDev->EnableAlphaTest(true);
			pDev->SetAlphaRef(0);
			pDev->SetAlphaTestFunc(ECMP_Greater);
			SortByMtlAndZVal(alphaTestArray);
			size=alphaTestArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=alphaTestArray[i];
				RenderOne(item,pSG);
			}
			pDev->EnableAlphaTest(false);

			block.Apply();

			//--删除裁剪面
			pDev->SetClipPlane(NULL);

			//--恢复Viewport
			if(bZScale)
				pDev->SetViewport(&oldViewport);

			pRT->End();
		}

		//--恢复矩阵
		pDev->MatPop(EMS_Prj);
		pDev->MatPop(EMS_View);
		pDev->MatPop(EMS_World);
		pDev->FFlushMatrix(true,true,true,false);
	}

	void RenderSysFixed::RenderShadowMap( SceneGraph* pSG,const vector<SceneNode*>& sceneNodes,IRenderTexture* pRT )
	{
		ASSERT(pSG);

		IDevice* pDev=Device();

		CameraBase* pCamera=pSG->GetCamera();

		//--设置矩阵
		pDev->MatPush(EMS_Prj);
		pDev->MatPush(EMS_View);
		pDev->MatPush(EMS_World);
		pDev->MatPush(EMS_Tex0);
		pDev->MatPush(EMS_Tex1);
		pDev->MatLoad(EMS_Prj,pCamera->GetPrj());
		pDev->MatLoad(EMS_View,pCamera->GetView());
		pDev->FFlushMatrix(false,true,true,false);

		//--
		Matrix4 viewprjMat;
		D3DXMatrixMultiply(&viewprjMat,pCamera->GetView(),pCamera->GetPrj());
		if(pRT->Begin())
		{
			bool bDepth=pRT->HaveDepthBuffer();
			bool bStencil=bDepth&&pRT->HaveStencilBuffer();
			pDev->Clear(true,bDepth,bStencil,0,1.0f,0);

			//--
			pDev->SetVertShader(NULL);
			pDev->SetPixelShader(NULL);

			//--分类
			vector<RenderItem> trnArray;
			vector<RenderItem> opaqueArray;
			vector<RenderItem> alphaTestArray;
			BuildRenderItems(pSG,sceneNodes,trnArray,opaqueArray,alphaTestArray);

			size_t size,i;

			m_matView=*(Device()->GetMatrix(EMS_View));
			m_matPrj=*(Device()->GetMatrix(EMS_Prj));

			//--地形
			SortByZVal(trnArray,true);

			pDev->MatLoad(EMS_World,&MathConst::IdentityMat);
			pDev->FFlushMatrix(true,false,false,false);

			size=trnArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=trnArray[i];

				IMaterial *pMtl=item.pSGNode->GetMtl(item.pNode->GetMtlID());
				ASSERT(pMtl!=NULL&&pMtl->IsResourceReady());

				Matrix4 worldMat=item.pSGNode->GetWorldMat();
				Matrix4 worldviewprjMat;
				D3DXMatrixMultiply(&worldviewprjMat,&worldMat,&viewprjMat);
				RenderNode::m_pSpecialVS->SetConstValue("g_matWorldViewPrj",(void*)&worldviewprjMat,sizeof(Matrix4));

				RenderTerrainPatch(item,pSG);
			}

			//--不透明
			SortByMtlAndZVal(opaqueArray);
			size=opaqueArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=opaqueArray[i];

				Matrix4 worldMat=item.pSGNode->GetWorldMat();
				Matrix4 worldviewprjMat;
				D3DXMatrixMultiply(&worldviewprjMat,&worldMat,&viewprjMat);
				RenderNode::m_pSpecialVS->SetConstValue("g_matWorldViewPrj",(void*)&worldviewprjMat,sizeof(Matrix4));

				RenderOne(item,pSG);
			}

			//--渲染m_alphaTestArray
			SortByMtlAndZVal(alphaTestArray);
			size=alphaTestArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=alphaTestArray[i];

				Matrix4 worldMat=item.pSGNode->GetWorldMat();
				Matrix4 worldviewprjMat;
				D3DXMatrixMultiply(&worldviewprjMat,&worldMat,&viewprjMat);
				RenderNode::m_pSpecialVS->SetConstValue("g_matWorldViewPrj",(void*)&worldviewprjMat,sizeof(Matrix4));

				RenderOne(item,pSG);
			}

			pRT->End();
		}

		//--恢复矩阵
		pDev->MatPop(EMS_Prj);
		pDev->MatPop(EMS_View);
		pDev->MatPop(EMS_World);
		pDev->MatPop(EMS_Tex0);
		pDev->MatPop(EMS_Tex1);
		pDev->FFlushMatrix();
	}

	void RenderSysFixed::RenderCubeShadowMap( SceneGraph* pSG,const vector<SceneNode*>& sceneNodes,IRenderCubeTexture* pRT )
	{
		ASSERT(pSG);

		IDevice* pDev=Device();

		CameraBase* pCamera=pSG->GetCamera();

		//--设置矩阵
		pDev->MatPush(EMS_Prj);
		pDev->MatPush(EMS_View);
		pDev->MatPush(EMS_World);
		pDev->MatPush(EMS_Tex0);
		pDev->MatPush(EMS_Tex1);
		pDev->MatLoad(EMS_Prj,pCamera->GetPrj());
		pDev->MatLoad(EMS_View,pCamera->GetView());		
		pDev->FFlushMatrix(false,true,true,false);

		tagViewport oldViewport;
		pDev->GetViewport(oldViewport);

		//--
		if(pRT->Begin())
		{
			bool bDepth=pRT->HaveDepthBuffer();
			bool bStencil=bDepth&&pRT->HaveStencilBuffer();
			pDev->Clear(true,bDepth,bStencil,0,1.0f,0);

			//--
			pDev->SetVertShader(NULL);
			pDev->SetPixelShader(NULL);

			//--分类
			vector<RenderItem> trnArray;
			vector<RenderItem> opaqueArray;
			vector<RenderItem> alphaTestArray;
			BuildRenderItems(pSG,sceneNodes,trnArray,opaqueArray,alphaTestArray);

			size_t size,i;

			m_matView=*(Device()->GetMatrix(EMS_View));
			m_matPrj=*(Device()->GetMatrix(EMS_Prj));

			//--地形
			SortByZVal(trnArray,true);

			pDev->MatLoad(EMS_World,&MathConst::IdentityMat);
			pDev->FFlushMatrix(true,false,false,false);

			size=trnArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=trnArray[i];

				IMaterial *pMtl=item.pSGNode->GetMtl(item.pNode->GetMtlID());
				ASSERT(pMtl!=NULL&&pMtl->IsResourceReady());

				Matrix4 worldMat=item.pSGNode->GetWorldMat();
				RenderNode::m_pSpecialVS->SetConstValue("g_matWorld",(void*)&worldMat,sizeof(Matrix4));

				RenderTerrainPatch(item,pSG);
			}

			//--不透明
			SortByMtlAndZVal(opaqueArray);
			size=opaqueArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=opaqueArray[i];

				Matrix4 worldMat=item.pSGNode->GetWorldMat();
				RenderNode::m_pSpecialVS->SetConstValue("g_matWorld",(void*)&worldMat,sizeof(Matrix4));

				RenderOne(item,pSG);
			}

			//--渲染m_alphaTestArray
			SortByMtlAndZVal(alphaTestArray);
			size=alphaTestArray.size();
			for(i=0;i<size;i++)
			{
				RenderItem& item=alphaTestArray[i];

				Matrix4 worldMat=item.pSGNode->GetWorldMat();
				RenderNode::m_pSpecialVS->SetConstValue("g_matWorld",(void*)&worldMat,sizeof(Matrix4));

				RenderOne(item,pSG);
			}

			pRT->End();
		}

		//--恢复矩阵
		pDev->MatPop(EMS_Prj);
		pDev->MatPop(EMS_View);
		pDev->MatPop(EMS_World);
		pDev->MatPop(EMS_Tex0);
		pDev->MatPop(EMS_Tex1);
		pDev->FFlushMatrix();
	}

	void RenderSysFixed::BuildRenderItems(
			SceneGraph* pSG,
			const vector<SceneNode*>& sceneNodes,
			vector<RenderItem>& trnArray,
			vector<RenderItem>& opaqueArray,
			vector<RenderItem>& alphaTestArray,
			vector<RenderItem>* pAlphaTestAndBlendArray/*=NULL*/)
	{
		for(vector<SceneNode*>::const_iterator iter=sceneNodes.begin();
			iter!=sceneNodes.end();++iter)
		{
			SceneNode* pSGNode=(*iter);

			AnimationCtrl *pAniCtrl=GetAniCtrlFromSGNode(pSGNode);
			int numRenderNodes=pSGNode->GetNumRenderNode();
			for(int i=0;i<numRenderNodes;i++)
			{
				RenderNode *pRender=pSGNode->GetRenderNode(i);
				if(pRender==NULL)
					continue;
				int nLod=0;
				if(pRender->SupportLod())
					nLod=pSG->CalcDetailLevel(pRender->GetAABBox(),&pSGNode->GetWorldMat());

				RenderItem newItem;
				newItem.pSGNode=pSGNode;
				newItem.pNode=pRender;
				newItem.pAni=pAniCtrl;
				newItem.nLod=nLod;

				//--
				if(pRender->IS_STATIC_CLASS(TerrainPatchRender)
					||pRender->IS_STATIC_CLASS(RSimpleTerrain))
				{
					trnArray.push_back(newItem);
				}
				else if(pRender->GetMtlTrait(newItem.pSGNode)==EMT_Opacity)
				{
					opaqueArray.push_back(newItem);
				}
				else if(pRender->GetMtlTrait(newItem.pSGNode)==EMT_AlphaTest)
				{
					alphaTestArray.push_back(newItem);
				}
				else if(pRender->GetMtlTrait(newItem.pSGNode)==EMT_AlphaTestAndBlend&&NULL!=pAlphaTestAndBlendArray)
				{
					pAlphaTestAndBlendArray->push_back(newItem);
				}
			}//end for
		}//end for
	}
}//namespace Cool3D