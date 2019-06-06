#include "..\SceneWriter.h"
#include "..\SceneMesh\OctreeBuilder.h"
#include <vector>

#pragma once

namespace Cool3D
{
	/**	\class StaticMeshWriter 
		\brief 没有任何预处理的简单几何物体的输出
	*/
	class StaticMeshWriter : public SceneWriter  
	{
	public:
		StaticMeshWriter();
		virtual ~StaticMeshWriter();

		virtual bool Write(const TCHAR* szPath, INode *pNode=NULL);
		
	protected:
		virtual void Prepare();

	protected:
		void ComputeAABBox();
		Vector3	min,max;

		//考虑到顶点格式的变化，这里以浮点数为单位进行存储
		std::vector<BYTE>		m_vertBuf;
		std::vector<LodSubMesh> m_subMeshs;
		TagNodeFrame			m_tagNodeFrm;
		OctreeBuilder			m_octree;
		INode					*m_pOriginNode;
		
		void GetVertices(ExpMesh *pMesh);
		void GetSubMeshs(WORD baseIndex,WORD numVert,ExpMesh* pMesh);
	};
}//namespace Cool3D