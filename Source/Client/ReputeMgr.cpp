#include "StdAfx.h"
#include "ReputeMgr.h"
#include "..\WorldDefine\msg_famehall.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "CombatSysUtil.h"

ReputeMgr::ReputeMgr(void):m_Trunk(this)
{
}

ReputeMgr::~ReputeMgr(void)
{
}

VOID ReputeMgr::Init()
{
    // 初始化一些成员变量
    for (int i = ECLT_BEGIN; i < ECLT_NUM; ++i)
    {
        m_nReputeValue[i]   =   0;
        m_bisFame[i]        =   FALSE;
        m_nActCount[i]      =   0;
        UlityCalReputeLevel(m_ReputeLevel[i], m_nReputeValue[i]);
    }

	// 声望进度条颜色文件
	m_strPrgssForePic_Red = _T("data\\ui\\state\\L_sw-jindu-c.bmp");
	m_strPrgssForePic_Yellow = _T("data\\ui\\state\\L_sw-jindu-e.bmp");
	m_strPrgssForePic_Green = _T("data\\ui\\state\\L_sw-jindu-a.bmp");
	m_strPrgssForePic_Blue = _T("data\\ui\\state\\L_sw-jindu-b.bmp");
	m_strPrgssForePic_Orange = _T("data\\ui\\state\\L_sw-jindu-d.bmp");

	m_pCmdMgr = TObjRef<NetCmdMgr>();
	m_pUtil	= TObjRef<Util>();
	m_pFrameMgr = TObjRef<GameFrameMgr>();
	m_pSession = TObjRef<NetSession>();

	//注册网络消息
	m_pCmdMgr->Register("NS_GetRoleInitState_Reputation",   (NETMSGPROC)m_Trunk.sfp2(&ReputeMgr::OnInitReputation),         _T("NS_GetRoleInitState_Reputation"));
	m_pCmdMgr->Register("NS_RoleReputeChange",              (NETMSGPROC)m_Trunk.sfp2(&ReputeMgr::OnReputeChange),           _T("NS_RoleReputeChange"));
    m_pCmdMgr->Register("NS_BecomeFame",                    (NETMSGPROC)m_Trunk.sfp2(&ReputeMgr::OnBecomeFame),             _T("NS_BecomeFame"));
    m_pCmdMgr->Register("NS_TreasureActCountChange",        (NETMSGPROC)m_Trunk.sfp2(&ReputeMgr::OnActCountChange),         _T("NS_TreasureActCountChange"));
    m_pCmdMgr->Register("NS_ClanConUpperLimit",             (NETMSGPROC)m_Trunk.sfp2(&ReputeMgr::OnNS_ClanConUpperLimit),   _T("NS_ClanConUpperLimit"));
}

VOID ReputeMgr::Destroy()
{
	//注销网络消息
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_Reputation", (NETMSGPROC)m_Trunk.sfp2(&ReputeMgr::OnInitReputation));
	m_pCmdMgr->UnRegister("NS_RoleReputeChange",            (NETMSGPROC)m_Trunk.sfp2(&ReputeMgr::OnReputeChange));
    m_pCmdMgr->UnRegister("NS_BecomeFame",                  (NETMSGPROC)m_Trunk.sfp2(&ReputeMgr::OnBecomeFame));
    m_pCmdMgr->UnRegister("NS_TreasureActCountChange",      (NETMSGPROC)m_Trunk.sfp2(&ReputeMgr::OnActCountChange));
    m_pCmdMgr->UnRegister("NS_ClanConUpperLimit",           (NETMSGPROC)m_Trunk.sfp2(&ReputeMgr::OnNS_ClanConUpperLimit));
}

ReputeMgr g_ReputeMgr;
ReputeMgr* ReputeMgr::Inst()
{
	return &g_ReputeMgr;
}

VOID ReputeMgr::SetReputeValue( ECLanType eCtype, INT nValue, INT nChange )
{
	if (JDG_VALID(ECLT, eCtype))
	{
		m_nReputeValue[eCtype] = nValue;
		UlityLocalPlayerRepuLvl(eCtype, nValue, nChange);
		UlityCalReputeLevel(m_ReputeLevel[eCtype], nValue);

		tagReputeChange msg(_T("ReputeChange"), NULL);
		msg.eClanType	=	eCtype;
		msg.nValue		=	nValue;
		msg.nChange		=	nChange;
		m_pFrameMgr->SendEvent(&msg);
	}
}

