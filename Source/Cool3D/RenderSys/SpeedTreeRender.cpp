#include "StdAfx.h"
#include "..\SpeedTreeRT\include\SpeedTreeRT.h"

#include ".\speedtreerender.h"
#include "..\Util\Exception.h"
#include "TreeNode.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Device\StateBlock.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\SceneGraph\EntityNode.h"
#include "..\Math\CameraBase.h"
#include "..\Math\Transform.h"

namespace F3D
{
	bool SpeedTreeRender::s_bWindEnable=true;
	const	int		c_nNumWindMatrices = 4;                             // number of unique wind matrices

	SpeedTreeRender::SpeedTreeRender(void)
	{
		m_fWindStrength=0.3f;
	}

	SpeedTreeRender::~SpeedTreeRender(void)
	{
	}

	void SpeedTreeRender::Clear()
	{
		m_trees.clear();
	}

	void SpeedTreeRender::Init()
	{
		CSpeedTreeRT::SetNumWindMatrices(c_nNumWindMatrices);
	}

	void SpeedTreeRender::AttachTree(const RenderItem& item)
	{
		size_t i;
		for(i=0;i<m_trees.size();i++)
		{
			TreeType& treeType=m_trees[i];
			ASSERT(treeType.size() > 0);//必须至少有一个树
			if(item.pNode == treeType[0].pNode)
			{
				treeType.push_back(item);
				return;
			}
		}

		//--make a new tree type
		TreeType newType;
		newType.push_back(item);
		m_trees.push_back(newType);
	}

	void SpeedTreeRender::RenderAllTrees(SceneGraph *pSG)
	{
		CameraBase *pCamera=pSG->GetCamera();
		Vector3 ldir=pCamera->GetDirection();
		Vector3 lpos=pCamera->GetPos();
		Vector4 dir,pos;
		
		//因为我们使用LH,Speed使用RH,所以需要将镜头的参数转换到RH
		D3DXVec3Transform(&dir,&ldir,&RenderSys::LH2RHMat);
		D3DXVec3Transform(&pos,&lpos,&RenderSys::LH2RHMat);

		CSpeedTreeRT::SetCamera((float*)&pos,(float*)&dir);
		CSpeedTreeRT::SetTime(Kernel::Inst()->GetAccumTime());

		IDevice *pDev=Device();		

		pDev->FSetTextureColorOp(0,ETOP_SelectArg1,ETA_Texture,ETA_Diffuse);
		pDev->FSetTextureColorOp(1,ETOP_Mul,ETA_Texture,ETA_Current);

		StateBlock stateBlock;
		stateBlock.SaveState(ERS_CullMode);

		if(s_bWindEnable)
			UpdateWind();

		size_t i;
		for(i=0;i<m_trees.size();i++)
		{
			TreeType& treeType=m_trees[i];
			RenderTreeType(treeType);
		}
		
		stateBlock.Apply();
	}

	void SpeedTreeRender::RenderTreeType(TreeType& treeType)
	{
		ASSERT(treeType.size() > 0);
		RenderItem& item=treeType[0];
		TreeNode *pTree=static_cast<TreeNode *>(item.pNode);

		//--一类树唯一的计算
		pTree->UpdateBuffers();

		//--画出每棵数
		int lod=item.nLod*16;

		IDevice *pDev=Device();
		size_t numTree=treeType.size();

		//--draw branch
		pDev->SetCullMode(ECull_CCW);
		pTree->BeginBranch();
		for(size_t i=0;i<numTree;i++)
		{
			item=treeType[i];
			pDev->MatLoad(EMS_World,&item.pSGNode->GetWorldMat());
			pDev->FFlushMatrix();

			pTree->DrawBranch(lod);
		}
		pTree->EndBranch();

		//--draw frond
		pDev->SetCullMode(ECull_None);
		if(pTree->BeginFrond())
		{
			for(size_t i=0;i<numTree;i++)
			{
				item=treeType[i];
				pDev->MatLoad(EMS_World,&item.pSGNode->GetWorldMat());
				pDev->FFlushMatrix();
				pTree->DrawFrond(lod);
			}
			pTree->EndFrond();
		}

		//--draw leafs
		pDev->SetCullMode(ECull_None);
		for(size_t i=0;i<numTree;i++)
		{
			item=treeType[i];
			pDev->MatLoad(EMS_World,&item.pSGNode->GetWorldMat());
			pDev->FFlushMatrix();
			pTree->DrawLeafs(lod);
		}

		pTree->EndLeafsTreeType();
	}

	void SpeedTreeRender::UpdateWind()
	{
		// wind
		const   int     c_nNumMatricesPerTree = 3;                          // number of individual matrices used per tree
		const	float	c_fShimmerSpeed = 50.0f;							// controls how fast the shimmer map rotates
		const	float	c_fShimmerExponent = 3.0f;							// controls linearity of shimmer speed

		// matrix computational data
		static float afMatrixTimes[c_nNumWindMatrices] = { 0.0f };
		static float afFrequencies[c_nNumWindMatrices][2] = 
		{
			{ 0.15f, 0.17f },
			{ 0.25f, 0.15f },
			{ 0.19f, 0.05f },
			{ 0.15f, 0.22f }
		};

		float fTimeSinceLastCall = Kernel::Inst()->GetDeltaTime();

		// wind strength
		static float fOldStrength = m_fWindStrength;

		// increment matrix times
		for (int i = 0; i < c_nNumWindMatrices; ++i)
			afMatrixTimes[i] += fTimeSinceLastCall;
		
		// compute maximum branch throw
		float fBaseAngle = m_fWindStrength * 35.0f;

		// build rotation matrices
		for (i = 0; i < c_nNumWindMatrices; ++i)
		{
			// adjust time to prevent "jumping"
			if (m_fWindStrength != 0.0f)
				afMatrixTimes[i] = (afMatrixTimes[i] * fOldStrength) / m_fWindStrength;

			// compute percentages for each axis
			float fBaseFreq = m_fWindStrength * 20.0f;
			float fXPercent = sinf(fBaseFreq * afFrequencies[i % c_nNumWindMatrices][0] * afMatrixTimes[i]);
			float fYPercent = cosf(fBaseFreq * afFrequencies[i % c_nNumWindMatrices][1] * afMatrixTimes[i]);

			// build compound rotation matrix (rotate on 'x' then on 'y')
			const float c_fDeg2Rad = 57.2957795f;
			float fSinX = sinf(fBaseAngle * fXPercent / c_fDeg2Rad);
			float fSinY = sinf(fBaseAngle * fYPercent / c_fDeg2Rad);
			float fCosX = cosf(fBaseAngle * fXPercent / c_fDeg2Rad);
			float fCosY = cosf(fBaseAngle * fYPercent / c_fDeg2Rad);

			float afMatrix[16] = { 0.0f };
			afMatrix[0] = fCosY;
			afMatrix[2] = -fSinY;
			afMatrix[4] = fSinX * fSinY;
			afMatrix[5] = fCosX;
			afMatrix[6] = fSinX * fCosY;
			afMatrix[8] = fSinY * fCosX;
			afMatrix[9] = -fSinX;
			afMatrix[10] = fCosX * fCosY;
			afMatrix[15] = 1.0f;

			CSpeedTreeRT::SetWindMatrix(i, afMatrix);
		}

		// track wind strength
		fOldStrength = m_fWindStrength;
	}
}//namespace F3D