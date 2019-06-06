#include "StdAfx.h"
#include ".\hugequadtreesg.h"
#include "..\VFS\IFS.h"
#include "..\Util\DebugMisc.h"
#include "..\RenderSys\RenderSys.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Kernel.h"
#include "..\math\Frustum.h"
#include "..\Device\IDevice.h"
#include "..\Device\IRenderTexture.h"
#include "..\Device\IRenderCubeTexture.h"
#include "..\Device\IRenderToEnvMap.h"
#include "..\Device\IDeviceVizQuery.h"
#include "..\Device\StateBlock.h"
#include "SGQuadTreeNode.h"
#include "AvatarNode.h"
#include "EntityNode.h"
#include "..\Device\RenderStatus.h"
#include "..\Util\SpeedAnalyser.h"
#include "..\RenderSys\WaterReflectMapRender.h"
#include "..\Terrain\TPatchLodTable.h"
#include "..\Terrain\TerrainPatch.h"
#include "..\Terrain\SGSimpleTerrain.h"
#include "..\Terrain\SGSimpleTerrainPatch.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(HugeQuadTreeSG,SceneGraph);

	HugeQuadTreeSG::HugeQuadTreeSG(void)
	{
		for(int i=0;i<EQTT_Num;i++)
			m_pRoot[i]=NULL;
		m_pSimpleTerrain=NULL;

		float sz=256.0f*50.0f;
		m_viewZoneSize[EVZT_LitNode]=Vector3(sz,sz,sz);
		sz=512.0f*50.0f;
		m_viewZoneSize[EVZT_MidNode]=Vector3(sz,sz,sz);
		sz=800.0f*50.0f;
		m_viewZoneSize[EVZT_BigNode]=Vector3(sz,sz,sz);
		sz=256.0f*50.0f;
		m_viewZoneSize[EVZT_DynamicNode]=Vector3(sz,sz,sz);
		sz=2000.0f*50.0f;
		m_viewZoneSize[EVZT_SimpleTerrain]=Vector3(sz,sz,sz);

		m_bLoaded=false;
	}

	HugeQuadTreeSG::~HugeQuadTreeSG(void)
	{
		for(int i=0;i<EQTT_Num;i++)
		{
			SAFE_DELETE(m_pRoot[i]);
			m_data[i].Close();
		}

		SAFE_DELETE(m_pSimpleTerrain);

		for(set<SceneNode*>::iterator iter=m_dynaList.begin();
			iter!=m_dynaList.end();
			iter++)
		{
			SceneNode *pEntity=(*iter);
			delete pEntity;
		}
		m_dynaList.clear();
	}

	void HugeQuadTreeSG::BindCamera(CameraBase *pCamera,const bool bSetDefaultFogDist/*=true*/)
	{
		SceneGraph::BindCamera(pCamera,bSetDefaultFogDist);
	}

	void HugeQuadTreeSG::LoadFromFile(IFS *pFS,const TCHAR* szMapName)
	{
		ASSERT(m_bLoaded==false);//防止重复加载

		TCHAR szPath[512];
		for(int i=0;i<EQTT_Num;i++)
		{
			_stprintf(szPath,_T("data\\map\\%s\\%s%d.fsg"),szMapName,szMapName,i);
			if(pFS->GetSize(szPath)!=-1)
			{
				m_data[i].OpenFromFile(pFS,szPath);
				m_pRoot[i]=m_data[i].LoadNode(0);
				ASSERT(m_pRoot[i]!=NULL);
			}
		}
	
		SAFE_DELETE(m_pSimpleTerrain);
		m_pSimpleTerrain=new SGSimpleTerrain;

		_stprintf(szPath,_T("data\\map\\%s\\trn\\simple_terrain.fsg"),szMapName);
		m_pSimpleTerrain->LoadQuadTree(pFS,szPath);

		m_bLoaded=true;
	}

	void HugeQuadTreeSG::AttachDynamicNode(SceneNode *pNode)
	{
		m_dynaList.insert(pNode);
	}

	void HugeQuadTreeSG::DetachDynamicNode(const SceneNode *pNode)
	{
		m_dynaList.erase(const_cast<SceneNode*>(pNode));
	}

	void HugeQuadTreeSG::SyncInit()
	{
		if(!m_bLoaded)
			return;

		BeginSyncInit();

		//--处理所有静态node
		for(int i=0;i<EQTT_Num;i++)
		{
			if(m_pRoot[i]!=NULL)
				m_pRoot[i]->ForceUpdate(this);
		}
		//--处理所有动态节点
		for(set<SceneNode*>::iterator iter=m_dynaList.begin();
			iter!=m_dynaList.end();
			iter++)
		{
			SceneNode *pNode=(*iter);
			pNode->ForceUpdate(0.0f,this);
		}

		EndSyncInit();
	}

	void HugeQuadTreeSG::Render(float deltaTime)
	{
		if(!m_bLoaded)
			return;


		BeginSpeedTest(_T("Cool3D::HugeQuadTreeSG::Render.1"));

		Frustum frustum[EVZT_Num];
		Matrix4 matPrj;
		int i;
		for(i=0;i<EVZT_Num;i++)
		{
			m_pCamera->GetPrj(m_viewZoneSize[i].z,matPrj);
			frustum[i].Extract(m_pCamera->GetView(),&matPrj);
		}
		
		RenderSys *pRender=Kernel::Inst()->GetRenderSys();
		pRender->Begin(this);

		TPatchLodTable::Inst()->Clear(TerrainPatch::MAX_LOD);
		TPatchLodTable::Inst_SimpleTerrain()->Clear(SGSimpleTerrainPatch::MAX_LOD);

		//--attach所有静态node
		WaterReflectMapRender::Inst()->Begin(this);
		for(int i=0;i<EQTT_Num;i++)
		{
			if(m_pRoot[i]!=NULL)
				m_pRoot[i]->AttachToRenderSys(this,pRender,&frustum[i],deltaTime,true);
		}

		WaterReflectMapRender::Inst()->End();

		m_pSimpleTerrain->Update(frustum[EVZT_SimpleTerrain],0,this);
		pRender->AttachNode(m_pSimpleTerrain);

		EndSpeedTest(_T("Cool3D::HugeQuadTreeSG::Render.1"));

		BeginSpeedTest(_T("Cool3D::HugeQuadTreeSG::Render.2"));

		//--attach所有动态节点
		AABBox viewZone;
		CalCurrentViewZone(EVZT_DynamicNode,viewZone);
		AABBox nodeBox;
		for(set<SceneNode*>::iterator iter=m_dynaList.begin();
			iter!=m_dynaList.end();
			iter++)
		{
			SceneNode *pNode=(*iter);

			pNode->ForceUpdate(deltaTime,this);
			if(pNode->IS_KIND_OF(AvatarNode))//todo:avatar为什么不行？
				nodeBox=viewZone;
			else
				pNode->GetBox(nodeBox);

			if(viewZone.Intersects(nodeBox) &&
				pNode->IsInFrustum(&frustum[EVZT_DynamicNode]))
			{
				pNode->Update(deltaTime,this);
				pRender->AttachNode(pNode);
			}
		}

		EndSpeedTest(_T("Cool3D::HugeQuadTreeSG::Render.2"));

		BeginSpeedTest(_T("Cool3D::HugeQuadTreeSG::Render.3"));

		SceneGraph::Render(deltaTime);
		pRender->End(this,true);

		EndSpeedTest(_T("Cool3D::HugeQuadTreeSG::Render.3"));
	}

	void HugeQuadTreeSG::SetViewZoneSize(EViewZoneType type,const Vector3& size)
	{
		m_viewZoneSize[type]=size;
	}

	const Vector3& HugeQuadTreeSG::GetViewZoneSize(EViewZoneType type)
	{
		return m_viewZoneSize[type];
	}

	void HugeQuadTreeSG::CalCurrentViewZone(EViewZoneType type,AABBox& out)
	{
		ASSERT(m_pCamera!=NULL);
		Vector3 zoneCenter=m_pCamera->GetLookat();
		out.max=zoneCenter+m_viewZoneSize[type];
		out.min=zoneCenter-m_viewZoneSize[type];
	}

	void HugeQuadTreeSG::InvalidViewZone()
	{
		if(!m_bLoaded)
			return;

		//--静态模型
		AABBox viewZone;
		for(int i=0;i<EQTT_Num;i++)
		{
			if(m_pRoot[i]!=NULL)
			{
				CalCurrentViewZone((EViewZoneType)i,viewZone);
				m_pRoot[i]->InvalidViewData(viewZone,&m_data[i]);
			}
		}
		//--远处地形
		CalCurrentViewZone(EVZT_SimpleTerrain,viewZone);
		m_pSimpleTerrain->InvalidViewZone(viewZone);
	}

	void HugeQuadTreeSG::BuildProjectorMesh(const tagLight *pLight,const AABBox& box,DynamicMesh *pMesh)
	{
		m_pRoot[EVZT_BigNode]->BuildProjectorMesh_Trn(box,pMesh);	
	}

	void HugeQuadTreeSG::BuildDecalMesh( const AABBox& box,DynamicMesh *pMesh, const Vector3* pNormal )
	{
		for( int nTree = 0; nTree < EQTT_Num; nTree ++ )
		{
			if( m_pRoot[nTree] )
			{
				//TCHAR szBuff[64];
				//_sntprintf( szBuff, 64, _T("HugeQuadTreeSG::BuildDecalMesh.%d"), nTree );
				//BeginSpeedTest( szBuff );

				m_pRoot[nTree]->BuildDecalMesh( box, pMesh, pNormal );

				//EndSpeedTest( szBuff );
			}
		}
	}

	AABBox HugeQuadTreeSG::GetBox() const
	{	
		return m_pRoot[EVZT_BigNode]->GetBox();
	}

	//测试视点是否包含在物体中以及遮挡视点的物体半透明(输入参数为当前视点位置和上一次的视点位置以及看位置，返回为与视点碰撞的物体的包围盒)
	bool HugeQuadTreeSG::TestContainBetweenViewPointAndSceneObject(Vector3 vFrom,Vector3 vOldFrom,Vector3 vLookAt,Cool3D::AABBox& box)
	{
		if(!m_bLoaded)
			return false;

		for(int i=0;i<EQTT_Num;i++)
		{
			if(m_pRoot[i]==NULL)
				continue;
			if(m_pRoot[i]->TestContainBetweenViewPointAndSceneObject(vFrom,vOldFrom,vLookAt,box))
				return true;
		}
		return false;
	}

	TResult HugeQuadTreeSG::RayCollide(const Ray& ray)
	{
		TResult result(false,0);
		TResult lastTR(false,FLOAT_MAX);
		if(m_bLoaded)
		{
			for(int i=0;i<EQTT_Num;i++)
			{
				if(m_pRoot[i]==NULL)
					continue;
				result=m_pRoot[i]->RayCollide(ray);
				if(result.first
					&& result.second<lastTR.second)
					lastTR=result;
			}
		}
		return lastTR;
	}

	void HugeQuadTreeSG::RenderReflectMap(CameraBase* pCamera,IRenderTexture* pRT,float surfaceHeight,bool bRenderTrnOnly)
	{
		ASSERT(pCamera && pRT);

		if(!m_bLoaded)
			return;

		//--
		CameraBase* pOldCamera=m_pCamera;
		m_pCamera=pCamera;

		//--update skynode
		SceneGraph::Render(0);

		//--scene nodes
		Frustum frustum[EVZT_Num];
		Matrix4 matPrj;
		for(int i=0;i<EVZT_Num;i++)
		{
			m_pCamera->GetPrj(m_viewZoneSize[i].z,matPrj);
			frustum[i].Extract(m_pCamera->GetView(),&matPrj);
		}

		vector<SceneNode*> sceneNodes;
		for(int i=0;i<EQTT_Num;i++)
		{
			if(m_pRoot[i]==NULL)
				continue;
			m_pRoot[i]->GetSceneNodes(&frustum[i],sceneNodes,bRenderTrnOnly,true);
		}

		//--simple terrain
		m_pSimpleTerrain->Update(frustum[EVZT_SimpleTerrain],0,this);
		sceneNodes.push_back(m_pSimpleTerrain);

		//--render
		RenderSys* pRenderSys=Kernel::Inst()->GetRenderSys();
		pRenderSys->RenderReflectMap(this,sceneNodes,pRT,surfaceHeight,true);

		//--
		m_pCamera=pOldCamera;
	}

	void HugeQuadTreeSG::SetSceneFog(const tagDistFog& fog)
	{
		SceneGraph::SetSceneFog(fog);
		m_pSimpleTerrain->SetColor(fog.color);
	}

	DWORD HugeQuadTreeSG::GetNumStaticNodes()
	{
		if(!m_bLoaded)
			return 0;

		int numNodes=0;
		for(int i=0;i<EQTT_Num;i++)
		{
			numNodes+=m_data[i].GetNumNode();
		}
		return numNodes;
	}

	void HugeQuadTreeSG::GetStaticNodes(vector<SceneNode*>& sceneNodes)
	{
		if(!m_bLoaded)
			return;

		for(int i=0;i<EQTT_Num;i++)
		{
			if(m_pRoot[i]==NULL)
				continue;
			m_pRoot[i]->GetSceneNodes(sceneNodes);
		}
	}

	bool HugeQuadTreeSG::RenderForRayCollide(Vector3& origin,Vector3& dir,float dist,IRenderTexture* pRenderTex,IDeviceVizQuery* pVizQuery)
	{
		if(!m_bLoaded)
			return false;

		CameraBase* pOldCamera=GetCamera();
    
		CameraBase camera;
		Vector3 lookat=origin+dir;
		camera.SetLookAt(origin,lookat);
		camera.SetOrtho(0.1f,0.1f,10.0f,dist);
		BindCamera(&camera);

		Frustum frustum;
		frustum.Extract(camera.GetView(),camera.GetPrj());

		TPatchLodTable::Inst()->Clear(TerrainPatch::MAX_LOD);
		RenderSys* pSys=Kernel::Inst()->GetRenderSys();

		StateBlock block;
		block.SaveState(ERS_CullMode);	Device()->SetCullMode(ECull_None);

		pRenderTex->Begin();
			Device()->Clear(true,true,true,0,1,0);
			pVizQuery->Begin();
				pSys->Begin(this);
				for(int i=0;i<EQTT_Num;i++)
				{
					if(m_pRoot[i]!=NULL)
					{
						m_pRoot[i]->AttachToRenderSysForShadowMap(this,pSys,&frustum,0,true);
					}
				}
				pSys->End(this);
			pVizQuery->End();
		pRenderTex->End();

		block.Apply();

		BindCamera(pOldCamera);

		return pVizQuery->GetResultSync()==0;
	}

	void HugeQuadTreeSG::RenderHemisphere(Vector3& eye,Vector3& dir,float dist,IRenderTexture* pRenderTex,IRenderToEnvMap* pRenderToEnvMap)
	{
		if(!m_bLoaded)
			return;

		CameraBase* pOldCamera=GetCamera();

		CameraBase camera;
		camera.SetPerspective((float)pRenderTex->Width(),(float)pRenderTex->Height(),FLOAT_PI/2,0.1f,dist);

		TPatchLodTable::Inst()->Clear(TerrainPatch::MAX_LOD);

		RenderSys* pSys=Kernel::Inst()->GetRenderSys();
		IDevice* pDev=Device();

		pRenderToEnvMap->BeginHemisphere(pRenderTex);
		for(int face=0;face<6;face++)
		{
			pRenderToEnvMap->Face(face);
			pDev->Clear(true,true,true,0,1.0f,0);

			Vector3 lookAt=eye+dir*10.0f;
			camera.SetLookAt(eye,lookAt,face);
			BindCamera(&camera);

			Frustum frustum;
			frustum.Extract(camera.GetView(),camera.GetPrj());

			pSys->Begin(this);
			for(int i=0;i<EQTT_Num;i++)
			{
				if(m_pRoot[i]!=NULL)
					m_pRoot[i]->AttachToRenderSys(this,pSys,&frustum,0,true);
			}
			pSys->End(this);	
		}
		pRenderToEnvMap->End();
		
		BindCamera(pOldCamera);
	}

	void HugeQuadTreeSG::RenderShadowMap( CameraBase& camera,IRenderTexture* pRenderTex )
	{
		if(!m_bLoaded)
			return;

		CameraBase* pOldCamera=GetCamera();
		BindCamera(&camera);

		Frustum frustum;
		frustum.Extract(camera.GetView(),camera.GetPrj());

		TPatchLodTable::Inst()->Clear(TerrainPatch::MAX_LOD);

		vector<SceneNode*> sceneNodes;
		for(int i=0;i<EQTT_Num;i++)
		{
			if(m_pRoot[i]==NULL)
				continue;
			m_pRoot[i]->GetSceneNodes(&frustum,sceneNodes,false,true,true);
		}

		//--render
		RenderSys* pRenderSys=Kernel::Inst()->GetRenderSys();
		pRenderSys->RenderShadowMap(this,sceneNodes,pRenderTex);

		BindCamera(pOldCamera);
	}

	void HugeQuadTreeSG::RenderCubeShadowMap( CameraBase& camera,IRenderCubeTexture* pRenderTex )
	{
		if(!m_bLoaded)
			return;

		CameraBase* pOldCamera=GetCamera();
		BindCamera(&camera);

		Frustum frustum;
		frustum.Extract(camera.GetView(),camera.GetPrj());

		TPatchLodTable::Inst()->Clear(TerrainPatch::MAX_LOD);

		vector<SceneNode*> sceneNodes;
		for(int i=0;i<EQTT_Num;i++)
		{
			if(m_pRoot[i]==NULL)
				continue;
			m_pRoot[i]->GetSceneNodes(&frustum,sceneNodes,false,true,true);
		}

		//--render
		RenderSys* pRenderSys=Kernel::Inst()->GetRenderSys();
		pRenderSys->RenderCubeShadowMap(this,sceneNodes,pRenderTex);

		BindCamera(pOldCamera);
	}
}//namespace Cool3D