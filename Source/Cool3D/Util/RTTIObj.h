#pragma once
#include "..\Cool3D.h"
#include "RTTInfo.h"

/**	\class RTTIObj
	\biref Runtime type identify object, 需要运行时类型信息的对象的最基类
	\remraks 实现一个RTTI系统,能得到某个对象属于什么类,是否可以cast到某个类(包括父类),给出类信息可以在运行时创建对象;
*/
class Cool3D_API RTTIObj
{
public:
	RTTIObj(void);
	virtual ~RTTIObj(void);

	bool	IsKindOf(const TCHAR* szClassName) const;
	bool	IsStaticClass(const TCHAR* szClassName) const;
	bool	IsKindOf(const RTTInfo *pInfo) const;
	bool	IsStaticClass(const RTTInfo *pInfo) const;
	
	const TCHAR* GetClassName();
	//--RTTI系列的宏所声明的东西
	static	RTTInfo		m_classRTTIObj;
	virtual RTTInfo*	GetRTTInfo() const;
	//static RTTIObj	*CreateObj();
};

//--不能动态创建实例的class的声明
#define DECL_RTTI(className)\
	public:\
	static	RTTInfo		m_class##className;\
    virtual RTTInfo*	GetRTTInfo() const;

#define IMP_RTTI(className,superClass)\
	RTTInfo		className::m_class##className(_T(#className),&superClass::m_class##superClass,NULL);\
	RTTInfo*	className::GetRTTInfo() const\
	{	return &m_class##className; }

//--能动态重建实例的class的声明
#define DECL_RTTI_DYNC(className)\
	public:\
	static	RTTInfo		m_class##className;\
	virtual RTTInfo*	GetRTTInfo() const;\
	static RTTIObj	*CreateObj();
#define IMP_RTTI_DYNC(className,superClass)\
	RTTIObj*	className::CreateObj()\
	{ return new className;}\
	RTTInfo		className::m_class##className(_T(#className),&superClass::m_class##superClass,className::CreateObj);\
	RTTInfo*	className::GetRTTInfo() const\
	{	return &m_class##className; }

//--
#define IS_KIND_OF(className)\
	IsKindOf(&className::m_class##className)
#define IS_STATIC_CLASS(className)\
	IsStaticClass(&className::m_class##className)
	
	
	