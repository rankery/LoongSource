#include "StdAfx.h"
#include ".\mtlrendercachefixed.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Util\Exception.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\IRenderMtl.h"
#include "..\RenderMtl\IMtlModifier.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\RenderMtl\MRenderTex.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\RenderMtl\MtlCombiner.h"
#include "..\RenderMtl\MMUVTrans.h"
#include "..\RenderMtl\MMUVPage.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\RenderMtl\MMAlphaMask.h"
#include "..\RenderMtl\MMDepthFrame.h"
#include "..\RenderMtl\MMColorWriteControl.h"
#include "..\RenderMtl\MMColor.h"
#include "..\RenderMtl\MMTransparent.h"
#include "..\RenderMtl\MMUsingMtlColor.h"
#include "..\RenderMtl\MPreCombineBitmap.h"
#include "..\RenderMtl\MTwoLayers.h"
#include "..\RenderMtl\MWaterReflect.h"
#include "..\RenderMtl\MWaterRefract.h"
#include "..\RenderMtl\MWater.h"
#include "..\RenderMtl\MDyeTex.h"
#include "..\RenderMtl\MSEMTex.h"
#include "..\RenderMtl\MSky.h"
#include "..\EditorSupport\TEditorMtl.h"
#include "..\EditorSupport\TRiverEditorMtl.h"
#include "..\RenderMtl\MStatic.h"
#include "..\RenderMtl\MKeyFrame.h"
#include "..\RenderMtl\MSkin.h"
#include "..\terrain\TerrainMtlMultiLayers.h"
#include "..\terrain\TerrainPatchRes.h"
#include "..\terrain\TerrainMtlTwoLayers.h"
#include "..\terrain\TerrainMtlPrecombine.h"
#include "..\terrain\SimpleTerrainMtl.h"
#include "..\ResSys\ResTexture.h"
#include "..\ResSys\ResVertShader.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\EditorSupport\ResDynaAlphaMap.h"
#include "..\Util\StlExt.h"

namespace Cool3D
{
	MtlRenderCacheFixed::MtlRenderCacheFixed(void)
	{
		m_pCurPass=NULL;
		m_pVS=NULL;
		m_pPS=NULL;
	}

	MtlRenderCacheFixed::~MtlRenderCacheFixed(void)
	{
		FreePtrVector<DevPixelStageFixed> freePasses;
		freePasses(m_passes);
	}

	void MtlRenderCacheFixed::Begin(IVertShader* pVS,IPixelShader* pPS)
	{
		IDevice* pDev=Device();
		if (pVS)
			pDev->SetVertShader(pVS);
		else
			pDev->SetVertShader(m_pVS);
		if (pPS)
			pDev->SetPixelShader(pPS);
		else
			pDev->SetPixelShader(m_pPS);

		MtlRenderCache::Begin();
	}

	int MtlRenderCacheFixed::GetNumPass() const
	{
		return m_passes.size();
	}

	bool MtlRenderCacheFixed::BeginPass(int i)
	{
		if(i<0 || i>=(int)m_passes.size())
			return false;
		m_pCurPass=m_passes[i];
		for(int n=0;n<(int)m_pCurPass->m_stages.size();n++)
		{
			if(m_pCurPass->m_stages[n].uvTransType!=EUVT_Disable)
			{
				Device()->MatPush(EMatrixStackType(EMS_Tex0+n));
			}
		}
		m_pCurPass->Set2Device();
		return true;
	}

	void MtlRenderCacheFixed::EndPass(bool clearTex)
	{
		if(m_pCurPass!=NULL)
		{
			int numStage=m_pCurPass->m_stages.size();
			tagTexStage defaultStage;
			for(int i=0;i<numStage;i++)
				DevPixelStageFixed::SetStage2Device(i,defaultStage,clearTex);
			DWORD texIndex=0;
			for(int i=0;i<numStage;i++)
			{
				if(m_pCurPass->m_stages[i].uvTransType!=EUVT_Disable)
				{
					Device()->MatPop(EMatrixStackType(EMS_Tex0+i));
					texIndex|=TexStage2MatrixIndex(i);
				}
			}
			if(texIndex!=0)
			{
				Device()->FFlushMatrix(false,false,false,true,texIndex);
			}
			m_pCurPass=NULL;
		}
	}

	EMtlTrait MtlRenderCacheFixed::GetMtlTrait()
	{
		bool bAlphaTest=false;
		bool bAlphaBlend=false;
		bool bDisableZWrite=false;
		bool bDisableZTest=false;
		bool bFixTrans=false;

		for(int i=0;i<(int)m_passes.size();i++)
		{
			DevPixelStageFixed* pPass=m_passes[i];

			if(pPass->m_bMultiPass)
			{
				bFixTrans=true;
				break;
			}

			vector<DevPixelStageFixed::RenderState>& states=pPass->m_states;
			for(int j=0;j<(int)states.size();j++)
			{
				DevPixelStageFixed::RenderState& state=states[j];
				if(state.first==ERS_AlphaTestEnable
					&&state.second!=0)
					bAlphaTest=true;
				if(state.first==ERS_AlphaBlendEnable
					&&state.second!=0)
					bAlphaBlend=true;
				if(state.first==ERS_DepthTestEnable 
					&&state.second==0)
					bDisableZTest=true;
				if(state.first==ERS_DepthWriteEnable
					&&state.second==0)
					bDisableZWrite=true;
			}
		}

		if(bFixTrans)
			return EMT_AlphaTestAndBlend;
		if(bAlphaBlend&&bDisableZWrite&&bDisableZTest)
			return EMT_AlphaBlendDisableZ;
		if(bAlphaBlend&&bDisableZWrite&&!bDisableZTest)
			return EMT_AlphaBlendDisableZWrite;
		if(bAlphaBlend&&!bDisableZWrite&&!bDisableZTest)
			return EMT_AlphaBlend;
		if(bAlphaTest)
			return EMT_AlphaTest;
		return EMT_Opacity;
	}

