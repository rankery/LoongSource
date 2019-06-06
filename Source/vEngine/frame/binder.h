//-----------------------------------------------------------------------------
//!\file binder.h
//!\author Lyp
//!\data 2004/3/11
//! last 2004/3/14
//!
//!\brief 与类厂等配合使用的绑定器
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {
//-----------------------------------------------------------------------------
//! 把一些基本接口绑定到指定类
//-----------------------------------------------------------------------------
class BinderAbstract
{ 	
public:
	virtual LPVOID Create()=0;	// 创建对象的接口
	virtual VOID Destroy(LPVOID)=0;	// 删除对象的接口
};


//-----------------------------------------------------------------------------
//! 把create,destroy绑定到指定类,与类厂等配合使用
//-----------------------------------------------------------------------------
template<typename concrete_class>class TBinder : public BinderAbstract
{
public:
	LPVOID Create() { return (LPVOID)(new concrete_class); }
	VOID Destroy(LPVOID p) { delete((concrete_class*)p); }
};


//-----------------------------------------------------------------------------
//! 单纯用来传递类型信息的模板类
//-----------------------------------------------------------------------------
template<typename concrete_class>class TWrap{};



//-----------------------------------------------------------------------------
//! 类型函数指针委托
//-----------------------------------------------------------------------------
class Delegate 
{
public:
      virtual void Invoke()=0;
protected:
      Delegate(){}
      virtual ~Delegate(){}
};


class NonTypeDelegate : public Delegate
{
public:
	void Invoke() { m_pfn(m_nParam);}
	NonTypeDelegate(void (*pfn)(int),int nParam):m_pfn(pfn),m_nParam(nParam){}
	virtual ~NonTypeDelegate(){}

private:
	void (*m_pfn)(int);
	int m_nParam;
};


template <typename T>class TTypeDelegate : public Delegate
{
public:
	void Invoke(){(m_t.*m_pfn)(m_nParam);}
	TTypeDelegate(T &t, void (T::*pfn)(int), int nParam):m_t(t),m_pfn(pfn),m_nParam(nParam){}
	~TTypeDelegate(){}
private:
	T &m_t;
	void (T::*m_pfn)(int);
	int m_nParam;
};



} // namespace vEngine {
