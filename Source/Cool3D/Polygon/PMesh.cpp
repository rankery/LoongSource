#include "StdAfx.h"
#include ".\pmesh.h"
#include "..\Util\Exception.h"

namespace Cool3D
{
	const float BOUNDARY_WEIGHT = 10000; // used to weight border edges so they don't collapse

	PMesh::PMesh(void)
	{
		m_pOriginMesh=NULL;
		m_pNewMesh=NULL;
		m_nVisTriangles=0;
	}

	PMesh::~PMesh(void)
	{
		Destroy();
	}

	void PMesh::Destroy()
	{
		m_pOriginMesh=NULL;
		m_edgeCollList.clear();
		SAFE_DELETE(m_pNewMesh);
		m_nVisTriangles=0;
	}

	bool PMesh::CollapseEdge()
	{
		// Collapse an edge (remove one vertex & edge, and possibly some triangles.)
		
		// Iterator always points to next collapse to perform
		if (m_edgeCollapseIter == m_edgeCollList.end()) 
			return false; // no more edge collapses in list

		tagEdgeCollapse& ec = *m_edgeCollapseIter;
		int numRemove=ec.trisRemoved.size();
		int numAffected=ec.trisAffected.size();

		set<int> affectedVerts; // vertices affected by this edge collapse
		int v1, v2, v3; // vertex indices

		// Remove triangles 
		set<int>::iterator tripos;
		for (tripos = ec.trisRemoved.begin(); tripos != ec.trisRemoved.end(); ++tripos) 
		{
			// get triangle
			int triIndex = *tripos;
			MeshTopo::tagTriangle& t = m_pNewMesh->GetTri(triIndex);
			t.GetVerts(v1, v2, v3); // get triangle vertices
			t.bActive=false;
			affectedVerts.insert(v1); // add vertices to list
			affectedVerts.insert(v2); // of vertices affected
			affectedVerts.insert(v3); // by this collapse
		}

		// Adjust vertices of triangles
		for (tripos = ec.trisAffected.begin(); tripos != ec.trisAffected.end(); ++tripos) 
		{
			// get triangle
			int triIndex = *tripos;
			MeshTopo::tagTriangle& t = m_pNewMesh->GetTri(triIndex);
			t.ChangeVertex(ec.vfrom, ec.vto); // update the vertex of this triangle
			m_pNewMesh->CalcTriPlane(triIndex);// reset the normal for the triangle
			t.GetVerts(v1, v2, v3); // get triangle vertices
			affectedVerts.insert(v1); // add vertices to list
			affectedVerts.insert(v2); // of vertices affected
			affectedVerts.insert(v3); // by this collapse
		}

		// redo the vertex normal for the vertices affected.  these are
		// vertices of triangles which were shifted around as a result
		// of this edge collapse.
		set<int>::iterator affectedVertsIter;
		for (affectedVertsIter = affectedVerts.begin(); affectedVertsIter != affectedVerts.end(); ++affectedVertsIter) 
		{
			if (ec.vfrom == *affectedVertsIter) 
				continue; // skip the from vertex -- it's no longer active

			// We have the affected vertex index, so redo the its normal (for Gouraud shading);
			//??????????????????
			//_newmesh.calcOneVertNormal(*affectedVertsIter);
		}

		// Since iterator always points to next collapse to perform, go to the next
		// collapse in list.
		++m_edgeCollapseIter;

		m_nVisTriangles -=  ec.trisRemoved.size();

		return true;
	}

