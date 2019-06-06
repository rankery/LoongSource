#pragma once
#include ".\MathBaseDefine.h"
#include ".\FileUtil.h"
#include ".\TagNodeDefine.h"
#include ".\VertexDefine.h"

namespace Cool3D
{
	const TCHAR* const FSKEL_MAGIC=_T("SKEL");
	struct FSkelHeader
	{
		TCHAR	magic[5];
		BYTE	ver;
		DWORD	structOffset;
		DWORD	numFrame;
		DWORD	frameOffset;
		DWORD	reserve[64];
	};
	const TCHAR* const FSKIN_MAGIC=_T("SKIN");
	struct tagFSkinHeader
	{
		TCHAR	magic[5];
		BYTE	ver;
		EVertType vertType;
		DWORD	numVert;
		DWORD	vertOffset;
		DWORD	numSubMesh;
		DWORD	subMeshOffset;
		DWORD	numMtl;
		DWORD	mtlOffset;
		Vector3	max,
				min;
		DWORD	reserve[64];
	};
	const TCHAR* const FCLOTH_MAGIC=_T("CLOTH");
	struct tagFClothHeader 
	{
		WCHAR	magic[5];
		BYTE	ver;
		DWORD	numVert;
		DWORD	vertOffset;
		DWORD	numSubMesh;
		DWORD	subMeshOffset;
		DWORD	numMtl;
		DWORD	mtlOffset;
		DWORD	clothOffset;
		DWORD	tagOffset;
		DWORD	springOffset;
		DWORD	numBlock;
		DWORD	blockOffset;
		Vector3 max,
				min;

		BYTE	reserve[51];	// 保留字节
	};

	struct tagBoneInf
	{
		int		boneId;
		float	weight;
		Vector3	offset;
	};

	struct tagSpring 
	{
		WORD	p1;			// 关联顶点
		WORD	p2;			// 关联顶点
		float	restLen;	// 静态弹簧长度
		float	ks;			// 弹性恢复系数
		float	kd;			// 阻尼系数
		BYTE	type;		// 0-structural 1-shearing 2-bend
	};

	//--skin---------------------------------------------------------------
	class SkinVert
	{
	public:
		Vector3				pos;
		Vector3				normal;
		Vector3				tangent;
		Vector4				blendWeight;
		DWORD				blendIndices;
		Vector2				uv;
		vector<tagBoneInf>	boneInfs;

	public:
		SkinVert()	{}
		~SkinVert()	{}
		SkinVert(const SkinVert& other)	
		{	Copy(other); }
		const SkinVert& operator = (const SkinVert& other)
		{
			if(this==&other)
				return *this;
			Copy(other);
			return *this;
		}

		void WriteFile(FILE *fp)
		{
			FWriteValue(fp,pos);
			FWriteValue(fp,normal);
			FWriteValue(fp,tangent);
			FWriteValue(fp,blendWeight);
			FWriteValue(fp,blendIndices);
			FWriteValue(fp,uv);
			FWriteValVector(fp,boneInfs);
		}
#ifdef VFS_READ
		void ReadFile(IFS *pFS,DWORD hFile)
		{
			FReadValue(pFS,hFile,pos);
			FReadValue(pFS,hFile,normal);
			FReadValue(pFS,hFile,tangent);
			FReadValue(pFS,hFile,blendWeight);
			FReadValue(pFS,hFile,blendIndices);
			FReadValue(pFS,hFile,uv);
			FReadValVector(pFS,hFile,boneInfs);
		}
#else
		void ReadFile(FILE *fp)
		{
			FReadValue(fp,pos);
			FReadValue(fp,normal);
			FReadValue(fp,tangent);
			FReadValue(fp,blendWeight);
			FReadValue(fp,blendIndices);
			FReadValue(fp,uv);
			FReadValVector(fp,boneInfs);
		}
#endif

	private:
		void Copy(const SkinVert& other)
		{
			pos=other.pos;
			normal=other.normal;
			tangent=other.tangent;
			blendWeight=other.blendWeight;
			blendIndices=other.blendIndices;
			uv=other.uv;
			boneInfs=other.boneInfs;
		}
	};

