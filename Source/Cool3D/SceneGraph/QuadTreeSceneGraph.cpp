#include "StdAfx.h"
#include ".\QuadTreeSceneGraph.h"
#include "..\Util\DebugMisc.h"
#include "..\RenderSys\RenderSys.h"
#include "..\Kernel.h"
#include "QuadTreeNode.h"
#include "..\math\Frustum.h"
#include "..\Device\IDevice.h"
#include "EntityNode.h"

namespace F3D
{
	QuadTreeSceneGraph::QuadTreeSceneGraph(void)
	{
		m_pRoot=NULL;
	}

	QuadTreeSceneGraph::~QuadTreeSceneGraph(void)
	{
		SAFE_DELETE(m_pRoot);
	}

	void QuadTreeSceneGraph::LoadFromFile()
	{}

	EntityNode *QuadTreeSceneGraph::CreateEntityNode(const char* szResName)
	{
		EntityNode *pNewNode=new EntityNode;
		pNewNode->LoadRes(szResName);
		m_entityList.push_back(pNewNode);
		return pNewNode;
	}

	void QuadTreeSceneGraph::DestroyEntityNode(EntityNode *pNode)
	{
		m_entityList.remove(pNode);
		delete pNode;
	}

	void QuadTreeSceneGraph::Update(float deltaTime)
	{}

	void QuadTreeSceneGraph::Render()
	{
		Frustum frustum;
		Matrix4 *pView=Device()->GetMatrix(EMS_View);
		Matrix4 *pPrj=Device()->GetMatrix(EMS_Prj);
		frustum.Extract(pView,pPrj);

		RenderSys *pRender=Kernel::Inst()->GetRenderSys();
		pRender->Begin(this);
		
		AddNodeToRenderSysR(m_pRoot,pRender,&frustum);

		for(list<EntityNode*>::iterator iter=m_entityList.begin();
			iter!=m_entityList.end();
			iter++)
			pRender->AttachNode((*iter));

		pRender->End();
	}

	void QuadTreeSceneGraph::AddNodeToRenderSysR(QuadTreeNode* pNode,RenderSys *pSys,Frustum *pFrustum)
	{
		if(pNode == NULL)
			return;
		if(!pNode->IsInFrustum(pFrustum))
			return;
		//pSys->AttachNode(pNode);

		for(UINT i=0;i<4;i++)
		{
			QuadTreeNode *pChild=pNode->GetChild(i);
			if(pChild != NULL)
				AddNodeToRenderSysR(pChild,pSys,pFrustum);
		}
	}
}//namespace F3D