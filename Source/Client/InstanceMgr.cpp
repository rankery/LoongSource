#include "stdafx.h"
#include "InstanceMgr.h"
#include "TeamSys.h"
#include "MapLogicData.h"
#include "CountDownFrame.h"
#include "QuestTargetWnd.h"
#include "InstanceProtoData.h"
#include "MapMgr.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "MapLogicData.h"
#include "CombatSysUtil.h"
#include "InstanceQuitFrame.h"
#include "WorldFrame.h"

InstanceMgr::InstanceMgr()
: m_trunk(this)
, m_pVarContainer(NULL)
, m_pNetCmdMgr(NULL)
, m_pGameFrameMgr(NULL)
, m_pSession(NULL)
, m_pCountDownFrame(NULL)
, m_pQuestTargetWnd(NULL)
, m_eHardMode(EIHM_Normal)
{
}


InstanceMgr* InstanceMgr::Inst()
{
	static InstanceMgr inst;
	return &inst;
}

BOOL InstanceMgr::Init()
{
	// 注册网络消息处理
	m_pNetCmdMgr->Register("NS_InstanceNofity", 
		(NETMSGPROC)(m_trunk.sfp2(&InstanceMgr::OnNS_InstanceNofity)), 
		_T("NS_InstanceNofity"));

	m_pNetCmdMgr->Register("NS_InstanceAgree", 
		(NETMSGPROC)(m_trunk.sfp2(&InstanceMgr::OnNS_InstanceAgree)), 
		_T("NS_InstanceAgree"));

	m_pNetCmdMgr->Register("NS_InstanceComplete", 
		(NETMSGPROC)(m_trunk.sfp2(&InstanceMgr::OnNS_InstanceComplete)), 
		_T("NS_InstanceComplete"));

	m_pNetCmdMgr->Register("NS_EnterInstance", 
		(NETMSGPROC)(m_trunk.sfp2(&InstanceMgr::OnNS_EnterInstance)), 
		_T("NS_EnterInstance"));

	// 注册游戏事件处理
	m_pGameFrameMgr->RegisterEventHandle(_T("OnInstanceNotifyBox"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnInstanceNotifyBox)));

	m_pGameFrameMgr->RegisterEventHandle(_T("OnInstanceAgreeBox"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnInstanceAgreeBox)));

	m_pGameFrameMgr->RegisterEventHandle(_T("OnInstanceCompleteBox"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnInstanceCompleteBox)));

	m_pGameFrameMgr->RegisterEventHandle(_T("OnAskEnterMsgBox"), 
		(FRAMEEVENTPROC(m_trunk.sfp1(&InstanceMgr::OnAskEnterMsgBox))));

	m_pGameFrameMgr->RegisterEventHandle(_T("OnAskOutMsgBox"), 
		(FRAMEEVENTPROC(m_trunk.sfp1(&InstanceMgr::OnAskOutMsgBox))));
	
	m_pGameFrameMgr->RegisterEventHandle(_T("OnInstanceDifficultyBox"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnInstanceDifficultyBox)));

	m_pGameFrameMgr->RegisterEventHandle(_T("EnterInstance"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnEnterInstanceEvent)));

	m_pGameFrameMgr->RegisterEventHandle(_T("LeaveInstance"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnLeaveInstanceEvent)));

	m_pGameFrameMgr->RegisterEventHandle(_T("OnActiveTransmitPoint"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnActiveTransmitPoint)));

	m_pGameFrameMgr->RegisterEventHandle(_T("tagLeaveTeamEvent"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnLeaveTeamEvent)));

	if (!LoadInstanceProtoTable())
		return FALSE;

	return TRUE;
}

