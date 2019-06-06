#include "StdAfx.h"
#include ".\rendersubmesh.h"
#include "..\Util\StlExt.h"

namespace Cool3D
{
	RenderSubMesh::RenderSubMesh(void)
	{
	}

	RenderSubMesh::~RenderSubMesh(void)
	{
		FreePtrVector<IIndexBuffer> freeIBs;
		freeIBs(m_lodFaces);
	}
}//namespace Cool3D