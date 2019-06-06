#include "StdAfx.h"
#include ".\mmuvtrans.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MMUVTrans,IMtlModifier);

	MMUVTrans::MMUVTrans(void)
	{
		D3DXMatrixIdentity(&m_mat);
	}

	MMUVTrans::~MMUVTrans(void)
	{
	}

	/**
	s  0  0  x
	0  t  0  x  // s is x scaling, t is y scaling.
	a  b  1  x  // a is x translation, b is y translation.
	x  x  x  x  // Outer values are don't cares.

	For some reason 2D texture coords are viewed by the hardware as 
	(u,v,1) and not (u,v,0,1). Therefore you need to put the translation 
	into _31 and _32 in the matrix (create an identity matrix and put the 
	values directly into it). If you use 3D texture coordinates (such as 
	those generated automatically with D3DTSS_TCI_...), the matrices will 
	work as normal.

	Eyal
	*/
	void MMUVTrans::SetScale(float uScale,float vScale)
	{
		m_mat._11=uScale;
		m_mat._22=vScale;
	}

	void MMUVTrans::SetOffset(float uOffset,float vOffset)
	{
		m_mat._31=uOffset;
		m_mat._32=vOffset;
	}

	void MMUVTrans::SetRotate(float rot)
	{
		float c=cosf(rot);
		float s=sinf(rot);
		m_mat._11=c;
		m_mat._12=s;

		m_mat._21=-s;
		m_mat._22=c;
	}

	void MMUVTrans::Serialize(FArchive& ar)
	{
		ar<<m_mat;
		IMtlModifier::Serialize(ar);
	}
	void MMUVTrans::Deserialize(FArchive& ar)
	{
		ar>>m_mat;
		IMtlModifier::Deserialize(ar);
	}
}//namespace Cool3D