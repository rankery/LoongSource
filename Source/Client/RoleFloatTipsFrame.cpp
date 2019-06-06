#include "stdafx.h"
#include "RoleFloatTipsFrame.h"
#include ".\RoleMgr.h"
#include ".\LocalPlayer.h"
#include "WorldFrame.h"
#include "CombatEvent.h"
#include "RoleEvent.h"
#include "SkillMgr.h"
#include "..\WorldDefine\msg_talent.h"
#include "TeamEvent.h"
#include "GameSet.h"
#include "SkillProtoData.h"
#include "ReputeMgr.h"

const float DelayTime = 0.5f;				//停留多长时间后开始上升
const float BombTime = 0.2f;				//弹出爆炸效果时间
const int nInfoRaisePixel = 250;			//最高上升多少个象素

class RoleFloatTipSortFunc
{
public:
	bool operator() (const tagFloatInfo& tip1,const tagFloatInfo& tip2)
	{
		return tip1.srcPos.z > tip2.srcPos.z;
	}
};

RoleFloatTipsFrame::RoleFloatTipsFrame(void):m_Trunk(this)
{
	m_pFont				= NULL;
	m_nMP				= GT_INVALID;
	m_bHPPer10			= TRUE;
	m_bMPPer10			= TRUE;
}

RoleFloatTipsFrame::~RoleFloatTipsFrame(void)
{

}

