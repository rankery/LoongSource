#include "StdAfx.h"
#include "..\SpeedTreeRT\include\SpeedTreeRT.h"

#include "..\Math\CameraBase.h"
#include "RenderSys.h"
#include ".\treenode.h"
#include "..\Util\Exception.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IIndexBuffer.h"
#include "..\Device\IStreamDefine.h"
#include "..\ResSys\VertStream.h"
#include "..\ResSys\ResMgr.h"
#include "..\RenderMtl\TreeMtl.h"
#include "SpeedTreeRender.h"
#include "..\Util\FMemory.h"

namespace F3D
{
	#define AGBR2ARGB(dwColor) (dwColor & 0xff00ff00) + ((dwColor & 0x00ff0000) >> 16) + ((dwColor & 0x000000ff) << 16)

	namespace Detail
	{
		struct tagBranchVertex
		{
			Vector3			m_vPosition;			// Always Used							
			Vector3			m_vNormal;				// Dynamic Lighting Only			
			DWORD			m_dwDiffuseColor;		// Static Lighting Only	
			float			m_fTexCoords[2];		// Always Used
			float			m_fShadowCoords[2];		// Texture coordinates for the shadows
		};

		struct tagLeafVertex
		{
			D3DXVECTOR3		m_vPosition;			// Always Used							
			D3DXVECTOR3		m_vNormal;				// Dynamic Lighting Only			
			DWORD			m_dwDiffuseColor;		// Static Lighting Only	
			FLOAT			m_fTexCoords[2];		// Always Used
		};

	}

	using namespace Detail;

	IMP_RTTI_DYNC(TreeNode,RenderNode);

	class TreeNode::Member
	{
	public:
		CSpeedTreeRT				*m_pSpeedTree;
		AABBox						m_boundingBox;
		CSpeedTreeRT::SGeometry		*m_pGeometryCache;
		// branch buffers
		VertStream					*m_pBranchVertexBuffer;			// branch vertex buffer
		UINT						m_unBranchVertexCount;			// number of vertices in branches
		IIndexBuffer				*m_pBranchIndexBuffer;			// branch index buffer
		UINT						m_BranchLod;
		WORD						*m_pBranchIndexCounts;			// number of indexes per branch LOD level

		// frond buffers
		VertStream					*m_pFrondVertexBuffer;			// frond vertex buffer
		UINT						m_unFrondVertexCount;			// number of vertices in frond
		IIndexBuffer				*m_pFrondIndexBuffer;			// frond index buffer
		UINT						m_FrondLod;
		WORD						*m_pFrondIndexCounts;			// number of indexes per frond LOD level

		// leaf buffers
		unsigned short				m_usNumLeafLods;				// the number of leaf LODs
		VertStream					**m_pLeafVertexBuffer;			// leaf vertex buffer
		bool						*m_pLeavesUpdatedByCpu;			// stores which LOD's have been updated already per frame

		//--
		IStreamDefine				*m_pBranchStreamDef;
		IStreamDefine				*m_pLeafStreamDef;
		IVertBuffer					*m_hdBranchVB;
		IVertBuffer					*m_hdFrondVB;
		UINT						m_renderID;
		TreeMtl						m_mtl;

	public:
		Member()
		{
			m_pBranchStreamDef=NULL;
			m_pLeafStreamDef=NULL;
			m_renderID=0xFFFFFFFF;
			m_pSpeedTree=NULL;
			m_pGeometryCache=NULL;

			m_pBranchVertexBuffer=NULL;	
			m_unBranchVertexCount=0;	
			m_pBranchIndexBuffer=NULL;	
			m_pBranchIndexCounts=NULL;	

			m_pFrondVertexBuffer=NULL;
			m_unFrondVertexCount=0;
			m_pFrondIndexBuffer=NULL;
			m_pFrondIndexCounts=NULL;

			m_usNumLeafLods=0;		
			m_pLeafVertexBuffer=NULL;	
			m_pLeavesUpdatedByCpu=NULL;

			m_hdBranchVB=NULL;
			m_hdFrondVB=NULL;

			m_BranchLod=0;
			m_FrondLod=0;

		}
		~Member()
		{
			m_pSpeedTree=NULL;
			SAFE_DELETE(m_pGeometryCache);

			SAFE_DELETE(m_pBranchVertexBuffer);	
			SAFE_DELETE(m_pBranchIndexBuffer);	
			SAFE_DELETE(m_pBranchIndexCounts);

			SAFE_DELETE(m_pFrondVertexBuffer);
			SAFE_DELETE(m_pFrondIndexBuffer);
			SAFE_DELETE(m_pFrondIndexCounts);

			SAFE_DELETE(m_pLeafVertexBuffer);	//???!!!!
			SAFE_DELETE(m_pLeavesUpdatedByCpu);

			SAFE_DELETE(m_pBranchStreamDef);
			SAFE_DELETE(m_pLeafStreamDef);
		}