VOID InstanceMgr::Destroy()
{
	m_pNetCmdMgr->UnRegister("NS_InstanceNofity", (NETMSGPROC)(m_trunk.sfp2(&InstanceMgr::OnNS_InstanceNofity)));

	m_pNetCmdMgr->UnRegister("NS_InstanceAgree", (NETMSGPROC)(m_trunk.sfp2(&InstanceMgr::OnNS_InstanceAgree)));

	m_pNetCmdMgr->UnRegister("NS_InstanceComplete", (NETMSGPROC)(m_trunk.sfp2(&InstanceMgr::OnNS_InstanceComplete)));

	m_pNetCmdMgr->UnRegister("NS_EnterInstance", (NETMSGPROC)(m_trunk.sfp2(&InstanceMgr::OnNS_EnterInstance)));

	m_pGameFrameMgr->UnRegisterEventHandler(_T("OnInstanceNotifyBox"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnInstanceNotifyBox)));

	m_pGameFrameMgr->UnRegisterEventHandler(_T("OnInstanceAgreeBox"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnInstanceAgreeBox)));

	m_pGameFrameMgr->UnRegisterEventHandler(_T("OnInstanceCompleteBox"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnInstanceCompleteBox)));

	m_pGameFrameMgr->UnRegisterEventHandler(_T("OnAskEnterMsgBox"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnAskEnterMsgBox)));

	m_pGameFrameMgr->UnRegisterEventHandler(_T("OnAskOutMsgBox"), 
		(FRAMEEVENTPROC(m_trunk.sfp1(&InstanceMgr::OnAskOutMsgBox))));

	m_pGameFrameMgr->UnRegisterEventHandler(_T("OnInstanceDifficultyBox"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnInstanceDifficultyBox)));

	m_pGameFrameMgr->UnRegisterEventHandler(_T("EnterInstance"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnEnterInstanceEvent)));

	m_pGameFrameMgr->UnRegisterEventHandler(_T("LeaveInstance"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnLeaveInstanceEvent)));

	m_pGameFrameMgr->UnRegisterEventHandler(_T("OnActiveTransmitPoint"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnActiveTransmitPoint)));

	m_pGameFrameMgr->UnRegisterEventHandler(_T("tagLeaveTeamEvent"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&InstanceMgr::OnLeaveTeamEvent)));

	if (P_VALID(GetObj("InstanceNotifyBox")))
		KillObj("InstanceNotifyBox");

	if (P_VALID(GetObj("InstanceAgreeBox")))
		KillObj("InstanceAgreeBox");

	if (P_VALID(GetObj("InstanceCompleteBox")))
		KillObj("InstanceCompleteBox");

	if (P_VALID(GetObj("InstanceLevelLowerLimitBox")))
		KillObj("InstanceLevelLowerLimitBox");

	if (P_VALID(GetObj("InstanceLevelUpperLimitBox")))
		KillObj("InstanceLevelUpperLimitBox");

	if (P_VALID(GetObj("InstanceDifficultyBox")))
		KillObj("InstanceDifficultyBox");

	if (P_VALID(GetObj("InstanceRoleFullBox")))
		KillObj("InstanceRoleFullBox");

	if (P_VALID(GetObj("InstanceRoleLackBox")))
		KillObj("InstanceRoleLackBox");

	if (P_VALID(GetObj("AskEnterMsgBox2")))
		KillObj("AskEnterMsgBox2");

	if (P_VALID(m_pQuestTargetWnd))
	{
		m_pQuestTargetWnd->Destroy();
		delete m_pQuestTargetWnd;
		m_pQuestTargetWnd = NULL;	
	}
}

BOOL InstanceMgr::LoadInstanceProtoTable()
{
	LPCTSTR szPath = _T("data\\system\\attdata\\instance_proto.xml");

	if (!(m_instanceProtoData.LoadFromFile(szPath)))
		return FALSE;

	return TRUE;
}

VOID InstanceMgr::TryGotoInstance(DWORD dwTriggerID, DWORD dwMapID)
{
	Reset();
	m_dwTargetMapID = dwMapID;
	m_dwTriggerID = dwTriggerID;
	if (!IsAskBeforeEnter())		//如果进入时不询问
	{
		if (!IsAbleToGotoInstance())	//如果不满足进入副本的条件
		{
			return;
		}
		else
		{
			Send_TriggerInstance(dwTriggerID, static_cast<DWORD>(EIHM_Normal));
		}
	}
	else
	{
		CreateAskEnterMsgBox();
	}
}

