#include "StdAfx.h"
#include "CombatFrame_MsgShow.h"
#include "..\WorldDefine\msg_pk.h"
#include "CombatSysUtil.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "NPC.h"
#include "Role.h"
#include "CreatureData.h"
#include "SkillMgr.h"
#include "PlayerNameTab.h"
#include "RoleEvent.h"
#include "SkillProtoData.h"
#include "..\WorldDefine\currency_define.h"

const TCHAR* STR_COL_BROWN = _T("<color=(83,144,213)>");	//棕色
const TCHAR* STR_COL_GREEN = _T("<color=(0,255,0)>");		//绿色
const TCHAR* STR_COL_WHILTE = _T("<color=(218,243,255)>");	//白色
const TCHAR* STR_COL_YELLOW = _T("<color=(70,227,255)>");	//黄色
const TCHAR* STR_COL_BLUE = _T("<color=(255,213,99)>");		//蓝色
const TCHAR* STR_COL_ORANGE = _T("<color=(68,187,255)>");	//橙色
const TCHAR* STR_COL_ZHI = _T("<color=(255,153,222)>");		//紫色


CombatFrame_MsgShow::CombatFrame_MsgShow(void):m_trunk(this)
{
}

CombatFrame_MsgShow::~CombatFrame_MsgShow(void)
{
}

BOOL CombatFrame_MsgShow::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	//--注册网络消息处理函数
	m_pCmdMgr->Register("NS_RoleHPChange",	(NETMSGPROC)m_trunk.sfp2(&CombatFrame_MsgShow::NetRecvMsgNS_RoleHPChange),	_T("NS_RoleHPChange"));
	m_pCmdMgr->Register("NS_Skill",			(NETMSGPROC)m_trunk.sfp2(&CombatFrame_MsgShow::NetRecvMsgNS_Skill),			_T("NS_Skill"));
	m_pCmdMgr->Register("NS_UseItem",		(NETMSGPROC)m_trunk.sfp2(&CombatFrame_MsgShow::NetRecvMsgNS_UseItem),		_T("NS_UseItem"));
	m_pCmdMgr->Register("NS_RoleDead",		(NETMSGPROC)m_trunk.sfp2(&CombatFrame_MsgShow::NetRecvMsgNS_RoleDead),		_T("NS_RoleDead"));

	//--注册游戏事件
	pMgr->RegisterEventHandle( _T("tagUpdateLPLevelExp"), (FRAMEEVENTPROC)m_trunk.sfp1(&CombatFrame_MsgShow::OnUpdateLPLevelExp));
	pMgr->RegisterEventHandle( _T("CombatMsg_ItemAdd"), (FRAMEEVENTPROC)m_trunk.sfp1(&CombatFrame_MsgShow::OnCombatMsg_ItemAdd));
	pMgr->RegisterEventHandle(_T("CombatMsg_ItemRemove"), (FRAMEEVENTPROC)m_trunk.sfp1(&CombatFrame_MsgShow::OnCombatMsg_ItemRemove));
	pMgr->RegisterEventHandle(_T("Bag_Silver"), (FRAMEEVENTPROC)m_trunk.sfp1(&CombatFrame_MsgShow::OnBag_Silver));
	return GameFrame::Init(pMgr,this,0);
}

