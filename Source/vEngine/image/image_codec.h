//-----------------------------------------------------------------------------
// File: image_codec
// Time: 2004-2-24
// Auth: CTCRST
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{
struct tagImageInfo
{
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwChannels;
	DWORD dwBitCount;
	DWORD dwPalEntryNum;
	DWORD dwWholeSize;
	//DWORD dwColorkey;
};

//-----------------------------------------------------------------------------
// 各种打包文件的统一接口
//-----------------------------------------------------------------------------
class ImageCodec
{
public:
	virtual DWORD LoadFrom(LPCVOID pSource, DWORD dwSize=0){return 0;}
	virtual BOOL DrawToHdc(HDC hDC, LPRECT prcArea=NULL, BOOL bFlip=FALSE);

	ImageCodec();
	virtual ~ImageCodec();
	
	DWORD	GetWidth() { return m_Info.dwWidth; }
	DWORD	GetHeight() { return m_Info.dwHeight; }
	DWORD	GetBitCount() { return m_Info.dwBitCount; }

protected:
	tagImageInfo		m_Info;
	LPVOID				m_pDIB;	// 注意这里m_pDIB指向真实数据
	
	BITMAPINFOHEADER*	m_pDIBInfo;	// 指向m_pDIB中的数据
	LPVOID				m_pBits;	// 指向m_pDIB中的数据
	LPVOID				m_pPalette;	
};

}