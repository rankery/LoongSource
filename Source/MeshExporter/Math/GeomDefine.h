#pragma once
#include "math.h"
#include <vector>
using namespace std;

namespace Cool3D
{
	//一些简单几何体的定义
	//--Plane
	enum Halfspace
	{
		NEGATIVE = -1,
		ON_PLANE = 0,
		POSITIVE = 1
	};

	struct Plane
	{
		Vector3 normal;
		float	dist;
		
		void Normalize();
		float DistToPoint(const Vector3& pos) const;
		Halfspace ClassifyPoint(const Vector3& pos) const;
	};

	struct Line
	{
		Vector3 origin;
		Vector3 to;
	};

	bool IntersectsTri(Vector3& v1,Vector3& v2,Vector3& v3,const Line& line);

	//--bounding box
	struct AABBox
	{
		Vector3 min;
		Vector3 max;

		bool Contain( const Vector3& pt )
		{
			return pt.x<=max.x && pt.y<=max.y && pt.z<=max.z
				&& pt.x>=min.x && pt.y>=min.y && pt.z>=min.z;
		}

		bool Contain( const AABBox& rhs );
		bool Intersects(const AABBox& other) const;
		bool Intersects(const AABBox& other,AABBox& intersectBox);

		void Merge( const AABBox& rhs );
		void GetCorners(Vector3 pt[8]) const;
		void GetLines(Line lines[4]) const;

		//判断一条线段是否与AABB相交  add by chenliang
		bool IntersectsWithLine(Vector3& vPoint1, Vector3& vPoint2);

		bool IntersectsTriangle(Vector3& v1,Vector3& v2,Vector3& v3);

		Vector3 GetCenter() const
		{
			return Vector3(max.x+min.x,max.y+min.y,max.z+min.z)*0.5f;
		}

	};

	struct OBBox
	{
		Matrix4 trans;
		Vector3 size;
	};
	struct Sphere
	{
		Vector3	centre;
		float	radius;
	};

	struct QuadFace
	{
		Vector3	pt[4];
	};

	void TriangleCentroid(const Vector3& p1,const Vector3& p2,const Vector3& p3,Vector3& out);
	void BuildPlane(const Vector3& p1,const Vector3& p2,const Vector3& p3,float out[4]);
}//endof namespace Cool3D