BOOL CombatFrame_MsgShow::Destroy()
{
	//--注销网络消息处理函数
	m_pCmdMgr->UnRegister("NS_RoleHPChange",	(NETMSGPROC)m_trunk.sfp2(&CombatFrame_MsgShow::NetRecvMsgNS_RoleHPChange));
	m_pCmdMgr->UnRegister("NS_Skill",			(NETMSGPROC)m_trunk.sfp2(&CombatFrame_MsgShow::NetRecvMsgNS_Skill));
	m_pCmdMgr->UnRegister("NS_UseItem",			(NETMSGPROC)m_trunk.sfp2(&CombatFrame_MsgShow::NetRecvMsgNS_UseItem));
	m_pCmdMgr->UnRegister("NS_RoleDead",		(NETMSGPROC)m_trunk.sfp2(&CombatFrame_MsgShow::NetRecvMsgNS_RoleDead));

	//--注销游戏事件
	m_pMgr->UnRegisterEventHandler( _T("tagUpdateLPLevelExp"), (FRAMEEVENTPROC)m_trunk.sfp1(&CombatFrame_MsgShow::OnUpdateLPLevelExp));
	m_pMgr->UnRegisterEventHandler( _T("CombatMsg_ItemAdd"), (FRAMEEVENTPROC)m_trunk.sfp1(&CombatFrame_MsgShow::OnCombatMsg_ItemAdd));
	m_pMgr->UnRegisterEventHandler(_T("CombatMsg_ItemRemove"), (FRAMEEVENTPROC)m_trunk.sfp1(&CombatFrame_MsgShow::OnCombatMsg_ItemRemove));
	m_pMgr->UnRegisterEventHandler(_T("Bag_Silver"), (FRAMEEVENTPROC)m_trunk.sfp1(&CombatFrame_MsgShow::OnBag_Silver));
	return 0;
}

VOID CombatFrame_MsgShow::Update()
{

}

DWORD CombatFrame_MsgShow::OnUpdateLPLevelExp( tagUpdateLPLevelExp* pEvent )
{
	TCHAR szText[X_LONG_NAME];
	if(pEvent->nExpInc > 0)
	{
		_stprintf(szText, g_StrTable[_T("CombatForChat11")], pEvent->nExpInc);
		CombatSysUtil::Inst()->ShowCombatMsg(szText);
	}
	else if(pEvent->nExpInc < 0)
	{
		_stprintf(szText, g_StrTable[_T("CombatForChat12")], abs(pEvent->nExpInc));
		CombatSysUtil::Inst()->ShowCombatMsg(szText);
	}
	return 0;
}

DWORD CombatFrame_MsgShow::NetRecvMsgNS_RoleDead(tagNS_RoleDead* pMsg, DWORD dwParam)
{
	DWORD dwLocalID = RoleMgr::Inst()->GetLocalPlayerID();
		TCHAR szText[X_LONG_NAME];
	if(pMsg->dwRoleID == dwLocalID)
	{
		//被怪杀死
		if( IS_CREATURE(pMsg->dwSrcRoleID) )
		{
			Role* pRole = RoleMgr::Inst()->FindRole( pMsg->dwSrcRoleID );
			if( !P_VALID(pRole) )
				return 0;
			NPC *pNpc = (NPC*)pRole;
			DWORD dwTypeID = pNpc->GetTypeID();
			const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(dwTypeID);
			if( !P_VALID(pCreature) )
				return 0;

			_stprintf(szText, g_StrTable[_T("CombatForChat13")], pCreature->szName);
			CombatSysUtil::Inst()->ShowCombatMsg(szText);
		}
		else if( IS_PLAYER(pMsg->dwSrcRoleID) )
		{
			tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwSrcRoleID);
			if(strPlayerName!=_T(""))
			{
				_stprintf(szText, g_StrTable[_T("CombatForChat13")], strPlayerName.c_str());
				CombatSysUtil::Inst()->ShowCombatMsg(szText);
			}
		}
		
		// 如果在跑商状态下
		if (RoleMgr::Inst()->GetLocalPlayer()->GetRoleState(ERS_Commerce))
		{
			CombatSysUtil::Inst()->ShowCombatMsg(g_StrTable[_T("CombatForChat21")]);
		}
	

	}
	else if(pMsg->dwSrcRoleID == dwLocalID)
	{
		if( IS_PLAYER(pMsg->dwRoleID) )
		{
			tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID);
			if(strPlayerName!=_T(""))
			{
				_stprintf(szText, g_StrTable[_T("CombatForChat14")], strPlayerName.c_str());
				CombatSysUtil::Inst()->ShowCombatMsg(szText);
			}
		}	
		else if( IS_CREATURE(pMsg->dwRoleID) )
		{
			Role* pRole = RoleMgr::Inst()->FindRole( pMsg->dwRoleID );
			if( !P_VALID(pRole) )
				return 0;
			NPC *pNpc = (NPC*)pRole;
			DWORD dwTypeID = pNpc->GetTypeID();
			const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(dwTypeID);
			if( !P_VALID(pCreature) )
				return 0;

			//采集不显示
			if( pCreature->eType==ECT_GameObject
				/*&& pCreature->nType2==EGOT_Gather*/ )
				return 0;

			_stprintf(szText, g_StrTable[_T("CombatForChat14")], pCreature->szName);
			CombatSysUtil::Inst()->ShowCombatMsg(szText);
		}
	}
	return 0;
}

