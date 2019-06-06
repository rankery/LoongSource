#include "StdAfx.h"
#define VFS_READ
#include ".\resstaticmesh.h"
#include "..\Math\GeomDefine.h"
#include "..\Util\Exception.h"
#include "..\RenderSys\RenderNode.h"
#include "..\RenderSys\RStatic.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Device\IVertBuffer.h"
#include "VertStream.h"

#include "..\Public\StaticMeshFile.h"
#include "..\Public\TagNodeDefine.h"

#include "..\Util\FMemory.h"
#include "..\Device\DrawX.h"

#include "..\Public\SubMesh.h"

#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\MStatic.h"

namespace Cool3D
{
	IMP_RTTI(ResStaticMesh,ResRenderable);

	class ResStaticMesh::Member
	{
	public:
		/** 从文件名为ResBase::m_szName的文件中读取
		*/
		void LoadFromFile(IFS *pFS,const TCHAR* szName);
		/**	为每个SubMesh创建一个RenderNode
		*/
		void CreateRenderNodes()
		{
			SharedVB pVB(Device()->NewVB());
			pVB->Create(m_pVertStream->GetBufferSize(),EB_StaticWriteOnly,m_pVertStream->GetStride());
			pVB->CopyFromVertStream(m_pVertStream);

			for(size_t i=0;i<m_subMeshs.size();i++)
			{
				const LodSubMesh& m=m_subMeshs[i];
				RStatic *pNode=new RStatic;
				pNode->Create(pVB,m_pVertStream->GetType(),&m);
				m_renderArray.push_back(pNode);
			}
		}
		
		UINT GetVertBufSize()
		{ 
			return m_pVertStream->GetBufferSize();
		}
		UINT GetIndexBufSize()
		{
			UINT ret=0;
			for(size_t i=0;i<m_subMeshs.size();i++)
			{
				const LodSubMesh& m=m_subMeshs[i];
				ret+=m.baseFaces.size()*3*sizeof(WORD);
			}
			return ret;
		}

		Member()
			: m_pVertStream(0)
			, m_lightmapSize(128)
		{
		}
		~Member()
		{
			SAFE_DELETE(m_pVertStream);

			for(size_t i=0;i<m_renderArray.size();i++)
			{
				RStatic *pNode=m_renderArray[i];
				delete pNode;
			}
			m_renderArray.clear();
		}

	public:
		VertStream			*m_pVertStream;
		vector<tagMtlDesc>	m_mtlDescArray;//subMesh中的tagMtlDesc指针指向这个数组的成员
		vector<LodSubMesh>	m_subMeshs;
		TagNodeFrame		m_tagNodes;

		vector<RStatic*>	m_renderArray;
		AABBox				m_aabBox;
		int					m_lightmapSize;

		vector<AABBox>		m_octreeBoxes;
		vector<WORD>		m_indexes;
	};

	ResStaticMesh::ResStaticMesh(const TCHAR* szName):ResRenderable(szName)
	{
		m_p=new Member;
	}

	ResStaticMesh::~ResStaticMesh(void)
	{
		SAFE_DELETE(m_p);
	}

	
	const AABBox	*ResStaticMesh::GetAABBox() const
	{
		return &m_p->m_aabBox;
	}

	UINT ResStaticMesh::GetNumRenderNode() const
	{
		if(!IsCreated())
			return 0;

		return m_p->m_renderArray.size();
	}

	RenderNode *ResStaticMesh::GetRenderNode(UINT i) const
	{
		if(!IsCreated())
			return NULL;
		ASSERT(i<GetNumRenderNode());
		return m_p->m_renderArray[i];
	}

	int ResStaticMesh::GetNumMtl() const
	{
		ASSERT(IsCreated());
		return m_p->m_mtlDescArray.size();
	}

