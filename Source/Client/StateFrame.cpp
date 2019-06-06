#include "stdafx.h"
#include "StateFrame.h"
#include "CharacterFrame.h"
#include "ButtonEx.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "RoleEvent.h"
#include "PlayerNameTab.h"
#include "LevelUpData.h"
#include "NetSession.h"
#include "..\WorldDefine\role_att.h"
#include "ChatFrame.h"
#include "CombatSysUtil.h"

StateFrame::StateFrame(void):m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndFather = NULL;
	INT i;
	for (i = 0; i < EDP_End; i++)
	{
		m_pStcAtt[i] = NULL;
		m_pBtnAtt[i] = NULL;
	}
	for (i = 0; i < EDPE_End; i++)
	{
		m_pStcAttEx[i] = NULL;
	}
	for (i = 0; i < EDR_End; i++)
	{
		m_pStcResPic[i] = NULL;
		m_pStcResValue[i] = NULL;
	}
	m_pStcPlayerName = NULL;
	m_pStcPlayerLevel = NULL;
	m_pStcPlayerClass = NULL;
	m_pStcPlayerGuild = NULL;
	m_pStcOverplus = NULL;
	m_pPbnDisplayAtt = NULL;
	m_pPbnDisplayEquip = NULL;
	m_pPbnDisplayFashion = NULL;
    m_pWndState = NULL;

	m_isSwappingSuitPattern = false;

	m_pWndAtt = NULL;
	m_pWndAttEx = NULL;
	m_pWndAttIn = NULL;
	m_pWndRes = NULL;
	m_pBtnSure = NULL;
	m_pBtnCancel = NULL;

	for (i=0; i<EDT_Origin_End; ++i)
	{
		m_Temporary[i] = 0;
	}
	for (i=0; i<X_ERA_ATTA_NUM; ++i)
	{
		m_nOgnAtt[i] = 0;
		m_nPreBid[i] = 0;
		m_nBidden[i] = 0;
	}
	m_nInternalInjury = GT_INVALID;
}

StateFrame::~StateFrame(void)
{
}

BOOL StateFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	// 注册游戏事件处理函数
	m_pMgr->RegisterEventHandle(_T("tagUpdateLPAttEvent"),	  (FRAMEEVENTPROC)m_Trunk.sfp1(&StateFrame::OnGameEvent));
	m_pMgr->RegisterEventHandle(_T("tagUpdateLPBidAttEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&StateFrame::OnGameEvent));
	m_pMgr->RegisterEventHandle(_T("tagUpdateLPLevel"),		  (FRAMEEVENTPROC)m_Trunk.sfp1(&StateFrame::OnGameEvent));
	m_pMgr->RegisterEventHandle(_T("tagRoleGetNameEvent"),	  (FRAMEEVENTPROC)m_Trunk.sfp1(&StateFrame::OnGameEvent));
    m_pMgr->RegisterEventHandle(_T("tagInitLPAttEvent"),	  (FRAMEEVENTPROC)m_Trunk.sfp1(&StateFrame::OnGameEvent));

	return bRet;
}

BOOL StateFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();
	
	// 注销游戏事件处理函数
	m_pMgr->UnRegisterEventHandler(_T("tagUpdateLPAttEvent"),	 (FRAMEEVENTPROC)m_Trunk.sfp1(&StateFrame::OnGameEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagUpdateLPBidAttEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&StateFrame::OnGameEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagUpdateLPLevel"),		 (FRAMEEVENTPROC)m_Trunk.sfp1(&StateFrame::OnGameEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagRoleGetNameEvent"),	 (FRAMEEVENTPROC)m_Trunk.sfp1(&StateFrame::OnGameEvent));
    m_pMgr->UnRegisterEventHandler(_T("tagInitLPAttEvent"),	     (FRAMEEVENTPROC)m_Trunk.sfp1(&StateFrame::OnGameEvent));
	
	return bRet;
}

