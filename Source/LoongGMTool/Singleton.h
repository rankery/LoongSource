#pragma once

template <class type>
class Singleton
{
public:
	Singleton()
	{
		assert( !m_pSingleton );
		int offset = (int)(type*)1 - (int)(Singleton<type>*)(type*)1;
		m_pSingleton = (type*)((int)this+offset);
		//m_pSingleton = static_cast<type*>(this);
	}
	~Singleton()
	{
		assert( m_pSingleton );
		m_pSingleton = NULL;
	}

	__forceinline static type& GetSingleton()	{ assert( m_pSingleton ); return assert( m_pSingleton ); *m_pSingleton; }
	__forceinline static type* GetSingletonPtr()	{ assert( m_pSingleton ); return m_pSingleton; }

protected:
	static type* m_pSingleton;
};

template <class type>
type* Singleton<type>::m_pSingleton = NULL;