#include "StdAfx.h"
#include "..\SpeedTreeRT\include\SpeedTreeRT.h"

#include "..\Math\GeomDefine.h"
#include ".\restree.h"
#include "..\Util\Exception.h"
#include "..\VFS\IFS.h"
#include "..\Kernel.h"
#include "..\RenderSys\TreeNode.h"

namespace F3D
{
	IMP_RTTI(ResTree,ResRenderable);

	class ResTree::Member
	{
	public:
		CSpeedTreeRT	*m_pSpeedTree;
		AABBox			m_box;
		TreeNode		*m_pTreeNode;

		Member()
		{
			m_pSpeedTree=NULL;
			m_pTreeNode=NULL;
		}
		~Member()
		{
			SAFE_DELETE(m_pTreeNode);
			SAFE_DELETE(m_pSpeedTree);
		}

		void SetupOption()
		{
			const F3DOption& option=Kernel::Inst()->GetOption();
			BYTE treeDetail=3;
			switch(treeDetail)
			{
			case 3:
				m_pSpeedTree->SetBranchLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
				m_pSpeedTree->SetLeafLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
				m_pSpeedTree->SetFrondLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);

				m_pSpeedTree->SetBranchWindMethod(CSpeedTreeRT::WIND_CPU);
				m_pSpeedTree->SetLeafWindMethod(CSpeedTreeRT::WIND_CPU);
				m_pSpeedTree->SetFrondWindMethod(CSpeedTreeRT::WIND_CPU);
				break;
			case 2:
			case 1:
				m_pSpeedTree->SetBranchLightingMethod(CSpeedTreeRT::LIGHT_STATIC);
				m_pSpeedTree->SetLeafLightingMethod(CSpeedTreeRT::LIGHT_STATIC);
				m_pSpeedTree->SetFrondLightingMethod(CSpeedTreeRT::LIGHT_STATIC);

				m_pSpeedTree->SetBranchWindMethod(CSpeedTreeRT::WIND_CPU);
				m_pSpeedTree->SetLeafWindMethod(CSpeedTreeRT::WIND_CPU);
				m_pSpeedTree->SetFrondWindMethod(CSpeedTreeRT::WIND_CPU);
				break;

			default:
				m_pSpeedTree->SetBranchLightingMethod(CSpeedTreeRT::LIGHT_STATIC);
				m_pSpeedTree->SetLeafLightingMethod(CSpeedTreeRT::LIGHT_STATIC);
				m_pSpeedTree->SetFrondLightingMethod(CSpeedTreeRT::LIGHT_STATIC);

				m_pSpeedTree->SetBranchWindMethod(CSpeedTreeRT::WIND_NONE);
				m_pSpeedTree->SetLeafWindMethod(CSpeedTreeRT::WIND_NONE);
				m_pSpeedTree->SetFrondWindMethod(CSpeedTreeRT::WIND_NONE);
			}
		}

		void LoadFromFile(IFS *pFS,const char* szName)
		{
			//--load file to memory
			ASSERT(pFS!=NULL);
			UINT fileSize=pFS->GetSize(szName);
			ASSERT(fileSize > 0);
			BYTE *pFileBuf=(BYTE *)malloc(fileSize);
			ASSERT(pFileBuf!=NULL);
			pFS->LoadFile(pFileBuf,szName);

			//--load speed tree
			m_pSpeedTree=new CSpeedTreeRT;
			m_pSpeedTree->SetTextureFlip(true);
			bool bLoadTreeOk=m_pSpeedTree->LoadTree(pFileBuf,fileSize);
			ASSERT(bLoadTreeOk);

			//--free memory
			free(pFileBuf);

			SetupOption();//ÉèÖÃwind,ligtht

			m_pSpeedTree->SetNumLeafRockingGroups(1);

			bool bComputeTreeOk=m_pSpeedTree->Compute(NULL);
			ASSERT(bComputeTreeOk);

			// make the leaves rock in the wind
			m_pSpeedTree->SetLeafRockingState(true);

			m_pSpeedTree->GetBoundingBox((float*)&m_box);

			// adjust lod distances
			float c_fNearLodFactor=1.0f;
			float c_fFarLodFactor=1.0f;
			float fHeight = m_box.max.y-m_box.min.y;;
			m_pSpeedTree->SetLodLimits(fHeight * c_fNearLodFactor, fHeight * c_fFarLodFactor);
		}

		void CreateRenderNode(const char* szResName)
		{
			m_pTreeNode=new TreeNode;
			m_pTreeNode->Create(m_pSpeedTree,szResName);
		}
	};

	//--
	ResTree::ResTree(const char* szName) : ResRenderable(szName)
	{
		m_p=new Member;
	}

	ResTree::~ResTree(void)
	{
		delete m_p;
	}

	
	UINT ResTree::CreateContent(DWORD param)
	{
		m_p->LoadFromFile(GetSafeFS(),GetName());
		m_p->CreateRenderNode(GetName());
		return ResBase::CreateContent(param);
	}
	
	void ResTree::SimpleDraw()
	{}

	UINT ResTree::GetNumRenderNode() const
	{
		if(ResBase::IsCreated())
			return 1;
		else
			return 0;
	}

	RenderNode *ResTree::GetRenderNode(UINT i) const
	{
		return m_p->m_pTreeNode;
	}

	const AABBox	*ResTree::GetAABBox() const
	{
		return &m_p->m_box;
	}

}//namespace F3D