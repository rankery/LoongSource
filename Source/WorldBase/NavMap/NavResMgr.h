#pragma once
#include "..\WorldBase.h"

#define NAV_RES_MGR_RELEASE_RES(x)	if(x!=NULL) {NavResMgr::Inst()->ReleaseRes(x);x=NULL;}

class IFS;
namespace WorldBase
{
	class NavResBase;

	/** \class NavResMgr
	\brief 资源系统对外的接口
	\remarks
	\par 支持异步模式,但是并没有内建Resource Proxy,即外部在使用资源时需确认NavResBase::IsCreated()
	\par 所有模型的贴图路径都应该是以应用程序运行目录为起点的相对路径
	\see NavResBase::IsCreated()
	\todo:实现文件后缀与ResType的注册,这样外部的ResType就可以注册进来. 目前的问题是NavResBase构造函数有参数,与RTTIObj不兼容
	*/
	class WORLD_BASE_API NavResMgr
	{
	public:
		enum EFSType
		{
			EFST_Model,			//模型
			EFST_System,		//其它系统数据
			EFST_Map,			//场景
			EFST_Num
		};
	public:
		NavResMgr(void);
		~NavResMgr(void);

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

		/**	创建内部的资源池
		*/
		void Init(UINT poolSize,bool bPoolThreadSync=false);
		/**	销毁
		*/
		void Destroy();

		void SetFS(EFSType type,IFS* pFS);
		IFS* GetFS(EFSType type);
		IFS* GetFS(const TCHAR* szResName);
		IFS* DefaultFS();
		/** 如果FS为NULL，返回DefaultFS
		*/
		IFS* SafeGetFS(EFSType type);
		IFS* SafeGetFS(const TCHAR* szResName);
		void OnFSChanged(EFSType type);

		/**	新建NavResSceneMesh派生类的资源
		*/
		NavResBase* NewRes(const TCHAR* szName,DWORD param=0);
		void ReleaseRes(NavResBase *pRes);

		//!	手动执行垃圾收集
		void DoGC();

		//!	将当前的ResPool的状态列出报表到Log文件中,帮助应用程序分析资源使用状态
		void Report();
		static NavResMgr *Inst();

		int GetAsyncQueueSize();

	private:
		struct Member;
		Member	*m_p;
	};
}//namespace WorldBase