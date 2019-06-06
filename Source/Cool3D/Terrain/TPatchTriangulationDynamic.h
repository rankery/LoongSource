#pragma once
#include <vector>
#include <map>
using namespace std;
#include <windows.h>
#include "..\Device\IIndexBuffer.h"
#include "..\RenderSys\HardwareBufferPool.h"

namespace Cool3D
{

	class TPatchTriangulationDynamic;
	/** 创建并保存一个LOD的三角形索引
	*/
	class TPatchLodDynamic
	{
		struct tagIB
		{
			vector<WORD> indices;
			IIndexBuffer* pIB;
		};
	public:
		TPatchLodDynamic();
		~TPatchLodDynamic();

		void Create(TPatchTriangulationDynamic* pParent,int lod);
		void Destroy();

		IIndexBuffer* GetIB_HW(int leftLod,int topLod,int rightLod,int bottomLod,int& numFaces);
		const vector<WORD>& GetIB(int leftLod,int topLod,int rightLod,int bottomLod);

	private:
		inline WORD MakeIndex(int x, int y);

		void BuildIndices(int leftLod,int topLod,int rightLod,int bottomLod,vector<WORD>& indices);

		void MakeTopTri(int sideGrids, int x, int y, int gridSizeX,
			int indexCenter, int indexLeftTop, int indexRightTop, int topLod,vector<WORD>& indices);

		void MakeBottomTri(int sideGrids, int x, int y1, int gridSizeX,
			int indexCenter, int indexLeftBottom, int indexRightBottom, int bottomLod,vector<WORD>& indices);

		void MakeLeftTri(int sideGrids, int x, int y, int gridSizeY,
			int indexCenter, int indexLeftTop, int indexLeftBottom, int leftLod,vector<WORD>& indices);

		void MakeRightTri(int sideGrids, int x1, int y, int gridSizeY,
			int indexCenter, int indexRightTop, int indexRightBottom, int rightLod,vector<WORD>& indices);

		const tagIB& _GetIB(int leftLod,int topLod,int rightLod,int bottomLod);

	private:
		int m_lod;
		int m_mapW,m_mapH;
		TPatchTriangulationDynamic* m_pParent;

		HardwareBufferPool<StaticIBAlloc> m_ibPool;
		map<int,tagIB> m_ibCache;
	};


	/**	\class TPatchTriangulationDynamic
		\brief 为地块产生三角形列表
		\remark 
			\par 支持LOD
			\par 内建硬件IndexBuffer缓存池，保存已创建的三角形排列方案
	*/
	class TPatchTriangulationDynamic
	{
		friend class TPatchLodDynamic;
	public:
		enum 
		{ 
			NUM_LOD=3
		};
	public:
		void Create();
		void Destroy();

		IIndexBuffer* GetIB_HW(int lod,int leftLod,int topLod,int rightLod,int bottomLod,int& numVerts,int& numFaces);
		const vector<WORD>& GetIB(int lod,int leftLod,int topLod,int rightLod,int bottomLod);

		static TPatchTriangulationDynamic* Inst();
        
	private:
		TPatchTriangulationDynamic(void);
		~TPatchTriangulationDynamic(void);

		TPatchLodDynamic m_lods[NUM_LOD];
		static int m_sideGrids[NUM_LOD];//每边grid数量
		static int m_sideVerts[NUM_LOD];//每边顶点数量
	};

}//end namespace Cool3D