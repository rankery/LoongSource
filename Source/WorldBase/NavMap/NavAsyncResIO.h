#pragma once
#include "NavResRequestQueueTS.h"

namespace WorldBase
{
	/**	\class NavAsyncResIO
		\brief 实现异步资源读取
		\remarks 
		\par 为了解决一个资源Load请求仍然在队列中,而ResPool可能需要delete resource对象的问题,
			ResourcePool必须把delete对象实体的操作也加到请求队列中排队.
	*/
	class NavAsyncResIO
	{
	public:
		NavAsyncResIO(void);
		~NavAsyncResIO(void);

		static NavAsyncResIO *Inst();
		void Create();
		void Destroy();
		void QueueRequest(EResIOCmd cmd,NavResBase *pRes,DWORD param);

		void WorkerThreadFunc();
		UINT GetQueueSize();

		void CancleByFS(IFS* pFS)
		{
			m_requestQueue.CancleByFS(pFS);
		}

		UINT GetQueuePushCount();
		UINT GetQueuePopCount();
		void ZeroQueuePushPopCount();

	private:
		bool					m_bTerminate;
		NavResRequestQueueTS	m_requestQueue;
		HANDLE					m_hWorkThread;
	};
}//namespace WorldBase