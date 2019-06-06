//-----------------------------------------------------------------------------
// File: Util.h
// Desc: Game System Util 2.0
// Auth: Lyp
// Date: 2003/11/13
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {


#define UTIL_PROBABILITY_MOD	8192	// 概率取模数
//-----------------------------------------------------------------------------
// 工具类
//-----------------------------------------------------------------------------
class VENGINE_API Util
{
public:
	DWORD	GetFreeID();
	
	// 随机数
	VOID	Randomize(DWORD dwSeed);
	DWORD	GetRandHolder() { return m_nRandHolder; }
	INT		Rand();

	// 给出概率百分数，看是否命中
	BOOL	Probability(INT nProbCent);
	// 正负浮动数值
	FLOAT	Fluctuate(FLOAT fNum, FLOAT fIncPercent, FLOAT fDecPercent);

	// 上下限取中间值
	INT		RandomInRange(INT nMin, INT nMax);
	
	// Win98操作系统下IntersectionRect有BUG, 这里我们重写一个
	BOOL	RectIntersection(RECT& rcOut, RECT rc1, RECT rc2);

	// 计算32位CRC,算法快速
	__forceinline DWORD	Crc32(LPCSTR lpData);	// 专用来计算字符串
	__forceinline DWORD	Crc32(LPCTSTR pData);	// 专用来计算字符串
	__forceinline DWORD	Crc32(LPBYTE pData, INT nByteCount);
	// 计算16位CRC from quake2, 算法没有上面的Crc32快
	WORD	Crc16(PBYTE pData, INT nByteCount);

	// unicode与unicode8转换
	// 使用时注意使用的是公共buff,避免多次调用后内容覆盖
	__forceinline LPCSTR	UnicodeToUnicode8(LPCTSTR szSrc, LPSTR szOut=NULL);
	__forceinline LPCTSTR	Unicode8ToUnicode(LPCSTR szSrc, LPTSTR szOut=NULL);
	__forceinline LPCSTR	UnicodeToAnsi(LPCTSTR szSrc);
	__forceinline LPCTSTR	AnsiToUnicode(LPCSTR szSrc);

	// 去掉tstring首尾的空格
	VOID CutSpaceOfStringHeadAndTail(tstring& string);
	// 将长串字符转为token;
	VOID StringToToken(std::vector<tstring>& token, LPCWSTR szString, WCHAR chSeparator=_T(' '));

	// 检测注册表中指定键值是否存在
	BOOL CheckReg(HKEY key, LPCTSTR subKey);
	// 将指定键值写入注册表中指定路径,如果指定路径没有,则创建并写入
	BOOL WriteReg(LPCTSTR KEY_ROOT/*注册表中的路径*/, LPCTSTR KEY_NAME, LPCTSTR sz);
	// 读取注册表中指定路径的键值
	BOOL ReadReg(LPCTSTR KEY_ROOT, LPCTSTR KEY_NAME, DWORD& key_value);
	BOOL ReadReg(LPCTSTR KEY_ROOT, LPCTSTR KEY_NAME, LPTSTR outstr);


	// 求点到直线的距离 (fX,fY)是点，(fX1,fY1)-(fX2,fY2)是直线
	FLOAT Point2LineDist(FLOAT fX, FLOAT fY, FLOAT fX1, FLOAT fY1, FLOAT fX2, FLOAT fY2);

	// IsDebuggerPresent需要WinNT4.0以上系统才有，所以这里我们自己实现一个
	bool IsDebuggerPresent();

public:
	// 获取配置文件完整路径(加密或者未加密)
	BOOL GetIniPath( LPTSTR szPath, LPTSTR pFilePathPart );

	// 用提供的密钥对某段内存区域进行加密（这段内存要满足8字节的整数倍）
	BOOL Encrypt(VOID* pIn, VOID* pOut, const INT nLen, UCHAR* key);
	// 用提供的密钥对某段内存区域进行解密（这段内存要满足8字节的整数倍）
	BOOL Decrypt(VOID* pIn, VOID* pOut, const INT nLen, UCHAR* key);
	// 对某个区域的文件内存进行解密，密钥是针对密钥数组中的一个索引
	BOOL Encrypt(VOID* pIn, VOID* pOut, const INT nLen, const INT nIndex, const INT nMethod = 0);
	BOOL Decrypt(VOID* pIn, VOID* pOut, const INT nLen, const INT nIndex, const INT nMethod = 0);

private:
	// 3des加解密通用函数（bEncypt代表是否加密）
	BOOL Crypt(UCHAR* pIn, UCHAR* pOut, const INT nLen, UCHAR* key, bool bEncypt=0);

public:
	VOID	MD5ForBuffer(BYTE* pBuffer, UINT uBufferLen, BYTE digest[16]);
	VOID	MD5ForString(CHAR* szString, BYTE digest[16]);
	BOOL	MD5ForFile(LPCTSTR szFileName, BYTE digest[16]);
	VOID	MD5ToString(std::string& str, BYTE digest[16]);

	Util();
	~Util();

private:

	CRITICAL_SECTION	m_Lock;

	DWORD	m_dwIDHolder;		// 记录ID使用状态
	INT		m_nRandHolder;
	INT		m_nRandCallCount;
	CHAR*	m_pCharPool;
	TCHAR*	m_pTCharPool;


	typedef struct 
	{
		DWORD	state[4];		/* state (ABCD) */
		DWORD	count[2];		/* number of bits, modulo 2^64 (lsb first) */
		BYTE	buffer[64];		/* input buffer */
	} MD5_CTX;
	MD5_CTX	m_context;


	void	MD5Init();
	void	MD5Update(BYTE *pInput, UINT uInputLen);
	void	MD5Final(BYTE digest[16]);

	void	MD5Transform (DWORD[4], unsigned char[64]);
	//从UINT4按字节转换为BYTE
	void	Encode(BYTE* pOutput, DWORD* pInput, unsigned int);
	//从BYTE按字节转换为UINT4
	void	Decode(DWORD* pOutput, BYTE* pInput, unsigned int);

};


#define CHAR_POOL_SIZE	1024*64
#define MASKBITS 0x3F
#define MASKBYTE 0x80
#define MASK2BYTES 0xC0
#define MASK3BYTES 0xE0
//-----------------------------------------------------------------------------
// 将unicode转成unicode8
//-----------------------------------------------------------------------------
LPCSTR Util::UnicodeToUnicode8(LPCTSTR szSrc, LPSTR szOut)
{
	if( !szOut )
		szOut = m_pCharPool;
	LPSTR szDest = szOut;

	while( *szSrc )
	{
		if(*szSrc < 0x80)	// 0xxxxxxx
		{
			*szDest++ = (BYTE)*szSrc;
		}
		else if( *szSrc < 0x800 )	// 110xxxxx 10xxxxxx
		{
			*szDest++ = ((byte)(MASK2BYTES | *szSrc >> 6));
			*szDest++ = ((byte)(MASKBYTE | *szSrc & MASKBITS));
		}
		else	// 1110xxxx 10xxxxxx 10xxxxxx
		{
			*szDest++ = ((byte)(MASK3BYTES | *szSrc >> 12));
			*szDest++ = ((byte)(MASKBYTE | *szSrc >> 6 & MASKBITS));
			*szDest++ = ((byte)(MASKBYTE | *szSrc & MASKBITS));
		}
		++szSrc;
	}

	*szDest = 0; // 加上0结束符
	return szOut;
}


//-----------------------------------------------------------------------------
// 将unicode8转成unicode
//-----------------------------------------------------------------------------
LPCTSTR	Util::Unicode8ToUnicode(LPCSTR szSrc, LPTSTR szOut)
{
	if( !szOut )
		szOut = m_pTCharPool;

	LPTSTR szDest = szOut;

	while( *szSrc )
	{
		if( !(*szSrc & MASKBYTE) )	// 0xxxxxxx
		{
			*szDest++ = *szSrc++;
		}
		else if( (*szSrc & MASK3BYTES) == MASK3BYTES )	// 1110xxxx 10xxxxxx 10xxxxxx
		{
			*szDest++ = ((*szSrc & 0x0F) << 12) | ((*(szSrc+1) & MASKBITS) << 6) | (*(szSrc+2) & MASKBITS);
			++szSrc;
			++szSrc;
			++szSrc;
		}
		else if( (*szSrc & MASK2BYTES) == MASK2BYTES )	// 110xxxxx 10xxxxxx
		{
			*szDest++ = ((*szSrc & 0x1F) << 6) | (*(szSrc+1) & MASKBITS);
			++szSrc;
			++szSrc;
		}
	}

	*szDest = 0; // 加上0结束符
	return szOut;
}


//-----------------------------------------------------------------------------
// 将unicode转成ANSI
//-----------------------------------------------------------------------------
LPCSTR Util::UnicodeToAnsi(LPCTSTR szSrc)
{
	m_pCharPool[0] = 0;
	WideCharToMultiByte(CP_ACP, 0, szSrc, -1, m_pCharPool, CHAR_POOL_SIZE, NULL, NULL);
	return m_pCharPool;
}

//-----------------------------------------------------------------------------
// 将ANSI转成unicode
//-----------------------------------------------------------------------------
LPCTSTR Util::AnsiToUnicode(LPCSTR szSrc)
{
	m_pCharPool[0] = 0;
	MultiByteToWideChar(CP_ACP, 0, szSrc, -1, m_pTCharPool, CHAR_POOL_SIZE);
	return m_pTCharPool;
}

//-----------------------------------------------------------------------------
// 计算32位CRC,比一般算法快速很多
//-----------------------------------------------------------------------------
DWORD Util::Crc32(LPCSTR lpData)
{
	//对ASC超过127的字符， 必须用unsigned char, 否则会出错
	LPBYTE pData = (LPBYTE)lpData;
	if( 0 == pData[0] ) 
		return (DWORD)GT_INVALID;

	UINT unResult = *pData++ << 24;
	if( *pData )
	{
		unResult |= *pData++ << 16;
		if( *pData )
		{
			unResult |= *pData++ << 8;
			if( *pData ) unResult |= *pData++;
		}
	}
	unResult = ~ unResult;
	while( *pData )
	{
		unResult = (unResult << 8 | *pData) ^ globe_Crc32Table[unResult >> 24];
		pData++;
	}
	return ~unResult;
}

//-----------------------------------------------------------------------------
// 计算32位CRC,比一般算法快速很多
//-----------------------------------------------------------------------------
DWORD Util::Crc32(LPCTSTR lpData)
{
	LPBYTE pData = (LPBYTE)lpData;
	if( !(*pData || *(pData+1)) ) 
		return (DWORD)GT_INVALID;

	UINT unResult = *pData++ << 24;
	unResult |= *pData++ << 16;
	if( *pData || *(pData+1) )
	{
		unResult |= *pData++ << 8;
		unResult |= *pData++;
	}
	unResult = ~ unResult;
	INT nCount = 0;
	while( (nCount&1) ? true : (*pData || *(pData+1)) )
	{
		unResult = (unResult << 8 | *pData) ^ globe_Crc32Table[unResult >> 24];
		++pData;
		++nCount;
	}

	return ~unResult;
}

//-----------------------------------------------------------------------------
// 计算32位CRC,比一般算法快速很多
//-----------------------------------------------------------------------------
DWORD Util::Crc32(LPBYTE pData, INT nByteCount)
{
	if( nByteCount <= 0 )
		return (DWORD)GT_INVALID;

	UINT unResult = *pData++ << 24;
	if( --nByteCount > 0 )
	{
		unResult |= *pData++ << 16;
		if( --nByteCount > 0 )
		{
			unResult |= *pData++ << 8;
			if( --nByteCount > 0 ) unResult |= *pData++;
		}
	}
	unResult = ~ unResult;
	while( --nByteCount > 0 )
	{
		unResult = (unResult << 8 | *pData) ^ globe_Crc32Table[unResult >> 24];
		pData++;
	}

	return ~unResult;
}


} // namespace vEngine {
