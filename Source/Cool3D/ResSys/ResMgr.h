#pragma once
#include "..\Cool3D.h"

#define RES_MGR_RELEASE_RENDERABLE(x)	if(x!=NULL) {ResMgr::Inst()->ReleaseRenderable(x);x=NULL;}
#define RES_MGR_RELEASE_ALPHAMAP(x)		if(x!=NULL) {ResMgr::Inst()->ReleaseDyncAlphaMap(x);x=NULL;}
#define RES_MGR_RELEASE_VS(x)			if(x!=NULL) {ResMgr::Inst()->ReleaseVS(x);x=NULL;}
#define RES_MGR_RELEASE_PS(x)			if(x!=NULL) {ResMgr::Inst()->ReleasePS(x);x=NULL;}

	class IFS;
namespace Cool3D
{
	class UserResMgr;
	class ResRenderable;
	class ResTexture;
	class IDeviceTex;
	class ResSkeletonAction;
	class ResSpecialEffect;
	class ResDynaAlphaMap;
	class ResVertShader;
	class ResPixelShader;
	/** \class ResMgr
		\brief 资源系统对外的接口
		\remarks
		\par 支持异步模式,但是并没有内建Resource Proxy,即外部在使用资源时需确认ResBase::IsCreated()
		\par 所有模型的贴图路径都应该是以应用程序运行目录为起点的相对路径
		\see ResBase::IsCreated()
		\todo:实现文件后缀与ResType的注册,这样外部的ResType就可以注册进来. 目前的问题是ResBase构造函数有参数,与RTTIObj不兼容
	*/
	class Cool3D_API ResMgr
	{
	public:
		enum EFSType
		{
			EFST_Animation=0,	//动画
			EFST_Model,			//模型
			EFST_Effect,		//特效
			EFST_System,		//其它系统数据
			EFST_Map,			//场景
			EFST_Num
		};
	public:
		ResMgr(void);
		~ResMgr(void);

		/**	设置是否启动异步IO模式
			\param	是否打开异步模式
			\return void
			\remarks 如果使用异步模式,则Resource的Create会立即返回,但只有ResBase::IsCreate()返回True时,此Resource才可用
			\see ResBase::IsCreated()
		*/
		void EnableIOAsync(bool bAsync);
		bool IsIOAsync() const;
		/**	更新内部状态,主要用于监视异步IO的状态
		*/
		void Update();
		/**	得到有多少个Resource正在Load,特指异步IO模式
		*/
		UINT GetNumLoading();
		/** 得到Resource加载计数
		*/
		void GetLoadCount(UINT& finishedCount,UINT& reqCount);
		/** 清空Resource加载计数
		*/
		void ZeroLoadCount();
		/**	Attach一个用户资源管理器,当内部RenderableFactory返回空的时候调用它
		*/
		void AttachUserResMgr(UserResMgr *pMgr);
		/**	返回内部保存的支持
		*/
		UserResMgr* GetUserResMgr() const;

		/**	创建内部的资源池
			\param	贴图池的最大容量,可渲染资源的最大容量(主要是Index和vertex buffer)
		*/
		void Init(UINT texPoolSizeInBytes,UINT renderableSize,UINT skeletonSize,UINT specialEffectSize,UINT vsNum,UINT psNum);
		/**	销毁
		*/
		void Destroy();
		
		void SetFS(EFSType type,IFS* pFS);
		IFS* GetFS(EFSType type);
		IFS* GetFS(const TCHAR* szResName);
		/** 如果FS为NULL，返回DefaultFS
		*/
		IFS* SafeGetFS(EFSType type);
		IFS* SafeGetFS(const TCHAR* szResName);
		void OnFSChanged(EFSType type);

		/**
			\param szName,贴图文件名,应使用相对路径
			\param numLevels,mip map层数,0为自动生成最大层数
		*/
		ResTexture*	NewTexture(const TCHAR* szName,UINT numLevels=0);
		void ReleaseTexture(ResTexture *pTex);
		static IDeviceTex *GetTexFallBack();

		/**	新建ResRenderable派生类的资源
		*/
		ResRenderable* NewRenderable(const TCHAR* szName,DWORD param=0);
		void ReleaseRenderable(ResRenderable *pMesh);

		/** 新建ResSkeleton资源
		*/
		ResSkeletonAction* NewSkeleton(const TCHAR* szName,DWORD param=0,bool bAsync=true);
		void ReleaseSkeleton(ResSkeletonAction *pSkeleton);

		/** 新建ResSpecialEffect资源
		*/
		ResSpecialEffect* NewSpecialEffect(const TCHAR* szName,DWORD param=0);
		void ReleaseSpecialEffect(ResSpecialEffect *pSpecialEffect);

		ResDynaAlphaMap* NewDyncAlphaMap(const TCHAR* szName,int initVal,UINT size);
		void ReleaseDyncAlphaMap(ResDynaAlphaMap* pAlphaMap);

		/** 新建ResVertShader资源
		*/
		ResVertShader* NewVS(const TCHAR* szName,DWORD ver);
		void ReleaseVS(ResVertShader* pVS);

		/** 新建ResPixelShader资源
		*/
		ResPixelShader* NewPS(const TCHAR* szName,DWORD ver);
		void ReleasePS(ResPixelShader* pPS);

		void DrawInfo(int x,int y);

		//!	手动执行垃圾收集
		void DoGC();

		//!	将当前的ResPool的状态列出报表到Log文件中,帮助应用程序分析资源使用状态
		void Report();
		static ResMgr *Inst();

		int GetAsyncQueueSize();
		
	private:
		struct Member;
		Member	*m_p;

		static IDeviceTex *s_pTexFallback;
	};
}//namespace Cool3D