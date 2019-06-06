#include "stdafx.h"
#include "SkillItem.h"
#include "SkillButton.h"
#include "SkillMgr.h"

SkillItem::SkillItem(void)
{
	m_pSkill = NULL;
	m_pStcBack = NULL;
	m_pStcName = NULL;
	m_pStcDesc = NULL;
	m_pStcLv = NULL;
	m_pProExp = NULL;
}

SkillItem::~SkillItem(void)
{
}

VOID SkillItem::ReloadUI(GUIWnd* pWnd, INT nIndex)
{
	TCHAR szTmp[X_SHORT_NAME]={0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("name%d"), nIndex+1);
	m_pStcName = (GUIStatic*)pWnd->GetChild(szTmp);
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("skill%d"), nIndex+1);
	m_pStcBack = (GUIStatic*)pWnd->GetChild(szTmp);
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("skbutt%d"), nIndex+1);
	m_pSkill = (SkillButton*)m_pStcBack->GetChild(szTmp);
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("grad%d"), nIndex+1);
	m_pStcLv = (GUIStatic*)pWnd->GetChild(szTmp);
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("word%d"), nIndex+1);
	m_pStcDesc = (GUIStatic*)pWnd->GetChild(szTmp);
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("pro%d"), nIndex+1);
	m_pProExp = (GUIProgress*)pWnd->GetChild(szTmp);
}

VOID SkillItem::SetInvisible(BOOL b)
{
	m_pStcBack->SetInvisible(b);
	m_pStcName->SetInvisible(b);
	m_pStcDesc->SetInvisible(b);
	m_pStcLv->SetInvisible(b);
	m_pProExp->SetInvisible(b);
	m_pSkill->SetInvisible(b);
	if (b)
	{
		m_pStcBack->SetRefresh();
		m_pStcName->SetRefresh();
		m_pStcDesc->SetRefresh();
		m_pStcLv->SetRefresh();
		m_pProExp->SetRefresh();
		m_pSkill->SetRefresh();
	}
}

GUIWnd* SkillItem::GetSkillWnd()
{
	return m_pSkill;
}

VOID SkillItem::SetName(LPCTSTR szName)
{
	m_pStcName->SetText(szName);
	m_pStcName->FlipToTop();
}

VOID SkillItem::SetLv(INT nLv)
{
	if(NULL == nLv)//--等级为零，则不显示技能等级
	{
		m_pStcLv->SetInvisible(TRUE);
		return;
	}
	TCHAR szTmp[X_SHORT_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("Skill_Level")], nLv);
	m_pStcLv->SetText(szTmp);
	m_pStcLv->SetInvisible(FALSE);
	m_pStcLv->SetRefresh();
}

VOID SkillItem::SetExp(INT nExp, INT nMax)
{
	if(NULL == nMax)//--最大值为零，则表示没有经验，不显示经验条
	{
		m_pProExp->SetInvisible(TRUE);
		return;
	}

	m_pProExp->SetMaxValue((FLOAT)nMax);
	m_pProExp->SetValue((FLOAT)nExp, true);

	TCHAR szTmp[X_SHORT_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d/%d"), nExp, nMax);
	m_pProExp->SetText(szTmp);
	m_pProExp->SetInvisible(FALSE);
	m_pProExp->SetRefresh();
}

VOID SkillItem::SetType( INT nType )
{
	if(NULL == nType || 1 == nType)// 没有技能类型或者普通攻击不显示类型
	{
		m_pStcDesc->SetInvisible(TRUE);
		return;
	}
	
	TCHAR szTmp[X_SHORT_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("Skill_Type%d"), nType);
	m_pStcDesc->SetText(g_StrTable[szTmp]);
	m_pStcDesc->SetInvisible(FALSE);
	m_pStcDesc->SetRefresh();

}

VOID SkillItem::UpdateSkill(DWORD dwID)
{
	FLOAT fCDCount = 0.0f;
	FLOAT fRestoreRatio = 0.0f;

	SetInvisible(FALSE);
	const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillData(dwID);
	if(P_VALID(pSkill))
	{
		SetName(pSkill->pProto->szName);
		SetLv(pSkill->nLevel);

		SetExp(pSkill->nExp, pSkill->pProto->nLevelUpExp);
		SetType(pSkill->pProto->nType2);
		
		m_pSkill->ReflashSkill(pSkill->dwTypeID);
		
		//设置冷却
		SkillMgr::Inst()->GetSkillCDTime(dwID, fCDCount, fRestoreRatio);
		m_pSkill->SetCDTime(fCDCount);
		m_pSkill->SetRestoreRatio(fRestoreRatio);

		return;
	}
		
	SetInvisible(TRUE);
}

DWORD SkillItem::GetSkillID()
{
	return m_pSkill->GetBtnData().GetData1();
}

