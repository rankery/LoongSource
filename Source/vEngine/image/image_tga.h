//-----------------------------------------------------------------------------
//!\file image_tga.h
//!\author Lyp
//!
//!\date 2004-04-14
//! last 2004-04-14
//!
//!\brief tga decoder
//-----------------------------------------------------------------------------
#pragma once
#include "image_codec.h"

namespace vEngine {

//-----------------------------------------------------------------------------
//!\brief CIamgeTga
//!
//!
//-----------------------------------------------------------------------------
class ImageTga : public ImageCodec
{
public:
	virtual DWORD	LoadFrom(LPCVOID pSource, DWORD dwSize);
private:

};

typedef struct
{
	BYTE	IDLength;
	BYTE	ColorMapType;
	BYTE	ImageType;
	WORD	ColormapFirstEntry;
	WORD	ColormapLength;
	BYTE	ColormapEntrySize;
	WORD	X_origin;
	WORD	Y_origin;
	WORD	ImageWidth;
	WORD	ImageHeight;
	BYTE	PixelDepth;
	BYTE	ImageDescriptor;
} TGAFILEHEADER;




} // namespace vEngine {
