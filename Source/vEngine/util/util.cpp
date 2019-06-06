//-----------------------------------------------------------------------------
// File: Util.cpp
// Desc: Game System Util 2.0
// Auth: Lyp
// Date: 2003/11/13
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "util.h"
#include "vector.h"
#include "des.h"
#include "../system/DiscIOMgr.h"

#undef new
#undef delete
#undef malloc
#undef calloc
#undef realloc
#undef free

static UCHAR des_keys_A[20][3][8] = 
{
	23,90,37,87,165,16,2,7,
	24,114,151,238,148,164,166,173,
	25,139,35,18,166,75,241,159,
	47,186,221,239,150,225,211,115,
	49,235,201,185,80,50,6,121,
	72,52,253,183,8,58,64,185,
	96,148,146,74,82,140,204,134,
	121,14,160,234,61,201,150,29,
	168,182,87,66,127,73,223,252,
	217,144,231,42,169,242,210,207,
	34,178,154,196,110,97,52,4,
	130,53,207,148,3,100,152,156,
	251,49,1,149,152,252,149,50,
	164,213,214,108,5,2,151,201,
	126,84,43,151,156,158,54,255,
	160,244,32,183,84,242,41,41,
	35,24,56,239,68,55,96,137,
	31,55,111,95,163,218,59,196,
	195,250,106,201,109,72,131,72,
	66,61,167,113,222,39,170,242,
	226,32,199,57,24,63,233,220,
	6,38,237,39,63,126,104,69,
	37,75,57,96,159,30,134,203,
	232,52,109,205,109,139,18,221,
	43,95,204,154,8,147,165,131,
	14,109,58,212,220,112,22,153,
	20,129,187,144,109,221,243,141,
	57,186,118,7,116,82,70,211,
	34,220,83,90,206,33,103,59,
	77,42,125,215,166,199,47,106,
	91,133,3,218,129,73,120,226,
	111,244,247,210,84,157,22,248,
	168,157,149,104,188,90,112,105,
	202,104,253,102,35,125,237,87,
	24,128,126,228,8,133,115,202,
	115,243,114,87,95,228,88,35,
	226,214,73,160,255,228,61,96,
	139,98,171,76,76,49,110,206,
	86,184,100,176,252,46,156,107,
	110,39,139,60,57,103,4,111,
	225,9,148,208,10,113,117,228,
	196,205,98,51,61,174,36,9,
	80,30,128,179,240,159,195,204,
	166,196,69,248,233,137,77,26,
	21,217,31,24,2,139,216,242,
	246,208,239,8,10,149,110,97,
	187,140,124,132,142,36,146,243,
	12,152,21,153,40,76,222,210,
	178,75,96,249,34,110,77,32,
	199,19,115,109,143,253,75,107,
	190,209,69,178,66,64,139,246,
	122,76,145,68,134,198,82,73,
	134,210,100,168,47,245,72,145,
	57,12,112,25,72,62,134,24,
	1,13,125,150,222,29,163,187,
	191,204,74,224,191,220,128,60,
	58,7,81,50,241,206,79,139,
	192,199,25,75,61,12,91,230,
	249,193,218,38,99,111,202,177,
	250,188,151,189,33,144,91,13
};

