

#include "stdafx.h"
#include "CmdTarget.h"
#include "CmdDispatch.h"
#include "DebugMisc.h"


namespace Cool3D
{
	CmdTarget::CmdTarget(CmdDispatch *pOwner) : m_pOwner(pOwner)
	{
		ASSERT(m_pOwner != NULL);
		m_pOwner->AttachTarget(this);
	}
	
	CmdTarget::~CmdTarget()
	{
		m_pOwner->DetachTarget(this);	
	}
}//namespace Cool3D