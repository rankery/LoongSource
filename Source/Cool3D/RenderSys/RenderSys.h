#pragma once
#include "..\Cool3D.h"
#include "..\Math\Math.h"
#include "..\Math\GeomDefine.h"
#include "..\Util\RTTIObj.h"
#include "..\Public\vertexdefine.h"
#include "..\Device\IDevice.h"

namespace Cool3D
{
	class CameraBase;
	class SceneGraph;
	class RenderNode;
	class SceneNode;
	class SGLightNode;

	class ResStaticMesh;
	class IVertBuffer;
	class IIndexBuffer;
	class IRenderTexture;

	class AnimationCtrl;
	class IStreamDefine;
	class IMaterial;
	class PostProcess;
	class IRenderCallBack;

	class PostProcessHeatHaze;
	class PostProcessBlur;

	enum EDrawFlags
	{
		EDrawFlag_OccluderAlpha=1,	//因为遮挡住里视线(camera from->lookAt之间的线段)而自动画成半透明的
	};

	const int RMax_Light=4;//单个RenderNode可以有几个灯光
	struct RenderItem
	{
		SceneNode		*pSGNode;
		RenderNode		*pNode;
		AnimationCtrl	*pAni;
		int				nLod;
		DWORD			drawFlags;//是否画半透明等等
		tagLight*		pLights[RMax_Light];
		float			zVal;//for sort
		float			lightModulus[RMax_Light];

		RenderItem()
		{
			memset(this,0,sizeof(RenderItem));
		}
	};

	enum ERenderSysCap
	{
		ERender_ProjectTexture,		//使用Vertex Shader自动生成projective texture坐标
		ERender_ShaderMatrixPal,	//使用矩阵调色板计算骨骼动画
	};
	/** \class RenderSys
		\brief 提供给SceneGraph的高层接口，负责渲染, 渲染相关的Device细节,渲染相关的计算（例如关键帧插值）
		\remarks 
		\par Projection和View矩阵从Device取得
		\par 创建多种RenderSys的主要目的是针对不同显卡的技术进行优化,主要是所支持的Shader的不同
		\par 关键帧插值交给RenderSys主要是考虑到VertexShader可以做这部分工作;
	*/
	class Cool3D_API RenderSys : public RTTIObj
	{
	public:
		RenderSys(void);
		virtual ~RenderSys(void);

		//--
		/**	build mtl render cache if necessary;
			\return num of passes
		*/
		virtual int PrepMtl(IMaterial *pMtl) =0;
		virtual bool GetCapacity(ERenderSysCap cap) =0;
		virtual void EnableFog(bool bEnable) =0;
		/**	将scene graph计算出来的lod数字缩放为node的lod数字
			\remraks scene graph是按照一个MAX_LOAD数字计算出来的,而某个Node的max lod可能与scenegraph的不同
		*/
		static int	 ScaleSGLod(int sgLod,int nodeMaxLod);

		virtual void Begin(SceneGraph *pSG);
		virtual void AttachNode(const SceneNode *pNode);
		virtual void End(SceneGraph *pSG,bool bZScale=false);
		/**	渲染单个的scene node, 无需在begin(),end()中间调用.*/
		virtual void RenderSceneNode(CameraBase *pCamera,const SceneNode *pNode,IRenderTexture *pTarget,bool clearTarget,int lod,DWORD targetColor=0xFFFFFFFF,IRenderCallBack* pCallBack=NULL,SceneGraph* pSG=NULL,const tagViewport* pNewViewport=NULL) =0;

		/**	渲染单个的rendernode，无需在begin()，end()中间调用
		*/
		virtual void RenderRenderNode(CameraBase *pCamera,const SceneNode *pSceneNode,const RenderNode *pRenderNode,IRenderTexture *pTarget,bool clearTarget,int lod,DWORD targetColor=0xFFFFFFFF,IRenderCallBack* pCallBack=NULL,SceneGraph *pSG=NULL) =0;

		virtual IStreamDefine* GetStreamDefine(EVertType vertType) =0;
		/**	从内部的Pool中得到一个VertBuffer,适合于那些使用动态VB做渲染的应用
			\remarks 注意:外部必须使用VertStream拷贝到VB,否则VB无法得到正确的stride!!!!
		*/
		IVertBuffer* AllocVB(UINT sizeInBytes);
		/**	将一个从内部Pool分配的VertBuffer归还给Pool
		*/
		void FreeVB(IVertBuffer* pVB);
		/** 从内部Pool得到一个IndexBuffer
		*/
		IIndexBuffer* AllocIB(UINT sizeInBytes);
		/** 将一个内部Pool分配的IndexBuffer归还给Pool
		*/
		void FreeIB(IIndexBuffer* pIB);

		/** 添加一个后期处理对象
			\remarks this管理生存期 
		*/
		void AddPostProcess(PostProcess* pPostProcess);
		/** 删除一个后期处理对象
		*/
		void DelPostProcess(PostProcess* pPostProcess);
		/** 删除所有后期处理对象
		*/
		void ClearAllPostProcess();

		/** 渲染反射贴图
		*/
		virtual void RenderReflectMap(SceneGraph* pSG,const vector<SceneNode*>& sceneNodes,IRenderTexture* pRT,float surfaceHeight,bool bZScale=false){}

		/** 渲染ShadowMap
		*/
		virtual void RenderShadowMap(SceneGraph* pSG,const vector<SceneNode*>& sceneNodes,IRenderTexture* pRT){}
		virtual void RenderCubeShadowMap(SceneGraph* pSG,const vector<SceneNode*>& sceneNodes,IRenderCubeTexture* pRT) {}

		/** 后期处理开关，关闭则不进行所有后期处理渲染（但不删除后处理对象，默认为打开）
		*/
		void EnablePostProcess( const bool bEnable ) { m_bEnablePostProcess = bEnable; }
		bool IsEnablePostProcess() const { return m_bEnablePostProcess; }
	public:
		static tagRenderColor	WhiteRenderColor;
		void IgnoreSGLights(bool ignore)	{	m_bIgnoreSGLights=ignore;}
		PostProcessHeatHaze* GetPostProcessHeatHaze() { return m_pPostProcessHeatHaze; }
		bool EnablePostProcessBlur( const bool bEnalbe );
	protected:
		friend class Kernel;//只允许Kernel调用这个函数
		virtual void InitDevice();//派生类需要调用父类的此函数

		void AssignLights(const SceneNode* pNode,vector<SGLightNode*>& lgts);
		AnimationCtrl *GetAniCtrlFromSGNode(const SceneNode *pNode);
		
		void BuildRenderItems(SceneGraph *pSG);
		virtual void AddRenderItem(RenderItem& newItem) =0;

		bool	m_bIgnoreSGLights;	//忽略SceneGraph的灯光

		vector<PostProcess*> m_PostProcessArray;	 // 后期处理对象数组	
		PostProcessHeatHaze* m_pPostProcessHeatHaze; // 热薄雾处理器，场景中所有的热处理特效都使用它统一处理
		PostProcessBlur*     m_pPostProcessBlur;

		bool				 m_bEnablePostProcess;   // 是否允许后期处理
	private:
		class Member;
		Member *m_p;

		DECL_RTTI(RenderSys);
	};
}//namespace Cool3D