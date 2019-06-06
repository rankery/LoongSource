#include "StdAfx.h"
#include ".\meshtopo.h"
#include "..\ResSys\VertStream.h"
#include "..\Math\GeomDefine.h"
#include "..\Util\Exception.h"

namespace Cool3D
{
	MeshTopo::tagVert::tagVert()
	{
		index=-1;
		minCostNeighbor=-1;
		bActive=true;
		cost=-1.0;
	}
	void MeshTopo::tagTriangle::ChangeVertex(int vFrom, int vTo)
	{
		ASSERT(vFrom != vTo);
		ASSERT(vFrom == vert[0] || vFrom == vert[1] || vFrom == vert[2]);
		if (vFrom == vert[0]) {
			vert[0] = vTo;
		} 
		else if (vFrom == vert[1]) {
			vert[1] = vTo;
		}
		else if (vFrom == vert[2]) {
			vert[2] = vTo;
		}
		else {
			//!FIX error
			ASSERT(0);
		}
	}

	MeshTopo::MeshTopo(void)
	{
	}

	MeshTopo::~MeshTopo(void)
	{
	}

	void MeshTopo::GetActiveTris(vector<WORD>& indices)
	{
		indices.clear();
		for(size_t i=0;i<m_triArray.size();i++)
		{
			tagTriangle& t=m_triArray[i];
			if(!t.bActive)
				continue;
			indices.push_back(t.vert[0]);
			indices.push_back(t.vert[1]);
			indices.push_back(t.vert[2]);
		}
		
	}

	void MeshTopo::CalcTriPlane(UINT triIndex)
	{
		tagTriangle& tri=GetTri(triIndex);
		int v1=tri.vert[0];
		int v2=tri.vert[1];
		int v3=tri.vert[2];
		Vector3& p1=m_vertArray[v1].pos;
		Vector3& p2=m_vertArray[v2].pos;
		Vector3& p3=m_vertArray[v3].pos;
		BuildPlane(p1,p2,p3,tri.plane);
	}

	float MeshTopo::CalcTriArea(UINT triIndex)
	{
		tagTriangle& tri=GetTri(triIndex);
		int v1=tri.vert[0];
		int v2=tri.vert[1];
		int v3=tri.vert[2];
		Vector3& p1=m_vertArray[v1].pos;
		Vector3& p2=m_vertArray[v2].pos;
		Vector3& p3=m_vertArray[v3].pos;

		return TriangleArea(p1,p2,p3);
	}

	void MeshTopo::Build(VertStream *pVS,WORD *pTriListIndices,UINT numTri)
	{
		ASSERT(pVS!=NULL);
		ASSERT(pTriListIndices!=NULL);
		ASSERT(numTri > 0);

		int i;
		//--copy vert pos
		for(i=0;i<(int)pVS->GetNumVert();i++)
		{
			tagVert v;
			v.index=i;
			v.bActive=true;
			pVS->GetPos(i,v.pos);

			m_vertArray.push_back(v);
		}

		//--copy triangles
		WORD *pIdx=pTriListIndices;
		for(i=0;i<(int)numTri;i++)
		{
			tagTriangle tri;
			int v1,v2,v3;
			v1=*pIdx;	pIdx++;
			v2=*pIdx;	pIdx++;
			v3=*pIdx;	pIdx++;
			
			tri.vert[0]=v1;
			tri.vert[1]=v2;
			tri.vert[2]=v3;
			Vector3& p1=m_vertArray[v1].pos;
			Vector3& p2=m_vertArray[v2].pos;
			Vector3& p3=m_vertArray[v3].pos;
			BuildPlane(p1,p2,p3,tri.plane);
			tri.area=TriangleArea(p1,p2,p3);
			tri.bActive=true;

			m_triArray.push_back(tri);

			// update each vertex w/ its neighbors (vertrices & triangles)
			m_vertArray[v1].AddTriNeighbor(i);
			m_vertArray[v1].AddVertNeighbor(v2);
			m_vertArray[v1].AddVertNeighbor(v3);

			m_vertArray[v2].AddTriNeighbor(i);
			m_vertArray[v2].AddVertNeighbor(v1);
			m_vertArray[v2].AddVertNeighbor(v3);

			m_vertArray[v3].AddTriNeighbor(i);
			m_vertArray[v3].AddVertNeighbor(v1);
			m_vertArray[v3].AddVertNeighbor(v2);
		}

		CalcVertNormals();
	}

