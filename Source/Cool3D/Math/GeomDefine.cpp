#include "stdafx.h"
#include "Ray.h"
#include ".\geomdefine.h"
#include "..\Util\DebugMisc.h"

namespace Cool3D
{

	//--class Plane
	Plane::Plane(const Vector3& point,const Vector3& _normal)
	{
		normal=_normal;
		dist=-D3DXVec3Dot(&_normal,&point);
		//D3DXPLANE plane;
		//D3DXPlaneFromPointNormal(&plane,&point,&_normal);
		//normal.x=plane.a;
		//normal.y=plane.b;
		//normal.z=plane.c;
		//dist=plane.d;
	}

	void Plane::Normalize()
	{
		float mag=D3DXVec3Length(&normal);
		normal/=mag;
		dist/=mag;
	}

	float Plane::DistToPoint(const Vector3& pos) const
	{
		return D3DXVec3Dot(&normal,&pos)+dist;
	}

	Halfspace Plane::ClassifyPoint(const Vector3& pos) const
	{
		float d=DistToPoint(pos);
		if (d < 0)
			return NEGATIVE;
		if (d > 0)
			return POSITIVE;
		return ON_PLANE;
	}

	void AABBox::GetCorners(Vector3 pt[8]) const
	{
		pt[0]=min;
		pt[1]=Vector3(max.x,min.y,min.z);
		pt[2]=Vector3(max.x,min.y,max.z);
		pt[3]=Vector3(min.x,min.y,max.z);
		

		pt[4]=Vector3(min.x,max.y,min.z);
		pt[5]=Vector3(max.x,max.y,min.z);
		pt[6]=Vector3(min.x,max.y,max.z);
		pt[7]=max;
	}

	void AABBox::Transform( const Matrix4& matrix )
	{
		Vector3 mCorners[8];
		GetCorners(mCorners);
		min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
		max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		//--
		Vector3 temp;
		for( size_t i = 0; i < 8; ++i )
		{
			// Transform and check extents
			temp = mCorners[i]*matrix;
			if( temp.x > max.x )
				max.x = temp.x;
			if( temp.y > max.y )
				max.y = temp.y;
			if( temp.z > max.z )
				max.z = temp.z;
			if( temp.x < min.x )
				min.x = temp.x;
			if( temp.y < min.y )
				min.y = temp.y;
			if( temp.z < min.z )
				min.z = temp.z;
		}

	}

	void AABBox::TransformFast( const Matrix4& matrix,const Vector3& scale )
	{
		Vector3 center=GetCenter();
		D3DXVec3TransformCoord(&center,&center,&matrix);
		Vector3 halfSize(max.x-min.x,max.y-min.y,max.z-min.z);
		halfSize*=0.5f;
		halfSize.x*=scale.x;
		halfSize.y*=scale.y;
		halfSize.z*=scale.z;
		min=center-halfSize;
		max=center+halfSize;
	}

	void AABBox::Merge( const AABBox& rhs )
	{
		if(rhs.max.x > max.x)
			max.x=rhs.max.x;
		if(rhs.max.y > max.y)
			max.y=rhs.max.y;
		if(rhs.max.z > max.z)
			max.z=rhs.max.z;
		//--
		if(rhs.min.x < min.x)
			min.x=rhs.min.x;
		if(rhs.min.y < min.y)
			min.y=rhs.min.y;
		if(rhs.min.z < min.z)
			min.z=rhs.min.z;
	}

	void AABBox::Merge( const Vector3& v )
	{
		if( v.x > max.x )
			max.x = v.x;
		if( v.y > max.y )
			max.y = v.y;
		if( v.z > max.z )
			max.z = v.z;
		if( v.x < min.x )
			min.x = v.x;
		if( v.y < min.y )
			min.y = v.y;
		if( v.z < min.z )
			min.z = v.z;
	}

	bool AABBox::Contain( const AABBox& rhs ) const
	{
		return max.x>=rhs.max.x
			&& max.y>=rhs.max.y
			&& max.z>=rhs.max.z
			&& min.x<=rhs.min.x
			&& min.y<=rhs.min.y
			&& min.z<=rhs.min.z;
	}

