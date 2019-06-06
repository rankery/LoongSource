//-----------------------------------------------------------------------------
// File: interface_helper
// Auth: Lyp
// Date: 2004/3/31
// Last: 2004/3/31
//
// Desc: 接口辅助类
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {
//-----------------------------------------------------------------------------
// 指向各种支撑类
//-----------------------------------------------------------------------------
template<typename T> class TObjRef
{
public:
	T* operator->(){ return (T*)m_pRaw; }	// 标准指针
	operator BOOL() { return (m_pRaw!=NULL); }	// 可以和NULL进行比较
	operator T*() { return (T*)m_pRaw; }	// 自动转换为指针类型

	T* operator=(LPCSTR szObjName) 
	{ 
		if( !vEngine::g_pInterfaceMgr )
			return NULL;

		if( szObjName )
			m_pRaw = vEngine::g_pInterfaceMgr->Get(szObjName);
		else
			m_pRaw = vEngine::g_pInterfaceMgr->GetByTypeName(typeid(T*).raw_name()); 
		return (T*)m_pRaw;
	}

	TObjRef(LPCSTR szObjName=NULL)
	{ 
		operator=(szObjName); 
	}

private:
	LPVOID	m_pRaw;
};


} // namespace vEngine {
