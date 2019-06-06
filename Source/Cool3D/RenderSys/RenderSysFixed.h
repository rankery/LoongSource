#include "..\Device\IDevice.h"
#pragma once
#include "rendersys.h"
#include "..\Math\Math.h"

namespace Cool3D
{
	class IMaterial;
	class TerrainMtlMultiLayers;
	class IDeviceTex;
	class SGSkyNode;
	class SGEffectNode;

	/**	\class RenderSysFixed 
		\brief 基于Fixed pipeline渲染
		\par
	*/
	class RenderSysFixed : public RenderSys
	{
	public:
		RenderSysFixed(void);
		virtual ~RenderSysFixed(void);

		//--RenderSys
		virtual int PrepMtl(IMaterial *pMtl);
		virtual bool GetCapacity(ERenderSysCap cap);
		virtual void EnableFog(bool bEnable);
				
		virtual void Begin(SceneGraph *pSG);
		virtual void End(SceneGraph *pSG,bool bZScale=false);
		virtual void RenderSceneNode(CameraBase *pCamera,const SceneNode *pNode,IRenderTexture *pTarget,bool clearTarget,int lod,DWORD targetColor=0xFFFFFFFF,IRenderCallBack* pCallBack=NULL,SceneGraph* pSG=NULL,const tagViewport* pNewViewport=NULL);
		virtual void RenderRenderNode(CameraBase *pCamera,const SceneNode *pSceneNode,const RenderNode *pRenderNode,IRenderTexture *pTarget,bool clearTarget,int lod,DWORD targetColor=0xFFFFFFFF,IRenderCallBack* pCallBack=NULL,SceneGraph *pSG=NULL);
		virtual IStreamDefine* GetStreamDefine(EVertType vertType);

		virtual void RenderReflectMap(SceneGraph* pSG,const vector<SceneNode*>& sceneNodes,IRenderTexture* pRT,float surfaceHeight,bool bZScale=false);
		virtual void RenderShadowMap(SceneGraph* pSG,const vector<SceneNode*>& sceneNodes,IRenderTexture* pRT);
		virtual void RenderCubeShadowMap(SceneGraph* pSG,const vector<SceneNode*>& sceneNodes,IRenderCubeTexture* pRT);
	protected:
		//--RenderSys
		virtual void InitDevice();
	private:
		virtual void AddRenderItem(RenderItem& newItem);
		void SortByMtl(vector<RenderItem>& array);
		void SortByZVal(vector<RenderItem>& array,bool bNear2Far);
		void SortByMtlAndZVal(vector<RenderItem>& array);
		void RenderOne(RenderItem& item,SceneGraph* pSG);
			void RenderStaticNode(RenderItem& item,SceneGraph* pSG);
			void RenderKeyFrameNode(RenderItem& item,SceneGraph* pSG);
			void RenderTerrainPatch(RenderItem& item,SceneGraph* pSG);
		void RenderTrees();
		
		void RenderSGNodeR(const SceneNode *pNode,CameraBase *pCamera,int lod,IRenderCallBack* pCallBack=NULL,SceneGraph* pSG=NULL);
		void RenderSky(SceneGraph* pSG);
		void RenderSun(SceneGraph* pSG);
		void RenderSunFlare(SceneGraph* pSG);

		void BuildRenderItems(SceneGraph* pSG,
							const vector<SceneNode*>& sceneNodes,
							vector<RenderItem>& trnArray,
							vector<RenderItem>& opaqueArray,
							vector<RenderItem>& alphaTestArray,
							vector<RenderItem>* pAlphaTestAndBlendArray=NULL);

	private:
		bool				m_bLightEnable;//是否使用光照
		UINT				m_renderCount;
		Matrix4				m_matView;
		Matrix4				m_matPrj;
		vector<RenderItem>	m_trnArray;
		vector<RenderItem>  m_grassArray;
		vector<RenderItem>	m_opaqueArray;
		vector<RenderItem>	m_alphaTestArray;
		vector<RenderItem>	m_alphaBlendArray;
		vector<RenderItem>	m_alphaBlendDisableZArray;
		vector<RenderItem>	m_alphaBlendDisableZWriteArray;
		vector<RenderItem>	m_alphaTestAndBlendArray;
		
		IStreamDefine*		m_streamDef[EVType_Num];

		// 为后期处理准备的包含两个RenderTexture的交换链
		class RTChain;
		RTChain				*m_pRTChain;

		DECL_RTTI_DYNC(RenderSysFixed);
	};
}//namespace Cool3D