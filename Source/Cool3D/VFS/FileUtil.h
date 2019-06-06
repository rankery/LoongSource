#include "IFS.h"
#pragma once

#define VFS_READ
#include "..\Public\FileUtil.h"

#include <io.h>
inline bool IsFileExist(const TCHAR* szPath)
{
	return _taccess(szPath, 0) != -1;
}