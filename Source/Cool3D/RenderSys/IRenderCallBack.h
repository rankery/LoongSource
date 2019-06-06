#pragma once
#include "..\Cool3D.h"

namespace Cool3D
{
	class SceneNode;
	class RenderNode;
	/** \class IRenderCallBack
		\brief 渲染器回调接口
	*/
	class Cool3D_API IRenderCallBack
	{
	public:
		IRenderCallBack(void);
		virtual ~IRenderCallBack(void);

		virtual void OnRenderBefore(SceneNode* pSGNode,RenderNode* pRenderNode) =0;
	};
}//namespace Cool3D