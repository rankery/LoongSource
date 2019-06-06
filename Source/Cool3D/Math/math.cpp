#include "stdafx.h"
#include "math.h"

namespace Cool3D
{
	void MatrixPerspectiveFovLH_INF(Matrix4 *pOut,float fov,float aspect, float znear)
	{
		memset(pOut,0,sizeof(Matrix4));
		float h=1.0f/tanf(fov*0.5f);
		float w=h*aspect;

		pOut->_11=w;
		pOut->_22=h;
		pOut->_33=1.0f;
		pOut->_34=1.0f;
		pOut->_43=-znear;
	}

	void GetTranAndQuatFromMatrix4(const Matrix4& mat, Vector3& tran, Quaternion& quat)
	{
		float m11=mat._11,m12=mat._12,m13=mat._13;
		float m21=mat._21,m22=mat._22,m23=mat._23;
		float m31=mat._31,m32=mat._32,m33=mat._33;
		tran.x=mat._41;
		tran.y=mat._42;
		tran.z=mat._43;

		float w,x,y,z;

		float fourWSquaredMinus1=m11+m22+m33;
		float fourXSquaredMinus1=m11-m22-m33;
		float fourYSquaredMinus1=m22-m11-m33;
		float fourZSquaredMinus1=m33-m11-m22;

		// Determin witch of w,x,y,or z has the largest absolute value
		int biggestIndex=0;
		float fourBiggestSquaredMinus1=fourWSquaredMinus1;
		if(fourXSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1=fourXSquaredMinus1;
			biggestIndex=1;
		}
		if(fourYSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1=fourYSquaredMinus1;
			biggestIndex=2;
		}
		if(fourZSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1=fourZSquaredMinus1;
			biggestIndex=3;
		}

		// Perform square root and division
		float biggestVal=sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
		float mult=0.25f/biggestVal;

		// Apply table to compute quaternion values
		switch(biggestIndex)
		{
		case 0:
			w=biggestVal;
			x=(m23-m32)*mult;
			y=(m31-m13)*mult;
			z=(m12-m21)*mult;
			break;
		case 1:
			x=biggestVal;
			w=(m23-m32)*mult;
			y=(m12+m21)*mult;
			z=(m31+m13)*mult;
			break;
		case 2:
			y=biggestVal;
			w=(m31-m13)*mult;
			x=(m12+m21)*mult;
			z=(m23+m32)*mult;
			break;
		case 3:
			z=biggestVal;
			w=(m12-m21)*mult;
			x=(m31+m13)*mult;
			y=(m23+m32)*mult;
			break;
		}
		quat.w=w;
		quat.x=x;
		quat.y=y;
		quat.z=z;
	}

