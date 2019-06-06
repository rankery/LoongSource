#include "StdAfx.h"
#include ".\mtlrendercache.h"

namespace Cool3D
{
	MtlRenderCache::MtlRenderCache(void)
	{
	}

	MtlRenderCache::~MtlRenderCache(void)
	{
	}

	void MtlRenderCache::Begin(IVertShader* pVS,IPixelShader* pPS)
	{
		m_stateSave.Clear();
		for(set<ERStateType>::iterator iter=m_influenceState.begin();
			iter!=m_influenceState.end();++iter)
		{
			m_stateSave.SaveState(*iter);
		}
	}

	void MtlRenderCache::End()
	{
		m_stateSave.Apply();
	}
}//namespace Cool3D