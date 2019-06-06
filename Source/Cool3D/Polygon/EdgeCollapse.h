#pragma once
#include "..\Math\Math.h"

namespace F3D
{
	class VertStream;

	/**	\class EdgeCollapse
		\brief 使用边塌陷的方法来简化Mesh
	*/
	class EdgeCollapse
	{
		struct tagEdge
		{
			WORD		index[2];	//组成这条边的两个顶点的索引
			ste<WORD>	faces;	//共享这两条边的两个face
			float		ccost[2];	//这条边上两个顶点分别的简化代价,d[0]为vert0移动到vert1
		};
		struct tagTriFace
		{
			WORD	index[3];	//组成这个三角形的三个顶点的索引
			Vector3	normal;		//单位法向量
			bool	bRemoved;	//是否已经被简化掉了
		};
		struct tagVertShare
		{
			set<WORD>		faces;	//共享这个顶点的所有面
			Vector3			normal;	//这个顶点的平均法向量
		};
	public:
		EdgeCollapse(void);
		virtual ~EdgeCollapse(void);

		void BuildTriListMesh(VertStream *pVertStream,WORD *pIndices,UINT numTri);

		/**	小于某个简化代价的边被压缩,以简化Mesh
			\param 最大的可简化代价
		*/
		void Simplify(float maxCost);
		void GetIndices(vector<WORD>& indices);

	protected:
		/**	选择简化代价最小的一条边,进行简化
			\remarks 删除一条边,一个顶点,多个face
		*/
		void CollaspeOne();
		/**	计算边的共享face和vert的共享face
		*/
		void CalMeshTopo();
		/**	计算一条边上两个顶点的简化代价
		*/
		void CalCost(tagEdge& edge);

	protected:
		VertStream				*m_pVertStream;
		vector<tagEdge>			m_edgeArray;	//原始Mesh的边数组
		vector<tagTriFace>		m_faceArray;	//原始Mesh的面数组
		vector<tagVertShare>	m_vertArray;	//顶点共享信息数组
	};
}//namespace F3D