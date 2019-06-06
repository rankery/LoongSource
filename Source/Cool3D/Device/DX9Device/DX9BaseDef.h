
#pragma once
#include "..\IDevice.h"
#include <d3d9types.h>

namespace Cool3D
{
	namespace Detail
	{

		inline EPixelFormat DX9FMT2Cool3D(D3DFORMAT fmt)
		{
			switch(fmt)
			{
			case D3DFMT_R5G6B5:		return EPF_R5G6B5;
			case D3DFMT_R8G8B8:		return EPF_R8G8B8;
			case D3DFMT_A1R5G5B5:	return EPF_A1R5G5B5;
			case D3DFMT_A8R8G8B8:	return EPF_A8R8G8B8;
			case D3DFMT_X8R8G8B8:	return EPF_X8R8G8B8;
			case D3DFMT_A32B32G32R32F:	return EPF_A32R32G32B32F;
			case D3DFMT_DXT1:		return EPF_DXT1;
			}
			return EPF_Num;
		}
		inline D3DFORMAT PF2DX9(EPixelFormat pf)
		{
			switch(pf)
			{
			case EPF_R5G6B5:	return D3DFMT_R5G6B5;
			case EPF_R8G8B8:	return D3DFMT_R8G8B8;
			case EPF_A1R5G5B5:	return D3DFMT_A1R5G5B5;
			case EPF_A8R8G8B8:	return D3DFMT_A8R8G8B8;
			case EPF_X8R8G8B8:	return D3DFMT_X8R8G8B8;
			case EPF_A32R32G32B32F: return D3DFMT_A32B32G32R32F;
			case EPF_DXT1:		return D3DFMT_DXT1;
			case EPF_R32F:		return D3DFMT_R32F;
			}
			return D3DFMT_FORCE_DWORD;
		}

		inline D3DFORMAT ZF2DX9(EDepthFormat zf)
		{
			switch(zf)
			{
			case EDF_D16:	return D3DFMT_D16;
			case EDF_D24S8:	return D3DFMT_D24S8;
			}

			return D3DFMT_FORCE_DWORD;
		}

		inline D3DCMPFUNC CmpFunc2DX9(ECmpFunc cf)
		{
			switch(cf)
			{
			case ECMP_Never:	return D3DCMP_NEVER;
			case ECMP_Less:		return D3DCMP_LESS;
			case ECMP_Equal:	return D3DCMP_EQUAL;
			case ECMP_LessEqual:return D3DCMP_LESSEQUAL;
			case ECMP_Greater:	return D3DCMP_GREATER;
			case ECMP_NotEqual:	return D3DCMP_NOTEQUAL;
			case ECMP_GreaterEqual: return D3DCMP_GREATEREQUAL;
			case ECMP_Always:	return D3DCMP_ALWAYS;
			}//endof switch
			return D3DCMP_FORCE_DWORD;
		}

		inline D3DCULL CullMode2DX9(ECullMode cm)
		{
			switch(cm)
			{
			case ECull_None:	return D3DCULL_NONE;
			case ECull_CW:		return D3DCULL_CW;
			case ECull_CCW:		return D3DCULL_CCW;
			}

			return D3DCULL_FORCE_DWORD;
		}

		inline D3DFILLMODE FillMode2DX9(EFillMode fm)
		{
			switch(fm)
			{
			case EFill_Point:	return D3DFILL_POINT;
			case EFill_Wireframe: return D3DFILL_WIREFRAME;
			case EFill_Solid:	return D3DFILL_SOLID;
			}
			return D3DFILL_FORCE_DWORD;
		}

		inline D3DSHADEMODE ShadeMode2DX9(EShadeMode sm)
		{
			switch(sm)
			{
			case EShade_Flat:	return D3DSHADE_FLAT;
			case EShade_Gouraud:return D3DSHADE_GOURAUD;
			}
			return D3DSHADE_FORCE_DWORD;
		}

		inline D3DBLEND Blend2DX9(EBlend bd)
		{
			switch(bd)
			{
			case EBlend_Zero:			return D3DBLEND_ZERO;
			case EBlend_One:			return D3DBLEND_ONE;
			case EBlend_SrcColor:		return D3DBLEND_SRCCOLOR;
			case EBlend_InvSrcColor:	return D3DBLEND_INVSRCCOLOR;
			case EBlend_SrcAlpha:		return D3DBLEND_SRCALPHA;
			case EBlend_InvSrcAlpha:	return D3DBLEND_INVSRCALPHA;
			case EBlend_DestAlpha:		return D3DBLEND_DESTALPHA;
			case EBlend_InvDestAlpha:	return D3DBLEND_INVDESTALPHA;
			case EBlend_DestColor:		return D3DBLEND_DESTCOLOR;
			case EBlend_InvDestColor:	return D3DBLEND_INVDESTCOLOR;
			}

			return D3DBLEND_FORCE_DWORD;
		}