	void PMesh::CreateEdgeCollapseList(MeshTopo* pOriginMesh,EEdgeCost cost)
	{
		// Where most of the work of the program is done.
		// This will create a list of edge collapses.  Each edge collapse
		// is a set of 2 vertices, a from vertex & a to vertex.  The from
		// vertex will be collapsed to the to vertex.  No new vertices are created,
		// only vertices in the original mesh are used.

		Destroy();
		//--
		m_pOriginMesh=pOriginMesh;
		m_nVisTriangles = m_pOriginMesh->GetNumTris(); // number of visible triangles
		// create the new Mesh
		m_pNewMesh=m_pOriginMesh->Clone();

		// calculate all 4x4 Q matrices for each vertex 
		CalcAllQMatrices(m_pNewMesh);

		vertexPtrSet vertSet;
		vector<vertexPtrSet::iterator> vertSetVec(m_pNewMesh->GetNumVerts());
		// Go through, calc cost here for all vertices
		CalcEdgeCollapseCosts(vertSet,vertSetVec,m_pNewMesh);

		// For all vertices:
		//	find lowest cost
		//	store the edge collapse structure
		//	update all verts, triangles affected by the edge collapse
		BuildEdgeCollapseList(m_pNewMesh,m_edgeCollList,vertSet,vertSetVec);

		// set iterator to point to beginning
		m_edgeCollapseIter = m_edgeCollList.begin();

		// recreate the new Mesh
		delete m_pNewMesh;
		m_pNewMesh=m_pOriginMesh->Clone();

	}

	void PMesh::CalcEdgeCollapseCosts(vertexPtrSet &vertSet, vector<vertexPtrSet::iterator> &vertSetVec,MeshTopo *pMesh)
	{
		// Calculate edge collapse costs.  Edges with low costs
		// are collapsed first.

		int nVerts=(int)pMesh->GetNumVerts();
		for(int i=0;i<nVerts;i++)
		{
			tagVertexPtr v;
			v.index = i;
			v.pMesh = pMesh;

			QuadricCollapseCost(pMesh,i);
			vertSetVec[i]=vertSet.insert(v);
		}
	}

	void PMesh::BuildEdgeCollapseList(MeshTopo *pMesh, list<tagEdgeCollapse> &edgeCollList,
		vertexPtrSet &vertSet,vector<vertexPtrSet::iterator> &vertSetVec)
	{
		// Calculate the list of edge collapses.  Each edge collapse
		// consists of two vertices:  a "from vertex" and a "to vertex".
		// The "from vertex" is collapsed to the "to vertex".  The
		// "from vertex" is removed from the mesh.

		for (;;)
		{
			if (0 == vertSet.size())
				break;// we're done
			//已经排好序了,第一个就是cost最低的
			const tagVertexPtr vp = *(vertSet.begin()); // This is a copy of the first element
			MeshTopo::tagVert vc = pMesh->GetVert(vp.index);
			ASSERT(vp.index == vc.index);

			tagEdgeCollapse ec; // create tagEdgeCollapse structure

			bool bBadVertex = false;
			// Make sure this edge collapse has a valid "to vertex"
			InsureEdgeCollapseValid(ec, vp.index, pMesh, bBadVertex);

			pMesh->GetVert(ec.vfrom).bActive=false;
			vertSet.erase(vertSet.begin());

			if (bBadVertex) 
				continue;
			
			MeshTopo::tagVert& to=pMesh->GetVert(ec.vto);
			MeshTopo::tagVert& from=pMesh->GetVert(ec.vfrom);

			SetToVertexQuadric(to,from);

			set<int> affectedVerts;
			
			// We are removing a vertex and an edge.  Look at all triangles
			// which use this vertex.  Each of these triangles is either being
			// removed or updated with a new vertex.
			UpdateTriangles(ec, vp.index, affectedVerts, pMesh);

			set<int> affectedQuadricVerts;

			// These vertices were in triangles which either were removed or
			// were updated with new vertices.  Removed these vertices if they're
			// not connected to an active triangle.  Update these vertices if they're
			// still being displayed.

			UpdateAffectedVerts(pMesh, vertSetVec, vertSet, ec, affectedVerts, affectedQuadricVerts);

			// If using the quadric collapse method, 
			// recalculate the edge collapse costs for the affected vertices.
			RecalcQuadricCollapseCosts(affectedQuadricVerts, pMesh);

			m_edgeCollList.push_back(ec); // inserts a copy

		}//endof for

	}

