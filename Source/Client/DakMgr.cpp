#include "stdafx.h"
#include "DakMgr.h"
#include "CreatureData.h"
#include "QuestEvent.h"
#include "DakFrame.h"

DakMgr::DakMgr( void ):m_Trunk(this)
{
	m_pDakFrame = NULL;
}

DakMgr::~DakMgr( void )
{

}

DakMgr g_DakMgr;
DakMgr* DakMgr::Inst()
{
	return &g_DakMgr;
}

BOOL DakMgr::Init()
{
	// 注册游戏事件
	TObjRef<GameFrameMgr>()->RegisterEventHandle( _T("tagStartNPCTalkEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&DakMgr::OnEvent));

	return TRUE;
}

VOID DakMgr::Destroy()
{

	TObjRef<GameFrameMgr>()->UnRegisterEventHandler( _T("tagStartNPCTalkEvent"), 
		(FRAMEEVENTPROC)m_Trunk.sfp1(&DakMgr::OnEvent));

	if( P_VALID(TObjRef<GameFrameMgr>()->GetFrame(_T("Dak"))) )
		TObjRef<GameFrameMgr>()->DestroyFrame(m_pDakFrame);

	m_pDakFrame = NULL;
}

DWORD DakMgr::OnEvent( tagGameEvent* pEvent )
{
	if( _T("tagStartNPCTalkEvent") == pEvent->strEventName )	// 与NPC对话
	{
		tagStartNPCTalkEvent* pEvt = (tagStartNPCTalkEvent*)pEvent;
		const tagCreatureProto* pCreatureProto = CreatureData::Inst()->FindNpcAtt(pEvt->typeID);
		if(pCreatureProto->eFunctionType==EFNPCT_Dak)
		{
			if(!CreateDakFrame())
				return 0;
			if( P_VALID(m_pDakFrame) )
			{
				m_pDakFrame->SetDakData(pEvt->globalID, pCreatureProto->uFunctionID.dwDakID);
				m_pDakFrame->SetCaption(pCreatureProto->szName);
				m_pDakFrame->ShowWnd();
			}
		}
	}

	return 0;
}

BOOL DakMgr::CreateDakFrame()
{
	//已经打开界面了就不要再打开了
	if( P_VALID((DakFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Dak"))) )
		return FALSE;

	if( !TObjRef<GameFrameMgr>()->CreateFrame(_T("World"), _T("Dak"), _T("DakFrame"), 0) )
		return FALSE;

	m_pDakFrame = (DakFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Dak"));
	return TRUE;
}