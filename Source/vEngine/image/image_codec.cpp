//-----------------------------------------------------------------------------
// File: image_codec
// Time: 2004-2-24
// Auth: CTCRST
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "image_codec.h"



namespace vEngine{
//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
ImageCodec::ImageCodec():m_pDIB(0)
{
}


ImageCodec::~ImageCodec()
{
	if( m_pDIB )
		free( m_pDIB);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL ImageCodec::DrawToHdc(HDC hDC, LPRECT prcArea, BOOL bFlip)
{
	int nLeft=0, nTop=0;
	int nWidth = m_Info.dwWidth, nHeight = m_Info.dwHeight;

	if( prcArea )
	{
		nLeft = prcArea->left;
		nTop = prcArea->top;
		nWidth = prcArea->right - prcArea->left;
		nHeight = prcArea->bottom - prcArea->top;
	}

	INT nResult = 0;
	if( FALSE == bFlip )
	{
		nResult = StretchDIBits(hDC, nLeft, nTop, nWidth, nHeight, 0, 
			m_pDIBInfo->biHeight+1, m_pDIBInfo->biWidth, -m_pDIBInfo->biHeight,
			m_pBits, (LPBITMAPINFO)m_pDIBInfo, DIB_RGB_COLORS, SRCCOPY);
	}
	else
	{
		nResult = StretchDIBits(hDC, nLeft, nTop, nWidth, nHeight,
				0, 0, m_pDIBInfo->biWidth, m_pDIBInfo->biHeight, 
				m_pBits, (LPBITMAPINFO)m_pDIBInfo, DIB_RGB_COLORS, SRCCOPY);
	}

	if( GDI_ERROR == nResult )
		return FALSE;

	return TRUE;
}


}