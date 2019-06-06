#pragma once
#include "CriticalSection.h"
#include "ThreadLock.h"

#include <list>

namespace Cool3D
{
	template<typename T>
	class TSList
	{
	public:
		TSList(void)	
		{
			m_iter = m_list.end();
		}
		~TSList(void)	{}

		void push_back(const T& val)	
		{
			ThreadLock accessLock(&m_lock);
			m_list.push_back(val);
		}

		void remove(const T& val)
		{
			ThreadLock accessLock(&m_lock);
			m_list.remove(val);
		}

		bool begin_iterator()
		{
			m_lock.Lock();
			m_iter = m_list.begin();
			return m_iter != m_list.end();
		}

		bool goto_next()
		{
			++m_iter;
			if(m_iter == m_list.end())
				return false;
			else
				return true;
		}

		void get_current(T& val)
		{
			val = (*m_iter);
		}

		void remove_current()
		{
			if(m_iter != m_list.end())
			{
				m_iter = m_list.erase(m_iter);
			}
		}

		void end_iterator()
		{
			m_lock.Unlock();
			m_iter = m_list.end();
		}
		

	private:
		typename std::list<T>			m_list;
		typename std::list<T>::iterator	m_iter;

		CriticalSection		m_lock;
	};
	/** unit test
	//--
	TSList<int> testList;
	for(int i=0;i<10;i++)
	{
	testList.push_back(i);
	}

	testList.remove(2);
	testList.remove(5);

	bool bNext = testList.begin_iterator();
	while(bNext)
	{
	int c;
	testList.get_current(c);
	bNext = testList.goto_next();

	_Trace("test list:%d\r\n",c);
	}

	testList.end_iterator();
	**/
}//namespace Cool3D