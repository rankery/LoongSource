#pragma once

CONST INT GT_NET_BLOCK_TIME = 1000*150;	// 150 ms

//-----------------------------------------------------------------------------
// 计算32位CRC,比一般算法快速很多
//-----------------------------------------------------------------------------
static DWORD Crc32(LPCSTR lpData)
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

static DWORD Crc32(LPCTSTR lpData)
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
// 网络消息基本结构
//-----------------------------------------------------------------------------
struct tagNetCmd
{
	DWORD	dwID;
	DWORD	dwSize;

	DWORD Crc32(LPCSTR lpData)
	{
		UINT unResult;
		LPBYTE pData = (LPBYTE)lpData;
		if( 0 == pData[0] ) 
			return (DWORD)GT_INVALID;
		unResult = *pData++ << 24;
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
};

//-----------------------------------------------------------------------------
// 方便网络消息的定义
//-----------------------------------------------------------------------------

#ifndef CMD_START
#define CMD_START(name)	\
struct tag##name : public tagNetCmd{	\
	tag##name(){ ZeroMemory(this,sizeof(*this));  \
	dwID = Crc32(#name); dwSize = sizeof(*this); }
#define CMD_END		};
#endif

