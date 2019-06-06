#pragma once

#pragma pack(push, 1)
#include "msg_common_errorcode.h"

struct tagExternalLink
{
	TCHAR linkName[8];
	TCHAR linkValue[X_LONG_NAME];
};

CMD_START(NS_GetExternalLinks)
	DWORD				dwElementNum;
	tagExternalLink		links[1];
CMD_END

#pragma pack(pop)