DWORD CombatFrame_MsgShow::NetRecvMsgNS_Skill(tagNS_Skill* pMsg, DWORD dwParam)
{
	DWORD dwLocalID = RoleMgr::Inst()->GetLocalPlayerID();
	if(pMsg->dwErrorCode!=E_Success)
	{
		//显示错误信息屏显
		CombatSysUtil::Inst()->ShowSkillSvrErrMsg(pMsg->dwErrorCode);
		return 0;
	}


	TCHAR szText[X_LONG_NAME];
	if(pMsg->dwSrcRoleID == dwLocalID)
	{
		if( IS_CREATURE(pMsg->dwTarRoleID) )
		{
			Role* pRole = RoleMgr::Inst()->FindRole( pMsg->dwTarRoleID );
			if( !P_VALID(pRole) )
				return 0;
			NPC *pNpc = (NPC*)pRole;
			DWORD dwTypeID = pNpc->GetTypeID();
			const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(dwTypeID);
			if( !P_VALID(pCreature) )
				return 0;

			const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillDataByTypeID(pMsg->dwSkillID);
			if( !P_VALID(pSkill) )
				return 0;

			if( pCreature->eType==ECT_GameObject
				|| pSkill->pProto->nType2 == ESSTE_Produce )
				return 0;

			_stprintf(szText, g_StrTable[_T("CombatForChat8")], pCreature->szName, pSkill->pProto->szName);
			CombatSysUtil::Inst()->ShowCombatMsg(szText);
		}
		else if( IS_PLAYER(pMsg->dwTarRoleID) )
		{
			tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwTarRoleID);
			if(strPlayerName!=_T(""))
			{
				const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillDataByTypeID(pMsg->dwSkillID);
				if( !P_VALID(pSkill) )
					return 0;

				_stprintf(szText, g_StrTable[_T("CombatForChat8")], strPlayerName.c_str(), pSkill->pProto->szName);
				CombatSysUtil::Inst()->ShowCombatMsg(szText);
			}
		}
	}
	return 0;
}

