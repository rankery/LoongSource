#pragma once

#include <vector>

typedef struct tagVector2D{
	FLOAT	fx;
	FLOAT	fy;
} VECTOR2D;

typedef struct tagMatrix2D{
	FLOAT A[2][2];
} MATRIX2D;

class CVector2D : public VECTOR2D
{
public:
	CVector2D();
	CVector2D(FLOAT ix, FLOAT iy=0.0);
	CVector2D(const FLOAT* pv);
	CVector2D(VECTOR2D v);
	virtual ~CVector2D();

public:
	//operators declaration
	CVector2D operator+(VECTOR2D v) const;
	CVector2D operator-(VECTOR2D v) const;
	VOID operator+=(VECTOR2D v);
	VOID operator-=(VECTOR2D v);
	CVector2D operator*(FLOAT f) const;
	VOID operator*=(FLOAT f);
	CVector2D operator/(FLOAT f) const;
	VOID operator/=(FLOAT f);

	//matrix transformation
	CVector2D operator*(const MATRIX2D& matrix ) const;
	VOID operator*=(const MATRIX2D& matrix );

	//methods declaration
	FLOAT GetLength()const;
	CVector2D GetNormal()const;
	VOID  Normalize();
	VOID  Vec2DRotateAroundOrigin(FLOAT);
};

class CMatrix2D : public MATRIX2D
{
public:
	CMatrix2D();
	CMatrix2D(const MATRIX2D&);
	CMatrix2D(FLOAT f1, FLOAT f2, FLOAT f3, FLOAT f4);
	CMatrix2D(const FLOAT *);
	virtual ~CMatrix2D();

	//operators
	CMatrix2D operator*(const MATRIX2D&) const;
	VOID operator*=(const MATRIX2D&);
	CVector2D operator*(const CVector2D&);

	//methods
	void	IdenticalMatrix();
	FLOAT	GetValue() const;
public:
	// static member functions
	static FLOAT GetValue(FLOAT a00, FLOAT a01,
		FLOAT a10, FLOAT a11);
	static CMatrix2D CreateMirrorMatrix(VECTOR2D vect);
	static CMatrix2D CreateRotateMatrix(CVector2D v);
	static CMatrix2D CreateScaleMatrix(FLOAT);
	static CMatrix2D CreateRotateMatrix(FLOAT);
};  







