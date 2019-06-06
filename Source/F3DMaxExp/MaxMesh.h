#pragma once

#include "MaxMath.h"
#include "MaxVNormal.h"

class NullView : public View 
{		
public:	
	Point2 ViewToScreen(Point3 p)			
	{ return Point2(p.x,p.y); }
	
	NullView() 
	{		
		worldToView.IdentityMatrix();	
		screenW=640.0f; screenH = 480.0f;
	}
};

class MaxMesh
{
public:
	MaxMesh(INode *pNode,TimeValue time)
	{
		m_pINode=pNode;
		ObjectState os=pNode->EvalWorldState(time);
		m_tm=pNode->GetObjectTM(time);
		m_bCCWFace=Max_TMNegParity(m_tm);
		NullView view;
		int needDel;
		m_pMaxMesh=((GeomObject*)os.obj)->GetRenderMesh(time,pNode,view,needDel);
		
		MAX_ComputeVertexNormals(m_pMaxMesh,m_normalArray);
	}
	virtual ~MaxMesh()	{}

	int GetNumMaxVerts()
	{
		return m_pMaxMesh->getNumVerts();
	}
	// get the absolute vertex position
	void Vert_GetPos(int vertId,Point3& pos)
	{
		pos = m_pMaxMesh->getVert(vertId) * m_tm;
	}
	// get the absolute vertex normal
	void Vert_GetNormal(int vertId,Point3& normal)
	{
		normal = normal * Inverse(Transpose(m_tm));
		normal = normal.Normalize();
	}

protected:
	INode				*m_pINode;
	std::vector<NormalVerSm>	m_normalArray;
	Mesh				*m_pMaxMesh;
	Matrix3				m_tm;
	bool				m_bCCWFace;
private:
	MaxMesh(const MaxMesh& mesh);
	const MaxMesh& operator = (const MaxMesh& mesh);
};