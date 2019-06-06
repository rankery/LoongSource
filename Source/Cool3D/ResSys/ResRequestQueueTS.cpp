#include "StdAfx.h"
#include ".\resrequestqueuets.h"
#include "..\Util\DebugMisc.h"
#include "ResBase.h"

namespace Cool3D
{
	ResRequestQueueTS::ResRequestQueueTS(void)
	{
		InitializeCriticalSection(&m_Lock);
		m_hEvent = ::CreateEvent(NULL, 
			FALSE, //自动Reset
			TRUE, NULL);
		m_pushCount=0;
		m_popCount=0;
	}

	ResRequestQueueTS::~ResRequestQueueTS(void)
	{
		if( m_hEvent !=NULL )
			CloseHandle(m_hEvent);
		DeleteCriticalSection(&m_Lock);
	}

	void ResRequestQueueTS::Push(EResIOCmd cmd,ResBase *pRes,DWORD param)
	{
		tagResReq newReq(cmd,pRes,param);
		// 进入互斥区
		EnterCriticalSection(&m_Lock);	
		m_pushCount++;
		m_queue.push_back(newReq);
		LeaveCriticalSection(&m_Lock);
		/*if(cmd==EResIO_Delete)
			TRACE(_T("ResAsync:%s queued\r\n"),pRes->GetName());*/
		::SetEvent(m_hEvent);
	}
	
	tagResReq ResRequestQueueTS::Pop()
	{
		EnterCriticalSection(&m_Lock);
		m_popCount++;
		tagResReq ret=m_queue.front();
		m_queue.pop_front();
		LeaveCriticalSection(&m_Lock);
		//TRACE(_T("ResAsync:%s poped\r\n"),ret.pRes->GetName());
		return ret;
	}

	bool ResRequestQueueTS::IsEmpty()
	{
		bool ret=true;
		EnterCriticalSection(&m_Lock);
		ret=m_queue.empty();
		LeaveCriticalSection(&m_Lock);
		return ret;
	}
	UINT ResRequestQueueTS::GetSize()
	{
		UINT ret=0;
		EnterCriticalSection(&m_Lock);
		ret=m_queue.size();
		LeaveCriticalSection(&m_Lock);
		return ret;
	}

	void ResRequestQueueTS::CancleByFS(IFS* pFS)
	{
		EnterCriticalSection(&m_Lock);
		for (deque<tagResReq>::iterator iter = m_queue.begin();
			iter!=m_queue.end();
			)
		{
			const tagResReq& req = (*iter);
			if(req.pRes->GetSafeFS() == pFS)
			{
				iter = m_queue.erase(iter);
				m_pushCount--;
			}
			else
				++iter;
		}

		LeaveCriticalSection(&m_Lock);
	}

	void ResRequestQueueTS::ZeroPushPopCount()
	{
		EnterCriticalSection(&m_Lock);

		m_pushCount=m_queue.size();
		m_popCount=0;

		LeaveCriticalSection(&m_Lock);
	}
}//namespace Cool3D