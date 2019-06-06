
#include "stdafx.h"
#include "CmdDispatch.h"
#include "CmdTarget.h"
#include "DebugMisc.h"

namespace Cool3D
{
	CmdDispatch::CmdDispatch()
	{
		
	}
	
	CmdDispatch::~CmdDispatch()
	{
		
	}
	
	void CmdDispatch::AttachTarget(CmdTarget *pTarget)
	{
		ASSERT(pTarget != NULL);
		m_targets.push_back(pTarget);
	}
	
	void CmdDispatch::DetachTarget(CmdTarget *pTarget)
	{
		ASSERT(pTarget != NULL);
		m_targets.remove(pTarget);
	}
	
	
	void CmdDispatch::DispatchCmd(DWORD cmd, DWORD param1, DWORD param2)
	{
		CmdTarget *pTarget=NULL;
		for(list<CmdTarget*>::iterator li=m_targets.begin();
				li!=m_targets.end(); li++)
				{
					pTarget=(*li);
					ASSERT(pTarget != NULL);
					pTarget->OnCmd(cmd,param1,param2);
				}
	}
}//namespace Cool3D