	void GetEulerAngFromQuatO2I(const Quaternion&q, float* yaw, float* pitch, float* roll)
	{
		float sp = -2.0f * (q.y*q.z - q.w*q.x);

		// Check for Gimbal lock, giving slight tolerance for numerical imprecision
		if (fabs(sp) > 0.9999f)
		{
			// Looking straight up or down
			*pitch = 1.570796f * sp;	// pi/2

			// Compute heading, slam bank to zero
			*yaw = atan2(-q.x*q.z + q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
			*roll = 0.0f;
		}
		else
		{
			// Compute angles
			*pitch = asin(sp);
			*yaw = atan2(q.x*q.z + q.w*q.y, 0.5f - q.x*q.x - q.y*q.y);
			*roll = atan2(q.x*q.y + q.w*q.z, 0.5f - q.x*q.x - q.z*q.z);
		}
	}

	void GetEulerAngFromQuatI2O(const Quaternion&q, float* yaw, float* pitch, float* roll)
	{
		float sp = -2.0f * (q.y*q.z + q.w*q.x);

		// Check for Gimbal lock, giving slight tolerance for numerical imprecision
		if (fabs(sp) > 0.9999f)
		{
			// Looking straight up or down
			*pitch = 1.570796f * sp;	// pi/2

			// Compute heading, slam bank to zero
			*yaw = atan2(-q.x*q.z - q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
			*roll = 0.0f;
		}
		else
		{
			// Compute angles
			*pitch = asin(sp);
			*yaw = atan2(q.x*q.z - q.w*q.y, 0.5f - q.x*q.x - q.y*q.y);
			*roll = atan2(q.x*q.y - q.w*q.z, 0.5f - q.x*q.x - q.z*q.z);
		}
	}

	inline void sinCos(float *returnSin, float *returnCos, float theta)
	{
		// For simplicity, we'll just use the normal trig functions
		// Note that on some platforms we may be able to do better

		*returnSin = sin(theta);
		*returnCos = cos(theta);
	}

	Quaternion RotateObjectToInertial(float yaw, float pitch, float roll)
	{
		Quaternion q;
		float sPitch, sRoll, sYaw;
		float cPitch, cRoll, cYaw;

		sinCos(&sPitch, &cPitch, pitch*0.5f);
		sinCos(&sRoll, &cRoll, roll*0.5f);
		sinCos(&sYaw, &cYaw, yaw*0.5f);

		// Compute values
		q.w = cYaw*cPitch*cRoll + sYaw*sPitch*sRoll;
		q.x = cYaw*sPitch*cRoll + sYaw*cPitch*sRoll;
		q.y = -cYaw*sPitch*sRoll + sYaw*cPitch*cRoll;
		q.z = -sYaw*sPitch*cRoll + cYaw*cPitch*sRoll;

		return q;
	}

	Quaternion RotateInertialToObject(float yaw, float pitch, float roll)
	{
		Quaternion q;
		float sPitch, sRoll, sYaw;
		float cPitch, cRoll, cYaw;

		sinCos(&sPitch, &cPitch, pitch*0.5f);
		sinCos(&sRoll, &cRoll, roll*0.5f);
		sinCos(&sYaw, &cYaw, yaw*0.5f);

		// Compute values
		q.w = cYaw*cPitch*cRoll + sYaw*sPitch*sRoll;
		q.x = -cYaw*sPitch*cRoll - sYaw*cPitch*sRoll;
		q.y = cYaw*sPitch*sRoll - sYaw*cPitch*cRoll;
		q.z = sYaw*sPitch*cRoll - cYaw*cPitch*sRoll;

		return q;
	}

	Quaternion GetRotationTo(const Vector3& from, const Vector3& to)
	{
		Quaternion q;
		//* NOTE: assumes normalized input
		Vector3 v1 = from;
		D3DXVec3Normalize(&v1, &v1);

		Vector3 v2 = to;
		D3DXVec3Normalize(&v2, &v2);

		float tx,ty,tz,temp,dist;
		float cost,len,ss;

		// get dot product of two vectors
		cost=v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;

		// check if parallel
		if (cost > 0.999999f)
		{
			D3DXQuaternionIdentity(&q);
			return q;
		}
		else if (cost < -0.999999f)
		{
			// check if opposite

			// check if we can use cross product of from vector with [1, 0, 0]
			tx=0.0f;
			ty=v1[0];
			tz= -v1[1];

			len=sqrtf(ty*ty + tz*tz);

			if (len < 1e-6f)
			{
				// no, we need cross product of from vector with [0, 1, 0]
				tx= -v1[2];
				ty=0.0f;
				tz=v1[0];
			}

			// normalize
			temp=tx*tx + ty*ty + tz*tz;

			dist=(float)(1.0f/sqrtf(temp));

			tx*=dist;
			ty*=dist;
			tz*=dist;

			q.w = 0.0f;
			q.x = tx;
			q.y = ty;
			q.z = tz;

			return q;
		}

		// else just cross two vectors

		tx=v1[1]*v2[2] - v1[2]*v2[1];
		ty=v1[2]*v2[0] - v1[0]*v2[2];
		tz=v1[0]*v2[1] - v1[1]*v2[0];

		temp=tx*tx + ty*ty + tz*tz;
		dist=1.0f/sqrtf(temp);

		tx*=dist;
		ty*=dist;
		tz*=dist;

		// use half-angle formula (sin^2 t=( 1 - cos(2t) )/2)

		ss=sqrtf(0.5f * (1.0f - cost));

		tx*=ss;
		ty*=ss;
		tz*=ss;

		// cos^2 t=( 1 + cos (2t) ) / 2
		// w part is cosine of half the rotation angle
		q.w = sqrtf(0.5f * (1.0f + cost));
		q.x = tx;
		q.y = ty;
		q.z = tz;

		return q;
	}

	void SetPrjZScaleOffset(Matrix4& matPrj,float zScale,float zOffset)
	{
		matPrj._33=matPrj._33*zScale+zOffset;
		matPrj._43=matPrj._43*zScale;
	}
}//namespace Cool3D