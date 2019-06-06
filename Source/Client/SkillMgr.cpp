#include "stdafx.h"
#include "SkillMgr.h"
#include "SkillProtoData.h"
#include "SkillEvent.h"
#include "CombatEvent.h"
#include "ItemMgr.h"
#include "Item.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "EffectMgr.h"
#include "CombatSysUtil.h"

SkillMgr::SkillMgr():m_Trunk(this)
{
	
}

SkillMgr::~SkillMgr()
{
}

SkillMgr g_skillmgr;
SkillMgr* SkillMgr::Inst()
{
	return &g_skillmgr;
}

void SkillMgr::Init()
{
	m_pCmdMgr = TObjRef<NetCmdMgr>();
	m_pSession = TObjRef<NetSession>();
	m_pUtil	= TObjRef<Util>();
	m_pFrameMgr = TObjRef<GameFrameMgr>();


	//--注册网络消息处理函数
	m_pCmdMgr->Register("NS_GetRoleInitState_Skill",	(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetInitLocalPlayerSkill), _T("NS_GetRoleInitState_Skill"));
	m_pCmdMgr->Register("NS_AddSkill",					(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetAddSkill),			  _T("NS_AddSkill"));
	m_pCmdMgr->Register("NS_UpdateSkill",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetUpdateSkill),		  _T("NS_UpdateSkill"));
	m_pCmdMgr->Register("NS_RemoveSkill",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetRemoveSkill),		  _T("NS_RemoveSkill"));
	m_pCmdMgr->Register("NS_LearnSkill",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetLearnSkill),			  _T("NS_LearnSkill"));
	m_pCmdMgr->Register("NS_LevelUpSkill",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetLevelUpSkill),		  _T("NS_LevelUpSkill"));
	m_pCmdMgr->Register("NS_ForgetSkill",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetForgetSkill),		  _T("NS_ForgetSkill"));
	m_pCmdMgr->Register("NS_ClearTalent",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetClearTalent),		  _T("NS_ClearTalent"));
	m_pCmdMgr->Register("NS_AddTalent",					(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetAddTalent),			  _T("NS_AddTalent"));
	m_pCmdMgr->Register("NS_RemoveTalent",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetRemoveTalent),		  _T("NS_RemoveTalent"));
	m_pCmdMgr->Register("NS_UpdateTalent",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetUpdateTalent),		  _T("NS_UpdateTalent"));
	m_pCmdMgr->Register("NS_UpdateSkillCoolDown",		(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetUpdateSkillCD),		  _T("NS_UpdateSkillCoolDown"));



	//--注册游戏事件
}

void SkillMgr::Destroy()
{
	FreeMap();

	m_pCmdMgr->UnRegister("NS_GetRoleInitState_Skill",	(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetInitLocalPlayerSkill));
	m_pCmdMgr->UnRegister("NS_AddSkill",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetAddSkill));
	m_pCmdMgr->UnRegister("NS_UpdateSkill",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetUpdateSkill));
	m_pCmdMgr->UnRegister("NS_RemoveSkill",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetRemoveSkill));
	m_pCmdMgr->UnRegister("NS_LearnSkill",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetLearnSkill));
	m_pCmdMgr->UnRegister("NS_LevelUpSkill",			(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetLevelUpSkill));
	m_pCmdMgr->UnRegister("NS_ForgetSkill",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetForgetSkill));
	m_pCmdMgr->UnRegister("NS_ClearTalent",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetClearTalent));
	m_pCmdMgr->UnRegister("NS_AddTalent",				(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetAddTalent));
	m_pCmdMgr->UnRegister("NS_RemoveTalent",			(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetRemoveTalent));
	m_pCmdMgr->UnRegister("NS_UpdateTalent",			(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetUpdateTalent));
	m_pCmdMgr->UnRegister("NS_UpdateSkillCoolDown",		(NETMSGPROC)m_Trunk.sfp2(&SkillMgr::NetUpdateSkillCD));

}