VOID InstanceMgr::TryOutInstance(DWORD dwTriggerID, DWORD dwMapID)
{
	m_dwOutTargetMapID = dwMapID;
	m_dwOutTriggerID = dwTriggerID;

	CreateAskOutMsgBox();
}




VOID InstanceMgr::Send_TriggerInstance(DWORD dwTriggerID, DWORD dwLevel)
{
	if (m_pSession->IsConnect())
	{
		tagNC_RoleMapTrigger cmd;
		cmd.dwTriggerID = dwTriggerID;
		cmd.dwMisc = dwLevel;
		m_pSession->Send(&cmd);
	}
}

VOID InstanceMgr::Send_NotifyTeammate(bool bNofify)
{
	if (m_pSession->IsConnect())
	{
		tagNC_InstanceNotify m;
		m.bNotify = bNofify;
		m_pSession->Send(&m);
	}
}

VOID InstanceMgr::Send_AgreeGotoInstance(bool bAgree)
{
	if (m_pSession->IsConnect())
	{
		tagNC_InstanceAgree m;
		m.bAgree = bAgree;
		m_pSession->Send(&m);
	}
}

VOID InstanceMgr::Send_RequestLeaveInstance()
{
	if (m_pSession->IsConnect())
	{
		tagNC_LeaveInstance m;
		m_pSession->Send(&m);
	}
}

VOID InstanceMgr::CreateQuestTargetWindow()
{
	if (!P_VALID(m_pQuestTargetWnd))
	{
		m_pQuestTargetWnd = (QuestTargetWnd *)m_pGameFrameMgr->CreateFrame( GetWorldFrm()->GetName().c_str(), _T("QuestTargetWindow"), _T("QuestTargetWnd"), 0);
	}
	m_pQuestTargetWnd->Show();
}

VOID InstanceMgr::DestroyQuestTargetWindow()
{
	if (P_VALID(m_pQuestTargetWnd))
	{
		m_pQuestTargetWnd->Hide();
	}
}

DWORD InstanceMgr::OnNS_InstanceNofity(tagNS_InstanceNofity *pCmd, DWORD)
{
	if (P_VALID(GetObj("InstanceNotifyBox")))
		KillObj("InstanceNotifyBox");
	CreateObj("InstanceNotifyBox", "MsgBox");
	TObjRef<MsgBox>("InstanceNotifyBox")->Init(_T(""), 
		g_StrTable[_T("Instance_NotifyTeamnate")], _T("OnInstanceNotifyBox"), 
		(MsgBoxFlag)(MBF_OK | MBF_Cancel), TRUE);

	return 0;

}

DWORD InstanceMgr::OnNS_InstanceAgree(tagNS_InstanceAgree *pCmd, DWORD)
{
	Reset();

	// 如果不在队伍，直接返回
	if (TeamSys::Inst()->IsInTeam() == false)
		return 0;
	
	if (RoleMgr::Inst()->GetLocalPlayer()->GetRoleState(ERS_Commerce) == true)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Instance_Commerce")]);
		return 0;
	}
	if (RoleMgr::Inst()->GetLocalPlayer()->GetRoleState(ERS_PrisonArea) == true)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Instance_PrisonArea")]);
		return 0;
	}


	m_dwTargetMapID = pCmd->dwMapID;
	m_eHardMode = static_cast<EInstanceHardMode>(pCmd->dwMisc);
	tstring strInviterName = TeamSys::Inst()->GetTeammateNameByID(pCmd->dwInviterID);
	tstring strMapShowName = MapLogicData::Inst()->FindMapShowName(pCmd->dwMapID);


	if (P_VALID(GetObj("InstanceAgreeBox")))
		KillObj("InstanceAgreeBox");
	CreateObj("InstanceAgreeBox", "MsgBox");
	TCHAR szText[512];
	_stprintf(szText, g_StrTable[_T("Instance_AgreeGotoInstance")], 
		strInviterName.c_str(), strMapShowName.c_str());

	TObjRef<MsgBox>("InstanceAgreeBox")->Init(_T(""), szText, _T("OnInstanceAgreeBox"), 
		(MsgBoxFlag)(MBF_OK | MBF_Cancel), TRUE);

	return 0;
}