static UCHAR des_keys_B[20][3][8] = 
{
	23,201,185,80,50,6,121,72,52,253,
	183,7,204,74,224,191,220,128,60,58,
	7,81,50,241,206,79,139,192,199,25,
	75,61,12,91,230,121,14,160,234,61,
	201,150,29,168,182,87,66,127,73,223,
	249,193,24,114,151,238,148,164,166,173,
	225,211,115,49,235,185,96,148,146,74,
	82,140,204,134,252,152,156,251,49,1,
	149,152,252,149,50,164,217,144,231,42,
	169,242,210,207,34,178,154,196,110,97,
	52,4,130,53,207,148,3,100,213,214,
	108,5,2,151,201,126,84,43,151,156,
	158,54,255,160,244,32,183,84,242,41,
	41,35,24,56,239,68,55,96,137,31,
	55,111,95,163,218,59,196,195,250,106,
	201,109,72,131,72,66,61,167,113,222,
	39,170,242,226,32,199,57,24,63,233,
	220,6,38,237,39,63,126,104,69,37,
	75,57,96,159,30,134,203,232,52,109,
	205,109,139,18,221,43,95,204,154,8,
	147,165,131,14,109,58,212,220,112,22,
	153,20,129,187,144,109,221,243,141,57,
	186,118,7,116,82,70,211,34,220,83,
	90,206,33,103,59,77,42,125,215,166,
	199,47,106,91,133,3,218,129,73,120,
	226,111,244,247,210,84,157,22,248,168,
	157,149,104,188,90,112,105,202,104,253,
	102,35,125,237,87,24,128,126,228,8,
	133,115,202,115,243,114,87,95,228,88,
	35,226,214,73,160,255,228,61,96,139,
	98,171,76,76,49,110,206,86,184,100,
	176,252,46,156,107,110,39,139,60,57,
	103,4,111,225,9,148,208,10,113,117,
	228,196,205,98,51,61,174,36,9,80,
	30,128,179,240,159,195,204,166,196,69,
	248,233,137,77,26,21,217,31,24,2,
	139,216,242,246,208,239,8,10,149,110,
	97,187,140,124,132,142,36,146,243,12,
	152,21,153,40,76,222,210,178,75,96,
	249,34,110,77,32,199,19,115,109,143,
	253,75,107,190,209,69,178,66,64,139,
	246,122,76,145,68,134,198,82,73,134,
	210,100,168,47,245,72,145,57,12,112,
	25,72,62,134,24,1,13,125,150,222,
	29,163,187,191,218,8,58,64,90,37,
	87,165,16,2,38,99,111,202,177,250,
	188,151,189,33,144,91,13,25,139,35,
	18,166,75,241,159,47,186,221,239,150
};