BOOL StateFrame::ReloadUI()
{
	// 创建
    m_pWndState = m_pGUI->GetWnd(((CharacterFrame*)m_pFather)->GetStateWndFullName().c_str());
	m_pWndFather = m_pGUI->GetWnd(((CharacterFrame*)m_pFather)->GetGUIFullName().c_str());
	m_pWnd = m_pWndFather->GetChild(_T("rstatepic"));
	
	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&StateFrame::EventHandler));

	TCHAR szTmp[X_SHORT_NAME] = {0};
	//人物名称
	{
		m_pStcPlayerName = (GUIStatic*)m_pWnd->GetChild(_T("rname"));
		m_pStcPlayerLevel = (GUIStatic*)m_pWnd->GetChild(_T("rgrad1"));
		m_pStcPlayerClass = (GUIStatic*)m_pWnd->GetChild(_T("rcareer"));
		m_pStcPlayerGuild = (GUIStatic*)m_pWnd->GetChild(_T("rpost"));
		m_pStcPlayerLevel->SetText(_T(""));
	}
	//一级属性和投点
	{
		m_pWndAtt = m_pWnd->GetChild(_T("equipback\\role\\rs_add"));
		
		for (INT i = 0; i < EDP_End; i++)
		{
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("rnum%d"), i+1);
			m_pStcAtt[i] = (GUIStatic*)m_pWndAtt->GetChild(szTmp);
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("ra_butt%d"), i+1);
			m_pBtnAtt[i] = (GUIButton*)m_pWndAtt->GetChild(szTmp);
			m_pStcAtt[i]->SetText(_T(""));
		}

		m_pStcOverplus = (GUIStatic*)m_pWndAtt->GetChild(_T("num7"));
		m_pBtnCancel = (GUIButton*)m_pWndAtt->GetChild(_T("cancle"));
		m_pBtnSure = (GUIButton*)m_pWndAtt->GetChild(_T("sure"));
		m_pBtnSure->SetEnable(FALSE);
		m_pBtnCancel->SetEnable(FALSE);
	}
	//二级属性和固有属性
	{
		m_pWndAttIn = m_pWnd->GetChild(_T("rs_pic1"));
		m_pWndAttEx = m_pWnd->GetChild(_T("rs_pic2"));
		for (INT i = 0; i < EDPE_End; i++)
		{
			if(i <= EDPE_Cultivate0)
			{
				_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("num%d"), i+1);
				m_pStcAttEx[i] = (GUIStatic*)m_pWndAttIn->GetChild(szTmp);
			}
			else
			{
				_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("num%d"), i+1);
				m_pStcAttEx[i] = (GUIStatic*)m_pWndAttEx->GetChild(szTmp);
			}
			m_pStcAttEx[i]->SetText(_T(""));
		}
	}
	//抗性
	{
		m_pWndRes = m_pWnd->GetChild(_T("rs_pic3"));
		for (INT i = 0; i < EDR_End; i++)
		{
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("spic%d"), i+1);
			m_pStcResPic[i] = (GUIStatic*)m_pWndRes->GetChild(szTmp);
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("dnum%d"), i+1);
			m_pStcResValue[i] = (GUIStatic*)m_pWndRes->GetChild(szTmp);
			m_pStcResValue[i]->SetText(_T(""));
			m_pStcResPic[i]->SetInvisible(TRUE);
		}
	}
	//界面显示
	{
		m_pPbnDisplayAtt = (GUIPushButton*)m_pWnd->GetChild(_T("word1\\optnutt1"));
		m_pPbnDisplayAtt->SetState(EGUIBS_PushDown, FALSE);

		// 时装和装备转换
		m_pFmSuitOpt = (GUIFrameMutex*)m_pWnd->GetChild(_T("word1\\ra_opt"));
		m_pPbnDisplayEquip = (GUIPushButton*)m_pWnd->GetChild(_T("word1\\ra_opt\\optbutt2"));
		m_pPbnDisplayFashion = (GUIPushButton*)m_pWnd->GetChild(_T("word1\\ra_opt\\optbutt3"));
	}
	
	return TRUE;
}

