#include "StdAfx.h"
#include ".\rendersys.h"
#include "RenderNode.h"
#include "..\Util\Exception.h"
#include "..\Util\MemPool.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Device\IIndexBuffer.h"
#include "..\Device\IVertBuffer.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\SceneGraph\EntityNode.h"
#include "..\SceneGraph\SGLightNode.h"
#include "..\SceneGraph\AvatarNode.h"
#include "..\PostProcess\PostProcess.h"
#include "..\PostProcess\PostProcessHeatHaze.h"
#include "..\PostProcess\PostProcessBlur.h"

#include "HardwareBufferPool.h"

namespace Cool3D
{
	//--
	IMP_RTTI(RenderSys,RTTIObj);

	tagRenderColor	RenderSys::WhiteRenderColor;

	class SortByDistFun
	{
	public:
		SceneNode *m_pNode;

		SortByDistFun(SceneNode *pNode):m_pNode(pNode)
		{}

		bool operator()(SGLightNode *pLight1,SGLightNode *pLight2)
		{
			Vector3 light1=pLight1->GetLight()->pos;
			Vector3 light2=pLight2->GetLight()->pos;

			AABBox box;
			m_pNode->GetBox(box);
			Vector3 pos=box.GetCenter();

			float dist1=D3DXVec3Length( &(light1-pos) );
			float dist2=D3DXVec3Length( &(light2-pos) );

			return dist1<dist2;
		}
	};

	//--Member class
	class RenderSys::Member
	{
	public:
		HardwareBufferPool<DynaVBAlloc>		m_vbPool;
		HardwareBufferPool<DynaIBAlloc>		m_ibPool;

		vector<SceneNode*>		m_sceneNodes;
		vector<SGLightNode*>	m_lightNodes;

	public:
		Member()	{}
		~Member()
		{
#if 1
			tstring szRep;
			m_vbPool.Report(szRep,false);
			LOG(_T("--RenderSys VB::"));
			LOG(szRep.c_str());
			m_vbPool.Destroy();

			m_ibPool.Report(szRep,false);
			LOG(_T("--RenderSys IB::"));
			LOG(szRep.c_str());
			m_ibPool.Destroy();
#endif
		}
	};
	//--class RenderSys
	RenderSys::RenderSys(void)
	{
		m_p=new Member;

		WhiteRenderColor.diffuse=Color4f(1,1,1,1);
		WhiteRenderColor.ambient=Color4f(1,1,1,1);
		WhiteRenderColor.specular=Color4f(1,0.3f,0.3f,0.3f);
		WhiteRenderColor.emissive=Color4f(0,0,0,0);
		WhiteRenderColor.power=8;

		m_bIgnoreSGLights=false;
		m_pPostProcessHeatHaze = NULL;
		m_pPostProcessBlur=NULL;
		m_bEnablePostProcess=true;
	}

	RenderSys::~RenderSys(void)
	{
		delete m_p;

		ClearAllPostProcess();
	}

	void RenderSys::InitDevice()
	{
		//--vbpool
		vector<int> cfg;
		int i;
		for(i=0;i<16;i++)
			cfg.push_back(64*1024);
		for(i=0;i<8;i++)
			cfg.push_back(128*1024);
		for(i=0;i<4;i++)
			cfg.push_back(256*1024);
		for(i=0;i<4;i++)
			cfg.push_back(512*1024);
		m_p->m_vbPool.Init(cfg);

		//--ibpool
		cfg.clear();
		for(i=0;i<16;i++)
			cfg.push_back(8*1024);
		for(i=0;i<8;i++)
			cfg.push_back(16*1024);
		for(i=0;i<4;i++)
			cfg.push_back(64*1024);
		m_p->m_ibPool.Init(cfg);

	}

	void RenderSys::Begin(SceneGraph *pSG)
	{
		m_p->m_lightNodes.clear();
		m_p->m_sceneNodes.clear();
	}

	void RenderSys::End(SceneGraph *pSG,bool bZScale)
	{
	}

	AnimationCtrl *RenderSys::GetAniCtrlFromSGNode(const SceneNode *pNode)
	{
		AnimationCtrl *pAniCtrl=NULL;
		if(pNode->IS_KIND_OF(EntityNode))
		{
			const EntityNode *pEntity=static_cast<const EntityNode *>(pNode);
			pAniCtrl=pEntity->GetAniCtrl();
		}
		if(pNode->IS_KIND_OF(AvatarNode))
		{
			const AvatarNode* pAvatar=static_cast<const AvatarNode*>(pNode);
			pAniCtrl=(AnimationCtrl*)(const_cast<AvatarNode*>(pAvatar))->GetSkeletonAniCtrl();
		}
		return pAniCtrl;
	}