BOOL RoleFloatTipsFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;
	
	m_pMgr->RegisterEventHandle( _T("tagShowHPChangeEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnShowHPChangeEvent));
	m_pMgr->RegisterEventHandle( _T("tagUpdateLPLevelExp"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnUpdateLPLevelExp));
	m_pMgr->RegisterEventHandle( _T("tagLRoleCombatState"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnLRoleCombatState));
	m_pMgr->RegisterEventHandle( _T("tagAddTeammateEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnAddTeammateEvent));
	m_pMgr->RegisterEventHandle( _T("tagLeaveTeamEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnLeaveTeamEvent));
	m_pMgr->RegisterEventHandle( _T("tagPctNExpAchieve"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnPctNExpAchieve));
	m_pMgr->RegisterEventHandle( _T("ReputeChange"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnReputeChange));
	m_pMgr->RegisterEventHandle( _T("tagReputeLvlChange"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnReputeLvlChange));
	m_pMgr->RegisterEventHandle( _T("tagUpdateLPAttEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnUpdateLPAttEvent));


	m_pCmdMgr->Register("NS_Skill",			(NETMSGPROC)m_Trunk.sfp2(&RoleFloatTipsFrame::OnNS_Skill), _T("NS_Skill"));
	m_pCmdMgr->Register("NS_LearnSkill",	(NETMSGPROC)m_Trunk.sfp2(&RoleFloatTipsFrame::OnNS_LearnSkill), _T("NS_LearnSkill"));

	m_pFont = Device()->NewDepthFont();
	m_pFont->Create(16, 40, 0, FALSE, g_StrTable[_T("Font_Hanyi")]);

	return TRUE;
}

BOOL RoleFloatTipsFrame::Destroy()
{
	for(vector<tagFloatInfo>::iterator iter=m_vFloatInfoArray.begin();
		iter!=m_vFloatInfoArray.end();)
	{
		iter = m_vFloatInfoArray.erase(iter);
	}
	m_pMgr->UnRegisterEventHandler( _T("tagShowHPChangeEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnShowHPChangeEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagUpdateLPLevelExp"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnUpdateLPLevelExp));
	m_pMgr->UnRegisterEventHandler( _T("tagLRoleCombatState"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnLRoleCombatState));
	m_pMgr->UnRegisterEventHandler( _T("tagAddTeammateEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnAddTeammateEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagLeaveTeamEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnLeaveTeamEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagPctNExpAchieve"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnPctNExpAchieve));
	m_pMgr->UnRegisterEventHandler( _T("ReputeChange"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnReputeChange));
	m_pMgr->UnRegisterEventHandler( _T("tagReputeLvlChange"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnReputeLvlChange));
	m_pMgr->UnRegisterEventHandler( _T("tagUpdateLPAttEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&RoleFloatTipsFrame::OnUpdateLPAttEvent));


	m_pCmdMgr->UnRegister("NS_Skill",		(NETMSGPROC)m_Trunk.sfp2(&RoleFloatTipsFrame::OnNS_Skill));
	m_pCmdMgr->UnRegister("NS_LearnSkill",	(NETMSGPROC)m_Trunk.sfp2(&RoleFloatTipsFrame::OnNS_LearnSkill));
	m_AttackOwnPicFont.Destroy();
	m_AttackOtherPicFont.Destroy();
	SAFE_DELETE(m_pFont);
	return TRUE;
}

VOID RoleFloatTipsFrame::OnEvent(tagGameEvent* pEvent)
{

}

DWORD RoleFloatTipsFrame::OnUpdateLPAttEvent(tagUpdateLPAttEvent* pEvent)
{
	DWORD dwLocalID = RoleMgr::Inst()->GetLocalPlayerID();
	
	if( pEvent->eType==ERA_MP && (m_nMP!=GT_INVALID && m_nMP!=0) )
	{
		LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
		if( P_VALID(pLocalPlayer) ) 
		{
			int nCurMP = pLocalPlayer->GetAttribute( ERA_MP );
			int nChange = nCurMP - m_nMP;
			m_nMP  = nCurMP;
			if( nChange>0 )
				PushMPInfo( dwLocalID, nChange);
		}
	}
	else if( pEvent->eType==ERA_MP )
	{
		LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
		if( P_VALID(pLocalPlayer) ) 
			m_nMP = pLocalPlayer->GetAttribute( ERA_MP );
		else 
			m_nMP = GT_INVALID;
	}

	if( pEvent->eType==ERA_HP || pEvent->eType==ERA_MP )
		OnHPorMpExcessPer10();
	return 0;
}

DWORD RoleFloatTipsFrame::OnShowHPChangeEvent(tagShowHPChangeEvent* pEvent)
{
	DWORD dwLocalID = RoleMgr::Inst()->GetLocalPlayerID();
	if(pEvent->dwRoleID==dwLocalID || pEvent->dwSrcRoleID==dwLocalID)
	{
		PushHPInfo(pEvent->dwRoleID, pEvent->nHPChange, pEvent->bMiss, pEvent->bCrit, pEvent->bBlocked);
	}
	return 0;
}

DWORD RoleFloatTipsFrame::OnUpdateLPLevelExp(tagUpdateLPLevelExp* pEvent)
{
	PushUpdateLPLevelExp(pEvent->nExpInc);
	return 0;
}

DWORD RoleFloatTipsFrame::OnPctNExpAchieve(tagPctNExpAchieve* pEvent)
{
	tagFloatInfo hpInfo(RoleMgr::Inst()->GetLocalPlayerID());
	hpInfo.eTypeChange = EChange_NExp;
	_stprintf(hpInfo.szInfo, _T("%d%s"), pEvent->nPctMultiplied, g_StrTable[_T("ShowFloatTips_NExp")]);
	CreateFloatInfo(hpInfo);
	return 0;
}

DWORD RoleFloatTipsFrame::OnLRoleCombatState(tagLRoleCombatState* pEvent)
{	
	tagFloatInfo hpInfo(RoleMgr::Inst()->GetLocalPlayerID());
	if(pEvent->bEnterCombat)
	{
		hpInfo.eTypeChange = EChange_EnterCombat;
		_stprintf(hpInfo.szInfo, _T("%s"), g_StrTable[_T("ShowFloatTips_EnterCombat")]);
	}
	else 
	{
		hpInfo.eTypeChange = EChange_ExitCombat;
		_stprintf(hpInfo.szInfo, _T("%s"), g_StrTable[_T("ShowFloatTips_ExitCombat")]);
	}
	CreateFloatInfo(hpInfo);
	return 0;
}

DWORD RoleFloatTipsFrame::OnAddTeammateEvent(tagAddTeammateEvent* pEvent)
{
	if( pEvent->dwRoleID == RoleMgr::Inst()->GetLocalPlayerID() )
	{
		tagFloatInfo hpInfo(pEvent->dwRoleID);
		hpInfo.eTypeChange = EChange_EnterTeam;
		_stprintf(hpInfo.szInfo, _T("%s"), g_StrTable[_T("ShowFloatTips_EnterTeam")]);
		CreateFloatInfo(hpInfo);
	}
	return 0;
}

DWORD RoleFloatTipsFrame::OnLeaveTeamEvent(tagLeaveTeamEvent* pEvent)
{
	tagFloatInfo hpInfo(RoleMgr::Inst()->GetLocalPlayerID());
	hpInfo.eTypeChange = EChange_ExitTeam;
	_stprintf(hpInfo.szInfo, _T("%s"), g_StrTable[_T("ShowFloatTips_ExitTeam")]);
	CreateFloatInfo(hpInfo);
	return 0;
}

DWORD RoleFloatTipsFrame::OnReputeChange(tagReputeChange* pEvent)
{
	//声望的变化
	tagFloatInfo hpInfo(RoleMgr::Inst()->GetLocalPlayerID());
	hpInfo.eTypeChange = EChange_Reputation;
	TCHAR szBuff[X_SHORT_NAME] = {0};
	_sntprintf( szBuff, X_SHORT_NAME, _T("QuestMain_RepName2%d"), pEvent->eClanType );
	if( pEvent->nChange >= 0 )
	{
		_stprintf(hpInfo.szInfo, _T("+%d%s%s"), pEvent->nChange, g_StrTable[szBuff], g_StrTable[_T("Reputation")]);
	}
	else 
	{
		_stprintf(hpInfo.szInfo, _T("%d%s%s"), pEvent->nChange, g_StrTable[szBuff], g_StrTable[_T("Reputation")]);
	}
	
	CreateFloatInfo(hpInfo);
	return 0;
}

DWORD RoleFloatTipsFrame::OnReputeLvlChange(tagReputeLvlChange* pEvent)
{
	//声望等级变化
	tagFloatInfo hpInfo(RoleMgr::Inst()->GetLocalPlayerID());
	hpInfo.eTypeChange = EChange_ReputaLel;
	TCHAR szRepuLev[X_SHORT_NAME] = {0};
	TCHAR szRepu[X_SHORT_NAME] = {0};
	_sntprintf( szRepu, X_SHORT_NAME, _T("QuestMain_RepName2%d"), pEvent->eClanType );
	_sntprintf( szRepuLev, X_SHORT_NAME, _T("ReputeLevel%d"), pEvent->eLevel );
	_stprintf(hpInfo.szInfo, _T("%s%s%s"), g_StrTable[szRepu], g_StrTable[_T("ReputeIs")], g_StrTable[szRepuLev]);
	CreateFloatInfo(hpInfo);
	return 0;
}

DWORD RoleFloatTipsFrame::OnNS_LearnSkill(tagNS_LearnSkill* pMsg, DWORD dwParam)
{
	if(pMsg->dwErrorCode==E_Success)
	{
		PushStudySkill(pMsg->dwSkillID);
	}
	return 0;
}

DWORD RoleFloatTipsFrame::OnNS_Skill(tagNS_Skill* pMsg, DWORD dwParam)
{
	if( GameSet::Inst()->Get(EGS_ShowSkillNameFloatTip) )
	{
		DWORD dwLocalID = RoleMgr::Inst()->GetLocalPlayerID();
		if(pMsg->dwErrorCode!=E_Success)
			return 0; 
		if(pMsg->dwSrcRoleID==dwLocalID)
			PushLocalRoleUseSkill(pMsg->dwSkillID);
	}
	return 0;
}

VOID RoleFloatTipsFrame::Update()
{
	
}

VOID RoleFloatTipsFrame::Render3D()
{
	DrawFloatInfo();
}

void RoleFloatTipsFrame::DrawFloatInfo()
{
	float nowTime = Kernel::Inst()->GetAccumTime();
	ClientCamera* pCamera = GetWorldFrm()->GetCamera();
	
	for(vector<tagFloatInfo>::iterator iter=m_vFloatInfoArray.begin();
		iter!=m_vFloatInfoArray.end();)
	{
		tagFloatInfo& info = (*iter);
		Role *pRole = RoleMgr::Inst()->FindRole(info.roleID,false,true);
		tagPoint ptView;
		if(nowTime-info.startTime>info.stayTime || pRole==NULL )
		{
			iter = m_vFloatInfoArray.erase(iter);
		}
		else
		{	
			if( !info.bFrist )
			{
				pRole->GetHeadPos(info.worldPos);	
				info.bFrist = true;
			}

			info.pt = pCamera->WorldToScreen(info.worldPos);
			info.srcPos = pCamera->WorldToScreenF(info.worldPos);
			info.srcPos.z /= 3;
			switch( info.eTypeChange )
			{
			case EChange_NExp:
				info.pt.y += 75;
				break;
			case EChange_ReputaLel:
				info.pt.y -= 40;
				break;
			case EChange_Reputation:
				info.pt.y -= 20;
				break;
			}
			++iter;
		}
	}

	//按照Z值排序
	RoleFloatTipSortFunc sortFunc;
	std::sort(m_vFloatInfoArray.begin(), m_vFloatInfoArray.end(), sortFunc);

	//依次画出
	int clientW=Device()->GetClientWidth();
	int clientH=Device()->GetClientHeight();

	IDraw2D::Inst()->BeginDraw(IDraw2D::EBlend_Alpha, true);
	for(vector<tagFloatInfo>::iterator iter=m_vFloatInfoArray.begin(); 
		iter!=m_vFloatInfoArray.end(); ++iter)
	{
		tagFloatInfo& info=(*iter);
		info.pt.y -= 25;
		if( (IS_PLAYER(info.roleID) || IS_CREATURE(info.roleID)) )
			info.pt.y -= 30;
		
		//插值
		float fRaiseR = 0.0f;
		float fZoom = 1.1f;
		
		//停留一定的时间
		if((nowTime-info.startTime) > BombTime)
		{
			fRaiseR = (nowTime-info.startTime) / info.stayTime;	
		}
		else
		{
			fZoom = (nowTime-info.startTime)*1.8f / BombTime;	
			info.pt.x -= (INT)((fZoom - 1.0f) * info.nWidth / 2);
			info.pt.y -= (INT)((fZoom - 1.0f) * info.nHeight / 2);
		}

		//0.5秒后淡出
		if( (nowTime-info.startTime) > DelayTime )
		{
			info.fontColor.A = BYTE(255*(1.2f-fRaiseR));
		}
		
		fRaiseR = CosLerp(0.0f, 1.0f, fRaiseR);
		CalFontPos(info, (int(nInfoRaisePixel * fRaiseR)), info.pt);
	
		switch( info.eTypeChange )
		{
		case EChange_HPInc:
		case EChange_HPDec:
		case EChange_MPInc:
		case EChange_MPDec:
		case EChange_Miss:
		case EChange_Crit:
			{
				
				if( info.roleID == RoleMgr::Inst()->GetLocalPlayer()->GetID() )
					m_AttackOwnPicFont.DrawString(info.szInfo,info.pt.x,info.pt.y,info.srcPos.z,info.fontColor,-1,1,fZoom);
				else
					m_AttackOtherPicFont.DrawString(info.szInfo,info.pt.x,info.pt.y,info.srcPos.z,info.fontColor,-1,1,fZoom);	
				
			}
			break;
		case EChange_Blocked:			//格挡
		case EChange_EnterSafe:			//进入安全区
		case EChange_ExitSafe:			//离开安全区
		case EChange_Exp:
		case EChange_NExp:
		case EChange_Skill:
		case EChange_StudySkill:
		case EChange_EnterCombat:
		case EChange_ExitCombat:	
		case EChange_EnterTeam:
		case EChange_ExitTeam:
		case EChange_Reputation:
		case EChange_ReputaLel:
		case EChange_HPPer10:
		case EChange_MPPer10:
			{
				m_pFont->Begin();
				RECT rcTitle;
				rcTitle.left = info.pt.x - info.ptTxtSize.x;
				rcTitle.right = info.pt.x + info.ptTxtSize.x;
				rcTitle.top = info.pt.y - info.ptTxtSize.y-8;
				rcTitle.bottom = info.pt.y - 8;

				if(rcTitle.left >=0 && rcTitle.top >= 0
					&& rcTitle.right < clientW && rcTitle.bottom < clientH)
				{
					RECT rect1;
					rect1.top = rcTitle.top-1;
					rect1.bottom = rcTitle.bottom-1;
					rect1.left = rcTitle.left;
					rect1.right = rcTitle.right;
					m_pFont->DrawText(info.szInfo, &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, info.srcPos.z);

					rect1.top = rcTitle.top+1;
					rect1.bottom = rcTitle.bottom+1;
					rect1.left = rcTitle.left;
					rect1.right = rcTitle.right;
					m_pFont->DrawText(info.szInfo, &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, info.srcPos.z);

					rect1.top = rcTitle.top;
					rect1.bottom = rcTitle.bottom;
					rect1.left = rcTitle.left-1;
					rect1.right = rcTitle.right-1;
					m_pFont->DrawText(info.szInfo, &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, info.srcPos.z);

					rect1.top = rcTitle.top;
					rect1.bottom = rcTitle.bottom;
					rect1.left = rcTitle.left+1;
					rect1.right = rcTitle.right+1;
					m_pFont->DrawText(info.szInfo, &rect1, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, info.srcPos.z);

					info.fontColor.A = BYTE(255);
					m_pFont->DrawText(info.szInfo, &rcTitle, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, info.fontColor, info.srcPos.z);
				}
				m_pFont->End();				
			}
		}
	}
	IDraw2D::Inst()->EndDraw();
	
}

void RoleFloatTipsFrame::PushHPInfo(DWORD roleID, int damage, bool bMiss, bool bCrit, bool bBlocked)
{
	//--处理头顶的数字
	tagFloatInfo hpInfo(roleID);
	if(damage > 0)															//加血
	{
		hpInfo.eTypeChange = EChange_HPInc;		
		TCHAR szBuff[64]={0};
		_itot(damage, szBuff, 10);
		tstringstream strBuff;
		strBuff << _T("+") << szBuff;
		_stprintf(hpInfo.szInfo, _T("%s"), strBuff.str().c_str());
	}
	else if( damage < 0)													//减血
	{
		
		TCHAR szBuff[64]={0};
		_itot(damage, szBuff, 10);
		tstringstream strBuff;
		strBuff << szBuff;
		if(bCrit)
		{
			hpInfo.eTypeChange = EChange_Crit;	
			_stprintf(hpInfo.szInfo, _T("%s%s"),  g_StrTable[_T("ShowFloatTips_Crit")], strBuff.str().c_str());
		}
		else
		{
			hpInfo.eTypeChange = EChange_HPDec;	
			_stprintf(hpInfo.szInfo, _T("%s"), strBuff.str().c_str());
		}
	}
	else if( damage==0 )
	{
		if(bMiss)
		{
			hpInfo.eTypeChange = EChange_Miss;	
			_stprintf(hpInfo.szInfo, _T("%s"), g_StrTable[_T("ShowFloatTips_Miss")]);
		}
		else if(bBlocked)
		{
			hpInfo.eTypeChange = EChange_Blocked;
			_stprintf(hpInfo.szInfo, _T("%s"), g_StrTable[_T("ShowFloatTips_Blocked")]);
		}
	}

	if( hpInfo.eTypeChange == EChange_Blocked )
	{	
		CreateFloatInfo(hpInfo);
		return;
	}
	//计算字产生时的位置
	CalCreateFontPos(hpInfo);
	//计算字的颜色
	CalFontColor(hpInfo.roleID, hpInfo.eTypeChange, hpInfo.fontColor);
	m_AttackOwnPicFont.GetDrawStringSize(hpInfo.szInfo, hpInfo.nWidth, hpInfo.nHeight);
	//缓存
	m_vFloatInfoArray.push_back(hpInfo);
	
}

void RoleFloatTipsFrame::PushMPInfo(DWORD roleID, int damage)
{
	//--处理头顶的数字
	tagFloatInfo hpInfo(roleID);
	if(damage > 0)															//加MP
	{
		hpInfo.eTypeChange = EChange_MPInc;		
		TCHAR szBuff[64]={0};
		_itot(damage, szBuff, 10);
		tstringstream strBuff;
		strBuff << _T("+") << szBuff;
		_stprintf(hpInfo.szInfo, _T("%s"), strBuff.str().c_str());
	}
	else if( damage < 0)													//减MP
	{
		hpInfo.eTypeChange = EChange_MPDec;	
		TCHAR szBuff[64]={0};
		_itot(damage, szBuff, 10);
		tstringstream strBuff;
		strBuff << szBuff;
		_stprintf(hpInfo.szInfo, _T("%s"), strBuff.str().c_str());
	}

	//计算字产生时的位置
	CalCreateFontPos(hpInfo);
	//计算字的颜色
	CalFontColor(hpInfo.roleID, hpInfo.eTypeChange, hpInfo.fontColor);
	m_AttackOwnPicFont.GetDrawStringSize(hpInfo.szInfo, hpInfo.nWidth, hpInfo.nHeight);
	//缓存
	m_vFloatInfoArray.push_back(hpInfo);

}

void RoleFloatTipsFrame::PushEnterOrExitSafe(DWORD roleID, bool bEnter)
{
	tagFloatInfo hpInfo(roleID);
	if(bEnter)
	{
		hpInfo.eTypeChange = EChange_EnterSafe;		
		_stprintf(hpInfo.szInfo, _T("%s"), g_StrTable[_T("ShowFloatTips_EnterSafe")]);
	}
	else
	{
		hpInfo.eTypeChange = EChange_ExitSafe;		
		_stprintf(hpInfo.szInfo, _T("%s"), g_StrTable[_T("ShowFloatTips_ExitSafe")]);
	}
	CreateFloatInfo(hpInfo);
}

void RoleFloatTipsFrame::PushUpdateLPLevelExp(INT nExpInc)
{
	if( nExpInc>0 )
	{
		tagFloatInfo hpInfo(RoleMgr::Inst()->GetLocalPlayerID());
		hpInfo.eTypeChange = EChange_Exp;
		_stprintf(hpInfo.szInfo, _T("%d %s"), nExpInc, g_StrTable[_T("ShowFloatTips_Exp")]);
		CreateFloatInfo(hpInfo);
	}
	else if( nExpInc<0 )
	{
		tagFloatInfo hpInfo(RoleMgr::Inst()->GetLocalPlayerID());
		hpInfo.eTypeChange = EChange_Exp;
		_stprintf(hpInfo.szInfo, _T("%d %s"), nExpInc, g_StrTable[_T("ShowFloatTips_Exp")]);
		CreateFloatInfo(hpInfo);
	}
}

void RoleFloatTipsFrame::PushLocalRoleUseSkill(DWORD dwSkill)
{
	//普通攻击不显示
	DWORD dwSkillID = MTransSkillID(dwSkill);
	if( IsNormalAttackSkill(dwSkillID) )
		return;

	//生产技能以及特殊说明隐藏的技能不显示
	const tagSkillProtoClient* pSkillData = SkillProtoData::Inst()->FindSkillProto(dwSkill);
	if( !P_VALID(pSkillData) )
		return;
	if( pSkillData->nType2 == ESSTE_Produce )
		return;
	if( pSkillData->nType3 == ESSTE2_Hidden )
		return;

	const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillDataByTypeID(dwSkill);
	if( !P_VALID(pSkill) )
		return;

	tagFloatInfo skillInfo(RoleMgr::Inst()->GetLocalPlayerID());
	skillInfo.eTypeChange = EChange_Skill;
	_stprintf(skillInfo.szInfo, _T("%s"), pSkill->pProto->szName);
	CreateFloatInfo(skillInfo);
}

void RoleFloatTipsFrame::PushStudySkill(DWORD dwSkillID)
{
	const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillDataByTypeID(dwSkillID);
	if( !P_VALID(pSkill) )
		return;
	tagFloatInfo skillInfo(RoleMgr::Inst()->GetLocalPlayerID());
	skillInfo.eTypeChange = EChange_StudySkill;
	_stprintf(skillInfo.szInfo, g_StrTable[_T("LearnSkill_Success_Float")], pSkill->pProto->szName);
	CreateFloatInfo(skillInfo);
}

void RoleFloatTipsFrame::CreateFloatInfo(tagFloatInfo& info)
{
	switch(info.eTypeChange)
	{
	case EChange_Blocked:		
	case EChange_EnterSafe:
	case EChange_ExitSafe:
	case EChange_EnterCombat:
	case EChange_ExitCombat:
	case EChange_EnterTeam:
	case EChange_ExitTeam:
	case EChange_Exp:
	case EChange_NExp:
	case EChange_Skill:
	case EChange_StudySkill:
	case EChange_Reputation:
	case EChange_ReputaLel:
	case EChange_HPPer10:
	case EChange_MPPer10:		
		{

			tagPoint txtSize;	
			m_pRender->GetTextSize(info.szInfo, NULL, txtSize);
			info.ptTxtSize.x = txtSize.x*2;
			info.ptTxtSize.y = txtSize.y*2;
		}
		break;
	}
	//计算字的颜色
	CalFontColor(info.roleID, info.eTypeChange, info.fontColor);
	//计算字产生时的位置
	CalCreateFontPos(info);
	//缓存
	m_vFloatInfoArray.push_back(info);
}

void RoleFloatTipsFrame::CalCreateFontPos(tagFloatInfo& info)
{
	for(vector<tagFloatInfo>::iterator iter=m_vFloatInfoArray.begin(); 
		iter!=m_vFloatInfoArray.end(); ++iter)
	{
		//相同角色相同类型的飘浮信息产生偏移值
		if(iter->roleID==info.roleID)
		{
			switch(info.eTypeChange)
			{
			case EChange_HPInc:
			case EChange_HPDec:
			case EChange_MPInc:
			case EChange_MPDec:
			case EChange_Miss:
			case EChange_Crit:
				{
					info.ptOffset.x = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%(200-150)+150)) : m_pUtil->Rand()%(200-150)+150); 
					info.ptOffset.y = (((m_pUtil->Rand()%100)>50) ? m_pUtil->Rand()%10 : m_pUtil->Rand()%(80-20)+20); 
				}
				return;
				break;
			case EChange_Blocked:
			case EChange_ReputaLel:
				{
					info.ptOffset.x = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%(20-10)+10)) : m_pUtil->Rand()%(20-10)+10); 
					info.ptOffset.y = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%10)) : m_pUtil->Rand()%50); 
				}
				return;
				break;
			case EChange_Exp:
			case EChange_NExp:
			case EChange_EnterSafe:
			case EChange_ExitSafe:
			case EChange_EnterCombat:
			case EChange_ExitCombat:
			case EChange_EnterTeam:
			case EChange_ExitTeam:
			case EChange_Reputation:
				{
					info.ptOffset.x = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%5)) : m_pUtil->Rand()%5); 
					info.ptOffset.y = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%10)) : m_pUtil->Rand()%50); 
				}
				return;
				break;
			case EChange_Skill:
			case EChange_StudySkill:	
			case EChange_HPPer10:
			case EChange_MPPer10:
				{
					info.ptOffset.x = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%(150-120)+120)) : m_pUtil->Rand()%(150-120)+120); 
					info.ptOffset.y = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%20)) : m_pUtil->Rand()%60); 
				}
				return;
				break;
			default:
				{
					info.ptOffset.x = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%120)) : m_pUtil->Rand()%120); 
					info.ptOffset.y = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%10)) : m_pUtil->Rand()%50); 
				}
				return;
				break;
			}
		}
	}

	switch(info.eTypeChange)
	{
	case EChange_HPInc:
	case EChange_HPDec:
	case EChange_MPInc:
	case EChange_MPDec:
	case EChange_Miss:
	case EChange_Crit:
		{
			info.ptOffset.x = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%(200-150)+150)) : m_pUtil->Rand()%(200-150)+150); 
			info.ptOffset.y = (((m_pUtil->Rand()%100)>50) ? m_pUtil->Rand()%10 : m_pUtil->Rand()%(80-20)+20); 
		};
		break;
	case EChange_Blocked:
	case EChange_ReputaLel:
		{
			info.ptOffset.x = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%(20-10)+10)) : m_pUtil->Rand()%(20-10)+10); 
			info.ptOffset.y = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%10)) : m_pUtil->Rand()%50); 
		}
		break;
	case EChange_Exp:
	case EChange_NExp:
	case EChange_EnterSafe:
	case EChange_ExitSafe:
	case EChange_EnterCombat:
	case EChange_ExitCombat:
	case EChange_EnterTeam:
	case EChange_ExitTeam:
	case EChange_Reputation:
		{
			info.ptOffset.x = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%5)) : m_pUtil->Rand()%5); 
			info.ptOffset.y = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%10)) : m_pUtil->Rand()%50); 
		}
		break;
	case EChange_Skill:
	case EChange_StudySkill:	
	case EChange_HPPer10:
	case EChange_MPPer10:
		{
			info.ptOffset.x = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%(150-120)+120)) : m_pUtil->Rand()%(150-120)+120); 
			info.ptOffset.y = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%20)) : m_pUtil->Rand()%60); 
		}
		break;
	default:
		{
			info.ptOffset.x = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%120)) : m_pUtil->Rand()%120); 
			info.ptOffset.y = (((m_pUtil->Rand()%100)>50) ? (-(m_pUtil->Rand()%10)) : m_pUtil->Rand()%50); 
		}
		break;
	}
}