	//判断一条线段是否与AABB相交  add by chenliang
	bool AABBox::IntersectsWithLine(Vector3& vPoint1, Vector3& vPoint2)
	{
		Vector3 vLineMiddle=(vPoint1+vPoint2)/2.0f;
		Vector3 vLineVect=vPoint2-vPoint1;
		Vec3Normalize(vLineVect);
        float fHalfLength=Vec3Dist(vPoint1,vPoint2)/2.0f;
		
		const Vector3 e = (max - min) * 0.5f;
		const Vector3 t = (min + e) - vLineMiddle;
		float r;

		if ((fabs(t.x) > e.x + fHalfLength * fabs(vLineVect.x)) || 
			(fabs(t.y) > e.y + fHalfLength * fabs(vLineVect.y)) ||
			(fabs(t.z) > e.z + fHalfLength * fabs(vLineVect.z)) )
				return false;

		r = e.y * fabs(vLineVect.z) + e.z * fabs(vLineVect.y);

		if (fabs(t.y * vLineVect.z - t.z * vLineVect.y) > r)
				return false;

		r = e.x * fabs(vLineVect.z) + e.z * fabs(vLineVect.x);
		if (fabs(t.z * vLineVect.x - t.x * vLineVect.z) > r)
				return false;

		r = e.x * fabs(vLineVect.y) + e.y * fabs(vLineVect.x);
		if (fabs(t.x * vLineVect.y - t.y * vLineVect.x) > r)
				return false;

		return true;
	}

	float AABBox::DistToPointSq(const Vector3& point) const
	{
		Vector3 dist=MathConst::Zero3;

		if(point.x<min.x || point.x>max.x)
			dist.x=min(fabs(min.x-point.x),fabs(max.x-point.x));
		if(point.y<min.y || point.y>max.y)
			dist.y=min(fabs(min.y-point.y),fabs(max.y-point.y));
		if(point.z<min.z || point.z>max.z)
			dist.z=min(fabs(min.z-point.z),fabs(max.z-point.z));

		return D3DXVec3LengthSq(&dist);
	}

	float AABBox::DistToPoint(const Vector3& point) const
	{
		return sqrtf(DistToPointSq(point));
	}

	bool AABBox::TopIntersectsWithVLine( const Vector3& startPos,float height,Vector3& intersectPos ) const
	{
		if(startPos.x>max.x
			||startPos.x<min.x)
			return false;

		if(startPos.z>max.z
			||startPos.z<min.z)
			return false;

		if(startPos.y<max.y)
			return false;

		if(startPos.y-height>max.y)
			return false;

		if(max.y>intersectPos.y)
			intersectPos.y=max.y;

		return true;
	}

	bool AABBox::IntersectsWithVLine( const Vector3& startPos,float height ) const
	{
		if(startPos.x>max.x
			||startPos.x<min.x)
			return false;

		if(startPos.z>max.z
			||startPos.z<min.z)
			return false;

		if(startPos.y<min.y)
			return false;

		if(startPos.y-height>max.y)
			return false;
		
		return true;
	}

	int AABBox::ClipTriangle(const Vector3* tri, Vector3 *newVertex) const
	{
		Vector3		tempVertex[9];

		// Clip against all six planes
		int count = ClipPolygonAgainstPlane(0, 3, tri, tempVertex);
		if (count != 0)
		{
			count = ClipPolygonAgainstPlane(1, count, tempVertex, newVertex);
			if (count != 0)
			{
				count = ClipPolygonAgainstPlane(2, count, newVertex, tempVertex);
				if (count != 0)
				{
					count = ClipPolygonAgainstPlane(3, count, tempVertex, newVertex);
					if (count != 0)
					{
						count = ClipPolygonAgainstPlane(4, count, newVertex, tempVertex);
						if (count != 0)
						{
							count = ClipPolygonAgainstPlane(5, count, tempVertex, newVertex);
						}
					}
				}
			}
		}

		return (count);
	}

