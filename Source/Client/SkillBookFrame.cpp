#include "stdafx.h"
#include "SkillBookFrame.h"
#include "TalentFrame.h"
#include "SkillMgr.h"
#include "DragBtn.h"
#include "ToolTipFrame.h"
#include "ToolTipCreator.h"
#include "SkillEvent.h"


SkillBookFrame::SkillBookFrame():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pStcPage = NULL;
	m_pBtnPageUp = NULL;
	m_pBtnPageDown = NULL;

}

SkillBookFrame::~SkillBookFrame()
{

}

BOOL SkillBookFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
	
	m_pFrameMgr->RegisterEventHandle(_T("tagUpdateLPSkillEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&SkillBookFrame::OnUpdateLPSkillEvent));
	m_pFrameMgr->RegisterEventHandle(_T("Weapon_Swap"),			  (FRAMEEVENTPROC)m_Trunk.sfp1(&SkillBookFrame::OnWeaponSwap));
	
	return bRet;
}

BOOL SkillBookFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	m_pFrameMgr->UnRegisterEventHandler(_T("tagUpdateLPSkillEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&SkillBookFrame::OnUpdateLPSkillEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("Weapon_Swap"),			 (FRAMEEVENTPROC)m_Trunk.sfp1(&SkillBookFrame::OnWeaponSwap));

	return bRet;
}

BOOL SkillBookFrame::ReloadUI()
{
	m_pWnd = ((TalentFrame*)m_pFather)->GetSkillBook();

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&SkillBookFrame::EventHandler));

	m_pStcPage = (GUIStatic*)m_pWnd->GetChild(_T("page\\pagenum"));
	m_pBtnPageUp = (GUIButton*)m_pWnd->GetChild(_T("page\\butt2"));
	m_pBtnPageDown = (GUIButton*)m_pWnd->GetChild(_T("page\\butt1"));
	
	for (INT i=0; i<MAX_SKILL_BOOK_NUM; ++i)
	{
		m_SkillItem[i].ReloadUI(m_pWnd, i);
		m_SkillItem[i].SetInvisible(TRUE);
	}
	
	//--技能列表初始化
	m_skillList.Clear();
	m_skillIter = m_skillList.Begin();
	SetCurPage(0);

	return TRUE;
}