VOID StateFrame::Update()
{
	/*! 计算累积时间，让玩家点击换装备/时装模式时有所延迟 */
	static DWORD dwAccumTime = 0;
	if (m_isSwappingSuitPattern)
		{
		dwAccumTime += Kernel::Inst()->GetDeltaTimeDW();
		if (dwAccumTime >= BUTTON_DELAY_MS)
		{
			dwAccumTime = 0;
			m_isSwappingSuitPattern = false;
			m_pPbnDisplayEquip->SetEnable(true);
			m_pPbnDisplayEquip->SetRefresh();
			m_pPbnDisplayFashion->SetEnable(true);
			m_pPbnDisplayFashion->SetRefresh();
		}
	}

    //刷新骑乘速度
    static DWORD dwTimeFirst = 0;
    LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
    if (P_VALID(m_pWndState)
        && !m_pWndState->IsInvisible()
        && P_VALID(pLp)
        && pLp->IsAttributeInited()
        && Kernel::Inst()->GetAccumTimeDW() - dwTimeFirst > 3000)
    {
        if (Kernel::Inst()->GetAccumTimeDW() - dwTimeFirst > 3000)
        {
            UpdateAttribute(ERA_Speed_XZ);
            UpdateAttribute(ERA_Speed_Mount);

            dwTimeFirst = Kernel::Inst()->GetAccumTimeDW();
        }
    }
}
DWORD StateFrame::OnGameEvent(tagGameEvent* pGameEvent)
{
	if (pGameEvent->strEventName == _T("tagUpdateLPAttEvent"))
	{
		tagUpdateLPAttEvent* pEvent = (tagUpdateLPAttEvent*)pGameEvent;
		UpdateAttribute(pEvent->eType);
	}
	else if (pGameEvent->strEventName == _T("tagUpdateLPLevel"))
	{
		tagUpdateLPLevel* pEvent = (tagUpdateLPLevel*)pGameEvent;
		SetPlayerLevel(pEvent->nCurLevel);
		//移除预投点
		RemoveBidPoint();
		//刷新相关属性
		UpdateBidAttribute();
	}
	else if (pGameEvent->strEventName == _T("tagRoleGetNameEvent"))
	{
		tagRoleGetNameEvent* pEvent = (tagRoleGetNameEvent*)pGameEvent;
		if(pEvent->dwRoleID == RoleMgr::Inst()->GetLocalPlayerID())
		{
			SetPlayerName(PlayerNameTab::Inst()->FindNameByID(pEvent->dwRoleID));
		}
	}
	else if (pGameEvent->strEventName == _T("tagUpdateLPBidAttEvent"))
	{
		tagUpdateLPAttEvent* pEvent = (tagUpdateLPAttEvent*)pGameEvent;
		
	}
    else if (pGameEvent->strEventName == _T("tagInitLPAttEvent"))
    {
        // 角色初始装备模式
        LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
        if( P_VALID( pLP ) && pLP->IsAttributeInited() )
        {
            bool bFashion = RoleMgr::Inst()->GetLocalPlayer()->GetDisplaySet().bFashionDisplay;
            if (bFashion)
                m_pPbnDisplayFashion->SetState(EGUIBS_PushDown, FALSE);
            else
                m_pPbnDisplayEquip->SetState(EGUIBS_PushDown, FALSE);
        }
    }
	return 0;
}

DWORD StateFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnSure)//确定
			{
				
				//发送投点命令 
				SendNetBidAtt();
				//移除预投点
				RemoveBidPoint();
			}
			else if (pWnd == m_pBtnCancel)//撤销
			{
				//移除预投点
				RemoveBidPoint();
				//刷新相关属性
				UpdateBidAttribute();
			} 
			else
			{
				for (INT i = 0; i < EDP_End; i++)
				{
					if (pWnd == m_pBtnAtt[i])
					{
						//预投点
						if(m_nOverplus > 0)
						{
							AddPreBidPoint(TransBtn2Att((EDisplayProperty)i), 1);
							SetPlayerOverplus(m_nOverplus - 1);
						}
						break;
					}
				}
			}
		}
		break;
	case EGUIE_PushButtonDown:
		{
			if (pWnd == m_pPbnDisplayAtt)
			{
				//投点面板
				m_pWndAtt->SetInvisible(pEvent->dwParam1 == 2);

			}
			else if(pWnd == m_pPbnDisplayEquip)
			{
				m_isSwappingSuitPattern = true;
				m_pPbnDisplayEquip->SetEnable(false);
				m_pPbnDisplayEquip->SetRefresh();
				m_pPbnDisplayFashion->SetEnable(false);
				m_pPbnDisplayFashion->SetRefresh();
				/*! 发送设置装备模式游戏事件 */
				tagFashionEvent e(_T("FashionEvent"), this);
				e.bDisFashion = false;
				m_pFrameMgr->SendEvent(&e);
			}
			else if(pWnd == m_pPbnDisplayFashion)
			{
				m_isSwappingSuitPattern = true;
				m_pPbnDisplayEquip->SetEnable(false);
				m_pPbnDisplayEquip->SetRefresh();
				m_pPbnDisplayFashion->SetEnable(false);
				m_pPbnDisplayFashion->SetRefresh();
				/*! 发送设置时装模式游戏事件 */
				tagFashionEvent e(_T("FashionEvent"), this);
				e.bDisFashion = true;
				m_pFrameMgr->SendEvent(&e);
			}
		}
	}
	return 0;
}

VOID StateFrame::UpdatePalyerAtt()
{
	LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
	if(P_VALID(pLp))
	{
		SetPlayerName(PlayerNameTab::Inst()->FindNameByID(pLp->GetID()));
		SetPlayerLevel(pLp->GetRoleLevel());
		//移除预投点
		RemoveBidPoint();
		//刷新相关属性
		UpdateBidAttribute();
	}
}

VOID StateFrame::SetPlayerName(LPCTSTR szName)
{
	m_pStcPlayerName->SetText(szName);
}

VOID StateFrame::SetPlayerLevel(INT nLevel)
{
	TCHAR szTmp[X_SHORT_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("Role_Level")], nLevel);
	m_pStcPlayerLevel->SetText(szTmp);
	m_pStcPlayerLevel->SetRefresh(TRUE);

	// 等级改变，相应自身成长值也要发生变化
	const tagExperience* pExp = LevelUpData::Inst()->FindExpData(nLevel);
	if(P_VALID(pExp))
	{
		for(INT i=0; i<X_ERA_ATTA_NUM; ++i)
		{
			m_nOgnAtt[i] = pExp->nAtt[i];
		}
	}
}

