#include "StdAfx.h"
#include ".\PostProcessHeatHaze.h"
#include "..\Util\Exception.h"
#include "..\Kernel.h"
#include "..\Public\VertexDefine.h"
#include "..\Device\StateBlock.h"
#include "..\RenderSys\RenderSys.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Device\DX9Device\DX9Tex.h"
#include "..\Device\DX9Device\DX9Device.h"
#include "..\Device\DX9Device\DX9RenderTex.h"
#include "..\ResSys\ResTexture.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\RenderMtl\MMUVTrans.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\Device\IPixelShader.h"
#include "..\Util\SpeedAnalyser.h"
#include "..\Draw2D\IDraw2D.h"

namespace Cool3D
{
	IMP_RTTI(PostProcessHeatHaze,RTTIObj);

	PostProcessHeatHaze::PostProcessHeatHaze(void) :
	m_pPS( NULL )
	{
	}

	PostProcessHeatHaze::~PostProcessHeatHaze(void)
	{
		SAFE_DELETE( m_pPS );
	}

	bool PostProcessHeatHaze::Init()
	{
		SAFE_DELETE( m_pPS );
		m_pPS = Device()->NewPS();
		m_pPS->CreateFromFile( _T("data\\system\\shader\\heathaze.ps"), "ps_2_0", "main" );
		return true;
	}

	void PostProcessHeatHaze::SetDeviceStates( IDevice* pDev, StateBlock* pBlock )
	{
		pBlock->SaveState(ERS_FFogEnable);			pDev->FEnableFog(false);
		pBlock->SaveState(ERS_FLightEnable);		pDev->FEnableLight(false);
		pBlock->SaveState(ERS_DepthTestEnable);		pDev->EnableDepthTest(true);
		pBlock->SaveState(ERS_DepthWriteEnable);	pDev->EnableDepthWrite(true);
		pBlock->SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);
		pBlock->SaveState(ERS_AlphaTestEnable);		pDev->EnableAlphaTest(false);
		pBlock->SaveState(ERS_AlphaBlendEnable);	pDev->EnableAlphaBlend(true);
		pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);

		pDev->FSetTexCoordSrc( 0, 0 );
		pDev->FSetTexCoordSrc( 1, 1, EUVI_CameraSpacePos );
		pDev->FSetUVTrans( 0, EUVT_Count2 );
		pDev->FSetUVTrans( 1, EUVT_Project4 );
		pDev->SetSimplerAddress( 0, ETAddress_Wrap, ETAddress_Wrap, ETAddress_Wrap );
		pDev->SetSimplerAddress( 1, ETAddress_Clamp, ETAddress_Clamp, ETAddress_Clamp );
		pDev->SetSimplerFilter( 0, ETFilter_POINT, ETFilter_POINT);
		pDev->SetSimplerFilter( 1, ETFilter_POINT, ETFilter_POINT);
		pDev->SetSimplerMipFilter( 0, ETFilter_NONE );
		pDev->SetSimplerMipFilter( 1, ETFilter_NONE );

		pDev->SetVertShader( NULL );
		pDev->SetPixelShader( m_pPS );
	}

	void PostProcessHeatHaze::ResumeDeviceStates( IDevice* pDev, StateBlock* pBlock )
	{
		pDev->FSetUVTrans( 0, EUVT_Disable );
		pDev->FSetUVTrans( 1, EUVT_Disable );
		pDev->SetPixelShader( NULL );
		pBlock->Apply();
	}
	bool PostProcessHeatHaze::Start( IRenderTexture* pSrcTex, IRenderTexture* pDstTex, RenderSys* pRenderSys, bool bRenderToBackBuffer, SceneGraph* pSG )
	{
		ASSERT( pSrcTex && pDstTex && pRenderSys && pSG );

		if( NULL == m_pPS )
		{
			m_listHeatHaze.clear();
			return false;
		}
		if( m_listHeatHaze.empty() )
			return false;

		IDevice* pDev = Device();
		CameraBase* pCamera = pSG->GetCamera();

		if( !bRenderToBackBuffer ) pDstTex->Begin();

		// src -> desc
		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = pDev->GetClientWidth();
		rc.bottom = pDev->GetClientHeight();
		IDraw2D::Inst()->BeginDraw( IDraw2D::EBlend_Disable );
		IDraw2D::Inst()->Draw( &rc, &rc, pSrcTex );
		IDraw2D::Inst()->EndDraw();

		//--设置渲染状态
		StateBlock block;
		SetDeviceStates( pDev, &block );

		// 渲染列表中的Mesh
		Matrix4 matTexScene = *pCamera->GetPrj();
		for( list<tagHeatHaze>::iterator itr = m_listHeatHaze.begin(); itr != m_listHeatHaze.end(); itr++ )
		{
			tagHeatHaze& hh = *itr;

			pDev->MatPush( EMS_World );
			pDev->MatPush( EMS_Tex0 );
			pDev->MatPush( EMS_Tex1 );
			pDev->MatLoad( EMS_World, &hh.matWorld );
			pDev->MatLoad( EMS_Tex0, &hh.matBumpMap );
			pDev->MatLoad( EMS_Tex1, &matTexScene );
			pDev->FFlushMatrix( true, false, false, true, ETM_Tex0 | ETM_Tex1 );
			m_pPS->SetFloat( "g_OffsetFactor", hh.fOffsetFator );

			IStreamDefine *pDef = pRenderSys->GetStreamDefine( hh.pMesh->GetVertStream()->GetType() );
			pDev->SetStreamDefine( pDef );
			IVertBuffer *pVB = pRenderSys->AllocVB( hh.pMesh->GetVertStream()->GetBufferSize() );
			ASSERT( pVB != NULL );
			pVB->CopyFromVertStream( hh.pMesh->GetVertStream() );
			pDev->SetVB( 0, pVB );

			pDev->SetTexture( 0, hh.pHeatMtl->m_pBitmap->GetDeviceTex() );
			pDev->SetTexture( 1, pSrcTex );

			pDev->Draw( hh.pMesh->GetDrawType(), hh.pMesh->GetNumDraw() );

			pRenderSys->FreeVB( pVB );
			pDev->MatPop( EMS_World );
			pDev->MatPop( EMS_Tex0 );
			pDev->MatPop( EMS_Tex1 );
			pDev->FFlushMatrix( true, false, false, true, ETM_Tex0 | ETM_Tex1 );
		}
		if( !bRenderToBackBuffer ) pDstTex->End();

		// 处理完成，清空列表
		m_listHeatHaze.clear();

		// 恢复渲染状态
		ResumeDeviceStates( pDev, &block );

		return true;
	}

	void PostProcessHeatHaze::AddHeatHaze( const tagHeatHaze& hh )
	{
		if( NULL == hh.pMesh || NULL == hh.pHeatMtl )
			return;
		if( !hh.pHeatMtl->IsResourceReady() )
			return;

		m_listHeatHaze.push_back( hh );
	}

	bool PostProcessHeatHaze::IsHWSupport()
	{
		if( Device()->CapsPixelShaderVersion() < D3DPS_VERSION( 2, 0 ) )
			return false;
		return true;
	}
}//namespace Cool3D

