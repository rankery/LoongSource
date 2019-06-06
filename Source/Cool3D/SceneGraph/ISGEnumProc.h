#pragma once

namespace Cool3D
{
	/**	\class ISGEnumProc
		\brief 枚举SceneGraph中所有节点的接口
		\see class SceneGraph;
	*/
	class ISGEnumProc
	{
	public:
		enum ERet
		{
			ERet_Continue	=0,		//继续
			ERet_IgnoreChildren,	//忽略当前节点的所有子节点,然后继续
			ERet_Abord,				//停止
		};
	public:
		ISGEnumProc(void)	{	}
		virtual ~ISGEnumProc(void)	{	}

		virtual ERet CallBack(SceneNode *pNode) =0;
	};
}//namespace Cool3D