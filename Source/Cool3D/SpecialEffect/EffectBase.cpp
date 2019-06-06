#include "StdAfx.h"
#include ".\effectbase.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\RenderMtl\MMDepthFrame.h"
#include "..\RenderMtl\MMUVPage.h"
#include "..\ResSys\ResMgr.h"
#include "..\PostProcess\PostProcessHeatHaze.h"
#include "..\RenderSys\RenderNode.h"
#include "..\RenderSys\DynamicMesh.h"
#include "SGEffectNode.h"
#include "..\Kernel.h"

namespace Cool3D
{
	bool EffectBase::s_reloadTex=false;
	MConstColor EffectBase::s_defaultMtl;

	IMP_RTTI(EffectBase,RTTIObj);

	EffectBase::EffectBase(void)
	{
		m_pMtl=NULL;
		s_defaultMtl.m_bUsingVertColor=true;//默认使用顶点颜色
		s_defaultMtl.m_bTwoSide=true;

		m_bClosing=false;
		m_closeTime=0;
		m_mmUVPage = NULL;
	}

	EffectBase::~EffectBase(void)
	{
		SAFE_DELETE(m_pMtl);
	}

	void EffectBase::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_mtlDesc);
	}

	void EffectBase::Deserialize(FArchive& ar)
	{
		ar.ValueRead(m_mtlDesc);

		CreateMtl(m_mtlDesc);
	}

	int EffectBase::AdvanceState(int curState,int numState,float curStateLastTime,float& stateTime,bool loop)
	{
		if(stateTime>curStateLastTime)
		{
			stateTime-=curStateLastTime;
			curState++;
			if(curState>=numState)
			{
				if(loop)
					curState=0;
				else
				{
					curState=numState-1;
					m_bClosing=true;
				}
			}
		}
		return curState;
	}

	void EffectBase::CreateMtl(const tagMtlEffectDesc& desc)
	{
		CreateBasicMtl(desc);

		//处理贴图动画
		if(m_pMtl!=NULL)
		{
			m_mmUVPage = new MMUVPage;
			m_mmUVPage->SetParam(desc.frmU,desc.frmV,desc.fps,desc.bPlayOnce);
			m_pMtl->AttachModifier( m_mmUVPage );
		}//endof if
	}

	IMaterial* EffectBase::GetMtl()
	{	
		if(m_pMtl)
			return m_pMtl;
		return &s_defaultMtl;
	}

	void EffectBase::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
	{
		if(m_pMtl)
		{
			m_pMtl->UpdateModifiers(deltaTime);
			if( NULL != m_mmUVPage )
				m_mmUVPage->SetOffset( m_mtlDesc.fScrollSpeedU * runTime, m_mtlDesc.fScrollSpeedV * runTime );

			if( m_mtlDesc.bHeatHaze && 
				NULL != Kernel::Inst()->GetRenderSys() &&
				NULL != Kernel::Inst()->GetRenderSys()->GetPostProcessHeatHaze() )
			{
				int numRenderNode = GetNumRenderNodes();
				for( int i = 0; i < numRenderNode; i++ )
				{
					RenderNode* pRenderNode = GetRenderNode( i );
					if( NULL == pRenderNode )
						continue;
					if( !pRenderNode->IS_STATIC_CLASS( DynamicMesh ) )
						continue;

					// 加入后处理
					tagHeatHaze hh;
					hh.pMesh = (DynamicMesh*)pRenderNode;
					hh.pHeatMtl = m_pMtl;
					if( hh.pMesh->IsIdentityWorldMat() )
						hh.matWorld = MathConst::IdentityMat;
					else
						hh.matWorld = pSGNode->GetWorldMat();
					if( NULL != m_mmUVPage )
						hh.matBumpMap = *m_mmUVPage->GetMatrix();
					else
						hh.matBumpMap = MathConst::IdentityMat;
					hh.fOffsetFator = m_mtlDesc.fHeatOffsetFator;
					Kernel::Inst()->GetRenderSys()->GetPostProcessHeatHaze()->AddHeatHaze( hh );
				}
			}
		}
	}

	void EffectBase::UpdateClosingFade(float runTime,float timeLen)
	{
		if(m_bClosing)
		{
			float t=runTime-m_closeTime;
			if(t>=timeLen)
				m_closingFade=0;
			else
				m_closingFade=LinearLerp(1.0f,0.0f,t/timeLen);
		}
	}

	void EffectBase::ResetPlayState()
	{
		if(m_pMtl)
			m_pMtl->ResetModAni();
		m_bClosing=false;
		m_closingFade=1.0f;
	}

	void EffectBase::Close(float sgNodeRunTime)
	{
		if(!m_bClosing)
		{
			m_bClosing=true;
			m_closeTime=sgNodeRunTime;
		}
	}
	
	bool EffectBase::IsClosed()
	{
		return m_bClosing && m_closingFade<=0.0f;
	}

	void EffectBase::OnDetachFromSGNode(SGEffectNode *pSGNode)
	{}

	void EffectBase::CreateBasicMtl(const tagMtlEffectDesc& desc)
	{
		SAFE_DELETE(m_pMtl);

		m_mtlDesc=desc;

		if(desc.szTexName[0]==0)//贴图文件名没有,则不创建
			return;

		m_pMtl=new MBitmap;
		m_pMtl->m_bEnableLight=desc.bEnableLight; 
		m_pMtl->m_bTwoSide=true;	//特效物体都使用双面材质

		// 加载纹理
		const bool bIOAsync = ResMgr::Inst()->IsIOAsync();
		if( s_reloadTex && bIOAsync )
			ResMgr::Inst()->EnableIOAsync( false );

		m_pMtl->SetTexture(0,desc.szTexName);
		ResMgr::Inst()->EnableIOAsync( bIOAsync );

		// 热处理特效不作AlphaBlend
		if( desc.bHeatHaze )
			return;

		switch(desc.blendType)
		{
		case 1://alpha blend
			{
				MMFrameBlend *pBlend=new MMFrameBlend;
				pBlend->m_blendEnable=true;
				pBlend->m_src=EBlend_SrcAlpha;
				pBlend->m_dst=EBlend_InvSrcAlpha;
				m_pMtl->AttachModifier(pBlend);
			}
			break;
		case 2://additive blend
			{
				MMFrameBlend *pBlend=new MMFrameBlend;
				pBlend->m_blendEnable=true;
				pBlend->m_src=EBlend_SrcAlpha;
				pBlend->m_dst=EBlend_One;
				m_pMtl->AttachModifier(pBlend);
			}
			break;
		case 3://颜色相乘
			{
				MMFrameBlend *pBlend=new MMFrameBlend;
				pBlend->m_blendEnable=true;
				pBlend->m_src=EBlend_Zero;
				pBlend->m_dst=EBlend_SrcColor;
				m_pMtl->AttachModifier(pBlend);
			}
			break;
		case 4://屏幕反色
			{
				MMFrameBlend *pBlend=new MMFrameBlend;
				pBlend->m_blendEnable=true;
				pBlend->m_src=EBlend_InvDestColor;//只有Src color为纯白,才是正确的屏幕反色
				pBlend->m_dst=EBlend_Zero;
				m_pMtl->AttachModifier(pBlend);
			}
			break;
		default://0没有alpha混合
			break;
		}//endof switch

		if(!desc.bTestZ)
		{
			MMDepthFrame *pMDepth=new MMDepthFrame;
			pMDepth->m_depthWriteEnable=false;
			pMDepth->m_depthTestEnable=false;
			m_pMtl->AttachModifier(pMDepth);
		}
		else
		{
			// 有Alpha混合就不写ZBuffer，避免不正确的混合
			if( desc.blendType >= 1 && desc.blendType <= 4 )
			{
				MMDepthFrame *pMDepth=new MMDepthFrame;
				pMDepth->m_depthWriteEnable=false;
				pMDepth->m_depthTestEnable=true;
				m_pMtl->AttachModifier(pMDepth);
			}
		}
	}

	IMaterial *EffectBase::GetMtlFallback()
	{
		return &s_defaultMtl;
	}
}//namespace Cool3D