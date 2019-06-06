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
class ImageJpg : public ImageCodec
{
public:
	virtual DWORD	LoadFrom(LPCVOID pSource, DWORD dwSize);

	ImageJpg();	
	virtual ~ImageJpg();

private:
	LPVOID m_pJpgProper;
};




}