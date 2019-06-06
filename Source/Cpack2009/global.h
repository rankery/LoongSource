#pragma	   once

//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <strstream>
#include <string>
using namespace std;
//----------------------------------------------------------------------
//忽略某些警告

#pragma warning(disable:4244) //“参数” : 从“SOCKET”转换到“DWORD”，可能丢失数据
#pragma warning(disable:4201)
#pragma warning(disable:4100)
#pragma warning(disable:4005)
#pragma warning(disable:4312) //类型转换：从“DWORD”转换到更大的...
#pragma warning(disable:4311) //“类型转换”...
#pragma warning(disable:4355) //“this” : 用于基成员初始值设定项列表
#pragma warning(disable:4533) //“goto跳过了某些初始化操作

#if defined(_WIN64)
typedef __int64 INT3264, *PINT3264;
typedef unsigned __int64 UINT3264, *PUINT3264;
typedef __int64 LONG3264, *PLONG3264;
typedef unsigned __int64 ULONG3264, *PULONG3264, DWORD3264, *PDWORD3264;
#define __int3264   __int64
#else
typedef int INT3264, *PINT3264;
typedef unsigned int UINT3264, *PUINT3264;
typedef long LONG3264, *PLONG3264;
typedef unsigned long ULONG3264, *PULONG3264, DWORD3264, *PDWORD3264;
#define __int3264   __int32
#endif

typedef signed char 			SBYTE;
typedef signed short			SWORD;
typedef signed long 			SDWORD;
typedef unsigned char			*POINTER;
typedef unsigned short int		UINT2;
typedef unsigned long int		UINT4;

//释放内存数组
#define FREEARR(p)		{ {delete [] (p);(p) = NULL;} }
#define FREE(p)			{ {delete (p); (p) = NULL;} }

//---------------------------------------------------------------------

typedef UINT (WINAPI* THREADPROC)(LPVOID);


