#include "StdAfx.h"
#include "AniSkin_SW.h"
#include "AnimationCtrl.h"
#include "..\VFS\IFS.h"
#define VFS_READ
#include "..\Public\SkinBoneDefine.h"
#include "..\ResSys\ResSkin.h"

namespace Cool3D
{
	AniSkin_SW::AniSkin_SW(void)
	{
		m_pResSkin=NULL;
		m_numLod=0;
	}

	AniSkin_SW::~AniSkin_SW(void)
	{
		m_pResSkin=NULL;
	}

	void AniSkin_SW::Create(const ResSkin *pSkin)
	{
		m_pResSkin=const_cast<ResSkin*>(pSkin);
		CreateVertStream(pSkin);
		CreateIndices(pSkin);
	}

	void AniSkin_SW::CreateVertStream(const ResSkin *pSkin)
	{
		//--初始化m_vertStream
		const vector<SkinVert>& skinVertArray=pSkin->GetSkinVerts();
		size_t numVert=skinVertArray.size();
		m_vertStream.Create(EVType_PNT,numVert);
		Vert_PNT *pVB=(Vert_PNT *)m_vertStream.GetRawBuffer();

		size_t i;
		for(i=0;i<numVert;i++)
		{
			pVB[i].pos=skinVertArray[i].pos;
			pVB[i].normal=skinVertArray[i].normal;
			pVB[i].uv=skinVertArray[i].uv;
		}
	}

	void AniSkin_SW::CreateIndices(const ResSkin *pSkin)
	{
		//--清空现有的index
		m_numLod=0;
		int i;
		for(i=0;i<NUM_LOD;i++)
			m_indices[i].clear();

		//--找出所有lod的index，保存到临时set
		set<int> tmp[NUM_LOD];
		const vector<LodSkinSubMesh*>& lodSubMesh=pSkin->GetLodSubMesh();
		for(i=0;i<(int)lodSubMesh.size();i++)
		{
			const LodSubMesh* pLodSubMesh=lodSubMesh[i];
			int numLod=min(NUM_LOD,pLodSubMesh->numLod);
			if(numLod>m_numLod)
				m_numLod=numLod;
			for(int lod=0;lod<numLod;lod++)
			{
				const FaceBuffer& faceBuffer=pLodSubMesh->lodFaces[lod];
				for(size_t j=0;j<faceBuffer.size();j++)
				{
					const TriFace& face=faceBuffer[j];
					tmp[lod].insert(face.index[0]);
					tmp[lod].insert(face.index[1]);
					tmp[lod].insert(face.index[2]);
				}
			}
		}
		//--从临时set导出到vector,使用vector能提高cpu cache命中率?
		for(i=0;i<m_numLod;i++)
		{
			set<int>& src=tmp[i];
			vector<int>& dst=m_indices[i];
			for(set<int>::iterator iter=src.begin();
				iter!=src.end();iter++)
			{
				dst.push_back(*iter);
			}
		}
	}

	void AniSkin_SW::ComputeSkinVB(const NodeAnimationCtrl *pCtrl, int lod)
	{
		if(m_pResSkin==NULL
			|| !m_pResSkin->IsCreated())
			return;

		const vector<SkinVert>& skinVerts=m_pResSkin->GetSkinVerts();
		Vert_PNT *pVB=(Vert_PNT *)m_vertStream.GetRawBuffer();
		size_t numSkinVert=skinVerts.size();

		if(lod==0||m_numLod==0)
		{
			size_t i,j;
			size_t numBoneInf=0;
			for(i=0;i<numSkinVert;i++)
			{
				const SkinVert& skinVert=skinVerts[i];
				pVB->pos=MathConst::Zero3;
				numBoneInf=skinVert.boneInfs.size();
				for(j=0;j<numBoneInf;j++)
				{
					const tagBoneInf& inf=skinVert.boneInfs[j];
					const Matrix4& boneSpace=*(pCtrl->GetBoneSpace(inf.boneId));
					pVB->pos+=(inf.offset*boneSpace)*inf.weight;
				}
				pVB++;
			}
		}
		else if(lod>0&&m_numLod>0)
		{
			size_t j;
			int index;
			size_t numBoneInf=0;
			lod--;
			if(lod>=m_numLod)
				lod=m_numLod-1;
			vector<int>& indices =m_indices[lod];
			size_t numIndex=indices.size();
			for(size_t i=0;i<numIndex;i++)
			{
				index=indices[i];
				const SkinVert& skinVert=skinVerts[index];
				Vert_PNT& vert=pVB[index];

				vert.pos=MathConst::Zero3;
				numBoneInf=skinVert.boneInfs.size();
				for(j=0;j<numBoneInf;j++)
				{
					const tagBoneInf& inf=skinVert.boneInfs[j];
					const Matrix4& boneSpace=*(pCtrl->GetBoneSpace(inf.boneId));
					vert.pos+=(inf.offset*boneSpace)*inf.weight;
				}
			}
		}
	}
}//namespace Cool3D