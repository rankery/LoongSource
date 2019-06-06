#include "stdafx.h"
#include "SubMeshOptimize.h"
#include "NvTriStrip\include\nvTriStrip.h"
#include <assert.h>

void OptimizeFaceBuffer(FaceBuffer *pFaces,int numVerts)
{
	nVidia::PrimitiveGroup *primGroups=NULL;
	WORD numGroups=0;

	TriFace *pf=&(*pFaces->begin());
	nVidia::GenerateStrips((WORD*)pf,pFaces->size()*3,&primGroups,&numGroups);
	nVidia::PrimitiveGroup *primGroupsRemap=NULL;
	//nVidia::RemapIndices(primGroups,numGroups,numVerts,&primGroupsRemap);
	primGroupsRemap=primGroups;

	if(numGroups>1)
		return;
	if(primGroupsRemap->type!=nVidia::PT_LIST)
		return;

	pFaces->clear();
	assert(primGroupsRemap->numIndices%3==0);
	for(int i=0;i<primGroupsRemap->numIndices;i+=3)
	{
		TriFace tmp;
		tmp.index[0]=primGroupsRemap->indices[i];
		tmp.index[1]=primGroupsRemap->indices[i+1];
		tmp.index[2]=primGroupsRemap->indices[i+2];
		pFaces->push_back(tmp);
	}

	delete[] primGroups;
}

bool SubMeshOptimize(LodSubMesh *pMesh)
{

	nVidia::SetCacheSize(CACHESIZE_GEFORCE3);
	nVidia::SetListsOnly(true);//vert cache optimize only

	OptimizeFaceBuffer(&pMesh->baseFaces,pMesh->numVert);

	for(int i=0;i<pMesh->numLod;i++)
		OptimizeFaceBuffer(&pMesh->lodFaces[i],pMesh->numVert);

	return true;
}