		void CreateLeafBuffers(CSpeedTreeRT *pTree)
		{
			// set up constants
			const short anVertexIndices[6] = { 0, 1, 2, 0, 2, 3 };

			// set up the leaf counts for each LOD
			m_usNumLeafLods = pTree->GetNumLeafLodLevels( );

			// create arrays for the number of LOD levels
			m_pLeafVertexBuffer = new VertStream*[m_usNumLeafLods];
			m_pLeavesUpdatedByCpu = new bool[m_usNumLeafLods];

			for (unsigned int unLod = 0; unLod < m_usNumLeafLods; ++unLod)
			{
				m_pLeavesUpdatedByCpu[unLod] = false;

				// if this level has no leaves, then skip it
				unsigned short usLeafCount = m_pGeometryCache->m_sLeaves0.m_usLeafCount;
				if (usLeafCount < 1)
					continue;

				// create the vertex buffer for storing leaf vertices
				m_pLeafVertexBuffer[unLod]=new VertStream;
				m_pLeafVertexBuffer[unLod]->Create(sizeof(tagLeafVertex),usLeafCount * 6);

				// fill the vertex buffer by interleaving SpeedTree data
				tagLeafVertex* pVertexBuffer = (tagLeafVertex*)m_pLeafVertexBuffer[unLod]->GetRawBuffer();
				tagLeafVertex* pVertex = pVertexBuffer;
				for (unsigned int unLeaf = 0; unLeaf < usLeafCount; ++unLeaf)
				{
					const CSpeedTreeRT::SGeometry::SLeaf* pLeaf = &(m_pGeometryCache->m_sLeaves0);
					for (unsigned int unVert = 0; unVert < 6; ++unVert)  // 6 verts == 2 triangles
					{
						// position
						FMemcpy(pVertex->m_vPosition, &(pLeaf->m_pCenterCoords[unLeaf * 3]), 3 * sizeof(float));
						// normal
						if(pLeaf->m_pNormals)
							FMemcpy(&pVertex->m_vNormal, &(pLeaf->m_pNormals[unLeaf * 3]), 3 * sizeof(float));
						// color
						if(pLeaf->m_pColors)
							pVertex->m_dwDiffuseColor = AGBR2ARGB(pLeaf->m_pColors[unLeaf]);
						// tex coord
						FMemcpy(pVertex->m_fTexCoords, &(pLeaf->m_pLeafMapTexCoords[unLeaf][anVertexIndices[unVert] * 2]), 2 * sizeof(float));
						++pVertex;
					}

				}
			}
		}

