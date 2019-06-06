/**********************************************************************
 *<
	FILE: CSExport.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __CSEXPORT__H
#define __CSEXPORT__H

#include "resource.h"

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#include "..\F3DMaxExp\MsgBox.h"
extern MsgBox g_msgBox;

#endif // __CSEXPORT__H