		inline D3DSTENCILOP StencilOP2DX9(EStencilOP sop)
		{
			switch(sop)
			{
			case ESOP_Keep:		return D3DSTENCILOP_KEEP;
			case ESOP_Zero:		return D3DSTENCILOP_ZERO;
			case ESOP_Replace:	return D3DSTENCILOP_REPLACE;
			case ESOP_IncrSat:	return D3DSTENCILOP_INCRSAT;
			case ESOP_DecrSat:	return D3DSTENCILOP_DECRSAT;
			case ESOP_Invert:	return D3DSTENCILOP_INVERT;
			case ESOP_Incr:		return D3DSTENCILOP_INCR;
			case ESOP_Decr:		return D3DSTENCILOP_DECR;
			}

			return D3DSTENCILOP_FORCE_DWORD;
		}

		inline D3DTEXTUREADDRESS TAddress2DX9(ETextureAddress ta)
		{
			switch(ta)
			{
			case ETAddress_Wrap:	return D3DTADDRESS_WRAP;
			case ETAddress_Mirror:	return D3DTADDRESS_MIRROR;
			case ETAddress_Clamp:	return D3DTADDRESS_CLAMP;
			case ETAddress_Border:	return D3DTADDRESS_BORDER;
			}

			return D3DTADDRESS_FORCE_DWORD;
		}

		inline D3DTEXTUREFILTERTYPE TFilter2DX9(ETextrueFilter tf)
		{
			switch(tf)
			{
			case ETFilter_NONE:		return D3DTEXF_NONE;
			case ETFilter_POINT:	return D3DTEXF_POINT;
			case ETFilter_LINEAR:	return D3DTEXF_LINEAR;
			case ETFilter_ANISOTROPIC:	return D3DTEXF_ANISOTROPIC;
			case ETFilter_GAUSSIAN: return D3DTEXF_GAUSSIANQUAD;
			}

			return D3DTEXF_FORCE_DWORD;
		}


		inline D3DRENDERSTATETYPE RState2DX9(ERStateType rs)
		{
			switch(rs)
			{
			case ERS_DepthFunc:
				return D3DRS_ZFUNC;
			case ERS_DepthTestEnable:
				return D3DRS_ZENABLE;
			case ERS_DepthWriteEnable:
				return D3DRS_ZWRITEENABLE;
			case ERS_CullMode:
				return D3DRS_CULLMODE;
			case ERS_FillMode:
				return D3DRS_FILLMODE;
			case ERS_ShadeMode:
				return D3DRS_SHADEMODE;
			case ERS_AlphaTestEnable:
				return D3DRS_ALPHATESTENABLE;
			case ERS_AlphaTestFunc:
				return D3DRS_ALPHAFUNC;
			case ERS_AlphaRef:
				return D3DRS_ALPHAREF;
			case ERS_AlphaBlendEnable:
				return D3DRS_ALPHABLENDENABLE;
			case ERS_SrcBlend:
				return D3DRS_SRCBLEND;
			case ERS_DstBlend:
				return D3DRS_DESTBLEND;
			case ERS_StencilEnable:
				return D3DRS_STENCILENABLE;
			case ERS_StencilFunc:
				return D3DRS_STENCILFUNC;
			case ERS_StencilOpFail:
			case ERS_StencilOpZFail:
			case ERS_StencilOpPass:
				ASSERT(0);
				
			}//endof switch

			return D3DRS_FORCE_DWORD;
		}

		inline D3DCUBEMAP_FACES CubeMapFace2DX9(int face)
		{
			switch(face)
			{
			case 0:
				return D3DCUBEMAP_FACE_POSITIVE_X;
			case 1:
				return D3DCUBEMAP_FACE_NEGATIVE_X;
			case 2:
				return D3DCUBEMAP_FACE_POSITIVE_Y; 
			case 3:
				return D3DCUBEMAP_FACE_NEGATIVE_Y;
			case 4:
				return D3DCUBEMAP_FACE_POSITIVE_Z;
			case 5:
				return D3DCUBEMAP_FACE_NEGATIVE_Z;
			}
			return D3DCUBEMAP_FACE_FORCE_DWORD;
		}

		inline D3DMULTISAMPLE_TYPE MultiSampleType2DX9(EMultisampleType type)
		{
			switch(type)
			{
			case EMT_None:
				return D3DMULTISAMPLE_NONE;
			case EMT_2x:
				return D3DMULTISAMPLE_2_SAMPLES;
			case EMT_4x:
				return D3DMULTISAMPLE_4_SAMPLES;
			case EMT_8x:
				return D3DMULTISAMPLE_8_SAMPLES;
			}
			return D3DMULTISAMPLE_FORCE_DWORD;
		}

	}
}//namespace Cool3D