#pragma once

#include <string>
#include <tchar.h>

using namespace std;

#ifdef _UNICODE
	typedef std::wstring		tstring;
	typedef std::wstringstream	tstringstream;
	typedef std::wstreambuf		tstreambuf;
	typedef std::wifstream      tifstream;
	typedef std::wofstream      tofstream;
	typedef std::wfstream       tfstream;
#else
	typedef std::string			tstring;
	typedef std::stringstream	tstringstream;
	typedef std::streambuf		tstreambuf;
	typedef std::ifstream       tifstream;
	typedef std::ofstream       tofstream;
	typedef std::fstream        tfstream;
#endif


#ifndef __TFILE__
#ifdef _UNICODE
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __TFILE__ WIDEN(__FILE__)
#else
#define __TFILE__ __FILE__
#endif
#endif

#ifndef __TDATE__
#ifdef _UNICODE
#define __TDATE__ WIDEN(__DATE__)
#else
#define __TDATE__ __DATE__
#endif
#endif

inline wstring CharToWide(const char* src)
{
	WCHAR buf[512]={0};
	MultiByteToWideChar(
		CP_ACP,
		0,
		src,
		strlen(src),
		buf,
		512);
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