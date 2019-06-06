#pragma once

namespace Cool3D
{
	class CmdTarget;
	/**	\class CmdDispatch
		\brief 记录所有CmdTarget对象指针, 并将消息分发给它们
	*/
	class CmdDispatch  
	{
	public:
		void DispatchCmd(DWORD cmd,DWORD param1,DWORD param2);
		void DetachTarget(CmdTarget *pTarget);
		void AttachTarget(CmdTarget* pTarget);
		CmdDispatch();
		virtual ~CmdDispatch();

	protected:
		list<CmdTarget*> m_targets;
	};
}//namespace Cool3D