	struct tagBlock 
	{
		SkinVert v0;
		SkinVert v1;
		SkinVert v2;
	};
	//--bone/skeleton------------------------------------------------------
	class Bone
	{
	public:
		Bone()	{}
		~Bone()	{}
		Bone(const Bone& other)
		{
			Copy(other);
		}
		const Bone& operator = (const Bone& other)
		{
			if(this==&other)
				return *this;
			Copy(other);
			return *this;
		}

		void WriteFile(FILE *fp)
		{
			FWriteValue(fp,m_trans);
			FWriteValue(fp,m_rotat);
			FWriteValue(fp,m_bMirrored);
		}
#ifdef VFS_READ
		void ReadFile(IFS *pFS,DWORD hFile)
		{
			FReadValue(pFS,hFile,m_trans);
			FReadValue(pFS,hFile,m_rotat);
			FReadValue(pFS,hFile,m_bMirrored);
		}
#else
		void ReadFile(FILE *fp)
		{
			FReadValue(fp,m_trans);
			FReadValue(fp,m_rotat);
			FReadValue(fp,m_bMirrored);
		}
#endif//#ifdef VFS_READ
	public:
		Vector3			m_trans;		//平移
		Quaternion		m_rotat;		//旋转
		bool			m_bMirrored;
	private:
		void Copy(const Bone& other)
		{
			m_trans		=	other.m_trans;
			m_rotat		=	other.m_rotat;
			m_bMirrored	=	other.m_bMirrored;
		}
	};

	struct BoneStruct
	{
		UINT			id;
		UINT			parentID;
		vector<UINT>	children;
	};
	class SkeletonStruct
	{
	public:
		SkeletonStruct()	{}
		~SkeletonStruct()	{}
		SkeletonStruct(const SkeletonStruct& other)
		{
			m_boneNames=other.m_boneNames;
			m_rootBones=other.m_rootBones;
			m_boneStruct=other.m_boneStruct;
		}
		const SkeletonStruct& operator = (const SkeletonStruct& other)
		{
			if(this==&other)
				return *this;
			m_boneNames=other.m_boneNames;
			m_rootBones=other.m_rootBones;
			m_boneStruct=other.m_boneStruct;
			return *this;
		}
		
		void WriteFile(FILE *fp) const
		{
			int numBone=m_boneNames.size();
			FWriteValue(fp,numBone);
			for(int i=0;i<numBone;i++)
			{
				FWriteString(fp,m_boneNames[i].c_str());
			}
			FWriteValVector(fp,m_rootBones);

			for(int i=0;i<numBone;i++)
			{
				FWriteValue(fp,m_boneStruct[i].id);
				FWriteValue(fp,m_boneStruct[i].parentID);
				FWriteValVector(fp,m_boneStruct[i].children);
			}
		}
#ifdef VFS_READ
		void ReadFile(IFS *pFS,DWORD hFile)
		{
			int numBone=0;
			FReadValue(pFS,hFile,numBone);
			m_boneNames.clear();
			for(int i=0;i<numBone;i++)
			{
				tstring tmp;
				FReadString(pFS,hFile,tmp);
				m_boneNames.push_back(tmp);
			}
			FReadValVector(pFS,hFile,m_rootBones);

			m_boneStruct.resize(numBone);
			for(int i=0;i<numBone;i++)
			{
				FReadValue(pFS,hFile,m_boneStruct[i].id);
				FReadValue(pFS,hFile,m_boneStruct[i].parentID);
				FReadValVector(pFS,hFile,m_boneStruct[i].children);
			}
		}
#else
		void ReadFile(FILE *fp)
		{
			int numBone=0;
			FReadValue(fp,numBone);
			m_boneNames.clear();
			for(int i=0;i<numBone;i++)
			{
				tstring tmp;
				FReadString(fp,tmp);
				m_boneNames.push_back(tmp);
			}
			FReadValVector(fp,m_rootBones);

			m_boneStruct.resize(numBone);
			for(int i=0;i<numBone;i++)
			{
				FReadValue(fp,m_boneStruct[i].id);
				FReadValue(fp,m_boneStruct[i].parentID);
				FReadValVector(fp,m_boneStruct[i].children);
			}
		}
#endif//#ifdef VFS_READ
	public:
		vector<tstring>		m_boneNames;
		vector<UINT>		m_rootBones;
		vector<BoneStruct>	m_boneStruct;
	};
}//namespace Cool3D