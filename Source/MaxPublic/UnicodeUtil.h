#pragma once
#include <Windows.h>
#include <tchar.h>
#include <string>
using namespace std;

#ifndef tstring
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif

inline wstring CharToWide(const char* src)
{
	WCHAR buf[255]={0};
	MultiByteToWideChar(
		CP_ACP,
		0,
		src,
		strlen(src),
		buf,
		255);
	return wstring(buf);
}

inline string WideToChar(const WCHAR* src)
{
	char buf[1024]={0};
	WideCharToMultiByte(
		CP_ACP,
		0,
		src,
		wcslen(src),
		buf,
		1024,
		NULL,
		NULL);
	return string(buf);
}