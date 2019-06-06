#ifndef MAX_MATH_H
#define MAX_MATH_H
#include "..\MaxPublic\MathBaseDefine.h"
#include "Max.h"
#include "decomp.h"

inline void Point3ToVector3(const Point3& in,Cool3D::Vector3& out)
{
	out.x=in.x;
	out.y=in.y;
	out.z=in.z;
}

inline void Point3ToVector3_D3D(const Point3& in,Cool3D::Vector3& out)
{
	out.x=in.x;
	out.y=in.z;
	out.z=in.y;
}
inline void Point3ToUV_D3D(const Point3& in,Cool3D::Vector2& out)
{
	out.x=in.x;
	out.y=1.0f-in.y;
}

// Determine wether the node has negative scaling(mirrored).
// This is used for mirrored objects for example. They have a negative scale factor
// so when calculating the normal we should take the vertices counter clockwise.
// If we don't compensate for this these objects will be 'inverted'.
//
//judge whether scaling is negative.
//in 3dmax,if it is true,the order of every triangle's vertex has to be counterer clockwise
inline bool Max_TMNegParity( const Matrix3 &m )
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?true:false;
}

inline Matrix3 Transpose(Matrix3& matrix)
{
	float transpose[4][3];
	
	int row;
	for(row = 0; row < 3; row++)
	{
		int column;
		for(column = 0; column < 3; column++)
		{
			transpose[column][row] = matrix.GetAddr()[row][column];
		}
	}
	
	int column;
	for(column = 0; column < 3; column++)
	{
		transpose[3][column] = 0;
	}
	
	Matrix3 transposeMatrix(transpose);
	transposeMatrix.NoTrans();
	
	return transposeMatrix;
}

inline void Max_GetAniRange(Interface *i,TimeValue& start,TimeValue& end)	
{
	Interval animRange = i->GetAnimRange();
	start=animRange.Start();
	end=animRange.End();
}

inline Matrix3 Max_GetBoneNodeTM(INode *pNode,TimeValue time)
{
	// initialize matrix with the identity
	Matrix3 tm;
	tm.IdentityMatrix();

	// only do this for valid nodes
	if(pNode != NULL)
	{
		// get the node transformation
		tm = pNode->GetNodeTM(time);

		// make the transformation uniform
		tm.NoScale();
	}

	return tm;
}

inline bool Max_GetBoneNodeInvTM(INode *pNode,TimeValue time,Cool3D::Vector3& outTrans,Cool3D::Quaternion& outQuat)
{
	// calculate the relative transformation
	Matrix3 tm;
	tm = Inverse(Max_GetBoneNodeTM(pNode, time));

	// calculate the translation and rotation  component
	Point3 p;
	Quat q;
	AffineParts ap;
	decomp_affine(tm,&ap);
	p=ap.t;
	q=ap.q;

	//--convert to D3D
	outTrans.x=p.x;
	outTrans.y=p.z;
	outTrans.z=p.y;


	Point3 axis;
	float ang;
	q.Normalize();
	AngAxisFromQ(q,&ang,axis);
	Point3 axisD3D(axis.x,axis.z,axis.y);
	q=QFromAngAxis(ang,axisD3D);
	q.Normalize();
	outQuat.x=q.x;
	outQuat.y=q.y;
	outQuat.z=q.z;
	outQuat.w=q.w;

	if(ap.f<0)
		return true;
	else
		return false;
}

inline bool Max_GetNodeLocalAni(INode *pNode,INode *pParentNode,TimeValue time
								,Cool3D::Vector3& outTrans,Cool3D::Quaternion& outQuat)
{
	// calculate the relative transformation
	Matrix3 tm;
	if(pParentNode!=NULL)
		tm = Max_GetBoneNodeTM(pNode, time) * Inverse(Max_GetBoneNodeTM(pParentNode, time));
	else
		tm = Max_GetBoneNodeTM(pNode, time);

	// calculate the translation and rotation  component
	Point3 p;
	Quat q;
	AffineParts ap;
	decomp_affine(tm,&ap);
	p=ap.t;
	q=ap.q;
	
	//--convert to D3D
	outTrans.x=p.x;
	outTrans.y=p.z;
	outTrans.z=p.y;
	

	Point3 axis;
	float ang;
	q.Normalize();
	AngAxisFromQ(q,&ang,axis);
	Point3 axisD3D(axis.x,axis.z,axis.y);
	q=QFromAngAxis(ang,axisD3D);
	q.Normalize();
	outQuat.x=q.x;
	outQuat.y=q.y;
	outQuat.z=q.z;
	outQuat.w=q.w;

	if(ap.f<0)
		return true;
	else
		return false;
}

#endif //#ifndef MAX_MATH_H