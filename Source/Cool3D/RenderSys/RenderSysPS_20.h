#pragma once
#include "rendersys.h"

namespace Cool3D
{
	/**	\class RenderSysPS_20 
		\brief 使用PixelShader2.0,VertexShader2.0的渲染系统
		\remarks 使用Shader,但是并不代表只能使用shader,部分功能使用FixedFun pipeline更简便,则可以使用之.
	*/
	class RenderSysPS_20 : public RenderSys
	{
	public:
		RenderSysPS_20(void);
		virtual ~RenderSysPS_20(void);

		DECL_RTTI(RenderSysPS_20);
	};
}//namespace Cool3D