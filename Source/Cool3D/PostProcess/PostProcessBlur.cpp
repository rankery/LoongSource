#include "StdAfx.h"
#include ".\postprocessblur.h"
#include "..\Util\Exception.h"
#include "..\Kernel.h"
#include "..\Public\VertexDefine.h"
#include "..\Device\StateBlock.h"
#include "..\RenderSys\RenderSys.h"
#include "..\Device\DX9Device\DX9Tex.h"
#include "..\Device\DX9Device\DX9Device.h"
#include "..\Device\DX9Device\DX9RenderTex.h"

using namespace Cool3D::Detail;

namespace Cool3D
{
	IMP_RTTI(PostProcessBlur,RTTIObj);

	//-----------------------------------------------------
	// 平均模糊位置数组,将以常量形式传送给PixelShader
	float PostProcessBlur::m_BlurPixel[4][2] =
	{
		{  0,  -1 },
		{  0,   1 },

		{ -1,   0 },
		{  1,   0 },
	};


	PostProcessBlur::PostProcessBlur(void)
	{
		m_pBlurPS = NULL;
		m_pScaleDownPS = NULL;
		m_pMulPS = NULL;
		m_pBlurHPS = NULL;
		m_pBlurVPS = NULL;
		
		m_pTempTex1 = NULL;
		m_pTempTex2 = NULL;

		m_pVb = NULL;

	}

	PostProcessBlur::~PostProcessBlur(void)
	{
		SAFE_DELETE(m_pBlurPS);
		SAFE_DELETE(m_pScaleDownPS);
		SAFE_DELETE(m_pMulPS);
		SAFE_DELETE(m_pBlurHPS);
		SAFE_DELETE(m_pBlurVPS);

		SAFE_DELETE(m_pTempTex1);
		SAFE_DELETE(m_pTempTex2);

		SAFE_DELETE(m_pVb);

	}

	bool PostProcessBlur::Init()
	{
		IDevice* pDev = Device();
		if( pDev->CapsPixelShaderVersion() < D3DPS_VERSION( 2, 0 ) )
			return false;

		//--创建所有PixelShader
		m_pScaleDownPS = pDev->NewPS();
		m_pBlurPS = pDev->NewPS();
		m_pMulPS = pDev->NewPS();
		m_pScaleDownPS->CreateFromFile( _T("data\\System\\PostProcess\\Blur\\ScaleDown.ps"),"ps_2_0", "PSMain" );
		m_pBlurPS->CreateFromFile( _T("data\\System\\PostProcess\\Blur\\Blur.ps"),"ps_2_0", "PSMain" );
		m_pMulPS->CreateFromFile( _T("data\\System\\PostProcess\\Blur\\Mul.ps"),"ps_2_0", "PSMain" );

		//--准备模糊位置参数
		for( int i=0; i<4; i++ )
		{
			m_BlurPixelParam[i].x = m_BlurPixel[i][0] / pDev->GetClientWidth();
			m_BlurPixelParam[i].y = m_BlurPixel[i][1] / pDev->GetClientHeight();
			m_BlurPixelParam[i].z = m_BlurPixelParam[i].w = 0.0f;
		}
		
		//--创建临时RenderTarget
		m_pTempTex1 = pDev->NewRenderTexture();
		m_pTempTex1->Create( 256, 256, EPF_A8R8G8B8, false );
		m_pTempTex2 = pDev->NewRenderTexture();
		m_pTempTex2->Create( 256, 256, EPF_A8R8G8B8, false );
		
		//--创建顶点缓冲区
		m_vertStream.Create( EVType_PtT, 4 );
		m_pVb = pDev->NewVB();
		m_pVb->Create( m_vertStream.GetBufferSize(), EB_DynamicWriteOnly, m_vertStream.GetStride() );

		return true;
	}

