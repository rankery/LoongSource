#include "stdafx.h"
#include "ResCloth.h"
#include "..\RenderSys\RCloth.h"
#include "..\RenderSys\RSkin.h"
#include "..\Util\Exception.h"
#include "..\Util\StlExt.h"
#include "..\Public\MtlDesc.h"
#include "..\Math\GeomDefine.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\Cool3DOption.h"

namespace Cool3D
{
	class ResCloth::Member
	{
	public:
		vector<SkinVert>			m_vertices;	//cloth顶点数组
		vector<LodClothSubMesh*>	m_subMeshs;	//face mesh数组
		vector<RenderNode*>			m_skinRenderNodes;
		vector<RenderNode*>			m_clothRenderNodes;
		vector<tagMtlDesc>			m_mtlDescs;
		AABBox						m_box;
		IVertBuffer*				m_pStaticVB;
		vector<DWORD>				m_clothArray;
		vector<DWORD>				m_tagArray;
		vector<tagSpring>			m_springArray;
		vector<tagBlock>			m_blockArray;
		EVertType					m_staticVertType;

	public:
		Member() : m_pStaticVB(0)
		{}

		~Member()	
		{
			FreePtrVector<LodClothSubMesh> freeSubMeshs;
			freeSubMeshs(m_subMeshs);
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
					if(::MessageBox(NULL,szName,_T("cloth file open failed"),MB_RETRYCANCEL|MB_ICONWARNING)==IDCANCEL)
						THROW_NULLEX(NULL,_T("cloth file open failed."),szName);
				}
				else
					break;
			}

			tagFClothHeader header;
			pFS->Read(&header,sizeof(tagFClothHeader),hFile);

			size_t i;
			// read vertices
			pFS->Seek(hFile,header.vertOffset,FILE_BEGIN);
			m_vertices.resize(header.numVert);
			for(i=0;i<header.numVert;i++)
				m_vertices[i].ReadFile(pFS,hFile);

			// read sub meshes
			pFS->Seek(hFile,header.subMeshOffset,FILE_BEGIN);
			for(i=0;i<header.numSubMesh;i++)
			{
				LodClothSubMesh *pMesh=new LodClothSubMesh;
				pMesh->ReadFile(pFS,hFile);
				m_subMeshs.push_back(pMesh);
			}

			// read mtls
			pFS->Seek(hFile,header.mtlOffset,FILE_BEGIN);
			for(i=0;i<header.numMtl;i++)
			{
				tagMtlDesc md;
				pFS->Read(&md,sizeof(tagMtlDesc),hFile);
				m_mtlDescs.push_back(md);
			}

			// read cloth
			pFS->Seek(hFile,header.clothOffset,FILE_BEGIN);
			FReadValVector(pFS,hFile,m_clothArray);

			// read tag
			pFS->Seek(hFile,header.tagOffset,FILE_BEGIN);
			FReadValVector(pFS,hFile,m_tagArray);

			// read spring
			pFS->Seek(hFile,header.springOffset,FILE_BEGIN);
			FReadValVector(pFS,hFile,m_springArray);

			// read block
			pFS->Seek(hFile,header.blockOffset,FILE_BEGIN);
			for( i=0; i<header.numBlock; i++ )
			{
				tagBlock block;
				block.v0.ReadFile(pFS,hFile);
				block.v1.ReadFile(pFS,hFile);
				block.v2.ReadFile(pFS,hFile);
				m_blockArray.push_back(block);
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

				m_pStaticVB=Device()->NewVB();
				m_pStaticVB->Create(vertStream.GetBufferSize(),EB_StaticWriteOnly,vertStream.GetStride());
				m_pStaticVB->CopyFromVertStream(&vertStream);
			}
		}

		void CreateRenderNodes(ResCloth *pFather)
		{
			for(size_t i=0;i<m_subMeshs.size();i++)
			{
				LodClothSubMesh *pMesh = m_subMeshs[i];

				if( pMesh->m_isSkin )
				{
					RSkin *pNode=new RSkin;
					pNode->Create(m_subMeshs[i],m_pStaticVB,m_staticVertType,i);
					m_skinRenderNodes.push_back(pNode);
				}
				else
				{
					RCloth *pNode=new RCloth;
					pNode->Create(m_subMeshs[i],EVType_PNT,i);
					m_clothRenderNodes.push_back(pNode);
				}
			}
		}
	};

	//--class ResCloth--------------------------------------------
	IMP_RTTI(ResCloth,ResRenderable);

	ResCloth::ResCloth(const TCHAR* szName) : ResRenderable(szName)
	{
		m_p=new Member;
	}

	ResCloth::~ResCloth(void)
	{
		delete m_p;
	}

	UINT ResCloth::CreateContent(DWORD param)
	{
		m_p->LoadFromFile(GetSafeFS(),GetName());
		m_p->CreateStaticVB();
		m_p->CreateRenderNodes(this);

		return ResRenderable::CreateContent(param);
	}

	UINT ResCloth::GetNumRenderNode() const
	{
		return (m_p->m_skinRenderNodes.size() + m_p->m_clothRenderNodes.size() );
	}

	RenderNode *ResCloth::GetRenderNode(UINT i) const
	{
		ASSERT(i<GetNumRenderNode());

		if( i < m_p->m_skinRenderNodes.size() )
		{
			return m_p->m_skinRenderNodes[i];
		}
		else
		{
			return m_p->m_clothRenderNodes[i-m_p->m_skinRenderNodes.size()];
		}
	}

	const AABBox *ResCloth::GetAABBox() const
	{
		return &m_p->m_box;
	}

	int ResCloth::GetNumMtl() const
	{
		return m_p->m_mtlDescs.size();
	}

	void ResCloth::CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor) const
	{
		mtls.clear();
		for(size_t i=0;i<m_p->m_mtlDescs.size();i++)
		{
			const tagMtlDesc& mtlDesc=m_p->m_mtlDescs[i];
			MGeneral *pNewMtl=new MGeneral;
			pNewMtl->CreateFromMaxExport(mtlDesc,0,NULL,mtlDiffuseFactor);
			mtls.push_back(pNewMtl);
		}
	}

	const vector<SkinVert>& ResCloth::GetClothVerts() const
	{
		return m_p->m_vertices;
	}

	const vector<LodClothSubMesh*>& ResCloth::GetLodSubMesh() const
	{
		return m_p->m_subMeshs;
	}

	const vector<tagBoneOffsetMatrix>& ResCloth::GetBoneOffsetMatrix(int subMeshID) const
	{
		ASSERT(subMeshID>=0 && subMeshID<(int)m_p->m_subMeshs.size());
		LodSkinSubMesh* pSubMesh=m_p->m_subMeshs[subMeshID];
		return pSubMesh->baseBoneOM;
	}

	TResult ResCloth::RayCollide(const Ray& ray,const Matrix4* pObj2World,const VertStream* pVS)
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

	const vector<DWORD>& ResCloth::GetClothIndex() const
	{
		return m_p->m_clothArray;
	}

	const vector<DWORD>& ResCloth::GetTagIndex() const
	{
		return m_p->m_tagArray;
	}

	vector<tagSpring>& ResCloth::GetSprings()
	{
		return m_p->m_springArray;
	}

	vector<tagBlock>& ResCloth::GetBlocks()
	{
		return m_p->m_blockArray;
	}

	UINT ResCloth::GetNumSkinRenderNode() const
	{
		return m_p->m_skinRenderNodes.size();
	}

	RenderNode* ResCloth::GetSkinRenderNode( UINT i ) const
	{
		return m_p->m_skinRenderNodes[i];
	}
}//namespace Cool3D