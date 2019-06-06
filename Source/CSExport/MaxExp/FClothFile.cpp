#include "..\stdafx.h"
#include ".\ExpCloth.h"
#include ".\FClothFile.h"
#include "..\..\F3DMaxExp\SubMeshOptimize.h"
using namespace std;

namespace
{
	void quick_sort(ExpClothMesh *pMesh, vector<DWORD>& index, int low, int high)
	{
		if( low == high )
			return;
		else if( low+1 == high ) 
		{
			SkinVert& lowVert = pMesh->GetClothVert(index[low]);
			SkinVert& highVert = pMesh->GetClothVert(index[high]);

			if( lowVert.pos.y - highVert.pos.y > 2.0f
				|| (fabsf(lowVert.pos.y - highVert.pos.y)<2.0f && lowVert.pos.x>highVert.pos.x ) )
			{
				swap(index[low], index[high]);
				return;
			}
		}

		int middle = (low + high) / 2;
		SkinVert& pivot = pMesh->GetClothVert(index[middle]);

		int i=low;
		int j=high;
		do 
		{
			SkinVert iVert = pMesh->GetClothVert(index[i]);
			while( (pivot.pos.y - iVert.pos.y > 2.0f
				|| (fabsf(iVert.pos.y-pivot.pos.y)<2.0f && iVert.pos.x<pivot.pos.x))
				&& i<high )
			{
				i++;
				iVert = pMesh->GetClothVert(index[i]);
			}

			SkinVert jVert = pMesh->GetClothVert(index[j]);
			while( (jVert.pos.y - pivot.pos.y > 2.0f
				|| (fabsf(jVert.pos.y-pivot.pos.y)<2.0f && pivot.pos.x<jVert.pos.x))
				&& j>low )
			{
				j--;
				jVert = pMesh->GetClothVert(index[j]);
			}

			if( i<=j ) 
			{
				swap(index[i],index[j]);
				i++;
				j--;
			}
		} while( i<=j );

		if( low < j ) 
		{
			quick_sort(pMesh, index, low, j);
		}
		if( high > i )
		{
			quick_sort(pMesh, index, i, high);
		}
	}
}

FClothFile::FClothFile()
{

}

FClothFile::~FClothFile()
{

}

bool FClothFile::WriteToFile(const TCHAR* szFileName,ExpCloth& cloth,ExpClothMesh *pMesh)
{
	if(pMesh==NULL)
		return false;
	FILE *fp=_tfopen(szFileName,_T("wb"));
	if(fp==NULL)
		return false;

	vector<tagBlock>& blocks = cloth.GetBlocks();

	//--write header
	tagFClothHeader header;
	memset(&header,0,sizeof(tagFClothHeader));
	wcscpy(header.magic,FCLOTH_MAGIC);
	header.ver=1;
	header.numVert=pMesh->GetNumVert();
	header.numSubMesh=pMesh->GetNumSubMesh();
	header.numBlock = blocks.size();
	pMesh->BuildAABBox(header.max,header.min);
	FWriteValue(fp,header);

	UINT i;
	//--write vert
	header.vertOffset=ftell(fp);
	for(i=0;i<header.numVert;i++)
	{
		SkinVert& v=pMesh->GetClothVert(i);
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
	ExpClothMeshMtlCallBack *pMtlCallBack=pMesh->GetMtl();
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

		pMtlCallBack->m_mtls[i]->GetMtlDesc(md);
		fwrite(&md,sizeof(tagMtlDesc),1,fp);
	}

	//--cloth，选出适合做mass的顶点
	header.clothOffset = ftell(fp);
	vector<DWORD> clothList;
	vector<DWORD> tagList;
	vector<DWORD> clothArray;
	vector<DWORD> tagArray;
	int u,v;
	for(i=0;i<header.numVert;i++)
	{
		SkinVert& v=pMesh->GetClothVert(i);

		if( v.boneInfs.size() == 0 )
			clothList.push_back(i);
		else
			tagList.push_back(i);
	}
	GetClothTag(pMesh,clothList,tagList,clothArray,tagArray,u,v);
	FWriteValVector(fp,clothArray);

	//--tag
	header.tagOffset = ftell(fp);
	FWriteValVector(fp,tagArray);

	//--spring
	header.springOffset = ftell(fp);
	vector<tagSpring> springArray;
	CalcSpring(pMesh,clothArray,u,v,springArray);
	FWriteValVector(fp,springArray);

	//--block
	header.blockOffset = ftell(fp);
	for( i=0; i<header.numBlock; i++ )
	{
		tagBlock& block = blocks[i];
		block.v0.WriteFile(fp);
		block.v1.WriteFile(fp);
		block.v2.WriteFile(fp);
	}

	//--write back header
	fseek(fp,0,SEEK_SET);
	FWriteValue(fp,header);

	fclose(fp);
	return true;
}

void FClothFile::GetClothTag( ExpClothMesh *pMesh,vector<DWORD>& oldCloth,vector<DWORD>& oldTag,vector<DWORD>& newCloth,vector<DWORD>& newTag,int& u,int& v )
{
	quick_sort(pMesh, oldCloth, 0, oldCloth.size()-1);

	// 从挂载点中找出距离布料最近的点作为来自骨骼的布料外力
	SkinVert& maxVert = pMesh->GetClothVert(oldCloth[oldCloth.size()-1]);
	float maxy = maxVert.pos.y;
	float dist = 1.0e10f;
	for( size_t i=0; i<oldTag.size(); i++ )
	{
		SkinVert& v = pMesh->GetClothVert(oldTag[i]);

		if( fabsf(v.pos.y - maxy) - dist < 2.0f && fabsf(v.pos.y - maxy) - dist > -2.0f )
		{
			newTag.push_back(oldTag[i]);
		}
		else if( fabsf(v.pos.y - maxy) < dist )
		{
			dist = fabsf(v.pos.y - maxy);
			newTag.clear();
			newTag.push_back(oldTag[i]);
		}
	}

	u = newTag.size();
	quick_sort(pMesh, newTag, 0, newTag.size()-1);

	newCloth.resize( newTag.size() + oldCloth.size() );
	memcpy(&newCloth[0], &oldCloth[0], sizeof(DWORD)*oldCloth.size());
	memcpy(&newCloth[oldCloth.size()], &newTag[0], sizeof(DWORD)*newTag.size());

	v = newCloth.size() / u;

	assert( u*v == newCloth.size() );
}