DWORD CombatFrame_MsgShow::NetRecvMsgNS_RoleHPChange(tagNS_RoleHPChange* pMsg, DWORD dwParam)
{
	DWORD dwLocalID = RoleMgr::Inst()->GetLocalPlayerID();
	TCHAR szText[X_LONG_NAME];
	TCHAR szBuff[X_SHORT_NAME] = {0};
	if(pMsg->nHPChange < 0)
	{
		if(pMsg->bCrit)
		{
			//伤害类型
			if( pMsg->eCause == ERHPCC_SkillDamage )
			{
				const tagSkillProtoClient* pSkill = SkillProtoData::Inst()->FindSkillProto( pMsg->dwMisc );
				if( P_VALID(pSkill) )
				{
					switch(pSkill->eDmgType)
					{
					case ESDGT_Ordinary:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_1")] );
						break;
					case ESDGT_Bleeding:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_2")] );
						break;
					case ESDGT_Brunt:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_3")] );
						break;
					case ESDGT_Bang:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_4")] );
						break;
					case ESDGT_Poison:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_5")] );
						break;
					case ESDGT_Thinker:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_6")] );
						break;
					case ESDGT_Injury:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_7")] );
						break;
					case ESDGT_Stunt:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_8")] );
						break;
					}
				}
			}

			//对其他玩家或怪造成暴击
			if(pMsg->dwSrcRoleID == dwLocalID)
			{
				if( IS_CREATURE(pMsg->dwRoleID) )
				{
					Role* pRole = RoleMgr::Inst()->FindRole( pMsg->dwRoleID );
					if( !P_VALID(pRole) )
						return 0;
					NPC *pNpc = (NPC*)pRole;
					DWORD dwTypeID = pNpc->GetTypeID();
					const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(dwTypeID);
					if( !P_VALID(pCreature) )
						return 0;
					
					_stprintf(szText, g_StrTable[_T("CombatForChat3")], pCreature->szName, abs(pMsg->nHPChange), szBuff);
					CombatSysUtil::Inst()->ShowCombatMsg(szText);
				}
				else if( IS_PLAYER(pMsg->dwRoleID) )
				{
					tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID);
					if(strPlayerName!=_T(""))
					{
						_stprintf(szText, g_StrTable[_T("CombatForChat3")], strPlayerName.c_str(), abs(pMsg->nHPChange), szBuff);
						CombatSysUtil::Inst()->ShowCombatMsg(szText);
					}
				}	// end if 
			} 
			// 怪或其他玩家对我造成的爆击
			else if(pMsg->dwRoleID == dwLocalID)
			{
				if( IS_CREATURE(pMsg->dwSrcRoleID) )
				{
					Role* pRole = RoleMgr::Inst()->FindRole( pMsg->dwSrcRoleID );
					if( !P_VALID(pRole) )
						return 0;
					NPC *pNpc = (NPC*)pRole;
					DWORD dwTypeID = pNpc->GetTypeID();
					const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(dwTypeID);
					if( !P_VALID(pCreature) )
						return 0;

					_stprintf(szText, g_StrTable[_T("CombatForChat4")], pCreature->szName, abs(pMsg->nHPChange), szBuff);
					CombatSysUtil::Inst()->ShowCombatMsg(szText);
				}
				else if( IS_PLAYER(pMsg->dwSrcRoleID) )
				{
					tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwSrcRoleID);
					if(strPlayerName!=_T(""))
					{
						_stprintf(szText, g_StrTable[_T("CombatForChat4")], strPlayerName.c_str(), abs(pMsg->nHPChange), szBuff);
						CombatSysUtil::Inst()->ShowCombatMsg(szText);
					}
				}	
			} 
		} // end if 爆击
		//普通攻击
		else 
		{
			//伤害类型
			if( pMsg->eCause == ERHPCC_SkillDamage )
			{
				const tagSkillProtoClient* pSkill = SkillProtoData::Inst()->FindSkillProto( pMsg->dwMisc );
				if( P_VALID(pSkill) )
				{
					switch(pSkill->eDmgType)
					{
					case ESDGT_Ordinary:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_1")] );
						break;
					case ESDGT_Bleeding:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_2")] );
						break;
					case ESDGT_Brunt:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_3")] );
						break;
					case ESDGT_Bang:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_4")] );
						break;
					case ESDGT_Poison:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_5")] );
						break;
					case ESDGT_Thinker:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_6")] );
						break;
					case ESDGT_Injury:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_7")] );
						break;
					case ESDGT_Stunt:
						_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("ESkillDmgType_8")] );
						break;
					}
				}
			}

			// 对其他玩家或怪的普通攻击
			if(pMsg->dwSrcRoleID == dwLocalID)
			{
				if( IS_CREATURE(pMsg->dwRoleID) )
				{
					Role* pRole = RoleMgr::Inst()->FindRole( pMsg->dwRoleID );
					if( !P_VALID(pRole) )
						return 0;
					NPC *pNpc = (NPC*)pRole;
					DWORD dwTypeID = pNpc->GetTypeID();
					const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(dwTypeID);
					if( !P_VALID(pCreature) )
						return 0;

					_stprintf(szText, g_StrTable[_T("CombatForChat1")], pCreature->szName, abs(pMsg->nHPChange), szBuff);
					CombatSysUtil::Inst()->ShowCombatMsg(szText);
				}
				else if( IS_PLAYER(pMsg->dwRoleID) )
				{
					tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwRoleID);
					if(strPlayerName!=_T(""))
					{
						_stprintf(szText, g_StrTable[_T("CombatForChat1")], strPlayerName.c_str(), abs(pMsg->nHPChange), szBuff);
						CombatSysUtil::Inst()->ShowCombatMsg(szText);
					}
				}	// end if 
			} 
			// 其他玩家或怪对我的普通攻击
			else if(pMsg->dwRoleID == dwLocalID)
			{
				if( IS_CREATURE(pMsg->dwSrcRoleID) )
				{
					Role* pRole = RoleMgr::Inst()->FindRole( pMsg->dwSrcRoleID );
					if( !P_VALID(pRole) )
						return 0;
					NPC *pNpc = (NPC*)pRole;
					DWORD dwTypeID = pNpc->GetTypeID();
					const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(dwTypeID);
					if( !P_VALID(pCreature) )
						return 0;

					_stprintf(szText, g_StrTable[_T("CombatForChat2")], pCreature->szName, abs(pMsg->nHPChange), szBuff);
					CombatSysUtil::Inst()->ShowCombatMsg(szText);
				}
				else if( IS_PLAYER(pMsg->dwSrcRoleID) )
				{
					tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwSrcRoleID);
					if(strPlayerName!=_T(""))
					{
						_stprintf(szText, g_StrTable[_T("CombatForChat2")], strPlayerName.c_str(), abs(pMsg->nHPChange), szBuff);
						CombatSysUtil::Inst()->ShowCombatMsg(szText);
					}
				}	
			} 
		}// end if 普通攻击
	} // end if dwHPChange<0
	else if(pMsg->nHPChange == 0)
	{
		//躲闪
		if(pMsg->bMiss)	
		{
			// 攻击未命中
			if(pMsg->dwSrcRoleID == dwLocalID)
			{
				_stprintf(szText, g_StrTable[_T("CombatForChat6")]);
				CombatSysUtil::Inst()->ShowCombatMsg(szText);
			}
			// 躲闪了其他玩家或怪的攻击
			else if(pMsg->dwRoleID == dwLocalID)
			{
				if( IS_CREATURE(pMsg->dwSrcRoleID) )
				{
					Role* pRole = RoleMgr::Inst()->FindRole( pMsg->dwSrcRoleID );
					if( !P_VALID(pRole) )
						return 0;
					NPC *pNpc = (NPC*)pRole;
					DWORD dwTypeID = pNpc->GetTypeID();
					const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(dwTypeID);
					if( !P_VALID(pCreature) )
						return 0;

					_stprintf(szText, g_StrTable[_T("CombatForChat5")], pCreature->szName);
					CombatSysUtil::Inst()->ShowCombatMsg(szText);
				}
				else if( IS_PLAYER(pMsg->dwSrcRoleID) )
				{
					tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwSrcRoleID);
					if(strPlayerName!=_T(""))
					{
						_stprintf(szText, g_StrTable[_T("CombatForChat5")], strPlayerName.c_str());
						CombatSysUtil::Inst()->ShowCombatMsg(szText);
					}
				}
			}
		} 
		// 格挡
		else if(pMsg->bBlocked)
		{
			// 攻击未命中
			if(pMsg->dwSrcRoleID == dwLocalID)
			{
				_stprintf(szText, g_StrTable[_T("CombatForChat6")]);
				CombatSysUtil::Inst()->ShowCombatMsg(szText);
			}
			else if(pMsg->dwRoleID == dwLocalID)
			{
				if( IS_CREATURE(pMsg->dwSrcRoleID) )
				{
					Role* pRole = RoleMgr::Inst()->FindRole( pMsg->dwSrcRoleID );
					if( !P_VALID(pRole) )
						return 0;
					NPC *pNpc = (NPC*)pRole;
					DWORD dwTypeID = pNpc->GetTypeID();
					const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(dwTypeID);
					if( !P_VALID(pCreature) )
						return 0;

					_stprintf(szText, g_StrTable[_T("CombatForChat7")], pCreature->szName);
					CombatSysUtil::Inst()->ShowCombatMsg(szText);
				}
				else if( IS_PLAYER(pMsg->dwSrcRoleID) )
				{
					tstring strPlayerName = PlayerNameTab::Inst()->FindNameByID(pMsg->dwSrcRoleID);
					if(strPlayerName!=_T(""))
					{
						_stprintf(szText, g_StrTable[_T("CombatForChat7")], strPlayerName.c_str());
						CombatSysUtil::Inst()->ShowCombatMsg(szText);
					}
				}
			}
		} // end if 
	}// end if dwHPChange==0
	return 0;
}

