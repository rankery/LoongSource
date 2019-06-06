#include "StdAfx.h"
#include ".\rttinfo.h"
#include <windows.h>
#include <string>
#include <map>
using namespace std;

#include "DebugMisc.h"

RTTInfo::RTTInfo(const TCHAR* szClassName,RTTInfo *pSuperClass,CreateObj *pFunc)
					: m_szClassName(szClassName),
					m_pSuperClass(pSuperClass),
					m_pfnCreateObj(pFunc)

{
	RTTFactory::Inst()->AddRuntimeClass(this);
}

RTTInfo::~RTTInfo(void)
{
}

bool RTTInfo::IsKindOf(const RTTInfo *pInfo)
{
	if(pInfo==NULL)
		return false;
	RTTInfo *pInfoIter=this;
	while(pInfoIter!=NULL)
	{
		if(pInfoIter==pInfo)
			return true;
		else
			pInfoIter=pInfoIter->m_pSuperClass;
	}
	return false;
}

bool RTTInfo::IsKindOf(const TCHAR* szClassName)
{
	if(szClassName==NULL)
		return false;
	RTTInfo *pInfoIter=this;
	while(pInfoIter!=NULL)
	{
		if(_tcscmp(szClassName,pInfoIter->m_szClassName) == 0)
			return true;
		else
			pInfoIter=pInfoIter->m_pSuperClass;
	}
	return false;
}

typedef map<tstring,const RTTInfo*>RTTIMap;
class RTTFactory::Member
{
public:
	RTTIMap					m_rttiMap;	//map方式存储,用来快速查找
	vector<const RTTInfo*>	m_rttiArray;//vector方式存储,用来随机访问
	Member()
	{}
	~Member()
	{}
};

RTTFactory::RTTFactory()
{
	m_p=new Member;
}

RTTFactory::~RTTFactory()
{
	delete m_p;
}

RTTIObj	*RTTFactory::CreateObj(const TCHAR* szClassName)
{
	RTTIMap::iterator iter=m_p->m_rttiMap.find(szClassName);
	if(iter==m_p->m_rttiMap.end())
	{
		OutputDebugString(szClassName);
		OutputDebugString(_T(":类别未找到\r\n"));
		return NULL;
	}
	
	const RTTInfo *pInfo=(*iter).second;
	if(pInfo->m_pfnCreateObj==NULL)
	{
		OutputDebugString(szClassName);
		OutputDebugString(_T(":类别未定义动态重建或此类别为虚基类\r\n"));
		return NULL;
	}
	return pInfo->m_pfnCreateObj();
}

RTTFactory *RTTFactory::Inst()
{
	static RTTFactory g_rtcFactory;
	return &g_rtcFactory;
}

void	RTTFactory::AddRuntimeClass(const RTTInfo *pInfo)
{
	if(pInfo==NULL)
		return;
#ifdef _DEBUG
	RTTIMap::iterator iter=m_p->m_rttiMap.find(pInfo->m_szClassName);
	ASSERT(iter==m_p->m_rttiMap.end());
#endif
	m_p->m_rttiMap[pInfo->m_szClassName]=pInfo;
	m_p->m_rttiArray.push_back(pInfo);
}
unsigned int RTTFactory::GetNumClass() const
{
	return m_p->m_rttiArray.size();
}

const RTTInfo *RTTFactory::GetClass(unsigned int i) const
{
	ASSERT(i<GetNumClass());
	return m_p->m_rttiArray[i];
}