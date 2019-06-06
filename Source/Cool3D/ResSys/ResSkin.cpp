#include "StdAfx.h"
#include ".\resskin.h"
#include "..\Util\Exception.h"
#include "..\RenderSys\RSkin.h"
#include "..\Util\StlExt.h"
#include "..\Public\MtlDesc.h"
#include "..\Math\GeomDefine.h"
#include "..\RenderMtl\MSkin.h"
#include "..\Cool3DOption.h"

namespace Cool3D
{
	class ResSkin::Member
	{
	public:
		vector<SkinVert>			m_vertices;	//skin顶点数组
		vector<LodSkinSubMesh*>		m_subMeshs;	//face mesh数组
		vector<RSkin*>				m_renderNodes;
		vector<tagMtlDesc>			m_mtlDescs;
		AABBox						m_box;
		IVertBuffer*				m_pStaticVB;
		EVertType					m_staticVertType;

	public:
		Member()	
		{
			m_pStaticVB=NULL;
			m_staticVertType=EVType_None;
		}
		~Member()	
		{
			FreePtrVector<LodSkinSubMesh> freeSubMeshs;
			freeSubMeshs(m_subMeshs);
			FreePtrVector<RSkin>	  freeRenderNodes;
			freeRenderNodes(m_renderNodes);
			SAFE_DELETE(m_pStaticVB);
		}
	
	public:
		void LoadFromFile(IFS *pFS,const TCHAR* szName)
		{
			DWORD hFile=0;
			while(true)
			{
				hFile=pFS->Open(szName);
				if(hFile==0
					||hFile==-1)
				{
					TCHAR szError[256];
					errno_t err;
					_get_errno(&err);
					_stprintf(szError,_T("skin file open failed ErrorNo:%d"),err);
					if(::MessageBox(NULL,szName,szError,MB_RETRYCANCEL|MB_ICONWARNING)==IDCANCEL)
						THROW_NULLEX(NULL,_T("skin file open failed."),szName);
				}
				else
					break;
			}

			tagFSkinHeader header;
			pFS->Read(&header,sizeof(tagFSkinHeader),hFile);

			size_t i;
			//--read vertices
			pFS->Seek(hFile,header.vertOffset,FILE_BEGIN);
			m_vertices.resize(header.numVert);
			for(i=0;i<header.numVert;i++)
				m_vertices[i].ReadFile(pFS,hFile);

			//--read sub meshs
			pFS->Seek(hFile,header.subMeshOffset,FILE_BEGIN);
			for(i=0;i<header.numSubMesh;i++)
			{
				LodSkinSubMesh *pMesh=new LodSkinSubMesh;
				pMesh->ReadFile(pFS,hFile);
				m_subMeshs.push_back(pMesh);
			}

			//--read mtls
			pFS->Seek(hFile,header.mtlOffset,FILE_BEGIN);
			for(i=0;i<header.numMtl;i++)
			{
				tagMtlDesc md;
				pFS->Read(&md,sizeof(tagMtlDesc),hFile);
				m_mtlDescs.push_back(md);
			}

			//--close file
			pFS->Close(hFile);

			m_box.max=header.max;
			m_box.min=header.min;
		}

		bool HaveBumpMap() const
		{
			for(int i=0;i<(int)m_mtlDescs.size();i++)
			{
				if(_tcslen(m_mtlDescs[i].m_map[EMap_Bump])>0)
					return true;
			}
			return false;
		}

