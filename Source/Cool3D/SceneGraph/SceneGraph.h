#pragma once
#include "..\Cool3D.h"
#include "..\Math\Camerabase.h"
#include "..\Device\Light.h"
#include "..\Device\FogDefine.h"
#include "..\Util\RTTIObj.h"
#include "..\Math\GeomDefine.h"

class IFS;
namespace Cool3D
{
	class SGEffectNode;
	class SGSkyNode;
	class SceneNode;
	class DynamicMesh;
	class SGSunNode;
	class IRenderTexture;
	class TerrainPatch;
	
	/**	\class SceneGraph
		\brief 负责场景节点的存储，可见性剪裁，碰撞检测, 将节点提交到RenderSys
		\see RenderSys
		\todo 外部如果需要使用多个SceneGraph,那需要改动Kernel和this->Camera功能
	*/
	class Cool3D_API SceneGraph : public RTTIObj
	{
	public:
		enum { MAX_LOD=10 };
		SceneGraph(void);
		virtual ~SceneGraph(void);

		static SceneGraph* NewSceneGraph(const TCHAR* szClassName);
		virtual void LoadFromFile(IFS *pFS,const TCHAR* szMapName) =0;

		virtual void AttachDynamicNode(SceneNode *pNode) =0;
		/** If you detach the nod from scene graph,you need DELETE it yourself*/
		virtual void DetachDynamicNode(const SceneNode *pNode) =0;

		/**	使用视锥剪裁,遮挡剪裁等找到需要渲染的node,然后update它们,然后渲染它们
		*/
		virtual void Render(float deltaTime) =0;
		virtual void BuildProjectorMesh(const tagLight *pLight,const AABBox& box,DynamicMesh *pMesh) =0;
		virtual void BuildDecalMesh( const AABBox& box,DynamicMesh *pMesh, const Vector3* pNormal ) =0;

		/*
		virtual void AddSceneEffect() =0;//rain,snow,fog,sky
		*/

		int CalcDetailLevel(const AABBox& box,const Matrix4* pWorldMat=NULL);
		virtual void BindCamera(CameraBase *pCamera,const bool bSetDefaultFogDist=true);
		CameraBase* GetCamera() const			{ return m_pCamera;        }
		
		void SetSunLight(const tagLight& lgt)	{	m_sunLight=lgt;        }
		const tagLight& GetSunLight() const		{	return m_sunLight;	   }
		virtual void SetSceneFog(const tagDistFog& fog)	{	m_sceneFog=fog;}
		const tagDistFog& GetSceneFog() const	{	return m_sceneFog;     }
		void SwitchSceneFog( const tagDistFog& fog );
		void SetSceneFogFadeTime( const float& time ) { m_fFogFadeTime = time; }

		//!	设置天空节点,对象的生存期由this管理
		void SetSkyNode(SGSkyNode *pNode);
		SGSkyNode* GetSkyNode() const;

		//! 设置太阳结点，对象的生存期由this管理
		void SetSunNode(SGSunNode *pNode);
		SGSunNode* GetSunNode() const;

		/** 资源加载完成后，让所有SceneNodes进行同步初始化操作
		*/
		virtual void SyncInit(){};
		bool IsSyncInit()	{ return m_bSyncInit;}
		void BeginSyncInit() { m_bSyncInit=true;}
		void EndSyncInit()	{ m_bSyncInit=false;}

		/** 渲染反射贴图
		*/
		virtual void RenderReflectMap(CameraBase* pCamera,IRenderTexture* pRT,float surfaceHeight,bool bRenderTrnOnly){};

	private:
		void UpdataSceneFogFade();
	protected:
		tagDistFog		m_sceneFog;
		tagLight		m_sunLight;
		float			m_sunModulus;
		CameraBase		*m_pCamera;
		float			m_lodDistSq[MAX_LOD];
		SGSkyNode		*m_pSkyNode;//因为SkyNode在RenderSys中必须特殊处理,所以单独管理
		SGSunNode		*m_pSunNode;//SunNode在RendSys需特殊处理
		bool			m_bSyncInit;

		bool            m_bFogFading;           //雾是否正在过渡
		float           m_fFogSwitchTime;       //雾切换的时间
		float           m_fFogFadeTime;         //雾过渡的时间长度
		tagDistFog      m_lastFog;				//上次的雾
		tagDistFog      m_nextFog;				//切换的目标雾

		DECL_RTTI(SceneGraph);
	};
}//namespace Cool3D