VOID StateFrame::SetPlayerClass(LPCTSTR szClass)
{
	m_pStcPlayerLevel->SetText(szClass);
}

VOID StateFrame::SetPlayerGuild(LPCTSTR szGuild)
{
	m_pStcPlayerGuild->SetText(szGuild);
}

VOID StateFrame::SetPlayerOverplus(INT nValue)
{
	TCHAR szTmp[X_SHORT_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), nValue);
	m_pStcOverplus->SetText(szTmp);
	m_pStcOverplus->SetRefresh(TRUE);
	
	if(0 != m_nOverplus && 0 == nValue)
	{
		for(INT i=0; i<X_ERA_ATTA_NUM; ++i)
		{
			m_pBtnAtt[i]->SetEnable(FALSE);
		}
	}
	else if(0 == m_nOverplus && 0 != nValue)
	{
		for(INT i=0; i<X_ERA_ATTA_NUM; ++i)
		{
			m_pBtnAtt[i]->SetEnable(TRUE);
		}
	}
	m_nOverplus = nValue;
}

VOID StateFrame::SetPlayerAtt(EDisplayProperty eType, INT nValue, DWORD dwColor /* = ORDINARY_COLOR */)
{
	TCHAR szTmp[X_SHORT_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), nValue);
	m_pStcAtt[eType]->SetText(szTmp);
	m_pStcAtt[eType]->SetTextColor(dwColor);
	m_pStcAtt[eType]->SetRefresh(TRUE);
}

VOID StateFrame::SetPlayerAttEx(EDisplayPropertyEx eType, INT nValue, DWORD dwColor /* = ORDINARY_COLOR */)
{
	TCHAR szTmp[X_SHORT_NAME] = {0};
	switch(eType)
	{
	case EDPE_ArmDefine0:		// 护甲防御修正值
	case EDPE_MoveSpeed0:		// 移动速度修正值
	case EDPE_Luck0:			// 福缘修正值
	case EDPE_Feature0:			// 仪容修正值
	case EDPE_Command0:			// 统御修正值
	case EDPE_Morality0:		// 道德修正值
	case EDPE_Cultivate0:		// 修为修正值
	case EDPE_MeleeDamage0:		// 外功攻击修正值
	case EDPE_MeleeDefine0:		// 外功防御修正值
	case EDPE_MagicDamage0:		// 内功攻击修正值
	case EDPE_MagicDefine0:		// 内功防御修正值
	case EDPE_DamageSkill0:		// 攻击技巧修正值
	case EDPE_DefineSkill0:		// 防御技巧修正值
	case EDPE_Hit0:				// 命中修正值
	case EDPE_Dodge0:			// 躲闪修正值
		{
			if(MINUS_RED_COLOR == dwColor)
				_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), nValue);
			else if(PLUS_GREEN_COLOR == dwColor)
				_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("+%d"), nValue);
				
		}
		break;
	case EDPE_Energy0:			// 活力最大值
	case EDPE_Health0:			// 体力最大值
	case EDPE_Power0:			// 真气最大值
	case EDPE_Toughness0:		// 持久力最大值
		{
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("/%d"), nValue);
		}
		break;
	case EDPE_WeaponDamage0:
		{
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("-%d"), nValue);
		}
		break;
	case EDPE_MoveSpeed:
		{
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d%%"), nValue/100);
		}
		break;
	default:
		{
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), nValue);
		}
		break;
	}
	m_pStcAttEx[eType]->SetText(szTmp);
	m_pStcAttEx[eType]->SetTextColor(dwColor);
	m_pStcAttEx[eType]->SetRefresh(TRUE);
}

VOID StateFrame::SetPlayerResist(EDisplayResist eType, INT nValue)
{
	TCHAR szTmp[X_SHORT_NAME] = {0};
	if(nValue >= 0)
		_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("+%d"), abs(nValue));
	else
		_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("-%d"), abs(nValue));

	m_pStcResValue[eType]->SetText(szTmp);
	m_pStcResPic[eType]->SetInvisible(FALSE);
	m_pStcResPic[eType]->SetRefresh(TRUE);
}