		void CreateStaticVB()
		{
			SAFE_DELETE(m_pStaticVB);
			if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_HWSkin)==Cool3DOption::EHWS_Open)
			{
				VertStream vertStream;
				if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_NormalMap)==Cool3DOption::ENM_Open
					&& HaveBumpMap())
				{
					m_staticVertType=EVType_PNGWIT;
					vertStream.Create(m_staticVertType,m_vertices.size());
					Vert_PNGWIT* pVert=(Vert_PNGWIT*)vertStream.GetRawBuffer();
					for(int i=0;i<(int)m_vertices.size();i++)
					{
						const SkinVert& vert=m_vertices[i];
						pVert->pos=vert.pos;
						pVert->normal=vert.normal;
						pVert->tangent=vert.tangent;
						pVert->blendWeight=vert.blendWeight;
						pVert->blendIndices=vert.blendIndices;
						pVert->uv=vert.uv;
						pVert++;
					}
				}
				else
				{
					m_staticVertType=EVType_PNWIT;
					vertStream.Create(m_staticVertType,m_vertices.size());
					Vert_PNWIT* pVert=(Vert_PNWIT*)vertStream.GetRawBuffer();
					for(int i=0;i<(int)m_vertices.size();i++)
					{
						const SkinVert& vert=m_vertices[i];
						pVert->pos=vert.pos;
						pVert->normal=vert.normal;
						pVert->blendWeight=vert.blendWeight;
						pVert->blendIndices=vert.blendIndices;
						pVert->uv=vert.uv;
						pVert++;
					}
				}

				SAFE_DELETE(m_pStaticVB);
				m_pStaticVB=Device()->NewVB();
				m_pStaticVB->Create(vertStream.GetBufferSize(),EB_StaticWriteOnly,vertStream.GetStride());
				m_pStaticVB->CopyFromVertStream(&vertStream);
			}
		}

		void CreateRenderNodes(ResSkin *pFather)
		{
			for(size_t i=0;i<m_subMeshs.size();i++)
			{
				RSkin *pNode=new RSkin;
				pNode->Create(m_subMeshs[i],m_pStaticVB,m_staticVertType,i);
				m_renderNodes.push_back(pNode);
			}
		}
	};
	//--class ResSkin--------------------------------------------
	IMP_RTTI(ResSkin,ResRenderable);

	ResSkin::ResSkin(const TCHAR* szName) : ResRenderable(szName)
	{
		m_p=new Member;
	}

	ResSkin::~ResSkin(void)
	{
		delete m_p;
	}

	UINT ResSkin::CreateContent(DWORD param)
	{
		m_p->LoadFromFile(GetSafeFS(),GetName());
		m_p->CreateStaticVB();
		m_p->CreateRenderNodes(this);

		return ResRenderable::CreateContent(param);
	}

	UINT ResSkin::GetNumRenderNode() const
	{
		return m_p->m_renderNodes.size();
	}

	RenderNode *ResSkin::GetRenderNode(UINT i) const
	{
		ASSERT(i<GetNumRenderNode());
		return m_p->m_renderNodes[i];
	}

	const AABBox	*ResSkin::GetAABBox() const
	{
		return &m_p->m_box;
	}

	int ResSkin::GetNumMtl() const
	{
		return m_p->m_mtlDescs.size();
	}

	void ResSkin::CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor) const
	{
		mtls.clear();
		for(size_t i=0;i<m_p->m_mtlDescs.size();i++)
		{
			const tagMtlDesc& mtlDesc=m_p->m_mtlDescs[i];
			MSkin *pNewMtl=new MSkin;
			pNewMtl->CreateFromMaxExport(mtlDesc,0,NULL,mtlDiffuseFactor);
			pNewMtl->LoadShaders();
			mtls.push_back(pNewMtl);
		}
	}

	const vector<SkinVert>&	ResSkin::GetSkinVerts() const
	{
		return m_p->m_vertices;
	}

	const vector<LodSkinSubMesh*>& ResSkin::GetLodSubMesh() const
	{
		return m_p->m_subMeshs;
	}

	const vector<tagBoneOffsetMatrix>& ResSkin::GetBoneOffsetMatrix(int subMeshID) const
	{
		ASSERT(subMeshID>=0 && subMeshID<(int)m_p->m_subMeshs.size());
		LodSkinSubMesh* pSubMesh=m_p->m_subMeshs[subMeshID];
		return pSubMesh->baseBoneOM;
	}
	
	TResult ResSkin::RayCollide(const Ray& ray,const Matrix4* pObj2World,const VertStream* pVS)
	{
		if(!IsCreated())
			return TResult(false,0);

		if( !pVS || pVS->GetNumVert()!=m_p->m_vertices.size() )
			return TResult(false,0);

		//--把World空间的ray变换到object空间
		Ray localRay;
		Matrix4 world2Obj;
		D3DXMatrixInverse(&world2Obj,NULL,pObj2World);
		D3DXVec3TransformCoord(&localRay.origin,&ray.origin,&world2Obj);
		D3DXVec3TransformNormal(&localRay.dir,&ray.dir,&world2Obj);

		//检测节点内的每个三角形
		for( size_t n=0; n<m_p->m_subMeshs.size(); n++ )
		{
			FaceBuffer& faces = m_p->m_subMeshs[n]->baseFaces;
			int numFaces=(int)faces.size();
			if(numFaces>0)
			{
				ASSERT(pVS->GetType()==EVType_PNT);
				Vert_PNT *pVert=(Vert_PNT *)pVS->GetRawBuffer();
				for(int i=0;i<numFaces;i++)
				{
					TriFace& face=faces[i];
					TResult tr=localRay.IntersectTriangle(pVert[face.index[0]].pos,
						pVert[face.index[1]].pos,
						pVert[face.index[2]].pos);
					if(tr.first)
						return tr;
				}
			}
		}

		return TResult(false,0);
	}

	const tagMtlDesc& ResSkin::GetMtlDesc( const int i ) const
	{
		ASSERT( i >= 0 && i < (int)m_p->m_mtlDescs.size() );
		return m_p->m_mtlDescs[i];
	}

}//namespace Cool3D