	void PMesh::SetToVertexQuadric(MeshTopo::tagVert& to, MeshTopo::tagVert& from)
	{
		// Calculate the QEM for the "to vertex" in the edge collapse.

		int ct, ct2;

		double Qf[4][4], Qt[4][4];
		memcpy(Qt,to.Q,sizeof(Qt));
		memcpy(Qf,from.Q,sizeof(Qf));
		
		for (ct = 0; ct < 4; ++ct)
		{
			for (ct2 = 0; ct2 < 4; ++ct2)
			{
				Qt[ct][ct2] += Qf[ct][ct2];
			}
		}
		memcpy(to.Q,Qt,sizeof(to.Q));
	}

	void PMesh::InsureEdgeCollapseValid(tagEdgeCollapse &ec, int vi, MeshTopo* pMesh, bool &bBadVertex)
	{
		// We can't collapse Vertex1 to Vertex2 if Vertex2 is invalid.
		// This can happen if Vertex2 was previously collapsed to a
		// separate vertex.

		MeshTopo::tagVert& vc=pMesh->GetVert(vi);
		int nLoopCount = 0;
		for (;;) // at most this will loop twice -- the "to vertex" could have been removed, so it may need to be recalculated.
		{
			++nLoopCount;
			ec.vfrom = vc.index; // we'll collapse this vertex...
			ec.vto = vc.minCostNeighbor; // to this one

			if (-1 == vc.minCostNeighbor)
			{
				// this point is isolated -- it's not connected to another point by an edge
				// Erase this point & get the next one
				bBadVertex = true;
				break;
			}

			if (nLoopCount > 2 || !pMesh->GetVert(ec.vfrom).bActive)
			{
				bBadVertex = true;
				break;
			}

			// If not vertex active, recalc
			if (!pMesh->GetVert(ec.vto).bActive)//vc本来有运功车miniCosNeighbor,但是它已经被删除了,所以重新计算vc
				QuadricCollapseCost(pMesh,vi);
			else
				break;
			
		}//endof for()
	}

	void PMesh::UpdateTriangles(tagEdgeCollapse &ec, int vi, set<int> &affectedVerts, MeshTopo *pMesh)
	{
		// At this point, we have an edge collapse.  We're collapsing the "from vertex"
		// to the "to vertex."  For all the surrounding triangles which use this edge, 
		// update "from vertex" to the "to vertex".  Also keep track of the vertices
		// in the surrounding triangles. 

		MeshTopo::tagVert& vc=pMesh->GetVert(vi);
		set<int>& triNeighbors = vc.triNeighbors;
		set<int>::iterator pos;
		for (pos = triNeighbors.begin(); pos != triNeighbors.end(); ++pos) 
		{
			// get triangle
			int triIndex = *pos;
			MeshTopo::tagTriangle& t=pMesh->GetTri(triIndex);
			if (!t.bActive) 
				continue;
			bool bRemoveTri = false;

			if(t.HasVert(ec.vfrom) && t.HasVert(ec.vto))
			{
				ec.trisRemoved.insert(triIndex);
				t.ChangeVertex(ec.vfrom, ec.vto); // update the vertex of this triangle

				bRemoveTri = true;

				t.bActive=false; // triangle has two vertices which are now the same, so it's just a line segment
			}//endof if
			else
			{
				t.ChangeVertex(ec.vfrom, ec.vto); // update the vertex of this triangle
				pMesh->CalcTriPlane(triIndex);	 // reset the normal for the triangle

				// make sure the "to" vertex knows about this triangle
				pMesh->GetVert(ec.vto).AddTriNeighbor(triIndex);

				// If the triangle has an area effectively equal to 0, remove it.
				// NOTE: should this be done?  The triangle could get bigger through 
				// another edge collapse in another direction.
				if (pMesh->CalcTriArea(triIndex) < 1e-6) 
				{
					t.bActive=false;
					ec.trisRemoved.insert(triIndex);
					bRemoveTri = true;
				} 
				else 
				{
					ec.trisAffected.insert(triIndex);
				}
			}//endof else

			// update set of affected verts
			// note that if you insert the same element twice, 
			// it will only be stored once.
			int vert1, vert2, vert3;
			vert1=t.vert[0];
			vert2=t.vert[1];
			vert3=t.vert[2];
			

			affectedVerts.insert(vert1);
			affectedVerts.insert(vert2);
			affectedVerts.insert(vert3);

			// If triangle is being removed, update each vertex which references it.
			if (bRemoveTri)
			{
				pMesh->GetVert(vert1).RemoveTriNeighbor(triIndex);
				pMesh->GetVert(vert2).RemoveTriNeighbor(triIndex);
				pMesh->GetVert(vert3).RemoveTriNeighbor(triIndex);
			}

		}//endof for
	}

