#pragma once
#include "DebugMisc.h"

namespace Cool3D
{

	template<class T>
	class SharePtrDelete
	{
	public:
		void FreePtr(T* pObj)
		{ delete pObj; }
	};
	template<class T>
	class SharePtrCRTFree
	{
	public:
		void FreePtr(T* pObj)
		{ free(pObj); }
	};
	/**	\class SharedPtr
		\biref 含有引用计数的指针
		\remarks 对于一个指针对象分配一个UINT的空间来存放计数, 也就是说所有的SharePtr对象的计数器是共享的
	*/
	template<class T,class Del = SharePtrDelete<T> >
	class SharedPtr
	{
	protected:
		T* m_pObj;
		unsigned int *m_pCounter;

		void Release()
		{
			if(m_pCounter == NULL)
				return;
			
			if(*m_pCounter == 1)//仅剩下自己这唯一的引用了
			{
				delete m_pCounter;
				m_pCounter=NULL;
				Del tmp;
				tmp.FreePtr(m_pObj);
				m_pObj=NULL;
			}
			else
				(*m_pCounter)--;
		}
	public:
		virtual ~SharedPtr()
		{
			Release();
		}
		/** 如果使用此默认构造函数,则必须调用Bind()
			\see Bind()
		*/
		SharedPtr()
		{
			m_pObj=NULL;
			m_pCounter=NULL;
		}
		SharedPtr(T* pObj)
		{
			m_pObj=pObj;
			m_pCounter=new unsigned int;
			(*m_pCounter)=1;//初始化为有一个引用
		}
		SharedPtr(const SharedPtr& right)
		{
			if(right.m_pObj==NULL)
			{
				m_pObj=NULL;
				m_pCounter=NULL;
			}
			else
			{
				m_pObj=right.m_pObj;
				m_pCounter=right.m_pCounter;
				(*m_pCounter)++;
			}
		}
		const SharedPtr& operator=(const SharedPtr& right)
		{
			if(this==&right)
				return *this;
			
			Release();

			if(right.m_pObj==NULL)
			{
				m_pObj=NULL;
				m_pCounter=NULL;
			}
			else
			{
				m_pObj=right.m_pObj;
				m_pCounter=right.m_pCounter;
				(*m_pCounter)++;
			}
			return *this;
		}
		bool operator == (const SharedPtr& right) const
		{
			return m_pObj==right.m_pObj;
		}
		bool operator != (const SharedPtr& right) const
		{
			return m_pObj!=right.m_pObj;
		}

		void Bind(T* pObj)
		{
			Release();
			m_pObj=pObj;
			m_pCounter=new unsigned int;
			(*m_pCounter)=1;//初始化为有一个引用
		}

		//--模拟默认指针的行为
		T& operator*() const
		{ return *m_pObj; }
		T* operator->() const
		{ return m_pObj; }

		T* GetMem() const
		{ return m_pObj; }
		/*
		operator T*()//隐式转换到T*
		{	return m_pObj;	}
		operator void*()//增加转换到void*, 以增加自动转换的二义性, 避免外部调用delete SharedPtrObj;
		{ return m_pObj; }
		*/
	};

	//typedef SharedPtr<unsigned char,SharePtrCRTFree<unsigned char> > SharedBytePtr;
	//typedef SharedPtr<unsigned short,SharePtrCRTFree<unsigned short> > SharedWordPtr;
}//namespace Cool3D