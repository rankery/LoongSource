#include "MathBaseDefine.h"
#include "FileUtil.h"
#pragma once

namespace Cool3D
{
	struct TriFace
	{
		WORD index[3];
	};
	typedef std::vector<TriFace>	FaceBuffer;

	class SubMesh
	{
	public:
		SubMesh()	{}
		virtual ~SubMesh()	{}
	public:
		//--
		WORD	baseIndex;
		int		mtlID;
		WORD	numVert;
		
		Vector3 max;
		Vector3 min;

		FaceBuffer	baseFaces;

		virtual void WriteFile(FILE *fp)
		{
			FWriteValue(fp,baseIndex);
			FWriteValue(fp,mtlID);
			FWriteValue(fp,numVert);
			FWriteValue(fp,max);
			FWriteValue(fp,min);
			FWriteValVector(fp,baseFaces);
		}
#ifdef VFS_READ
		virtual void ReadFile(IFS *pFS,DWORD hFile)
		{
			FReadValue(pFS,hFile,baseIndex);
			FReadValue(pFS,hFile,mtlID);
			FReadValue(pFS,hFile,numVert);
			FReadValue(pFS,hFile,max);
			FReadValue(pFS,hFile,min);
			FReadValVector(pFS,hFile,baseFaces);
		}
#endif
		
	public:
		SubMesh(const SubMesh& other)
		{	CopySubMesh(&other);	}

		const SubMesh& operator = (const SubMesh& other)
		{
			if(this==&other)
				return *this;
			CopySubMesh(&other);
			return *this;
		}

		void CopySubMesh(const SubMesh* pOther)
		{
			baseIndex	=	pOther->baseIndex;
			mtlID		=	pOther->mtlID;
			numVert		=	pOther->numVert;
			max			=	pOther->max;
			min			=	pOther->min;
			baseFaces	=	pOther->baseFaces;
		}
	};

	class LodSubMesh : public SubMesh
	{
	public:
		LodSubMesh()	{}
		virtual ~LodSubMesh()	{}
	public:
		enum LOD_SUBMESH_CONST
		{	MAX_NUM_LOD=4};
		
		int			numLod;	//lod0 save in base class
		FaceBuffer	lodFaces[MAX_NUM_LOD];
		
	public:
		virtual void WriteFile(FILE *fp)
		{
			SubMesh::WriteFile(fp);
			FWriteValue(fp,numLod);
			for(int i=0;i<MAX_NUM_LOD;i++)
				FWriteValVector(fp,lodFaces[i]);
		}
#ifdef VFS_READ
		virtual void ReadFile(IFS *pFS,DWORD hFile)
		{
			SubMesh::ReadFile(pFS,hFile);
			FReadValue(pFS,hFile,numLod);
			for(int i=0;i<MAX_NUM_LOD;i++)
				FReadValVector(pFS,hFile,lodFaces[i]);
		}
#endif
		
		public:
			LodSubMesh(const LodSubMesh& other)
			{	CopyLodSubMesh(&other);	}
			const LodSubMesh& operator = (const LodSubMesh& other)
			{
				if(this==&other)
					return *this;
				CopyLodSubMesh(&other);
				return *this;
			}

		protected:
			void CopyLodSubMesh(const LodSubMesh* pOther)
			{
				numLod=pOther->numLod;
				for(int i=0;i<MAX_NUM_LOD;i++)
					lodFaces[i]=pOther->lodFaces[i];
				SubMesh::CopySubMesh(pOther);
			}
	};
}//namespace Cool3D