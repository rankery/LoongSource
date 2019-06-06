//-----------------------------------------------------------------------------
//!\file auto_factory.h
//!\author Lyp
//!\date 2004/3/11
//! last 2004/3/13
//!
//!\brief 模板类: 自动类厂
//-----------------------------------------------------------------------------
#pragma once
#include "binder.h"

namespace vEngine {
//-----------------------------------------------------------------------------
//!\brief 自动类厂
//!
//!	使用与注册都很方便的自动类厂
//!	使用Register( TBinder<YOUR_CLASS>(), DWORD dwID )的格式来注册
//!	然后使用Create( DWORD dwID )来产生类的实体
//-----------------------------------------------------------------------------
template<typename abstract_class>	// 以你需要的指针类型做模板参数
class TAutoFactory
{
public:
	//! 注册各种类
	template<typename concrete_class>
	BOOL Register(TWrap<concrete_class>, DWORD dwID)
	{ 
		TBinder<concrete_class>* pBinder = new TBinder<concrete_class>;
		if( NULL == pBinder )
			return FALSE;

		ASSERT( m_map.find(dwID) == m_map.end() );	// 不允许重复注册
		m_map.insert(std::pair<DWORD, BinderAbstract*>(dwID, pBinder));
		return TRUE;
	}

	template<typename concrete_class>
	BOOL Unregister(TWrap<concrete_class>, DWORD dwID)
	{
		std::map<DWORD, BinderAbstract*>::iterator iter = m_map.find(dwID);
		if( iter == m_map.end() )
			return FALSE;

		delete((TBinder<concrete_class>*)(iter->second));
		m_map.erase(dwID);
		return TRUE;
	}

	//! 产生ID相符的类的实体
	abstract_class* Create(DWORD dwID)
	{
		std::map<DWORD, BinderAbstract*>::iterator iter = m_map.find(dwID);
		if( iter == m_map.end() )
			return NULL;

		return (abstract_class*)iter->second->Create();
	}

	//! 删除ID相符的类的实体
	VOID Destroy(LPVOID p, DWORD dwID)
	{
		std::map<DWORD, BinderAbstract*>::iterator iter = m_map.find(dwID);
		if( iter != m_map.end() )
			iter->second->Destroy(p);
	}

	~TAutoFactory() 
	{	
		std::map<DWORD, BinderAbstract*>::iterator iter;
		for( iter=m_map.begin(); iter!=m_map.end(); ++iter )
			delete(iter->second);
	}

private:
	std::map<DWORD, BinderAbstract*> m_map;
};


} // namespace vEngine {
