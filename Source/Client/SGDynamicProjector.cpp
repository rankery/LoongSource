#include "stdafx.h"
#include "sgdynamicprojector.h"
#include "MapMgr.h"

IMP_RTTI(SGDynamicProjector,SceneNode);

SGDynamicProjector::SGDynamicProjector(void) :
m_pResPS( NULL ),
m_pMesh( NULL ),
m_pRenderTex( NULL )
{
}

SGDynamicProjector::~SGDynamicProjector(void)
{
	SAFE_DEL( m_pMesh );
	SAFE_DEL( m_pRenderTex );
	RES_MGR_RELEASE_PS( m_pResPS );
}

bool SGDynamicProjector::CreateData(int texW,int texH)
{
	ASSERT( NULL == m_pMesh );
	ASSERT( NULL == m_pRenderTex );

	m_pMesh=new DynamicMesh;
	m_pMesh->SetZBias( true );
	m_pMesh->SetPriority(RenderNode::EPri_Low);
	m_pRenderTex=new MRenderTex;
	IRenderTexture *pRTex=Device()->NewRenderTexture();
	if( !pRTex->Create(texW,texH) )
	{
		SAFE_DEL( m_pRenderTex );
		return false;
	}
	m_pRenderTex->m_pRTex=pRTex;
	m_pRenderTex->m_addressU=m_pRenderTex->m_addressV=m_pRenderTex->m_addressW=ETAddress_Clamp;
	m_pRenderTex->m_magFilter=m_pRenderTex->m_minFilter=ETFilter_LINEAR;

	MMAlphaMask *pMask=new MMAlphaMask;
	pMask->m_alphaRef=0;
	pMask->m_alphaTestFunc=ECMP_Greater;

	MMFrameBlend *pMM=new MMFrameBlend;
	pMM->m_blendEnable=true;
	pMM->m_src=EBlend_SrcAlpha;
	pMM->m_dst=EBlend_InvSrcAlpha;

	MMDepthFrame *pDepth=new MMDepthFrame;
	pDepth->m_depthWriteEnable=false;
	pDepth->m_depthTestEnable=true;
	pDepth->m_depthFunc=ECMP_LessEqual;

	m_pRenderTex->AttachModifier(pMask);
	m_pRenderTex->AttachModifier(pMM);
	m_pRenderTex->AttachModifier(pDepth);
	m_pRenderTex->m_bTwoSide = true;

	m_camLight.SetOrtho( (float)texW, (float)texH, 1.0f, 100000.0f );

	RES_MGR_RELEASE_PS( m_pResPS );
	m_pResPS = ResMgr::Inst()->NewPS( _T("data\\system\\shader\\shadow.ps" ), MAKELONG( 1 , 4 ) );

	return true;
}

bool SGDynamicProjector::IsInFrustum(const Frustum *pFrumstum) const
{
	return true;//由父节点做视锥剪裁,自己不再做
}

void SGDynamicProjector::LoadRes(const TCHAR* szName)
{
	ASSERT(0 && "SGDynamicProjector have no res");
}

UINT SGDynamicProjector::GetNumRenderNode() const
{
	return 1;
}

RenderNode *SGDynamicProjector::GetRenderNode(UINT i) const
{
	return m_pMesh;
}

const Matrix4& SGDynamicProjector::GetWorldMat() const
{
	return MathConst::IdentityMat;
}

void SGDynamicProjector::SetWorldMat(const Matrix4 *pMat)
{
}

void SGDynamicProjector::GetBox(AABBox& out) const
{
	if(GetParent())
	{
		GetParent()->GetBox(out);
	}
}

IMaterial*	SGDynamicProjector::GetMtl(int mtlID)
{
	return m_pRenderTex;
}

void SGDynamicProjector::MergeChildBox(const SceneNode* pNode,AABBox& boxParent)
{
	AABBox box;
	for(int i=0;i<pNode->GetNumChildren();i++)
	{
		const SceneNode* pChild=pNode->GetChild(i);
		if(pChild->IS_KIND_OF(EntityNode))
		{
			pChild->GetBox(box);
			boxParent.Merge(box);
		}
	}
}

void SGDynamicProjector::OnRenderBefore(SceneNode* pSGNode,RenderNode* pRenderNode)
{
	const EMtlTrait eTrait = pRenderNode->GetMtlTrait( pSGNode );
	IDevice* pDev = Device();
	if( EMT_AlphaTestAndBlend == eTrait || EMT_AlphaBlend == eTrait )
	{
		pDev->EnableAlphaBlend(false);
		pDev->EnableAlphaTest(true);
		pDev->SetAlphaTestFunc(ECMP_Greater);
		pDev->SetAlphaRef(240);
		m_pResPS->GetPS()->SetFloat( "g_fAlphaAdd", 0.0f );
	}
	else if( EMT_AlphaTest == eTrait )
	{
		pDev->EnableAlphaBlend(false);
		pDev->EnableAlphaTest(true);
		pDev->SetAlphaTestFunc(ECMP_Greater);
		pDev->SetAlphaRef(0);
		m_pResPS->GetPS()->SetFloat( "g_fAlphaAdd", 0.0f );
	}
	else
	{
		pDev->EnableAlphaTest(false);
		pDev->EnableAlphaBlend(false);
		m_pResPS->GetPS()->SetFloat( "g_fAlphaAdd", 1.0f );
	}
}