	void MtlRenderCacheFixed::CompileMaterial(IMaterial *pMtl)
	{
		ASSERT(pMtl!=NULL);

		ASSERT(m_passes.size() == 0);//no content

		m_passes.push_back(new DevPixelStageFixed);//1 pass at least
		//--
		if(pMtl->IS_STATIC_CLASS(TerrainMtlMultiLayers))
			CompileTerrainMtl((TerrainMtlMultiLayers*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(SimpleTerrainMtl))
			CompileSimpleTerrainMtl((SimpleTerrainMtl*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(TerrainMtlTwoLayers))
			CompileTerrainMtlTwoLayers((TerrainMtlTwoLayers*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(TerrainMtlPrecombine))
			CompileTerrainMtlPrecombine((TerrainMtlPrecombine*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MGeneral))
			CompileGeneral((MGeneral*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MStatic))
			CompileStatic((MStatic*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MKeyFrame))
			CompileKeyFrame((MKeyFrame*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MSkin))
			CompileSkin((MSkin*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MtlCombiner))
			CompileCombinerR((MtlCombiner*)pMtl,NULL);
		else if(pMtl->IS_STATIC_CLASS(MPreCombineBitmap))
			CompilePreCombineBitmap((MPreCombineBitmap*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MTwoLayers))
			CompileTwoLayers((MTwoLayers*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MWaterRefract))
			CompileWaterRefract((MWaterRefract*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MWaterReflect))
			CompileWaterReflect((MWaterReflect*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MWater))
			CompileWater((MWater*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MDyeTex))
			CompileDyeTex((MDyeTex*)pMtl);
		//--球形环境映射
		else if(pMtl->IS_STATIC_CLASS(MSEMTex))
			CompileSEMTex((MSEMTex*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(TEditorMtl))
			CompileTEditorMtl((TEditorMtl*)pMtl);
		//--河流材质
		else if(pMtl->IS_STATIC_CLASS(TRiverEditorMtl))
			CompileTRiverEditorMtl((TRiverEditorMtl*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MConstColor))
		{
			tagTexStage newStage;
			RenderMtl2Stage(newStage,(MConstColor*)pMtl);
			newStage.texOp=ETOP_SelectArg1;
			newStage.texArg1=ETA_Diffuse;
			newStage.alphaOp=ETOP_SelectArg1;
			newStage.alphaArg1=ETA_Diffuse;
			AddStage(newStage);
		}
		else if(pMtl->IS_STATIC_CLASS(MSky))
		{
			tagTexStage newStage;
			RenderMtl2Stage(newStage,(IRenderMtl*)pMtl);
			newStage.texArg1=ETA_Texture;
			newStage.texOp=ETOP_SelectArg1;
			newStage.alphaOp=ETOP_Disable;
			AddStage(newStage);
		}
		else if(pMtl->IS_KIND_OF(IRenderMtl))
		{
			tagTexStage newStage;
			RenderMtl2Stage(newStage,(IRenderMtl*)pMtl);
			newStage.texArg1=ETA_Texture;
			newStage.texArg2=ETA_Diffuse;
			if(newStage.pTex==NULL && newStage.pRTex==NULL)
			{
				newStage.texOp=ETOP_SelectArg2;
				newStage.alphaOp=ETOP_SelectArg2;
			}
			else
			{
				newStage.texOp=ETOP_Mul;
				newStage.alphaOp=ETOP_Mul;
			}
			newStage.alphaArg1=ETA_Texture;
			newStage.alphaArg2=ETA_Diffuse;

			AddStage(newStage);
		}
		else
			ASSERT(0);

		//--编译非IRenderMtl材质的修改器
		if(m_passes.size()>0
			&& !pMtl->IS_KIND_OF(IRenderMtl))
		{
			TexStageArray& stages=m_passes[0]->m_stages;
			if(stages.size()>0)
				CompileMtlModifier(stages[0],pMtl);
		}
	}

	void MtlRenderCacheFixed::CompileGeneral(MGeneral *pMtl)
	{
		ASSERT(pMtl!=NULL);
		
		if(pMtl->m_pDiffuse==NULL)//没有Diffuse贴图就不处理更多贴图
			return;

		ConstcolorMtl(pMtl->m_pColor);

		if(pMtl->m_pLightMap!=NULL)
		{
			//--diffuse
			tagTexStage diffStage;
			diffStage.texOp=ETOP_SelectArg1;
			diffStage.texArg1=ETA_Texture;
			diffStage.alphaOp=ETOP_SelectArg1;
			diffStage.alphaArg1=ETA_Texture;

			RenderMtl2Stage(diffStage,pMtl->m_pDiffuse);
			diffStage.uvIndex=pMtl->m_uvIndexDiffuse;
			AddStage(diffStage);

			//--light
			tagTexStage lightStage;
			lightStage.texOp=ETOP_Mul2x;
			lightStage.texArg1=ETA_Texture;
			lightStage.texArg2=ETA_Current;

			lightStage.alphaOp=ETOP_SelectArg1;
			lightStage.alphaArg1=ETA_Current;

			RenderMtl2Stage(lightStage,pMtl->m_pLightMap);
			lightStage.uvIndex=pMtl->m_uvIndexLightMap;
			AddStage(lightStage);
		}
		else
		{
			tagTexStage diffStage;
			diffStage.texOp=ETOP_Mul2x;
			diffStage.texArg1=ETA_Texture;
			diffStage.texArg2=ETA_Diffuse;

			diffStage.alphaOp=ETOP_Mul;
			diffStage.alphaArg1=ETA_Texture;
			diffStage.alphaArg2=ETA_Diffuse;
			RenderMtl2Stage(diffStage,pMtl->m_pDiffuse);
			diffStage.uvIndex=pMtl->m_uvIndexDiffuse;
			AddStage(diffStage);
		}
	}

	void MtlRenderCacheFixed::CompileStatic(MStatic *pMtl)
	{
		ASSERT(pMtl!=NULL);
		
		if(pMtl->m_shader!=MStatic::ES_None
			&& pMtl->m_pBump!=NULL)
		{
			ConstcolorMtl(pMtl->m_pColor);

			if(pMtl->m_pDiffuse==NULL)//没有Diffuse贴图就不处理更多贴图
				return;

			m_pVS=pMtl->m_pVS[pMtl->m_shader]->GetVS();
			m_pPS=pMtl->m_pPS[pMtl->m_shader]->GetPS();

			//--diffuse
			tagTexStage diffStage;
			RenderMtl2Stage(diffStage,pMtl->m_pDiffuse);
			if(m_pVS!=NULL)
				diffStage.bAutoUVIndex=true;
			else
				diffStage.uvIndex=pMtl->m_uvIndexDiffuse;
			AddStage(diffStage);

			//--bump
			tagTexStage bumpStage;
			RenderMtl2Stage(bumpStage,pMtl->m_pBump);
			if(m_pVS!=NULL)
				bumpStage.bAutoUVIndex=true;
			else
				bumpStage.uvIndex=pMtl->m_uvIndexDiffuse;
			AddStage(bumpStage);
		}
		else
		{
			CompileGeneral(pMtl);
		}
	}

	void MtlRenderCacheFixed::CompileKeyFrame(MKeyFrame *pMtl)
	{
		ASSERT(pMtl!=NULL);

		if(pMtl->m_shader!=MKeyFrame::ES_None
			&& pMtl->m_pBump!=NULL)
		{
			ConstcolorMtl(pMtl->m_pColor);

			if(pMtl->m_pDiffuse==NULL)//没有Diffuse贴图就不处理更多贴图
				return;

			m_pVS=pMtl->m_pVS[pMtl->m_shader]->GetVS();
			m_pPS=pMtl->m_pPS[pMtl->m_shader]->GetPS();

			//--diffuse
			tagTexStage diffStage;
			RenderMtl2Stage(diffStage,pMtl->m_pDiffuse);
			if(m_pVS!=NULL)
				diffStage.bAutoUVIndex=true;
			else
				diffStage.uvIndex=pMtl->m_uvIndexDiffuse;
			AddStage(diffStage);

			//--bump
			tagTexStage bumpStage;
			RenderMtl2Stage(bumpStage,pMtl->m_pBump);
			if(m_pVS!=NULL)
				bumpStage.bAutoUVIndex=true;
			else
				bumpStage.uvIndex=pMtl->m_uvIndexDiffuse;
			AddStage(bumpStage);
		}
		else
		{
			CompileGeneral(pMtl);
		}
	}

	void MtlRenderCacheFixed::CompileSkin(MSkin *pMtl)
	{
		ASSERT(pMtl!=NULL);

		if(pMtl->m_pDiffuse==NULL)//没有Diffuse贴图就不处理更多贴图
			return;

		ConstcolorMtl(pMtl->m_pColor);
		if(pMtl->m_shader!=MSkin::ES_None)
		{
			//--shader
			m_pVS=NULL;
			m_pPS=NULL;

			if(pMtl->m_pVS[pMtl->m_shader]!=NULL)
				m_pVS=pMtl->m_pVS[pMtl->m_shader]->GetVS();
			if(pMtl->m_pPS[pMtl->m_shader]!=NULL)
				m_pPS=pMtl->m_pPS[pMtl->m_shader]->GetPS();
		}

		if(pMtl->m_shader==MSkin::ES_Bump_Direct_Point1_Skin
			||pMtl->m_shader==MSkin::ES_Bump_Direct_Skin)
		{
			//--diffuse
			tagTexStage diffStage;
			RenderMtl2Stage(diffStage,pMtl->m_pDiffuse);
			if(m_pVS!=NULL)
				diffStage.bAutoUVIndex=true;
			else
				diffStage.uvIndex=pMtl->m_uvIndexDiffuse;
			AddStage(diffStage);

			//--bump
			ASSERT(pMtl->m_pBump!=NULL);
			tagTexStage bumpStage;
			RenderMtl2Stage(bumpStage,pMtl->m_pBump);
			if(m_pVS!=NULL)
				bumpStage.bAutoUVIndex=true;
			else
				bumpStage.uvIndex=pMtl->m_uvIndexDiffuse;
			AddStage(bumpStage);
		}
		else
		{
			if(NULL==pMtl->m_pBlendLayer)
			{
				//--diffuse
				tagTexStage diffStage;
				diffStage.texOp=ETOP_Mul2x;
				diffStage.texArg1=ETA_Texture;
				diffStage.texArg2=ETA_Diffuse;

				diffStage.alphaOp=ETOP_Mul;
				diffStage.alphaArg1=ETA_Texture;
				diffStage.alphaArg2=ETA_Diffuse;
				RenderMtl2Stage(diffStage,pMtl->m_pDiffuse);
				if(m_pVS!=NULL)
					diffStage.bAutoUVIndex=true;
				else
					diffStage.uvIndex=pMtl->m_uvIndexDiffuse;
				AddStage(diffStage);
			}
			else
			{
				//-- 选择Diffuse
				tagTexStage newStage0;
				newStage0.texOp=ETOP_SelectArg1;
				newStage0.texArg1=ETA_Texture;
				newStage0.alphaArg1=ETA_Texture;
				newStage0.alphaOp=ETOP_SelectArg1;
				RenderMtl2Stage(newStage0,pMtl->m_pDiffuse);
				if(m_pVS!=NULL)
					newStage0.bAutoUVIndex=true;
				else
					newStage0.uvIndex=pMtl->m_uvIndexDiffuse;
				AddStage(newStage0);

				//-- 混合BlendLayer
				tagTexStage newStage1;
				newStage1.texOp=ETOP_BlendTextureAlpha;
				newStage1.texArg1=ETA_Texture;
				newStage1.texArg2=ETA_Current;
				newStage1.alphaOp=ETOP_Disable;
				newStage0.alphaArg1=ETA_Current;
				newStage0.alphaOp=ETOP_SelectArg1;
				RenderMtl2Stage(newStage1,pMtl->m_pBlendLayer);
				if(m_pVS!=NULL)
					newStage1.bAutoUVIndex=true;
				else
					newStage1.uvIndex=pMtl->m_uvIndexDiffuse;
				AddStage(newStage1);

				//-- 光照处理
				tagTexStage newStage2;
				newStage2.texOp=ETOP_Mul2x;
				newStage2.texArg1=ETA_Diffuse;
				newStage2.texArg2=ETA_Current;
				newStage2.alphaOp=ETOP_Mul;
				newStage2.alphaArg1=ETA_Current;
				newStage2.alphaArg2=ETA_Diffuse;
				newStage2.bAutoUVIndex=true;
				AddStage(newStage2);
			}
		}
	}


	void MtlRenderCacheFixed::CompilePreCombineBitmap(MPreCombineBitmap *pMtl)
	{
		tagTexStage newStage;
		RenderMtl2Stage(newStage,pMtl);

		newStage.texOp=ETOP_Mul2x;
		newStage.texArg1=ETA_Texture;
		newStage.texArg2=ETA_Diffuse;

		newStage.alphaOp=ETOP_SelectArg1;
		newStage.alphaArg1=ETA_Texture;
		newStage.texArg2=ETA_Diffuse;

		AddStage(newStage);
	}

	void MtlRenderCacheFixed::CompileMtlModifier(tagTexStage& stage,IMaterial *pMtl)
	{
		if(pMtl==NULL)
			return;
		UINT numMod=pMtl->GetNumMods();
		for(UINT i=0;i<numMod;i++)
		{
			AddModifier(stage,pMtl->GetModifier(i));
		}
	}

	void MtlRenderCacheFixed::AddModifier(tagTexStage& stage,const IMtlModifier *pMod)
	{
		if(pMod==NULL)
			return;
		if(pMod->IS_STATIC_CLASS(MMUVTrans)
			||pMod->IS_STATIC_CLASS(MMUVPage))
			AddUVTransModifier(stage,static_cast<const MMUVTrans*>(pMod));
		else if(pMod->IS_STATIC_CLASS(MMFrameBlend))
			AddFrameBlend(static_cast<const MMFrameBlend*>(pMod));
		else if(pMod->IS_STATIC_CLASS(MMAlphaMask))
		{
			const MMAlphaMask *pAlphaMask=static_cast<const MMAlphaMask*>(pMod);
			AddRenderState(ERS_AlphaTestEnable,true);
			AddRenderState(ERS_AlphaTestFunc,pAlphaMask->m_alphaTestFunc);
			AddRenderState(ERS_AlphaRef,pAlphaMask->m_alphaRef);
		}
		else if(pMod->IS_STATIC_CLASS(MMDepthFrame))
		{
			const MMDepthFrame* pDepth=static_cast<const MMDepthFrame*>(pMod);
			AddRenderState(ERS_DepthWriteEnable,pDepth->m_depthWriteEnable);
			AddRenderState(ERS_DepthTestEnable,pDepth->m_depthTestEnable);
			AddRenderState(ERS_DepthFunc,pDepth->m_depthFunc);
		}
		else if(pMod->IS_STATIC_CLASS(MMColorWriteControl))
		{
			const MMColorWriteControl *pCC=static_cast<const MMColorWriteControl*>(pMod);
			AddRenderState(ERS_ColorWriteEnable,pCC->m_dwColorMask);
		}
		else if(pMod->IS_STATIC_CLASS(MMUsingMtlColor))
		{
			const MMUsingMtlColor *pMM=static_cast<const MMUsingMtlColor*>(pMod);
			AddRenderState(ERS_TextureFactor,Color4ub(pMM->m_diffuse));
			AddRenderState(ERS_FLightEnable,false);

			if(m_passes.size()>0)
			{
				DevPixelStageFixed *pPass0=m_passes[0];
				if(pPass0->m_stages.size()>0)
				{
					tagTexStage& stage0=pPass0->m_stages[0];
					stage0.texOp=ETOP_Mul;
					stage0.texArg1=ETA_Texture;
					stage0.texArg2=ETA_TFactor;

					stage0.alphaOp=ETOP_Mul;
					stage0.alphaArg1=ETA_Texture;
					stage0.alphaArg2=ETA_TFactor;
				}
			}
		}
	}
	
	void MtlRenderCacheFixed::AddFrameBlend(const MMFrameBlend *pBlend)
	{
		AddRenderState(ERS_AlphaBlendEnable,pBlend->m_blendEnable);
		AddRenderState(ERS_SrcBlend,pBlend->m_src);
		AddRenderState(ERS_DstBlend,pBlend->m_dst);

		if(m_passes.size()>0)
		{
			DevPixelStageFixed *pPass0=m_passes[0];
			pPass0->m_bMultiPass=pBlend->m_bMultiPass;
		}
	}

	void MtlRenderCacheFixed::AddUVTransModifier(tagTexStage& stage,const MMUVTrans *pUVTrans)
	{
		stage.uvTrans.CombineTrans(pUVTrans->GetMatrix());
		stage.uvTransType=EUVT_Count2;
	}

	void MtlRenderCacheFixed::CompileCombinerR(MtlCombiner *pCombiner,MtlCombiner *pFather)
	{
		if(pCombiner==NULL)
			return;
		const IMaterial *mtl=pCombiner->m_pMtl1;
		if(mtl!=NULL && mtl->IS_STATIC_CLASS(MtlCombiner))
				CompileCombinerR((MtlCombiner*)mtl,pCombiner);
		
		bool new1=false;
		tagTexStage newStage;
		if(mtl!=NULL)
		{
			ASSERT(mtl->IS_KIND_OF(IRenderMtl));//如果不是Combiner,就必须是render mtl
			new1=RenderMtl2Stage(newStage,(IRenderMtl*)mtl);
		}

		const IMaterial *mtl2=pCombiner->m_pMtl2;
		if(mtl2!=NULL && mtl2->IS_STATIC_CLASS(MtlCombiner))
				CompileCombinerR((MtlCombiner*)mtl2,pCombiner);

		bool new2=false;
		tagTexStage newStage2;
		if(mtl2!=NULL)
		{
			ASSERT(mtl2->IS_KIND_OF(IRenderMtl));//如果不是Combiner,就必须是render mtl
			new2=RenderMtl2Stage(newStage2,(IRenderMtl*)mtl2);
		}

		if(new1 && new2)//两个都是texture
		{
			DevPixelStageFixed *pCurPass=GetCurPass();
			if(pFather!=NULL && pCurPass->m_stages.size()>0)
			{
				newStage.texOp=pFather->m_colorOp;
				newStage.texArg1=ETA_Texture;
				newStage.texArg2=ETA_Current;
				newStage.alphaOp=pFather->m_alphaOp;
				newStage.alphaArg1=ETA_Texture;
				newStage.alphaArg2=ETA_Current;
			}
			else
			{
				newStage.texOp=ETOP_Mul2x;
				newStage.texArg1=ETA_Texture;
				newStage.texArg2=ETA_Diffuse;
				newStage.alphaOp=ETOP_Mul;
				newStage.alphaArg1=ETA_Texture;
				newStage.alphaArg2=ETA_Diffuse;
			}
			newStage2.texOp=pCombiner->m_colorOp;
			newStage2.texArg1=ETA_Texture;
			newStage2.texArg2=ETA_Current;
			newStage2.alphaOp=pCombiner->m_alphaOp;
			newStage2.alphaArg1=ETA_Texture;
			newStage2.alphaArg2=ETA_Current;
			AddStage(newStage);
			AddStage(newStage2);
		}
		else if(new1 || new2)
		{
			//一个是颜色值,一个是贴图,则Combiner只是确定颜色与贴图的计算关系
			tagTexStage *pStage=NULL;
			if(new1)
				pStage=&newStage;
			else
				pStage=&newStage2;
			pStage->texOp=pCombiner->m_colorOp;
			pStage->texArg1=ETA_Texture;
			pStage->texArg2=ETA_Diffuse;
			pStage->alphaOp=pCombiner->m_alphaOp;
			pStage->alphaArg1=ETA_Texture;
			pStage->alphaArg2=ETA_Diffuse;
			AddStage(*pStage);
		}
		else
		{
			//两个都是颜色值,则使用第二个颜色值,替换了第一个颜色值
		}
	}
	bool MtlRenderCacheFixed::RenderMtl2Stage(tagTexStage& newStage,IRenderMtl *pMtl)
	{
		if(pMtl==NULL)
			return false;

		AddRenderState(ERS_FLightEnable,pMtl->m_bEnableLight);
		if(pMtl->m_bTwoSide)
			AddRenderState(ERS_CullMode,ECull_None);

		for(UINT i=0;i<pMtl->GetNumMods();i++)
			AddModifier(newStage,pMtl->GetModifier(i));

		if(pMtl->IS_KIND_OF(MBitmap))
		{
			BitmapMtl2Stage(newStage,(MBitmap*)pMtl);
			return true;
		}//bitmap mtl
		else if(pMtl->IS_STATIC_CLASS(MConstColor))
		{
			ConstcolorMtl((MConstColor*)pMtl);
			return false;
		}
		else if(pMtl->IS_STATIC_CLASS(MRenderTex))
		{
			RenderTex2Stage(newStage,(MRenderTex*)pMtl);
			return true;
		}
		else
			ASSERT(0);
		return false;
	}

	void MtlRenderCacheFixed::ConstcolorMtl(MConstColor *pMtl)
	{
		if(pMtl==NULL)
			return;
		DevPixelStageFixed *pCurPass=GetCurPass();
		pCurPass->m_color=pMtl->m_color;

		if(pMtl->m_bUsingVertColor)
			AddRenderState(ERS_FLightEnable,false);
	}

	void MtlRenderCacheFixed::RenderTex2Stage(tagTexStage& stage,MRenderTex *pMtl)
	{
		if(pMtl==NULL)
			return;
		stage.pRTex=pMtl->m_pRTex;
		stage.magFilter=pMtl->m_magFilter;
		stage.minFilter=pMtl->m_minFilter;
		stage.mipFilter=pMtl->m_mipFilter;
		stage.addressU=pMtl->m_addressU;
		stage.addressV=pMtl->m_addressV;
		stage.addressW=pMtl->m_addressW;
		//--tmp
		AddRenderState(ERS_FLightEnable,false);
	}

	void MtlRenderCacheFixed::BitmapMtl2Stage(tagTexStage& stage,MBitmap *pMtl)
	{
		if(pMtl==NULL)
			return;
		stage.pTex=pMtl->m_pBitmap->GetDeviceTex();
		stage.magFilter=pMtl->m_magFilter;
		stage.minFilter=pMtl->m_minFilter;
		stage.mipFilter=pMtl->m_mipFilter;
		stage.addressU=pMtl->m_addressU;
		stage.addressV=pMtl->m_addressV;
		stage.addressW=pMtl->m_addressW;
	}
	
	void MtlRenderCacheFixed::CompileTerrainMtl(TerrainMtlMultiLayers *pMtl)
	{
		ASSERT(pMtl!=NULL);
		ASSERT(pMtl->m_layers.size()>=1);//one layer at least

		ConstcolorMtl(&pMtl->m_color);

		int numLayers=(int)pMtl->m_layers.size();
		if(numLayers>4)
			numLayers=4;

		m_pPS=pMtl->m_pPS->GetPS();

		int i;
		for(i=0;i<numLayers;i++)
		{
			TerrainMtlMultiLayers::tagLayer *pLayer=&pMtl->m_layers[i];

			tagTexStage diffStage;
			RenderMtl2Stage(diffStage,pLayer->m_pTileTex);
			diffStage.texOp=ETOP_Disable;
			diffStage.alphaOp=ETOP_Disable;
			diffStage.uvIndex=pLayer->uvSetIndex;
			AddStage(diffStage);
		}

		if(pMtl->m_pAlphaMap!=NULL && numLayers>1)
		{
			tagTexStage alphaStage;
			BitmapMtl2Stage(alphaStage,pMtl->m_pAlphaMap);
			alphaStage.texOp=ETOP_Disable;
			alphaStage.alphaOp= ETOP_Disable;
			alphaStage.uvIndex=TRN_MAX_UV_SET-1;
			AddStage(alphaStage);
		}

		if(pMtl->m_pLightMap!=NULL)
		{
			tagTexStage lightStage;
			BitmapMtl2Stage(lightStage,pMtl->m_pLightMap);
			lightStage.texOp=ETOP_Disable;
			lightStage.alphaOp= ETOP_Disable;
			lightStage.uvIndex=TRN_MAX_UV_SET-1;
			AddStage(lightStage);
		}
	}

	void MtlRenderCacheFixed::CompileTEditorMtl(TEditorMtl *pMtl)
	{
		ASSERT(pMtl!=NULL);

		ConstcolorMtl(&pMtl->m_color);

		if(pMtl->m_pAlphaMap==NULL)
		{
			tagTexStage stage;
			BitmapMtl2Stage(stage,pMtl->m_pTileTex);
			stage.texOp=ETOP_Mul2x;
			stage.texArg1=ETA_Texture;
			stage.texArg2=ETA_Diffuse;
			stage.alphaOp=ETOP_SelectArg1;
			stage.alphaArg1=ETA_Texture;
			stage.uvIndex=pMtl->m_uvSetIndex;
			AddStage(stage);
		}
		else
		{
			AddRenderState(ERS_AlphaBlendEnable,true);
			AddRenderState(ERS_SrcBlend,EBlend_SrcAlpha);
			AddRenderState(ERS_DstBlend,EBlend_InvSrcAlpha);

			tagTexStage alphaStage;
			tagTexStage diffStage;
			BitmapMtl2Stage(diffStage,pMtl->m_pTileTex);

			alphaStage.texOp=ETOP_SelectArg1;
			alphaStage.texArg1=ETA_Current;
			alphaStage.alphaOp=ETOP_SelectArg1;
			alphaStage.alphaArg1=ETA_Texture;
			alphaStage.addressU=ETAddress_Clamp;
			alphaStage.addressV=ETAddress_Clamp;
			alphaStage.addressW=ETAddress_Clamp;
			alphaStage.magFilter=ETFilter_LINEAR;
			alphaStage.minFilter=ETFilter_LINEAR;
			alphaStage.mipFilter=ETFilter_NONE;
			alphaStage.pTex=pMtl->m_pAlphaMap->GetDeviceTex();
			alphaStage.uvIndex=TRN_MAX_UV_SET-1;

			diffStage.texOp=ETOP_Mul2x;
			diffStage.texArg1=ETA_Texture;
			diffStage.texArg2=ETA_Diffuse;
			diffStage.alphaOp=ETOP_SelectArg1;
			diffStage.alphaArg1=ETA_Current;
			diffStage.uvIndex=pMtl->m_uvSetIndex;

			AddStage(alphaStage);
			AddStage(diffStage);
		}
	}

	void MtlRenderCacheFixed::CompileTRiverEditorMtl(TRiverEditorMtl *pMtl)
	{
		ASSERT(pMtl!=NULL);

		ConstcolorMtl(&pMtl->m_color);

		if(pMtl->m_pMap==NULL)
		{
			tagTexStage stage;
			RenderMtl2Stage(stage,pMtl->m_pTex);
			stage.texOp=ETOP_Mul2x;
			stage.texArg1=ETA_Texture;
			stage.texArg2=ETA_Diffuse;
			stage.alphaOp=ETOP_Disable;
			stage.uvIndex=0;
			AddStage(stage);
		}
		else
		{
			AddRenderState(ERS_AlphaBlendEnable,true);
			AddRenderState(ERS_SrcBlend,EBlend_SrcAlpha);
			AddRenderState(ERS_DstBlend,EBlend_InvSrcAlpha);

			tagTexStage alphaStage;
			tagTexStage diffStage;

			Color4f color(pMtl->m_alpha,1,1,1);
			AddRenderState(ERS_TextureFactor,Color4ub(color));

			alphaStage.texOp=ETOP_SelectArg1;
			alphaStage.texArg1=ETA_Current;
			alphaStage.alphaOp=ETOP_Mul;
			alphaStage.alphaArg1=ETA_Texture;
			alphaStage.alphaArg2=ETA_TFactor;
			alphaStage.addressU=ETAddress_Clamp;
			alphaStage.addressV=ETAddress_Clamp;
			alphaStage.addressW=ETAddress_Clamp;
			alphaStage.magFilter=ETFilter_LINEAR;
			alphaStage.minFilter=ETFilter_LINEAR;
			alphaStage.mipFilter=ETFilter_POINT;
			alphaStage.pTex=pMtl->m_pMap->GetDeviceTex();
			alphaStage.uvIndex=0;
			AddStage(alphaStage);

			diffStage.texOp=ETOP_Mul2x;
			diffStage.texArg1=ETA_Texture;
			diffStage.texArg2=ETA_Diffuse;
			diffStage.alphaOp=ETOP_SelectArg1;
			diffStage.alphaArg1=ETA_Current;
			diffStage.uvIndex=0;
			RenderMtl2Stage(diffStage,pMtl->m_pTex);
			AddStage(diffStage);
		}
	}

	void MtlRenderCacheFixed::CompileTwoLayers(MTwoLayers* pMtl)
	{
		ASSERT(pMtl!=NULL);
		ASSERT(pMtl->m_pLayer0!=NULL&&pMtl->m_pLayer1!=NULL);

		if(pMtl->m_pColor!=NULL)
			ConstcolorMtl(pMtl->m_pColor);

		//-- 选择layer0
		tagTexStage newStage0;
		newStage0.texOp=ETOP_SelectArg1;
		newStage0.texArg1=ETA_Texture;
		newStage0.alphaOp=ETOP_Disable;
		newStage0.uvIndex=0;
		RenderMtl2Stage(newStage0,pMtl->m_pLayer0);
		AddStage(newStage0);

		//-- 混合layer1
		tagTexStage newStage1;
		newStage1.texOp=ETOP_BlendTextureAlpha;
		newStage1.texArg1=ETA_Texture;
		newStage1.texArg2=ETA_Current;
		newStage1.alphaOp=ETOP_Disable;
		newStage1.uvIndex=0;
		RenderMtl2Stage(newStage1,pMtl->m_pLayer1);
		AddStage(newStage1);

		//-- 光照处理
		tagTexStage newStage2;
		newStage2.texOp=ETOP_Mul2x;
		newStage2.texArg1=ETA_Diffuse;
		newStage2.texArg2=ETA_Current;
		newStage2.alphaOp=ETOP_Disable;
		AddStage(newStage2);
	}

	void MtlRenderCacheFixed::CompileTerrainMtlTwoLayers(TerrainMtlTwoLayers* pMtl)
	{
		ASSERT(pMtl!=NULL);
		ASSERT(pMtl->m_pDetailTex!=NULL&&pMtl->m_pCombineTex!=NULL);

		ConstcolorMtl(&pMtl->m_color);

		//--
		tagTexStage newStage0;
		newStage0.texOp=ETOP_SelectArg1;
		newStage0.texArg1=ETA_Texture;
		newStage0.texArg2=ETA_Diffuse;
		newStage0.alphaOp=ETOP_Disable;
		newStage0.uvIndex=0;
		RenderMtl2Stage(newStage0,pMtl->m_pDetailTex);
		AddStage(newStage0);

		//--
		tagTexStage newStage1;
		newStage1.texOp=ETOP_Mul2x;
		newStage1.texArg1=ETA_Texture;
		newStage1.texArg2=ETA_Current;
		newStage1.alphaOp=ETOP_Disable;
		newStage1.uvIndex=1;
		RenderMtl2Stage(newStage1,pMtl->m_pCombineTex);
		AddStage(newStage1);
	}

	void MtlRenderCacheFixed::CompileTerrainMtlPrecombine(TerrainMtlPrecombine *pMtl)
	{
		ASSERT(pMtl!=NULL);
		ASSERT(pMtl->m_pPrecombineTex!=NULL);

		ConstcolorMtl(&pMtl->m_color);

		if(pMtl->m_pPS==NULL)
		{
			AddRenderState(ERS_TextureFactor,Color4ub(255,255,255,255));

			tagTexStage diffStage;
			diffStage.texOp=ETOP_Mul2x;
			diffStage.texArg1=ETA_Texture;
			diffStage.texArg2=ETA_TFactor;
			RenderMtl2Stage(diffStage,pMtl->m_pPrecombineTex);
			diffStage.uvIndex=1;
			AddStage(diffStage);
		}
		else
		{
			tagTexStage diffStage;
			RenderMtl2Stage(diffStage,pMtl->m_pPrecombineTex);
			diffStage.uvIndex=1;
			AddStage(diffStage);

			m_pPS=pMtl->m_pPS->GetPS();
		}
	}

	void MtlRenderCacheFixed::CompileWaterReflect(MWaterReflect* pMtl)
	{
		ASSERT(pMtl!=NULL);
		ASSERT(pMtl->m_pReflectMap!=NULL && pMtl->m_pBumpMap!=NULL);
		ASSERT(pMtl->m_pAlphaMap!=NULL);

		AddRenderState(ERS_AlphaBlendEnable,true);
		AddRenderState(ERS_SrcBlend,EBlend_SrcAlpha);
		AddRenderState(ERS_DstBlend,EBlend_InvSrcAlpha);

		tagTexStage stageBump;
		stageBump.bAutoUVIndex=true;
		RenderMtl2Stage(stageBump,pMtl->m_pBumpMap);
		AddStage(stageBump);

		tagTexStage stageAlpha;
		stageAlpha.bAutoUVIndex=true;
		RenderMtl2Stage(stageAlpha,pMtl->m_pAlphaMap);
		AddStage(stageAlpha);

		tagTexStage stageReflect;
		stageReflect.bAutoUVIndex=true;
		RenderMtl2Stage(stageReflect,pMtl->m_pReflectMap);
		AddStage(stageReflect);
	}

	void MtlRenderCacheFixed::UpdateWaterReflect(MWaterReflect* pMtl)
	{
		ASSERT(pMtl->m_pReflectMap!=NULL 
			&& pMtl->m_pBumpMap!=NULL );

		if(m_passes.empty())
			return;

		DevPixelStageFixed* pPass=m_passes.front();
		ASSERT(pPass->m_stages.size()==3);

		//bump map
		pPass->m_stages[0].pTex=pMtl->m_pBumpMap->m_pBitmap->GetDeviceTex();

		//alpha map
		pPass->m_stages[1].pTex=pMtl->m_pAlphaMap->m_pBitmap->GetDeviceTex();

		//reflect map
		pPass->m_stages[2].pRTex=pMtl->m_pReflectMap->m_pRTex;
	}

	void MtlRenderCacheFixed::CompileWaterRefract(MWaterRefract* pMtl)
	{
		ASSERT(pMtl!=NULL);
		ASSERT(pMtl->m_pReflectMap!=NULL && pMtl->m_pRefractMap!=NULL && pMtl->m_pBumpMap!=NULL );
		ASSERT(pMtl->m_pAlphaMap!=NULL);

		AddRenderState(ERS_AlphaBlendEnable,true);
		AddRenderState(ERS_SrcBlend,EBlend_SrcAlpha);
		AddRenderState(ERS_DstBlend,EBlend_InvSrcAlpha);

		tagTexStage stageBump;
		stageBump.bAutoUVIndex=true;
		RenderMtl2Stage(stageBump,pMtl->m_pBumpMap);
		AddStage(stageBump);

		tagTexStage stageAlpha;
		stageAlpha.bAutoUVIndex=true;
		RenderMtl2Stage(stageAlpha,pMtl->m_pAlphaMap);
		AddStage(stageAlpha);

		tagTexStage stageReflect;
		stageReflect.bAutoUVIndex=true;
		RenderMtl2Stage(stageReflect,pMtl->m_pReflectMap);
		AddStage(stageReflect);

		tagTexStage stageRefract;
		stageRefract.bAutoUVIndex=true;
		RenderMtl2Stage(stageRefract,pMtl->m_pRefractMap);
		AddStage(stageRefract);
	}

	void MtlRenderCacheFixed::UpdateWaterRefract(MWaterRefract* pMtl)
	{
		ASSERT(pMtl->m_pReflectMap!=NULL && pMtl->m_pRefractMap!=NULL && pMtl->m_pBumpMap!=NULL );

		if(m_passes.empty())
			return;

		DevPixelStageFixed* pPass=m_passes.front();
		ASSERT(pPass->m_stages.size()==4);

		//bump map
		pPass->m_stages[0].pTex=pMtl->m_pBumpMap->m_pBitmap->GetDeviceTex();

		//alpha map
		pPass->m_stages[1].pTex=pMtl->m_pAlphaMap->m_pBitmap->GetDeviceTex();

		//reflect map
		pPass->m_stages[2].pRTex=pMtl->m_pReflectMap->m_pRTex;

		//refract map
		pPass->m_stages[3].pRTex=pMtl->m_pRefractMap->m_pRTex;
	}

	void MtlRenderCacheFixed::CompileWater(MWater* pMtl)
	{
		ASSERT(pMtl->m_pFrames!=NULL);
		ASSERT(pMtl->m_pAlphaMap!=NULL);

		AddRenderState(ERS_AlphaBlendEnable,true);
		AddRenderState(ERS_SrcBlend,EBlend_SrcAlpha);
		AddRenderState(ERS_DstBlend,EBlend_InvSrcAlpha);

		ConstcolorMtl(&pMtl->m_color);	

		if(m_pPS!=NULL)
		{
			tagTexStage stageFrames;
			stageFrames.uvIndex=0;
			RenderMtl2Stage(stageFrames,pMtl->m_pFrames);
			AddStage(stageFrames);

			tagTexStage stageAlpha;
			stageAlpha.uvIndex=1;
			RenderMtl2Stage(stageAlpha,pMtl->m_pAlphaMap);
			AddStage(stageAlpha);
		}
		else
		{
			tagTexStage stageFrames;
			stageFrames.texOp=ETOP_SelectArg1;
			stageFrames.texArg1=ETA_Texture;
			stageFrames.alphaOp=ETOP_SelectArg1;
			stageFrames.alphaArg1=ETA_Current;
			stageFrames.uvIndex=0;
			RenderMtl2Stage(stageFrames,pMtl->m_pFrames);
			AddStage(stageFrames);

			tagTexStage stageAlpha;
			stageAlpha.texOp=ETOP_SelectArg1;
			stageAlpha.texArg1=ETA_Current;
			stageAlpha.alphaOp=ETOP_SelectArg1;
			stageAlpha.alphaArg1=ETA_Texture;
			stageAlpha.uvIndex=1;
			RenderMtl2Stage(stageAlpha,pMtl->m_pAlphaMap);
			AddStage(stageAlpha);
		}

	}

	void MtlRenderCacheFixed::UpdateWater(MWater* pMtl)
	{
		ASSERT(pMtl->m_pFrames!=NULL 
			&& pMtl->m_pAlphaMap!=NULL );

		if(m_passes.empty())
			return;

		DevPixelStageFixed* pPass=m_passes.front();
		ASSERT(pPass->m_stages.size()==2);

		//alpha map
		pPass->m_stages[1].pTex=pMtl->m_pAlphaMap->m_pBitmap->GetDeviceTex();
	}

	void MtlRenderCacheFixed::UpdateEditorMtl(TEditorMtl* pMtl)
	{
		ASSERT(pMtl);

		DevPixelStageFixed* pPass=m_passes.front();
		pPass->m_color=pMtl->m_color.m_color;
	}

	void MtlRenderCacheFixed::UpdateStaticMtl( MStatic* pMtl )
	{
		ASSERT(pMtl);

		DevPixelStageFixed* pPass=m_passes.front();
		MGeneral *pMG=(MGeneral*)pMtl;
		pPass->m_color=pMG->m_pColor->m_color;
	}

	void MtlRenderCacheFixed::UpdateGeneralMtl( MGeneral* pMtl )
	{
		ASSERT(pMtl);

		DevPixelStageFixed* pPass=m_passes.front();
		MGeneral *pMG=(MGeneral*)pMtl;
		pPass->m_color=pMG->m_pColor->m_color;
	}

	bool MtlRenderCacheFixed::CheckRemainStage(const tagTexStage& newStage)
	{
		DevPixelStageFixed *pCurPass=GetCurPass();
		IDevice *pDev=Device();

		if((int)pCurPass->m_stages.size()+1 > pDev->CapsMaxTextureStages())
			return false;

		if(pCurPass->CalcTexUsed(newStage) > pDev->FCapsMaxSimultaneousTextures())
			return false;

		return true;
	}

	void MtlRenderCacheFixed::AddStage(const tagTexStage& newStage)
	{
		DevPixelStageFixed *pCurPass=GetCurPass();
		if( !CheckRemainStage(newStage) )
		{
			pCurPass=new DevPixelStageFixed;
			m_passes.push_back(pCurPass);
		}
		pCurPass->AddStage(newStage);
	}
	
	void MtlRenderCacheFixed::AddRenderState(ERStateType type,DWORD val)
	{
		m_influenceState.insert(type);
		DevPixelStageFixed::RenderState s;
		s.first=type;
		s.second=val;
		DevPixelStageFixed *pCurPass=GetCurPass();

		//--删除已有的
		for(vector<DevPixelStageFixed::RenderState>::iterator iter=pCurPass->m_states.begin();
			iter!=pCurPass->m_states.end();)
		{
			if(iter->first==type)
			{
				iter=pCurPass->m_states.erase(iter);
			}
			else
				++iter;
		}

		pCurPass->m_states.push_back(s);
	}

	DevPixelStageFixed* MtlRenderCacheFixed::GetCurPass()
	{
		ASSERT(m_passes.size() >= 1);//have 1 pass at least
		return m_passes[m_passes.size()-1];
	}

	void MtlRenderCacheFixed::UpdateMtlMod(IMtlModifier *pMod)
	{
		if(pMod==NULL)
			return;
		if(pMod->IS_STATIC_CLASS(MMUVPage))
		{
			MMUVPage *pMM=(MMUVPage*)pMod;
			if(m_passes.size()>0)
			{
				DevPixelStageFixed *pPass0=m_passes[0];
				for(int i=0;i<(int)pPass0->m_stages.size();i++)
				{
					tagTexStage& stage=pPass0->m_stages[i];
					if(stage.uvTransType!=EUVT_Disable)
						stage.uvTrans.LoadMatrix(pMM->GetMatrix());
				}
			}
		}
		else if(pMod->IS_STATIC_CLASS(MMColor))
		{
			MMColor *pMM=(MMColor*)pMod;
			if(m_passes.size()>0)
			{
				DevPixelStageFixed *pPass0=m_passes[0];
				pPass0->m_color.ambient.R=pMM->m_ambient.x;
				pPass0->m_color.ambient.G=pMM->m_ambient.y;
				pPass0->m_color.ambient.B=pMM->m_ambient.z;

				pPass0->m_color.diffuse.R=pMM->m_diffuse.x;
				pPass0->m_color.diffuse.G=pMM->m_diffuse.y;
				pPass0->m_color.diffuse.B=pMM->m_diffuse.z;
			}
		}
		else if(pMod->IS_STATIC_CLASS(MMUsingMtlColor))
		{
			MMUsingMtlColor* pMM=(MMUsingMtlColor*)pMod;
			Color4ub diffuse(pMM->m_diffuse);

			if(m_passes.size()>0)
			{
				DevPixelStageFixed *pPass0=m_passes[0];
				pPass0->ModifyState(ERS_TextureFactor,diffuse);
			}
		}
		else
		{
			ASSERT(0&&"Mtl mod can't update");
		}
	}

	void MtlRenderCacheFixed::UpdateMtl(IMaterial* pMtl)
	{
		ASSERT(pMtl);

		if(pMtl->IS_STATIC_CLASS(MWaterRefract))
			UpdateWaterRefract((MWaterRefract*)pMtl);
		if(pMtl->IS_STATIC_CLASS(MWaterReflect))
			UpdateWaterReflect((MWaterReflect*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MWater))
			UpdateWater((MWater*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(TEditorMtl))
			UpdateEditorMtl((TEditorMtl*)pMtl);
		else if(pMtl->IS_STATIC_CLASS(MStatic))
			UpdateStaticMtl((MStatic*)pMtl);
		else if(pMtl->IS_KIND_OF(MGeneral))
			UpdateGeneralMtl((MGeneral*)pMtl);
	}

	void MtlRenderCacheFixed::CompileDyeTex(MDyeTex* pMtl)
	{
		ASSERT(pMtl!=NULL);
		ASSERT(pMtl->m_pAlphaMap!=NULL&&pMtl->m_pDiffuseMap!=NULL);

		if(pMtl->m_pColor!=NULL)
			ConstcolorMtl(pMtl->m_pColor);

		//--alpha map stage
		AddRenderState(ERS_TextureFactor,Color4ub(pMtl->m_dyeColor));
		tagTexStage alphaStage;
		alphaStage.texOp=ETOP_BlendTextureAlpha;
		alphaStage.texArg1=ETA_TFactor;
		alphaStage.texArg2=ETA_Texture;
		alphaStage.alphaOp=ETOP_SelectArg1;
		alphaStage.alphaArg1=ETA_Current;
		alphaStage.uvIndex=0;
		RenderMtl2Stage(alphaStage,pMtl->m_pAlphaMap);
		AddStage(alphaStage);

		//--diffuse map stage
		tagTexStage diffStage;
		diffStage.texOp=ETOP_Mul;
		diffStage.texArg1=ETA_Texture;
		diffStage.texArg2=ETA_Current;
		diffStage.alphaOp=ETOP_SelectArg1;
		diffStage.alphaArg1=ETA_Texture;
		diffStage.uvIndex=0;
		RenderMtl2Stage(diffStage,pMtl->m_pDiffuseMap);
		AddStage(diffStage);

		//--light stage
		tagTexStage lightStage;
		lightStage.texOp=ETOP_Mul2x;
		lightStage.texArg1=ETA_Diffuse;
		lightStage.texArg2=ETA_Current;
		lightStage.alphaOp=ETOP_Mul;
		lightStage.alphaArg1=ETA_Current;
		lightStage.alphaArg2=ETA_Diffuse;
		AddStage(lightStage);
	}

	void MtlRenderCacheFixed::CompileSEMTex(MSEMTex* pMtl)
	{
		ASSERT(pMtl!=NULL);
		ASSERT(pMtl->m_pEnvMap!=NULL&&pMtl->m_pBaseMap!=NULL);

		//--base map stage
		tagTexStage firstStage;
		firstStage.texOp=ETOP_Mul;
		firstStage.texArg1=ETA_Texture;
		firstStage.texArg2=ETA_Current;
		firstStage.alphaOp=ETOP_SelectArg1;
		firstStage.alphaArg1=ETA_Texture;
		firstStage.uvIndex=0;
		RenderMtl2Stage(firstStage,pMtl->m_pBaseMap);
		AddStage(firstStage);

		//--environment map stage
		tagTexStage secondStage;
		secondStage.texOp=ETOP_Mul2x;
		secondStage.texArg1=ETA_Texture;
		secondStage.texArg2=ETA_Current;
		secondStage.alphaOp=ETOP_SelectArg1;
		secondStage.alphaArg1=ETA_Current;
		secondStage.uvIndex=1;
		secondStage.bAutoUVIndex=false;
		//secondStage.uvTransType=EUVT_Count2;
		secondStage.uvIndexFlags=EUVI_SphereMap;
		RenderMtl2Stage(secondStage,pMtl->m_pEnvMap);
		AddStage(secondStage);
	}

	void MtlRenderCacheFixed::CompileSimpleTerrainMtl(SimpleTerrainMtl* pMtl)
	{
		ASSERT(pMtl!=NULL);

		AddRenderState(ERS_TextureFactor,pMtl->m_color);
		AddRenderState(ERS_FLightEnable,false);

		tagTexStage stage;
		stage.texOp=ETOP_SelectArg1;
		stage.texArg1=ETA_TFactor;
		stage.alphaOp=ETOP_Disable;
		stage.uvIndex=0;
		AddStage(stage);
	}
}//namespace Cool3D