		void CreateFrondBuffers(CSpeedTreeRT *pTree)
		{
			// reference to frond structure	
			CSpeedTreeRT::SGeometry::SIndexed* pFronds = &(m_pGeometryCache->m_sFronds);
			m_unFrondVertexCount = pFronds->m_usVertexCount; // we asked for a contiguous strip

			// check if this tree has fronds
			if (m_unFrondVertexCount <= 1)
				return;

			// create the vertex buffer for storing frond vertices
			m_pFrondVertexBuffer=new VertStream;
			m_pFrondVertexBuffer->Create(sizeof(tagBranchVertex),m_unFrondVertexCount);

			// fill the vertex buffer by interleaving SpeedTree data
			tagBranchVertex* pVertexBuffer = (tagBranchVertex*)m_pFrondVertexBuffer->GetRawBuffer();
			
			for (unsigned short i = 0; i < m_unFrondVertexCount; ++i)
			{
				// position
				FMemcpy(&pVertexBuffer->m_vPosition, &(pFronds->m_pCoords[i * 3]), 3 * sizeof(float));

				// normal
				if(pFronds->m_pNormals)
					FMemcpy(&pVertexBuffer->m_vNormal, &(pFronds->m_pNormals[i * 3]), 3 * sizeof(float));
				//color
				if(pFronds->m_pColors)
					pVertexBuffer->m_dwDiffuseColor = AGBR2ARGB(pFronds->m_pColors[i]);

				// texcoords for layer 0
				pVertexBuffer->m_fTexCoords[0] = pFronds->m_pTexCoords0[i * 2];
				pVertexBuffer->m_fTexCoords[1] = pFronds->m_pTexCoords0[i * 2 + 1];

				// texcoords for layer 1 (if enabled)
				if(pFronds->m_pTexCoords1)
				{
					pVertexBuffer->m_fShadowCoords[0] = pFronds->m_pTexCoords1[i * 2];
					pVertexBuffer->m_fShadowCoords[1] = pFronds->m_pTexCoords1[i * 2 + 1];
				}

				++pVertexBuffer;
			}


			// create and fill the index counts for each LOD
			unsigned int unNumLodLevels = pTree->GetNumFrondLodLevels( );
			m_pFrondIndexCounts = new unsigned short[unNumLodLevels];
			m_FrondLod=unNumLodLevels;
			for (i = 0; i < unNumLodLevels; ++i)
			{
				// force update of geometry for this LOD
				pTree->GetGeometry(*m_pGeometryCache, SpeedTree_FrondGeometry, -1, i);

				// check if this LOD has fronds
				if (pFronds->m_usNumStrips > 0)
					m_pFrondIndexCounts[i] = pFronds->m_pStripLengths[0];
				else
					m_pFrondIndexCounts[i] = 0;
			}
			// force update of geometry to highest LOD
			pTree->GetGeometry(*m_pGeometryCache, SpeedTree_FrondGeometry, -1, 0);

			// the first LOD level contains the most indices of all the levels, so
			// we use its size to allocate the index buffer
			m_pFrondIndexBuffer=Device()->NewIB();
			m_pFrondIndexBuffer->CreateWord(m_pFrondIndexCounts[0] * sizeof(WORD),EB_StaticWriteOnly);
			m_pFrondIndexBuffer->CopyFromMem((void*)pFronds->m_pStrips[0],pFronds->m_pStripLengths[0] * sizeof(WORD));
		}

		void CreateBranchBuffers(CSpeedTreeRT *pTree)
		{
			// reference to the branch structure
			CSpeedTreeRT::SGeometry::SIndexed* pBranches = &(m_pGeometryCache->m_sBranches);
			m_unBranchVertexCount = pBranches->m_usVertexCount; // we asked for a contiguous strip

			// check if this tree has branches
			if (m_unBranchVertexCount <= 1)
				return;

			m_pBranchVertexBuffer=new VertStream;
			m_pBranchVertexBuffer->Create(sizeof(tagBranchVertex),m_unBranchVertexCount);

			UINT i;
			// fill the vertex buffer by interleaving SpeedTree data
			tagBranchVertex* pVertexBuffer = (tagBranchVertex*)m_pBranchVertexBuffer->GetRawBuffer();
			for (i = 0; i < m_unBranchVertexCount; ++i)
			{
				// position
				FMemcpy(&pVertexBuffer->m_vPosition, &(pBranches->m_pCoords[i * 3]), 3 * sizeof(float));

				// normal
				if(pBranches->m_pNormals)
					FMemcpy(&pVertexBuffer->m_vNormal, &(pBranches->m_pNormals[i * 3]), 3 * sizeof(float));
				//color
				if(pBranches->m_pColors)
					pVertexBuffer->m_dwDiffuseColor = AGBR2ARGB(pBranches->m_pColors[i]);


				// texcoords for layer 0
				pVertexBuffer->m_fTexCoords[0] = pBranches->m_pTexCoords0[i * 2];
				pVertexBuffer->m_fTexCoords[1] = pBranches->m_pTexCoords0[i * 2 + 1];

				// texcoords for layer 1 (if enabled)
				if(pBranches->m_pTexCoords1)
				{
					pVertexBuffer->m_fShadowCoords[0] = pBranches->m_pTexCoords1[i * 2];
					pVertexBuffer->m_fShadowCoords[1] = pBranches->m_pTexCoords1[i * 2 + 1];
				}

				++pVertexBuffer;
			}
			
			// create and fill the index counts for each LOD
			UINT unNumLodLevels = pTree->GetNumBranchLodLevels( );
			m_pBranchIndexCounts = new WORD[unNumLodLevels];
			m_BranchLod=unNumLodLevels;
			for (i = 0; i < unNumLodLevels; ++i)
			{
				// force geometry update for this LOD
				pTree->GetGeometry(*m_pGeometryCache, SpeedTree_BranchGeometry, (short)i);

				// check if this LOD has branches
				if (pBranches->m_usNumStrips > 0)
					m_pBranchIndexCounts[i] = pBranches->m_pStripLengths[0];
				else
					m_pBranchIndexCounts[i] = 0;
			}
			// force update of geometry to highest LOD
			pTree->GetGeometry(*m_pGeometryCache, SpeedTree_BranchGeometry, 0);

			m_pBranchIndexBuffer=Device()->NewIB();
			m_pBranchIndexBuffer->CreateWord(m_pBranchIndexCounts[0] * sizeof(WORD),EB_StaticWriteOnly);
			m_pBranchIndexBuffer->CopyFromMem((void*)pBranches->m_pStrips[0],pBranches->m_pStripLengths[0] * sizeof(WORD));

		}

