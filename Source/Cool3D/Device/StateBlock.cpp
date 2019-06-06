#include "StdAfx.h"
#include ".\stateblock.h"
#include "..\Kernel.h"

namespace Cool3D
{
	StateBlock::StateBlock(void)
	{
	}

	StateBlock::~StateBlock(void)
	{
	}

	void StateBlock::Clear()
	{
		m_stateArray.clear();
	}
	
	void StateBlock::SaveState(ERStateType state)
	{
		tagStateVal tmp;
		tmp.state=state;
		tmp.val=Device()->GetRenderState(state);
		m_stateArray.push_back(tmp);
	}
	
	void StateBlock::Apply()
	{
		IDevice *pDev=Device();
		for(UINT i=0;i<m_stateArray.size();i++)
		{
			tagStateVal& tmp=m_stateArray[i];
			pDev->SetRenderState(tmp.state,tmp.val);
		}
	}
}//namespace Cool3D