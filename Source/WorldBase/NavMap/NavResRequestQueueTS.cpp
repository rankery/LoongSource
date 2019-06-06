#include "StdAfx.h"
#include ".\NavResRequestQueueTS.h"
#include "NavResBase.h"

namespace WorldBase
{
	NavResRequestQueueTS::NavResRequestQueueTS(void)
	{
		InitializeCriticalSection(&m_Lock);
		m_hEvent = ::CreateEvent(NULL, 
			FALSE, //自动Reset
			TRUE, NULL);
		m_pushCount=0;
		m_popCount=0;
	}

	NavResRequestQueueTS::~NavResRequestQueueTS(void)
	{
		if( m_hEvent !=NULL )
			CloseHandle(m_hEvent);
		DeleteCriticalSection(&m_Lock);
	}

	void NavResRequestQueueTS::Push(EResIOCmd cmd,NavResBase *pRes,DWORD param)
	{
		m_pushCount++;

		tagResReq newReq(cmd,pRes,param);
		// 进入互斥区
		EnterCriticalSection(&m_Lock);	
		m_queue.push_back(newReq);
		LeaveCriticalSection(&m_Lock);
		/*if(cmd==EResIO_Delete)
		TRACE(_T("ResAsync:%s queued\r\n"),pRes->GetName());*/
		::SetEvent(m_hEvent);
	}

	tagResReq NavResRequestQueueTS::Pop()
	{
		m_popCount++;

		EnterCriticalSection(&m_Lock);
		tagResReq ret=m_queue.front();
		m_queue.pop_front();
		LeaveCriticalSection(&m_Lock);
		//TRACE(_T("ResAsync:%s poped\r\n"),ret.pRes->GetName());
		return ret;
	}

	bool NavResRequestQueueTS::IsEmpty()
	{
		bool ret=true;
		EnterCriticalSection(&m_Lock);
		ret=m_queue.empty();
		LeaveCriticalSection(&m_Lock);
		return ret;
	}
	UINT NavResRequestQueueTS::GetSize()
	{
		UINT ret=0;
		EnterCriticalSection(&m_Lock);
		ret=m_queue.size();
		LeaveCriticalSection(&m_Lock);
		return ret;
	}

	void NavResRequestQueueTS::CancleByFS(IFS* pFS)
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

	void NavResRequestQueueTS::ZeroPushPopCount()
	{
		m_pushCount=0;
		m_popCount=0;
	}
}//namespace WorldBase