	void ResStaticMesh::CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor) const
	{
		ASSERT(IsCreated());
		int numMipMap=0;
		if(!m_bMipMap)
			numMipMap=1;

		mtls.clear();
		for(size_t i=0;i<m_p->m_mtlDescArray.size();i++)
		{
			const tagMtlDesc& mtlDesc=m_p->m_mtlDescArray[i];
			MStatic *pNewMtl=new MStatic;
			pNewMtl->CreateFromMaxExport(mtlDesc,numMipMap,szLightMap,mtlDiffuseFactor);
			pNewMtl->LoadShaders();
			mtls.push_back(pNewMtl);
		}
	}

	UINT ResStaticMesh::CreateContent(DWORD param)
	{
		m_p->LoadFromFile(GetSafeFS(),GetName());
		m_p->CreateRenderNodes();

		return ResRenderable::CreateContent(param);
	}

	namespace
	{
		void ReadBintree(IFS *pFS,DWORD hFile,vector<WORD>& bintreeBoxes)
		{
			AABBox box;
			FReadValue(pFS,hFile,box.min);
			FReadValue(pFS,hFile,box.max);
			vector<WORD> content;
			FReadValVector(pFS,hFile,content);
			for(UINT i=0;i<content.size();i++)
			{
				bintreeBoxes.push_back(content[i]);
			}
			for(int i=0;i<8;i++)
			{
				bool bChild=false;
				FReadValue(pFS,hFile,bChild);
				if(bChild)
					ReadBintree(pFS,hFile,bintreeBoxes);
			}
		}
	}

	void ResStaticMesh::Member::LoadFromFile(IFS *pFS,const TCHAR* szName)
	{
		ASSERT(pFS!=NULL);
		ASSERT(szName!=NULL);
		ASSERT(_tcslen(szName) > 0);

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
				_stprintf(szError,_T("ResStaticMesh open failed ErrorNo:%d"),err);
				if(::MessageBox(NULL,szName,szError,MB_RETRYCANCEL|MB_ICONWARNING)==IDCANCEL)
					THROW_NULLEX(NULL,_T("ResStaticMesh open failed."),szName);
			}
			else
				break;
		}

		StaticMeshHeader header;
		pFS->Read(&header,sizeof(StaticMeshHeader),hFile);
		ASSERT(_tcscmp(header.magic,StaticMesh_Magic) == 0);

		m_aabBox.max=header.max;
		m_aabBox.min=header.min;
		m_lightmapSize=header.lightmapSize;

		//--vert buffer
		ASSERT(m_pVertStream == NULL);
		m_pVertStream = new VertStream;
		pFS->Seek(hFile,header.offsetVert,FILE_BEGIN);
		m_pVertStream->CreateFromFile(pFS,hFile,header.vertType,header.numVert);

		//--sub meshs
		UINT i;
		pFS->Seek(hFile,header.offsetMesh,FILE_BEGIN);
		for(i=0;i<header.numMesh;i++)
		{
			LodSubMesh subMesh;
			subMesh.ReadFile(pFS,hFile);
			m_subMeshs.push_back(subMesh);
			
		}

		//--tagMtlDesc Array
		pFS->Seek(hFile,header.offsetMtl,FILE_BEGIN);
		for(i=0;i<header.numMtl;i++)
		{
			tagMtlDesc mtlDesc;
			pFS->Read(&mtlDesc,sizeof(tagMtlDesc),hFile);
			
			m_mtlDescArray.push_back(mtlDesc);
		}

		//--tag nodes
		pFS->Seek(hFile,header.offsetTagNode,FILE_BEGIN);
		m_tagNodes.ReadFile(pFS,hFile);

		pFS->Close(hFile);

		//todo leili 临时，为了在3DView里显示八叉碰撞树盒子
#if 0
		TCHAR szDrive[512];
		TCHAR szDir[512];
		TCHAR szFilePath[512];
		TCHAR szNavPath[512];
		_tsplitpath(szName,szDrive,szDir,szFilePath,NULL);
		_stprintf(szNavPath,_T("%s%s%s.Nav"),szDrive,szDir,szFilePath);
		hFile=pFS->Open(szNavPath);
		THROW_NULLEX(hFile,_T("Nav open file failed."),szName);
		SceneMeshNavHeader navHeader;
		FReadValue(pFS,hFile,navHeader);
		pFS->Seek(hFile,navHeader.offsetOctree,FILE_BEGIN);
		FReadValVector(pFS,hFile,m_octreeBoxes);
		ReadBintree(pFS,hFile,m_indexes);
		pFS->Close(hFile);