DWORD InstanceMgr::OnNS_InstanceComplete(tagNS_InstanceComplete *pCmd, DWORD)
{
	if (P_VALID(GetObj("InstanceCompleteBox")))
		KillObj("InstanceCompleteBox");
	CreateObj("InstanceCompleteBox", "MsgBox");
	TObjRef<MsgBox>("InstanceCompleteBox")->Init(_T(""), g_StrTable[_T("Instance_Complete")], 
		_T("OnInstanceCompleteBox"), MBF_OK, TRUE);

	return 0;
}

DWORD InstanceMgr::OnNS_EnterInstance(tagNS_EnterInstance *pCmd, DWORD)
{
	if (pCmd->dwErrorCode == E_Success)
	{
		if (pCmd->dwTimeLimit == GT_INVALID)
			return 0;
		m_pCountDownFrame = (CountDownFrame*)(m_pGameFrameMgr->GetFrame(_T("CountDown")));
		if (!P_VALID(m_pCountDownFrame))
		{
			m_pCountDownFrame = (CountDownFrame*)(m_pGameFrameMgr->CreateFrame(_T("World"), _T("CountDown"), _T("CountDownFrame"), 0));
			if (!P_VALID(m_pCountDownFrame))
				return 0;
		}
		else
		{
			m_pCountDownFrame->Show();
		}
		m_pCountDownFrame->SetCDTime(pCmd->dwTimeLimit * 1000);
		return 0;
	}
	else if (pCmd->dwErrorCode == E_Instance_Role_Full)
	{
		if (P_VALID(GetObj("InstanceRoleFullBox")))
			KillObj("InstanceRoleFullBox");
		CreateObj("InstanceRoleFullBox", "MsgBox");
		TObjRef<MsgBox>("InstanceRoleFullBox")->Init(_T(""), g_StrTable[_T("Instance_Num_Upper_Limit")], 
			NULL, MBF_OK, TRUE);	
	}
	else if (pCmd->dwErrorCode == E_Instance_Role_Lack)
	{
		if (P_VALID(GetObj("InstanceRoleLackBox")))
			KillObj("InstanceRoleLackBox");
		CreateObj("InstanceRoleLackBox", "MsgBox");

		const tagInstanceProtoData *pIPData = m_instanceProtoData.FindInstanceProtoData(m_dwTargetMapID);
		ASSERT(pIPData);
		if (!P_VALID(pIPData))
			return 0;
		TCHAR szText[512];
		_stprintf(szText, g_StrTable[_T("Instance_Num_Lower_Limit")], pIPData->nNumLowerLimit);

		TObjRef<MsgBox>("InstanceRoleLackBox")->Init(_T(""), szText, NULL, MBF_OK, TRUE);
	}
	else if (pCmd->dwErrorCode == E_Instance_Not_Exit)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Instance_Not_Exit")]);
	}
	else if (pCmd->dwErrorCode == E_Instance_Role_Not_Exit)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Instance_Role_Not_Exit")]);
	}
	else if (pCmd->dwErrorCode == E_Instance_Level_Down_Limit)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Instance_Level_Lower_limit")]);
	}
	else if (pCmd->dwErrorCode == E_Instance_Level_Up_Limit)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Instance_Level_Upper_Limit")]);
	}
	else if (pCmd->dwErrorCode == E_Instance_Not_Same_Team)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Instance_Not_Same_Team")]);
	}
	else if (pCmd->dwErrorCode == E_Instance_End_Delete)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Instance_End_Delete")]);
	}
	else if (pCmd->dwErrorCode == E_Instance_Already)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Instance_Already")]);
	}
	else if (pCmd->dwErrorCode == E_Instance_Full)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Instance_Full")]);
	}
	return 0;
}

