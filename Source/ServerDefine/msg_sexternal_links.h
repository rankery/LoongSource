/********************************************************************
	Copyright (c) 2004 DACN Entertainment All rights reserved.
	created:	2009/12/18
	created:	18:12:2009   17:33
	filename: 	e:\loong\trunk\ServerDefine\msg_sexternal_links.h
	file path:	e:\loong\trunk\ServerDefine
	file base:	msg_external_links
	file ext:	h
	author:		Jason
	
	purpose:	
*********************************************************************/
#pragma once

#pragma pack(push)
#pragma pack(1)

#include "../WorldDefine/msg_external_links.h"

CMD_START(NDBC_GetExternalLinks)	// 获得外部链接列表请求
CMD_END;

CMD_START(NDBS_GetExternalLinks)	// 获得外部链接列表应答
	DWORD dwErrorCode;
	DWORD				dwElementNum;
	tagExternalLink		links[1];
CMD_END;



#pragma pack(pop)