#endif
	}

	void ResStaticMesh::DrawOctree()
	{
		for(UINT i=0;i<m_p->m_octreeBoxes.size();i++)
			DrawX::DrawAABBox(m_p->m_octreeBoxes[i]);
	}

	void ResStaticMesh::SaveTagNodes()
	{
		FILE* pFile = _tfopen( GetName(), _T("r+b") );
		THROW_NULLEX(pFile,_T("ResStaticMesh open file failed."),GetName());

		StaticMeshHeader header;
		fread( &header,sizeof(StaticMeshHeader),1,pFile );
		ASSERT(_tcscmp(header.magic,StaticMesh_Magic) == 0);

		fseek(pFile,header.offsetTagNode,SEEK_SET);
		m_p->m_tagNodes.WriteFile(pFile);

		fclose(pFile);
	}

	const TagNodeFrame& ResStaticMesh::GetTagNodes() const
	{
		return m_p->m_tagNodes;
	}
    
	TResult ResStaticMesh::RayCollide(const Ray& ray,const Matrix4* pObj2World)
	{
		if(!IsCreated())
			return TResult(false,0);

		ASSERT(m_p->m_pVertStream);

		//--把World空间的ray变换到object空间
		Ray localRay;
		Matrix4 world2Obj;
		D3DXMatrixInverse(&world2Obj,NULL,pObj2World);
		D3DXVec3TransformCoord(&localRay.origin,&ray.origin,&world2Obj);
		D3DXVec3TransformNormal(&localRay.dir,&ray.dir,&world2Obj);

		//检测节点内的每个三角形
		for( size_t n=0; n<m_p->m_subMeshs.size(); n++ )
		{
			FaceBuffer& faces = m_p->m_subMeshs[n].baseFaces;
			int numFaces=(int)faces.size();
			if(numFaces>0)
			{
				ASSERT(m_p->m_pVertStream->GetType()==EVType_PNT);
				Vert_PNT *pVert=(Vert_PNT *)m_p->m_pVertStream->GetRawBuffer();
				for(int i=0;i<numFaces;i++)
				{
					TriFace& face=faces[i];
					TResult tr=localRay.IntersectTriangle(pVert[face.index[0]].pos,
						pVert[face.index[1]].pos,
						pVert[face.index[2]].pos);
					if(tr.first)
						return tr;
				}
			}//endof if
		}

		return TResult(false,0);
	}

	VertStream* ResStaticMesh::GetVertStream()
	{
		return m_p->m_pVertStream;
	}

	void ResStaticMesh::GetIndexBuffer(vector<WORD>& ib)
	{
		ASSERT(m_p->m_subMeshs.size()>0);
		ib.clear();
		for(int i=0;i<(int)m_p->m_subMeshs.size();i++)
		{
			LodSubMesh& mesh=m_p->m_subMeshs[i];
			FaceBuffer& face=mesh.baseFaces;
			for(int j=0;j<(int)face.size();j++)
			{
				ib.push_back(face[j].index[0]);
				ib.push_back(face[j].index[1]);
				ib.push_back(face[j].index[2]);
			}
		}
	}

	int ResStaticMesh::GetLightMapSize( void ) const
	{
		return m_p->m_lightmapSize;
	}

	bool ResStaticMesh::SetLightMapSize( int size )
	{
		FILE* fp=_tfopen(GetName(),_T("r+b"));
		if(fp == NULL)
			return false;
		StaticMeshHeader header;
		fread(&header,sizeof(StaticMeshHeader),1,fp);
		header.lightmapSize=size;
		fseek(fp,0,SEEK_SET);
		fwrite(&header,sizeof(header),1,fp);
		fclose(fp);

		return true;
	}

	const tagMtlDesc& ResStaticMesh::GetMtlDesc( const int i ) const
	{
		ASSERT( i >= 0 && i < (int)m_p->m_mtlDescArray.size() );
		return m_p->m_mtlDescArray[i];
	}
}//namespace Cool3D