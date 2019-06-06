//--------------------------------------------------------------------
#include "stdafx.h"
#include "md5_define.h"
#include "md5.h"
//--------------------------------------------------------------------
static BYTE PADDING[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

MD5_CTX CMD5::m_context;
//--------------------------------------------------------------------
//初始化
//--------------------------------------------------------------------
void CMD5::MD5Init()
{
	m_context.count[0] = m_context.count[1] = 0;

	m_context.state[0] = 0x67452301;
	m_context.state[1] = 0xefcdab89;
	m_context.state[2] = 0x98badcfe;
	m_context.state[3] = 0x10325476;
}
//--------------------------------------------------------------------
//计算
//--------------------------------------------------------------------
void CMD5::MD5Update(BYTE *pInput, UINT uInputLen)
{
	UINT i=0;
	UINT uIndex = 0;
	UINT uPartLen = 0;

	uIndex = (UINT)( (m_context.count[0] >> 3) & 0x3F );
	if( (m_context.count[0] += ((UINT4)uInputLen << 3)) < ((UINT4)uInputLen << 3) )
		m_context.count[1]++;
	m_context.count[1] += ((UINT4)uInputLen >> 29);
	uPartLen = 64 - uIndex;

	if( uInputLen >= uPartLen )
	{
		memcpy(m_context.buffer+uIndex, pInput, uPartLen);
		MD5Transform(m_context.state, m_context.buffer);

		for(i=uPartLen; i+63 < uInputLen; i+=64)
			MD5Transform(m_context.state, pInput+i);

		uIndex = 0;
	}
	else 
		i = 0;
	memcpy(m_context.buffer+uIndex, pInput+i, uInputLen-i);
}
//-----------------------------------------------------------------------
//结果
//-----------------------------------------------------------------------
void CMD5::MD5Final(BYTE digest[16])
{
	BYTE bits[8];
	UINT uIndex = 0;
	UINT uPadLen;

	Encode(bits, m_context.count, 8);

	uIndex = (UINT)((m_context.count[0] >> 3) & 0x3f);
	uPadLen = (uIndex < 56) ? (56-uIndex) : (120-uIndex);
	
	MD5Update(PADDING, uPadLen);
	MD5Update(bits, 8);
	Encode(digest, m_context.state, 16);

	memset(&m_context, 0, sizeof(MD5_CTX));
}
//-----------------------------------------------------------------------
//MD5 basic transformation. Transforms state based on block.
//-----------------------------------------------------------------------
void CMD5::MD5Transform (UINT4 state[4], unsigned char block[64])
{
	UINT4 a = state[0], b = state[1], c = state[2], d = state[3];
	UINT4 x[16];

	Decode (x, block, 64);

	/* Round 1 */
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	memset(x, 0, sizeof(x));
}
//-----------------------------------------------------------------------
//从UINT4按字节转换为BYTE
//-----------------------------------------------------------------------
void CMD5::Encode(BYTE *pOutput, UINT4 *pInput, UINT uLen)
{
	UINT i, j;

	for (i=0, j=0; j<uLen; i++, j+=4)
	{
		pOutput[j] = (unsigned char)(pInput[i] & 0xff);
		pOutput[j+1] = (unsigned char)((pInput[i] >> 8) & 0xff);
		pOutput[j+2] = (unsigned char)((pInput[i] >> 16) & 0xff);
		pOutput[j+3] = (unsigned char)((pInput[i] >> 24) & 0xff);
	}
}
//-----------------------------------------------------------------------
//从BYTE按字节转换为UINT4
//-----------------------------------------------------------------------
void CMD5::Decode(UINT4 *pOutput, BYTE *pInput,UINT uLen)
{
	UINT i, j;

	for (i=0, j=0; j<uLen; i++, j+=4)
		pOutput[i] = ((UINT4)pInput[j]) | (((UINT4)pInput[j+1]) << 8) |
		(((UINT4)pInput[j+2]) << 16) | (((UINT4)pInput[j+3]) << 24);
}
//-----------------------------------------------------------------------
//计算文件的MD5校验值
//-----------------------------------------------------------------------
BOOL	CMD5::MD5ForFile(char *szFileName, BYTE digest[16])
{
	if( NULL == szFileName || strlen(szFileName) == 0 )
		return FALSE;

	HANDLE hFile = ::CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if( INVALID_HANDLE_VALUE == hFile ) //文件打不开
		return FALSE;

	DWORD dwTick = GetTickCount();
	DWORD dwFileSize = ::GetFileSize(hFile, NULL);
	dwTick = GetTickCount() - dwTick;
	if( dwFileSize < 5000000 )
	{// < 5M
		BYTE buffer[1024];
		DWORD dwReadLen = 0;
		MD5Init();
		while( TRUE == ::ReadFile(hFile, buffer, 1024, &dwReadLen, NULL) && dwReadLen > 0 )	
		{
			MD5Update(buffer, dwReadLen);
		}
		MD5Final (digest);
		::CloseHandle(hFile);
	}
	else
	{ //大文件
		HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if( NULL == hFileMap )
		{
			::CloseHandle(hFile);
			return FALSE;
		}
		LPVOID pBuffer = ::MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, dwFileSize);

		MD5Init();
		MD5Update((BYTE*)pBuffer, dwFileSize);
		MD5Final(digest);

		::UnmapViewOfFile(pBuffer);
		CloseHandle(hFileMap);
		CloseHandle(hFile);
	}
	return TRUE;
}
//-----------------------------------------------------------------------
//计算字符串的MD5摘要信息
//-----------------------------------------------------------------------
BOOL	CMD5::MD5ForString(char *szString, BYTE digest[16])
{
	UINT uLen = (UINT)strlen(szString);
	return MD5ForBuffer((BYTE*)szString, uLen, digest);

}
//-----------------------------------------------------------------------
//计算buffer的MD5摘要信息
//-----------------------------------------------------------------------
BOOL	CMD5::MD5ForBuffer(BYTE *pBuffer, UINT uBufferLen, BYTE digest[16])
{
	MD5Init();
	MD5Update(pBuffer, uBufferLen);
	MD5Final(digest);

	return TRUE;
}