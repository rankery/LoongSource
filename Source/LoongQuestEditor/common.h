#ifndef COMMON_H_
#define COMMON_H_

//删除普通对象
#define  SAFEDELETE(p) { if(p) { delete (p); p = NULL; } }
//删除数组对象
#define  SAFEDELETEARRAY(p) { if(p) { delete [] (p); p = NULL; } }

static string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen<= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen -1] = 0;

	string strTemp(pszDst);
	delete [] pszDst;

	return strTemp;
}

//将wstring 转到 string
static string ws2s(wstring& inputws)
{ 
	return WChar2Ansi(inputws.c_str()); 
}

static wstring Ansi2WChar(LPCSTR pszSrc, int nLen)

{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if(nSize <= 0) 
	{
		return wstring(TEXT(""));
	}
	WCHAR *pwszDst = new WCHAR[nSize+1];
	if( NULL == pwszDst) return NULL;

	MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;

	if( pwszDst[0] == 0xFEFF)                    // skip Oxfeff
		for(int i = 0; i < nSize; i ++)
			pwszDst[i] = pwszDst[i+1];

	wstring wcharString(pwszDst);
	delete pwszDst;

	return wcharString;
}

static wstring s2ws(const string& s)
{ 
	return Ansi2WChar(s.c_str(),(int)s.size());
}

static string ConvertGBKToUtf8(const char* gbk)
{
	int len=MultiByteToWideChar(CP_ACP, 0, gbk, -1, NULL,0);
	unsigned short * wszUtf8 = new unsigned short[len+1];
	memset(wszUtf8, 0, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, gbk, -1, (LPWSTR)wszUtf8, len);

	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
	char *szUtf8=new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL);

	string strGBK = szUtf8;
	delete[] szUtf8;
	delete[] wszUtf8;

	return strGBK;
}

//static wstring ConvertGBKToUtf8W(  )
//{
//}

static string ConvertGBKToUtf8( const string& gbk )
{
	return ConvertGBKToUtf8( gbk.c_str() );
}

static string ConvertUtf8ToGBK( const char* utf8 ) 
{
	int len=MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL,0);
	unsigned short * wszGBK = new unsigned short[len+1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK=new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL,NULL);

	string strUtf8 = szGBK;
	delete[] szGBK;
	delete[] wszGBK;

	return strUtf8;
}

static wstring ConvertUtf8ToGBKW( const char* utf8 )
{
	int len=MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL,0);
	unsigned short * wszGBK = new unsigned short[len+1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, (LPWSTR)wszGBK, len);
	wstring wstr = (LPCWSTR)wszGBK;
	delete[] wszGBK;
	wszGBK = NULL;
	return wstr;
}

static string ConvertUtf8ToGBK( const string& utf8 )
{
	return ConvertUtf8ToGBK( utf8.c_str() );
}


#endif /* COMMON_H_ */