	MeshTopo *MeshTopo::Clone()
	{
		MeshTopo *pNewMesh=new MeshTopo;
		pNewMesh->m_vertArray=m_vertArray;
		pNewMesh->m_triArray=m_triArray;
		return pNewMesh;
	}

	MeshTopo::tagVert& MeshTopo::GetVert(UINT vertIndex)
	{
		ASSERT(vertIndex < m_vertArray.size());
		return m_vertArray[vertIndex];
	}

	MeshTopo::tagTriangle& MeshTopo::GetTri(UINT triIndex)
	{
		ASSERT(triIndex < m_triArray.size());
		return m_triArray[triIndex];
	}

	void MeshTopo::CalcVertNormals()
	{
		for(size_t i=0;i<m_vertArray.size();i++)
		{
			tagVert& vert=m_vertArray[i];
			const set<int>& triset = vert.triNeighbors;

			Vector3 vec;
			for (set<int>::const_iterator iter = triset.begin(); iter != triset.end(); ++iter)
			{
				const tagTriangle& tri=m_triArray[*iter];
				vec+=(tri.plane.normal*tri.area);
			}
			//顶点的normal为所有使用改顶点的三角形的加权平均值,三角形面积为权值,
			//see Eric Lengyel's Mathematics for 3D game programmer & CG
			D3DXVec3Normalize(&vert.normal,&vec);
		}
	}

	void MeshTopo::CalcVertQuadric(UINT vertIndex)
	{
		ASSERT(vertIndex < m_vertArray.size());
		tagVert& vert=m_vertArray[vertIndex];

		memset(vert.Q,0,sizeof(vert.Q));

		set<int>::iterator pos;
		for (pos = vert.triNeighbors.begin(); pos != vert.triNeighbors.end(); ++pos)
		{
			tagTriangle& tri=m_triArray[*pos];
			if(!tri.bActive)
				continue;
			float triArea = 1;

			const float a = tri.plane.normal.x;
			const float b = tri.plane.normal.y;
			const float c = tri.plane.normal.z;
			const float d = tri.plane.dist;
			
			vert.Q[0][0] += triArea * a * a;
			vert.Q[0][1] += triArea * a * b;
			vert.Q[0][2] += triArea * a * c;
			vert.Q[0][3] += triArea * a * d;

			vert.Q[1][0] += triArea * b * a;
			vert.Q[1][1] += triArea * b * b;
			vert.Q[1][2] += triArea * b * c;
			vert.Q[1][3] += triArea * b * d;

			vert.Q[2][0] += triArea * c * a;
			vert.Q[2][1] += triArea * c * b;
			vert.Q[2][2] += triArea * c * c;
			vert.Q[2][3] += triArea * c * d;

			vert.Q[3][0] += triArea * d * a;
			vert.Q[3][1] += triArea * d * b;
			vert.Q[3][2] += triArea * d * c;
			vert.Q[3][3] += triArea * d * d;
		}
	}

	void MeshTopo::GetVertAllBorderEdges(set<tagBorder> &borderSet, UINT vertIndex)
	{
		// Return all border info if the vertex is on an edge of the mesh.
		tagVert& vert=GetVert(vertIndex);

		// So go through the list of all neighboring vertices, and see how many
		// triangles this vertex has in common w/ each neighboring vertex.  Normally
		// there will be two triangles in common, but if there is only one, then this 
		// vertex is on an edge.
		set<int>::iterator pos, pos2;

		for (pos = vert.vertNeighbors.begin(); pos != vert.vertNeighbors.end(); ++pos)
		{
			int triCount = 0;
			int triIndex = -1;

			tagVert& v = GetVert(*pos);
			for (pos2 = v.vertNeighbors.begin(); pos2 != v.vertNeighbors.end(); ++pos2)
			{
				tagTriangle& tri=m_triArray[*pos2];
				if(tri.HasVert(vert.index))//tri同时拥有vert和v,即有这样一条edge
				{
					++triCount;
					triIndex=*pos2;
				}
			}//endof for v

			if (1 == triCount) // if only one triangle in common, it's an border
			{
				// store the smaller index first
				tagBorder b;
				b.triIndex = triIndex;
				if (vert.index < v.index)
				{
					b.vert1 = vert.index;
					b.vert2 = v.index;
				}
				else
				{
					b.vert1 = v.index;
					b.vert2 = vert.index;
				}
				borderSet.insert(b);
			}//end of if
		}//end of for
	}
}//namespace Cool3D