DWORD InstanceMgr::OnInstanceNotifyBox(tagMsgBoxEvent *pEvent)
{
	switch (pEvent->eResult)
	{
	case MBF_OK:
		{
			Send_NotifyTeammate(true);
		} break;
	case MBF_Cancel:
		{
			Send_NotifyTeammate(false);
		} break;
	default: break;
	}
	return 0;
}

DWORD InstanceMgr::OnInstanceAgreeBox(tagMsgBoxEvent *pEvent)
{
	switch (pEvent->eResult)
	{
	case MBF_OK:
		{
			if (!IsAbleToGotoInstance())
				return 0;
			Send_AgreeGotoInstance(true);
		} break;
	case MBF_Cancel:
		{
			Reset();
			Send_AgreeGotoInstance(false);

		} break;
	default:
		break;
	}
	return 0;
}

DWORD InstanceMgr::OnInstanceCompleteBox(tagMsgBoxEvent *pEvent)
{
	return 0;
}

BOOL InstanceMgr::IsAbleToGotoInstance()
{
	/*!
	\brief: 根据副本属性表进行预先判断，玩家是否能够进入副本。
	现在只判断玩家等级。
	*/
	const tagInstanceProtoData *pIPData = m_instanceProtoData.FindInstanceProtoData(m_dwTargetMapID);
	//ASSERT(pIPData);
	if (!pIPData)
		return FALSE;

	// 获得玩家等级
	int nRoleLevel = RoleMgr::Inst()->GetLocalPlayer()->GetRoleLevel();
	if (nRoleLevel < pIPData->nLevelLowerLimit)
	{
		// 等级不足
		if (P_VALID(GetObj("InstanceLevelLowerLimitBox")))
			KillObj("InstanceLevelLowerLimitBox");
		CreateObj("InstanceLevelLowerLimitBox", "MsgBox");
		TObjRef<MsgBox>("InstanceLevelLowerLimitBox")->Init(_T(""), g_StrTable[_T("Instance_Level_Lower_limit")], 
			NULL, MBF_OK, TRUE);
		return FALSE;
	}
	if (nRoleLevel > pIPData->nLevelUpperLimit)
	{
		// 等级过高
		if (P_VALID(GetObj("InstanceLevelUpperLimitBox")))
			KillObj("InstanceLevelUpperLimitBox");
		CreateObj("InstanceLevelUpperLimitBox", "MsgBox");
		TObjRef<MsgBox>("InstanceLevelUpperLimitBox")->Init(_T(""), g_StrTable[_T("Instance_Level_Upper_Limit")], 
			NULL, MBF_OK, TRUE);
		return FALSE;
	}
	return TRUE;
}

BOOL InstanceMgr::IsAskBeforeEnter()
{
	const tagInstanceProtoData *pIPData = m_instanceProtoData.FindInstanceProtoData(m_dwTargetMapID);
	ASSERT(pIPData);
	return pIPData->bAskEnter;

	return FALSE;
}

BOOL InstanceMgr::IsAbleToCombat()
{
	if (!IsInInstance())
		return TRUE;
	DWORD dwMapID = MapMgr::Inst()->GetCurMapID();
	const tagInstanceProtoData *pIPData = m_instanceProtoData.FindInstanceProtoData(dwMapID);
	ASSERT(pIPData);
	return pIPData->bCombat;
}

BOOL InstanceMgr::IsAbleToPK()
{
	if (!IsInInstance())
		return TRUE;
	DWORD dwMapID = MapMgr::Inst()->GetCurMapID();
	const tagInstanceProtoData *pIPData = m_instanceProtoData.FindInstanceProtoData(dwMapID);
	ASSERT(pIPData);
	return pIPData->bPK;
}

