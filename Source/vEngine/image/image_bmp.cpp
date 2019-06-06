//-----------------------------------------------------------------------------
// File: image_jpeg
// Time: 2004-3-10
// Auth: CTCRST
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
// 类继承(公共接口)
#include "image_bmp.h"

namespace vEngine{
//-----------------------------------------------------------------------------
// LoadFrom
//-----------------------------------------------------------------------------
DWORD ImageBmp::LoadFrom(LPCVOID pSource, DWORD dwSize)
{
	ASSERT(pSource);

	if( m_pDIB )
		free( m_pDIB);

	//file head
	DWORD dwBitOffset = ((BITMAPFILEHEADER*)pSource)->bfOffBits;
	m_pDIB = malloc( dwSize - sizeof(BITMAPFILEHEADER) );
	if( NULL == m_pDIB )
		return (DWORD)GT_INVALID;
 
	// load bmp
	memcpy( m_pDIB, (PBYTE)pSource+sizeof(BITMAPFILEHEADER), 
		dwSize - sizeof(BITMAPFILEHEADER) );
    
	m_pDIBInfo = (BITMAPINFOHEADER*)m_pDIB;

	//info
	m_Info.dwWidth = m_pDIBInfo->biWidth;
    m_Info.dwHeight = m_pDIBInfo->biHeight;
	m_Info.dwWholeSize = dwSize - dwBitOffset;
	m_Info.dwBitCount = m_pDIBInfo->biBitCount;
	
	if( m_Info.dwBitCount <=8 )
		m_Info.dwChannels = 1;
	else
		m_Info.dwChannels = 3;

	m_pBits = (PBYTE)m_pDIB + (dwBitOffset - sizeof(BITMAPFILEHEADER));
	if( m_Info.dwBitCount <=8 )
		m_pPalette = (PBYTE)m_pDIB + sizeof(BITMAPINFOHEADER);

	// 处理调色板
	if( m_Info.dwBitCount==1 || m_Info.dwBitCount==2 
		|| m_Info.dwBitCount==4 || m_Info.dwBitCount==8 )
	{
		m_Info.dwPalEntryNum = 1 << m_Info.dwBitCount;
		// 暂时不处理调色板
		ASSERT(0);
	}

	return m_Info.dwWholeSize;
}




}