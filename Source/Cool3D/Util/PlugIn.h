#pragma once
#include "..\Cool3D.h"
#include "Noncopyable.h"
namespace Cool3D
{
	/** \class PlugIn
		\brief 一个外部插件的加载和使用
		\remarks 外部的插件必须遵守以下规范:1.它必须编译成一个独立的DLL;
		2.它有一个class是从引擎中已有的class(应该是纯虚类)派生的;
		3.这个DLL应该输出四个指定格式的函数,函数名称和声明见下:
	*/
	class Cool3D_API PlugIn : private Noncopyable
	{
		typedef bool		(_cdecl Plug_Init)();	//初始化一次
		typedef void		(_cdecl Plug_Destroy)();	//销毁
		typedef int			(_cdecl Plug_GetNumClass)();	//得到该DLL中包含几个PlugIn的Class
		typedef const TCHAR* (_cdecl Plug_GetClassName)(int i);	//返回一个字符串,用来判定这个PlugIn提供的是一个什么接口的实现
		typedef void*		(_cdecl Plug_NewObject)(const TCHAR* szClassName,DWORD param1,const TCHAR* param2);	//返回一个可用的接口的实现对象的指针
		typedef void		(_cdecl Plug_DelObject)(const TCHAR* szClassName,void *pObj,const TCHAR* param2);	//释放Plug_NewObject()所得到的对象指针
		typedef void*		(_cdecl Plug_GetObject)(const TCHAR* szName);	//返回Plugin中的一个全局对象,此对象在Plugin中已经创建好
	public:
		PlugIn(void);
		virtual ~PlugIn(void);

		bool LoadFromDll(const TCHAR* szPath);
		void Close();

		int			GetNumClass();
		const TCHAR*	GetClassName(int i);
		void*		NewObject(const TCHAR* szClassName,DWORD param1,const TCHAR* param2);
		void		DelObject(const TCHAR* szClassName,void *pObj,const TCHAR* param2);
		void*		GetPlugObject(const TCHAR* szName);

	protected:
		HMODULE				m_hDll;
		Plug_GetNumClass	*m_funGetNumClass;
		Plug_GetClassName	*m_funGetClassName;
		Plug_NewObject		*m_funNewObject;
		Plug_DelObject		*m_funDelObject;
		Plug_Destroy		*m_funDestroy;
		Plug_GetObject		*m_funGetObj;
	};
}//namespace Cool3D