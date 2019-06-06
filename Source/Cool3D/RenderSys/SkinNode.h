#pragma once
#include "rendernode.h"

namespace F3D
{
	class ResSkin;
	/**	\class SkinNode 
		\brief Skin渲染节点
		\par
		在此节点Attach到RenderSys时需要从SceneNode得到AniCtrl接口
		\par
		一个ResSkin对象使用一个SkinNod对象,已方便vert buffer处理
	*/
	class SkinNode : public RenderNode
	{
	public:
		SkinNode(void);
		virtual ~SkinNode(void);

		virtual void Create(ResSkin *pRes) =0;
		
	};
}//namespace F3D