	void PMesh::UpdateAffectedVerts(MeshTopo* pMesh, vector<vertexPtrSet::iterator> &vertSetVec, 
		vertexPtrSet &vertSet, const tagEdgeCollapse &ec, 
		set<int> &affectedVerts, set<int> &affectedQuadricVerts)
	{
		// Update the vertices affected by the most recent edge collapse
		set<int>::iterator mappos;
		for (mappos = affectedVerts.begin(); mappos != affectedVerts.end(); ++mappos)
		{
			MeshTopo::tagVert& vert=pMesh->GetVert(*mappos);
			ASSERT(vert.index==*mappos);

			// Always erase, maybe add in.
			// Can't change in place, 'cause will screw up order of set
			vertSet.erase(vertSetVec[*mappos]);
			vertSetVec[*mappos] = vertSet.end(); // set to "invalid" value

			UpdateAffectedVertNeighbors(vert, ec, affectedVerts);

			// Remove vertex if it's not attached to any active triangle
			RemoveVertIfNecessary(vert, vertSet, vertSetVec, pMesh, affectedQuadricVerts);

		}//endof for
	}

	void PMesh::RemoveVertIfNecessary(MeshTopo::tagVert &vert, vertexPtrSet &vertSet, 
		vector<vertexPtrSet::iterator> &vertSetVec, 
		MeshTopo *pMesh, set<int> &affectedQuadricVerts)
	{
		// If this vertex has no active triangles (i.e. triangles which have
		// not been removed from the mesh) then set it to inactive.

		bool bActiveVert = false;
		set<int>& mytriNeighbors = vert.triNeighbors;
		set<int>::iterator pos2;
		for (pos2 = mytriNeighbors.begin(); pos2 != mytriNeighbors.end(); ++pos2) 
		{
			// get triangle
			int triIndex = *pos2;
			if(pMesh->GetTri(triIndex).bActive)
			{
				bActiveVert = true;
				break;
			}
		}//endof for

		if (bActiveVert) // if vert is active
		{
			tagVertexPtr vp;
			vp.index = vert.index;
			vp.pMesh = pMesh;
			vertSetVec[vp.index] = vertSet.insert(vp);

			ASSERT(vertSetVec[vp.index]->index == vp.index);
			pMesh->GetVert(vp.index).bActive=true; 
			
			// If we're calculating quadric costs, keep track of
			// every active vertex which was affect by this collapse,
			// so we can recalculate collapse costs.
			affectedQuadricVerts.insert(vert.index);
		}//endof if
		else
			pMesh->GetVert(vert.index).bActive=false;
	}

	void PMesh::UpdateAffectedVertNeighbors(MeshTopo::tagVert &vert, const tagEdgeCollapse &ec, 
		const set<int> &affectedVerts)
	{
		// These vertices are not in the current collapse, but are in the triangles
		// which share the collapsed edge.

		if (vert.index != ec.vto)
		{
			vert.AddVertNeighbor(ec.vto); // make sure vertex knows it has a new neighbor
		}//endof if
		else
		{
			set<int>::const_iterator mappos2;

			// Make sure the "to" vertex knows about its
			// new neighbors
			for (mappos2 = affectedVerts.begin(); mappos2 != affectedVerts.end(); ++mappos2)
			{
				if (*mappos2 != ec.vto)
				{
					vert.AddVertNeighbor(*mappos2); 
				}
			}//endof for
		}//endof else
	}