DWORD CombatFrame_MsgShow::NetRecvMsgNS_UseItem( tagNS_UseItem* pMsg, DWORD dwParam )
{
	if(pMsg->dwErrorCode!=E_Success)
	{
		//显示错误信息屏显
		CombatSysUtil::Inst()->ShowItemSvrErrMsg(pMsg->dwErrorCode);
	}
	return 0;
}

DWORD CombatFrame_MsgShow::OnCombatMsg_ItemAdd( tagCombatMsgForItemChange* pEvent )
{
	const tagItemProto* pItem = ItemProtoData::Inst()->FindItemProto(pEvent->dwTypeID);
	if( P_VALID(pItem) )
	{
		tstring strItemName = pItem->pDisplayInfo->szName;
		TCHAR szBuff[X_LONG_NAME];
		INT16 n16Num =  pEvent->n16Num;
		if( n16Num>1 )
		{
			_stprintf(szBuff, g_StrTable[_T("CombatForChat15")], strItemName.c_str());
			CombatSysUtil::Inst()->ShowCombatMsg(szBuff);
			CombatSysUtil::Inst()->ShowScreenCenterMsg(szBuff);
		}
		else if( n16Num==1 )
		{
			_stprintf(szBuff, g_StrTable[_T("CombatForChat15")], strItemName.c_str());
			CombatSysUtil::Inst()->ShowCombatMsg(szBuff);
			CombatSysUtil::Inst()->ShowScreenCenterMsg(szBuff);
		}
		//m_pLog->Write(szBuff);
	}
	return 0;
}

