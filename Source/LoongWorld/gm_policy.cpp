#include "StdAfx.h"
#include "gm_policy.h"
#include "world.h"
#include "role_mgr.h"

#include "../ServerDefine/gm_tool_define.h"
#include "../ServerDefine/msg_rt_s.h"
#include "../ServerDefine/base_define.h"

#include "../WorldDefine/chat.h"

GMPolicy g_GMPolicy;

//-----------------------------------------------------------------------------
// 双倍策略
//-----------------------------------------------------------------------------
GMDoublePolicy::GMDoublePolicy()
{
	for (INT i=0; i<NUM_EDT; ++i)
	{
		m_fCurDouble[i] = 1.0f;
		m_bDouble[i]	= FALSE;
	}
}

VOID GMDoublePolicy::SetRate( EDoubleType eDoubleType, DWORD dwRate, DWORD dwStart, DWORD dwLast )
{
	if (!EDT_VALID(eDoubleType))
		return;
	m_bDouble[eDoubleType] = TRUE;

	// 设置倍率
	m_fCurDouble[eDoubleType] = dwRate / 100.0f;

	// 记录关闭时间
	m_dwEndTime[eDoubleType] = IncreaseTime(dwStart, dwLast);
}

VOID GMDoublePolicy::Update()
{
	for (INT i=0; i<NUM_EDT; ++i)
	{
		if (m_bDouble[i] == TRUE)
		{
			// 如果目前设置了多倍率，那么检查是否到时
			tagDWORDTime dwNow = GetCurrentDWORDTime();

			if(dwNow >= m_dwEndTime[i])
			{
				// 多倍率时间已到
				m_fCurDouble[i] = 1.0f;
				m_bDouble[i] = FALSE;
			}
		}
	}
}
