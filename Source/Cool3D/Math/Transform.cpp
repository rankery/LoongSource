#include "StdAfx.h"
#include ".\transform.h"
#include "..\Util\Exception.h"

namespace Cool3D
{
	Transform::Transform(void):m_mode(ECM_ToLeft)
	{
		LoadIdentity();
	}

	Transform::Transform(ECombineMode mode):m_mode(mode)
	{
		LoadIdentity();
	}

	Transform::~Transform()
	{
	}

	void Transform::LoadIdentity()
	{
		D3DXMatrixIdentity(&m_mat);
	}

	void Transform::Translate(const Vector3& offset)
	{
		if(m_mode==ECM_ToLeft)
		{
			m_mat._41 += offset.x; 
			m_mat._42 += offset.y; 
			m_mat._43 += offset.z;
		}
		else
		{
			Matrix4 mat;
			D3DXMatrixTranslation(&mat,offset.x,offset.y,offset.z);
			D3DXMatrixMultiply(&m_mat,&m_mat,&mat);//(Out = M1 * M2).
		}
	}

	void Transform::Rotate(float yaw,float pitch,float roll)
	{
		Matrix4 mat;
		D3DXMatrixRotationYawPitchRoll(&mat,yaw,pitch,roll);

		if(m_mode==ECM_ToLeft)
			D3DXMatrixMultiply(&m_mat,&mat,&m_mat);//(Out = M1 * M2).
		else
			D3DXMatrixMultiply(&m_mat,&m_mat,&mat);//(Out = M1 * M2).
	}

	void Transform::RotateQuat(const Quaternion& q)
	{
		Matrix4 mat;
		D3DXMatrixRotationQuaternion(&mat,&q);

		if(m_mode==ECM_ToLeft)
			D3DXMatrixMultiply(&m_mat,&mat,&m_mat);//(Out = M1 * M2).
		else
			D3DXMatrixMultiply(&m_mat,&m_mat,&mat);//(Out = M1 * M2).
	}

	void Transform::RotateAxis(char axis,float rad)
	{
		Matrix4 mat;
		switch(axis)
		{
		case 'X':
		case 'x':
			D3DXMatrixRotationX(&mat,rad);
			break;
		case 'Y':
		case 'y':
			D3DXMatrixRotationY(&mat,rad);
			break;
		case 'Z':
		case 'z':
			D3DXMatrixRotationZ(&mat,rad);
			break;
		default:
			ASSERT(0 && "axis must be x,y or z");
		}

		if(m_mode==ECM_ToLeft)
			D3DXMatrixMultiply(&m_mat,&mat,&m_mat);//(Out = M1 * M2).
		else
			D3DXMatrixMultiply(&m_mat,&m_mat,&mat);//(Out = M1 * M2).
	}

	void Transform::RotateAxis(const Vector3& axis,float rad)
	{
		Matrix4 mat;
		D3DXMatrixRotationAxis(&mat,&axis,rad);

		if(m_mode==ECM_ToLeft)
			D3DXMatrixMultiply(&m_mat,&mat,&m_mat);//(Out = M1 * M2).
		else
			D3DXMatrixMultiply(&m_mat,&m_mat,&mat);//(Out = M1 * M2).
	}

	void Transform::Scale(float x,float y,float z)
	{
		Matrix4 mat;
		D3DXMatrixScaling(&mat,x,y,z);

		if(m_mode==ECM_ToLeft)
			D3DXMatrixMultiply(&m_mat,&mat,&m_mat);//(Out = M1 * M2).
		else
			D3DXMatrixMultiply(&m_mat,&m_mat,&mat);//(Out = M1 * M2).
	}

	void Transform::CombineTrans(const Matrix4* pMatrix)
	{
		ASSERT(pMatrix!=NULL);
		if(m_mode==ECM_ToLeft)
			D3DXMatrixMultiply(&m_mat,pMatrix,&m_mat);
		else
			D3DXMatrixMultiply(&m_mat,&m_mat,pMatrix);
	}
	void Transform::CombineTransRight(const Matrix4* pMatrix)
	{
		ASSERT(pMatrix!=NULL);
		m_mat=m_mat*(*pMatrix);
	}
	void Transform::LoadMatrix(const Matrix4* pMatrix)
	{
		ASSERT(pMatrix!=NULL);
		m_mat=*pMatrix;
	}

	void Transform::TransVec3(Vector3& v)
	{
		/*Vector4 tmp;
		D3DXVec3Transform(&tmp,&v,&m_mat);
		v.x=tmp.x;
		v.y=tmp.y;
		v.z=tmp.z;
		*/
		v=v*m_mat;
	}
}//namespace Cool3D