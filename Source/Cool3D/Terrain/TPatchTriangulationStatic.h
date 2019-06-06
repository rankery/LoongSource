#pragma once
#include <vector>
#include <map>
using namespace std;
#include <windows.h>

namespace Cool3D
{

	class TPatchTriangulationStatic;
	/** 创建并保存一个LOD的三角形索引
	*/
	class TPatchLodStatic
	{
		enum 
		{ 
			MAX_GRIDS=256
		};
	public:
		TPatchLodStatic();
		~TPatchLodStatic();

		//! 生成三角形索引
		void Create(int lod,TPatchTriangulationStatic* pParent);

		//! 获取三角形索引
		void GetIndices(const vector<int>& grids, vector<WORD>& indices);
		void GetIndices(const vector<int>& grids, vector<WORD>& indices,WORD baseIndex);
		void GetIndices(vector<WORD>& indices);

	private:
		
		//! heightmap坐标转换成索引
		inline WORD MakeIndex(int x, int y);
	private:
		vector<WORD> m_indices[MAX_GRIDS];

		int m_mapW, m_mapH;
		int m_lod;
		int m_numGrids;

		TPatchTriangulationStatic* m_pParent;
	};


	/**	\class TPatchTriangulationStatic
		\brief 为地块产生三角形列表
		\remark 
			\par 支持LOD
			\par 相同LOD的地块内部的三角形密度相同
			\par 地块划分成NxN个GRID,GRID划分成4个三角形
	*/
	class TPatchTriangulationStatic
	{
		friend class TPatchLodStatic;
	public:
		enum 
		{ 
			NUM_LOD=1
		};
	public:

		//! 生成所有LOD的三角形索引
		void Create();

		//! 获取指定LOD的三角形索引
		void GetIndices(int lod,const vector<int>& grids,vector<WORD>& indices,int& numFace);
		void GetIndices(int lod,vector<WORD>& indices);

		static TPatchTriangulationStatic* Inst();
	private:
		TPatchTriangulationStatic(void);
		~TPatchTriangulationStatic(void);

		TPatchLodStatic m_lods[NUM_LOD];
		static int m_sideGrids[NUM_LOD];//每边grid数量
		static int m_sideVerts[NUM_LOD];//每边顶点数量
	};

}//end namespace Cool3D