
#include "stdafx.h"
#include "Ray.h"
#include <limits>

namespace Cool3D
{
	
	TResult Ray::Intersect(const Plane &plane) const
	{
		float denom=D3DXVec3Dot(&plane.normal,&dir);
		if (fabsf(denom) < FLOAT_EPSILON)
			return TResult(false, 0);// Parallel
		else
        {
            float nom = D3DXVec3Dot(&plane.normal,&origin) + plane.dist;
            float t = -(nom/denom);
            
			return TResult(t >= 0, t);//背面相交返回false
        }
	}

	TResult Ray::Intersect(const Sphere& sphere, bool discardInside) const
	{
		const Vector3& raydir = dir;
        // Adjust ray origin relative to sphere center
        const Vector3& rayorig = origin - sphere.centre;
        float radius = sphere.radius;

        // Check origin inside first
        if (D3DXVec3LengthSq(&rayorig) <= radius*radius && discardInside)
        {
            return TResult(true, 0);
        }

        // Mmm, quadratics
        // Build coeffs which can be used with std quadratic solver
        // ie t = (-b +/- sqrt(b*b + 4ac)) / 2a
        float a = D3DXVec3Dot(&raydir,&raydir);
        float b = 2 * D3DXVec3Dot(&rayorig,&raydir);
        float c = D3DXVec3Dot(&rayorig,&rayorig) - radius*radius;

        // Calc determinant
        float d = (b*b) - (4 * a * c);
        if (d < 0)
        {
            // No intersection
            return TResult(false, 0);
        }
        else
        {
            // BTW, if d=0 there is one intersection, if d > 0 there are 2
            // But we only want the closest one, so that's ok, just use the 
            // '-' version of the solver
            float t = ( -b - sqrtf(d) ) / (2 * a);
            if (t < 0)
                t = ( -b + sqrtf(d) ) / (2 * a);
            return TResult(true, t);
        }
	}

	inline bool operator < ( const Vector3& lhs ,const Vector3& rhs )
	{
		if( lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z )
			return true;
		return false;
	}

	inline bool operator > ( const Vector3& lhs ,const Vector3& rhs )
	{
		if( lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z )
			return true;
		return false;
	}

	TResult Ray::Intersect(const AABBox& box) const
	{
		float lowt = 0.0f;
        float t;
        bool hit = false;
        Vector3 hitpoint;
        const Vector3& min = box.min;
        const Vector3& max = box.max;
        const Vector3& rayorig = origin;
        const Vector3& raydir = dir;

        // Check origin inside first
        if ( rayorig > min && rayorig < max )
        {
            return TResult(true, 0);
        }

        // Check each face in turn, only check closest 3
        // Min x
        if (rayorig.x < min.x && raydir.x > 0)
        {
            t = (min.x - rayorig.x) / raydir.x;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max x
        if (rayorig.x > max.x && raydir.x < 0)
        {
            t = (max.x - rayorig.x) / raydir.x;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Min y
        if (rayorig.y < min.y && raydir.y > 0)
        {
            t = (min.y - rayorig.y) / raydir.y;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max y
        if (rayorig.y > max.y && raydir.y < 0)
        {
            t = (max.y - rayorig.y) / raydir.y;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Min z
        if (rayorig.z < min.z && raydir.z > 0)
        {
            t = (min.z - rayorig.z) / raydir.z;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max z
        if (rayorig.z > max.z && raydir.z < 0)
        {
            t = (max.z - rayorig.z) / raydir.z;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }

		// Check length
		if(hit&&lowt>length)
			hit=false;

        return TResult(hit, lowt);
	}
	
	TResult Ray::IntersectTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3) const
	{
		/* find vectors for two edges sharing vert0 */		
		Vector3 edge1=v2-v1;
		Vector3 edge2=v3-v1;

		/* begin calculating determinant - also used to calculate U parameter */
		Vector3 pvec;
		D3DXVec3Cross(&pvec, &dir, &edge2);

		/* if determinant is near zero, ray lies in plane of triangle */
		float det=D3DXVec3Dot(&edge1, &pvec);

		if (det > -FLOAT_EPSILON && det < FLOAT_EPSILON)
			return TResult(false, 0);
		float inv_det = 1.0f / det;

		/* calculate distance from vert0 to ray origin */
		Vector3 tvec=origin-v1;

		/* calculate U parameter and test bounds */
		float u = D3DXVec3Dot(&tvec, &pvec) * inv_det;
		if (u < 0.0f || u > 1.0f)
			return TResult(false, 0);

		/* prepare to test V parameter */
		Vector3 qvec;
		D3DXVec3Cross(&qvec, &tvec, &edge1);

		/* calculate V parameter and test bounds */
		float v = D3DXVec3Dot(&dir, &qvec) * inv_det;
		if (v < 0.0f || u + v > 1.0f)
			return TResult(false, 0);

		/* calculate t, ray intersects triangle */
		float t = D3DXVec3Dot(&edge2, &qvec) * inv_det;

		return TResult(true, t);
	}
}//namespace Cool3D