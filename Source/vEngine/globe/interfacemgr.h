//-----------------------------------------------------------------------------
// File: obj_mgr
// Auth: Lyp
// Date: 2003/12/11
//-----------------------------------------------------------------------------
#pragma once
#include "frame\auto_factory.h"

namespace vEngine{
//-----------------------------------------------------------------------------
// 设计思想
// 用Interface类来方便管理全局的对象
// 使用全局对象可以使得各种只需要一个对象的组件使用更加方便,但是如果使用
// 普通的Singleton方法来管理有很多弊病,所以这里建立一个统一管理的接口
//-----------------------------------------------------------------------------
class VENGINE_API ObjMgr
{
public:
	LPVOID	Get(LPCSTR szName);	// 根据szName获得对象
	LPVOID	GetByTypeName(LPCSTR szName);	// 根据typename得到对应的对象

	BOOL	Create(LPCSTR szDestObjName, LPCSTR szClassName);	// 根据注册过的对象创建对象
	BOOL	Kill(LPCSTR szObjName);	// 析构对象

	// 注册对象类
	template<typename class_type>
	VOID Register(TWrap<class_type> wrap, LPCSTR szName);

	ObjMgr();~ObjMgr();

private:
	struct tagObj
	{
		LPVOID			pObj;
		DWORD			dwClassID;
		std::string		strObjName;
	};

	std::map<DWORD, tagObj>	m_mapObj;
	std::map<DWORD, DWORD>	m_mapTypeObj;	// typename -> m_mapObj ID
	std::list<std::string>	m_listObj;
	TAutoFactory<VOID>		m_AF;
	CRITICAL_SECTION		m_cs;

	BOOL CheckName(LPCSTR szName);

	// 为了不牵扯CUtil，本地实现一个CRC32
	DWORD Crc32(LPCSTR lpData);
};


//-----------------------------------------------------------------------------
// 外部注册
//-----------------------------------------------------------------------------
template<typename class_type>
VOID ObjMgr::Register(TWrap<class_type> wrap, LPCSTR szName)
{
	DWORD dwID = Crc32(szName); 
	DWORD dwTypeID = Crc32(typeid(class_type*).raw_name());
	CheckName(szName);

	EnterCriticalSection(&m_cs);
	m_AF.Register(wrap, dwID);
	tagObj obj;
	obj.pObj = NULL;
	obj.dwClassID = dwID;
	obj.strObjName.assign(szName);
	m_mapObj.insert( std::pair<DWORD, tagObj>(dwID, obj) );
	m_listObj.push_back(szName);

	// 同时注册type id
	m_mapTypeObj.insert(std::pair<DWORD, DWORD>(dwTypeID, dwID));
	LeaveCriticalSection(&m_cs);
}


}	// namespace vEngine{
