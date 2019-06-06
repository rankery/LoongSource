#include "StdAfx.h"
#include ".\terrainpatch.h"
#include "..\Util\Exception.h"
#include ".\TerrainPatchRes.h"
#include ".\Heightmap.h"
#include ".\SGTrnDecoPatch.h"
#include ".\TerrainPatchRender.h"
#include ".\TerrainMultiLayersRender.h"
#include ".\TerrainPatchRender.h"
#include ".\TPatchLodTable.h"
#include ".\TPatchTriangulationStatic.h"
#include "..\ResSys\ResMgr.h"
#include "..\Math\Frustum.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\Util\StlExt.h"
#include "..\Util\SpeedAnalyser.h"
#include "..\RenderSys\RenderSys.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\Image\ImageTGA.h"
#include "..\Image\Image.h"
#include "..\Cool3DOption.h"
#include "..\Kernel.h"
#include ".\GrassPatch.h"

namespace Cool3D
{
	const DWORD FORCE_UPDATE_TIMER=1000;
	IMP_RTTI_DYNC(TerrainPatch,SceneNode);

	TerrainPatch::TerrainPatch(void)
	{
		m_pTerrainRes=NULL;
		m_pTerrainPatchRes=NULL;
		m_pTerrainPatchRenderGroupsRes=NULL;
		m_pRender=new TerrainPatchRender;
		m_lastForceUpdateTime=Kernel::Inst()->GetAccumTimeDW();
		m_sgLod=0;
		memset(m_mtls,0,EMType_Num*sizeof(IMaterial*));
	}

	TerrainPatch::~TerrainPatch(void)
	{
		SAFE_DELETE(m_pRender);

		RES_MGR_RELEASE_RENDERABLE(m_pTerrainRes);
		RES_MGR_RELEASE_RENDERABLE(m_pTerrainPatchRes);
		RES_MGR_RELEASE_RENDERABLE(m_pTerrainPatchRenderGroupsRes);

		SAFE_DELETE(m_mtls[EMType_PreCombine]);
		SAFE_DELETE(m_mtls[EMType_TwoLayers]);
		SAFE_DELETE(m_mtls[EMType_Editor]);

		FreePtrVector<IMaterial> freemtls;
		freemtls(m_renderGroupsMtl);
	}

	void TerrainPatch::Serialize(FArchive& ar)
	{
		ar<<m_pTerrainRes->GetName();
		ar<<m_pTerrainPatchRes->GetName();
		ar<<m_pTerrainPatchRenderGroupsRes->GetName();
		
		SceneNode::Serialize(ar);
	}
	
	void TerrainPatch::Deserialize(FArchive& ar)
	{
		ar>>m_szTerrainRes;
		ar>>m_szTerrainPatchRes;
		ar>>m_szTerrainPatchRenderGroupsRes;
		
		LoadTerrainRes(m_szTerrainRes.c_str());
		LoadTerrainPatchRes(m_szTerrainPatchRes.c_str());

		SceneNode::Deserialize(ar);
	}

	bool TerrainPatch::IsInFrustum(const Frustum *pFrumstum) const
	{
		if(m_pTerrainPatchRes==NULL
			||!m_pTerrainPatchRes->IsCreated())
			return false;
		return pFrumstum->BoxInFrustum(*m_pTerrainPatchRes->GetAABBox());
	}

	void TerrainPatch::LoadRes(const TCHAR* szName)
	{
	}

	void TerrainPatch::LoadTerrainRes(const TCHAR* szName)
	{
		ASSERT(m_pTerrainRes==NULL);
		m_pTerrainRes=(TerrainRes*)ResMgr::Inst()->NewRenderable(szName);
	}

	void TerrainPatch::LoadTerrainPatchRes(const TCHAR* szName)
	{
		ASSERT(m_pTerrainPatchRes==NULL);
		m_pTerrainPatchRes=(TerrainPatchRes*)ResMgr::Inst()->NewRenderable(szName);
		m_pRender->SetTerrainPatchRes(m_pTerrainPatchRes);
	}

	void TerrainPatch::LoadTerrainPatchRenderGroupsRes(const TCHAR* szName)
	{
		ASSERT(m_pTerrainPatchRenderGroupsRes==NULL);
		m_pTerrainPatchRenderGroupsRes=(TerrainPatchRenderGroupsRes*)ResMgr::Inst()->NewRenderable(szName);
	}

