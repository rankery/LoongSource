#include "StdAfx.h"
#include ".\SGVisitor.h"
#include "..\SceneGraph\HugeQuadTreeSG.h"
#include "..\SpecialEffect\SGEffectNode.h"
#include "..\SceneGraph\SceneNode.h"
#include ".\ISGVisitorCallBack.h"
#include "..\Kernel.h"

namespace Cool3D
{
	SGVisitor::SGVisitor(void)
	{
	}

	SGVisitor::~SGVisitor(void)
	{
	}

	bool SGVisitor::Start(const TCHAR* szSGFile,float visitorViewDist,float renderViewDist,ISGVisitorCallBack* pCallBack,DWORD param)
	{
		HugeQuadTreeSG visitorSG;
		HugeQuadTreeSG renderSG;

		visitorSG.LoadFromFile(Kernel::Inst()->DefaultFS(),szSGFile);
		renderSG.LoadFromFile(Kernel::Inst()->DefaultFS(),szSGFile);
			
		Vector3 visitorViewZone(visitorViewDist,2048*50.0f,visitorViewDist);
		visitorSG.SetViewZoneSize(HugeQuadTreeSG::EVZT_LitNode,visitorViewZone);
		visitorSG.SetViewZoneSize(HugeQuadTreeSG::EVZT_MidNode,visitorViewZone);
		visitorSG.SetViewZoneSize(HugeQuadTreeSG::EVZT_BigNode,visitorViewZone);
		visitorSG.SetViewZoneSize(HugeQuadTreeSG::EVZT_SimpleTerrain,visitorViewZone);

		Vector3 renderViewZone(renderViewDist,2048*50.0f,renderViewDist);
		renderSG.SetViewZoneSize(HugeQuadTreeSG::EVZT_LitNode,renderViewZone);
		renderSG.SetViewZoneSize(HugeQuadTreeSG::EVZT_MidNode,renderViewZone);
		renderSG.SetViewZoneSize(HugeQuadTreeSG::EVZT_BigNode,renderViewZone);
		renderSG.SetViewZoneSize(HugeQuadTreeSG::EVZT_SimpleTerrain,renderViewZone);

		CameraBase visitorCamera;
		float clientW=(float)Device()->GetClientWidth();
		float clientH=(float)Device()->GetClientHeight();
		visitorCamera.SetPerspective(clientW,clientH,FLOAT_PI/4,100.0f,2048*50.0f);
		visitorSG.BindCamera(&visitorCamera);

		CameraBase renderCamera;
		renderCamera.SetPerspective(clientW,clientH,FLOAT_PI/4,100.0f,2048*50.0f);
		renderSG.BindCamera(&renderCamera);

		set<int> idSet;
		
		int index=0;
		int totalNum=visitorSG.GetNumStaticNodes();
		AABBox box=visitorSG.GetBox();

		for(float z=box.min.z;z<box.max.z;z+=visitorViewDist)
		{
			for(float x=box.min.x;x<box.max.x;x+=visitorViewDist)
			{
				//--
				Vector3 lookAt;
				lookAt.x=x+visitorViewDist*0.5f;
				lookAt.y=box.max.y;
				lookAt.z=z+visitorViewDist*0.5f;
				SetCameraLookAt(visitorSG,visitorCamera,lookAt, false);

				//--
				vector<SceneNode*> staticNodes;
				INT nTotalNodes = visitorSG.GetNumStaticNodes();
				staticNodes.reserve(nTotalNodes);

				visitorSG.GetStaticNodes(staticNodes);
				INT nVisiableNode = staticNodes.size();
				for(int i=0; i<nVisiableNode; i++)
				{
					SceneNode* pNode=staticNodes[i];

					//--跳过特效元素
					if( pNode->IS_STATIC_CLASS(SGEffectNode) )
						continue;

					int id=pNode->GetStaticNodeID();
					set<int>::iterator iter=idSet.find(id);
					if(iter==idSet.end())
					{
						//--把镜头移到节点的包围盒中进行拍摄
						AABBox nodeBox;
						pNode->GetBox(nodeBox);
						Vector3 nodeLookAt=nodeBox.GetCenter();
						nodeLookAt.y=box.max.y;
						SetCameraLookAt(renderSG,renderCamera,nodeLookAt, true);

						if(!pCallBack->OnVisitSceneNode(&renderSG,pNode,index,totalNum,param))
						{
							return false;
						}

						idSet.insert(id);
						index++;
					}
				}
			}
		}
		
		return true;
	}

	void SGVisitor::SetCameraLookAt( HugeQuadTreeSG& sg,CameraBase& camera,Vector3& lookAt, bool bWait)
	{
		Vector3 lookdir(1,-1,1);
		Vector3 from=lookAt-lookdir;
		camera.SetLookAt(from,lookAt);

		sg.InvalidViewZone();
		sg.SyncInit();
/*
		if( bWait )
		{
			do 
			{
				bWait = false;
				vector<SceneNode*> vecNodes;
				INT nTotalNodes = sg.GetNumStaticNodes();
				vecNodes.reserve(nTotalNodes);

				sg.GetStaticNodes(vecNodes);
				INT nVisiableNode = vecNodes.size();
				for(int i=0; i<nVisiableNode; i++)
				{
					SceneNode* pNode=vecNodes[i];
					if( !pNode->IsLoaded() )
					{
						bWait = true;
						break;
					}
				}

				Sleep(200);
				sg.SyncInit();

			} while (bWait);
		}*/
	}
}//namespace Cool3D