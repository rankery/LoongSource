#include "StdAfx.h"
#include "CombatFrame_Cursor.h"
#include "CombatEvent.h"
#include "CombatSysUtil.h"

CombatFrame_Cursor::CombatFrame_Cursor(void)
{
}

CombatFrame_Cursor::~CombatFrame_Cursor(void)
{
}

BOOL CombatFrame_Cursor::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	m_lastRefleshTime = Kernel::Inst()->GetAccumTimeDW();

	return GameFrame::Init(pMgr,this,0);
}

BOOL CombatFrame_Cursor::Destroy()
{
	return 0;
}

VOID CombatFrame_Cursor::Update()
{
	if(Kernel::Inst()->GetAccumTimeDW()-m_lastRefleshTime>1000)
	{
		m_lastRefleshTime=Kernel::Inst()->GetAccumTimeDW();

		//todo
	}
}
