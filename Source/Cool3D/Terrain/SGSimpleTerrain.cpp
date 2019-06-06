#include "StdAfx.h"
#include ".\SGSimpleTerrain.h"
#include ".\RSimpleTerrain.h"
#include "..\SceneGraph\QuadTreeData.h"
#include "..\SceneGraph\SGQuadTreeNode.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\Math\CameraBase.h"
#include "..\Math\Frustum.h"
#include ".\SGSimpleTerrainPatch.h"
#include "..\Util\SpeedAnalyser.h"
#include ".\SimpleTerrainMtl.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(SGSimpleTerrain,SceneNode);

	SGSimpleTerrain::SGSimpleTerrain(void)
	{
		m_pMtl=new SimpleTerrainMtl;
		m_pRoot=NULL;
		m_pRenderNode=new RSimpleTerrain;
	}

	SGSimpleTerrain::~SGSimpleTerrain(void)
	{
		SAFE_DELETE(m_pMtl);
		SAFE_DELETE(m_pRoot);
		SAFE_DELETE(m_pRenderNode);
		m_data.Close();
	}

	void SGSimpleTerrain::Serialize(FArchive& ar)
	{
		SceneNode::Serialize(ar);
	}
	
	void SGSimpleTerrain::Deserialize(FArchive& ar)
	{
		SceneNode::Deserialize(ar);
	}

	bool SGSimpleTerrain::IsInFrustum(const Frustum *pFrumstum) const
	{
		return true;
	}

	void SGSimpleTerrain::LoadRes(const TCHAR* szName)
	{
	}

	UINT SGSimpleTerrain::GetNumRenderNode() const
	{
		return 1;
	}

	RenderNode* SGSimpleTerrain::GetRenderNode(UINT i) const
	{
		return m_pRenderNode;
	}

	const Matrix4& SGSimpleTerrain::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGSimpleTerrain::GetBox(AABBox& out) const
	{
		out=MathConst::BigBox;
	}

	IMaterial*	SGSimpleTerrain::GetMtl(int mtlID)
	{
		return m_pMtl;
	}

	void SGSimpleTerrain::Update(float deltaTime,SceneGraph *pSG)
	{
	}

	void SGSimpleTerrain::Update(Frustum& frustum,float deltaTime,SceneGraph *pSG)
	{
		//--
		m_visNodes.clear();
		m_pRoot->GetSceneNodes(&frustum,m_visNodes,false,false);
		for(int i=0;i<(int)m_visNodes.size();i++)
		{
			SceneNode* pNode=m_visNodes[i];
			pNode->Update(deltaTime,pSG);
		}
	}

	void SGSimpleTerrain::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
	}

	void SGSimpleTerrain::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT(0 && _T("cann't set world transform to a terrain patch"));
	}

	void SGSimpleTerrain::LoadQuadTree(IFS* pFS,const TCHAR* szFileName)
	{
		m_data.Close();
		SAFE_DELETE(m_pRoot);

		m_data.OpenFromFile(pFS,szFileName);
		m_pRoot=m_data.LoadNode(0);
		ASSERT(m_pRoot!=NULL);
	}

	void SGSimpleTerrain::InvalidViewZone(const AABBox& viewZone)
	{
		m_pRoot->InvalidViewData(viewZone,&m_data);
	}

	void SGSimpleTerrain::SetColor(const Color4ub& color)
	{
		if(m_pMtl->m_color!=color)
		{
			m_pMtl->m_color=color;
			m_pMtl->InvalidateRenderCache();
		}
	}


}//namespace Cool3D                      