void RoleFloatTipsFrame::CalFontPos(const tagFloatInfo& info,int nChange, POINT& pt )
{
	pt.x += info.ptOffset.x;
	pt.y += info.ptOffset.y;
}

void RoleFloatTipsFrame::CalFontColor( DWORD dwRoleID, EChangeType eType, Color4ub &fontColor )
{	
	static Color4ub pinkColor = Color4ub(255, 255, 229, 165);			//粉色
	static Color4ub redColor = Color4ub(255, 238, 0, 0 );				//红色
	static Color4ub greenColor = Color4ub(255, 0, 182, 94);				//绿色
	static Color4ub greenColor1 = Color4ub(255, 17, 235, 0);			//淡绿色
	static Color4ub greenColor2 = Color4ub(255, 198, 255, 0);			//苹果绿色
	static Color4ub blueColor = Color4ub(255, 0, 162, 255);				//蓝色
	static Color4ub buleColor1 = Color4ub(255, 35, 221, 255);			//淡蓝色
	static Color4ub purpleColor = Color4ub(255, 185, 137, 251);			//紫色
	static Color4ub whiteColor = Color4ub(255, 255, 247, 224);			//白色
	static Color4ub goldenColor = Color4ub(255, 187, 173, 55);			//金色
	static Color4ub brownColor = Color4ub(255, 233, 174, 33);			//褐色
																		//梅红色
	static Color4ub yellowColor = Color4ub(255, 255, 226, 94);			//黄色
	static Color4ub yellowColor1 = Color4ub(255, 255, 235, 144);		//淡黄色


	// 字体的颜色设置
	if(dwRoleID==RoleMgr::Inst()->GetLocalPlayer()->GetID())
	{
		//玩家本人
		switch( eType )
		{
		case EChange_HPDec:
			fontColor = whiteColor;
			break;
		case EChange_EnterCombat:
			fontColor = redColor;
			break;
		case EChange_HPInc:
		case EChange_ExitCombat:
			fontColor = greenColor;
			break;
		case EChange_Crit:
			fontColor = whiteColor;
			break;
		case EChange_Miss:
			fontColor = whiteColor;
			break;
		case EChange_MPInc:
		case EChange_MPDec:
		case EChange_EnterTeam:
		case EChange_ExitTeam:
			fontColor = blueColor;
			break;
		case EChange_Blocked:
			fontColor = brownColor;
			break;
		case EChange_Exp:
			fontColor = goldenColor;
			break;
		case EChange_NExp:
			fontColor = goldenColor;
			break;
		case EChange_EnterSafe:
			fontColor = Color4ub(255, 0, 255, 0);
			break;
		case EChange_ExitSafe:
			fontColor = Color4ub(255, 255, 0, 0);
			break;
		case EChange_Skill:
		case EChange_StudySkill:
			fontColor = goldenColor;
			break;
		case EChange_Reputation:
			fontColor = brownColor;
			break;
		case EChange_ReputaLel:
			fontColor = greenColor;
			break;
		case EChange_HPPer10:
			fontColor = redColor;
			break;
		case EChange_MPPer10:
			fontColor = blueColor;
			break;
		}
	}
	//其他玩家或怪
	else if(IS_PLAYER(dwRoleID) || IS_CREATURE(dwRoleID))
	{
		switch( eType )
		{
		case EChange_HPDec:
			fontColor = whiteColor;
			break;
		case EChange_HPInc:
			fontColor = greenColor;
			break;
		case EChange_Crit:
			fontColor = whiteColor;
			break;
		case EChange_Miss:
			fontColor = whiteColor;
			break;
		case EChange_Blocked:
			fontColor = pinkColor;
			break;
		case EChange_MPInc:
		case EChange_MPDec:
			fontColor = blueColor;
			break;
		}
	}
}