DWORD ReputeMgr::OnInitReputation( tagNS_GetRoleInitState_Reputation* pNetCmd, DWORD )
{
	for (int i = ECLT_BEGIN; i < ECLT_NUM; ++i)
    {
		m_nReputeValue[i]   =   pNetCmd->nReputation[i];
        m_bisFame[i]        =   (BOOL)pNetCmd->bisFame[i];
        m_nActCount[i]      =   (INT)pNetCmd->nActiveCount[i];
		
		OnInitRepuLvl((ECLanType)i, pNetCmd->nReputation[i]);
        UlityCalReputeLevel(m_ReputeLevel[i], pNetCmd->nReputation[i]);
    }

	// 跟在这后面发送当前所有激活的氏族珍宝请求
	for (INT i = ECLT_BEGIN; i < ECLT_NUM; ++i)
	{
		tagNC_GetActClanTreasure e;
		e.byClanType	=	(ECLanType)i;
		if (m_pSession->IsConnect())
			m_pSession->Send(&e);
	}
	
	// 如果玩家帮派ID有效，发送帮派初始化请求消息
	if(GT_VALID(RoleMgr::Inst()->GetLocalPlayer()->GetRoleGuildID()))
	{
		tagNC_GetRoleInitState cmd;
		cmd.eType = ERIT_Guild;
		m_pSession->Send(&cmd);
	}
	else
	{
		// 在初始化消息最后发送本地玩家初始化完成消息
		tagGameEvent evtInit( _T("tagInitLPAttEvent"), NULL );
		m_pFrameMgr->SendEvent(&evtInit);
	}

	return 0;
}

DWORD ReputeMgr::OnReputeChange( tagNS_RoleReputeChange* pNetCmd, DWORD )
{
	SetReputeValue((ECLanType)pNetCmd->byClanType, pNetCmd->nRepute, pNetCmd->nChange);

	return 0;
}

