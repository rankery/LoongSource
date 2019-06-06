#pragma once
#include "postprocess.h"

namespace Cool3D
{
	class ResPixelShader;
	class DynamicMesh;
	class IRenderTexture;
	class ResTexture;
	class IDevice;
	class StateBlock;
	class MBitmap;

	/** \struct tagHeatHaze
	    \brief  包含了热处理所需的数据供外部加入时填充
	*/
	struct tagHeatHaze
	{
		DynamicMesh* pMesh;			// 几何图形
		MBitmap*	 pHeatMtl;		// 热材质，用来扰动后缓冲
		Matrix4      matWorld;		// 世界矩阵
		Matrix4      matBumpMap;	// 扰动纹理的矩阵，用来产生动画
		float        fOffsetFator;	// 扰动系数

		tagHeatHaze() : pMesh( NULL ), pHeatMtl( NULL ), 
			            matWorld( MathConst::IdentityMat ), matBumpMap( MathConst::IdentityMat ),
						fOffsetFator( 0.0f ) {}
	};

	/** \class   PostProcessHeatHaze
		\brief	 热薄雾后处理效果
		\remarks 使用一张纹理的R,G通道扰动后缓冲，产生热气折射效果
	*/
	class Cool3D_API PostProcessHeatHaze :	public PostProcess
	{
	public:
		PostProcessHeatHaze(void);
		virtual ~PostProcessHeatHaze(void);

		// PostProcess
		virtual bool Init();
		virtual bool Start( IRenderTexture* pSrcTex, IRenderTexture* pDstTex, RenderSys* pRenderSys, bool bRenderToBackBuffer, SceneGraph* pSG );

		/** \加入一个热处理特效
		*/
		void AddHeatHaze( const tagHeatHaze& hh );
		/** \硬件是否支持
		*/
		static bool IsHWSupport();
	private:
		void SetDeviceStates( IDevice* pDev, StateBlock* pBlock );
		void ResumeDeviceStates( IDevice* pDev, StateBlock* pBlock );
	private:
		IPixelShader* m_pPS;
		list<tagHeatHaze> m_listHeatHaze;

		DECL_RTTI(PostProcessHeatHaze);
	};

}//namespace Cool3D