void FClothFile::CalcSpring( ExpClothMesh *pMesh,const vector<DWORD>& clothArray,int u,int v,vector<tagSpring>& springArray )
{
	int i,j;

	// Structural Horizontal Springs
	for( i=0; i<v; i++ )
	{
		for( j=0; j<u-1; j++)
		{
			tagSpring spring;
			spring.type = 0;
			spring.ks = 4.0f;
			spring.kd = 0.6f;
			spring.p1 = i*u+j;
			spring.p2 = i*u+j+1;

			SkinVert& v1 = pMesh->GetClothVert(clothArray[spring.p1]);
			SkinVert& v2 = pMesh->GetClothVert(clothArray[spring.p2]);
			spring.restLen = D3DXVec3Length( &(v1.pos - v2.pos) );

			springArray.push_back( spring );
		}
	}

	// Structural Vertical Springs
	for( i=0; i<u; i++ )
	{
		for( j=0; j<v-1; j++ )
		{
			tagSpring spring;
			spring.type = 0;
			spring.ks = 4.0f;
			spring.kd = 0.6f;
			spring.p1 = j*u+i;
			spring.p2 = (j+1)*u+i;

			SkinVert& v1 = pMesh->GetClothVert(clothArray[spring.p1]);
			SkinVert& v2 = pMesh->GetClothVert(clothArray[spring.p2]);
			spring.restLen = D3DXVec3Length( &(v1.pos - v2.pos) );

			springArray.push_back( spring );
		}
	}

	// Shearing Springs
	for( i=0; i<v-1; i++)
	{
		for( j=0; j<u-1; j++ )
		{
			tagSpring spring;
			spring.type = 1;
			spring.ks = 4.0f;
			spring.kd = 0.6f;
			spring.p1 = i*u+j;
			spring.p2 = (i+1)*u+j+1;

			SkinVert& v1 = pMesh->GetClothVert(clothArray[spring.p1]);
			SkinVert& v2 = pMesh->GetClothVert(clothArray[spring.p2]);
			spring.restLen = D3DXVec3Length( &(v1.pos - v2.pos) );

			springArray.push_back( spring );

			spring.ks = 4.0f;
			spring.kd = 0.6f;
			spring.p1 = (i+1)*u+j;
			spring.p2 = i*u+j+1;

			SkinVert& v3 = pMesh->GetClothVert(clothArray[spring.p1]);
			SkinVert& v4 = pMesh->GetClothVert(clothArray[spring.p2]);
			spring.restLen = D3DXVec3Length( &(v3.pos - v4.pos) );

			springArray.push_back( spring );
		}
	}

	// Bend Springs
	for( i=0; i<v; i++ )
	{
		for( j=0; j<u-2; j++)
		{
			tagSpring spring;
			spring.type = 2;
			spring.ks = 2.4f;
			spring.kd = 0.8f;
			spring.p1 = i*u+j;
			spring.p2 = i*u+j+2;

			SkinVert& v1 = pMesh->GetClothVert(clothArray[spring.p1]);
			SkinVert& v2 = pMesh->GetClothVert(clothArray[spring.p2]);
			spring.restLen = D3DXVec3Length( &(v1.pos - v2.pos) );

			springArray.push_back( spring );
		}

		tagSpring spring;
		spring.type = 2;
		spring.ks = 2.4f;
		spring.kd = 0.8f;
		spring.p1 = i*u+u-3;
		spring.p2 = i*u+u-1;

		SkinVert& v1 = pMesh->GetClothVert(clothArray[spring.p1]);
		SkinVert& v2 = pMesh->GetClothVert(clothArray[spring.p2]);
		spring.restLen = D3DXVec3Length( &(v1.pos - v2.pos) );

		springArray.push_back( spring );
	}
	for( i=0; i<u; i++ )
	{
		for( j=0; j<v-2; j++ )
		{
			tagSpring spring;
			spring.type = 2;
			spring.ks = 2.4f;
			spring.kd = 0.8f;
			spring.p1 = j*u+i;
			spring.p2 = (j+2)*u+i;

			SkinVert& v1 = pMesh->GetClothVert(clothArray[spring.p1]);
			SkinVert& v2 = pMesh->GetClothVert(clothArray[spring.p2]);
			spring.restLen = D3DXVec3Length( &(v1.pos - v2.pos) );

			springArray.push_back( spring );
		}

		tagSpring spring;
		spring.type = 2;
		spring.ks = 2.4f;
		spring.kd = 0.8f;
		spring.p1 = (v-3)*u+i;
		spring.p2 = (v-1)*u+i;

		SkinVert& v1 = pMesh->GetClothVert(clothArray[spring.p1]);
		SkinVert& v2 = pMesh->GetClothVert(clothArray[spring.p2]);
		spring.restLen = D3DXVec3Length( &(v1.pos - v2.pos) );

		springArray.push_back( spring );
	}
}