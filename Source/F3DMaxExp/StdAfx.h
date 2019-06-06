// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7FB3DF6C_1BC5_49EE_9F86_5F375499BEB6__INCLUDED_)
#define AFX_STDAFX_H__7FB3DF6C_1BC5_49EE_9F86_5F375499BEB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

// TODO: reference additional headers your program requires here
//--3ds max
#include <CS\BipExp.h>
#include <CS\PhyExp.h>
#include "stdmat.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "Max.h"
#if MAX_RELEASE >= 4000
#include "iskin.h"
#endif

//--stl
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
using namespace std;

#include "..\MaxPublic\UnicodeUtil.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7FB3DF6C_1BC5_49EE_9F86_5F375499BEB6__INCLUDED_)