DWORD SkillMgr::NetInitLocalPlayerSkill(tagNS_GetRoleInitState_Skill* pNetCmd, DWORD dwParam)
{
	
	//清除技能数据
	FreeMap();

	for(INT j=0; j<X_MAX_TALENT_PER_ROLE; ++j)
	{
		SetTalentPoint(pNetCmd->Talent[j]);
	}

	//添加本地玩家技能信息
	for(INT i=0; i<pNetCmd->nNum; ++i)
	{
		AddSkill(pNetCmd->Skill[i]);
	}
	
	

	tagNC_GetRoleInitState m;
	m.eType = ERIT_Item;
	m_pSession->Send(&m);

	return 0;
}

void SkillMgr::AddSkill(tagSkillMsgInfo data)
{
	tagSkillData* pSkill = new tagSkillData;
	pSkill->dwSkillID = data.dwID;
	pSkill->nCurCD = data.nCoolDown;
	pSkill->nLevel = data.nLevel;
	pSkill->nLearnLevel = data.nLearnLevel;
	pSkill->nExp = data.nProficiency;
	pSkill->dwTypeID = MTransSkillTypeID(data.dwID, data.nLevel);
	tagSkillProtoClient* pProto = const_cast<tagSkillProtoClient*>(SkillProtoData::Inst()->FindSkillProto(pSkill->dwTypeID));
	if(P_VALID(pProto))
	{
		pSkill->pProto = pProto;
		m_skillMap.Add(pSkill->dwSkillID, pSkill);
		
		// 合成其他类型
		pSkill->nCurCD = data.nCoolDown;
		pSkill->nLearnLevel = data.nLearnLevel;
		pSkill->nCooldown = data.nMaxCoolDown;
		pSkill->nPrepareTime = data.nPrepareTime;
		pSkill->fOPDist = data.fOpDist;
		pSkill->nSkillDmg = 0;
		for (INT i=0; i<MAX_CHANNEL_TIMES; ++i)
		{
			pSkill->nSkillDmg += data.nChannelDmg[i];
		}
		for (INT i=0; i<ESCT_End; ++i)
		{
			pSkill->nCost[i] = data.nCost[i];
		}
		// 如果冷却倒计时不等于零则更新冷缺计算开始时间
		if(0 != data.nCoolDown)
		{
			INT nIncMilliSecond = data.nCoolDown - data.nMaxCoolDown;
			pSkill->dwStartTime = Kernel::Inst()->GetAccumTimeDW() + nIncMilliSecond;
		}

		// 发送游戏事件
		tagUpdateLPSkillEvent event(_T("tagUpdateLPSkillEvent"),NULL);
		event.bAdd	= TRUE;
		event.dwID	= pSkill->dwSkillID;
		event.eType = pSkill->pProto->eUseType;
		TObjRef<GameFrameMgr>()->SendEvent(&event);
	}
	else
	{
		SAFE_DEL(pSkill);
	}
}

void SkillMgr::RemoveSkill(DWORD dwID)
{
	tagSkillData* pSkill = m_skillMap.Peek(dwID);
	if(P_VALID(pSkill))
	{
		BOOL bRet = m_skillMap.Erase(dwID);
		if(bRet)
		{
			// 发送游戏事件
			tagUpdateLPSkillEvent event(_T("tagUpdateLPSkillEvent"),NULL);
			event.bAdd  = FALSE;
			event.dwID  = dwID;
			event.eType = pSkill->pProto->eUseType;
			TObjRef<GameFrameMgr>()->SendEvent(&event);
			
			//从列表中删除后，删除该指针
			SAFE_DEL(pSkill);
		}
	}
}

