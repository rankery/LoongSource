#include "StdAfx.h"
#include ".\resscenemesh.h"
#include "AABBTreeCollider.h"
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
#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\MStatic.h"

#include "..\Util\FMemory.h"
#include "..\Device\DrawX.h"

#define VFS_READ
#include "..\Public\SubMesh.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Util\SpeedAnalyser.h"

namespace Cool3D
{
	IMP_RTTI(ResSceneMesh,ResRenderable);

	class ResSceneMesh::Member
	{
	public:
		VertStream			*m_pVertStream;
		vector<tagMtlDesc>	m_mtlDescArray;//subMesh中的tagMtlDesc指针指向这个数组的成员
		vector<LodSubMesh>	m_subMeshs;
		TagNodeFrame		m_tagNodes;

		vector<RStatic*>	m_renderArray;
		AABBox				m_aabBox;
		AABBTreeCollider	m_cdTree;
		vector<AABBox>		m_octreeBoxes;
		vector<WORD>		m_indexes;
		int					m_lightmapSize;
	public:
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
	};

	ResSceneMesh::ResSceneMesh(const TCHAR* szName) : ResRenderable(szName)
	{
		m_p=new Member;
	}

	ResSceneMesh::~ResSceneMesh(void)
	{
		delete m_p;
	}

	UINT ResSceneMesh::CreateContent(DWORD param)
	{
		m_p->LoadFromFile(GetSafeFS(),GetName());
		m_p->CreateRenderNodes();

		return ResRenderable::CreateContent(param);
	}

	UINT ResSceneMesh::GetNumRenderNode() const
	{
		if(!IsCreated())
			return 0;

		return m_p->m_renderArray.size();
	}

	RenderNode *ResSceneMesh::GetRenderNode(UINT i) const
	{
		if(!IsCreated())
			return NULL;
		ASSERT(i<GetNumRenderNode());
		return m_p->m_renderArray[i];
	}

	const AABBox	*ResSceneMesh::GetAABBox() const
	{
		return &m_p->m_aabBox;
	}

	int ResSceneMesh::GetNumMtl() const
	{
		ASSERT(IsCreated());
		return m_p->m_mtlDescArray.size();
	}