	void PMesh::RecalcQuadricCollapseCosts(set<int> &affectedQuadricVerts, MeshTopo *pMesh)
	{
		// Recalculate the QEM matrices (yeah, that's redundant)
		set<int>::iterator mappos;
		for (mappos = affectedQuadricVerts.begin(); mappos != affectedQuadricVerts.end(); ++mappos)
		{			
			int vert= (*mappos);
			QuadricCollapseCost(pMesh, vert);
		}
	}

	void PMesh::CalcAllQMatrices(MeshTopo *pMesh)
	{
		// Calculate the 4x4 Q Matrix used for the Quadric calculation
		// for each vertex

		set<MeshTopo::tagBorder> borderSet;
		UINT nVerts = pMesh->GetNumVerts();

		for (UINT i = 0; i < nVerts; ++i)
		{
			pMesh->CalcVertQuadric(i);
			MeshTopo::tagVert& currVert = pMesh->GetVert(i);

			double myQ[4][4];
			memcpy(myQ,currVert.Q,sizeof(myQ));
			double triArea = 1;

			CalcQuadricError(myQ, currVert, triArea);

			// Is the current vertex on a border?  If so, get the
			// edge information
			pMesh->GetVertAllBorderEdges(borderSet, i);
		}

		// Keep the mesh borders from being "eaten away".
		if (!borderSet.empty())
		{
			ApplyBorderPenalties(borderSet, pMesh);
		}
	}

	void PMesh::ApplyBorderPenalties(set<MeshTopo::tagBorder> borderSet,MeshTopo *pMesh)
	{
		// Border penalties are used to prevent mesh edges from collapsing
		// too soon.  This causes holes & T-junctions in the mesh to expand,
		// and causes the edges of the mesh to be "eaten away".  2-manifold,
		// closed meshes will not need to worry about this, and won't have
		// any border penalties.

		set<MeshTopo::tagBorder>::iterator pos;

		for (pos = borderSet.begin(); pos != borderSet.end(); ++pos) 
		{
			// First, determine the plane equation of plane perpendicular 
			// to the edge triangle.

			MeshTopo::tagBorder edgeInfo = *pos;

			MeshTopo::tagVert& v1 = pMesh->GetVert(edgeInfo.vert1);
			MeshTopo::tagVert& v2 = pMesh->GetVert(edgeInfo.vert2);

			Vector3 &vec1 = v1.pos;
			Vector3 &vec2 = v2.pos;

			Vector3 edge = vec1 - vec2;

			MeshTopo::tagTriangle &tri = pMesh->GetTri(edgeInfo.triIndex);
			Vector3 normal = tri.plane.normal;

			Vector3 abc;
			D3DXVec3Cross(&abc,&edge,&normal);
			D3DXVec3Normalize(&abc,&abc);
			float &a = abc.x;
			float &b = abc.y;
			float &c = abc.z;

			float d = -D3DXVec3Dot(&abc,&vec1);

			double QuadricConstraint[4][4];
			
			QuadricConstraint[0][0] = BOUNDARY_WEIGHT * a * a;
			QuadricConstraint[0][1] = BOUNDARY_WEIGHT * a * b;
			QuadricConstraint[0][2] = BOUNDARY_WEIGHT * a * c;
			QuadricConstraint[0][3] = BOUNDARY_WEIGHT * a * d;

			QuadricConstraint[1][0] = BOUNDARY_WEIGHT * b * a;
			QuadricConstraint[1][1] = BOUNDARY_WEIGHT * b * b;
			QuadricConstraint[1][2] = BOUNDARY_WEIGHT * b * c;
			QuadricConstraint[1][3] = BOUNDARY_WEIGHT * b * d;

			QuadricConstraint[2][0] = BOUNDARY_WEIGHT * c * a;
			QuadricConstraint[2][1] = BOUNDARY_WEIGHT * c * b;
			QuadricConstraint[2][2] = BOUNDARY_WEIGHT * c * c;
			QuadricConstraint[2][3] = BOUNDARY_WEIGHT * c * d;

			QuadricConstraint[3][0] = BOUNDARY_WEIGHT * d * a;
			QuadricConstraint[3][1] = BOUNDARY_WEIGHT * d * b;
			QuadricConstraint[3][2] = BOUNDARY_WEIGHT * d * c;
			QuadricConstraint[3][3] = BOUNDARY_WEIGHT * d * d;

			// Now add the constraint quadric to the quadrics for both of the 
			// vertices.
			double Q1[4][4], Q2[4][4];
			memcpy(Q1,v1.Q,sizeof(Q1));
			memcpy(Q2,v2.Q,sizeof(Q2));
			
			for (int ct = 0; ct < 4; ++ct)
			{
				for (int ct2 = 0; ct2 < 4; ++ct2)
				{
					Q1[ct][ct2] += QuadricConstraint[ct][ct2];
					Q2[ct][ct2] += QuadricConstraint[ct][ct2];
				}
			}
			memcpy(v1.Q,Q1,sizeof(Q1));
			memcpy(v2.Q,Q2,sizeof(Q2));
		}
	}

