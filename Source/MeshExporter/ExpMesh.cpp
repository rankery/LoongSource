#include "ExpMesh.h"
#include <assert.h>
#include <algorithm>
#include "SceneContext.h"
#include "stdmat.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include "..\F3DMaxExp\MaxMath.h"
#include "..\F3DMaxExp\MaxVNormal.h"

namespace Cool3D
{
	int SceneMtlCallBack::CallBack(Mtl *pMaxMtl, int sub,Mtl *pBakedMtl)
	{
		return SceneContext::Inst()->MergeMtl(pMaxMtl,sub,pBakedMtl);
	}

	ExpMesh::ExpMesh(INode *pNode,TimeValue time,bool tri)
	{
		F3DExpMesh::Build(pNode,time,&m_mtlCallBack,tri);
	}
	
	ExpMesh::~ExpMesh()
	{
	
	}
	
}