	int AABBox::ClipPolygonAgainstPlane( const int nPlaneIndex, const int vertexCount, const Vector3 *vertex, Vector3 *newVertex ) const
	{
		bool	negative[10];

		// Classify vertices
		int negativeCount = 0;
		for (int a = 0; a < vertexCount; a++)
		{
			bool neg = ( VecToAABBoxPlaneDist(vertex[a], nPlaneIndex) < 0.0F );
			negative[a] = neg;
			negativeCount += neg;
		}

		// Discard this polygon if it's completely culled
		if (negativeCount == vertexCount) return (0);

		int count = 0;
		for ( int b = 0; b < vertexCount; b++ )
		{
			// c is the index of the previous vertex
			int c = (b != 0) ? b - 1 : vertexCount - 1;

			if (negative[b])
			{
				if (!negative[c])
				{
					// Current vertex is on negative side of plane,
					// but previous vertex is on positive side.
					const Vector3& v1 = vertex[c];
					const Vector3& v2 = vertex[b];
					float t = VecToAABBoxPlaneDist(v1, nPlaneIndex) / AABBoxPlaneDist( v1, v2, nPlaneIndex );
					newVertex[count] = v1 * (1.0F - t) + v2 * t;
					count++;
				}
			}
			else
			{
				if (negative[c])
				{
					// Current vertex is on positive side of plane,
					// but previous vertex is on negative side.
					const Vector3& v1 = vertex[b];
					const Vector3& v2 = vertex[c];
					float t = VecToAABBoxPlaneDist( v1, nPlaneIndex) / AABBoxPlaneDist( v1, v2, nPlaneIndex );
					newVertex[count] = v1 * (1.0F - t) + v2 * t;
					count++;
				}

				// Include current vertex
				newVertex[count] = vertex[b];
				count++;
			}
		}

		// Return number of vertices in clipped polygon
		return (count);
	}

	float AABBox::VecToAABBoxPlaneDist( const Vector3& vec, const int nPlaneIndex ) const
	{
		switch( nPlaneIndex )
		{
			case 0:    return vec.x - min.x;      //left
			case 1:    return max.x - vec.x;      //right
			case 2:    return vec.z - min.z;      //near
			case 3:    return max.z - vec.z;      //far
			case 4:    return vec.y - min.y;      //bottom
			case 5:    return max.y - vec.y;      //top
			default:   
				ASSERT(0); 
				return 0.0f;
		}
	}

	float AABBox::AABBoxPlaneDist( const Vector3& vec1, const Vector3& vec2, const int nPlaneIndex ) const
	{
		switch( nPlaneIndex )
		{
			case 0:    return vec1.x - vec2.x;    //left
			case 1:    return vec2.x - vec1.x;    //right
			case 2:    return vec1.z - vec2.z;    //near
			case 3:    return vec2.z - vec1.z;    //far
			case 4:    return vec1.y - vec2.y;    //bottom
			case 5:    return vec2.y - vec1.y;    //top
			default:   
				ASSERT(0); 
				return 0.0f;
		}
	}

	bool AABBox::IntersectsTriangle( Vector3& v1,Vector3& v2,Vector3& v3 )
	{
		//box完全包含三角形
		if(Contain(v1)
			&& Contain(v2)
			&& Contain(v3))
			return true;

		//线段与box相交
		if(IntersectsWithLine(v1,v2)
			|| IntersectsWithLine(v2,v3)
			|| IntersectsWithLine(v1,v3))
			return true;

		//box的线段与三角形相交
		Line lines[4];
		GetLines(lines);
		for(int i=0;i<4;i++)
		{
			if(lines[i].IntersectsTri(v1,v2,v3))
				return true;
		}

		return false;
	}

	void AABBox::GetLines( Line lines[4] ) const
	{
		Vector3 corners[8];
		GetCorners(corners);

		/** 0-7
		*/
		lines[0].origin=corners[0];
		lines[0].to=corners[7];

		/** 1-6
		*/
		lines[1].origin=corners[1];
		lines[1].to=corners[6];

		/** 2-4
		*/
		lines[2].origin=corners[2];
		lines[2].to=corners[4];

		/** 3-5
		*/
		lines[3].origin=corners[3];
		lines[3].to=corners[5];
	}

	bool Intersect(const Sphere& sphere, const AABBox& box)
	{
		// Use splitting planes
        const Vector3& center = sphere.centre;
        float radius = sphere.radius;
        const Vector3& min = box.min;
        const Vector3& max = box.max;

        // just test facing planes, early fail if sphere is totally outside
        if (center.x < min.x && 
            min.x - center.x > radius)
        {
            return false;
        }
        if (center.x > max.x && 
            center.x  - max.x > radius)
        {
            return false;
        }

        if (center.y < min.y && 
            min.y - center.y > radius)
        {
            return false;
        }
        if (center.y > max.y && 
            center.y  - max.y > radius)
        {
            return false;
        }

        if (center.z < min.z && 
            min.z - center.z > radius)
        {
            return false;
        }
        if (center.z > max.z && 
            center.z  - max.z > radius)
        {
            return false;
        }

        // Must intersect
        return true;
	}

