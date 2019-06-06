
#include "..\Cool3D.h"
#include "math.h"
#pragma once

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

	class Cool3D_API Plane
	{
	public:
		Vector3 normal;
		float	dist;

		Plane():normal(0,1,0),dist(0)
		{}
		Plane(const Vector3& point,const Vector3& _normal);
		
		void Normalize();
		float DistToPoint(const Vector3& pos) const;
		Halfspace ClassifyPoint(const Vector3& pos) const;
	};

	struct Line
	{
		Vector3 origin;
		Vector3 to;

		bool IntersectsTri(Vector3& v1,Vector3& v2,Vector3& v3) const;
	};

	//--bounding box
	class Cool3D_API AABBox
	{
	public:
		Vector3 min;
		Vector3 max;

		AABBox()
		{
			min=Vector3(0,0,0);
			max=Vector3(1,1,1);
			
		}

		void GetCorners(Vector3 pt[8]) const;
		/** Transforms the box according to the matrix supplied.
		    @remarks
			    By calling this method you get the axis-aligned box which
			    surrounds the transformed version of this box. Therefore each
			    corner of the box is transformed by the matrix, then the
			    extents are mapped back onto the axes to produce another
			    AABB. Useful when you have a local AABB for an object which
			    is then transformed.
	    */
	    void Transform( const Matrix4& matrix );
		void TransformFast( const Matrix4& matrix,const Vector3& scale );
		/** Returns true if there is any overlap */
		__forceinline bool Intersects(const AABBox& other) const;
		__forceinline bool IntersectsXZ(const AABBox& other) const;
		__forceinline bool Intersects(const AABBox& other,AABBox& intersectBox) const;
		/** Merges the passed in box into the current box. The result is the
		    box which encompasses both.
	    */
	    void Merge( const AABBox& rhs );
		void Merge( const Vector3& v );
		bool Contain( const AABBox& rhs ) const;
		void GetLines(Line lines[4]) const;

		bool Contain( const Vector3& pt )
		{
			return pt.x<=max.x && pt.y<=max.y && pt.z<=max.z
				&& pt.x>=min.x && pt.y>=min.y && pt.z>=min.z;
		}

		bool Contain( const Vector2& pt )
		{
			return pt.x<=max.x && pt.y<=max.z
				&& pt.x>=min.x && pt.y>=min.z;
		}

		Vector3 GetCenter() const
		{
			return Vector3(max.x+min.x,max.y+min.y,max.z+min.z)*0.5f;
		}

		//判断一条线段是否与AABB相交  add by chenliang
		bool IntersectsWithLine(Vector3& vPoint1, Vector3& vPoint2);

		bool IntersectsTriangle(Vector3& v1,Vector3& v2,Vector3& v3);

		//判断垂直线是否与this AABB顶面相交
		bool TopIntersectsWithVLine(const Vector3& startPos,float height,Vector3& intersectPos) const;
		//判断other AABB是否与this AABB顶面相交
		__forceinline bool AABBoxCollideBoxTopHighest(const AABBox& other,float& intersectY) const;

		//判断垂直线是否与AABB相交
		bool IntersectsWithVLine(const Vector3& startPos,float height) const;

		float DistToPoint(const Vector3& point) const;
		float DistToPointSq(const Vector3& point) const;

		/**使用AABBox剪裁多变形，生成的顶点存入newVertex中,外部调用时应事先给newVertex分配9个顶点的内存
		   （因为一个三角形被6个面剪裁最多会生成9个顶点），返回值为剪裁过后面的顶点数目
		*/
		int ClipTriangle(const Vector3* tri, Vector3 *newVertex) const;
	private:
		/**单个面剪裁，返回定点数目
		*/
		int ClipPolygonAgainstPlane( const int nPlaneIndex, const int vertexCount, const Vector3 *vertex, Vector3 *newVertex ) const;
		/**求1个点到AABBox指定面的有向距离
		*/
		inline float VecToAABBoxPlaneDist( const Vector3& vec, const int nPlaneIndex ) const;
		/**求2个点在AABBox指定面的法线方向的距离
		*/
		inline float AABBoxPlaneDist( const Vector3& vec1, const Vector3& vec2, const int nPlaneIndex ) const;

	};

	//--class AABBox
	bool AABBox::Intersects(const AABBox& other) const
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

	bool AABBox::Intersects( const AABBox& other,AABBox& intersectBox ) const
	{
		if(!Intersects(other))
			return false;

		intersectBox.min.x=max(min.x,other.min.x);
		intersectBox.min.y=max(min.y,other.min.y);
		intersectBox.min.z=max(min.z,other.min.z);
		intersectBox.max.x=min(max.x,other.max.x);
		intersectBox.max.y=min(max.y,other.max.y);
		intersectBox.max.z=min(max.z,other.max.z);

		return true;
	}

	bool AABBox::IntersectsXZ(const AABBox& other) const
	{
		if(min.x > other.max.x
			|| max.x < other.min.x)
			return false;
		if(min.z > other.max.z
			|| max.z < other.min.z)
			return false;
		return true;
	}

	bool AABBox::AABBoxCollideBoxTopHighest( const AABBox& other,float& intersectY ) const
	{
		if(min.x > other.max.x
			|| max.x < other.min.x)
			return false;
		if(min.z > other.max.z
			|| max.z < other.min.z)
			return false;
		if(max.y > other.max.y
			|| max.y < other.min.y)//this的maxy在other的miny和maxy之间
			return false;

		if(max.y<=intersectY)
			return false;

		intersectY=max.y;
		return true;
	}

	//--Triangle
	class Cool3D_API Triangle
	{
	public:
		Vector3 a;
		Vector3 b;
		Vector3 c;

		Triangle(){}
		~Triangle(){}

		void BuildAABBox(AABBox& box);
	};

	//--OBBox
	class Cool3D_API OBBox
	{
	public:
		Matrix4 trans;
		Vector3 size;
	};
	class Cool3D_API Sphere
	{
	public:
		Vector3	centre;
		float	radius;

		Sphere():centre(0,0,0),radius(1)
		{}

		/** Returns whether or not this sphere interects another sphere. */
		bool Intersect(const Sphere& s) const
		{
			return Vec3Dist(centre,s.centre) <=	(s.radius + radius);
		}
	};

	struct Cool3D_API QuadFace
	{
		Vector3	pt[4];
	};

	struct Cool3D_API Rectf
	{
		float top,left,bottom,right;
	};

	/** Sphere / box intersection test. */
    bool Intersect(const Sphere& sphere, const AABBox& box);
	/** Plane / box intersection test. */
    bool Intersect(const Plane& plane, const AABBox& box);
	/** Sphere / plane intersection test. 
        @remarks NB just do a plane.getDistance(sphere.getCenter()) for more detail!
        */
	bool Intersect(const Sphere& sphere, const Plane& plane);

	/**	计算一个三角形的中心点	*/
	Cool3D_API void TriangleCentroid(const Vector3& p1,const Vector3& p2,const Vector3& p3,Vector3& out);
	/**	计算一个三角形的面积	*/
	Cool3D_API float TriangleArea(const Vector3& p1,const Vector3& p2,const Vector3& p3);
	Cool3D_API void BuildPlane(const Vector3& p1,const Vector3& p2,const Vector3& p3,Plane& out);

	class Cool3D_API MathConst
	{
	public:
		MathConst();
		~MathConst();

	public:
		static Vector3			AxisX;
		static Vector3			AxisY;
		static Vector3			AxisZ;
		static Vector3			Zero3;
		static Matrix4			IdentityMat;
		static Matrix4			RH2LHMat;//右手系坐标转换到左手系坐标
		static Matrix4			LH2RHMat;//左手系坐标转换到右手系坐标
		static AABBox			SmallBox;
		static AABBox			BigBox;
	};

	struct Cool3D_API Point
	{
	public:
		int x,y;

		Point(){}
		Point(int _x,int _y):x(_x),y(_y)
		{}
		~Point(){}
	};
}//endof namespace Cool3D