BOOL ReputeMgr::UlityCalReputeLevel( tagReputeLevel& ReputeSrc, INT nReputeValue )
{
	if (nReputeValue >= -46800 && nReputeValue < -10800)
	{
		ReputeSrc.eLevel			=	ERL_Hostile;
		ReputeSrc.eColor			=	EPC_RED;
		ReputeSrc.szLevel			=	g_StrTable[_T("ReputeLevel0")];
		ReputeSrc.nFrom				=	-46800;
		ReputeSrc.nTo				=	-10800;
		ReputeSrc.nDiffValue		=	36000;
		ReputeSrc.nMaxContribute	=	0;
	}
	else if (nReputeValue >= -10800 && nReputeValue < -3600)
	{
		ReputeSrc.eLevel			=	ERL_Hated;
		ReputeSrc.eColor			=	EPC_RED;
		ReputeSrc.szLevel			=	g_StrTable[_T("ReputeLevel1")];
		ReputeSrc.nFrom				=	-10800;
		ReputeSrc.nTo				=	-3600;
		ReputeSrc.nDiffValue		=	7200;
		ReputeSrc.nMaxContribute	=	0;
	}
	else if (nReputeValue >= -3600 && nReputeValue < 0)
	{
		ReputeSrc.eLevel			=	ERL_Callous;
		ReputeSrc.eColor			=	EPC_YELLOW;
		ReputeSrc.szLevel			=	g_StrTable[_T("ReputeLevel2")];
		ReputeSrc.nFrom				=	-3600;
		ReputeSrc.nTo				=	0;
		ReputeSrc.nDiffValue		=	3600;
		ReputeSrc.nMaxContribute	=	5000;
	}
	else if (nReputeValue >= 0 && nReputeValue < 3600)
	{
		ReputeSrc.eLevel			=	ERL_Neutrality;
		ReputeSrc.eColor			=	EPC_YELLOW;
		ReputeSrc.szLevel			=	g_StrTable[_T("ReputeLevel3")];
		ReputeSrc.nFrom				=	0;
		ReputeSrc.nTo				=	3600;
		ReputeSrc.nDiffValue		=	3600;
		ReputeSrc.nMaxContribute	=	5000;
	}
	else if (nReputeValue >= 3600 && nReputeValue < 10800)
	{
		ReputeSrc.eLevel			=	ERL_Friendly;
		ReputeSrc.eColor			=	EPC_GREEN;
		ReputeSrc.szLevel			=	g_StrTable[_T("ReputeLevel4")];
		ReputeSrc.nFrom				=	3600;
		ReputeSrc.nTo				=	10800;
		ReputeSrc.nDiffValue		=	7200;
		ReputeSrc.nMaxContribute	=	10000;
	}
	else if (nReputeValue >= 10800 && nReputeValue < 22800)
	{
		ReputeSrc.eLevel			=	ERL_Intimate;
		ReputeSrc.eColor			=	EPC_GREEN;
		ReputeSrc.szLevel			=	g_StrTable[_T("ReputeLevel5")];
		ReputeSrc.nFrom				=	10800;
		ReputeSrc.nTo				=	22800;
		ReputeSrc.nDiffValue		=	12000;
		ReputeSrc.nMaxContribute	=	20000;
	}
	else if (nReputeValue >= 22800 && nReputeValue < 40800)
	{
		ReputeSrc.eLevel			=	ERL_Reliable;
		ReputeSrc.eColor			=	EPC_GREEN;
		ReputeSrc.szLevel			=	g_StrTable[_T("ReputeLevel6")];
		ReputeSrc.nFrom				=	22800;
		ReputeSrc.nTo				=	40800;
		ReputeSrc.nDiffValue		=	18000;
		ReputeSrc.nMaxContribute	=	40000;
	}
	else if (nReputeValue >= 40800 && nReputeValue < 76800)
	{
		ReputeSrc.eLevel			=	ERL_Respected;
		ReputeSrc.eColor			=	EPC_BLUE;
		ReputeSrc.szLevel			=	g_StrTable[_T("ReputeLevel7")];
		ReputeSrc.nFrom				=	40800;
		ReputeSrc.nTo				=	76800;
		ReputeSrc.nDiffValue		=	36000;
		ReputeSrc.nMaxContribute	=	70000;
	}
	else if (nReputeValue >= 76800 && nReputeValue < 148800)
	{
		ReputeSrc.eLevel			=	ERL_Worshipped;
		ReputeSrc.eColor			=	EPC_BLUE;
		ReputeSrc.szLevel			=	g_StrTable[_T("ReputeLevel8")];
		ReputeSrc.nFrom				=	76800;
		ReputeSrc.nTo				=	148800;
		ReputeSrc.nDiffValue		=	72000;
		ReputeSrc.nMaxContribute	=	120000;
	}
	else if (nReputeValue >= 148800 && nReputeValue <= 328800)
	{
		ReputeSrc.eLevel			=	ERL_Legend;
		ReputeSrc.eColor			=	EPC_ORANGE;
		ReputeSrc.szLevel			=	g_StrTable[_T("ReputeLevel9")];
		ReputeSrc.nFrom				=	148800;
		ReputeSrc.nTo				=	328800;
		ReputeSrc.nDiffValue		=	180000;
		ReputeSrc.nMaxContribute	=	200000;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

VOID ReputeMgr::UlitySetPrgrssColor( ProgressEx* pProgress, EPrgrssColor eColor )
{
	switch (eColor)
	{
	case EPC_RED:
		if (pProgress->GetForePic() != m_strPrgssForePic_Red)
			pProgress->SetForePic(m_strPrgssForePic_Red);
		break;
	case EPC_YELLOW:
		if (pProgress->GetForePic() != m_strPrgssForePic_Yellow)
			pProgress->SetForePic(m_strPrgssForePic_Yellow);
		break;
	case EPC_GREEN:
		if (pProgress->GetForePic() != m_strPrgssForePic_Green)
			pProgress->SetForePic(m_strPrgssForePic_Green);
		break;
	case EPC_BLUE:
		if (pProgress->GetForePic() != m_strPrgssForePic_Blue)
			pProgress->SetForePic(m_strPrgssForePic_Blue);
		break;
	case EPC_ORANGE:
		if (pProgress->GetForePic() != m_strPrgssForePic_Orange)
			pProgress->SetForePic(m_strPrgssForePic_Orange);
		break;
	}
}

DWORD ReputeMgr::OnBecomeFame( tagNS_BecomeFame* pNetCmd, DWORD )
{
    ECLanType   eCtype   =   (ECLanType)pNetCmd->eClanType;
    if (JDG_VALID(ECLT, eCtype))
    {
        m_bisFame[eCtype]   =   TRUE;
    }

    // 发送游戏事件
    tagBecomeEvent e(_T("tagBecomeEvent"), NULL);
    m_pFrameMgr->SendEvent(&e);
	return 0;
}

DWORD ReputeMgr::OnActCountChange( tagNS_TreasureActCountChange* pNetCmd, DWORD )
{
    ECLanType   eCtype   =   (ECLanType)pNetCmd->eClanType;
    if (JDG_VALID(ECLT, eCtype))
    {
        m_nActCount[eCtype] =   (INT)pNetCmd->nActCount;
    }

    // 发送游戏事件
    tagTreasureActCountChange e(_T("tagTreasureActCountChange"), NULL);
    m_pFrameMgr->SendEvent(&e);
	return 0;
}

VOID ReputeMgr::OnInitRepuLvl( ECLanType eType, INT nReputeValue )
{
	if (nReputeValue >= -46800 && nReputeValue < -10800)
	{
		m_eRepuLvl[eType]			= ERL_Hostile;
	}
	else if (nReputeValue >= -10800 && nReputeValue < -3600)
	{
		m_eRepuLvl[eType]			= ERL_Hated;
	}
	else if (nReputeValue >= -3600 && nReputeValue < 0)
	{
		m_eRepuLvl[eType]			= ERL_Callous;
	}
	else if (nReputeValue >= 0 && nReputeValue < 3600)
	{
		m_eRepuLvl[eType]			= ERL_Neutrality;
	}
	else if (nReputeValue >= 3600 && nReputeValue < 10800)
	{
		m_eRepuLvl[eType]			= ERL_Friendly;
	}
	else if (nReputeValue >= 10800 && nReputeValue < 22800)
	{
		m_eRepuLvl[eType]			= ERL_Intimate;
	}
	else if (nReputeValue >= 22800 && nReputeValue < 40800)
	{
		m_eRepuLvl[eType]			= ERL_Reliable;
	}
	else if (nReputeValue >= 40800 && nReputeValue < 76800)
	{
		m_eRepuLvl[eType]			= ERL_Respected;
	}
	else if (nReputeValue >= 76800 && nReputeValue < 148800)
	{
		m_eRepuLvl[eType]			= ERL_Worshipped;
	}
	else if (nReputeValue >= 148800 && nReputeValue <= 328800)
	{
		m_eRepuLvl[eType]			= ERL_Legend;
	}
}

void ReputeMgr::UlityLocalPlayerRepuLvl( ECLanType eType, INT nReputeValue, INT nChange )
{
	bool bChange = false;
	if (nReputeValue >= -46800 && nReputeValue < -10800)
	{
		if( m_eRepuLvl[eType] != ERL_Hostile)
		{
			bChange = true;
		}
		m_eRepuLvl[eType]			= ERL_Hostile;
	}
	else if (nReputeValue >= -10800 && nReputeValue < -3600)
	{
		if( m_eRepuLvl[eType] != ERL_Hated)
		{
			bChange = true;
		}
		m_eRepuLvl[eType]			= ERL_Hated;
	}
	else if (nReputeValue >= -3600 && nReputeValue < 0)
	{
		if( m_eRepuLvl[eType] != ERL_Callous)
		{
			bChange = true;
		}
		m_eRepuLvl[eType]			= ERL_Callous;
	}
	else if (nReputeValue >= 0 && nReputeValue < 3600)
	{
		if( m_eRepuLvl[eType] != ERL_Neutrality)
		{
			bChange = true;
		}
		m_eRepuLvl[eType]			= ERL_Neutrality;
	}
	else if (nReputeValue >= 3600 && nReputeValue < 10800)
	{
		if( m_eRepuLvl[eType] != ERL_Friendly)
		{
			bChange = true;
		}
		m_eRepuLvl[eType]			= ERL_Friendly;
	}
	else if (nReputeValue >= 10800 && nReputeValue < 22800)
	{
		if( m_eRepuLvl[eType] != ERL_Intimate)
		{
			bChange = true;
		}
		m_eRepuLvl[eType]			= ERL_Intimate;
	}
	else if (nReputeValue >= 22800 && nReputeValue < 40800)
	{
		if( m_eRepuLvl[eType] != ERL_Reliable)
		{
			bChange = true;
		}
		m_eRepuLvl[eType]			= ERL_Reliable;
	}
	else if (nReputeValue >= 40800 && nReputeValue < 76800)
	{
		if( m_eRepuLvl[eType] != ERL_Respected)
		{
			bChange = true;
		}
		m_eRepuLvl[eType]			= ERL_Respected;
	}
	else if (nReputeValue >= 76800 && nReputeValue < 148800)
	{
		if( m_eRepuLvl[eType] != ERL_Worshipped)
		{
			bChange = true;
		}
		m_eRepuLvl[eType]			= ERL_Worshipped;
	}
	else if (nReputeValue >= 148800 && nReputeValue <= 328800)
	{
		if( m_eRepuLvl[eType] != ERL_Legend)
		{
			bChange = true;
		}
		m_eRepuLvl[eType]			= ERL_Legend;
	}

	if( bChange )
	{
		tagReputeLvlChange change(_T("tagReputeLvlChange"), NULL);
		change.eClanType = eType;
		change.eLevel = m_eRepuLvl[eType];
		m_pFrameMgr->SendEvent(&change);
	}
}

DWORD ReputeMgr::OnNS_ClanConUpperLimit( tagNS_ClanConUpperLimit* pNetCmd, DWORD )
{
    CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ClanContributeLimit")], _T(""));

    return 0;
}