	UINT TerrainPatch::GetNumRenderNode() const
	{
		return 1;
	}

	RenderNode* TerrainPatch::GetRenderNode(UINT i) const
	{
		return m_pRender;
	}

	const Matrix4& TerrainPatch::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void TerrainPatch::GetBox(AABBox& out) const
	{
		if(m_pTerrainPatchRes)
			out=*(m_pTerrainPatchRes->GetAABBox());
	}

	IMaterial* TerrainPatch::GetMtl(int mtlID)
	{
		if(mtlID>=0&&mtlID<EMType_Num)
			return m_mtls[mtlID];
		return NULL;
	}

	IMaterial* TerrainPatch::GetRenderGroupMtl(int group)
	{
		if(group>=0&&group<(int)m_renderGroupsMtl.size())
			return m_renderGroupsMtl[group];
		return NULL;
	}

	inline int _CalcTrnLod(int sgLod)
	{
		int lod=0;
		if(sgLod<=4)//小于等于512个格子
			lod=0;
		else if(sgLod<=6)//小于等于1024个格子
			lod=1;
		else//大于1024个格子
			lod=TerrainPatch::MAX_LOD;
		return lod;
	}

	void TerrainPatch::Update(float deltaTime,SceneGraph *pSG)
	{
		if(m_pTerrainPatchRes!=NULL 
			&&m_pTerrainPatchRes->IsCreated())
		{
			m_sgLod=pSG->CalcDetailLevel(*m_pTerrainPatchRes->GetAABBox());
			int trnLod=_CalcTrnLod(m_sgLod);
			TPatchLodTable::Inst()->SafeSetLod(m_pTerrainPatchRes->GetPatchX(),m_pTerrainPatchRes->GetPatchZ(),trnLod);
		}

		SceneNode::Update(deltaTime,pSG);
	}