VOID StateFrame::UpdateAttribute(ERoleAttribute eType)
{
	INT nValue1,nValue2,nValue3;
	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	nValue1 = pLP->GetAttribute(eType);
	DWORD dwColor = ORDINARY_COLOR;

	switch(eType)
	{
	case ERA_Physique:			// 当前筋骨
		{
			// 当前筋骨的显示需要加上当前的预投点值，用于界面显示效果
			nValue1 = nValue1 + m_nPreBid[eType];
			SetPlayerAtt(EDP_Physique, nValue1);

			// 原始体力=当前筋骨×10
			m_Temporary[EDT_Origin_Health] = (INT)(nValue1 * 10);
			if(m_nPreBid[eType] > 0)
			{
				nValue3 = pLP->GetAttribute(ERA_MaxHP) + (INT)(m_nPreBid[eType] * 10);
				SetPlayerAttEx(EDPE_Health0, nValue3);
			}
			
			// 当前劲力的显示需要加上当前的预投点值，用于界面显示效果 
			nValue2 = pLP->GetAttribute(ERA_Strength) + m_nPreBid[ERA_Strength];
			// 原始外攻防御=当前筋骨×5+当前劲力×1
			m_Temporary[EDT_Origin_ExDefine] = (INT)(nValue1 * 5 + nValue2 * 1); 
			SetPlayerAttEx(EDPE_MeleeDefine, m_Temporary[EDT_Origin_ExDefine]);
		}
		break;
	case ERA_Strength:			// 当前劲力
		{
			// 当前劲力的显示需要加上当前的预投点值，用于界面显示效果
			nValue1 = nValue1 + m_nPreBid[eType];
			SetPlayerAtt(EDP_Strength, nValue1);

			// 原始外攻攻击=当前劲力×5
			m_Temporary[EDT_Origin_ExDamge] = (INT)(nValue1 * 5); 
			SetPlayerAttEx(EDPE_MeleeDamage, m_Temporary[EDT_Origin_ExDamge]);
			
			
			nValue2 = pLP->GetAttribute(ERA_Physique) + m_nPreBid[ERA_Physique];
			// 原始外攻防御=当前筋骨×5+当前劲力×1
			m_Temporary[EDT_Origin_ExDefine] = (INT)(nValue2 * 5 + nValue1 * 1); 
			SetPlayerAttEx(EDPE_MeleeDefine, m_Temporary[EDT_Origin_ExDefine]);
		}
		break;
	case ERA_Pneuma:			// 当前元气
		{
			nValue1 = nValue1 + m_nPreBid[eType];
			SetPlayerAtt(EDP_Pneuma, nValue1);

			// 原始真气=当前元气×10
			m_Temporary[EDT_Origin_Power] = (INT)(nValue1 * 10);
			if(m_nPreBid[eType] > 0)
			{
				nValue3 = pLP->GetAttribute(ERA_MaxMP) + (INT)(m_nPreBid[eType] * 10);
				SetPlayerAttEx(EDPE_Power0, nValue3);
			}
			
			// 原始内攻防御=当前元气×5+当前内力×1
			nValue2 = pLP->GetAttribute(ERA_InnerForce) + m_nPreBid[ERA_InnerForce];
			m_Temporary[EDT_Origin_InDefine] = (INT)(nValue1 * 5 + nValue2 * 1);
			SetPlayerAttEx(EDPE_MagicDefine, m_Temporary[EDT_Origin_InDefine]);
		}
		break;
	case ERA_InnerForce:		// 当前内力
		{
			nValue1 = nValue1 + m_nPreBid[eType];
			SetPlayerAtt(EDP_InnerForce, nValue1);

			// 原始内攻攻击=当前内力×5
			m_Temporary[EDT_Origin_InDamage] = (INT)(nValue1 * 5);
			SetPlayerAttEx(EDPE_MagicDamage, m_Temporary[EDT_Origin_InDamage]);
			
			// 原始内攻防御=当前元气×5+当前内力×1
			nValue2 = pLP->GetAttribute(ERA_Pneuma) + m_nPreBid[ERA_Pneuma];
			m_Temporary[EDT_Origin_InDefine] = (INT)(nValue2 * 5 + nValue1 * 1);
			SetPlayerAttEx(EDPE_MagicDefine, m_Temporary[EDT_Origin_InDefine]);
		}
		break;
	case ERA_Technique:			// 当前技力
		{
			nValue1 = nValue1 + m_nPreBid[eType];
			SetPlayerAtt(EDP_Technique, nValue1);

			// 原始攻击技巧=当前技力×6
			m_Temporary[EDT_Origin_DmgSkill] = (INT)(nValue1 * 6); 
			SetPlayerAttEx(EDPE_DamageSkill, m_Temporary[EDT_Origin_DmgSkill]);

			// 原始命中=当前技力×10
			m_Temporary[EDT_Origin_Hit] = (INT)(nValue1 * 10);
			SetPlayerAttEx(EDPE_Hit, m_Temporary[EDT_Origin_Hit]);

			// 原始持久力=[98]+技力当前值×0.1+身法当前值×0.1
		}
		break;
	case ERA_Agility:			// 当前身法
		{
			nValue1 = nValue1 + m_nPreBid[eType];
			SetPlayerAtt(EDP_Agility, nValue1);

			// 原始防御技巧=当前身法×4
			m_Temporary[EDT_Origin_DefSkill] = (INT)(nValue1 * 4);
			SetPlayerAttEx(EDPE_DefineSkill, m_Temporary[EDT_Origin_DefSkill]);
			
			// 原始闪避=当前身法×10
			m_Temporary[EDT_Origin_Dodge] = (INT)(nValue1 * 10);
			SetPlayerAttEx(EDPE_Dodge, m_Temporary[EDT_Origin_Dodge]);

			// 原始持久力=[98]+技力当前值×0.1+身法当前值×0.1
		}
		break;
	case ERA_AttPoint:			// 当前属性点数
		{
			SetPlayerOverplus(nValue1);
		}
		break;
	case ERA_MaxHP:				// 最大体力
		{
			nValue2 = nValue1 - m_Temporary[EDT_Origin_Health];
			SetPlayerAttEx(EDPE_Health0, nValue1, GETTRUECOLOR(nValue2));
		}
		break;
	case ERA_HP:				// 当前体力
		{
			SetPlayerAttEx(EDPE_Health, nValue1);
		}
		break;
	case ERA_MaxMP:				// 最大真气
		{
			nValue2 = nValue1 - m_Temporary[EDT_Origin_Power];
			SetPlayerAttEx(EDPE_Power0, nValue1, GETTRUECOLOR(nValue2));
		}
		break;
	case ERA_MP:				// 当前真气
		{
			SetPlayerAttEx(EDPE_Power, nValue1);
		}
		break;
	case ERA_ExAttack:			// 外功攻击
		{
			nValue2 = nValue1 - m_Temporary[EDT_Origin_ExDamge];
			SetPlayerAttEx(EDPE_MeleeDamage0, nValue2, GETTRUECOLOR(nValue2));
		}
		break;
	case ERA_ExDefense:			// 外功防御
		{
			nValue2 = nValue1 - m_Temporary[EDT_Origin_ExDefine];
			SetPlayerAttEx(EDPE_MeleeDefine0, nValue2, GETTRUECOLOR(nValue2));
		}
		break;
	case ERA_InAttack:			// 内功攻击
		{
			nValue2 = nValue1 - m_Temporary[EDT_Origin_InDamage];
			SetPlayerAttEx(EDPE_MagicDamage0, nValue2, GETTRUECOLOR(nValue2));
		}
		break;
	case ERA_InDefense:			// 内功防御
		{
			nValue2 = nValue1 - m_Temporary[EDT_Origin_InDefine];
			SetPlayerAttEx(EDPE_MagicDefine0, nValue2, GETTRUECOLOR(nValue2));
		}
		break;
	case ERA_AttackTec:			// 攻击技巧
		{
			nValue2 = nValue1 - m_Temporary[EDT_Origin_DmgSkill];
			SetPlayerAttEx(EDPE_DamageSkill0, nValue2, GETTRUECOLOR(nValue2));
		}
		break;
	case ERA_DefenseTec:		// 防御技巧
		{
			nValue2 = nValue1 - m_Temporary[EDT_Origin_DefSkill];
			SetPlayerAttEx(EDPE_DefineSkill0, nValue2, GETTRUECOLOR(nValue2));
		}
		break;
	case ERA_HitRate:			// 命中率
		{
			nValue2 = nValue1 - m_Temporary[EDT_Origin_Hit];
			SetPlayerAttEx(EDPE_Hit0, nValue2, GETTRUECOLOR(nValue2));
		}
		break;
	case ERA_Dodge:				// 闪避率
		{
			nValue2 = nValue1 - m_Temporary[EDT_Origin_Dodge];
			SetPlayerAttEx(EDPE_Dodge0, nValue2, GETTRUECOLOR(nValue2));
		}
		break;
	case ERA_Endurance:			// 持久力
		{
			SetPlayerAttEx(EDPE_Toughness, nValue1);
			//nValue2 = nValue1 - m_Temporary[EDT_Origin_Toughness];
			//SetPlayerAttEx(EDPE_Toughness0, nValue2, GETTRUECOLOR(nValue2));
		}
		break;
	case ERA_MaxEndurance:
		{
			SetPlayerAttEx(EDPE_Toughness0, nValue1);
		}
		break;
	case ERA_Vitality:			// 活力
		{
			SetPlayerAttEx(EDPE_Energy, nValue1);
		}
		break;
	case ERA_MaxVitality:		// 最大活力
		{
			SetPlayerAttEx(EDPE_Energy0, nValue1);
		}
		break;
	case ERA_Speed_XZ:			// XZ平面速度速度
		{
            // 是否是骑乘状态
            LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
            if (P_VALID(pLp) && !P_VALID(pLp->GetMount()))
                SetPlayerAttEx(EDPE_MoveSpeed, nValue1);
		}
		break;
    case ERA_Speed_Mount:
        {
            // 是否是骑乘状态
            LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
            if (P_VALID(pLp) && P_VALID(pLp->GetMount()))
                SetPlayerAttEx(EDPE_MoveSpeed, nValue1);
        }
        break;
	case ERA_WeaponDmgMin:		// 最小武器攻击
		{
			SetPlayerAttEx(EDPE_WeaponDamage, nValue1);
		}
		break;
	case ERA_WeaponDmgMax:		// 最大武器攻击
		{
			SetPlayerAttEx(EDPE_WeaponDamage0, nValue1);
		}
		break;
	case ERA_WeaponSoul:		// 武魂
		{
			SetPlayerAttEx(EDPE_WuHun, nValue1);
		}
		break;
	case ERA_Armor:				// 防具护甲
		{
			SetPlayerAttEx(EDPE_ArmDefine, nValue1);
		}
		break;
	case ERA_Derate_Ordinary:	// 普通伤害减免
		{
            SetPlayerResist(EDR_OrdinaryResist, CalExAttackResist(nValue1));
		}
		break;
	case ERA_Derate_Bleeding:	// 出血伤害减免
		{
			SetPlayerResist(EDR_BleedResist, CalExAttackResist(nValue1));
		}
		break;
	case ERA_Derate_Brunt:		// 冲击伤害减免
		{
			SetPlayerResist(EDR_BruntResist, CalExAttackResist(nValue1));
		}
		break;
	case ERA_Derate_Bang:		// 重击伤害减免
		{
			SetPlayerResist(EDR_BangResist, CalExAttackResist(nValue1));
		}
		break;
	case ERA_Derate_Poison:		// 毒性伤害减免
		{
			SetPlayerResist(EDR_PoisonResist, CalInAttackResist(nValue1));
		}
		break;
	case ERA_Derate_Thinker:	// 心智伤害减免
		{
			SetPlayerResist(EDR_ThinkerResist, CalInAttackResist(nValue1));
		}
		break;
	case ERA_Derate_Injury:		// 内损伤害减免
		{
			SetPlayerResist(EDR_InjuryResist, CalInAttackResist(nValue1));
		}
		break;
	case ERA_Derate_Stunt:		// 绝技伤害减免
		{
		}
		break;
	case ERA_Derate_ExAttack:	// 外功伤害减免
		{
            UpdateAttribute(ERA_Derate_Ordinary);
            UpdateAttribute(ERA_Derate_Bleeding);
            UpdateAttribute(ERA_Derate_Brunt);
            UpdateAttribute(ERA_Derate_Bang);
		}
		break;
	case ERA_Derate_InAttack:	// 内功伤害减免
		{
            UpdateAttribute(ERA_Derate_Poison);
            UpdateAttribute(ERA_Derate_Thinker);
            UpdateAttribute(ERA_Derate_Injury);
		}
		break;
	case ERA_Derate_ALL:		// 所有伤害减免
		{
            UpdateAttribute(ERA_Derate_Ordinary);
            UpdateAttribute(ERA_Derate_Bleeding);
            UpdateAttribute(ERA_Derate_Brunt);
            UpdateAttribute(ERA_Derate_Bang);

            UpdateAttribute(ERA_Derate_Poison);
            UpdateAttribute(ERA_Derate_Thinker);
            UpdateAttribute(ERA_Derate_Injury);
		}
		break;
	case ERA_Morale:			// 士气
		{
			SetPlayerAttEx(EDPE_Morale, nValue1);
		}
		break;
	case ERA_Injury:			// 内伤
		{
			SetPlayerAttEx(EDPE_InternalInjury, nValue1, MINUS_RED_COLOR);

			//初始化时如果内伤为0则不提示
			if( nValue1!=m_nInternalInjury && 0==nValue1 )
			{
				m_nInternalInjury = nValue1;
				return;
			}

			if( nValue1 == 0 )
			{
				ChatFrame* pFrame = (ChatFrame*)m_pMgr->GetFrame(_T("Chat"));
				if( P_VALID(pFrame) )
				{
					pFrame->PushInfo( g_StrTable[_T("RoleChat_Injury1")], ESCC_System );
				}
				CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("RoleChat_Injury3")]);
			}
			else if( nValue1 > 0  )
			{
				ChatFrame* pFrame = (ChatFrame*)m_pMgr->GetFrame(_T("Chat"));
				if( P_VALID(pFrame) )
				{
					pFrame->PushInfo( g_StrTable[_T("RoleChat_Injury0")], ESCC_System );
				}
				CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("RoleChat_Injury2")]);

				LocalPlayer *pLp = RoleMgr::Inst()->GetLocalPlayer();
				if( P_VALID(pLp) )
				{
					int nvalue = pLp->GetAttribute( ERA_HP );
					if( nvalue == 0 || pLp->IsDead() )
					{
						TCHAR szInjury[X_LONG_NAME] = {0};
						_sntprintf( szInjury, X_LONG_NAME, g_StrTable[_T("Dead_Injury")], nValue1 );
						CombatSysUtil::Inst()->ShowScreenCenterMsgEx(szInjury);
					}
				}
			}
		}
		break;
	case ERA_Fortune:			// 福缘
		{
			SetPlayerAttEx(EDPE_Luck, nValue1);
		}
		break;
	case ERA_Appearance:		// 仪容
		{
			SetPlayerAttEx(EDPE_Feature, nValue1);
		}
		break;
	case ERA_Rein:				// 统御
		{
			SetPlayerAttEx(EDPE_Command, nValue1);
		}
		break;
	case ERA_Knowledge:			// 阅历
		{

		}
		break;
	case ERA_Morality:			// 道德
		{
			SetPlayerAttEx(EDPE_Morality, nValue1);
		}
		break;
	case ERA_Culture:			// 修为
		{
			SetPlayerAttEx(EDPE_Cultivate, nValue1);
		}
		break;
	default:
		break;
	}
}

