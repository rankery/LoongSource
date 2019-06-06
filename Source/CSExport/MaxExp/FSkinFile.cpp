#include "..\stdafx.h"
#include ".\ExpSkin.h"
#include ".\FSkinFile.h"
#include "..\..\F3DMaxExp\SubMeshOptimize.h"
using namespace std;

FSkinFile::FSkinFile()
{

}

FSkinFile::~FSkinFile()
{

}

bool FSkinFile::WriteToFile(const TCHAR* szFileName,ExpSkinMesh *pMesh)
{
	if(pMesh==NULL)
		return false;
	FILE *fp=_tfopen(szFileName,_T("wb"));
	if(fp==NULL)
		return false;

	//--write header
	tagFSkinHeader header;
	memset(&header,0,sizeof(tagFSkinHeader));
	wcscpy(header.magic,FSKIN_MAGIC);
	header.ver=1;
	header.vertType=pMesh->GetVertType();
	header.numVert=pMesh->GetNumVert();
	header.numSubMesh=pMesh->GetNumSubMesh();
	pMesh->BuildAABBox(header.max,header.min);
	FWriteValue(fp,header);

	UINT i;
	//--write vert
	header.vertOffset=ftell(fp);
	for(i=0;i<header.numVert;i++)
	{
		SkinVert& v=pMesh->GetSkinVert(i);
		v.WriteFile(fp);
	}

	//--write sub mesh
	header.subMeshOffset=ftell(fp);
	for(i=0;i<header.numSubMesh;i++)
	{
		LodSkinSubMesh *pm=pMesh->GetLodSubMesh(i);
		if( pm!=NULL )
		{
			SubMeshOptimize(pm);
			pm->WriteFile(fp);
		}
	}
	
	//--mtl
	ExpSkinMeshMtlCallBack *pMtlCallBack=pMesh->GetMtl();
	header.mtlOffset=ftell(fp);
	header.numMtl=pMtlCallBack->m_mtls.size();

	tstring szExportPath=szFileName;
	int p=szExportPath.find_last_of(_T('\\'));
	if(p!=tstring::npos)
	{
		szExportPath.erase(p);
	}

	for(i=0;i<header.numMtl;i++)
	{
		tagMtlDesc md;

		////--蒙皮材质不能有半透明贴图，因为运算量很大
		//if(md.m_bOpcityMap
		//	&& md.m_opcityVal==255)
		//	return false;

		pMtlCallBack->m_mtls[i]->GetMtlDesc(md);
		fwrite(&md,sizeof(tagMtlDesc),1,fp);
	}

	//--write back header
	fseek(fp,0,SEEK_SET);
	FWriteValue(fp,header);

	fclose(fp);
	return true;
}