namespace vEngine {

//-----------------------------------------------------------------------------
// construction
//-----------------------------------------------------------------------------
Util::Util():m_dwIDHolder(1),m_nRandHolder(0),m_nRandCallCount(0)
{
	InitializeCriticalSection(&m_Lock);
	m_pCharPool = new CHAR[CHAR_POOL_SIZE];
	m_pTCharPool = new TCHAR[CHAR_POOL_SIZE];
}

Util::~Util()
{
	DeleteCriticalSection(&m_Lock);
	SAFE_DEL_ARRAY(m_pCharPool);
	SAFE_DEL_ARRAY(m_pTCharPool);
}

//-----------------------------------------------------------------------------
// 得到下一个空闲的ID
//-----------------------------------------------------------------------------
DWORD Util::GetFreeID()
{
	if( m_dwIDHolder >= 0x7fffffff )	// 此种情况应该很难发生
		m_dwIDHolder = 1;
	return m_dwIDHolder++;
}


//-----------------------------------------------------------------------------
// 用给定的随机数种子初始化随机数发生器
//-----------------------------------------------------------------------------
VOID Util::Randomize(DWORD dwSeed)
{
	m_nRandHolder = (INT)dwSeed;
	m_nRandCallCount = 0;
}


//-----------------------------------------------------------------------------
// 得到随机数
//-----------------------------------------------------------------------------
INT Util::Rand()
{
	EnterCriticalSection(&m_Lock);
	m_nRandCallCount++;
	
	// code from GCC
	INT nR = (((m_nRandHolder = m_nRandHolder * 1103515245 + 12345) >> 1 ) & LONG_MAX);
	
	// code from VC
	//INT nR = (((m_nRandHolder = m_nRandHolder * 214013L + 2531011L) >> 16) & 0x7fff);

	LeaveCriticalSection(&m_Lock);
	return nR;
}


//-----------------------------------------------------------------------------
// 给出概率百分数，看是否命中
//-----------------------------------------------------------------------------
BOOL Util::Probability(INT nProbCent)
{
	INT nTemp = 0, nMax = 0;
	if( nProbCent<=0 )
		return FALSE;

	nMax = (INT)(((FLOAT)UTIL_PROBABILITY_MOD) / 100.0f * (FLOAT)nProbCent);
	nTemp = (this->Rand() % UTIL_PROBABILITY_MOD);
	if (nTemp < nMax)
		return TRUE;
	else
		return FALSE;
}


//-----------------------------------------------------------------------------
// 正负浮动数值
//-----------------------------------------------------------------------------
FLOAT Util::Fluctuate(FLOAT fNum, FLOAT fIncPercent, FLOAT fDecPercent)
{
	FLOAT fMax = fNum * ( 100.0f + fIncPercent ) / 100.0f;
	FLOAT fMin = fNum * ( 100.0f - fDecPercent ) / 100.0f;
	FLOAT fResult = fMin;
	if( (INT)(fMax-fMin) == 0 )
		fResult = fMin;
	else
		fResult = fMin + (FLOAT)(Rand() % ((INT)(fMax - fMin)));

    return fResult;
}

//-----------------------------------------------------------------------------
// 上下限取中间值
//-----------------------------------------------------------------------------
INT Util::RandomInRange(INT nMin, INT nMax)
{
	if( nMin < 0 || nMax < 0 ) return 0;

	if( nMax <= nMin ) return nMin;

	return nMin + Rand() % (nMax - nMin + 1);
}


//-----------------------------------------------------------------------------
// IntersectionRect 在Win98操作系统下有BUG, 这里我们重写一个
//-----------------------------------------------------------------------------
BOOL Util::RectIntersection(RECT& rcOut, RECT rc1, RECT rc2)
{
	if (rc1.left > rc2.right || rc2.left > rc1.right 
		|| rc1.top > rc2.bottom || rc2.top > rc1.bottom)
		return FALSE;	// 两矩形不相交

	// 下面计算相交矩形
	rcOut.left = rc1.left > rc2.left ? rc1.left : rc2.left;
	rcOut.top = rc1.top > rc2.top ? rc1.top : rc2.top;
	rcOut.right = rc1.right < rc2.right ? rc1.right : rc2.right;
	rcOut.bottom = rc1.bottom < rc2.bottom ? rc1.bottom : rc2.bottom;
	return TRUE;
}


//-----------------------------------------------------------------------------
// 计算16位CRC,算法没有上面的Crc32快,code from quake2
//-----------------------------------------------------------------------------
WORD Util::Crc16(PBYTE pData, INT nByteCount)
{
	WORD wCrc = 0xffff;
	while( nByteCount-- )
		wCrc = (unsigned short)((wCrc << 8) ^ globe_Crc16Table[(wCrc >> 8) ^ *pData++]);

	return wCrc;
}


//-----------------------------------------------------------------------------
// 去掉tstring首尾的空格
//-----------------------------------------------------------------------------
VOID Util::CutSpaceOfStringHeadAndTail(tstring& string)
{
	// 必须检查:如果字符串中只有空格,不能进行此操作
	if( string.find_first_not_of(_T(" ")) != -1 )
	{
		string.assign(string, string.find_first_not_of(_T(" ")),
			string.find_last_not_of(_T(" "))-string.find_first_not_of(_T(" "))+1);
	}
	else
	{
		string.clear();
	}
}


//-----------------------------------------------------------------------------
// 将长串字符转为token;
//-----------------------------------------------------------------------------
VOID Util::StringToToken(std::vector<tstring>& token, LPCWSTR szString, WCHAR chSeparator)
{
	ASSERT(szString);
	tstring str;
	token.clear();

	bool bIntoQuotMark = false;	// 是否进入引号部分
	for(LPCTSTR pStr = szString; *pStr; pStr++)
	{
		if( *pStr == chSeparator )
		{
			if( !str.empty() )
			{
				if( bIntoQuotMark )
					str.append(chSeparator, 1);	// 保留引号里的分隔符
				else
				{
					token.push_back(str);
					str.clear();
				}
			}
		}
		else if( *pStr == _T('"') )
		{
			bIntoQuotMark = !bIntoQuotMark;
		}
		else
		{
			str.append(pStr, 1);
		}
	}

	if(!str.empty())	// 加上剩余的字符
		token.push_back(str);
}



//-----------------------------------------------------------------------------
// 检测注册表中指定键值是否存在
//-----------------------------------------------------------------------------
BOOL Util::CheckReg(HKEY key, LPCTSTR subKey)
{
	HKEY hKey;
	long ret0 = RegOpenKeyEx(key, subKey, 0, KEY_READ, &hKey);
	if ( ret0 == ERROR_SUCCESS )
	{
		RegCloseKey(hKey);
		return true;
	}

	RegCloseKey(hKey);
	return false;
}



//-----------------------------------------------------------------------------
// 将指定键值写入注册表中指定路径,如果指定路径没有,则创建并写入
//-----------------------------------------------------------------------------
BOOL Util::WriteReg(LPCTSTR KEY_ROOT/*注册表中的路径*/, LPCTSTR KEY_NAME, LPCTSTR sz)
{
	if( KEY_ROOT == NULL || KEY_NAME == NULL ) 
		return FALSE;

	HKEY hKey;
	DWORD dwDisposition = REG_CREATED_NEW_KEY;

	long ret0 = 0;
	TCHAR buf[256]={0};
	if ( !CheckReg(HKEY_LOCAL_MACHINE, KEY_ROOT) )
	{
		ret0 = RegCreateKeyEx(HKEY_LOCAL_MACHINE, KEY_ROOT, 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
		if ( ret0 != ERROR_SUCCESS )
			return FALSE;

		RegCloseKey(hKey);
	}

	// -- 写入
	RegOpenKeyEx( HKEY_LOCAL_MACHINE, KEY_ROOT, 0, KEY_WRITE, &hKey );
	ret0 = RegSetValueEx(hKey, KEY_NAME, NULL, REG_SZ, (const BYTE*)sz, lstrlen(sz)*sizeof(sz[0])+sizeof(sz[0]));
	if ( ret0 != ERROR_SUCCESS )
		return FALSE;

	RegCloseKey(hKey);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 读取注册表中指定路径的键值
//-----------------------------------------------------------------------------
BOOL Util::ReadReg(LPCTSTR KEY_ROOT, LPCTSTR KEY_NAME, DWORD& key_value)
{
	HKEY hKey;
	long ret0 = RegOpenKeyEx( HKEY_LOCAL_MACHINE, KEY_ROOT, 0, KEY_QUERY_VALUE, &hKey );
	if( ret0 != ERROR_SUCCESS )
		return FALSE;

	DWORD dwBufLen=32;
	RegQueryValueEx( hKey, KEY_NAME, NULL, NULL, (LPBYTE)&key_value, &dwBufLen);
	RegCloseKey(hKey);

	return TRUE;
}


//-----------------------------------------------------------------------------
// 读注册表
//-----------------------------------------------------------------------------
BOOL Util::ReadReg(LPCTSTR KEY_ROOT, LPCTSTR KEY_NAME, LPTSTR outstr)
{
	ASSERT( outstr != NULL );
	HKEY hKey;
	long ret0 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_ROOT, 0, KEY_READ, &hKey);
	if ( ret0 != ERROR_SUCCESS )
	{
		outstr[0] = 0;
		return FALSE;
	}
	DWORD dwLen = MAX_PATH, dwType = REG_SZ;
	// if char* (LPBYTE)char*
	// if char [] (LPBYTE)&char
	RegQueryValueEx(hKey, KEY_NAME, NULL, &dwType, (LPBYTE)outstr, &dwLen);

	RegCloseKey(hKey);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 求点到直线的距离 (fX,fY)是点，(fX1,fY1)-(fX2,fY2)是直线
//-----------------------------------------------------------------------------
FLOAT Util::Point2LineDist(FLOAT fX, FLOAT fY, FLOAT fX1, FLOAT fY1, 
							FLOAT fX2, FLOAT fY2)
{
	xVec3D vec1 = xVec3D(fX1-fX, fY1-fY, 0);
	xVec3D vec2 = xVec3D(fX2-fX1, fY2-fY1, 0);
	xVec3D vec3;
	vec1.CrossProduct(&vec2, &vec3);
	return vec3.Length() / sqrt((fX2-fX1)*(fX2-fX1) + (fY2-fY1)*(fY2-fY1));

}


//-----------------------------------------------------------------------------
// IsDebuggerPresent需要WinNT4.0以上系统才有，所以这里我们自己实现一个
//-----------------------------------------------------------------------------
bool Util::IsDebuggerPresent()
{
	__asm
	{
		mov     eax, fs:18h
		mov     eax, [eax+30h]
		movzx   eax, byte ptr [eax+2]
	}
}

//----------------------------------------------------------------------------------
// 加解密通用函数(3DES算法)
//----------------------------------------------------------------------------------
BOOL Util::Crypt(UCHAR* pIn, UCHAR* pOut, const INT nLen, UCHAR* key, bool bEncypt)
{
	// 检测输入和输出是否是8字节的整数倍
	if( nLen % 8 != 0 || nLen <= 0 )
		return FALSE;

	// 设置密钥
	des3_context ctx3;
	des3_set_3keys(&ctx3, key, key+8, key+16);

	// 依次读取输入buff里面的8个字节，并进行加解密
	for(INT i = 0; i < nLen; i += 8)
	{
		if( bEncypt )
			des3_encrypt(&ctx3, &pIn[i], &pOut[i]);
		else
			des3_decrypt(&ctx3, &pIn[i], &pOut[i]);
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------
// 对某段内存区域进行加密
//---------------------------------------------------------------------------------------
BOOL Util::Encrypt(VOID* pIn, VOID* pOut, const INT nLen, UCHAR* key)
{
	return Crypt((UCHAR*)pIn, (UCHAR*)pOut, nLen, key, 1);
}

//---------------------------------------------------------------------------------------
// 对某段内存区域进行解密
//---------------------------------------------------------------------------------------
BOOL Util::Decrypt(VOID* pIn, VOID* pOut, const INT nLen, UCHAR* key)
{
	return Crypt((UCHAR*)pIn, (UCHAR*)pOut, nLen, key, 0);
}
UCHAR* GetKeys(const INT nIndex, const INT nMethod)
{
	switch(nMethod)
	{
	case 0:
		return (UCHAR*)des_keys_A[nIndex];
	case 1:
		return (UCHAR*)des_keys_B[nIndex];
	default:
		ASSERT(0);
		return NULL;
	}	
}
//-----------------------------------------------------------------------------------------
// 对某段内存区域进行加密，使用静态密钥索引
//-----------------------------------------------------------------------------------------
BOOL Util::Encrypt(VOID* pIn, VOID *pOut, const INT nLen, const INT nIndex, const INT nMethod)
{
	ASSERT( nIndex >= 0 && nIndex < 20 );
	ASSERT( nMethod >= 0 && nMethod < 2);
	UCHAR* pKeys = GetKeys(nIndex, nMethod);
	if(!P_VALID(pKeys))
		return FALSE;
	return Crypt((UCHAR*)pIn, (UCHAR*)pOut, nLen, pKeys, 1);
}

//-----------------------------------------------------------------------------------------
// 对某段内存区域进行解密，使用静态密钥索引
//-----------------------------------------------------------------------------------------
BOOL Util::Decrypt(VOID* pIn, VOID *pOut, const INT nLen, const INT nIndex, const INT nMethod)
{
	ASSERT( nIndex >= 0 && nIndex < 20 );
	ASSERT( nMethod >= 0 && nMethod < 2);
	UCHAR* pKeys = GetKeys(nIndex, nMethod);
	if(!P_VALID(pKeys))
		return FALSE;
	return Crypt((UCHAR*)pIn, (UCHAR*)pOut, nLen, pKeys, 0);
}





//-----------------------------------------------------------------------------
// MD5 运算相关
//-----------------------------------------------------------------------------
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21
//--------------------------------------------------------------------
//f,g,h,i基本的线性函数
//--------------------------------------------------------------------

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

//--------------------------------------------------------------------
//向右旋转,往左挪移
//--------------------------------------------------------------------
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

//--------------------------------------------------------------------
//关键段,ff,gg,hh,ii操作
//--------------------------------------------------------------------
#define FF(a, b, c, d, x, s, ac) \
{ \
	(a) += F ((b), (c), (d)) + (x) + (DWORD)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac)\
{ \
	(a) += G ((b), (c), (d)) + (x) + (DWORD)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) \
{ \
	(a) += H ((b), (c), (d)) + (x) + (DWORD)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define II(a, b, c, d, x, s, ac) \
{ \
	(a) += I ((b), (c), (d)) + (x) + (DWORD)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}


//--------------------------------------------------------------------
static BYTE PADDING[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


//--------------------------------------------------------------------
//初始化
//--------------------------------------------------------------------
void Util::MD5Init()
{
	m_context.count[0] = m_context.count[1] = 0;

	m_context.state[0] = 0x67452301;
	m_context.state[1] = 0xefcdab89;
	m_context.state[2] = 0x98badcfe;
	m_context.state[3] = 0x10325476;
}


//--------------------------------------------------------------------
// 计算
//--------------------------------------------------------------------
void Util::MD5Update(BYTE *pInput, UINT uInputLen)
{
	UINT i=0;
	UINT uIndex = 0;
	UINT uPartLen = 0;

	uIndex = (UINT)( (m_context.count[0] >> 3) & 0x3F );
	if( (m_context.count[0] += ((DWORD)uInputLen << 3)) < ((DWORD)uInputLen << 3) )
		m_context.count[1]++;
	m_context.count[1] += ((DWORD)uInputLen >> 29);
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
// 结果
//-----------------------------------------------------------------------
void Util::MD5Final(BYTE digest[16])
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
// MD5 basic transformation. Transforms state based on block.
//-----------------------------------------------------------------------
void Util::MD5Transform (DWORD state[4], unsigned char block[64])
{
	DWORD a = state[0], b = state[1], c = state[2], d = state[3];
	DWORD x[16];

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
// 从DWORD按字节转换为BYTE
//-----------------------------------------------------------------------
void Util::Encode(BYTE *pOutput, DWORD *pInput, UINT uLen)
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
//  从BYTE按字节转换为DWORD
//-----------------------------------------------------------------------
void Util::Decode(DWORD *pOutput, BYTE *pInput,UINT uLen)
{
	UINT i, j;

	for (i=0, j=0; j<uLen; i++, j+=4)
		pOutput[i] = ((DWORD)pInput[j]) | (((DWORD)pInput[j+1]) << 8) |
		(((DWORD)pInput[j+2]) << 16) | (((DWORD)pInput[j+3]) << 24);
}


//-----------------------------------------------------------------------
// 计算文件的MD5校验值
//-----------------------------------------------------------------------
BOOL Util::MD5ForFile(LPCTSTR szFileName, BYTE digest[16])
{
	if( NULL == szFileName )
		return FALSE;

	HANDLE hFile = ::CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if( INVALID_HANDLE_VALUE == hFile ) //文件打不开
		return FALSE;


	INT64 n64FileSize = 0;
	INT64 n64Current = 0;
	SYSTEM_INFO si;
	::GetSystemInfo(&si); 
	// 每次映射不要超过128MB
	INT64 n64PerSection = (1024*1024*128) / si.dwAllocationGranularity * si.dwAllocationGranularity;

	::GetFileSizeEx(hFile, (PLARGE_INTEGER)&n64FileSize);	// 处理大尺寸文件

	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if( NULL == hFileMap )
	{
		::CloseHandle(hFile);
		return FALSE;
	}

	MD5Init();
	LPVOID pBuffer = NULL;
	while( n64FileSize > 0 )
	{
		if( n64FileSize > n64PerSection )
		{
			pBuffer = ::MapViewOfFile(hFileMap, FILE_MAP_READ, (DWORD)(n64Current>>32), (DWORD)n64Current, (DWORD)n64PerSection);
			MD5Update((BYTE*)pBuffer, (DWORD)n64PerSection);
			n64Current += n64PerSection;
			n64FileSize -= n64PerSection;
		}
		else
		{
			pBuffer = ::MapViewOfFile(hFileMap, FILE_MAP_READ, (DWORD)(n64Current>>32), (DWORD)n64Current, (DWORD)n64FileSize);
			MD5Update((BYTE*)pBuffer, (DWORD)n64FileSize);
			n64FileSize = 0;
		}

		::UnmapViewOfFile(pBuffer);
	}

	MD5Final(digest);
	CloseHandle(hFileMap);
	CloseHandle(hFile);


	return TRUE;
}


//-----------------------------------------------------------------------
// 计算字符串的MD5摘要信息
//-----------------------------------------------------------------------
VOID Util::MD5ToString(std::string& str, BYTE digest[16])
{
	char szBuf[256];
	ZeroMemory(szBuf, sizeof(szBuf));
	for(int i=0; i<16; i++)
		sprintf(szBuf, "%s%02x", szBuf, digest[i]);

	str = szBuf;
}


//-----------------------------------------------------------------------
// 计算字符串的MD5摘要信息
//-----------------------------------------------------------------------
VOID Util::MD5ForString(char *szString, BYTE digest[16])
{
	UINT uLen = (UINT)strlen(szString);
	MD5ForBuffer((BYTE*)szString, uLen, digest);
}


//-----------------------------------------------------------------------
// 计算buffer的MD5摘要信息
//-----------------------------------------------------------------------
VOID Util::MD5ForBuffer(BYTE *pBuffer, UINT uBufferLen, BYTE digest[16])
{
	MD5Init();
	MD5Update(pBuffer, uBufferLen);
	MD5Final(digest);
}

//-----------------------------------------------------------------------
// 生成正确的配置文件名称
//-----------------------------------------------------------------------
BOOL Util::GetIniPath( LPTSTR tszPath, LPTSTR pFilePathPart )
{
	TObjRef<DiscIOMgr> pDisc = "DiscIOMgr";

	_tcscpy(tszPath, pFilePathPart);
	_tcscat(tszPath, _T(".ini"));
	if (pDisc->IsFileExist(tszPath))
	{
		return TRUE;
	}

	_tcscpy(tszPath, pFilePathPart);
	_tcscat(tszPath, _T(".inc"));
	if (pDisc->IsFileExist(tszPath))
	{
		return TRUE;
	}

	return FALSE;
}

} // namespace vEngine {


