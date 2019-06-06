#pragma once
#include "mtlrendercache.h"
#include "DevPixelStageFixed.h"

namespace Cool3D
{
	class IDeviceTex;
	class IMaterial;
	class MtlCombiner;
	class MBitmap;
	class MRenderTex;
	class MConstColor;
	class IRenderMtl;
	class MGeneral;
	class MStatic;
	class MKeyFrame;
	class MSkin;
	class IMtlModifier;
	class MMUVTrans;
	class MMFrameBlend;
	class MPreCombineBitmap;
	class MTwoLayers;
	class MWaterReflect;
	class MWaterRefract;
	class MWater;
	class TerrainMtlMultiLayers;
	class TerrainMtlTwoLayers;
	class TerrainMtlPrecombine;
	class MDyeTex;
	class MSEMTex;
	class MCEMTex;
	class TEditorMtl;
	class TRiverEditorMtl;
	class SimpleTerrainMtl;
	class IVertShader;
	class IPixelShader;

	/**	\class MtlRenderCacheFixed
		\brief MtlRenderCache for RenderSysFixed
	*/
	class MtlRenderCacheFixed :	public MtlRenderCache
	{
	public:
		MtlRenderCacheFixed(void);
		virtual ~MtlRenderCacheFixed(void);

		//--MtlRenderCache
		virtual void CompileMaterial(IMaterial *pMtl);
		virtual void UpdateMtlMod(IMtlModifier *pMod);
		virtual void Begin(IVertShader* pVS=NULL,IPixelShader* pPS=NULL);
		virtual int GetNumPass() const;
		virtual bool BeginPass(int i);
		virtual void EndPass(bool clearTex=true);
		virtual void UpdateMtl(IMaterial* pMtl);
		virtual EMtlTrait GetMtlTrait();

	private:
		void CompileTerrainMtl(TerrainMtlMultiLayers *pMtl);
		void CompileTerrainMtlTwoLayers(TerrainMtlTwoLayers *pMtl);
		void CompileTerrainMtlPrecombine(TerrainMtlPrecombine *pMtl);
		void CompileTEditorMtl(TEditorMtl* pMtl);
		void CompileTRiverEditorMtl(TRiverEditorMtl *pMtl);
		void CompileSimpleTerrainMtl(SimpleTerrainMtl* pMtl);
		void CompileGeneral(MGeneral *pMtl);
		void CompileStatic(MStatic *pMtl);
		void CompileKeyFrame(MKeyFrame *pMtl);
		void CompileSkin(MSkin *pMtl);
		void CompilePreCombineBitmap(MPreCombineBitmap *pMtl);
		void CompileTwoLayers(MTwoLayers *pMtl);
		void CompileWaterReflect(MWaterReflect *pMtl);
		void CompileWaterRefract(MWaterRefract *pMtl);
		void CompileWater(MWater* pMtl);
		void CompileDyeTex(MDyeTex* pMtl);
		void CompileSEMTex(MSEMTex* pMtl);
		void CompileCEMTex(MCEMTex* pMtl);
		void CompileMtlModifier(tagTexStage& stage,IMaterial *pMtl);
			void AddModifier(tagTexStage& stage,const IMtlModifier *pMod);
				void AddUVTransModifier(tagTexStage& stage,const MMUVTrans *pUVTrans);
				void AddFrameBlend(const MMFrameBlend *pBlend);

		void CompileCombinerR(MtlCombiner *pCombiner,MtlCombiner *pFather);
			/**	
				\return 是否有新的Texture stage
			*/
			bool RenderMtl2Stage(tagTexStage& newStage,IRenderMtl *pMtl);
				void BitmapMtl2Stage(tagTexStage& stage,MBitmap *pMtl);
				void RenderTex2Stage(tagTexStage& stage,MRenderTex *pMtl);
				void ConstcolorMtl(MConstColor *pMtl);

		void AddStage(const tagTexStage& newStage);
		void AddRenderState(ERStateType type,DWORD val);
		DevPixelStageFixed* GetCurPass();
		bool CheckRemainStage(const tagTexStage& newStage);//是否还有texture stage可用

		void UpdateWaterReflect(MWaterReflect* pMtl);
		void UpdateWaterRefract(MWaterRefract* pMtl);
		void UpdateWater(MWater* pMtl);
		void UpdateEditorMtl(TEditorMtl* pMtl);
		void UpdateStaticMtl(MStatic* pMtl);
		void UpdateGeneralMtl(MGeneral* pMtl);

	private:
		vector<DevPixelStageFixed*>	m_passes;//pass数据
		DevPixelStageFixed*			m_pCurPass;//begin pass所调用的pass
		IVertShader*				m_pVS;//vertex shader
		IPixelShader*				m_pPS;//pixel shader
	};
}//namespace Cool3D