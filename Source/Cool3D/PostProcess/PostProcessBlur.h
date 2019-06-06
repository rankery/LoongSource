#pragma once
#include "postprocess.h"
#include "..\Device\IPixelShader.h"
#include "..\Device\IRenderTexture.h"
#include "..\Device\IVertBuffer.h"
#include "..\ResSys\VertStream.h"
#include "..\ResSys\ResTexture.h"

namespace Cool3D
{
	/** \class PostProcessBlur
		\brief 全屏柔化效果
	*/
	class Cool3D_API PostProcessBlur :	public PostProcess
	{
	public:
		PostProcessBlur(void);
		virtual ~PostProcessBlur(void);

		//--PostProcess
		virtual bool Init();
		virtual bool Start( IRenderTexture* pSrcTex, IRenderTexture* pDstTex, RenderSys* pRenderSys, bool bRenderToBackBuffer, SceneGraph* pSG );
		/** \硬件是否支持
		*/
		static bool IsHWSupport();
	private:
		//!缩放顶点坐标与贴图坐标
		void UpdateVertStream( float tarW, float tarH );
		//!处理一遍
		void RenderOnce( IRenderTexture* pSrcTex, IRenderTexture* pDstTex, IPixelShader* pPS, bool bRenderToBackBuffer );

		//!模糊PS
		IPixelShader* m_pBlurPS;	
		IPixelShader* m_pBlurHPS;
		IPixelShader* m_pBlurVPS;

		//!缩小PS
		IPixelShader* m_pScaleDownPS;
		//!放大PS
		IPixelShader* m_pScaleUpPS;
		//!相乘PS
		IPixelShader* m_pMulPS;

		//!临时texture
		IRenderTexture* m_pTempTex1;
		IRenderTexture* m_pTempTex2;

		//!顶点数据
		VertStream m_vertStream;
		//!顶点缓冲区
		IVertBuffer* m_pVb;

		//!模糊位置
		static float m_BlurPixel[4][2];
		Vector4	m_BlurPixelParam[4];

		DECL_RTTI(PostProcessBlur);
	};

}//namespace Cool3D
