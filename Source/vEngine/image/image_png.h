//-----------------------------------------------------------------------------
//!\file image_png.h
//!\author Lyp
//!
//!\date 2004-04-14
//! last 2004-04-14
//!
//!\brief png decoder
//-----------------------------------------------------------------------------
#pragma once
#include "image_codec.h"

namespace vEngine {

//-----------------------------------------------------------------------------
//!\brief IamgePng
//!
//!
//-----------------------------------------------------------------------------
class IamgePng : public ImageCodec
{
public:
	virtual DWORD	LoadFrom(LPCVOID pSource, DWORD dwSize);
	virtual DWORD	_DirectCreateSurface(LPCVOID pSource, DWORD dwSize);


	IamgePng();
	~IamgePng();

private:

};






} // namespace vEngine {
