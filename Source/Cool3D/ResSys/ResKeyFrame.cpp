#include "StdAfx.h"
#include "Reskeyframe.h"
#include "..\VFS\IFS.h"
#include "..\Util\Exception.h"
#include "VertStream.h"
#include "..\Public\AKFile.h"
#include "..\RenderSys\RKeyFrame.h"
#include "..\Util\StlExt.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\MKeyFrame.h"
#include "AniMsgTable.h"
#include "..\Util\FileName.h"
#include "..\Animation\KeyFrame.h"
#include "..\Public\StaticMeshFile.h"
#include "..\Device\DrawX.h"

#define VFS_READ
#include "..\Public\SubMesh.h"

namespace Cool3D
{
	IMP_RTTI(ResKeyFrame,ResRenderable);

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

	class ResKeyFrame::Member
	{
	public:
		vector<RenderNode*>		renderNodes;
		AABBox					box;
		vector<LodSubMesh*>		subMeshs;
		vector<tagMtlDesc>		mtlDescArray;		//subMesh中的tagMtlDesc指针指向这个数组的成员
		vector<VertexKeyFrame*> frameArray;
		vector<TagNodeFrame>	tagNodeFrameArray;	//与frameArray一一对应
		AniMsgTable				msgTable;

		map<DWORD,TrackInfo>	m_mapTrackInfo;		//动画map
		vector<float>			m_vecFrameTime;		//每帧时间

		vector<AABBox>			m_octreeBoxes;
		vector<WORD>			m_indexes;

		int						m_lightmapSize;

	public:
		Member(void)
			: m_lightmapSize(0)
		{}

		~Member(void)
		{
			FreePtrVector<RenderNode> freeRenderNodes;
			freeRenderNodes(renderNodes);
			FreePtrVector<LodSubMesh> freeSubMeshs;
			freeSubMeshs(subMeshs);

			for(size_t i=0;i<frameArray.size();i++)
			{
				VertexKeyFrame *pFrm=frameArray[i];
				SAFE_DELETE(pFrm);
			}
			frameArray.clear();
		}

		UINT LoadFromFile(IFS *pFS,const TCHAR* szName)
		{
			ASSERT(pFS!=NULL);
			AKHeader header;

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
					_stprintf(szError,_T("KeyFrame file open failed ErrorNo:%d"),err);
					if(::MessageBox(NULL,szName,szError,MB_RETRYCANCEL|MB_ICONWARNING)==IDCANCEL)
						THROW_NULLEX(NULL,_T("KeyFrame file open failed."),szName);
				}
				else
					break;
			}


			UINT ResSize=0;
			UINT ResBegin,ResEnd;
			pFS->Read(&header,sizeof(header),hFile);

			ASSERT(_tcscmp(header.magic,AK_MAGIC) == 0);
			box.max=header.max;
			box.min=header.min;
			m_lightmapSize=header.lightmapSize;

			UINT i;
			//--read sub meshes
			pFS->Seek(hFile,header.offsetMesh,FILE_BEGIN);
			ResBegin=pFS->Tell(hFile);
			for(i=0;i<header.numMesh;i++)
			{
				LodSubMesh *subMesh=new LodSubMesh;
				subMesh->ReadFile(pFS,hFile);
				subMeshs.push_back(subMesh);
			}
			ResEnd=pFS->Tell(hFile);
			ResSize+=ResEnd-ResBegin;
			//--tagMtlDesc Array
			pFS->Seek(hFile,header.offsetMtl,FILE_BEGIN);
			ResBegin=pFS->Tell(hFile);
			for(i=0;i<header.numMtl;i++)
			{
				tagMtlDesc mtlDesc;
				pFS->Read(&mtlDesc,sizeof(tagMtlDesc),hFile);
				mtlDescArray.push_back(mtlDesc);
			}
			ResEnd=pFS->Tell(hFile);
			ResSize+=ResEnd-ResBegin;
			//--frames
			pFS->Seek(hFile,header.offsetFrame,FILE_BEGIN);
			TrackInfo info;		
			DWORD dwTrackName = 0;
			std::vector<VertFrame*> frameBuf;

