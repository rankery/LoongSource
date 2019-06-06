//-----------------------------------------------------------------------------
// File: image_obj
// Time: 2004-2-24
// Auth: CTCRST
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "image_obj.h"

// 使用其它部件
#include "..\vfs\vfs.h"

#include "image_bmp.h"
#include "image_png.h"
#include "image_tga.h"

#ifdef _USE_IJL_
#include "image_jpeg.h"
#endif // _USE_IJL_


namespace vEngine{
// 注意：仅使用低16位
CONST DWORD F_BMP_MAGIC = 0x00004d42; // "BM"
CONST DWORD F_JPG_MAGIC = 0x0000d8ff; // magic of jpg
CONST DWORD F_TGA_MAGIC = 0x00000000; // "DD"

//-----------------------------------------------------------------------------
// 测试包是否匹配当前包对象
//-----------------------------------------------------------------------------
ImageObj::ImageObj()
{
	m_pCodec = NULL;
}


ImageObj::~ImageObj()
{
	SAFE_DEL(m_pCodec);
}


//-----------------------------------------------------------------------------
// load
//-----------------------------------------------------------------------------
DWORD ImageObj::Load(LPCTSTR szFileName)
{
	DWORD dwFileSize = m_pVFS->GetSize(szFileName);
	if( dwFileSize == GT_INVALID )
		return GT_INVALID;

	LPVOID pSource = new BYTE[dwFileSize];
	m_pVFS->LoadFile(pSource, szFileName);
	
	if( m_pCodec )
		SAFE_DEL(m_pCodec);

	// 只取两字节,因为BMP文件的MagicCode只有两字节
	DWORD dwMagicCode = (*(PWORD)pSource);	
	switch( dwMagicCode )
	{
	case F_BMP_MAGIC:
		m_pCodec = new ImageBmp;
		break;
	case F_JPG_MAGIC:
#ifdef _USE_IJL_
		m_pCodec = new ImageJpg;
#endif
		break;
	case F_TGA_MAGIC:
		m_pCodec = new ImageTga;
		break;
	}

	DWORD dwSize = (DWORD)GT_INVALID;
	if( m_pCodec )
		dwSize = m_pCodec->LoadFrom(pSource, dwFileSize);

	delete[](pSource);
	return dwSize;
}


//-----------------------------------------------------------------------------
// render to hdc
//-----------------------------------------------------------------------------
BOOL ImageObj::DrawToHdc(HDC hDC, LPRECT prcArea, BOOL bFlip)
{
	if( m_pCodec )
		return m_pCodec->DrawToHdc(hDC, prcArea, bFlip);
	else
		return FALSE;
}


//-----------------------------------------------------------------------------
//!\param szFileName picture file name
//!\return surface handle
//-----------------------------------------------------------------------------
DWORD ImageObj::_DirectCreateSurface(LPCVOID pSource, DWORD dwSize)
{
	


	return 0;
}



//-----------------------------------------------------------------------------
// 图片宽度
//-----------------------------------------------------------------------------
DWORD ImageObj::GetWidth()
{
	if( m_pCodec )
		return m_pCodec->GetWidth();
	else
		return 0;
}


//-----------------------------------------------------------------------------
// 图片高度
//-----------------------------------------------------------------------------
DWORD ImageObj::GetHeight()
{
	if( m_pCodec )
		return m_pCodec->GetHeight();
	else
		return 0;
}



//-----------------------------------------------------------------------------
// 图片色深
//-----------------------------------------------------------------------------
DWORD ImageObj::GetBitCount() 
{ 
	if( m_pCodec )
		return m_pCodec->GetBitCount();
	else
		return 0;
}



}