DWORD SkillBookFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if(pWnd == m_pBtnPageDown)
			{
				OnPageUp(FALSE);
			}
			else if(pWnd == m_pBtnPageUp)
			{
				OnPageUp(TRUE);
			}
			else if(_tcsnccmp(pWnd->GetName().c_str(), _T("skbutt"), 6) == 0)
			{

				for(INT i=0; i<MAX_SKILL_BOOK_NUM; ++i)
				{
					if(pWnd == m_SkillItem[i].GetSkillWnd())
					{
						SkillMgr::Inst()->OnUseSkill(m_SkillItem[i].GetSkillID());
						break;
					}
				}
			}
		}
		break;
	case EGUIE_Drag:
		{
			if(_tcsnccmp(pWnd->GetName().c_str(), _T("skbutt"), 6) == 0)
			{

				for(INT i=0; i<MAX_SKILL_BOOK_NUM; ++i)
				{
					if(pWnd == m_SkillItem[i].GetSkillWnd())
					{
						DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
						if (P_VALID(pDrag))
						{
							pEvent->dwParam1 = EDT_Skill;
							pEvent->dwParam2 = m_SkillItem[i].GetSkillID();
							pDrag->OnEvent(pEvent);
							// 需要隐藏Tip
							ShowTip(pWnd, NULL);
						}
						break;
					}
				}
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if(_tcsnccmp(pWnd->GetName().c_str(), _T("skbutt"), 6) == 0)
			{

				for(INT i=0; i<MAX_SKILL_BOOK_NUM; ++i)
				{
					if(pWnd == m_SkillItem[i].GetSkillWnd())
					{
						DWORD dwID = m_SkillItem[i].GetSkillID();
						ShowTip(pWnd, ToolTipCreator::Inst()->GetSkillTips(dwID).c_str());
					}
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

VOID SkillBookFrame::OnSkillBookOpen()
{
	if(m_skillList.Size() <= 0)//--如果为空则
	{
		SkillMgr::Inst()->GetSkillList(m_skillList);
	}

	ReflashCurPage();
}

DWORD SkillBookFrame::OnUpdateLPSkillEvent( tagUpdateLPSkillEvent* pGameEvent )
{
	//--只更新主动技能
	if(ESUT_Active == pGameEvent->eType)	
	{
		//if(pGameEvent->bAdd)//
		//{
		//	if(IsNormalAttackSkill(pGameEvent->dwID) && m_skillList.Size() > 0)
		//		return;
		//	
		//	m_skillList.PushBack(pGameEvent->dwID);
		//	//--如果界面可见且添加的技能为当前页，则刷新技能栏
		//	if(!m_pWnd->IsInvisible() && IsCurPage(m_skillList.Size()-1))
		//	{
		//		m_SkillItem[(m_skillList.Size()-1) % MAX_SKILL_BOOK_NUM].UpdateSkill(pGameEvent->dwID);
		//	}

		//	// 刷新按钮
		//	m_pBtnPageUp->SetEnable(m_nCurPage < m_skillList.Size()/MAX_SKILL_BOOK_NUM);
		//	m_pBtnPageDown->SetEnable(m_nCurPage > 0);
		//}
		//else
		//{
		SkillMgr::Inst()->GetSkillList(m_skillList);
		ReflashCurPage();
		/*}*/
	}
	return 0;
}

VOID SkillBookFrame::ReflashCurPage()
{
	// 如果技能列表数量小于当前技能页的最小值，则直接显示第一页的技能
	if(m_skillList.Size() <= MAX_SKILL_BOOK_NUM*m_nCurPage)
	{
		SetCurPage(0);
	}
	m_skillIter = m_skillList.Begin();
	INT i = 0;
	for(; !IsCurPage(i); ++i)
	{
		++m_skillIter;
	}

	DWORD dwID;
	i = 0;
	while(m_skillList.PeekNext(m_skillIter, dwID))
	{
		if(i >= MAX_SKILL_BOOK_NUM)	
			break;

		m_SkillItem[i].UpdateSkill(dwID);
		++i;
	}
	// 如果没有到最后一个格，剩余格不显示
	for(; i<MAX_SKILL_BOOK_NUM; ++i)
	{
		m_SkillItem[i].UpdateSkill(GT_INVALID);
	}

	// 刷新按钮
	m_pBtnPageUp->SetEnable(m_nCurPage < m_skillList.Size()/MAX_SKILL_BOOK_NUM);
	m_pBtnPageDown->SetEnable(m_nCurPage > 0);
}

VOID SkillBookFrame::OnPageUp( BOOL bPageUp )
{
	INT nMaxPage = m_skillList.Size() / MAX_SKILL_BOOK_NUM;
	if(bPageUp)
	{
		if(m_nCurPage < nMaxPage)
		{
			m_nCurPage++;
		}
	}
	else 
	{
		if(m_nCurPage > 0)
		{
			m_nCurPage--;
		}
	}
	// 刷新界面
	ReflashCurPage();

	SetCurPage(m_nCurPage);
}

VOID SkillBookFrame::SetCurPage( INT nPage )
{
	m_nCurPage = nPage;
	TCHAR szTmp[X_SHORT_NAME]={0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), nPage+1);
	m_pStcPage->SetText(szTmp);
}

DWORD SkillBookFrame::OnWeaponSwap( tagGameEvent* pGameEvent )
{	
	INT i = 0;
	DWORD dwID = GT_INVALID;
	m_skillList.ResetIterator();
	while (m_skillList.PeekNext(dwID))
	{
		if(IsNormalAttackSkill(dwID))
		{
			break;
		}
		++i;
	}
	if(IsCurPage(i))
	{
		m_SkillItem[i%MAX_SKILL_BOOK_NUM].UpdateSkill(dwID);
	}
	return 0;
}

VOID SkillBookFrame::Update()
{
	float deltaTime =  Cool3D::Kernel::Inst()->GetDeltaTime();

	GameFrame::Update();
}