void SkillMgr::UpdateSkill( tagSkillMsgInfo data )
{
	tagSkillData* pSkill = m_skillMap.Peek(data.dwID);
	if(P_VALID(pSkill))
	{
		// 等级不同，则模板也要发生变化
		if(pSkill->nLevel != data.nLevel)
		{
			pSkill->nLevel = data.nLevel;
			pSkill->dwTypeID = MTransSkillTypeID(data.dwID, data.nLevel);
			tagSkillProtoClient* pProto = const_cast<tagSkillProtoClient*>(SkillProtoData::Inst()->FindSkillProto(pSkill->dwTypeID));
			if(P_VALID(pProto))
			{
				pSkill->pProto = pProto;
			}
			else
			{
				pSkill->pProto = NULL;
			}
		}
		
		// 合成其他类型
		pSkill->nCurCD = data.nCoolDown;
		pSkill->nLearnLevel = data.nLearnLevel;
		pSkill->nCooldown = data.nMaxCoolDown;
		pSkill->nPrepareTime = data.nPrepareTime;
		pSkill->nSkillDmg = 0;
		pSkill->nExp = data.nProficiency;
		pSkill->fOPDist = data.fOpDist;
		for (INT i=0; i<MAX_CHANNEL_TIMES; ++i)
		{
			pSkill->nSkillDmg += data.nChannelDmg[i];
		}
		for (INT i=0; i<ESCT_End; ++i)
		{
			pSkill->nCost[i] = data.nCost[i];
		}
		// 如果冷却倒计时不等于零则更新冷缺计算开始时间
		if(0 != data.nCoolDown)
		{
			INT nIncMilliSecond = (INT)(data.nCoolDown - data.nMaxCoolDown);
			pSkill->dwStartTime = Kernel::Inst()->GetAccumTimeDW() + nIncMilliSecond;
		}
		
		// 发送游戏事件
		tagUpdateLPSkillEvent event(_T("tagUpdateLPSkillEvent"),NULL);
		event.bAdd  = FALSE;
		event.dwID  = pSkill->dwSkillID;
		event.eType = pSkill->pProto->eUseType;
		TObjRef<GameFrameMgr>()->SendEvent(&event);
	}
}

DWORD SkillMgr::GetNormalAttackID()
{
	EItemTypeEx eType = ItemMgr::Inst()->GetHandWeaponType();
	switch (eType)
	{
	case EITE_Sword:
		return NORMAL_ATTACK_SKILL_ID[1];
	case EITE_Blade:
		return NORMAL_ATTACK_SKILL_ID[2];
	case EITE_Wand:
		return NORMAL_ATTACK_SKILL_ID[3];
	case EITE_Bow:
		return NORMAL_ATTACK_SKILL_ID[4];
	case EITE_Faqi:
		return NORMAL_ATTACK_SKILL_ID[5];
	case EITE_Qin:
		return NORMAL_ATTACK_SKILL_ID[6];
	}
	return NORMAL_ATTACK_SKILL_ID[0];
}


void SkillMgr::GetSkillList( TList<DWORD>& List )
{
	//--加入徒手攻击的技能，普通攻击只显示一个，统一处理
	List.Clear();
	
	tagSkillData* pData;
	BOOL bRead = FALSE;
	m_skillMap.ResetIterator();
	while(m_skillMap.PeekNext(pData))
	{
		//--只显示一个普通攻击
		if(IsNormalAttackSkill(pData->dwSkillID))
		{
			if(bRead)
				continue;
			else 
				bRead = TRUE;
		}
		//--只显示不隐藏的主动技能
		if(ESUT_Active == pData->pProto->eUseType
			&& ESSTE2_Hidden != pData->pProto->nType3)
		{	
			List.PushBack(pData->dwSkillID);
		}

	}
}

const tagSkillData* SkillMgr::GetSkillData( DWORD dwID )
{
	if(IsNormalAttackSkill(dwID))
	{
		dwID = GetNormalAttackID();
	}
	return m_skillMap.Peek(dwID);
}


const tagSkillData* SkillMgr::GetSkillDataByTypeID( DWORD dwTypeID )
{
	return GetSkillData(MTransSkillID(dwTypeID));
}

void SkillMgr::FreeMap()
{
	tagSkillData *pSkill = NULL;

	m_skillMap.ResetIterator();
	while(m_skillMap.PeekNext(pSkill))
	{
		SAFE_DEL(pSkill);
	}
	m_skillMap.Clear();

	for(INT i=0; i<X_MAX_TALENT_PER_ROLE; ++i)
	{
		m_talentPoint[i].eType = ETT_Null;
		m_talentPoint[i].nPoint = NULL;
	}
}

DWORD SkillMgr::NetAddSkill( tagNS_AddSkill* pNetCmd, DWORD dwParam )
{
	AddSkill(pNetCmd->Skill);

	// 播放特效
	EffectMgr::Inst()->PlayRoleEffect( RoleMgr::Inst()->GetLocalPlayerID(), _T("Txxy06") );

	return 0;
}

