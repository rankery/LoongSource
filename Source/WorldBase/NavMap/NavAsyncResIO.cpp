#include "StdAfx.h"
#include ".\NavAsyncResIO.h"
#include "NavResBase.h"
#include <process.h>

namespace WorldBase
{
	NavAsyncResIO::NavAsyncResIO(void)
	{
		m_hWorkThread=NULL;
		m_bTerminate=false;
	}

	NavAsyncResIO::~NavAsyncResIO(void)
	{
	}

	static NavAsyncResIO g_asyncResIO;
	NavAsyncResIO *NavAsyncResIO::Inst()
	{
		//static NavAsyncResIO g_asyncResIO;//如果写在函数体内,则每个线程会生成一个对象
		return &g_asyncResIO;
	}

	unsigned int WINAPI ResIOThreadFunc(void* param)
	{
		NavAsyncResIO *pObj=(NavAsyncResIO *)param;
		pObj->WorkerThreadFunc();
		return 0L;
	}


	void NavAsyncResIO::Create()
	{
		ASSERT(m_hWorkThread==NULL);//没有开始

		m_bTerminate=false;
		unsigned int threadID;
		m_hWorkThread = (HANDLE)_beginthreadex(NULL,
			0,
			ResIOThreadFunc,
			(LPVOID)this,
			0,
			&threadID);
		TRACE(_T("Resource IO thread Created,ID=0x%x\r\n"),threadID);
	}

	void NavAsyncResIO::Destroy()
	{
		if(m_hWorkThread!=NULL)//如果线程已经启动
		{
			m_bTerminate=true;
			WaitForSingleObject(m_hWorkThread,INFINITE);
			DWORD nExitCode=44;
			GetExitCodeThread(m_hWorkThread,&nExitCode);
			TRACE(_T("Resource IO thread Closed,ExitCode=%d,ReqRemain=%d\r\n")
				,nExitCode,m_requestQueue.GetSize());
			::CloseHandle(m_hWorkThread);
			m_hWorkThread=NULL;
		}
	}

	void NavAsyncResIO::QueueRequest(EResIOCmd cmd,NavResBase *pRes,DWORD param)
	{
		m_requestQueue.Push(cmd,pRes,param);
	}

	UINT NavAsyncResIO::GetQueueSize()
	{
		return m_requestQueue.GetSize();
	}

	void NavAsyncResIO::WorkerThreadFunc()
	{
		while(!m_bTerminate)
		{
			for(;!m_requestQueue.IsEmpty();)
			{
				tagResReq req=m_requestQueue.Pop();
				switch(req.cmd)
				{
				case EResIO_Load:
					req.pRes->CreateContent(req.param);
					break;
				case EResIO_Delete:
					delete req.pRes;
					break;
				default:
					ASSERT(0);
				}
			}

			WaitForSingleObject(m_requestQueue.GetEvent(),200);
		}//endof while
	}

	UINT NavAsyncResIO::GetQueuePushCount()
	{
		return m_requestQueue.GetPushCount();
	}

	UINT NavAsyncResIO::GetQueuePopCount()
	{
		return m_requestQueue.GetPopCount();
	}

	void NavAsyncResIO::ZeroQueuePushPopCount()
	{
		m_requestQueue.ZeroPushPopCount();
	}
}//namespace WorldBase