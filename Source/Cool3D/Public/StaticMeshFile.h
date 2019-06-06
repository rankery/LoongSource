#pragma once
#include "MathBaseDefine.h"
#include "VertexDefine.h"

namespace Cool3D
{
	const TCHAR* const StaticMesh_Magic=_T("SMM");
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
		TCHAR	magic[4];
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

	const TCHAR* const SceneMesh_Magic=_T("SCN");

	struct SceneMeshHeader : public StaticMeshHeader
	{
		DWORD	boxTreeOffset;
	};

	struct SceneMeshNavHeader
	{
		TCHAR	magic[4];
		int		ver;
		DWORD	offsetOctree;
	};
	const TCHAR* const SceneMeshNav_Magic=_T("NAV");
}//namespace Cool3D