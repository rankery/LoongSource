#pragma once
#include "Loong_CommonDef.h"


class CServerMerge
{
public:
	CServerMerge(void);
	virtual ~CServerMerge(void);
public:

	virtual BOOL Init(tagConfigDetail stConfigOne[EM_DB_TYPE_END],
					  tagConfigDetail stConfigTwo[EM_DB_TYPE_END],
					  EDBMergeType eMergeType,
					  tstring strSrc2WorldName,
					  const std::multimap<ERoleCompensationType, tagCompensation*> mmCompensation) = 0;

	virtual BOOL ServerMerge() = 0;
};