BOOL InstanceMgr::IsloseSafeGuard()
{
	if (!IsInInstance())
		return FALSE;
	DWORD dwMapID = MapMgr::Inst()->GetCurMapID();
	const tagInstanceProtoData *pIPData = m_instanceProtoData.FindInstanceProtoData(dwMapID);
	ASSERT(pIPData);
	return pIPData->bLoseSafeGuard;
}

VOID InstanceMgr::CreateAskEnterMsgBox()
{
	if (!m_bIn)
	{
		const tagInstanceProtoData *pIPData = m_instanceProtoData.FindInstanceProtoData(m_dwTargetMapID);
		if (!pIPData)
			return;
		tstring strShowName = MapLogicData::Inst()->FindMapShowName(pIPData->dwInstanceID);

		TCHAR szText[512];
		_stprintf(szText, g_StrTable[_T("Instance_FindInstance")], strShowName.c_str());


		if (P_VALID(GetObj("AskEnterMsgBox")))
			KillObj("AskEnterMsgBox");
		CreateObj("AskEnterMsgBox", "MsgBox");
		TObjRef<MsgBox>("AskEnterMsgBox")->Init(_T(""), szText,  
			_T("OnAskEnterMsgBox"), (MsgBoxFlag)(MBF_OK | MBF_Cancel), 
			TRUE);
	}
	else
	{
		// 如果当前已经在副本内了，发出提示是否激活传送点...
		if (P_VALID(GetObj("AskEnterMsgBox2")))
			KillObj("AskEnterMsgBox2");
		CreateObj("AskEnterMsgBox2", "MsgBox");
		TObjRef<MsgBox>("AskEnterMsgBox2")->Init(_T(""), g_StrTable[_T("Instance_ActiveTransmitPoint")], 
			_T("OnActiveTransmitPoint"), (MsgBoxFlag)(MBF_OK | MBF_Cancel), TRUE);
	}
}

VOID InstanceMgr::CreateAskOutMsgBox()
{
	if (P_VALID(GetObj("AskOutMsgBox")))
		KillObj("AskOutMsgBox");
	CreateObj("AskOutMsgBox", "MsgBox");
	TObjRef<MsgBox>("AskOutMsgBox")->Init(_T(""), g_StrTable[_T("Instance_AskOut")], 
		_T("OnAskOutMsgBox"), (MsgBoxFlag)(MBF_OK | MBF_Cancel));
}



VOID InstanceMgr::CreateDifficultyChooseOptionBox()
{
	const tagInstanceProtoData *pIPData = m_instanceProtoData.FindInstanceProtoData(m_dwTargetMapID);
	ASSERT(pIPData);

	if (P_VALID(GetObj("InstanceDifficultyBox")))
		KillObj("InstanceDifficultyBox");
	CreateObj("InstanceDifficultyBox", "MsgOptionBox");

	TObjRef<MsgOptionBox>("InstanceDifficultyBox")->Init(_T(""), g_StrTable[_T("Instance_Difficulty_Ask")],
		g_StrTable[_T("Instance_Difficulty_Normal")], 
		g_StrTable[_T("Instance_Difficulty_Elite")], 
		g_StrTable[_T("Instance_Difficulty_Fiend")],
		(pIPData->bSelectNormal ? true : false),  
		(pIPData->bSelectElite ? true : false),
		(pIPData->bSelectDevil ? true : false),
		_T("OnInstanceDifficultyBox"), TRUE);

}

DWORD InstanceMgr::OnAskEnterMsgBox(tagMsgBoxEvent *pEvent)
{
	switch (pEvent->eResult)
	{
	case MBF_OK:
		{
			const tagInstanceProtoData *pIPData = m_instanceProtoData.FindInstanceProtoData(m_dwTargetMapID);
			ASSERT(pIPData);
			if (pIPData->bSelectHard)
				CreateDifficultyChooseOptionBox();
			else
			{
				if (!IsAbleToGotoInstance())
					return 0;
				Send_TriggerInstance(m_dwTriggerID, static_cast<DWORD>(EIHM_Normal));
			}
		} break;
	case MBF_Cancel:
		break;
	default:
		break;
	}
	return 0;
}