		void Create(CSpeedTreeRT *pTree,const char* szOwnerName)
		{
			pTree->GetBoundingBox((float*)&m_boundingBox);
			
			// read all the geometry for highest LOD into the geometry cache
			pTree->SetLodLevel(1.0f);
			if (m_pGeometryCache == NULL)
				m_pGeometryCache = new CSpeedTreeRT::SGeometry;
			pTree->GetGeometry(*m_pGeometryCache);

			CreateBranchBuffers(pTree);
			CreateFrondBuffers(pTree);
			CreateLeafBuffers(pTree);

			m_pSpeedTree=pTree;

			char szTexBasePath[_MAX_PATH];
			char szDriver[100];
			_splitpath(szOwnerName,szDriver,szTexBasePath,NULL,NULL);
			string szFullPath;
			szFullPath=szDriver;
			szFullPath+=szTexBasePath;
			m_mtl.CreateForTree(pTree);

			//--
			m_pBranchStreamDef=Device()->NewStreamDefine();
			m_pBranchStreamDef->Begin();
			m_pBranchStreamDef->AddElement(0,	0,			VET_FLOAT3,	VES_POSITION,0);
			m_pBranchStreamDef->AddElement(0,	4*3,		VET_FLOAT3,	VES_NORMAL,0);
			m_pBranchStreamDef->AddElement(0,	4*6,		VET_COLOUR,	VES_DIFFUSE,0);
			m_pBranchStreamDef->AddElement(0,	4*7,		VET_FLOAT2,	VES_TEXCOORD,0);
			m_pBranchStreamDef->AddElement(0,	4*9,		VET_FLOAT2,	VES_TEXCOORD,1);
			m_pBranchStreamDef->End();

			m_pLeafStreamDef=Device()->NewStreamDefine();
			m_pLeafStreamDef->Begin();
			m_pLeafStreamDef->AddElement(0,	0,			VET_FLOAT3,	VES_POSITION,0);
			m_pLeafStreamDef->AddElement(0,	4*3,		VET_FLOAT3,	VES_NORMAL,0);
			m_pLeafStreamDef->AddElement(0,	4*6,		VET_COLOUR,	VES_DIFFUSE,0);
			m_pLeafStreamDef->AddElement(0,	4*7,		VET_FLOAT2,	VES_TEXCOORD,0);
			m_pLeafStreamDef->End();

			pTree->SetWindStrength(0.3f);//tmp
			
		}