DWORD SkillMgr::NetUpdateSkill( tagNS_UpdateSkill* pNetCmd, DWORD dwParam )
{
	UpdateSkill(pNetCmd->Skill);

	return 0;
}

DWORD SkillMgr::NetRemoveSkill( tagNS_RemoveSkill* pNetCmd, DWORD dwParam )
{
	RemoveSkill(pNetCmd->dwSkillID);

	return 0;
}

DWORD SkillMgr::NetLearnSkill( tagNS_LearnSkill* pNetCmd, DWORD dwParam )
{
    if (E_LearnSkill_ClassLimit == pNetCmd->dwErrorCode)
    {
        CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("E_LearnSkill_ClassLimit")]);
    }

	return 0;
}

DWORD SkillMgr::NetLevelUpSkill( tagNS_LevelUpSkill* pNetCmd, DWORD dwParam )
{

	return 0;
}

DWORD SkillMgr::NetForgetSkill( tagNS_ForgetSkill* pNetCmd, DWORD dwParam )
{
	
	return 0;
}

DWORD SkillMgr::NetClearTalent( tagNS_ClearTalent* pNetCmd, DWORD dwParam )
{
	if (E_Success == pNetCmd->dwErrorCode)
	{
		tagGameEvent event(_T("SkillEvent_ClearTalent"), NULL);
		m_pFrameMgr->SendEvent(&event);
	}
	return 0;
}

DWORD SkillMgr::NetAddTalent( tagNS_AddTalent* pNetCmd, DWORD dwParam )
{
	tagRoleTalent talent;
	talent.eType = pNetCmd->eType;
	talent.nPoint = pNetCmd->nPoint;
	SetTalentPoint(talent);

	return 0;
}

DWORD SkillMgr::NetRemoveTalent( tagNS_RemoveTalent* pNetCmd, DWORD dwParam )
{
	tagRoleTalent talent;
	talent.eType = pNetCmd->eType;
	talent.nPoint = 0;
	SetTalentPoint(talent);

	return 0;
}

DWORD SkillMgr::NetUpdateTalent( tagNS_UpdateTalent* pNetCmd, DWORD dwParam )
{
	tagRoleTalent talent;
	talent.eType = pNetCmd->eType;
	talent.nPoint = pNetCmd->nPoint;
	SetTalentPoint(talent);

	return 0;
}


DWORD SkillMgr::NetUpdateSkillCD( tagNS_UpdateSkillCoolDown* pNetCmd, DWORD dwParam )
{
	tagSkillData* pSkill = m_skillMap.Peek(pNetCmd->dwSkillID);
	if(P_VALID(pSkill))
	{
		pSkill->nCurCD = pNetCmd->nCoolDown;
		INT nIncMilliSecond = pSkill->nCurCD - pSkill->nCooldown;
		pSkill->dwStartTime = Kernel::Inst()->GetAccumTimeDW() +  nIncMilliSecond;
		
		// 发送游戏事件
		tagUpdateLPSkillEvent event(_T("tagUpdateLPSkillEvent"),NULL);
		event.bAdd  = FALSE;
		event.dwID  = pSkill->dwSkillID;
		event.eType = pSkill->pProto->eUseType;
		TObjRef<GameFrameMgr>()->SendEvent(&event);
	}

	return 0;
}

void SkillMgr::ShowNetError( INT nErrorCode )
{
	if(GT_INVALID != nErrorCode && E_Success != nErrorCode)
	{

	}
}

BOOL SkillMgr::IsInsideOfMaxTalent( ETalentType eTalent )
{
    // 战技和奇术不设限制
    if (ETT_Action == eTalent || ETT_Jugglery == eTalent)
        return TRUE;
    
    // 已获得的天资，不受限制
    if (IsTalentAquired(eTalent))
        return TRUE;

    // 如果拥有MAX_BATTLE_TALENT_COUNT个战斗系技能，则关掉其它战斗系天资
    if (GetBattleTalentCount() < MAX_BATTLE_TALENT_COUNT)
        return TRUE;

	return FALSE;
}

