#pragma once
#include <set>
#include "..\Util\Noncopyable.h"
#include "..\Math\Math.h"
#include "..\Math\GeomDefine.h"
#include "..\Cool3D.h"


namespace Cool3D
{
	class VertStream;
	/**	\class MeshTopo
		\brief 带有topo信息的mesh
	*/
	class Cool3D_API MeshTopo : private Noncopyable
	{
	public:
		/** Used to store an edge -- two vertices which have only one
			triangle in common form an edge of the mesh.
		 */
		struct tagBorder
		{
			int vert1;
			int vert2;
			int triIndex;

			// We need operator< because it's used by the STL's set<> to determine equality
			// (if (not a<b) and (not b>a) then a is equal to b)
			bool operator<(const tagBorder& b) const 
			{
				int v1, v2, b1, b2;
				// make sure the smaller vert index is always first.
				if (vert1 < vert2) {
					v1 = vert1; v2 = vert2;
				} else {
					v1 = vert2; v2 = vert1;
				}
				if (b.vert1 < b.vert2) {
					b1 = b.vert1; b2 = b.vert2;
				} else {
					b1 = b.vert2; b2 = b.vert1;
				}
				if (v1 < b1) return true;
				if (v1 > b1) return false;
				return (v2 < b2); // v1 == b1
			}
		};

		struct tagVert
		{
			Vector3		pos;
			Vector3		normal;	//算出来的,而不是从VertStream拷贝而来

			int			index;
			set<int>	vertNeighbors;	// connected to this vertex via an edge
			set<int>	triNeighbors;	// triangles of which this vertex is a part
			
			double		Q[4][4];		// Used for Quadric error cost.

			double		cost;			// cost of removing this vertex from Progressive Mesh
			int			minCostNeighbor; // index of vertex at other end of the min. cost edge

			bool		bActive;

			void CopyFrom(const tagVert& other)
			{
				pos=other.pos;
				normal=other.normal;
				index=other.index;
				vertNeighbors=other.vertNeighbors;
				triNeighbors=other.triNeighbors;
				memcpy(Q,other.Q,sizeof(Q));
				cost=other.cost;
				minCostNeighbor=other.minCostNeighbor;
				bActive=other.bActive;
			}
			tagVert(const tagVert& other)
			{
				CopyFrom(other);
			}
			const tagVert& operator = (const tagVert& other)
			{
				if(&other==this)
					return *this;
				CopyFrom(other);
				return *this;
			}

			tagVert();
			~tagVert() {}
			bool operator == (const tagVert& v)
			{
				return pos==v.pos && normal==v.normal;
			}
			bool operator != (const tagVert& v)
			{
				return pos!=v.pos || normal!=v.normal;
			}
			// operator< & operator> are used to order vertices by edge removal costs
			bool operator<(const tagVert& v) const {return (cost < v.cost);}
			bool operator>(const tagVert& v) const {return (cost > v.cost);}

			void AddTriNeighbor(int i)	{	triNeighbors.insert(i); }
			void RemoveTriNeighbor(int i) {	triNeighbors.erase(i);	}
			void AddVertNeighbor(int i)	{	vertNeighbors.insert(i); }
		};
		struct tagTriangle
		{
			int		vert[3];	//三个定点
			float	area;		//面积
			Plane	plane;		//平面方程
			bool	bActive;	//

			void GetVerts(int& v1,int& v2,int& v3)	
			{
				v1=vert[0];
				v2=vert[1];
				v3=vert[2];
			}
			bool HasVert(int vi)
			{
				return vert[0]==vi || vert[1]==vi || vert[2]==vi;
			}
			// When we collapse an edge, we may change the 
			// vertex of a triangle.
			void ChangeVertex(int vFrom, int vTo);
			
		};

		MeshTopo(void);
		virtual ~MeshTopo(void);

		void GetActiveTris(vector<WORD>& indices);
		void Build(VertStream *pVS,WORD *pTriListIndices,UINT numTri);

		/**	calculate the 4x4 Quadric matrix for a vert
		*/
		void CalcVertQuadric(UINT vertIndex);
		/**
		Is the current vertex on an edge?  If so, get edge information.
		This is used to put constraints on the border so that the mesh
		edges aren't "eaten away" by the mesh simplification.
		*/
		void GetVertAllBorderEdges(set<tagBorder> &borderSet, UINT vertIndex);

		/**	new一个对象,并把自己的内容copy过去
			\remarks 外部需要记得delete这个返回的指针
		*/
		MeshTopo *Clone();

		tagTriangle& GetTri(UINT triIndex);
		tagVert& GetVert(UINT vertIndex);
		UINT GetNumVerts()	{	return m_vertArray.size(); }
		UINT GetNumTris()	{	return m_triArray.size(); }

		void CalcTriPlane(UINT triIndex);
		float CalcTriArea(UINT triIndex);

	protected:
		/**	Calculate the vertex normals after buliding the mesh
		*/
		void CalcVertNormals();
	protected:
		vector<tagVert>		m_vertArray;
		vector<tagTriangle>	m_triArray;	
	};
}//namespace Cool3D