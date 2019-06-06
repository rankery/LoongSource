#include "StdAfx.h"
#include ".\rttiobj.h"
#include "RTTInfo.h"


RTTInfo	RTTIObj::m_classRTTIObj(_T("RTTIObj"),NULL,NULL);

RTTIObj::RTTIObj(void)
{
}

RTTIObj::~RTTIObj(void)
{
}

RTTInfo* RTTIObj::GetRTTInfo() const
{
	return &m_classRTTIObj;
}

const TCHAR* RTTIObj::GetClassName()
{
	return GetRTTInfo()->m_szClassName;
}

bool	RTTIObj::IsKindOf(const TCHAR* szClassName) const
{
	RTTInfo *pInfo=const_cast<RTTInfo*>(GetRTTInfo());
	return pInfo->IsKindOf(szClassName);
}
bool	RTTIObj::IsStaticClass(const TCHAR* szClassName) const
{
	RTTInfo *pInfo=const_cast<RTTInfo*>(GetRTTInfo());
	if(pInfo==NULL)
		return false;
	return _tcscmp(szClassName,pInfo->m_szClassName) == 0;
}


bool	RTTIObj::IsKindOf(const RTTInfo *pInfo) const
{
	RTTInfo *pMyInfo=const_cast<RTTInfo*>(GetRTTInfo());
	return pMyInfo->IsKindOf(pInfo);
}

bool	RTTIObj::IsStaticClass(const RTTInfo *pInfo) const
{
	if(pInfo==NULL)
		return false;
	return pInfo==GetRTTInfo();
}