VOID StateFrame::AddPreBidPoint( ERoleAttribute eType, INT nInc )
{
	ASSERT(eType >= ERA_AttA_Start && eType <= ERA_AttA_End);

	m_nPreBid[eType] += nInc;
	UpdateAttribute(eType);
	
	m_pBtnSure->SetEnable(TRUE);
	m_pBtnCancel->SetEnable(TRUE);
}

VOID StateFrame::RemoveBidPoint()
{
	for(INT i=0; i<X_ERA_ATTA_NUM; ++i)
	{
		m_nPreBid[i] = 0;
	}

	m_pBtnSure->SetEnable(FALSE);
	m_pBtnCancel->SetEnable(FALSE);
}

VOID StateFrame::SendNetBidAtt()
{
	tagNC_RoleBidAttPoint cmd;
	for(INT i=0; i<X_ERA_ATTA_NUM; ++i)
	{
		cmd.nAttPointAdd[i] = m_nPreBid[i];
	}
	m_pSession->Send(&cmd);
}

ERoleAttribute StateFrame::TransBtn2Att( EDisplayProperty eType )
{
	switch (eType)
	{
	case EDP_Physique:
		return ERA_Physique;
	case EDP_Strength:
		return ERA_Strength;
	case EDP_InnerForce:
		return ERA_InnerForce;
	case EDP_Pneuma:
		return ERA_Pneuma;
	case EDP_Agility:
		return ERA_Agility;
	case EDP_Technique:
		return ERA_Technique;
	}
	return ERA_Null;
}

