#include ".\geomdefine.h"
using namespace std;

namespace Cool3D
{

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

	void TriangleCentroid(const Vector3& p1,const Vector3& p2,const Vector3& p3,Vector3& out)
	{
		out=(p1+p2+p3)*(1.0f/3.0f);
	}
	
	void BuildPlane(const Vector3& p1,const Vector3& p2,const Vector3& p3,float out[4])
	{
		Vector3 edge1=p3-p1;
		Vector3 edge2=p2-p1;
		Vector3 normal;
		D3DXVec3Cross(&normal,&edge1,&edge2);
		D3DXVec3Normalize(&normal,&normal);

		out[0]=normal.x;
		out[1]=normal.x;
		out[2]=normal.x;
		out[3]=-D3DXVec3Dot(&p1,&normal);
	}


	//判断一条线段是否与AABB相交  add by chenliang
	bool AABBox::IntersectsWithLine(Vector3& vPoint1, Vector3& vPoint2)
	{
		Vector3 vLineMiddle=(vPoint1+vPoint2)/2.0f;
		Vector3 vLineVect=vPoint2-vPoint1;
		D3DXVec3Normalize(&vLineVect,&vLineVect);
		Vector3 t1=vPoint1-vPoint2;
		float fHalfLength=D3DXVec3Length(&t1)/2.0f;

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
			if(IntersectsTri(v1,v2,v3,lines[i]))
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

	void AABBox::GetCorners( Vector3 pt[8] ) const
	{
		/** 
		顶部
			6-----7 max
			|     |
		min	4-----5 

		底部
			3-----2 max
			|     |
		min	0---- 1 

		*/

		pt[0]=min;
		pt[1]=Vector3(max.x,min.y,min.z);
		pt[2]=Vector3(max.x,min.y,max.z);
		pt[3]=Vector3(min.x,min.y,max.z);


		pt[4]=Vector3(min.x,max.y,min.z);
		pt[5]=Vector3(max.x,max.y,min.z);
		pt[6]=Vector3(min.x,max.y,max.z);
		pt[7]=max;
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

	bool AABBox::Contain( const AABBox& rhs )
	{
		return max.x>=rhs.max.x
			&& max.y>=rhs.max.y
			&& max.z>=rhs.max.z
			&& min.x<=rhs.min.x
			&& min.y<=rhs.min.y
			&& min.z<=rhs.min.z;
	}

	bool AABBox::Intersects( const AABBox& other ) const
	{
		// Must be overlap along all three axes.
		// Try to find a separating axis.
		if(min.x > other.max.x
			|| max.x < other.min.x)
			return false;
		if(min.y > other.max.y
			|| max.y < other.min.y)
			return false;
		if(min.z > other.max.z
			|| max.z < other.min.z)
			return false;
		return true;
	}

	bool AABBox::Intersects( const AABBox& other,AABBox& intersectBox )
	{
		if(!Intersects(other))
			return false;

		////--差距范围在5.0f之内，还是使用原来的值
		//float offset=1.0f;
		//if( fabsf(min.x-other.min.x)<offset )
		//	intersectBox.min.x=min.x;
		//else
		//	intersectBox.min.x=max(min.x,other.min.x);

		//if( fabsf(min.y-other.min.y)<offset )
		//	intersectBox.min.y=min.y;
		//else
		//	intersectBox.min.y=max(min.y,other.min.y);

		//if( fabsf(min.z-other.min.z)<offset )
		//	intersectBox.min.z=min.z;
		//else
		//	intersectBox.min.z=max(min.z,other.min.z);

		//if( fabsf(max.x-other.max.x)<offset )
		//	intersectBox.max.x=max.x;
		//else
		//	intersectBox.max.x=min(max.x,other.max.x);

		//if( fabsf(max.y-other.max.y)<offset )
		//	intersectBox.max.y=max.y;
		//else
		//	intersectBox.max.y=min(max.y,other.max.y);

		//if( fabsf(max.z-other.max.z)<offset )
		//	intersectBox.max.z=max.z;
		//else
		//	intersectBox.max.z=min(max.z,other.max.z);

		intersectBox.min.x=max(min.x,other.min.x);
		intersectBox.min.y=max(min.y,other.min.y);
		intersectBox.min.z=max(min.z,other.min.z);
		intersectBox.max.x=min(max.x,other.max.x);
		intersectBox.max.y=min(max.y,other.max.y);
		intersectBox.max.z=min(max.z,other.max.z);

		return true;
	}

	bool IntersectsTri( Vector3& v1,Vector3& v2,Vector3& v3,const Line& line )
	{
		Vector3 dir=line.to-line.origin;
		float dist;
		float length=D3DXVec3Length(&dir);
		dir/=length;
		if(D3DXIntersectTri(&v1,&v2,&v3,&line.origin,&dir,NULL,NULL,&dist))
		{
			if(dist<length)
				return true;
		}

		return false;
	}
}//endof namespace Cool3D