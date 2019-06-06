#pragma once
#include "..\Cool3D.h"
#include "..\math\Color.h"
#include "..\Math\Math.h"

namespace Cool3D
{
	enum ELightType
	{
		Light_None=0,
		Light_Point,
		Light_Spot,
		Light_Direct,
	
		Light_Num
	};
	struct Cool3D_API tagLight
	{
		int		type;
		Vector3 pos;
		Vector3 dir;
		Color4f ambient;
		Color4f diffuse;
		Color4f specular;
		float	range;
		Vector3 attenuation; //1, D, D^2;
		Vector3 spot;        //theta, phi, falloff

		tagLight()
		{
			memset(this,0,sizeof(tagLight));
		}
	};
}//namespace Cool3D