#pragma once
#include "..\Cool3D.h"
#include "..\Util\RTTIObj.h"
#include "..\Device\IRenderTexture.h"

namespace Cool3D
{
	class RenderSys;
	class SceneGraph;

	/** \class PostProcess
		\brief 后期处理基类,其派生类对象由RenderSys派生类对象调用
		\remarks 通过直接修改后缓冲来进行处理
		\par RenderSys将场景渲染到RenderTexture,再交给PostProcess
	*/
	class Cool3D_API PostProcess : public RTTIObj
	{
	public:
		PostProcess(void);
		virtual ~PostProcess(void);

		//!初始化,程序启动时调用一次
		virtual bool Init() =0;
		//!开始处理,每帧调用
		virtual bool Start( IRenderTexture* pSrcTex, IRenderTexture* pDstTex, RenderSys* pRenderSys, bool bRenderToBackBuffer, SceneGraph* pSG ) =0;

	private:
		DECL_RTTI(PostProcess);
	};

}//namespace Cool3D