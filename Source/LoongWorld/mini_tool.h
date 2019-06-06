/********************************************************************
	created:	2009/12/18
	created:	18:12:2009   11:36
	filename: 	e:\loong\trunk\LoongWorld\mini_tool.h
	file path:	e:\loong\trunk\LoongWorld
	file base:	mini_tool
	file ext:	h
	author:		Jason
	
	purpose:	小工具
*********************************************************************/


#ifndef mini_tool_h__
#define mini_tool_h__

namespace jm_tool{

/**
* \ingroup mythread
*
* 简单的锁定守卫类
*
* \version 1.0
* first version
*
* \date 2009-12-11
*
* \author Jason
*
* \log 
* 2009-12-11 created
*
*/
template<class MLOCK>
class MutexGuard
{
	MLOCK & m_lck;
public:
	MutexGuard(MLOCK &l)
		:m_lck(l)
	{
		m_lck.Lock();
	}
	~MutexGuard(void)
	{
		m_lck.Unlock();
	}
};

/**
* \ingroup mythread
*
* 锁的基类
*
* \version 1.0
* first version
*
* \date 2009-12-11
*
* \author Jason
*
* \log 
* 2009-12-11 created
*
*/
template<typename T>
class MLock
{
public:
	MLock()
	{}
	virtual ~MLock(void){}
	virtual void Lock(void){}
	virtual void Unlock(void){}
};

class CSCreatedLock : public MLock<CRITICAL_SECTION>
{
	CRITICAL_SECTION m_cs;
public:
	CSCreatedLock()
	{
		InitializeCriticalSection  ( &m_cs );
	}
	virtual ~CSCreatedLock(void)
	{
		DeleteCriticalSection  ( &m_cs );
	}
	virtual void Lock(void)
	{
		EnterCriticalSection  ( &m_cs );
	}
	virtual void Unlock(void)
	{
		LeaveCriticalSection  ( &m_cs );
	}
};


}

#endif // mini_tool_h__