	void RenderSys::BuildRenderItems(SceneGraph *pSG)
	{
		vector<SGLightNode*> lgts;
		for(vector<SceneNode*>::iterator iter=m_p->m_sceneNodes.begin();
			iter!=m_p->m_sceneNodes.end();++iter)
		{
			SceneNode* pSGNode=(*iter);
			AssignLights(pSGNode,lgts);
			
			SortByDistFun sortObj(pSGNode);
			sort(lgts.begin(),lgts.end(),sortObj);
			
			AnimationCtrl *pAniCtrl=GetAniCtrlFromSGNode(pSGNode);
			int numRenderNodes=pSGNode->GetNumRenderNode();
			for(int i=0;i<numRenderNodes;i++)
			{
				RenderNode *pRender=pSGNode->GetRenderNode(i);
				if(pRender==NULL)
					continue;
				int nLod=0;
				if(pRender->SupportLod())
					nLod=pSG->CalcDetailLevel(pRender->GetAABBox(),&pSGNode->GetWorldMat());

				RenderItem tmp;
				tmp.pSGNode=pSGNode;
				tmp.pNode=pRender;
				tmp.pAni=pAniCtrl;
				tmp.nLod=nLod;
				int numLgt=(int)lgts.size();
				for(int j=0;j<numLgt && j<RMax_Light;j++)
				{
					tmp.pLights[j]=lgts[j]->GetLight();
					tmp.lightModulus[j]=lgts[j]->GetLightModulus();
				}
				AddRenderItem(tmp);
			}
		}
	}
	
	void RenderSys::AssignLights(const SceneNode* pNode,vector<SGLightNode*>& lgts)
	{
		AABBox box;
		pNode->GetBox(box);
		lgts.clear();
		for(vector<SGLightNode*>::iterator iter=m_p->m_lightNodes.begin();
			iter!=m_p->m_lightNodes.end();++iter)
		{
			SGLightNode *pLgt=(*iter);
			if(pLgt->LightCost(box) > 0.005f)
				lgts.push_back(pLgt);
		}
	}

	void RenderSys::AttachNode(const SceneNode *pNode)
	{
		if(pNode==NULL)
			return;
		if(pNode->IS_STATIC_CLASS(SGLightNode))
		{
			SGLightNode *pLightNode=(SGLightNode*)pNode;
			m_p->m_lightNodes.push_back(pLightNode);
		}
		else
			m_p->m_sceneNodes.push_back((SceneNode*)pNode);

		//--递归的调用所有children
		for(int i=0;i<pNode->GetNumChildren();i++)
		{
			AttachNode(pNode->GetChild(i));
		}
	}

#if 0//对比一下mem pool的效率
	IVertBuffer* RenderSys::AllocVB(UINT sizeInBytes)
	{
		static IVertBuffer *pVB=NULL;
		if(pVB==NULL)
		{
			IDevice *pDev=Device();
			pVB=pDev->NewVB();
			sizeInBytes=1024*1024*2;
			pVB->Create(sizeInBytes,EB_DynamicWriteOnly,0);
		}
		return pVB;
	}
	void RenderSys::FreeVB(IVertBuffer* pVB)
	{
	}
#else
	IVertBuffer* RenderSys::AllocVB(UINT sizeInBytes)
	{
		return (IVertBuffer*)(m_p->m_vbPool.Alloc(sizeInBytes));
	}
	void RenderSys::FreeVB(IVertBuffer* pVB)
	{
		m_p->m_vbPool.Free(pVB);
	}
#endif

	IIndexBuffer* RenderSys::AllocIB(UINT sizeInBytes)
	{
		return (IIndexBuffer*)(m_p->m_ibPool.Alloc(sizeInBytes));
	}

	void RenderSys::FreeIB(IIndexBuffer* pIB)
	{
		m_p->m_ibPool.Free(pIB);
	}

	int	 RenderSys::ScaleSGLod(int sgLod,int nodeMaxLod)
	{
		return int(float(sgLod)/SceneGraph::MAX_LOD*nodeMaxLod);
	}

	void RenderSys::AddPostProcess(PostProcess* pPostProcess)
	{
		ASSERT(pPostProcess!=NULL);

		DelPostProcess(pPostProcess);
		m_PostProcessArray.push_back(pPostProcess);
	}

	void RenderSys::DelPostProcess(PostProcess* pPostProcess)
	{
		for( vector<PostProcess*>::iterator it=m_PostProcessArray.begin();
			it!=m_PostProcessArray.end(); ++it )
		{
			if( *it==pPostProcess )
			{
				m_PostProcessArray.erase(it);
				SAFE_DELETE(pPostProcess);
				break;
			}
		}		
	}

	void RenderSys::ClearAllPostProcess()
	{
		for( vector<PostProcess*>::iterator it=m_PostProcessArray.begin();
			it!=m_PostProcessArray.end(); ++it )
		{
			PostProcess* p = *it;
			SAFE_DELETE(p);
		}
		m_PostProcessArray.clear();

	}

	bool RenderSys::EnablePostProcessBlur( const bool bEnalbe )
	{
		if( NULL != m_pPostProcessBlur && bEnalbe )
			return true;
		if( NULL == m_pPostProcessBlur && !bEnalbe )
			return true;

		if( bEnalbe )
		{
			m_pPostProcessBlur = new PostProcessBlur;
			if( m_pPostProcessBlur->Init() )
			{
				AddPostProcess( m_pPostProcessBlur );
				return true;
			}
			else
			{
				SAFE_DELETE( m_pPostProcessBlur );
				return false;
			}
		}
		else
		{
			DelPostProcess( m_pPostProcessBlur );
			m_pPostProcessBlur = NULL;
			return true;
		}
	}
}//namespace Cool3D