	void ResSceneMesh::CreateMtlArray(vector<IMaterial*>& mtls,const TCHAR* szLightMap,float mtlDiffuseFactor ) const
	{
		ASSERT(IsCreated());

		mtls.clear();
		for(size_t i=0;i<m_p->m_mtlDescArray.size();i++)
		{
			const tagMtlDesc& mtlDesc=m_p->m_mtlDescArray[i];
			MStatic *pNewMtl=new MStatic;
			pNewMtl->CreateFromMaxExport(mtlDesc,0,szLightMap,mtlDiffuseFactor);
			pNewMtl->LoadShaders();
			mtls.push_back(pNewMtl);
		}
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

	void ResSceneMesh::Member::LoadFromFile(IFS *pFS,const TCHAR* szName)
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
				_stprintf(szError,_T("ResSceneMesh open failed ErrorNo:%d"),err);
				if(::MessageBox(NULL,szName,szError,MB_RETRYCANCEL|MB_ICONWARNING)==IDCANCEL)
					THROW_NULLEX(NULL,_T("ResSceneMesh open failed."),szName);
			}
			else
				break;
		}

		SceneMeshHeader header;
		pFS->Read(&header,sizeof(SceneMeshHeader),hFile);
		ASSERT(_tcscmp(header.magic,SceneMesh_Magic) == 0);

		m_aabBox.max=header.max;
		m_aabBox.min=header.min;

		m_lightmapSize=header.lightmapSize;

		//--vert buffer
		ASSERT(m_pVertStream==NULL);
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

		
		pFS->Seek(hFile,header.boxTreeOffset,FILE_BEGIN);
		m_cdTree.ReadFile(pFS,hFile);

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

	void ResSceneMesh::DrawAABBTree()
	{
		m_p->m_cdTree.Draw();
	}

	void ResSceneMesh::DrawOctree()
	{
		for(UINT i=0;i<m_p->m_octreeBoxes.size();i++)
			DrawX::DrawAABBox(m_p->m_octreeBoxes[i]);
	}

	//利用AABB树进行线段与物体的碰撞检测
	bool ResSceneMesh::TestLineCollision(Vector3 vPoint1,Vector3 vPoint2,Matrix4 WorldMat)
	{
		return m_p->m_cdTree.LineCollide(vPoint1,vPoint2,WorldMat);
	}

	TResult ResSceneMesh::RayCollide(const Ray& ray,const Matrix4 *pObj2World)
	{
		return m_p->m_cdTree.RayCollide(ray,pObj2World,m_p->m_pVertStream);
	}

	VertStream* ResSceneMesh::GetVertStream()
	{
		return m_p->m_pVertStream;
	}

	void ResSceneMesh::GetIndexBuffer(vector<WORD>& ib)
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

	void ResSceneMesh::BuildDecalMesh( const AABBox& boxLocal,DynamicMesh *pMesh, const Matrix4& matWorld, const Vector3* pNormal )
	{
		if( !IsCreated() )
			return;

		BuildDecalMeshByAABBTree( m_p->m_cdTree.GetNode( 0 ), boxLocal, pMesh, matWorld, pNormal );
	}

	void ResSceneMesh::BuildDecalMeshByAABBTree( AABBTreeNode* pNode, const AABBox& boxLocal,DynamicMesh *pMesh, const Matrix4& matWorld, const Vector3* pNormalLocal )
	{
		if( NULL == pNode )
			return;

		AABBox boxNode;
		boxNode.min = pNode->m_boxMin;
		boxNode.max = pNode->m_boxMax;

		if( !boxNode.Intersects( boxLocal ) )
			return;

		//BeginSpeedTest(_T("ResSceneMesh::BuildDecalMeshByAABBTree.ClipMesh"));

		int numFaces = (int)pNode->m_faces.size();
		if( numFaces > 0 )
		{
			Vector3 Tri[3];
			Vector3 newVertex[9];
			Vector3 newVertexWorld[9];
			Vector3 normal( 0.0f, 1.0f, 0.0f );
			for(int i = 0; i < numFaces; i++)
			{
				TriFace& face=pNode->m_faces[i];
				for( int j = 0; j < 3; j++ )
				{
					if( !m_p->m_pVertStream->GetPos( face.index[j], Tri[j] ) )
						return;
				}

				// 背面剔除
				if( NULL != pNormalLocal )
				{
					D3DXVec3Cross( &normal, &(Tri[2] - Tri[1]), &(Tri[1] - Tri[0]) );
					if( D3DXVec3Dot( pNormalLocal, &normal) < 0.0f )
						continue;
				}

				int vertCount = boxLocal.ClipTriangle( Tri, newVertex );
				for( int nVert = 0; nVert < vertCount; nVert++ )
				{
					newVertexWorld[nVert] = newVertex[nVert] * matWorld;
				}
				for( int nVert = 1; nVert < vertCount - 1; nVert++ )
				{
					pMesh->AddPos( newVertexWorld[0] );
					pMesh->AddPos( newVertexWorld[nVert] );
					pMesh->AddPos( newVertexWorld[nVert+1] );
				}
			}
		}

		//EndSpeedTest(_T("ResSceneMesh::BuildDecalMeshByAABBTree.ClipMesh"));

		for( int nChild = 0; nChild < 2; nChild++ )
		{
			if( -1 != pNode->m_childrenID[nChild] )
			{
				BuildDecalMeshByAABBTree( m_p->m_cdTree.GetNode( pNode->m_childrenID[nChild] ), boxLocal, pMesh, matWorld, pNormalLocal );
			}
		}

	}

	int ResSceneMesh::GetLightMapSize( void ) const
	{
		return m_p->m_lightmapSize;
	}

	bool ResSceneMesh::SetLightMapSize( int size )
	{
		FILE* fp=_tfopen(GetName(),_T("r+b"));
		if(fp == NULL)
			return false;
		SceneMeshHeader header;
		fread(&header,sizeof(SceneMeshHeader),1,fp);
		header.lightmapSize=size;
		fseek(fp,0,SEEK_SET);
		fwrite(&header,sizeof(header),1,fp);
		fclose(fp);

		return true;
	}
}//namespace Cool3D