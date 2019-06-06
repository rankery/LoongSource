#pragma once
#include "MathBaseDefine.h"
#include "VertexDefine.h"

namespace Cool3D
{
	const WCHAR* const StaticMesh_Magic=L"SMM";
/*--Simple Mesh 文件结构-------------------------
	[StaticMeshHeader]
	[Vert] * header.numVert;
	[baseIndex]-WORD	--|
	[numFace]-DWORD		  |
	[mtlID]-DWORD		  |} *header.numMesh
	[TriFace]*numFace	--|
	[MtlDesc] * header.numMtl;
	*/
	struct StaticMeshHeader
	{
		WCHAR	magic[4];
		int		ver;
		EVertType	vertType;
		DWORD	numVert;
		DWORD	offsetVert;
		DWORD	numMesh;
		DWORD	offsetMesh;
		DWORD	numMtl;
		DWORD	offsetMtl;
		DWORD	numTagNode;
		DWORD	offsetTagNode;
		Vector3	min;
		Vector3	max;

		int		lightmapSize;

		DWORD	reserve[63];
	};

	const WCHAR* const SceneMesh_Magic=L"SCN";

	struct SceneMeshHeader : public StaticMeshHeader
	{
		DWORD	boxTreeOffset;
	};

	struct SceneMeshNavHeader
	{
		WCHAR	magic[4];
		int		ver;		// 1-八叉树 2-二叉树
		DWORD	offsetOctree;
	};
	const WCHAR* const SceneMeshNav_Magic=L"NAV";
}//namespace Cool3D