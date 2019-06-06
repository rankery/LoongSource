#pragma once
#include "..\Cool3D.h"

namespace Cool3D
{
	class HugeQuadTreeSG;
	class SceneNode;
	/** \class ISGVisitorCallBack
		\brief SceneGraph遍历器回调接口
	*/
	class Cool3D_API ISGVisitorCallBack
	{
	public:
		ISGVisitorCallBack(void);
		virtual ~ISGVisitorCallBack(void);

		virtual void OnSceneGraphLoad(HugeQuadTreeSG* pSG){}
		virtual bool OnVisitSceneNode(HugeQuadTreeSG* pSG,SceneNode* pNode,int index,int totalNum,DWORD param) =0;
	};
}//namespace Cool3D