void SGDynamicProjector::Update(float deltaTime,SceneGraph *pSG)
{
	if( pSG == NULL )
	{
		m_pMesh->SetDrawLimit( 0 );
		return;
	}
	const SceneNode* pParentNode = GetParent();
	if( NULL == pParentNode )
	{
		m_pMesh->SetDrawLimit( 0 );
		return;
	}
	if( NULL == m_pResPS || !m_pResPS->IsCreated() )
	{
		m_pMesh->SetDrawLimit( 0 );
		return;
	}

	Vector3 lgtDir = pSG->GetSunLight().dir;

	AABBox box;
	GetParent()->GetBox(box);
	MergeChildBox(GetParent(),box);

	Vector3 center=box.GetCenter();
	float d=Vec3Dist(box.max,box.min);

	m_camLight.SetLookAt(center-lgtDir*d,center,MathConst::AxisY);
	m_camLight.AdjustAdaptive(box);

	// 渲染阴影纹理
	CameraBase* pOldCam = pSG->GetCamera();
	pSG->BindCamera( &m_camLight, false );
	RenderNode::m_pSpecialPS = m_pResPS->GetPS();

	// 视口缩小1像素，保证边缘像素透明（为了不使用BorderTexWrapMode）
	IDevice* pDev = Device();
	tagViewport newViewport;
	pDev->GetViewport(newViewport);
	newViewport.x = 1;
	newViewport.y = 1;
	newViewport.width = m_pRenderTex->m_pRTex->Width() - 2;
	newViewport.height = m_pRenderTex->m_pRTex->Height() - 2;

	m_stateBlock.Clear();
	m_stateBlock.SaveState(ERS_AlphaBlendEnable);
	m_stateBlock.SaveState(ERS_SrcBlend);
	m_stateBlock.SaveState(ERS_DstBlend);
	m_stateBlock.SaveState(ERS_AlphaTestEnable);
	m_stateBlock.SaveState(ERS_AlphaTestFunc);
	m_stateBlock.SaveState(ERS_AlphaRef);

	Kernel::Inst()->GetRenderSys()->RenderSceneNode( &m_camLight, pParentNode, m_pRenderTex->m_pRTex, true, 0, 0, this, pSG, &newViewport );

	m_stateBlock.Apply();

	RenderNode::m_pSpecialPS = NULL;
	pSG->BindCamera( pOldCam, false );

	// 建立网格
	m_pMesh->BeginBuild( 0, EDraw_TriangleList );
	BuildProjectorMesh( box, m_pMesh, &m_camLight );
	if( m_pMesh->BuildGetNumPos() == 0 )
		m_pMesh->SetDrawLimit( 0 );

	m_pMesh->EndBuild();
}

void SGDynamicProjector::ForceUpdate(float deltaTime,SceneGraph *pSG)
{
	//do thing
}

void SGDynamicProjector::BuildProjectorMesh( AABBox& boxRole, DynamicMesh *pMesh, CameraBase *pCamera )
{
	AABBox box = boxRole;
	float boxSize=Vec3Dist(box.max,box.min);

	// 防止box错误而导致生成一个巨大的网格
	if(boxSize>2000.0f)
		return;

	Vector3 lgtDir = pCamera->GetDirection();
	if(lgtDir.x>0)
		box.max.x+=boxSize;
	if(lgtDir.x<0)
		box.min.x-=boxSize;
	if(lgtDir.z>0)
		box.max.z+=boxSize;
	if(lgtDir.z<0)
		box.min.z-=boxSize;
	box.min.y-=boxSize*0.5f;

	MapMgr::Inst()->GetSceneGraph()->BuildDecalMesh( box, pMesh, &(-lgtDir) );
	int num=pMesh->BuildGetNumPos();
	for(int i=0;i<num;i++)
	{
		Vector3 pos = pMesh->BuildGetPos(i);
		Vector3 uvw=pCamera->WorldToScreenF(pos);
		uvw.x = uvw.x * 0.5f + 0.5f;
		uvw.y = -uvw.y * 0.5f + 0.5f;
		pMesh->AddTexCoord(uvw);
		pMesh->AddDiffuse( Color4f(0.6f,1.0f,1.0f,1.0f) );
	}
}

bool SGDynamicProjector::IsHWSupport()
{
	IDevice* pDev=Device();
	return ( pDev->CapsPixelShaderVersion() >= D3DPS_VERSION( 1, 4 ) );
}