//-----------------------------------------------------------------------------
// File: image_jpeg
// Time: 2004-3-10
// Auth: CTCRST
//-----------------------------------------------------------------------------
#include "..\stdafx.h"

#ifdef _USE_IJL_


#include "image_jpeg.h"

// 使用其它部件
#include "ijl.h"

namespace vEngine{
#pragma comment(lib,"ijl15.lib")	// 此库由intel发行
//-----------------------------------------------------------------------------
// LoadFrom
//-----------------------------------------------------------------------------
ImageJpg::ImageJpg()
{
	m_pJpgProper = new JPEG_CORE_PROPERTIES;
}

ImageJpg::~ImageJpg()
{
	SAFE_DEL(m_pJpgProper);
}

//-----------------------------------------------------------------------------
// LoadFrom
//-----------------------------------------------------------------------------
DWORD ImageJpg::LoadFrom(LPCVOID pSource, DWORD dwSize)
{
	IJLERR ijlErr;
	ZeroMemory( m_pJpgProper, sizeof(JPEG_CORE_PROPERTIES) );

	JPEG_CORE_PROPERTIES* pJpgProper = (JPEG_CORE_PROPERTIES*)m_pJpgProper;
	// Initialize the IntelR JPEG Library.
	ijlErr = ijlInit( pJpgProper );

	if( IJL_OK != ijlErr )
		return (DWORD)GT_INVALID;

	pJpgProper->JPGBytes = (BYTE*)pSource;
	pJpgProper->JPGSizeBytes = dwSize;
	ijlErr = ijlRead(pJpgProper, IJL_JBUFF_READPARAMS);

	if( IJL_OK != ijlErr )
	{
		ijlFree(pJpgProper);
		return (DWORD)GT_INVALID;
	}

	m_Info.dwWidth = pJpgProper->JPGWidth;
	m_Info.dwHeight = pJpgProper->JPGHeight;
	m_Info.dwChannels = pJpgProper->JPGChannels;
	m_Info.dwBitCount = 24;
	m_Info.dwPalEntryNum = 0;

	// Compute DIB padding
	int dib_line_width = m_Info.dwWidth * m_Info.dwChannels;
	int dib_pad_bytes = IJL_DIB_PAD_BYTES(m_Info.dwWidth, m_Info.dwChannels);

	// Compute size of desired pixel buffer.
	m_Info.dwWholeSize = ( dib_line_width + dib_pad_bytes ) * m_Info.dwHeight;
	// Allocate memory to hold the decompressed image data.
	m_pDIB = malloc(sizeof(BITMAPINFOHEADER) + m_Info.dwWholeSize);
	if( NULL == m_pDIB )
	{
		ijlFree(pJpgProper);
		return (DWORD)GT_INVALID;
	}

	m_pBits = (BYTE*)m_pDIB + sizeof(BITMAPINFOHEADER);

	m_pDIBInfo = (BITMAPINFOHEADER*)(m_pDIB);
	m_pDIBInfo->biSize = sizeof(BITMAPINFOHEADER);
	m_pDIBInfo->biWidth = m_Info.dwWidth;
	m_pDIBInfo->biHeight = m_Info.dwHeight;
	m_pDIBInfo->biPlanes = 1;

	m_pDIBInfo->biBitCount = 24;
	m_pDIBInfo->biCompression = BI_RGB;
	m_pDIBInfo->biSizeImage = 0;
	m_pDIBInfo->biXPelsPerMeter = 0;
	m_pDIBInfo->biYPelsPerMeter = 0;
	m_pDIBInfo->biClrUsed = 0;
	m_pDIBInfo->biClrImportant = 0;

	// Set up the info on the desired DIB properties.
	pJpgProper->DIBWidth = m_Info.dwWidth;
	pJpgProper->DIBHeight = m_Info.dwHeight; // Implies a bottom-up DIB.
	pJpgProper->DIBChannels = m_Info.dwChannels;
	pJpgProper->DIBColor = IJL_BGR;
	pJpgProper->DIBPadBytes = dib_pad_bytes;
	pJpgProper->DIBBytes = (BYTE*)m_pDIB + sizeof(BITMAPINFOHEADER);

	// Set the JPG color space
	switch(pJpgProper->JPGChannels)
	{
	case 1:
		pJpgProper->JPGColor = IJL_G;
		break;
	case 3:
		pJpgProper->JPGColor = IJL_YCBCR;
		break;
	default:
		// This catches everything else, but no
		// color twist will be performed by the IJL.
		pJpgProper->DIBColor = (IJL_COLOR)IJL_OTHER;
		pJpgProper->JPGColor = (IJL_COLOR)IJL_OTHER;
		break;
	}

	// Now get the actual JPEG image data into the pixel buffer.
	ijlErr = ijlRead(pJpgProper, IJL_JBUFF_READWHOLEIMAGE);
	
	if( IJL_OK != ijlErr )
	{
		if( m_pDIB )
			free(m_pDIB);

		ijlFree(pJpgProper);
		return (DWORD)GT_INVALID;		
	}
	
	ijlFree(pJpgProper);
	return m_Info.dwWholeSize;
}


}

#endif	// #ifdef _USE_IJL_