			for(i=0;i<header.numFrame;i++)
			{
				VertFrame *expFrm=new VertFrame;
				const VertFrame *preFrame=NULL;
				if( i>0 )
					preFrame=frameBuf[i-1];
				expFrm->ReadFile(pFS,hFile,preFrame,header.vertType);
				frameBuf.resize(frameBuf.size()+1);
				frameBuf[frameBuf.size()-1]=expFrm;

				//--copy
				VertexKeyFrame *pTempFrm=new VertexKeyFrame();
				pTempFrm->m_name=expFrm->name;
				pTempFrm->m_time=expFrm->time;
				pTempFrm->m_index=i;
				pTempFrm->m_pStream=new VertStream;
				pTempFrm->m_pStream->Create(header.vertType,header.numVert);
				memcpy(pTempFrm->m_pStream->GetRawBuffer(),&(*expFrm->vertices.begin()),pTempFrm->m_pStream->GetBufferSize());
				frameArray.push_back(pTempFrm);
				ResSize+=sizeof(pTempFrm->m_name)+sizeof(pTempFrm->m_time)+pTempFrm->m_pStream->GetBufferSize();

				//--set frame time
				m_vecFrameTime.push_back(pTempFrm->m_time);
				ResSize+=sizeof(pTempFrm->m_time);

				// set track info map
				if( i==0 )
				{
					dwTrackName=pTempFrm->m_name;
					info.fTime=pTempFrm->m_time;
					info.nBegin=0;
				}
				else if( dwTrackName!=pTempFrm->m_name )
				{
					info.nEnd=i-1;
					m_mapTrackInfo[dwTrackName]=info;

					dwTrackName=pTempFrm->m_name;
					info.fTime=pTempFrm->m_time;
					info.nBegin=i;
				}
				else
				{
					info.fTime+=pTempFrm->m_time;
				}
#ifdef _DEBUG
				if(expFrm->tagNodes.nodes.size()>0)
				{
					tagTagNode node=expFrm->tagNodes.nodes[0];
					tstring szTagName=_FourCC2Str(node.name);
				}
#endif
				tagNodeFrameArray.push_back(expFrm->tagNodes);
				ResSize+=expFrm->tagNodes.nodes.size()*sizeof(tagTagNode);
			}//end for

			//释放frameBuf
			std::vector<VertFrame*>::iterator iter=frameBuf.begin();
			for (;iter!=frameBuf.end();++iter)
			{
				delete *iter;
				*iter=NULL;
			}
			frameBuf.clear();
			
			// record the end track info
			info.nEnd = i-1;
			if( dwTrackName != 0 )
				m_mapTrackInfo[ dwTrackName ] = info;
			ResSize+=m_mapTrackInfo.size()*(sizeof(DWORD)+sizeof(TrackInfo));

			pFS->Close(hFile);

			//--Load msg table
			Filename szMsgFileName=szName;
			szMsgFileName=szMsgFileName.NoExtension()+_T(".csv");
			msgTable.LoadFromFile(pFS,szMsgFileName.c_str());
			ResSize+=msgTable.GetMsgMap().size()*(sizeof(DWORD)+sizeof(tagAniMsg));


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

