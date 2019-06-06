#include "..\Cool3D.h"
#include "math.h"
#include "GeomDefine.h"
#pragma once

namespace Cool3D
{
	typedef pair<bool,float> TResult;	//相交测试结果
	class Cool3D_API Ray  
	{
	public:
		Ray(){length=100000.0f;}
		Ray(Vector3 _dir, Vector3 _origin, float _length=100000.0f):dir(_dir), origin(_origin), length(_length){}

	public:
		Vector3 dir;
		Vector3 origin;
		float length;
		
	public:
		TResult IntersectTriangle(const Vector3& v1,const Vector3& v2,const Vector3& v3) const;
		/** Ray / plane intersection, returns boolean result and distance. */
		TResult Intersect(const Plane& plane) const;
		/** Ray / sphere intersection, returns boolean result and distance. */
        TResult Intersect(const Sphere& sphere, bool discardInside = true) const;
		/** Ray / box intersection, returns boolean result and distance. */
        TResult Intersect(const AABBox& box) const;
	};
}//namespace Cool3D