#include "StdAfx.h"
#include ".\nullscenegraph.h"
#include "..\Util\DebugMisc.h"
#include "..\Util\FileName.h"
#include "..\RenderSys\RenderSys.h"
#include "..\Kernel.h"
#include "..\math\Frustum.h"
#include "..\Device\IDevice.h"
#include "EntityNode.h"
#include "..\Terrain\TerrainPatch.h"
#include "QuadTreeDataBuilder.h"
#include "..\VFS\IFS.h"
#include "..\VFS\FileUtil.h"
#include "..\RenderSys\WaterReflectMapRender.h"
#include "..\Terrain\TerrainPatch.h"
#include "..\EditorSupport\TerrainEditor.h"
#include "..\Terrain\GrassPatch.h"
#include "..\Terrain\SGSimpleTerrainPatch.h"
#include "..\Terrain\Heightmap.h"
#include "..\Terrain\SGRiverPatch.h"
#include "..\SceneGraph\sglightnode.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(NullSceneGraph,SceneGraph);

	NullSceneGraph::NullSceneGraph(void)
	{
		m_bShowStaticPL=true;
		m_bShowDynamicPL=false;
	}

	NullSceneGraph::~NullSceneGraph(void)
	{
		Clear();
	}

	void NullSceneGraph::LoadFromFile(IFS *pFS,const TCHAR* szMapName)
	{

	}

	void NullSceneGraph::AttachDynamicNode(SceneNode *pNode)
	{
		m_dynaList.insert(pNode);
	}

	void NullSceneGraph::DetachDynamicNode(const SceneNode *pNode)
	{
		set<SceneNode*>::iterator iter=m_dynaList.find(const_cast<SceneNode*>(pNode));
		if(iter!=m_dynaList.end())
			m_dynaList.erase(iter);
	}

	void NullSceneGraph::AttachStaticNode(SceneNode *pNode,ESceneNodeViewLevel viewLevel)
	{
		if(pNode->IS_STATIC_CLASS(SGLightNode))
		{
			SGLightNode *pLight=(SGLightNode*)pNode;
			m_pointlights.push_back(pNode);

			if( (pLight->IsDynamic() && !m_bShowDynamicPL)
				|| (!pLight->IsDynamic() && !m_bShowStaticPL))
				return;
		}
		m_sceneList.insert(pNode);
		m_viewLevelmap[pNode]=viewLevel;
	}

	void NullSceneGraph::DetachStaticNode(const SceneNode *pNode)
	{
		set<SceneNode*>::iterator iter=m_sceneList.find(const_cast<SceneNode*>(pNode));
		if(iter!=m_sceneList.end())
			m_sceneList.erase(iter);

		if(pNode!=NULL 
			&& pNode->IS_STATIC_CLASS(SGLightNode))
		{
			vector<SceneNode*>::iterator lightIt=m_pointlights.begin();
			for (;lightIt!=m_pointlights.end();++lightIt)
			{
				if(*lightIt==pNode)
				{
					m_pointlights.erase(lightIt);
					break;
				}
			}
		}

		SNViewLevelIter vlIter=m_viewLevelmap.find(const_cast<SceneNode*>(pNode));
		if( vlIter!=m_viewLevelmap.end() )
			m_viewLevelmap.erase(vlIter);
	}

	void NullSceneGraph::Render(float deltaTime)
	{
		SceneGraph::Render(deltaTime);

		Frustum frustum;
		frustum.Extract(m_pCamera->GetView(),m_pCamera->GetPrj());

		//--
		RenderSys *pRender=Kernel::Inst()->GetRenderSys();
		pRender->Begin(this);

		for(set<SceneNode*>::iterator iter=m_dynaList.begin();
			iter!=m_dynaList.end();
			iter++)
		{
			SceneNode *pNode=(*iter);
			pNode->ForceUpdate(deltaTime,this);
			if(pNode->IsInFrustum(&frustum))
			{
				pNode->Update(deltaTime,this);
				pRender->AttachNode(pNode);
			}
		}
		WaterReflectMapRender::Inst()->Begin(this);
		for(set<SceneNode*>::iterator iter=m_sceneList.begin();
			iter!=m_sceneList.end();++iter)
		{
			SceneNode *pNode=(*iter);
			pNode->ForceUpdate(deltaTime,this);
			if(pNode->IsInFrustum(&frustum))
			{
				pNode->Update(deltaTime,this);
				pRender->AttachNode(pNode);
				WaterReflectMapRender::Inst()->AttachNode(pNode);
			}
		}
		WaterReflectMapRender::Inst()->End();

		pRender->End(this);
	}

	void NullSceneGraph::LoadTerrainPatch(const TCHAR* szTerrain,const TCHAR* szTerrainPatch,const TCHAR* szTerrainPatchRenderGroups,const TCHAR* szLightMap)
	{
		TerrainPatch* patch=new TerrainPatch;
		patch->SetLightmap(szLightMap);
		patch->LoadTerrainRes(szTerrain);
		patch->LoadTerrainPatchRes(szTerrainPatch);
		patch->LoadTerrainPatchRenderGroupsRes(szTerrainPatchRenderGroups);
		m_sceneList.insert(patch);
		m_viewLevelmap[patch]=ESNVL_Auto;
	}

	void NullSceneGraph::LoadRiver( const TCHAR *szRiver,const TCHAR *szPatch )
	{
		SGRiverPatch *pRiver=new SGRiverPatch();
		pRiver->LoadRiverRes(szRiver);
		pRiver->LoadPatchRes(szPatch);
		m_sceneList.insert(pRiver);
		m_viewLevelmap[pRiver]=ESNVL_Auto;
	}

	void NullSceneGraph::LoadGrassPatch(const TCHAR* szFileName)
	{
		GrassPatch* patch=new GrassPatch;
		patch->LoadRes(szFileName);
		m_sceneList.insert(patch);
		m_viewLevelmap[patch]=ESNVL_Auto;
	}

	void NullSceneGraph::CreateSimpleTerrainPatch(Heightmap* pHMap,RECT& rc)
	{
		SGSimpleTerrainPatch* pPatch=new SGSimpleTerrainPatch;
		pPatch->CreateGeom(pHMap,rc);
		m_sceneList.insert(pPatch);
		m_viewLevelmap[pPatch]=ESNVL_Auto;
	}

	bool NullSceneGraph::SaveAsQuadTreeData(const TCHAR* szFileName)
	{
		QuadTreeDataBuilder builder;
		builder.Begin();

		for(set<SceneNode*>::iterator iter=m_sceneList.begin();
			iter!=m_sceneList.end();++iter)
		{
			SceneNode *pNode=(*iter);
			builder.AddNode(pNode);
		}

		int baseNodeID=0;
		return builder.End(szFileName,baseNodeID);
	}

	bool NullSceneGraph::SaveAsThreeQuadTreeData(const TCHAR* szSaveBasePath,const TCHAR* szMapName)
	{
		QuadTreeDataBuilder builders[3];
		int i;
		for(i=0;i<3;i++)
		{
			builders[i].Begin();
		}

		AABBox box;
		for(set<SceneNode*>::iterator iter=m_sceneList.begin();
			iter!=m_sceneList.end();++iter)
		{
			SceneNode *pNode=(*iter);

			//--地形块和河流块放到远景四叉树
			if(pNode->IS_STATIC_CLASS(TerrainPatch)
				||pNode->IS_STATIC_CLASS(SGRiverPatch))
			{
				builders[2].AddNode(pNode);
			}
			//--草放到近景四叉树
			else if(pNode->IS_STATIC_CLASS(GrassPatch))
			{
				builders[0].AddNode(pNode);
			}
			else
			{
				SNViewLevelIter vlIter=m_viewLevelmap.find(const_cast<SceneNode*>(pNode));
				if( vlIter!=m_viewLevelmap.end() )
				{
					switch(m_viewLevelmap[pNode])
					{
					case ESNVL_Auto:
						{
							pNode->GetBox(box);
							float size=Vec3Dist(box.min,box.max);
							if(size<1000.0f)
								builders[0].AddNode(pNode);
							else if(size<7000.0f)
								builders[1].AddNode(pNode);
							else
								builders[2].AddNode(pNode);
						}
						break;
					case ESNVL_Near:
						builders[0].AddNode(pNode);
						break;
					case ESNVL_Mid:
						builders[1].AddNode(pNode);
						break;
					case ESNVL_Far:
						builders[2].AddNode(pNode);
						break;
					}
				}
			}
		}

		TCHAR szPath[512];
		int baseNodeID=0;
		for(i=0;i<3;i++)
		{
			//--先删除现有的
			_stprintf_s(szPath,sizeof(szPath)/sizeof(TCHAR),_T("%s\\%s\\%s%d.fsg"),szSaveBasePath,szMapName,szMapName,i);
			::DeleteFile(szPath);

			if(builders[i].BuildGetNumNodes()>0)
			{
				if(!builders[i].End(szPath,baseNodeID))
					return false;
			}
		}
		
		return baseNodeID>0;
	}

	void NullSceneGraph::BuildProjectorMesh(const tagLight *pLight,const AABBox& box,DynamicMesh *pMesh)
	{
	}

	void NullSceneGraph::BuildDecalMesh( const AABBox& box,DynamicMesh *pMesh, const Vector3* pNormal )
	{
		for(set<SceneNode*>::iterator iter=m_sceneList.begin();
			iter!=m_sceneList.end();++iter)
		{
			SceneNode *pNode = (*iter);
			if( pNode->IS_STATIC_CLASS( TerrainPatch ) )
				( (TerrainPatch*)pNode )->BuildDecalMesh( box, pMesh, pNormal );
			if( pNode->IS_STATIC_CLASS( EntityNode ) )
				( (EntityNode*)pNode )->BuildDecalMesh( box,pMesh, pNormal );
		}

		for(set<SceneNode*>::iterator iter=m_dynaList.begin();
			iter!=m_dynaList.end();++iter)
		{
			SceneNode *pNode = (*iter);
			if( pNode->IS_STATIC_CLASS( TerrainPatch ) )
				( (TerrainPatch*)pNode )->BuildDecalMesh( box, pMesh, pNormal );
			if( pNode->IS_STATIC_CLASS( EntityNode ) )
				( (EntityNode*)pNode )->BuildDecalMesh( box,pMesh, pNormal );
		}
	}

	void NullSceneGraph::BuildShadowMapForTerrain(const TCHAR* szBasePath, int numPatchW, int numPatchH)
	{
	}

	TResult NullSceneGraph::RayCollide(const Ray& ray)
	{
		TResult ret(false,FLOAT_MAX);
		//临时的,只检测Terrain
		for(set<SceneNode*>::iterator iter=m_sceneList.begin();
			iter!=m_sceneList.end();++iter)
		{
			SceneNode *pNode=(*iter);
			if(pNode->IS_STATIC_CLASS(TerrainPatch))//project to terrain
			{
				TerrainPatch *pTrn=(TerrainPatch*)pNode;
				TResult t=pTrn->RayCollide(ray);
				if(t.first && t.second<ret.second)
				{
					ret=t;
				}
			}
		}
		return ret;
	}

	void NullSceneGraph::RenderReflectMap(CameraBase* pCamera,IRenderTexture* pRT,float surfaceHeight,bool bRenderTrnOnly)
	{
		ASSERT(pCamera && pRT);

		//--
		CameraBase* pOldCamera=m_pCamera;
		m_pCamera=pCamera;

		//--Update skynode
		SceneGraph::Render(0);

		//--
		vector<SceneNode*> sceneNodes;

		Frustum frustum;
		frustum.Extract(pCamera->GetView(),pCamera->GetPrj());

		if(bRenderTrnOnly)
		{
			for(set<SceneNode*>::iterator iter=m_sceneList.begin();
				iter!=m_sceneList.end();++iter)
			{
				SceneNode *pNode=(*iter);
				if(!pNode->IS_STATIC_CLASS(TerrainPatch)
					&& pNode->IsInFrustum(&frustum))
				{
					sceneNodes.push_back(pNode);
				}
			}
		}
		else
		{
			for(set<SceneNode*>::iterator iter=m_sceneList.begin();
				iter!=m_sceneList.end();++iter)
			{
				SceneNode *pNode=(*iter);
				if(!pNode->IS_STATIC_CLASS(SGRiverPatch)
					&& pNode->IsInFrustum(&frustum))
				{
					sceneNodes.push_back(pNode);
				}
			}		
		}

		//--
		RenderSys* pRenderSys=Kernel::Inst()->GetRenderSys();
		pRenderSys->RenderReflectMap(this,sceneNodes,pRT,surfaceHeight);

		//--
		m_pCamera=pOldCamera;
	}

	void NullSceneGraph::OpenStaticPL()
	{
		m_bShowStaticPL=true;

		for(SNVecIter iter=m_pointlights.begin();iter!=m_pointlights.end();++iter)
		{
			SGLightNode *pNode=(SGLightNode*)(*iter);
			if(!pNode->IsDynamic())
			{
				SNSetIter findIter=m_sceneList.find(pNode);
				if(findIter==m_sceneList.end())
				{
					m_sceneList.insert(*iter);
					m_viewLevelmap[pNode]=ESNVL_Auto;
				}
			}
		}
	}

	void NullSceneGraph::OpenDynamicPL()
	{
		m_bShowDynamicPL=true;

		for(SNVecIter iter=m_pointlights.begin();iter!=m_pointlights.end();++iter)
		{
			SGLightNode *pNode=(SGLightNode*)(*iter);
			if(pNode->IsDynamic())
			{
				SNSetIter findIter=m_sceneList.find(pNode);
				if(findIter==m_sceneList.end())
				{
					m_sceneList.insert(*iter);
					m_viewLevelmap[pNode]=ESNVL_Auto;
				}
			}
		}
	}

	void NullSceneGraph::CloseStaticPL()
	{
		m_bShowStaticPL=true;

		for(SNVecIter iter=m_pointlights.begin();iter!=m_pointlights.end();++iter)
		{
			SGLightNode *pNode=(SGLightNode*)(*iter);
			if(!pNode->IsDynamic())
			{
				SNSetIter findIter=m_sceneList.find(pNode);
				if(findIter!=m_sceneList.end())
				{
					m_sceneList.erase(findIter);

					SNViewLevelIter vlIter=m_viewLevelmap.find(const_cast<SGLightNode*>(pNode));
					if( vlIter!=m_viewLevelmap.end() )
						m_viewLevelmap.erase(vlIter);
				}
			}
		}
	}

	void NullSceneGraph::CloseDynamicPL()
	{
		m_bShowDynamicPL=true;

		for(SNVecIter iter=m_pointlights.begin();iter!=m_pointlights.end();++iter)
		{
			SGLightNode *pNode=(SGLightNode*)(*iter);
			if(pNode->IsDynamic())
			{
				SNSetIter findIter=m_sceneList.find(pNode);
				if(findIter!=m_sceneList.end())
				{
					m_sceneList.erase(*findIter);

					SNViewLevelIter vlIter=m_viewLevelmap.find(const_cast<SGLightNode*>(pNode));
					if( vlIter!=m_viewLevelmap.end() )
						m_viewLevelmap.erase(vlIter);
				}
			}
		}
	}

	void NullSceneGraph::Clear()
	{
		for(set<SceneNode*>::iterator iter=m_dynaList.begin();
			iter!=m_dynaList.end();
			iter++)
		{
			SceneNode *pEntity=(*iter);
			delete pEntity;
		}
		m_dynaList.clear();
		for(set<SceneNode*>::iterator iter=m_sceneList.begin();
			iter!=m_sceneList.end();++iter)
		{
			SceneNode *pNode=(*iter);
			delete pNode;
		}
		m_sceneList.clear();
	}
}//namespace Cool3D