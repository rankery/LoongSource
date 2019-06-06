//-----------------------------------------------------------------------------
// File: image_pixel_format
// Auth: CTCRST
// Time: 2004-3-21
// Last: 2004-3-21
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{

//-----------------------------------------------------------------------------
// ÃèÊö¸÷ÖÖpixel format
//-----------------------------------------------------------------------------
enum EPixelFormat
{
    EPF_Unknow = 0,
    EPF_R8G8B8,
    EPF_A8R8G8B8,
    EPF_R5G6B5,
    EPF_A1R5G5B5,
    EPF_A4R4G4B4,
    EPF_P8,
    EPF_A8,
    EPF_X8R8G8B8,
    EPF_X1R5G5B5,
    EPF_X4R4G4B4,
	EPF_DXT1,
	EPF_DXT3,
	EPF_DXT5,
	EPF_End
};
/*
struct VENGINE_API tagPixelFormat
{
    EPixelFormat eType;
    INT bpp;
    gbColorQuad *Pal; //0----rgba,  else-----clut
    unsigned int r_mask, g_mask,b_mask,a_mask;
    unsigned int r_shift,g_shift,b_shift,a_shift;

public:
    void Init( int bpp, gbColorQuad *pal );
    void Init( gbPixelFmtType type );
    void Init( int bpp, unsigned int rm, unsigned int gm, unsigned int bm, unsigned int am );
    void Init( gbPixelFormat *p );
    void Release(void);
    void Convert( unsigned char *pfrom, gbPixelFormat *pfromfmt, unsigned char *pto, int num_pixels );
};
*/

}	// namespace vEngine{