	void TerrainPatch::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		if(pSG->IsSyncInit()
			||Kernel::Inst()->GetAccumTimeDW()-m_lastForceUpdateTime>=FORCE_UPDATE_TIMER)
		{
			m_lastForceUpdateTime=Kernel::Inst()->GetAccumTimeDW();

			if(m_pTerrainRes!=NULL
				&&m_pTerrainRes->IsCreated()
				&&m_pTerrainPatchRes!=NULL
				&&m_pTerrainPatchRes->IsCreated())
			{
				int terrainDetail=Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainDetail);

				if(m_mtls[EMType_PreCombine]==NULL)
				{
					m_mtls[EMType_PreCombine]=m_pTerrainPatchRes->CreatePrecombineMtl();
				}

				if(terrainDetail==Cool3DOption::ETD_TwoLayers)
				{
					if(m_mtls[EMType_TwoLayers]==NULL)
					{
						m_mtls[EMType_TwoLayers]=m_pTerrainPatchRes->CreateTwoLayersMtl();
					}
				}

				if(terrainDetail==Cool3DOption::ETD_Editor)
				{
					if(m_mtls[EMType_Editor]==NULL)
					{
						m_mtls[EMType_Editor]=
							m_pTerrainPatchRes->CreateEditorMtl(
								m_pTerrainRes->GetMtlDiffuse(),
								m_pTerrainRes->GetMtlAmbient(),
								m_pTerrainRes->GetMtlSpecular(),
								m_pTerrainRes->GetMtlPower());
					}
				}

				if(terrainDetail==Cool3DOption::ETD_MultiLayers)
				{
					int sgLod=pSG->CalcDetailLevel(*m_pTerrainPatchRes->GetAABBox());
					if(sgLod<=4)
					{
						if(m_pTerrainPatchRenderGroupsRes==NULL)
							LoadTerrainPatchRenderGroupsRes(m_szTerrainPatchRenderGroupsRes.c_str());

						if(m_renderGroupsMtl.empty()
							&&m_pTerrainPatchRenderGroupsRes!=NULL
							&&m_pTerrainPatchRenderGroupsRes->IsCreated())
						{
							m_pTerrainRes->CreateRenderGroupsMtls(m_pTerrainPatchRenderGroupsRes,GetLightmap(),m_renderGroupsMtl);
						}
					}
					else
					{
						if(m_pTerrainPatchRenderGroupsRes!=NULL)
							RES_MGR_RELEASE_RENDERABLE(m_pTerrainPatchRenderGroupsRes);

						if(!m_renderGroupsMtl.empty())
						{
							FreePtrVector<IMaterial> freemtls;
							freemtls(m_renderGroupsMtl);
						}
					}
				}
			}
		}

		SceneNode::ForceUpdate(deltaTime,pSG);
	}

	void TerrainPatch::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT(0 && _T("cann't set world transform to a terrain patch"));
	}

	void TerrainPatch::GetVertexBuffer(VertStream* vb)
	{
		vb->Create(EVType_PNDT,TerrainEditor::EPatchVerts);
		
		ASSERT(m_pTerrainPatchRes!=NULL&&m_pTerrainPatchRes->IsCreated());
		const VertStream& vbPN=m_pTerrainPatchRes->GetPosNormalVS();
		ASSERT(m_pTerrainRes!=NULL&&m_pTerrainRes->IsCreated());
		const VertStream& vbUV=m_pTerrainRes->GetTexCoord2VS();

		Vert_PNDT* pDest=(Vert_PNDT*)vb->GetRawBuffer();
		Vert_PN*   pPN=(Vert_PN*)vbPN.GetRawBuffer();
		Vert_T2*   pUV=(Vert_T2*)vbUV.GetRawBuffer();
		for(UINT i=0;i<vb->GetNumVert();i++)
		{
			pDest->pos=pPN->pos;
			pDest->normal=pPN->normal;
			pDest->uv=pUV->uv[1];
			ASSERT(pDest->uv.x<=1.0f);
			ASSERT(pDest->uv.y<=1.0f);
			pDest++;
			pPN++;
			pUV++;
		}
	}

	void TerrainPatch::GetIndexBuffer(vector<WORD>& ib)
	{
		TPatchTriangulationStatic::Inst()->GetIndices(0,ib);
	}

	void TerrainPatch::BuildDecalMesh( const AABBox& box, DynamicMesh *pMesh, const Vector3* pNormal )
	{
		if( NULL == m_pTerrainPatchRes )
			return;
		if( !m_pTerrainPatchRes->IsCreated() )
			return;

		if( !m_pTerrainPatchRes->GetAABBox()->Intersects( box ) )
			return;

		BuildDecalFromRes( box, pMesh );
	}

	void TerrainPatch::BuildDecalFromRes(const AABBox& box,DynamicMesh *pMesh)
	{
		if( NULL == m_pTerrainPatchRes || !m_pTerrainPatchRes->IsCreated() )
			return;

		VertStream& vbPN = const_cast<VertStream&>( m_pTerrainPatchRes->GetPosNormalVS() );

		// 计算顶点索引范围
		POINT ptMinIndex, ptMaxIndex;
		Vector3 vPosMin, vPosMax;
		vbPN.GetPos( 0, vPosMin );
		vbPN.GetPos( TerrainEditor::EPatchSideVerts * TerrainEditor::EPatchSideVerts - 1, vPosMax );
		const float fInvX = 1.0f / ( vPosMax.x - vPosMin.x ) * ( TerrainEditor::EPatchSideVerts - 1 );
		const float fInvZ = 1.0f / ( vPosMax.z - vPosMin.z ) * ( TerrainEditor::EPatchSideVerts - 1 );
		const LONG nGridVert = 2;

		ptMinIndex.x = LONG( ( box.min.x - vPosMin.x ) * fInvX - 1.0f );
		ptMinIndex.y = LONG( ( box.min.z - vPosMin.z ) * fInvZ - 1.0f );
		ptMaxIndex.x = LONG( ( box.max.x - vPosMin.x ) * fInvX + 1.0f );
		ptMaxIndex.y = LONG( ( box.max.z - vPosMin.z ) * fInvZ + 1.0f );

		if( ptMinIndex.x < 0 ) ptMinIndex.x = 0;
		if( ptMinIndex.y < 0 ) ptMinIndex.y = 0;
		if( ptMaxIndex.x > TerrainEditor::EPatchSideVerts - 1 ) ptMaxIndex.x = TerrainEditor::EPatchSideVerts - 1;
		if( ptMaxIndex.y > TerrainEditor::EPatchSideVerts - 1 ) ptMaxIndex.y = TerrainEditor::EPatchSideVerts - 1;

		// 与地形格子对齐
		ptMinIndex.x -= ptMinIndex.x % nGridVert;
		ptMinIndex.y -= ptMinIndex.y % nGridVert;
		if( ptMaxIndex.x % nGridVert != 0 )
			ptMaxIndex.x += ( nGridVert - ( ptMaxIndex.x % nGridVert ) );
		if( ptMaxIndex.y % nGridVert != 0 )
			ptMaxIndex.y += ( nGridVert - ( ptMaxIndex.y % nGridVert ) );

		// 取得顶点剪裁后加入pMesh，三角形构造方式必须与地形三角形相同
		for( int i = ptMinIndex.y; i <= ptMaxIndex.y - 2; i += 2 )
		{
			for( int j = ptMinIndex.x; j <= ptMaxIndex.x - 2; j += 2 )
			{
				Vector3 Tri[3];
				vbPN.GetPos(   i       * TerrainEditor::EPatchSideVerts + j,     Tri[0] );
				vbPN.GetPos(   i       * TerrainEditor::EPatchSideVerts + j + 2, Tri[1] );
				vbPN.GetPos( ( i + 1 ) * TerrainEditor::EPatchSideVerts + j + 1, Tri[2] );
				BuildDecalClipTri( Tri, box, pMesh );

				vbPN.GetPos(   i       * TerrainEditor::EPatchSideVerts + j + 2, Tri[0] );
				vbPN.GetPos( ( i + 1 ) * TerrainEditor::EPatchSideVerts + j + 1, Tri[1] );
				vbPN.GetPos( ( i + 2 ) * TerrainEditor::EPatchSideVerts + j + 2, Tri[2] );
				BuildDecalClipTri( Tri, box, pMesh );

				vbPN.GetPos( ( i + 2 ) * TerrainEditor::EPatchSideVerts + j + 2, Tri[0] );
				vbPN.GetPos( ( i + 1 ) * TerrainEditor::EPatchSideVerts + j + 1, Tri[1] );
				vbPN.GetPos( ( i + 2 ) * TerrainEditor::EPatchSideVerts + j,     Tri[2] );
				BuildDecalClipTri( Tri, box, pMesh );

				vbPN.GetPos( ( i + 2 ) * TerrainEditor::EPatchSideVerts + j,     Tri[0] );
				vbPN.GetPos( ( i + 1 ) * TerrainEditor::EPatchSideVerts + j + 1, Tri[1] );
				vbPN.GetPos(   i       * TerrainEditor::EPatchSideVerts + j,     Tri[2] );
				BuildDecalClipTri( Tri, box, pMesh );
			}
		}
	}

	void TerrainPatch::BuildDecalClipTri( const Vector3* pTri, const AABBox& box, DynamicMesh* pMesh )
	{

		Vector3 newVertex[9];
		const int vertCount = box.ClipTriangle( pTri, newVertex );
		for( int nVert = 1; nVert < vertCount - 1; nVert++ )
		{
			pMesh->AddPos( newVertex[0] );
			pMesh->AddPos( newVertex[nVert] );
			pMesh->AddPos( newVertex[nVert+1] );
		}
	}

	bool TerrainPatch::IsLoaded()
	{
		if(m_pTerrainRes==NULL
			||!m_pTerrainRes->IsCreated())
			return false;

		if(m_pTerrainPatchRes==NULL
			||!m_pTerrainPatchRes->IsCreated())
			return false;

		int terrainDetail=Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TerrainDetail);

		if(m_mtls[EMType_PreCombine]==NULL
			||!m_mtls[EMType_PreCombine]->IsResourceReady())
			return false;

		if(terrainDetail==Cool3DOption::ETD_TwoLayers)
		{
			if(m_mtls[EMType_TwoLayers]==NULL
				||!m_mtls[EMType_TwoLayers]->IsResourceReady())
				return false;
		}

		if(terrainDetail==Cool3DOption::ETD_Editor)
		{
			if(m_mtls[EMType_Editor]==NULL
				||!m_mtls[EMType_Editor]->IsResourceReady())
				return false;
		}

		/*
		if(terrainDetail==Cool3DOption::ETD_MultiLayers)
		{
			int sgLod=pSG->CalcDetailLevel(*m_pTerrainPatchRes->GetAABBox());
			if(sgLod<=4)
			{
				if(m_pTerrainPatchRenderGroupsRes==NULL
					||!m_pTerrainPatchRenderGroupsRes->IsCreated())
					return false;

				if(m_renderGroupsMtl.empty())
					return false;

				for(size_t i=0;i<m_renderGroupsMtl.size();i++)
				{
					if(!m_renderGroupsMtl[i]->IsResourceReady())
						return false;
				}
			}
		}*/

		return true;
	}
}//namespace Cool3D                      