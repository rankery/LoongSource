#pragma once
#include "..\WorldBase.h"
#include "NavResMgr.h"

namespace WorldBase
{
	class WORLD_BASE_API NavKernel
	{
	public:
		NavKernel(void);
		~NavKernel(void);

		static NavKernel *Inst();

		/** 初始化资源池等
			\param 资源池最大容量
			\param 资源池是否需要线程同步
		*/
		void Init(UINT poolBytes,bool bPoolThreadSync=false);
		void Destroy();

		//--时间处理
		/** 每帧执行一次
		*/
		void Tick()				
		{
			NavResMgr::Inst()->Update();
		}

		const TCHAR* GetAppPath() const;
		const TCHAR* GetWorkPath() const;
	
	private:
		class Member;
		Member *m_p;
	};
}//namespace WorldBase