		void UpdateBranchVert()
		{
			ASSERT(m_pSpeedTree!=NULL);
			m_pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_BranchGeometry);
			if (m_pGeometryCache->m_sBranches.m_usNumStrips > 0)
			{
				// update the vertex array
				tagBranchVertex* pVertexBuffer = (tagBranchVertex*)m_pBranchVertexBuffer->GetRawBuffer();
				for (unsigned int i = 0; i < m_unBranchVertexCount; ++i)
				{
					FMemcpy(&(pVertexBuffer[i].m_vPosition), &(m_pGeometryCache->m_sBranches.m_pCoords[i * 3]), 3 * sizeof(float));
				}
			}
		}
		void UpdateFrondVert()
		{
			// update the frond geometry for CPU wind
			m_pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_FrondGeometry);
			if (m_pGeometryCache->m_sFronds.m_usNumStrips > 0)
			{
				// update the vertex array
				tagBranchVertex* pVertexBuffer = (tagBranchVertex*)m_pFrondVertexBuffer->GetRawBuffer();
				for (unsigned int i = 0; i < m_unFrondVertexCount; ++i)
				{
					FMemcpy(&(pVertexBuffer[i].m_vPosition), &(m_pGeometryCache->m_sFronds.m_pCoords[i * 3]), 3 * sizeof(float));
				}
			}
		}

		void DrawBranch(int lod)
		{
			lod=RenderSys::ScaleSGLod(lod,m_BranchLod);//缩放到本节点的lod数字

			m_pSpeedTree->GetGeometry(*(m_pGeometryCache), SpeedTree_BranchGeometry);

			if (m_pGeometryCache->m_fBranchAlphaTestValue > 0.0f)
			{
				// set alpha test value
				Device()->SetAlphaRef(DWORD(m_pGeometryCache->m_fBranchAlphaTestValue));

				if(lod >= m_BranchLod)
					lod = m_BranchLod-1;
				// if tree has branches at this LOD, draw them
				if (m_pBranchIndexCounts 
					&& lod > -1 
					&& m_pBranchIndexCounts[lod] > 0)
					Device()->DrawIndexed(EDraw_TriangleStrip,0,
					m_pGeometryCache->m_sBranches.m_usVertexCount,
					m_pBranchIndexCounts[lod] - 2);
			}
		}
		void DrawFrond(int lod)
		{
			lod=RenderSys::ScaleSGLod(lod,m_FrondLod);//缩放到本节点的lod数字

			m_pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_FrondGeometry);

			if (m_pGeometryCache->m_fFrondAlphaTestValue > 0.0f)
			{
				Device()->SetAlphaRef(DWORD(m_pGeometryCache->m_fFrondAlphaTestValue));
				
				if(lod >= m_FrondLod)
					lod = m_FrondLod-1;
				// if tree has fronds, draw them
				if (m_pFrondIndexCounts 
					&& lod > -1 
					&& m_pFrondIndexCounts[lod] > 0)
					Device()->DrawIndexed(EDraw_TriangleStrip,0,
					m_pGeometryCache->m_sFronds.m_usVertexCount,
					m_pFrondIndexCounts[lod] - 2);
			}
		}

		void DrawLeafs(int unLod)
		{
			unLod=RenderSys::ScaleSGLod(unLod,m_usNumLeafLods);//缩放到本节点的lod数字

			if(unLod<0)
				return;
			if(unLod>=m_usNumLeafLods)
				unLod=m_usNumLeafLods-1;
			m_pSpeedTree->GetGeometry(*m_pGeometryCache, SpeedTree_LeafGeometry);


			// might need to draw 2 LOD's
			for (unsigned int i = 0; i < 2; ++i)
			{
				// reference to leaves structure
				const CSpeedTreeRT::SGeometry::SLeaf* pLeaf = (i == 0) ? &m_pGeometryCache->m_sLeaves0 : &m_pGeometryCache->m_sLeaves1;
				if (!pLeaf->m_bIsActive)
					continue;

				if(m_pLeavesUpdatedByCpu[unLod])//本帧已经Update过
					continue;
				m_pLeavesUpdatedByCpu[unLod]=true;
				// update the vertices
				tagLeafVertex* pVertex =(tagLeafVertex*)m_pLeafVertexBuffer[unLod]->GetRawBuffer();
				for (unsigned int unLeaf = 0; unLeaf < pLeaf->m_usLeafCount; ++unLeaf)
				{
					Vector3 vecCenter(&(pLeaf->m_pCenterCoords[unLeaf * 3]));
					Vector3 vec0(&pLeaf->m_pLeafMapCoords[unLeaf][0]);
					Vector3 vec1(&pLeaf->m_pLeafMapCoords[unLeaf][4]);
					Vector3 vec2(&pLeaf->m_pLeafMapCoords[unLeaf][8]);
					Vector3 vec3(&pLeaf->m_pLeafMapCoords[unLeaf][12]);

					(pVertex++)->m_vPosition = vecCenter + vec0;		// vertex 0
					(pVertex++)->m_vPosition = vecCenter + vec1;		// vertex 1
					(pVertex++)->m_vPosition = vecCenter + vec2;		// vertex 2
					(pVertex++)->m_vPosition = vecCenter + vec0;		// vertex 0
					(pVertex++)->m_vPosition = vecCenter + vec2;		// vertex 2
					(pVertex++)->m_vPosition = vecCenter + vec3;		// vertex 3
				}

			}//endof for


			// might need to draw 2 LOD's
			IDevice *pDev=Device();
			pDev->SetStreamDefine(m_pLeafStreamDef);
			for (unsigned int unLeafLevel = 0; unLeafLevel < 2; ++unLeafLevel)
			{
				const CSpeedTreeRT::SGeometry::SLeaf* pLeaf = (unLeafLevel == 0) ?
					&m_pGeometryCache->m_sLeaves0 : pLeaf = &m_pGeometryCache->m_sLeaves1;


				// if this LOD is active and has leaves, draw it
				if (unLod > -1 && pLeaf->m_bIsActive && pLeaf->m_usLeafCount > 0)
				{
					VertStream *pLeafVB=m_pLeafVertexBuffer[unLod];
					IVertBuffer *pVB=Kernel::Inst()->GetRenderSys()->AllocVB(pLeafVB->GetBufferSize());
					pVB->CopyFromVertStream(pLeafVB);
					pDev->SetVB(0,pVB);
					pDev->SetAlphaRef(DWORD(pLeaf->m_fAlphaTestValue));
					pDev->Draw(EDraw_TriangleList,pLeaf->m_usLeafCount * 2);
					Kernel::Inst()->GetRenderSys()->FreeVB(pVB);
				}
			}//endof for
		}
	};

	TreeNode::TreeNode(void)
	{
		m_p=new Member;
	}

	TreeNode::~TreeNode(void)
	{
		delete m_p;
	}

	//--RenderNode
	void TreeNode::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod)
	{
		
	}

	ETransparentType TreeNode::GetTransparentType(const SceneNode *pSGNode) const
	{
		return ETrans_None;
	}
	DWORD TreeNode::GetMtlTrait(SceneNode *pSGNode) const
	{
		return 0;
	}
	const AABBox& TreeNode::GetAABBox() const
	{
		return m_p->m_boundingBox;
	}

	//--
	void TreeNode::Create(CSpeedTreeRT *pTree,const char* szOwnerName)
	{
		m_p->Create(pTree,szOwnerName);
	}

	void TreeNode::UpdateBuffers()
	{
		if(SpeedTreeRender::s_bWindEnable)
		{
			m_p->m_pSpeedTree->ComputeWindEffects(true,true,true);
			m_p->UpdateBranchVert();

			if(m_p->m_pFrondVertexBuffer!=NULL)
				m_p->UpdateFrondVert();
		}
	}

	void TreeNode::BeginBranch()
	{
		
		//m_p->m_pSpeedTree->ComputeLodLevel( );
		
		m_p->m_hdBranchVB=Kernel::Inst()->GetRenderSys()->AllocVB(m_p->m_pBranchVertexBuffer->GetBufferSize());
		m_p->m_hdBranchVB->CopyFromVertStream(m_p->m_pBranchVertexBuffer);

		IDevice *pDev=Device();
		pDev->SetVB(0,m_p->m_hdBranchVB);
		pDev->SetIB(m_p->m_pBranchIndexBuffer);
		pDev->SetStreamDefine(m_p->m_pBranchStreamDef);
		
	}

	void TreeNode::DrawBranch(int lod)
	{	
		m_p->m_mtl.ActiveBranch();
		m_p->DrawBranch(lod);
	}

	void TreeNode::EndBranch()
	{
		if(m_p->m_hdBranchVB!=NULL)
		{
			Kernel::Inst()->GetRenderSys()->FreeVB(m_p->m_hdBranchVB);
			m_p->m_hdBranchVB=NULL;
		}
	}

	bool TreeNode::BeginFrond()
	{
		if(m_p->m_pFrondVertexBuffer==NULL)
			return false;

		m_p->m_hdBranchVB=Kernel::Inst()->GetRenderSys()->AllocVB(m_p->m_pFrondVertexBuffer->GetBufferSize());
		m_p->m_hdBranchVB->CopyFromVertStream(m_p->m_pFrondVertexBuffer);

		IDevice *pDev=Device();
		pDev->SetVB(0,m_p->m_hdBranchVB);
		pDev->SetIB(m_p->m_pFrondIndexBuffer);
		pDev->SetStreamDefine(m_p->m_pBranchStreamDef);

		return true;
	}

	void TreeNode::DrawFrond(int lod)
	{
		m_p->m_mtl.ActiveComposite();
		m_p->DrawFrond(lod);
	}

	void TreeNode::EndFrond()
	{
		EndBranch();
	}

	void TreeNode::DrawLeafs(int lod)
	{
		m_p->m_mtl.ActiveComposite();
		m_p->DrawLeafs(lod);
	}

	void TreeNode::EndLeafsTreeType()
	{
;		for(int i=0;i<m_p->m_usNumLeafLods;i++)
			m_p->m_pLeavesUpdatedByCpu[i]=false;
	}
}//namespace F3D