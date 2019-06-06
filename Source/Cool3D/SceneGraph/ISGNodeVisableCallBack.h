#pragma once

namespace Cool3D
{
	class SceneNode;
	/**	\class ISGNodeVisableCallBack
		\brief SceneGraph::Render()函数找到一个visable scene node时调用callback
		\see SceneGraph,SceneNode
	*/
	class ISGNodeVisableCallBack
	{
	public:
		ISGNodeVisableCallBack(void)	{}
		virtual ~ISGNodeVisableCallBack(void)	{}

		virtual void Begin() =0;
		virtual void CallBack(SceneNode *pVisNode) =0;
		virtual void End() =0;
	};

	/**	\class SGNodeVisableSet
		\brief 在SceneGraph::Render()函数调用是记录一个可见集
	*/
	class SGNodeVisableSet : public ISGNodeVisableCallBack
	{
	public:
		SGNodeVisableSet();
		virtual ~SGNodeVisableSet();

		virtual void Begin();
		virtual void CallBack(SceneNode *pVisNode);
		virtual void End();

		/**	测试一个SceneNode是否在可见集中*/
		bool IsNodeVisable(SceneNode *pNode) const;

	private:
		class Member;
		Member *m_p;
	};
}//namespace Cool3D