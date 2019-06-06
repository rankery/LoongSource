
#pragma once
#include "..\Util\SharedPtr.h"
#include "\Work\Public\MtlDesc.h"
#include "\Work\Public\VertexDefine.h"
#include "..\VFS\IFS.h"

namespace F3D
{
	struct tagSubMesh
	{
		WORD			baseIndex;
		DWORD			mtlID;
		WORD			numVert;
		DWORD			numFaces;
		Vector3			max;
		Vector3			min;

		SharedWordPtr	pFaces;//WORD face index buffer
		vector<FaceTopo> faceTopoArray;
		
		tagMtlDesc		*pMtlDesc;//指向Resource中的MtlDesc数据
		

		tagSubMesh()
		{
			numFaces=baseIndex=numVert=0;
			pMtlDesc=NULL;
			mtlID=0;
		}
		~tagSubMesh()
		{}
		tagSubMesh(const tagSubMesh& right)
		{
			pFaces=right.pFaces;
			numFaces=right.numFaces;
			pMtlDesc=right.pMtlDesc;
			baseIndex=right.baseIndex;
			numVert=right.numVert;
			mtlID=right.mtlID;
		}
		const tagSubMesh& operator = (const tagSubMesh& right)
		{
			if(this==&right)
				return *this;
			pFaces=right.pFaces;
			numFaces=right.numFaces;
			pMtlDesc=right.pMtlDesc;
			baseIndex=right.baseIndex;
			numVert=right.numVert;
			mtlID=right.mtlID;
			return *this;
		}

		UINT GetIndexBufSize() const
		{	return sizeof(WORD)*3*numFaces; }

		void ReadFromFile(IFS *pFS,DWORD hFile)
		{
			ASSERT(pFS!=NULL);
			ASSERT(hFile!=NULL);

			pFS->Read(&baseIndex,sizeof(baseIndex),hFile);
			pFS->Read(&mtlID,sizeof(mtlID),hFile);
			pFS->Read(&numVert,sizeof(numVert),hFile);
			pFS->Read(&numFaces,sizeof(numFaces),hFile);
			pFS->Read(&max,sizeof(Vector3),hFile);
			pFS->Read(&min,sizeof(Vector3),hFile);

			UINT size=GetIndexBufSize();
			pFaces.Bind((WORD*)malloc(size));
			ASSERT(pFaces.GetMem()!=NULL);
			pFS->Read(pFaces.GetMem(),size,hFile);

			int numFace=0;
			pFS->Read(&numFace,sizeof(int),hFile);
			faceTopoArray.reserve(numFace);
			int i=0;
			FaceTopo face;
			for(i=0;i<numFace;i++)
			{
				pFS->Read(&face,sizeof(FaceTopo),hFile);
				faceTopoArray.push_back(face);
			}
		}
	};
}//namespace F3D