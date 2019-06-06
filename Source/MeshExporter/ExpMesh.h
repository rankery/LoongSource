#include "Max.h"
#include <vector>

#pragma once
#include "..\F3DMaxExp\F3DExpMesh.h"

namespace Cool3D
{	
	class SceneMtlCallBack : public F3DExpMeshMtlCallBack
	{
	public:
		SceneMtlCallBack()	{}
		virtual ~SceneMtlCallBack()	{}
		virtual int CallBack(Mtl *pMaxMtl, int sub,Mtl *pBakedMtl);
	};

	/**	\class ExpMesh
		\brief Mesh节点数据
		主要包括Mesh的Vertices,Faces,MtlID
	*/
	
	class ExpMesh : public F3DExpMesh
	{
	public:
		ExpMesh(INode *pNode,TimeValue time,bool tri=true);
		virtual ~ExpMesh();

	private:
		SceneMtlCallBack	m_mtlCallBack;

	};	
}