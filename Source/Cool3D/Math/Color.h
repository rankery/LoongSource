
#pragma once
#include "..\Cool3D.h"
#include "Math.h"
#pragma warning( disable : 4244 )

namespace Cool3D
{
	class Cool3D_API Color4f;
	class Cool3D_API Color4ub
	{
	public:
		unsigned char R,G,B,A;

		Color4ub()
		{	A=255;	R=G=B=0; }
		Color4ub(unsigned char A,unsigned char R,unsigned char G,unsigned char B):A(A),R(R),G(G),B(B)
		{}
		Color4ub(DWORD color)
		{
			A=(color>>24);
			R=((color>>16)&0x000000FF);
			G=((color>>8)&0x000000FF);
			B=((color)&0x000000FF);
		}
		inline Color4ub(const Color4f& c4f);

		// casting
		operator DWORD () const
		{	return (A << 24) | (R << 16) | (G << 8) | B;	}
	};

	class Cool3D_API Color4f
	{
	public:
		float R,G,B,A;
		Color4f()
		{ A=1.0f; R=G=B=0.0f; }
		Color4f(float a,float r,float g,float b)
		{ R=r;G=g;B=b;A=a;}
		Color4f(DWORD color)
		{
			A=(color>>24)/255.0f;
			R=((color>>16)&0x000000FF)/255.0f;
			G=((color>>8)&0x000000FF)/255.0f;
			B=((color)&0x000000FF)/255.0f;
		}
		Color4f(const Vector4& vec):R(vec.x),G(vec.y),B(vec.z),A(vec.w)
		{}
		Color4f(const Color4ub& color)
		{
			A=color.A/255.0f;
			R=color.R/255.0f;
			G=color.G/255.0f;
			B=color.B/255.0f;
		}
		bool operator == (const Color4f& other) const
		{	return other.R==R && other.G==G && other.B==B && other.A==A; }

		const Color4f& operator * (float r)
		{
			R*=r;	G*=r;	B*=r;	A*=r;
			return *this;
		}

		const Color4f& operator / (float r)
		{
			R/=r;	G/=r;	B/=r;	A/=r;
			return *this;
		}

		const Color4f& operator *= (const Color4f& other)
		{
			R*=other.R;	G*=other.G;	B*=other.B;	A*=other.A;
			return *this;
		}

		const Color4f& operator *= (float r)
		{
			R*=r;	G*=r;	B*=r;	A*=r;
			return *this;
		}

		const Color4f& operator + (const Color4f& c)
		{
			R+=c.R;
			G+=c.G;
			B+=c.B;
			A+=c.A;
			return *this;
		}

		const Color4f& operator += (const Color4f& c)
		{
			R+=c.R;
			G+=c.G;
			B+=c.B;
			A+=c.A;
			return *this;
		}

		const Color4f& operator /= (float f)
		{
			R/=f;
			G/=f;
			B/=f;
			A/=f;
			return *this;
		}
	};

	inline Color4f operator * (const Color4f& c1,const Color4f& c2)
	{
		Color4f ret;
		ret.R=c1.R*c2.R;
		ret.G=c1.G*c2.G;
		ret.B=c1.B*c2.B;
		ret.A=c1.A*c2.A;
		return ret;
	}

	inline Color4ub::Color4ub(const Color4f& c4f)
	{
		A=c4f.A*255;
		R=c4f.R*255;
		G=c4f.G*255;
		B=c4f.B*255;
	}
}//namespace Cool3D

#pragma warning( default : 4244 )