DWORD InstanceMgr::OnAskOutMsgBox(tagMsgBoxEvent *pEvent)
{
	tagNC_RoleMapTrigger cmd;
	switch (pEvent->eResult)
	{
	case MBF_OK:
		cmd.dwTriggerID = m_dwOutTriggerID;
		m_pSession->Send(&cmd);
		break;
	case MBF_Cancel:
		break;
	default:
		break;
	}
	return 0;
}


DWORD InstanceMgr::OnInstanceDifficultyBox(tagMsgOptionBoxEvent *pEvent)
{
	switch (pEvent->eResult)
	{
	case EMOBF_Option1:
		{
			m_eHardMode = EIHM_Normal;
		} break;
	case EMOBF_Option2:
		{
			m_eHardMode = EIHM_Elite;
		} break;
	case EMOBF_Option3:
		{
			m_eHardMode = EIHM_Devil;
		} break;

	default: break;
	}

	if (!IsAbleToGotoInstance())
		return 0;
	Send_TriggerInstance(m_dwTriggerID, static_cast<DWORD>(m_eHardMode));
	return 0;
}

EInstanceHardMode InstanceMgr::GetHardMode() const
{
	return m_eHardMode;
}

VOID InstanceMgr::Reset()
{
	m_dwTargetMapID = GT_INVALID;
	m_dwTriggerID = GT_INVALID;
	m_dwOutTriggerID = GT_INVALID;
	m_dwOutTargetMapID = GT_INVALID;
	m_eHardMode = EIHM_Normal;
}

BOOL InstanceMgr::IsInInstance()
{
	return m_bIn;
}

DWORD InstanceMgr::OnEnterInstanceEvent(tagGameEvent*)
{
	m_bIn = TRUE;
	const tagInstanceProtoData *pData = m_instanceProtoData.FindInstanceProtoData(MapMgr::Inst()->GetCurMapID());
	ASSERT(pData);
	if (pData->eType == EIMT_Preface || pData->eType == EIMT_Rand)
	{
		CreateQuestTargetWindow();
	}
	return 0;
}

DWORD InstanceMgr::OnLeaveInstanceEvent(tagGameEvent*)
{
	m_bIn = FALSE;
	DestroyQuestTargetWindow();
	if (P_VALID(m_pGameFrameMgr->GetFrame(_T("CountDown"))))
		((CountDownFrame*)(m_pGameFrameMgr->GetFrame(_T("CountDown"))))->Hide();

	GameFrame* pQuitFrame = m_pGameFrameMgr->GetFrame( _T("QuitInstance") );
	if(P_VALID( pQuitFrame ))
		m_pGameFrameMgr->AddToDestroyList(pQuitFrame);

	return 0;
}

DWORD InstanceMgr::OnLeaveTeamEvent(tagGameEvent*)
{
	if (IsInInstance() == TRUE)
	{
		GameFrame* pQuitFrame = m_pGameFrameMgr->GetFrame( _T("QuitInstance") );
		if( !P_VALID( pQuitFrame ) )
			m_pGameFrameMgr->CreateFrame( _T("Root"), _T("QuitInstance"), _T("InstanceQuitFrame"), 0);
	}
	return 0;
}

DWORD InstanceMgr::OnActiveTransmitPoint(tagMsgBoxEvent *pEvent)
{
	switch (pEvent->eResult)
	{
	case MBF_OK:
		{
			Send_TriggerInstance(m_dwTriggerID, static_cast<DWORD>(m_eHardMode));
		} break;
	}
	return 0;
}

InstanceMgr::~InstanceMgr()
{
	m_pCountDownFrame = NULL;
	m_pQuestTargetWnd = NULL;
}