INT SkillMgr::GetTalentPoint( ETalentType eTalent )
{
	for(int i=0; i<X_MAX_TALENT_PER_ROLE; ++i)
	{
		if(eTalent == m_talentPoint[i].eType)
			return m_talentPoint[i].nPoint;
	}
	return 0;
}

void SkillMgr::SetTalentPoint( tagRoleTalent Talent )
{
	INT nMaxTalent = 0;
	BOOL bSave = FALSE;
	for (INT i=0; i<X_MAX_TALENT_PER_ROLE; ++i)
	{
		if(!bSave)
		{
			// 如果未存，则找相同的或者空的存
			if(m_talentPoint[i].eType == Talent.eType || 
				m_talentPoint[i].eType == ETT_Null)
			{
				if(0 != Talent.nPoint)
				{
					m_talentPoint[i].eType = Talent.eType;
					m_talentPoint[i].nPoint = Talent.nPoint;
				}
				else// 如果为零则为删除
				{
					m_talentPoint[i].eType = ETT_Null;
					m_talentPoint[i].nPoint = 0;

				}
				
				bSave = TRUE;
			}
		}
		else
		{
			// 如果已存还发现有相同的，则清除重复的内容
			if(m_talentPoint[i].eType == Talent.eType)
			{
				m_talentPoint[i].eType = ETT_Null;
				m_talentPoint[i].nPoint = 0;
			}
		}
		
		// 有不为空的数组则计数
		if(ETT_Null != m_talentPoint[i].eType)
		{
			++nMaxTalent;
		}
	}
	
	if(ETT_Null != Talent.eType)
	{
		// 发送游戏事件
		tagUpdateLPTalentPoint event(_T("tagUpdateLPTalentPoint"),NULL);
		event.eType = Talent.eType;
		event.nPoint = Talent.nPoint;
		event.nMaxTalent = nMaxTalent;
		TObjRef<GameFrameMgr>()->SendEvent(&event);
	}
}

VOID SkillMgr::OnUseSkill( DWORD dwTypeID )
{
	//如果是普通攻击，则要重新取一下TypeID
	if(IsNormalAttackSkill(MTransSkillID(dwTypeID)))
	{
		dwTypeID = GetNormalAttackTypeID();
	}

	tagUseSkillEvent event(_T("tagUseSkillEvent"),NULL);
	event.dwSkillID = dwTypeID;
	m_pFrameMgr->SendEvent(&event);
}

DWORD SkillMgr::GetNormalAttackTypeID()
{
	DWORD dwSkillID = GetNormalAttackID();
	tagSkillData* pSkill = m_skillMap.Peek(dwSkillID);
	if(P_VALID(pSkill))
	{
		return pSkill->dwTypeID;
	}
	return GT_INVALID;
}

BOOL SkillMgr::IsHaveSkillByTypeID( DWORD dwTypeID )
{
	tagSkillData* pSkill = m_skillMap.Peek(MTransSkillID(dwTypeID));
	//如果找到技能且模板ID相同
	if(P_VALID(pSkill) && MTransSkillLevel(dwTypeID) <= pSkill->nLearnLevel)
		return TRUE;
	
	return FALSE;
}

BOOL SkillMgr::IsHaveProduceSkillByTypeID(DWORD dwTypeID)
{
	tagSkillData* pSkill = m_skillMap.Peek(MTransSkillID(dwTypeID));
	if( P_VALID(pSkill) )
		return TRUE;

	return FALSE;
}

BOOL SkillMgr::IsSkillCDByTypeID( DWORD dwTypeID )
{
	FLOAT fCDCount = 0.0f;
	FLOAT fRestoreRatio = 0.0f;
	GetSkillCDTime(MTransSkillID(dwTypeID), fCDCount, fRestoreRatio);
	
	return fCDCount <=0;
}