	void PMesh::QuadricCollapseCost(MeshTopo *pMesh,UINT vertIndex)
	{
		MeshTopo::tagVert& vert=pMesh->GetVert(vertIndex);

		// get list of all active neighbors
		// calculate quadric cost
		double mincost = FLOAT_MAX; // from float.h
		bool bNeighborFound = false;

		double Q1[4][4];
		memcpy(Q1,vert.Q,sizeof(Q1));

		set<int>& neighbors = vert.vertNeighbors;
		for (set<int>::iterator pos	 = neighbors.begin(); pos != neighbors.end(); ++pos) 
		{
			MeshTopo::tagVert& n = pMesh->GetVert(*pos);
			if (!n.bActive) 
				continue;
			if(n == vert)
				continue;
			double Q2[4][4];
			double Qsum[4][4];

			// add two 4x4 Q matrices
			memcpy(Q2,n.Q,sizeof(Q2));
			
			for(int i = 0; i < 4; ++i) 
			{
				for ( int j = 0; j < 4; ++j) 
				{
					Qsum[i][j] = Q1[i][j] + Q2[i][j];
				}
			}

			double triArea = 1.0;
			double cost = CalcQuadricError(Qsum, n, triArea);

			if (cost < mincost)
			{
				bNeighborFound = true;
				mincost = cost;
				vert.cost = cost;
				vert.minCostNeighbor=(*pos);
				ASSERT(vert.minCostNeighbor >= 0 
					&& vert.minCostNeighbor < (int)pMesh->GetNumVerts());
			}
		}
	}

	double PMesh::CalcQuadricError(double Qsum[4][4], MeshTopo::tagVert& v, double triArea)
	{
		// Calculate the quadric error if using that edge collapse
		// algorithm.  We're calculating
		//
		//  T    
		// v  Q v
		//
		// This is the vertex multiplied by the 4x4 Q matrix, multiplied
		// by the vertex again.

		double cost;

		// 1st, consider vertex v a 1x4 matrix: [v.x v.y v.z 1]
		// Multiply it by the Qsum 4x4 matrix, resulting in a 1x4 matrix

		double result[4];

		const Vector3 v3 = v.pos;

		result[0] = v3.x * Qsum[0][0] + v3.y * Qsum[1][0] +
			v3.z * Qsum[2][0] + 1 * Qsum[3][0];
		result[1] = v3.x * Qsum[0][1] + v3.y * Qsum[1][1] +
			v3.z * Qsum[2][1] + 1 * Qsum[3][1];
		result[2] = v3.x * Qsum[0][2] + v3.y * Qsum[1][2] +
			v3.z * Qsum[2][2] + 1 * Qsum[3][2];
		result[3] = v3.x * Qsum[0][3] + v3.y * Qsum[1][3] +
			v3.z * Qsum[2][3] + 1 * Qsum[3][3];

		// Multiply this 1 x 4 matrix by the vertex v transpose (a 4 x 1 matrix).
		// This is just the dot product.

		cost =	result[0] * v3.x + result[1] * v3.y +
			result[2] * v3.z + result[3] * 1; 

		
		cost /= triArea;
		

		return cost;
	}
}//namespace Cool3D