VOID StateFrame::SetBiddenPoint( ERoleAttribute eType, INT nValue )
{
	ASSERT(eType >= ERA_AttA_Start && eType <= ERA_AttA_End);

	m_nPreBid[eType] = nValue;

	// 其他计算
}

VOID StateFrame::UpdateBidAttribute()
{
	for(INT i=0; i<X_ERA_ATTA_NUM; ++i)
	{
		UpdateAttribute((ERoleAttribute)i);
	}
    for (int i = 0; i < X_ERA_DERATE_NUM; ++i)
    {
        UpdateAttribute((ERoleAttribute)(ERA_Derate_Start + i));
    }
	UpdateAttribute(ERA_MaxHP);
	UpdateAttribute(ERA_MaxMP);
	UpdateAttribute(ERA_AttPoint);
}

int StateFrame::CalExAttackResist( int src )
{
    LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
    if (!P_VALID(pLP))
        return 0;

    int nRetValue = src
        + pLP->GetAttribute(ERA_Derate_ALL)
        + pLP->GetAttribute(ERA_Derate_ExAttack);

    if (nRetValue > MAX_PLAYER_RESIST)
        nRetValue = MAX_PLAYER_RESIST;
    else if (nRetValue < MIN_PLAYER_RESIST)
        nRetValue = MIN_PLAYER_RESIST;

    return nRetValue;
}

int StateFrame::CalInAttackResist( int src )
{
    LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
    if (!P_VALID(pLP))
        return 0;

    int nRetValue = src
        + pLP->GetAttribute(ERA_Derate_ALL)
        + pLP->GetAttribute(ERA_Derate_InAttack);

    if (nRetValue > MAX_PLAYER_RESIST)
        nRetValue = MAX_PLAYER_RESIST;
    else if (nRetValue < MIN_PLAYER_RESIST)
        nRetValue = MIN_PLAYER_RESIST;

    return nRetValue;
}