VOID SkillMgr::GetSkillCDTime( DWORD dwID, FLOAT& fCDCount, FLOAT& fRestoreRatio )
{
	const tagSkillData* pSkill = GetSkillData(dwID);
	if(P_VALID(pSkill) && P_VALID(pSkill->dwStartTime))
	{
		DWORD dwStartTime = pSkill->dwStartTime;
		FLOAT fCDTime = (FLOAT)pSkill->nCooldown;
		//离上一次该类物品使用的时间
		INT nPassMilliSecond = Kernel::Inst()->GetAccumTimeDW() - pSkill->dwStartTime;
		//倒计时时间
		fCDCount = fCDTime - nPassMilliSecond;	
		//倒计时旋转律
		fRestoreRatio = (fCDTime - fCDCount) / fCDTime + 0.000001f;
	}
}

tstring SkillMgr::GetSkillIconPath( DWORD dwTypeID )
{
	//如果是普通攻击需要查看
	if(IsNormalAttackSkill(MTransSkillID(dwTypeID)))
	{
		//主手武器存在
		Equipment* pEquip = ItemMgr::Inst()->GetCurEquip(EEP_RightHand);
		if(P_VALID(pEquip))
		{
			return ItemMgr::Inst()->GetItemIconPath(pEquip->GetItemTypeID());
		}
	}
	TCHAR szPath[X_LONG_NAME] = {0};
	const tagSkillProtoClient* pProto = SkillProtoData::Inst()->FindSkillProto(dwTypeID);
	_sntprintf(szPath, sizeof(szPath)/sizeof(TCHAR), _T("data\\ui\\Icon\\%s.tga"), pProto->szIcon);

	return szPath;
}


DWORD SkillMgr::GetGatherSkillTypeID( INT nType )
{
	switch (nType)
	{
	case EGT_Mine:
	case EGT_Herb:
		{
			tagSkillData* pSkill = m_skillMap.Peek(GATHER_SKILL_MINING);
			if (P_VALID(pSkill) && P_VALID(pSkill->pProto))
			{
				return pSkill->dwTypeID;
			}
		}
		break;
	case EGT_Food:
	case EGT_Cloth:
		{
			tagSkillData* pSkill = m_skillMap.Peek(GATHER_SKILL_HARVEST);
			if (P_VALID(pSkill) && P_VALID(pSkill->pProto))
			{
				return pSkill->dwTypeID;
			}
		}
		break;
	}
	return GT_INVALID;
}

DWORD SkillMgr::IsSkillCanLearn( DWORD dwTypeID )
{
	LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
	if(!P_VALID(pLp))
		return GT_INVALID;

	// 判断技能是否存在
	if(IsHaveSkillByTypeID(dwTypeID))
	{
		return E_LearnSkill_Existed;
	}
	// 获取技能原型
	const tagSkillProtoClient* pProto = SkillProtoData::Inst()->FindSkillProto(dwTypeID);
	if(!P_VALID(pProto))
	{
		return E_LearnSkill_ProtoNotFound;
	}

	// 职业限制是否达成
	

	// 等级限制是否达成
	if(pProto->nNeedRoleLevel > 1 && pProto->nNeedRoleLevel > pLp->GetRoleLevel())
	{
		return E_LearnSkill_NeedMoreLevel;
	}

	// 天资限制是否达成
	if(pProto->nNeedTalentPoint > SkillMgr::Inst()->GetTalentPoint(pProto->eTalentType))
	{
		return E_LearnSkill_NeedMoreTalentPoint;
	}

	// 技能前置是否达成
	if(GT_INVALID != pProto->dwPreLevelSkillID)
	{
		if(!IsHaveSkillByTypeID(pProto->dwPreLevelSkillID))
		{
			return E_LearnSkill_NoPreSkill;
		}
	}

	return E_Success;
}

int SkillMgr::GetBattleTalentCount()
{
    int nCount = 0;

    for (int i = 0; i < X_MAX_TALENT_PER_ROLE; ++i)
    {
        if (m_talentPoint[i].eType > ETT_Null
            && m_talentPoint[i].eType < ETT_End
            && m_talentPoint[i].eType != ETT_Action
            && m_talentPoint[i].eType != ETT_Jugglery)
            ++nCount;
    }

    return nCount;
}

BOOL SkillMgr::IsTalentAquired( ETalentType eType )
{
    for (int i = 0; i < X_MAX_TALENT_PER_ROLE; ++i)
    {
        if (eType == m_talentPoint[i].eType)
            return TRUE;
    }

    return FALSE;
}