DWORD CombatFrame_MsgShow::OnCombatMsg_ItemRemove( tagCombatMsgForItemChange* pEvent )
{
	const tagItemProto* pItem = ItemProtoData::Inst()->FindItemProto(pEvent->dwTypeID);
	if( P_VALID(pItem) )
	{
		tstring strItemName = pItem->pDisplayInfo->szName;
		TCHAR szBuff[X_LONG_NAME];
		INT16 n16Num =  pEvent->n16Num;
		if( n16Num>1 )
		{
			_stprintf(szBuff, g_StrTable[_T("CombatForChat16")], strItemName.c_str());
			CombatSysUtil::Inst()->ShowCombatMsg(szBuff);
		}
		else if( n16Num==1 )
		{
			_stprintf(szBuff, g_StrTable[_T("CombatForChat16")], strItemName.c_str());
			CombatSysUtil::Inst()->ShowCombatMsg(szBuff);
		}
		//m_pLog->Write(szBuff);
	}
	return 0;
}

DWORD CombatFrame_MsgShow::OnBag_Silver( tagRoleSilverChange* pEvent )
{
	INT64 n64Money = pEvent->n64Inc;
	TCHAR szBuff[X_LONG_NAME];
	if( n64Money > 0 )
	{
		INT nGold = MSilver2DBGold(n64Money);
		INT nSilver = MSilver2DBSilver(n64Money);
		if( nGold==0 )
		{
			_stprintf(szBuff, g_StrTable[_T("CombatForChat19")], nSilver);
		}
		else
		{
			_stprintf(szBuff, g_StrTable[_T("CombatForChat17")], nGold, nSilver);
		}
		
		CombatSysUtil::Inst()->ShowCombatMsg(szBuff);
	}
	else if( n64Money < 0 )
	{
		INT nGold =MSilver2DBGold(_abs64(n64Money));
		INT nSilver = MSilver2DBSilver(_abs64(n64Money));
		if( nGold == 0 )
		{
			_stprintf(szBuff, g_StrTable[_T("CombatForChat20")], nSilver);
		}
		else
		{
			_stprintf(szBuff, g_StrTable[_T("CombatForChat18")], nGold, nSilver);
		}
		CombatSysUtil::Inst()->ShowCombatMsg(szBuff);
	}
	//m_pLog->Write(szBuff);
	return 0;
}