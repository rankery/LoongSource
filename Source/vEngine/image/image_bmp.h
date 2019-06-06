//-----------------------------------------------------------------------------
// File: image_jpeg
// Time: 2004-3-10
// Auth: CTCRST
//-----------------------------------------------------------------------------
#pragma once
#include "image_codec.h"

namespace vEngine{

//-----------------------------------------------------------------------------
// 各种打包文件的统一接口
//-----------------------------------------------------------------------------
class ImageBmp:public ImageCodec
{
public:
	virtual DWORD	LoadFrom(LPCVOID pSource, DWORD dwSize);

	virtual BOOL DrawToHdc(HDC hDC, LPRECT prcArea=NULL, BOOL bFlip=FALSE)
	{
		// BMP是倒着存放的,我们把它倒过来
		return ImageCodec::DrawToHdc(hDC, prcArea, !bFlip);
	}

};




}