BOOL RoleFloatTipsFrame::ReloadUI()
{
	IFS* pFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");

	m_AttackOtherPicFont.LoadPic( _T('-'), _T("data\\ui\\mark\\battle\\L_red-minus.bmp"), pFS );
	m_AttackOtherPicFont.LoadPic( _T('+'), _T("data\\ui\\mark\\battle\\L_red-plus.bmp"), pFS );
	m_AttackOtherPicFont.LoadPic( 'A', _T("data\\ui\\mark\\battle\\L_red-critical.bmp"), pFS );		//暴击
	m_AttackOtherPicFont.LoadPic( 'B', _T("data\\ui\\mark\\battle\\L_red-miss.bmp"), pFS );			//闪

	m_AttackOwnPicFont.LoadPic( _T('-'), _T("data\\ui\\mark\\battle\\L_yel-minus.bmp"), pFS );
	m_AttackOwnPicFont.LoadPic( _T('+'), _T("data\\ui\\mark\\battle\\L_yel-plus.bmp"), pFS );
	m_AttackOwnPicFont.LoadPic( 'A', _T("data\\ui\\mark\\battle\\L_yel-critical.bmp"), pFS );		//暴击
	m_AttackOwnPicFont.LoadPic( 'B', _T("data\\ui\\mark\\battle\\L_yel-miss.bmp"), pFS );			//闪

	TCHAR szTemp[_MAX_PATH]={0};
	for (int i=0; i<10; i++)
	{
		_sntprintf( szTemp, _MAX_PATH, _T("data\\ui\\mark\\battle\\L_red-%c.bmp"), i+_T('0') );
		m_AttackOtherPicFont.LoadPic( TCHAR(i+_T('0')), szTemp, pFS );

		_sntprintf( szTemp, _MAX_PATH, _T("data\\ui\\mark\\battle\\L_yel-%c.bmp"), i+_T('0') );
		m_AttackOwnPicFont.LoadPic( TCHAR(i+_T('0')), szTemp, pFS );
	}
	return TRUE;
}

