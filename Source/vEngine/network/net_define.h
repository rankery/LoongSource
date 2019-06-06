//-----------------------------------------------------------------------------
//!\file net_define.h
//!\author Lyp
//!
//!\date 2004-07-14
//! last 2004-07-14
//!
//!\brief 网络客户端通讯底层基本定义
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {
CONST INT GT_NET_BLOCK_TIME = 1000*150;	// 150 ms


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

} // namespace vEngine {
