#pragma once
#include "SubMesh.h"

namespace Cool3D
{
	struct tagBoneOffsetMatrix
	{
		int		boneId;
		Matrix4 offsetMatrix;
	};

	class LodSkinSubMesh : public LodSubMesh
	{
	public:
		LodSkinSubMesh()	{}
		virtual ~LodSkinSubMesh()	{}
	public:
		vector<tagBoneOffsetMatrix> baseBoneOM;
		
	public:
		virtual void WriteFile(FILE *fp)
		{
			LodSubMesh::WriteFile(fp);
			FWriteValVector(fp,baseBoneOM);
		}
#ifdef VFS_READ
		virtual void ReadFile(IFS *pFS,DWORD hFile)
		{
			LodSubMesh::ReadFile(pFS,hFile);
			FReadValVector(pFS,hFile,baseBoneOM);
		}
#endif
		
		public:
			LodSkinSubMesh(const LodSkinSubMesh& other)
			{	CopyLodSubMesh(&other);	}
			const LodSkinSubMesh& operator = (const LodSkinSubMesh& other)
			{
				if(this==&other)
					return *this;
				CopyLodSubMesh(&other);
				return *this;
			}

		protected:
			void CopyLodSubMesh(const LodSkinSubMesh* pOther)
			{
				baseBoneOM=pOther->baseBoneOM;
				LodSubMesh::CopySubMesh(pOther);
			}
	};

	class LodClothSubMesh : public LodSkinSubMesh
	{
	public:
		LodClothSubMesh():m_isSkin(true)	{}
		virtual ~LodClothSubMesh()			{}

	public:
		bool	m_isSkin;

	public:
		virtual void WriteFile(FILE *fp)
		{
			LodSkinSubMesh::WriteFile(fp);
			FWriteValue(fp,m_isSkin);
		}
#ifdef VFS_READ
		virtual void ReadFile(IFS *pFS,DWORD hFile)
		{
			LodSkinSubMesh::ReadFile(pFS,hFile);
			FReadValue(pFS,hFile,m_isSkin);
		}
#endif

	public:
		LodClothSubMesh(const LodClothSubMesh& other)
		{	CopyLodSubMesh(&other);	}

		const LodClothSubMesh& operator = (const LodClothSubMesh& other)
		{
			if(this==&other)
				return *this;
			CopyLodSubMesh(&other);
			return *this;
		}

	protected:
		void CopyLodSubMesh(const LodClothSubMesh* pOther)
		{
			m_isSkin=pOther->m_isSkin;
			LodSkinSubMesh::CopySubMesh(pOther);
		}
	};
}//namespace Cool3D