void RoleFloatTipsFrame::OnHPorMpExcessPer10()
{
	LocalPlayer* pLP = NULL;
	pLP = RoleMgr::Inst()->GetLocalPlayer();
	if( P_VALID(pLP) )
	{
		//判断体力
		const int nMaxHp	= pLP->GetAttribute( ERA_MaxHP );
		const int nHp		= pLP->GetAttribute( ERA_HP );
		if( (nHp!=GT_INVALID && nMaxHp!=GT_INVALID) && nHp<nMaxHp*0.2 && !m_bHPPer10 )
		{
			m_bHPPer10 = TRUE;
			tagFloatInfo hpInfo(RoleMgr::Inst()->GetLocalPlayerID());
			hpInfo.eTypeChange = EChange_HPPer10;
			_stprintf( hpInfo.szInfo, _T("%s"), g_StrTable[_T("ShowFloatTips_HpExcess10")] );
			CreateFloatInfo(hpInfo);
		}
		else if( (nHp!=GT_INVALID && nMaxHp!=GT_INVALID) && nHp>nMaxHp*0.2 && m_bHPPer10 )
		{
			m_bHPPer10 = FALSE;
		}

		//判断真气
		const int nMaxMp	= pLP->GetAttribute( ERA_MaxMP );
		const int nMp		= pLP->GetAttribute( ERA_MP );
		if( (nMp!=GT_INVALID && nMaxMp!=GT_INVALID) && nMp<nMaxMp*0.2 && !m_bMPPer10 )
		{
			m_bMPPer10 = TRUE;
			tagFloatInfo hpInfo(RoleMgr::Inst()->GetLocalPlayerID());
			hpInfo.eTypeChange = EChange_MPPer10;
			_stprintf( hpInfo.szInfo, _T("%s"), g_StrTable[_T("ShowFloatTips_MpExcess10")]);
			CreateFloatInfo(hpInfo);
		}
		else if( (nMp!=GT_INVALID && nMaxMp!=GT_INVALID) && nMp>nMaxMp*0.2 && m_bMPPer10 )
		{
			m_bMPPer10 = FALSE;
		}
	}
}