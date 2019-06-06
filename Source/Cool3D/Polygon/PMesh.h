#pragma once
#include <set>
#include "..\Util\Noncopyable.h"
#include "MeshTopo.h"
#include "..\Cool3D.h"

namespace Cool3D
{
	/**	\class PMesh
		\brief Progressive Meshe
	*/
	class Cool3D_API PMesh : private Noncopyable
	{
		// This is a "pointer" to a vertex in a given mesh
		struct tagVertexPtr
		{
			MeshTopo* pMesh;
			int index; // ptr to vertex position in mesh

			bool operator<(const tagVertexPtr& vp) const 
			{
				return (pMesh->GetVert(index) < vp.pMesh->GetVert(vp.index));
			}
		};

		typedef multiset<tagVertexPtr, less<tagVertexPtr> > vertexPtrSet;
		/**	\struct tagEdgeCollapse
			\brief 边压缩结构
			\remarks "from"顶点被压缩到"to"顶点,可能删除多个三角形,并且影响
			所有含有"from"顶点的三角形,这些三角形将必须更新为使用新的顶点
		*/
		struct tagEdgeCollapse
		{
			int vfrom;
			int vto;
			set<int> trisRemoved;
			set<int> trisAffected;
		};
	public:
		enum EEdgeCost
		{
			EEdge_LenMulRugh=0,	//Eric Lengyel提出的edge长度乘以顶点周围三角形的粗糙程度的算法
			EEdge_Quadric,	//Garland & Heckbert Quadrics
		};
		PMesh(void);
		virtual ~PMesh(void);

		/** Create the list of the edge collapses used to simplify the mesh.
		*/
		void CreateEdgeCollapseList(MeshTopo* pOriginMesh,EEdgeCost cost=EEdge_Quadric);
		/** Collapse one vertex to another.
		*/
		bool CollapseEdge();

		int GetNumVisTris()		{	return m_nVisTriangles; }
		MeshTopo *GetNewMesh()	{	return m_pNewMesh; }
	protected:
		void CalcEdgeCollapseCosts(vertexPtrSet &vertSet, vector<vertexPtrSet::iterator> &vertSetVec,MeshTopo *pMesh);
		void BuildEdgeCollapseList(MeshTopo *pMesh, list<tagEdgeCollapse> &edgeCollList,
			vertexPtrSet &vertSet,vector<vertexPtrSet::iterator> &vertSetVec);
			void InsureEdgeCollapseValid(tagEdgeCollapse &ec, int vi, MeshTopo* pMesh, bool &bBadVertex);
			void SetToVertexQuadric(MeshTopo::tagVert& to, MeshTopo::tagVert& from);
			void UpdateTriangles(tagEdgeCollapse &ec, int vi, set<int> &affectedVerts, MeshTopo *pMesh);
			void UpdateAffectedVerts(MeshTopo* pMesh, vector<vertexPtrSet::iterator> &vertSetVec, 
				vertexPtrSet &vertSet, const tagEdgeCollapse &ec, 
				set<int> &affectedVerts, set<int> &affectedQuadricVerts);
			void RecalcQuadricCollapseCosts(set<int> &affectedQuadricVerts, MeshTopo *pMesh);
			void UpdateAffectedVertNeighbors(MeshTopo::tagVert &vert, const tagEdgeCollapse &ec, 
				const set<int> &affectedVerts);
			void RemoveVertIfNecessary(MeshTopo::tagVert &vert, vertexPtrSet &vertSet, 
				vector<vertexPtrSet::iterator> &vertSetVec, 
				MeshTopo *pMesh, set<int> &affectedQuadricVerts);

			

		void Destroy();
		void CalcAllQMatrices(MeshTopo *pMesh);
			void ApplyBorderPenalties(set<MeshTopo::tagBorder> borderSet,MeshTopo *pMesh);
		void QuadricCollapseCost(MeshTopo *pMesh,UINT vertIndex);
			double CalcQuadricError(double Qsum[4][4], MeshTopo::tagVert& v, double triArea);

	protected:
		MeshTopo						*m_pOriginMesh;	//原始的mesh
		MeshTopo						*m_pNewMesh;
		list<tagEdgeCollapse>			m_edgeCollList; // list of edge collapses
		
		list<tagEdgeCollapse>::iterator	m_edgeCollapseIter;
		int								m_nVisTriangles;
	};
}//namespace Cool3D