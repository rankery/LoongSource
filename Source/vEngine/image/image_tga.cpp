//-----------------------------------------------------------------------------
//!\file image_tga.cpp
//!\author Lyp
//!
//!\date 2004-04-14
//! last 2004-04-14
//!
//!\brief tga decoder
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "image_tga.h"


namespace vEngine {
//-----------------------------------------------------------------------------
// LoadFrom
//-----------------------------------------------------------------------------
DWORD ImageTga::LoadFrom(LPCVOID pSource, DWORD dwSize)
{
	if( m_pDIB )
		free( m_pDIB);

	LPBYTE head = (LPBYTE)pSource;
	TGAFILEHEADER fileHeader;
	fileHeader.IDLength = head[0];
	fileHeader.ColorMapType = head[1];
	fileHeader.ImageType = head[2];
	fileHeader.ColormapFirstEntry = *(WORD *)(head+3);
	fileHeader.ColormapLength = *(WORD *)(head+5);
	fileHeader.ColormapEntrySize = head[7];
	fileHeader.X_origin = *(WORD *)(head+8);
	fileHeader.Y_origin = *(WORD *)(head+10); 
	fileHeader.ImageWidth = *(WORD *)(head+12);
	fileHeader.ImageHeight = *(WORD *)(head+14);
	fileHeader.PixelDepth = head[16];
	fileHeader.ImageDescriptor = head[17]; 

	INT nBytes = fileHeader.ImageWidth * fileHeader.ImageHeight 
		* (INT)(fileHeader.PixelDepth/8);

	m_Info.dwWidth = fileHeader.ImageWidth;
	m_Info.dwHeight = fileHeader.ImageHeight;
	m_Info.dwChannels = fileHeader.PixelDepth;
	m_Info.dwWholeSize = nBytes + sizeof(BITMAPINFOHEADER);

	m_pDIB = malloc( m_Info.dwWholeSize );
	if( NULL == m_pDIB )
		return (DWORD)GT_INVALID;

	m_pBits = (BYTE*)m_pDIB + sizeof(BITMAPINFOHEADER);
	m_pDIBInfo = (BITMAPINFOHEADER*)(m_pDIB);
	m_pDIBInfo->biSize = sizeof(BITMAPINFOHEADER);
	m_pDIBInfo->biWidth = m_Info.dwWidth;
	m_pDIBInfo->biHeight = m_Info.dwHeight;
	m_pDIBInfo->biPlanes = 1;
	m_pDIBInfo->biBitCount = fileHeader.PixelDepth;
	m_pDIBInfo->biCompression = BI_RGB;
	m_pDIBInfo->biSizeImage = 0;
	m_pDIBInfo->biXPelsPerMeter = 0;
	m_pDIBInfo->biYPelsPerMeter = 0;
	m_pDIBInfo->biClrUsed = 0;
	m_pDIBInfo->biClrImportant = 0;

	// load tga
	switch( fileHeader.ImageDescriptor&0x30 )
	{
		case 0x20:	// start from bottom left
			memcpy( m_pBits, 
				(LPBYTE)pSource + sizeof(TGAFILEHEADER), nBytes);
			break;
		case 0x00:	// start from top left
		{
			INT nPitch = m_Info.dwWidth * (INT)(fileHeader.PixelDepth/8);
			for( INT i=m_Info.dwHeight-1, n=0; i>=0; i--,n++ )
				memcpy( (LPBYTE)m_pBits + i * nPitch, 
					(LPBYTE)pSource + sizeof(TGAFILEHEADER) + n*nPitch, 
					nPitch );
			break;
		}
		case 0x10:	// start from right. I haven't seen any TGA file stores like that, if
		case 0x30:	// you found such file, just add code here.
			return 0;	
	}

	return 0;
}







} // namespace vEngine {
