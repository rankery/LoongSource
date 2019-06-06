#include "StdAfx.h"
#include "creature_order.h"

// constructor&&destructor
CVector2D::CVector2D()
{
	fx=0.0;
	fy=0.0;
}

CVector2D::CVector2D(FLOAT ix,FLOAT iy)
{
	fx=ix;
	fy=iy;
}

CVector2D::CVector2D(const FLOAT* pv)
{
	fx=pv[0];
	fy=pv[1];
}

CVector2D::CVector2D(VECTOR2D v)
{	
	fx=v.fx;
	fy=v.fy;
}

CVector2D::~CVector2D()
{
}

CVector2D CVector2D::operator+(VECTOR2D v) const
{
	return CVector2D(fx+v.fx,fy+v.fy);
}

CVector2D CVector2D::operator-(VECTOR2D v) const
{
	return CVector2D(fx-v.fx,fy-v.fy);
}

VOID CVector2D::operator+=(VECTOR2D v)
{
	fx += v.fx;
	fy += v.fy;
}

VOID CVector2D::operator-=(VECTOR2D v)
{
	fx-=v.fx;
	fy-=v.fy;
}

CVector2D CVector2D::operator*(FLOAT f) const
{
	return CVector2D(fx*f,fy*f);
}

VOID CVector2D::operator*=(FLOAT f)
{
	fx *= f;
	fy *= f;
}

CVector2D CVector2D::operator/(FLOAT f) const
{
	return CVector2D(fx/f,fy/f);
}

VOID CVector2D::operator/=(FLOAT f)
{
	fx /= f;
	fy /= f;
}

//matrix transform
CVector2D CVector2D::operator*(const MATRIX2D& matrix) const
{
	FLOAT x,y;
	x = fx*matrix.A[0][0]+fy*matrix.A[1][0];
	y = fx*matrix.A[0][1]+fy*matrix.A[1][1];

	return CVector2D(x,y);
}

VOID CVector2D::operator*=(const MATRIX2D& matrix)
{
	(*this)=(*this)*matrix;
}

FLOAT CVector2D::GetLength() const
{
	return sqrt(fx*fx+fy*fy);
}

CVector2D CVector2D::GetNormal() const
{
	FLOAT len = GetLength();
	return CVector2D(fx/len,fy/len);
}

VOID CVector2D::Normalize()
{
	FLOAT len = GetLength();
	fx = fx/len;
	fy = fy/len;
}

VOID CVector2D::Vec2DRotateAroundOrigin(FLOAT ang)
{
	CMatrix2D mat;

	//rotate
	mat = CMatrix2D::CreateRotateMatrix(ang);

	//now transform the object's vertices
	*this = mat * (*this) ;
}

//construction&&destruction
CMatrix2D::CMatrix2D()
{
	for(int i=0;i<2;i++)
		for(int j=0;j<2;j++){
			if(i==j)
				A[i][j] = 1.0;
			else
				A[i][j] = 0.0;
		}
}

CMatrix2D::CMatrix2D(const MATRIX2D& matrix)
{
	for(int i=0;i<2;i++)
		for(int j=0;j<2;j++)
			A[i][j]=matrix.A[i][j];
}

CMatrix2D::CMatrix2D(const FLOAT* matrix)
{
	for(int i=0;i<2;i++)
		for(int j=0;j<2;j++)
			A[i][j]=matrix[i*2+j];
}

CMatrix2D::CMatrix2D(FLOAT f1, FLOAT f2, FLOAT f3, FLOAT f4)
{
	A[0][0] = f1;
	A[0][1] = f2;
	A[1][0] = f3;
	A[1][1] = f4;
}

CMatrix2D::~CMatrix2D()
{
}

//operators
CMatrix2D CMatrix2D::operator*(const MATRIX2D& matrix2) const
{
	CMatrix2D matrix;
	for(int i=0;i<2;i++)
		for(int j=0;j<2;j++){
			matrix.A[i][j] = A[i][0]*matrix2.A[0][j]
			+ A[i][1]*matrix2.A[1][j];
		}
		return matrix;
}

VOID CMatrix2D::operator*=(const MATRIX2D& matrix2)
{
	(*this)=(*this)*matrix2;
}

CVector2D CMatrix2D::operator*(const CVector2D& v)
{
	CVector2D vector;

	vector.fx = A[0][0]*v.fx + A[0][1]*v.fy;
	vector.fy = A[1][0]*v.fx + A[1][1]*v.fy;
	
	return vector;
}

//methods
VOID CMatrix2D::IdenticalMatrix()
{
	for(int i=0;i<2;i++)
		for(int j=0;j<2;j++){
			if(i==j)
				A[i][j] = 1.0;
			else
				A[i][j] = 0.0;
		}
}

FLOAT CMatrix2D::GetValue() const
{
	return A[0][0]*A[1][1]-A[0][1]*A[1][0];
}

// static member functions
FLOAT CMatrix2D::GetValue(FLOAT a00, FLOAT a01,
						   FLOAT a10, FLOAT a11)
{
	return a00*a11-a01*a10;
}

CMatrix2D CMatrix2D::CreateMirrorMatrix(VECTOR2D v)
{
	FLOAT len=((CVector2D)v).GetLength();
	CMatrix2D matrix;
	matrix.A[0][0]=(v.fx*v.fx - v.fy*v.fy)/len;
	matrix.A[0][1]=2.0f*(v.fx*v.fy)/len;
	matrix.A[1][0]=2.0f*(v.fx*v.fy)/len;
	matrix.A[1][1]= -matrix.A[0][0];
	return matrix;
}

CMatrix2D CMatrix2D::CreateRotateMatrix(CVector2D v)
{
	FLOAT len=v.GetLength();
	CMatrix2D m;
	m.A[0][0]=m.A[1][1]=v.fx/len;
	m.A[0][1]=v.fy/len;m.A[1][0]= -v.fy/len;
	return m;
}

CMatrix2D CMatrix2D::CreateScaleMatrix(FLOAT f)
{
	CMatrix2D m;
	m.A[0][0]=m.A[1][1]=f;
	return m;
}

CMatrix2D CMatrix2D::CreateRotateMatrix(FLOAT angle)
{
	CMatrix2D m;
	m.A[0][0]=m.A[1][1]=cos(angle);
	m.A[0][1]=sin(angle);m.A[1][0]= -sin(angle);
	return m;
}
