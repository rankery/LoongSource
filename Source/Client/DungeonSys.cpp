/********************************************************************
	created:	2009/02/02
	created:	2:2:2009   11:10
	filename: 	d:\loong_client\Client\DungeonSys.cpp
	file path:	d:\loong_client\Client
	file base:	DungeonSys
	file ext:	cpp
	author:		leili
	
	purpose:	牢狱系统声明
*********************************************************************/

#include "stdafx.h"
#include "DungeonSys.h"
#include "ChatEvent.h"
#include "PlayerNameTab.h"
#include "..\WorldDefine\msg_prison.h"

DungeonSys::DungeonSys( void )
: m_Trunk(this)
, m_flag(0)
, m_srcID(GT_INVALID)
, m_dstID(GT_INVALID)
{

}

DungeonSys::~DungeonSys( void )
{

}

BOOL DungeonSys::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	// 注册网络消息
	m_pCmdMgr->Register("NS_SendToPrison",(NETMSGPROC)m_Trunk.sfp2(&DungeonSys::NetRecvMessage_SendToPrison),_T("送入牢狱"));

	// 注册游戏事件
	pMgr->RegisterEventHandle(_T("tagRoleGetNameEvent"),(FRAMEEVENTPROC)m_Trunk.sfp1(&DungeonSys::OnEvent_GetRoleName));

	return GameFrame::Init(pMgr,pFather,dwParam);
}

BOOL DungeonSys::Destroy()
{
	// 注销网络消息
	m_pCmdMgr->UnRegister("NS_SendToPrison",(NETMSGPROC)m_Trunk.sfp2(&DungeonSys::NetRecvMessage_SendToPrison));

	// 注销游戏事件
	m_pMgr->UnRegisterEventHandler( _T("tagRoleGetNameEvent"),(FRAMEEVENTPROC)m_Trunk.sfp1(&DungeonSys::OnEvent_GetRoleName));

	return GameFrame::Destroy();
}

DWORD DungeonSys::NetRecvMessage_SendToPrison( tagNS_SendToPrison* pMsg, DWORD pParam )
{
	m_flag = EGRN_None;

	m_srcName = PlayerNameTab::Inst()->FindNameByID( pMsg->dwCaptorRoleID );
	if( m_srcName.size() > 0 )
		m_flag |= EGRN_GetSrc;
	else
		m_srcID = pMsg->dwCaptorRoleID;

	m_dstName = PlayerNameTab::Inst()->FindNameByID( pMsg->dwCatchedRoleID );
	if( m_dstName.size() > 0 )
		m_flag |= EGRN_GetDst;
	else
		m_dstID = pMsg->dwCatchedRoleID;

	if( m_flag&EGRN_GetSrc && m_flag&EGRN_GetDst )
		ShowSendToPrisonInfo();

	return 0;
}

VOID DungeonSys::ShowSendToPrisonInfo(void) const
{
	TCHAR szInfo[128];
	_stprintf( szInfo, g_StrTable[_T("Dungeon_SendToPrison")], m_srcName.c_str(), m_dstName.c_str() );
	tagDungeonEvent chatInfo(_T("tagDungeonEvent"), NULL);
	chatInfo.strInfo = szInfo;
	m_pMgr->SendEvent(&chatInfo);
}

DWORD DungeonSys::OnEvent_GetRoleName( tagRoleGetNameEvent* pEvent )
{
	if( !pEvent->bResult )
		return 0;

	if( !(m_flag&EGRN_GetSrc) )
	{
		if( m_srcID == pEvent->dwRoleID )
		{
			m_srcName = pEvent->strRoleName;
			m_flag |= EGRN_GetSrc;
			m_srcID = GT_INVALID;
		}
	}
	else if( !(m_flag&EGRN_GetDst) )
	{
		if( m_dstID == pEvent->dwRoleID )
		{
			m_dstName = pEvent->strRoleName;
			m_flag |= EGRN_GetDst;
			m_dstName = GT_INVALID;
		}
	}

	if( m_flag&EGRN_GetSrc && m_flag&EGRN_GetDst )
		ShowSendToPrisonInfo();

	return 0;
}