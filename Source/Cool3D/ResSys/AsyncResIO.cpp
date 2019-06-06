#include "StdAfx.h"
#include ".\asyncresio.h"
#include "..\Util\Exception.h"
#include "ResBase.h"
#include <process.h>
#include "..\Device\DX9Device\DX9Device.h"

namespace Cool3D
{
	AsyncResIO::AsyncResIO(void)
	{
		m_hWorkThread=NULL;
		m_bTerminate=false;
	}

	AsyncResIO::~AsyncResIO(void)
	{
	}

	static AsyncResIO g_asyncResIO;
	AsyncResIO *AsyncResIO::Inst()
	{
		//static AsyncResIO g_asyncResIO;//如果写在函数体内,则每个线程会生成一个对象
		return &g_asyncResIO;
	}

	unsigned int WINAPI ResIOThreadFunc(void* param)
	{
		_set_se_translator(Cool3D::Cool_SEH_Translation);
		try
		{
			AsyncResIO *pObj=(AsyncResIO *)param;
			pObj->WorkerThreadFunc();
		}
		catch(Exception& e)
		{
			LOG(e.What());
			::MessageBox(NULL,e.What(),_T("ResThread Exception"),MB_OK);
		}

		_endthreadex(0);
		return 0L;
	}


	void AsyncResIO::Create()
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

	void AsyncResIO::Destroy()
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

	void AsyncResIO::QueueRequest(EResIOCmd cmd,ResBase *pRes,DWORD param)
	{
		m_requestQueue.Push(cmd,pRes,param);
	}

	UINT AsyncResIO::GetQueueSize()
	{
		return m_requestQueue.GetSize();
	}

	void AsyncResIO::WorkerThreadFunc()
	{
		IDevice* pDev=Device();

		while(!m_bTerminate)
		{
			for(;!m_requestQueue.IsEmpty();)
			{
				tagResReq req=m_requestQueue.Pop();
				switch(req.cmd)
				{
				case EResIO_Load:
					{
						ThreadLock lock(pDev->GetResetLock());
						req.pRes->CreateContent(req.param);
					}
					break;
				case EResIO_Delete:
					{
						ThreadLock lock(pDev->GetResetLock());
						delete req.pRes;
					}
					break;
				default:
					ASSERT(0);
				}
			}

			WaitForSingleObject(m_requestQueue.GetEvent(),200);
		}//endof while
	}

	UINT AsyncResIO::GetQueuePushCount()
	{
		return m_requestQueue.GetPushCount();
	}

	UINT AsyncResIO::GetQueuePopCount()
	{
		return m_requestQueue.GetPopCount();
	}

	void AsyncResIO::ZeroQueuePushPopCount()
	{
		m_requestQueue.ZeroPushPopCount();
	}
}//namespace Cool3D