	bool Intersect(const Plane& plane, const AABBox& box)
	{
		// Get corners of the box
        Vector3 pCorners[8];
		box.GetCorners(pCorners);

        // Test which side of the plane the corners are
        // Intersection occurs when at least one corner is on the 
        // opposite side to another
        Halfspace lastSide = plane.ClassifyPoint(pCorners[0]);
        for (int corner = 1; corner < 8; ++corner)
        {
            if (plane.ClassifyPoint(pCorners[corner]) != lastSide)
            {
                return true;
            }
        }

        return false;
	}

	bool Intersect(const Sphere& sphere, const Plane& plane)
	{
		return (fabs(D3DXVec3Dot(&plane.normal,&sphere.centre)) <= sphere.radius );
	}

	void TriangleCentroid(const Vector3& p1,const Vector3& p2,const Vector3& p3,Vector3& out)
	{
		out=(p1+p2+p3)*(1.0f/3.0f);
	}

	float TriangleArea(const Vector3& p1,const Vector3& p2,const Vector3& p3)
	{
		Vector3 edge1=p3-p1;
		Vector3 edge2=p2-p1;
		Vector3 cross;
		D3DXVec3Cross(&cross,&edge1,&edge2);

		//叉积的一半可以算出三角形的面积,see Eric Lengyel's Mathematics for 3D game programmer & CG
		return 0.5f * D3DXVec3Length(&cross);
	}

	void BuildPlane(const Vector3& p1,const Vector3& p2,const Vector3& p3,Plane& out)
	{
		Vector3 edge1=p3-p1;
		Vector3 edge2=p2-p1;
		D3DXVec3Cross(&out.normal,&edge1,&edge2);
		D3DXVec3Normalize(&out.normal,&out.normal);

		out.dist=-D3DXVec3Dot(&p1,&out.normal);
	}

	Vector3	MathConst::AxisX;
	Vector3	MathConst::AxisY;
	Vector3	MathConst::AxisZ;
	Vector3	MathConst::Zero3;
	Matrix4	MathConst::IdentityMat;
	Matrix4	MathConst::RH2LHMat;
	Matrix4	MathConst::LH2RHMat;
	AABBox	MathConst::SmallBox;
	AABBox	MathConst::BigBox;

	MathConst g_mathConst;//目的是调用构造函数
	MathConst::MathConst()
	{
		AxisX=Vector3(1,0,0);
		AxisY=Vector3(0,1,0);
		AxisZ=Vector3(0,0,1);
		Zero3=Vector3(0,0,0);
		D3DXMatrixIdentity(&IdentityMat);
		D3DXMatrixRotationX(&RH2LHMat,-FLOAT_PI*0.5f);
		D3DXMatrixRotationX(&LH2RHMat,FLOAT_PI*0.5f);
		SmallBox.max=Vector3(1,1,1);
		SmallBox.min=Vector3(-1,-1,-1);
		BigBox.max=Vector3(1000000,1000000,1000000);
		BigBox.min=Vector3(-1000000,-1000000,-1000000);
	}

	MathConst::~MathConst()
	{}

	//--class Triangle
	void Triangle::BuildAABBox( AABBox& box )
	{
		box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);
		box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);

		if(a.x<box.min.x)box.min.x=a.x;
		if(a.x>box.max.x)box.max.x=a.x;
		if(a.y<box.min.y)box.min.y=a.y;
		if(a.y>box.max.y)box.max.y=a.y;
		if(a.z<box.min.z)box.min.z=a.z;
		if(a.z>box.max.z)box.max.z=a.z;

		if(b.x<box.min.x)box.min.x=b.x;
		if(b.x>box.max.x)box.max.x=b.x;
		if(b.y<box.min.y)box.min.y=b.y;
		if(b.y>box.max.y)box.max.y=b.y;
		if(b.z<box.min.z)box.min.z=b.z;
		if(b.z>box.max.z)box.max.z=b.z;

		if(c.x<box.min.x)box.min.x=c.x;
		if(c.x>box.max.x)box.max.x=c.x;
		if(c.y<box.min.y)box.min.y=c.y;
		if(c.y>box.max.y)box.max.y=c.y;
		if(c.z<box.min.z)box.min.z=c.z;
		if(c.z>box.max.z)box.max.z=c.z;
	}

	bool Line::IntersectsTri( Vector3& v1,Vector3& v2,Vector3& v3 ) const
	{
		Vector3 dir=to-origin;
		float dist;
		float length=D3DXVec3Length(&dir);
		dir/=length;
		if(D3DXIntersectTri(&v1,&v2,&v3,&origin,&dir,NULL,NULL,&dist))
		{
			if(dist<length)
				return true;
		}

		return false;
	}
}//endof namespace Cool3D