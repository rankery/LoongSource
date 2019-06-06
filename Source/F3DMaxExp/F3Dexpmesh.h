#pragma warning(disable:4251)
#pragma warning(disable:4275)

#pragma once
#include "F3DMaxExp.h"
#include "Noncopyable.h"
#include "MeshBaseDef.h"

struct NormalVerSm;
struct TangentVerSm;

namespace Cool3D
{
	struct F3D_MAX_API ExpSubMesh : private Noncopyable
	{
		unsigned int	m_nMtlID;
		std::vector<Polygon>	m_faceArray;
	};
	
	class F3D_MAX_API F3DExpMeshMtlCallBack
	{
	public:
		F3DExpMeshMtlCallBack()	{}
		virtual ~F3DExpMeshMtlCallBack()	{}
		virtual int CallBack(Mtl *pMaxMtl, int sub,Mtl *pBakedMtl) =0;
	};
	/**	\class F3DExpMesh
	\brief Mesh节点数据
	
	  主要包括Mesh的Vertices,Faces,MtlID
	*/
	
	class F3D_MAX_API F3DExpMesh : private Noncopyable
	{
	public:
		struct ExpVert
		{
			int nPosNormal;
			int nUVDiffuse;
			int nUVLightMap;
			DWORD	dwSmGrp;
			Texmap *pMap0;//for uvtrans
			Texmap *pMap1;
			int nFaceIndex;
			int nUVBumpDir;	//bump uv方向，只有正反两种方向，分别用0和1表示
			
			ExpVert(int pos,DWORD smGrp,int faceIndex,int uvDiffuse=-1,Texmap *_pMap0=NULL,int uvLightMap=-1,
				Texmap *_pMap1=NULL,int uvBumpDir=-1)
			{
				nPosNormal=pos;
				dwSmGrp=smGrp;
				nFaceIndex=faceIndex;
				nUVDiffuse=uvDiffuse;
				pMap0=_pMap0;
				nUVLightMap=uvLightMap;
				pMap1=_pMap1;
				nUVBumpDir=uvBumpDir;
			}
		};

			F3DExpMesh();
			virtual ~F3DExpMesh();
			
			void Build(INode *pNode,TimeValue time,F3DExpMeshMtlCallBack *pMtlCallBack,bool tri=true);
			int GetNumFaces()
			{
				int sum=0;
				for(size_t i=0;i<m_subMeshs.size();i++)
					sum+=m_subMeshs[i]->m_faceArray.size();
				return sum;
			}
			int GetNumSubMesh()
			{
				return m_subMeshs.size();
			}
			ExpSubMesh *GetSubMesh(int i)
			{
				return m_subMeshs[i];
			}
			int GetNumVert()
			{
				return m_posArray.size();
			}
			Point3 GetPos(int i)
			{ return m_posArray[i]; }
			Point3 GetNormal(int i)
			{ return m_normalArray[i]; }
			bool IfUV()
			{ return m_uvArray.size() > 0;}
			Point3 GetUV(int i)
			{
				return m_uvArray[i];
			}
			bool IfUV2()
			{ return m_uv2Array.size() > 0;}
			Point3 GetUV2(int i)
			{ 
				return m_uv2Array[i];
			}
			bool IfTangent()
			{
				return m_tangentArray.size()>0;
			}
			Point3 GetTangent(int i)
			{
				return m_tangentArray[i];
			}
			
			int GetNumMapChannel()
			{
				int num=0;
				if(m_uvArray.size() > 0)
					num++;
				if(m_uv2Array.size() > 0)
					num++;
				return num;
			}
			
		protected:
		/**增加一个Max Face, 根据face子材质不同自动分成不同的sub mesh
		\param pNodeMtl 此Node的mtl
		\param maxMesh 此Node的Mesh
		\param index 欲增加的Face在Mesh的Face数组中的下标
		\return void
		\see MergeVert()
			*/
			void MergeFace(Mtl* pNodeMtl,Mesh& maxMesh,int index,bool bSwapFace);
			/** 向Vertex buffer中增加一个顶点
			\param maxMesh renderable mesh
			\param exp 从MAX的Face以及TVFace中得到的顶点信息
			\return 在顶点数组中的索引
			\remarks 程序先在已有顶点缓冲中搜索, 如果没有则新建一个顶点加入vb中
			*/
			unsigned short MergeVert(Mesh& maxMesh,ExpVert& exp,int faceIndex);
			//--export info
			std::vector<ExpVert> m_expVerts;
			
			std::vector<NormalVerSm>	m_maxMeshNormals;
			std::vector<TangentVerSm>	m_maxMeshTangents;
			//--vertex data
			std::vector<Point3>	m_posArray;
			std::vector<Point3>	m_normalArray;
			std::vector<Point3>	m_uvArray;
			std::vector<Point3>	m_uv2Array;//lighting map
			std::vector<Point3> m_tangentArray;	//tangent
			
			//--sub mesh
			std::vector<ExpSubMesh*> m_subMeshs;
			
			bool	m_bTri;
			
			//--mtl call back
			F3DExpMeshMtlCallBack *m_pMtlCallBack;
		protected:
			void GetTransUV(Point3& uv,Texmap* pMap);
			Point3 FindNormal(DWORD vertexIndex,DWORD smGroup,int faceIndex);
			Point3 FindTangent(DWORD vertexIndex,DWORD smGroup,int faceIndex,DWORD _uv,int _uvDir);
	};	
}//namespace Cool3D

#pragma warning(default:4251)
#pragma warning(default:4275)