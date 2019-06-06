#include "stdafx.h"
#include "TalentTreeFrame.h"
#include "TalentFrame.h"
#include "..\WorldDefine\msg_talent.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "SkillMgr.h"
#include "GameClientDefine.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "TalentTree.h"
#include "RoleEvent.h"
#include "SkillEvent.h"

TalentTreeFrame::TalentTreeFrame():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pStcAvail = NULL;
	m_pStcBidden = NULL;
	for (INT i=0; i<ETT_End; ++i)
	{
		m_pPbnTalent[i]=NULL;
	}

	m_nCurBidden = GT_INVALID;
	m_eCurTalent = ETT_Null;
	m_nCurAvail = GT_INVALID;
	m_nCurTalentNum = GT_INVALID;

}

TalentTreeFrame::~TalentTreeFrame()
{
	
}

BOOL TalentTreeFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
	
	m_pFrameMgr->RegisterEventHandle(_T("tagUpdateLPSkillEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentTreeFrame::OnUpdateLPSkillEvent));
	m_pFrameMgr->RegisterEventHandle(_T("tagUpdateLPLevel"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentTreeFrame::OnUpdateLPLevel));
	m_pFrameMgr->RegisterEventHandle(_T("tagUpdateLPAttEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentTreeFrame::OnUpdateLPAttEvent));
	m_pFrameMgr->RegisterEventHandle(_T("tagUpdateLPTalentPoint"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentTreeFrame::OnUpdateLPTalentPoint));
	m_pFrameMgr->RegisterEventHandle(_T("SkillEvent_ClearTalent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentTreeFrame::OnEventClearTalent));
	
	return bRet;
}

BOOL TalentTreeFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();
	
	// 天赋树数据释放
	m_TalentTree.Destroy();

	m_pFrameMgr->UnRegisterEventHandler(_T("tagUpdateLPSkillEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentTreeFrame::OnUpdateLPSkillEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("tagUpdateLPLevel"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentTreeFrame::OnUpdateLPLevel));
	m_pFrameMgr->UnRegisterEventHandler(_T("tagUpdateLPAttEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentTreeFrame::OnUpdateLPAttEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("tagUpdateLPTalentPoint"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentTreeFrame::OnUpdateLPTalentPoint));
	m_pFrameMgr->UnRegisterEventHandler(_T("SkillEvent_ClearTalent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TalentTreeFrame::OnEventClearTalent));

	return bRet;
}

BOOL TalentTreeFrame::ReloadUI()
{
	m_pWnd = ((TalentFrame*)m_pFather)->GetTalentTree();
	
	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&TalentTreeFrame::EventHandler));

	m_pStcBidden				=	(GUIStatic*)m_pWnd->GetChild(_T("skillbuttback\\point1"));
	m_pStcAvail					=	(GUIStatic*)m_pWnd->GetChild(_T("skillbuttback\\point2"));
	m_pPbnTalent[ETT_Sword]		=	(GUIPushButton*)m_pWnd->GetChild(_T("skillbuttback\\buttframe\\escrime"));
	m_pPbnTalent[ETT_Blade]		=	(GUIPushButton*)m_pWnd->GetChild(_T("skillbuttback\\buttframe\\falchion"));
	m_pPbnTalent[ETT_Wand]		=	(GUIPushButton*)m_pWnd->GetChild(_T("skillbuttback\\buttframe\\stick"));
	m_pPbnTalent[ETT_Bow]		=	(GUIPushButton*)m_pWnd->GetChild(_T("skillbuttback\\buttframe\\bow"));
	m_pPbnTalent[ETT_Action]	=	(GUIPushButton*)m_pWnd->GetChild(_T("skillbuttback\\buttframe\\tactics"));
	m_pPbnTalent[ETT_Valor]		=	(GUIPushButton*)m_pWnd->GetChild(_T("skillbuttback\\buttframe\\strong"));
	m_pPbnTalent[ETT_Supple]	=	(GUIPushButton*)m_pWnd->GetChild(_T("skillbuttback\\buttframe\\tender"));
	m_pPbnTalent[ETT_Poison]	=	(GUIPushButton*)m_pWnd->GetChild(_T("skillbuttback\\buttframe\\drug"));
	m_pPbnTalent[ETT_Swing]		=	(GUIPushButton*)m_pWnd->GetChild(_T("skillbuttback\\buttframe\\swing"));
	m_pPbnTalent[ETT_Jugglery]	=	(GUIPushButton*)m_pWnd->GetChild(_T("skillbuttback\\buttframe\\jugglery"));

	// 天赋树数据创建
	m_TalentTree.Create(m_pWnd->GetChild(_T("skillttreewin\\skilltreebakpic")), m_pGUI);

	m_pPbnTalent[ETT_Sword]->SetState(EGUIBS_PushDown);

	return TRUE;
}

DWORD TalentTreeFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("talentleaf"), 10) == 0)
			{
				tagTalentLeaf* pLeaf = m_TalentTree.GetCurLeaf(pWnd->GetID());
				if(P_VALID(pLeaf))
				{
					if(pLeaf->bActive)
					{
						if(0 == pLeaf->nLearnLevel)
						{
							tagNC_LearnSkill m;
							m.dwSkillID = pLeaf->dwTalentLeaf;
							m.dwNPCID = GT_INVALID;
							m.n64ItemID = GT_INVALID;
							m_pSession->Send(&m);
						}
						else
						{
							tagNC_LevelUpSkill m;
							m.dwSkillID = pLeaf->dwTalentLeaf;
							m.dwNPCID = GT_INVALID;
							m.n64ItemID = GT_INVALID;
							m_pSession->Send(&m);
						}
					}
				}
			}
		}
		break;
	case EGUIE_PushButtonDown:
		{
			if (pEvent->dwParam1 != 1)//不是按下不处理
				return 0;

			for (INT i = 0; i < ETT_End; ++i)
			{
				if(pWnd == m_pPbnTalent[i])
				{
					m_TalentTree.SetCurPage(i);
					m_eCurTalent = (ETalentType)i;
					SetBidden(SkillMgr::Inst()->GetTalentPoint((ETalentType)i));
					break;
				}
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("talentleaf"), 10) == 0)
			{
				tagTalentLeaf* pLeaf = m_TalentTree.GetCurLeaf(pWnd->GetID());
				if(P_VALID(pLeaf))
				{
					tstring strTips;
					if(0 == pLeaf->nCurLevel)
					{
						strTips = ToolTipCreator::Inst()->GetTalentTips(MTransSkillTypeID(pLeaf->dwTalentLeaf, 1));
					}
					else
					{
						strTips = ToolTipCreator::Inst()->GetTalentTips(MTransSkillTypeID(pLeaf->dwTalentLeaf, pLeaf->nCurLevel));
					}
					ShowTip(pWnd, strTips.c_str());	
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(pWnd, NULL);
		}
		break;
	default :
		break;
	}
	return 0;
}

DWORD TalentTreeFrame::OnUpdateLPSkillEvent( tagUpdateLPSkillEvent* pGameEvent )
{
	m_TalentTree.UpdateTalentLeaf(pGameEvent->dwID, pGameEvent->bAdd);
	return 0;
}

DWORD TalentTreeFrame::OnUpdateLPLevel( tagUpdateLPLevel* pGameEvent )
{
	m_TalentTree.OnRoleLevelUp(pGameEvent->nCurLevel, pGameEvent->nInc);
	return 0;
}

DWORD TalentTreeFrame::OnUpdateLPAttEvent( tagUpdateLPAttEvent* pGameEvent )
{
	if(ERA_TalentPoint == pGameEvent->eType)
	{
		INT nPoint = RoleMgr::Inst()->GetLocalPlayer()->GetAttribute(pGameEvent->eType);
		if(0 == nPoint && 0 != m_nCurBidden)
		{
			m_TalentTree.OnNoTalentPoint();
		}
		else if(0 != nPoint)
		{
			m_TalentTree.OnHaveTalentPoint();
		}
		SetAvail(nPoint);
	}
	return 0;
}

DWORD TalentTreeFrame::OnUpdateLPTalentPoint( tagUpdateLPTalentPoint* pGameEvent )
{
	m_TalentTree.OnTalentPointChange(pGameEvent->eType, pGameEvent->nPoint);
	if(MAX_BATTLE_TALENT_COUNT == SkillMgr::Inst()->GetBattleTalentCount())
	{
		m_TalentTree.OnNoTalentTypeCanChoose();
	}
	if(pGameEvent->eType == m_eCurTalent)
	{
		SetBidden(pGameEvent->nPoint);
	}
	m_nCurTalentNum = pGameEvent->nMaxTalent;
	return 0;
}

VOID TalentTreeFrame::SetBidden( INT nValue )
{
	m_nCurBidden = nValue;

	TCHAR szTmp[X_SHORT_NAME]={0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), nValue);
	m_pStcBidden->SetText(szTmp);
	m_pStcBidden->SetRefresh();
}

VOID TalentTreeFrame::SetAvail( INT nValue )
{
	TCHAR szTmp[X_SHORT_NAME]={0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), nValue);
	m_pStcAvail->SetText(szTmp);
	m_pStcAvail->SetRefresh();
	m_nCurAvail = nValue;
}

VOID TalentTreeFrame::OnTalentTreeOpen()
{
	if(NULL == m_nCurAvail)
	{
		m_TalentTree.OnNoTalentPoint();
	}
    if(MAX_BATTLE_TALENT_COUNT == SkillMgr::Inst()->GetBattleTalentCount())
	{
		m_TalentTree.OnNoTalentTypeCanChoose();
	}
}

DWORD TalentTreeFrame::OnEventClearTalent( tagGameEvent* pGameEvent )
{
	m_TalentTree.OnClearAllTalent();

	return 0;
}
