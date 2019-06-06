#pragma once
#pragma warning(disable:4251)//Wraning of D3DXVECTOR3 etc need dll export
#include "..\..\MaxPublic\MathBaseDefine.h"

namespace Cool3D
{
	inline Vector3 operator * (const Vector3& v,const Matrix4& m)
	{
		return Vector3(v.x*m._11+v.y*m._21+v.z*m._31+m._41,
			v.x*m._12+v.y*m._22+v.z*m._32+m._42,
			v.x*m._13+v.y*m._23+v.z*m._33+m._43);
	}
}