			return ResSize;
		}

		void CreateRenderNodes()
		{
			for(size_t i=0;i<subMeshs.size();i++)
			{
				RKeyFrame *pNewNode=new RKeyFrame;
				LodSubMesh *pSubMesh=subMeshs[i];
				pNewNode->Create(pSubMesh);
				renderNodes.push_back(pNewNode);
			}
		}
	};

	ResKeyFrame::ResKeyFrame(const TCHAR* szName) 
		: ResRenderable(szName)
		, m_bMipMap(true)
	{
		m_p=new Member();
	}

	ResKeyFrame::~ResKeyFrame(void)
	{
		delete m_p;
	}

	UINT ResKeyFrame::CreateContent(DWORD param)
	{
		m_size=m_p->LoadFromFile(GetSafeFS(),GetName());
		m_p->CreateRenderNodes();

		InterlockedIncrement(&m_bCreated);
		return m_size;
	}
	
	UINT ResKeyFrame::GetNumRenderNode() const
	{
		if(!ResBase::IsCreated())
			return 0;
		return m_p->renderNodes.size();
	}

	RenderNode *ResKeyFrame::GetRenderNode(UINT i) const
	{
		if(!ResBase::IsCreated())
			return NULL;
		return m_p->renderNodes[i];
	}

	int ResKeyFrame::GetNumMtl() const
	{
		ASSERT(IsCreated());
		return m_p->mtlDescArray.size();
	}

	void ResKeyFrame::CreateMtlArray(vector<IMaterial*>& mtls,const TCHAR* szLightMap,float mtlDiffuseFactor) const
	{
		ASSERT(IsCreated());
		int numMipMap=0;
		if(!m_bMipMap)
			numMipMap=1;

		mtls.clear();
		for(size_t i=0;i<m_p->mtlDescArray.size();i++)
		{
			const tagMtlDesc& mtlDesc=m_p->mtlDescArray[i];
			MKeyFrame *pNewMtl=new MKeyFrame;
			pNewMtl->CreateFromMaxExport(mtlDesc,numMipMap,szLightMap,mtlDiffuseFactor);
			pNewMtl->LoadShaders();
			mtls.push_back(pNewMtl);
		}
	}

	const AABBox	*ResKeyFrame::GetAABBox() const
	{
		return &m_p->box;
	}

	const vector<VertexKeyFrame*>& ResKeyFrame::GetFrameArray() const
	{
		return m_p->frameArray;
	}

	const vector<TagNodeFrame>& ResKeyFrame::GetTagFrameArray() const
	{
		return m_p->tagNodeFrameArray;
	}

	const AniMsgTable* ResKeyFrame::GetMsgTable() const
	{
		return &m_p->msgTable;
	}

	UINT ResKeyFrame::GetNumFrame() const
	{
		if( m_p )
			return m_p->frameArray.size();

		return 0;
	}

	VertexKeyFrame* ResKeyFrame::GetFrame(UINT index) const
	{
		ASSERT( m_p && index>=0 && index<GetNumFrame() );
		return m_p->frameArray[index];
	}

	float ResKeyFrame::GetTrackTime( DWORD dwTrackName ) 
	{
		if( m_p == NULL )
			return 0.0f;

		map<DWORD,TrackInfo>::iterator it = m_p->m_mapTrackInfo.find( dwTrackName );
		if( it != m_p->m_mapTrackInfo.end() )
			return it->second.fTime;

		return 0.0f;
	}


	int	 ResKeyFrame::GetTrackRange( DWORD dwTrackName, int& nBegin, int& nEnd )
	{
		if( m_p == NULL )
			return -1;

		map<DWORD,TrackInfo>::iterator it = m_p->m_mapTrackInfo.find( dwTrackName );
		if( it != m_p->m_mapTrackInfo.end() )
		{
			nBegin = it->second.nBegin;
			nEnd = it->second.nEnd;
			return 0;
		}

		return -1;
	}

	int ResKeyFrame::GetTrackName( vector<tstring>& trackNames )
	{
		if( m_p == NULL )
			return 0;

		for( map<DWORD,TrackInfo>::iterator it = m_p->m_mapTrackInfo.begin(); it != m_p->m_mapTrackInfo.end(); ++it )
		{
			trackNames.push_back( _FourCC2Str( it->first ) );
		}
		return 0;
	}

	float ResKeyFrame::GetFrmTime( int nFrame )	const
	{	
		if( m_p == NULL )
			return 0.0f;

		ASSERT( nFrame >= 0 && nFrame < (int)m_p->m_vecFrameTime.size() );
		return m_p->m_vecFrameTime[nFrame];	
	}

	TResult ResKeyFrame::RayCollide(const Ray& ray,const VertStream* pVS,const Matrix4* pObj2World)
	{
		if(!IsCreated())
			return TResult(false,0);

		ASSERT(pVS);

		//--把World空间的ray变换到object空间
		Ray localRay;
		Matrix4 world2Obj;
		D3DXMatrixInverse(&world2Obj,NULL,pObj2World);
		D3DXVec3TransformCoord(&localRay.origin,&ray.origin,&world2Obj);
		D3DXVec3TransformNormal(&localRay.dir,&ray.dir,&world2Obj);

		//检测节点内的每个三角形
		for( size_t n=0; n<m_p->subMeshs.size(); n++ )
		{
			FaceBuffer& faces = m_p->subMeshs[n]->baseFaces;
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
			}//endof if
		}

		return TResult(false,0);
	}

	void ResKeyFrame::GetIndexBuffer(vector<WORD>& ib)
	{
		ASSERT(m_p->subMeshs.size()>0);
		ib.clear();
		for(int i=0;i<(int)m_p->subMeshs.size();i++)
		{
			LodSubMesh* pMesh=m_p->subMeshs[i];
			FaceBuffer& face=pMesh->baseFaces;
			for(int j=0;j<(int)face.size();j++)
			{
				ib.push_back(face[j].index[0]);
				ib.push_back(face[j].index[1]);
				ib.push_back(face[j].index[2]);
			}
		}
	}

	void ResKeyFrame::DrawOctree()
	{
		for(UINT i=0;i<m_p->m_octreeBoxes.size();i++)
			DrawX::DrawAABBox(m_p->m_octreeBoxes[i]);
	}

	int ResKeyFrame::GetLightMapSize( void ) const
	{
		return m_p->m_lightmapSize;
	}

	bool ResKeyFrame::SetLightMapSize( int size )
	{
		FILE* fp=_tfopen(GetName(),_T("r+b"));
		if(fp == NULL)
			return false;
		AKHeader header;
		fread(&header,sizeof(AKHeader),1,fp);
		header.lightmapSize=size;
		fseek(fp,0,SEEK_SET);
		fwrite(&header,sizeof(header),1,fp);
		fclose(fp);

		return true;
	}

	const tagMtlDesc& ResKeyFrame::GetMtlDesc( const int i ) const
	{
		ASSERT( i >= 0 && i < (int)m_p->mtlDescArray.size() );
		return m_p->mtlDescArray[i];
	}

}//namespace Cool3D