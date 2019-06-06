
#include "..\Math\Color.h"
#pragma once
#include "..\Cool3D.h"

namespace Cool3D
{
	enum EFogType
	{
		EFog_Dist,//æ‡¿ÎŒÌ
	};

	enum EFogFormula
	{
		EFFM_None,
		EFFM_Linear,
		EFFM_Exp,
		EFFM_Exp2,
	};

	struct Cool3D_API tagDistFog
	{
		Color4ub	color;
		float		start,
					end,
					density;
	};

	inline tagDistFog DistFogLerp( const tagDistFog& f1, const tagDistFog& f2, const float r )
	{
		tagDistFog ret;
		Color4f c1( f1.color );
		Color4f c2( f2.color );
		Color4f cOut;
		D3DXVec4Lerp( (Vector4*)&cOut, (Vector4*)&c1, (Vector4*)&c2, r );
		ret.color = (Color4ub)cOut;
		ret.start = f1.start + ( f2.start - f1.start ) * r;
		ret.end = f1.end + ( f2.end - f1.end ) * r;
		ret.density = f1.density + ( f2.end - f1.end ) * r;
		return ret;
	}
}//namespace Cool3D