	void PostProcessBlur::UpdateVertStream( float tarW, float tarH )
	{
		Vert_PtT* pVert = (Vert_PtT*)m_vertStream.GetRawBuffer();

		pVert[0].pos = Vector4( -0.5f,		-0.5f,		1.0f,	1.0f );
		pVert[1].pos = Vector4( tarW-0.5f,	-0.5f,		1.0f,	1.0f );
		pVert[2].pos = Vector4( -0.5f,		tarH-0.5f,	1.0f,	1.0f );
		pVert[3].pos = Vector4( tarW-0.5f,	tarH-0.5f,	1.0f,	1.0f );
		pVert[0].uv  = Vector2( 0.0f,		0.0f );
		pVert[1].uv  = Vector2( 1,			0.0f );
		pVert[2].uv  = Vector2( 0.0f,		1 );
		pVert[3].uv  = Vector2( 1,			1 );

		m_pVb->CopyFromVertStream( &m_vertStream );
	}

	void PostProcessBlur::RenderOnce( IRenderTexture* pSrcTex, IRenderTexture* pDstTex, IPixelShader* pPS, bool bRenderToBackBuffer )
	{
		IDevice* pDev = Device();

		if( !bRenderToBackBuffer ) pDstTex->Begin();

		pDev->SetPixelShader( pPS );
		pDev->SetTexture( 0, pSrcTex );
		pDev->Draw( EDraw_TriangleStrip, 2 );

		if( !bRenderToBackBuffer ) pDstTex->End();
	}

	bool PostProcessBlur::Start( IRenderTexture* pSrcTex, IRenderTexture* pDstTex, RenderSys* pRenderSys, bool bRenderToBackBuffer, SceneGraph* pSG )
	{
		ASSERT( pSrcTex && pDstTex && pRenderSys );

		IDevice* pDev = Device();

		//--设置渲染状态
		StateBlock block;
		block.SaveState(ERS_FFogEnable);		pDev->FEnableFog(false);
		block.SaveState(ERS_FLightEnable);		pDev->FEnableLight(false);
		block.SaveState(ERS_DepthTestEnable);	pDev->EnableDepthTest(false);
		block.SaveState(ERS_DepthWriteEnable);	pDev->EnableDepthWrite(false);
		block.SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);
		block.SaveState(ERS_AlphaBlendEnable);	pDev->EnableAlphaBlend(false);
		block.SaveState(ERS_AlphaTestEnable);	pDev->EnableAlphaTest(false);
		
		pDev->SetSimplerAddress(0,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
		pDev->SetSimplerFilter(0,ETFilter_LINEAR,ETFilter_LINEAR);
		pDev->SetSimplerMipFilter(0,ETFilter_NONE);
		pDev->FSetUVTrans(0,EUVT_Disable);

		pDev->SetVB( 0, m_pVb );
		pDev->SetStreamDefine( pRenderSys->GetStreamDefine( EVType_PtT ) );
		
		//--缩小: SrcTex -> TempTex1
		m_pScaleDownPS->SetVectorArray( "PixelCoords", &m_BlurPixelParam[0], 4 );
		UpdateVertStream( 256, 256 );
		RenderOnce( pSrcTex, m_pTempTex1, m_pScaleDownPS, false );

		//--模糊1次: TempTex1 -> TempTex2
		m_pBlurPS->SetVectorArray( "PixelCoords", &m_BlurPixelParam[0], 4 );
		RenderOnce( m_pTempTex1, m_pTempTex2, m_pBlurPS, false );

		//--模糊2次: TempTex2 -> TempTex1
		RenderOnce( m_pTempTex2, m_pTempTex1, m_pBlurPS, false );

		//--混合: TempTex1 * SrcTex -> BackBuffer
		UpdateVertStream( (float)pDev->GetClientWidth(), (float)pDev->GetClientHeight() );
		pDev->SetTexture( 1, pSrcTex );
		RenderOnce( m_pTempTex1, pDstTex, m_pMulPS, true );
		pDev->SetTexture( 1, (IDeviceTex*)NULL );

		//--恢复渲染状态
		pDev->SetPixelShader( NULL );
		block.Apply();

		return true;
	}

	bool PostProcessBlur::IsHWSupport()
	{
		if( Device()->CapsPixelShaderVersion() < D3DPS_VERSION( 2, 0 ) )
			return false;
		return true;
	}

}//namespace Cool3D

