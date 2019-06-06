#include "stdafx.h"
#include "SpecialSkillFrame.h"
#include "ShortcutButton.h"
#include "RoleEvent.h"
#include "Role.h"
#include "NPC.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "SkillProtoData.h"
#include "SkillMgr.h"
#include "WorldPickerFrame.h"
#include "CombatSys.h"
#include "CombatSysUtil.h"
#include "ToolTipFrame.h"
#include "ToolTipCreator.h"
#include "..\WorldDefine\msg_combat.h"

TCHAR*	SpecialSkillFrame::m_szAccArray[MAX_SPECIAL_BUTTON_NUM] = {_T("c.+1"), _T("c.+2"), _T("c.+3"), _T("c.+4"), _T("c.+5"), _T("c.+6"), _T("c.+7"), _T("c.+8"),};
int		SpecialSkillFrame::m_nAccArray[MAX_SPECIAL_BUTTON_NUM] = { ((DIK_1<<16)+DIK_LCONTROL), ((DIK_2<<16)+DIK_LCONTROL), ((DIK_3<<16)+DIK_LCONTROL), ((DIK_4<<16)+DIK_LCONTROL), ((DIK_5<<16)+DIK_LCONTROL), ((DIK_6<<16)+DIK_LCONTROL), ((DIK_7<<16)+DIK_LCONTROL), ((DIK_8<<16)+DIK_LCONTROL),};

SpecialSkillFrame::SpecialSkillFrame( void ):m_Trunk(this)
{
	m_pWnd				= NULL;
	for( int i=0; i<MAX_SPECIAL_BUTTON_NUM; ++i )
	{
		m_pBtn_Skill[i]	= NULL;
	}
	m_bActive			= TRUE;
	m_nCurButton		= 0;
}

SpecialSkillFrame::~SpecialSkillFrame( void )
{

}

BOOL SpecialSkillFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	//--注册游戏事件
	m_pMgr->RegisterEventHandle(_T("tagRoleBuffChangeEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&SpecialSkillFrame::OnRoleBuffChangeEvent));
	m_pMgr->RegisterEventHandle(_T("tagRolePickEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&SpecialSkillFrame::OnRolePickEvent));

	//--注册网络消息
	m_pCmdMgr->Register("NS_Skill",		(NETMSGPROC)m_Trunk.sfp2(&SpecialSkillFrame::OnNS_Skill), _T("NS_Skill"));

	TCHAR szBuff[X_SHORT_NAME] = {0};
	for(int i=0; i<MAX_SPECIAL_BUTTON_NUM; i++)
	{
		_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("HotKey_ContinueSkill")], i+1 );
		m_pKeyMap->Register(m_pUtil->Crc32(m_szAccArray[i]), szBuff,	
			(INPUTMAPEVENTHANDLE)m_Trunk.sfp2(&SpecialSkillFrame::OnKey), m_nAccArray[i], TRUE, FALSE, 2, 
			(INPUTMAPCHANGEEVENTHANDLE)m_Trunk.sfp3(&SpecialSkillFrame::OnKeyChange));
	}

	return TRUE;
}

BOOL SpecialSkillFrame::Destroy()
{
	BOOL ret = GameFrame::Destroy();

	//--注销游戏事件
	m_pMgr->UnRegisterEventHandler(_T("tagRoleBuffChangeEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&SpecialSkillFrame::OnRoleBuffChangeEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagRolePickEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&SpecialSkillFrame::OnRolePickEvent));

	//--注销网络消息
	m_pCmdMgr->UnRegister("NS_Skill",		(NETMSGPROC)m_Trunk.sfp2(&SpecialSkillFrame::OnNS_Skill));


	if( P_VALID(m_pWnd) )
		m_pGUI->DestroyWnd(m_pWnd);

	TCHAR szBuff[X_SHORT_NAME] = {0};
	for(int i=0; i<MAX_SPECIAL_BUTTON_NUM; i++)
	{
		_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("HotKey_ContinueSkill")], i+1 );
		m_pKeyMap->SetEnable(szBuff, false);
	}

	return ret;
}

BOOL SpecialSkillFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath +  _T("\\ui\\conti_skill.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());

	for(int i=0; i<MAX_SPECIAL_BUTTON_NUM; ++i)						
	{
		char temp[256] = {0};
		sprintf(temp, "conti_skillwin\\conti_skillback\\skill%d", i);
		BOOL m = m_pGUI->ChangeXml(&element, temp, "ClassName", "SpecialSkillButton");		
		m_pGUI->ChangeXml(&element, temp, "DisableTextColor", "0xFFFFFFFF");
		m_pGUI->ChangeXml(&element, temp, "DisablePicColor", "0xFF5B5B5B");
		m_pGUI->ChangeXml(&element, temp, "PushTextColor", "0xFFFFF2A4");
		m_pGUI->ChangeXml(&element, temp, "MoveTextColor", "0xFFFFF2A4");
	}

	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pWnd->SetInvisible(TRUE);
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&SpecialSkillFrame::EventHandler));

	m_pCancel = (GUIButton*)m_pWnd->GetChild(_T("closebutt"));
	for(int i=0; i<MAX_SPECIAL_BUTTON_NUM; ++i)			
	{
		TCHAR szBuff[X_LONG_NAME] = {0};
		_sntprintf( szBuff, X_LONG_NAME, _T("conti_skillback\\skill%d"), i );
		m_pBtn_Skill[i] = (SpecialSkillButton*)m_pWnd->GetChild(szBuff);
		m_pBtn_Skill[i]->SetKeyBar(m_szAccArray[i]);			//设置快捷键值
		m_pBtn_Skill[i]->SetDIKBar(m_pUtil->Crc32(m_szAccArray[i]));
	}	

	return TRUE;
}

VOID SpecialSkillFrame::Update()
{

}

DWORD SpecialSkillFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;
	switch(pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pCancel )
			{
				m_pWnd->SetInvisible(TRUE);
			}
		}
		break;
	case EGUISCB_LeftMouseDown:					
	case EGUISCB_RightMouseDown:
		{
			if( _tcscmp(pWnd->GetClassName().c_str(), _T("SpecialSkillButton")) == 0 )
			{
				ShortcutButton* pBtn = (ShortcutButton*)m_pGUI->GetWnd(pEvent->dwWndID);
				if( !P_VALID(pBtn) )
					return FALSE;
				const BtnData btnData = pBtn->GetBtnData();

				if(pBtn->m_bCanUse && btnData.GetType()==ESBT_Skill)
				{
					pBtn->BeginPressBtn();
					pBtn->InvokeGameEvent();
				}
				else if( btnData.GetType()==ESBT_Skill && !pBtn->m_bCanUse )
				{
					pBtn->BeginPressBtn();
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillSvrErr5")]);
				}
			}
		}
	case EGUIE_MoveIntoCtrl:
		{
			if( _tcscmp(pWnd->GetClassName().c_str(), _T("SpecialSkillButton")) == 0 )
			{
				ShortcutButton* pBtn = (ShortcutButton*)m_pGUI->GetWnd(pEvent->dwWndID);
				if( !P_VALID(pBtn) )
					return FALSE;
				const BtnData btnData = pBtn->GetBtnData();
				switch(btnData.GetType())
				{
				case ESBT_Skill:
					{
						const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillDataByTypeID(pEvent->dwParam2);
						if( !P_VALID(pSkill) )
							return FALSE;

						ShowTip(pWnd, ToolTipCreator::Inst()->GetSkillTips(pEvent->dwParam2).c_str());
					}
					break;
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(pWnd, NULL);
		}
		break;
	}
	return 0;
}

VOID SpecialSkillFrame::OnEvent( tagGameEvent* pEvent )
{
	
}

DWORD SpecialSkillFrame::OnRolePickEvent(tagRolePickEvent* pEvent)
{
	//没激活时不显示
	if( !m_bActive )
	{
		if( m_pWnd->IsInvisible()==FALSE )
			m_pWnd->SetInvisible(TRUE);
		return 0;
	}

	Role* pPickRole = NULL;
	pPickRole = RoleMgr::Inst()->FindRole( pEvent->dwRoleID );
	if( RoleMgr::Inst()->GetLocalPlayerID() != pEvent->dwRoleID )
	{
		RemoveAllSpecialSkill();

		SetSelfSpecialSkill();	

		CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
		if( !P_VALID( pCombatSys ) )
			return 0;

		SetOtherSpecialSkill( pCombatSys->GetCurTargetID() );

		if( m_nCurButton == 0 )
			m_pWnd->SetInvisible(TRUE);
		else 
		{
			m_pWnd->SetInvisible(FALSE);
			m_pWnd->SetRefresh(TRUE);
		}
	}

	return 0;
}

DWORD SpecialSkillFrame::OnRoleBuffChangeEvent( tagRoleBuffChangeEvent* pEvent )
{
	//没激活时不显示
	if( !m_bActive )
	{
		if( m_pWnd->IsInvisible()==FALSE )
			m_pWnd->SetInvisible(TRUE);
		return 0;
	}
	if( !P_VALID( m_pWnd ) )
		return 0;

	//自已buff有变化
	if( pEvent->dwRoleID == RoleMgr::Inst()->GetLocalPlayerID() )
	{
		RemoveAllSpecialSkill();

		SetSelfSpecialSkill();	

		CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
		if( !P_VALID( pCombatSys ) )
			return 0;		
		SetOtherSpecialSkill( pCombatSys->GetCurTargetID() );

	}
	//他人buff有变化,且为当前目标
	else 
	{
		CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame(_T("CombatSys"));
		if( !P_VALID( pCombatSys ) )
			return 0;

		if( pCombatSys->GetCurTargetID() == pEvent->dwRoleID )
		{
			RemoveAllSpecialSkill();
			SetSelfSpecialSkill();	
			SetOtherSpecialSkill( pEvent->dwRoleID );
		}
	}

	if( m_nCurButton == 0 )
		m_pWnd->SetInvisible(TRUE);
	else 
	{
		m_pWnd->SetInvisible(FALSE);
		m_pWnd->SetRefresh(TRUE);
	}

	return 0;
}

DWORD SpecialSkillFrame::OnNS_Skill(tagNS_Skill* pMsg, DWORD dwParam)
{
	if(pMsg->dwErrorCode!=E_Success)
		return 0; 

	if(pMsg->dwSrcRoleID==RoleMgr::Inst()->GetLocalPlayerID())
	{
		if( IsHaveSpecialSkill(pMsg->dwSkillID) )
			m_pWnd->SetRefresh(TRUE);
	}

	return 0;
}

void SpecialSkillFrame::RemoveAllSpecialSkill()
{
	m_nCurButton = 0;
	for( int i=0; i<MAX_SPECIAL_BUTTON_NUM; ++i )
	{
		m_pBtn_Skill[i]->SetBtnData( ESBT_Null, 0, 0, 0 );
		m_pBtn_Skill[i]->ClearPic();
		m_pBtn_Skill[i]->SetInvisible(TRUE);
	}
}

void SpecialSkillFrame::SetSelfSpecialSkill( )
{
	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	if( P_VALID( pLP ) )
	{
		TList<tagRoleBuff*>& buffList = pLP->GetBuffList();
		tagRoleBuff* pBuff = NULL;
		buffList.ResetIterator();
		while( buffList.PeekNext(pBuff) )
		{
			//连接技存在,且是自身的连接技
			const tagContinuteSkill* pCSkill = SkillProtoData::Inst()->FindContinuteSkillProto(pBuff->dwBuffTypeID/100);
			if( P_VALID(pCSkill) && pCSkill->nTarget==0 )
			{
				for( int i=0; i<MAX_CONTINUE_SKILL_NUM; ++i )
				{
					if( pCSkill->dwSkill[i]!=GT_INVALID && m_nCurButton<MAX_SPECIAL_BUTTON_NUM )
					{
						const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillData(pCSkill->dwSkill[i]);
						if( P_VALID(pSkill) )
						{
							DWORD dwSkillID = MTransSkillTypeID(pSkill->dwSkillID, pSkill->nLevel);
							if( IsHaveSpecialSkill(dwSkillID) )
								continue;
							m_pBtn_Skill[m_nCurButton]->SetBtnData( ESBT_Skill, dwSkillID, 0, 0 );
							tstring picName = SkillMgr::Inst()->GetSkillIconPath(dwSkillID);
							m_pBtn_Skill[m_nCurButton]->ClearPic();
							m_pBtn_Skill[m_nCurButton]->SetPicFileName(picName);
							m_pBtn_Skill[m_nCurButton]->SetInvisible(FALSE);
							++m_nCurButton;
						}
					}
				}	
			}
		}
	}
}

void SpecialSkillFrame::SetOtherSpecialSkill( DWORD dwRoleID )
{
	Role* pRole = RoleMgr::Inst()->FindRole(dwRoleID);
	if( P_VALID( pRole ) )
	{
		TList<tagRoleBuff*>& buffList = pRole->GetBuffList();
		tagRoleBuff* pBuff = NULL;
		buffList.ResetIterator();
		while( buffList.PeekNext(pBuff) )
		{
			//连接技存在,且是自身的连接技
			const tagContinuteSkill* pCSkill = SkillProtoData::Inst()->FindContinuteSkillProto(pBuff->dwBuffTypeID/100);
			if( P_VALID(pCSkill) && pCSkill->nTarget==1 )
			{
				for( int i=0; i<MAX_CONTINUE_SKILL_NUM; ++i )
				{
					if( pCSkill->dwSkill[i]!=GT_INVALID && m_nCurButton<MAX_SPECIAL_BUTTON_NUM )
					{
						const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillData(pCSkill->dwSkill[i]);
						if( P_VALID(pSkill) )
						{
							DWORD dwSkillID = MTransSkillTypeID(pSkill->dwSkillID, pSkill->nLevel);
							if( IsHaveSpecialSkill(dwSkillID) )
								continue;
							m_pBtn_Skill[m_nCurButton]->SetBtnData( ESBT_Skill, dwSkillID, 0, 0 );
							tstring picName = SkillMgr::Inst()->GetSkillIconPath(dwSkillID);
							m_pBtn_Skill[m_nCurButton]->ClearPic();
							m_pBtn_Skill[m_nCurButton]->SetPicFileName(picName);
							m_pBtn_Skill[m_nCurButton]->SetInvisible(FALSE);
							++m_nCurButton;
						}
					}
				}	
			}
		}
	}
}

DWORD SpecialSkillFrame::OnKey( DWORD dwID, BOOL bDown )
{
	if( m_pWnd->IsInvisible() )
		return 0;

	for( int i=0; i<MAX_SPECIAL_BUTTON_NUM; i++ )
	{
		if( dwID==m_pUtil->Crc32(m_szAccArray[i]) )
		{
			const BtnData btnData = m_pBtn_Skill[i]->GetBtnData();
			bool bCanUse = m_pBtn_Skill[i]->m_bCanUse;
			if(bCanUse && btnData.GetType()==ESBT_Skill && bDown)
			{
				m_pBtn_Skill[i]->BeginPressBtn();
				m_pBtn_Skill[i]->InvokeGameEvent();
			}
			else if( !bCanUse && btnData.GetType()==ESBT_Skill && bDown)
			{
				m_pBtn_Skill[i]->BeginPressBtn();
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillSvrErr5")]);
			}
			break;
		}
	}

	return 0;
}

BOOL SpecialSkillFrame::IsHaveSpecialSkill(DWORD dwSkillID)
{
	for( int i=0; i<(int)m_nCurButton; ++i )
	{
		const BtnData& data = m_pBtn_Skill[i]->GetBtnData();
		if(data.GetData1() == dwSkillID)
		{
			return TRUE;
		}
	}

	return FALSE;
}

DWORD SpecialSkillFrame::OnKeyChange( DWORD dwID, DWORD key1, DWORD key2 )
{
	for( int i=0; i<MAX_SPECIAL_BUTTON_NUM; ++i )
	{
		if( dwID==m_pUtil->Crc32(m_szAccArray[i]) )
		{
			tstring strKey1, strKey2;
			if( key2 )
				strKey2 = this->GetKeyName((DIK)key2);
			if( key1 )
			{
				if( !strKey2.empty() )
				{
					strKey1 = this->GetKeyName( (DIK)key1 );
					strKey1.append(_T("+"));
					strKey1.append(strKey2.c_str());
				}
				else 
					strKey1 = this->GetKeyName( (DIK)key1 );
			}

			for( int i=0; i<MAX_SPECIAL_BUTTON_NUM; ++i )
			{	
				DWORD dwKeyID = m_pBtn_Skill[i]->GetDIKBar();
				if( dwID==dwKeyID )
				{
					if( !strKey1.empty() )
						m_pBtn_Skill[i]->SetKeyBar(strKey1.c_str());
					else
						m_pBtn_Skill[i]->SetKeyBar(strKey2.c_str());
				}
			}
			break;
		}
	}
	m_pWnd->SetRefresh(TRUE);
	return 0;
}

LPCTSTR SpecialSkillFrame::GetKeyName( DIK key )
{
	switch(key)
	{
	case DIK_ESCAPE:		return _T("ESC");
	case DIK_1:				return _T("1");        
	case DIK_2:				return _T("2");               
	case DIK_3:				return _T("3");               
	case DIK_4:				return _T("4");               
	case DIK_5:				return _T("5");               
	case DIK_6:				return _T("6");               
	case DIK_7:				return _T("7");               
	case DIK_8:				return _T("8");               
	case DIK_9:				return _T("9");               
	case DIK_0:				return _T("0");               
	case DIK_MINUS:			return _T("-");
	case DIK_EQUALS:		return _T("=");
	case DIK_BACK:			return _T("BackSpace");
	case DIK_TAB:			return _T("TAB");             
	case DIK_Q:				return _T("Q");               
	case DIK_W:				return _T("W");               
	case DIK_E:				return _T("E");
	case DIK_R:				return _T("R");
	case DIK_T:				return _T("T");              
	case DIK_Y:				return _T("Y");
	case DIK_U:				return _T("U");
	case DIK_I:				return _T("I");
	case DIK_O:				return _T("O");
	case DIK_P:				return _T("P");
	case DIK_LBRACKET:		return _T("[");
	case DIK_RBRACKET:		return _T("]");
	case DIK_RETURN:		return _T("Enter");
	case DIK_LCONTROL:		return _T("c.");
	case DIK_A:				return _T("A");
	case DIK_S:				return _T("S");
	case DIK_D:				return _T("D");
	case DIK_F:				return _T("F");
	case DIK_G:				return _T("G");
	case DIK_H:				return _T("H");
	case DIK_J:				return _T("J");
	case DIK_K:				return _T("K");
	case DIK_L:				return _T("L");
	case DIK_SEMICOLON:		return _T(";");
	case DIK_APOSTROPHE:	return _T("'");
	case DIK_GRAVE:			return _T("`");					/* accent grave */
	case DIK_LSHIFT:		return _T("s.");
	case DIK_BACKSLASH:		return _T("\\");       
	case DIK_Z:				return _T("Z");               
	case DIK_X:				return _T("X");               
	case DIK_C:				return _T("C");               
	case DIK_V:				return _T("V");               
	case DIK_B:				return _T("B");                              
	case DIK_N:				return _T("N");                              
	case DIK_M:				return _T("M");                              
	case DIK_COMMA:			return _T(",");                  
	case DIK_PERIOD:		return _T(".");					/* . on main keyboard */
	case DIK_SLASH:			return _T("/");					/* / on main keyboard */
	case DIK_RSHIFT:		return _T("s.");                         
	case DIK_MULTIPLY:		return _T("NumPad*");           /* * on numeric keypad */
	case DIK_LMENU:			return _T("a.");             /* left Alt */
	case DIK_SPACE:			return _T("Space");                          
	case DIK_CAPITAL:		return _T("CapsLock");                        
	case DIK_F1:			return _T("F1");                             
	case DIK_F2:			return _T("F2");                                           
	case DIK_F3:			return _T("F3");                                           
	case DIK_F4:			return _T("F4");                                           
	case DIK_F5:			return _T("F5");                                           
	case DIK_F6:			return _T("F6");                                           
	case DIK_F7:			return _T("F7");                                           
	case DIK_F8:			return _T("F8");                                           
	case DIK_F9:			return _T("F9");                                           
	case DIK_F10:			return _T("F10");                                          
	case DIK_NUMLOCK :		return _T("NumLock");        
	case DIK_SCROLL:		return _T("ScrollLock");        /* Scroll Lock */
	case DIK_NUMPAD7:		return _T("NumPad7");         
	case DIK_NUMPAD8:		return _T("NumPad8");         
	case DIK_NUMPAD9:		return _T("NumPad9");         
	case DIK_SUBTRACT:		return _T("NumPad-");           /* - on numeric keypad */
	case DIK_NUMPAD4:		return _T("NumPad4");         
	case DIK_NUMPAD5:		return _T("NumPad5");         
	case DIK_NUMPAD6:		return _T("NumPad6");         
	case DIK_ADD:			return _T("NumPad+");           /* + on numeric keypad */
	case DIK_NUMPAD1:		return _T("NumPad1");         
	case DIK_NUMPAD2:		return _T("NumPad2");         
	case DIK_NUMPAD3:		return _T("NumPad3");         
	case DIK_NUMPAD0:		return _T("NumPad0");         
	case DIK_DECIMAL:		return _T("NumPad.");           /* . on numeric keypad */
	case DIK_OEM_102:		return _T("OEM_102");           /* <> or \| on RT 102-key keyboard (Non-U.S.) */
	case DIK_F11:			return _T("F11");             
	case DIK_F12:			return _T("F12");             
	case DIK_F13:			return _T("F13");               /*                     (NEC PC98) */
	case DIK_F14:			return _T("F14");               /*                     (NEC PC98) */
	case DIK_F15:			return _T("F15");               /*                     (NEC PC98) */
	case DIK_KANA:			return _T("Kana");              /* (Japanese keyboard)            */
	case DIK_ABNT_C1:		return _T("ABNT_C1");           /* /? on Brazilian keyboard */
	case DIK_CONVERT:		return _T("Convert");           /* (Japanese keyboard)            */
	case DIK_NOCONVERT:		return _T("NoConvert");         /* (Japanese keyboard)            */
	case DIK_YEN:			return _T("YEN");               /* (Japanese keyboard)            */
	case DIK_ABNT_C2:		return _T("ABNT_C2");           /* Numpad . on Brazilian keyboard */
	case DIK_NUMPADEQUALS:	return _T("NumPad=");		    /* = on numeric keypad (NEC PC98) */
	case DIK_PREVTRACK:		return _T("PreTrack");			/* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
	case DIK_AT:			return _T("AT");                /*                     (NEC PC98) */
	case DIK_COLON:			return _T("Colon");             /*                     (NEC PC98) */
	case DIK_UNDERLINE:		return _T("UnderLine");         /*                     (NEC PC98) */
	case DIK_KANJI:			return _T("Kanji");             /* (Japanese keyboard)            */
	case DIK_STOP:			return _T("Stop");              /*                     (NEC PC98) */
	case DIK_AX:			return _T("AX");                /*                     (Japan AX) */
	case DIK_UNLABELED:		return _T("UnLabeled");         /*                        (J3100) */
	case DIK_NEXTTRACK:		return _T("NextTrack");         /* Next Track */
	case DIK_NUMPADENTER:	return _T("NumPadEnter");       /* Enter on numeric keypad */
	case DIK_RCONTROL:		return _T("c.");        
	case DIK_MUTE:			return _T("Mute");              /* Mute */
	case DIK_CALCULATOR:	return _T("Calculator");        /* Calculator */
	case DIK_PLAYPAUSE:		return _T("Play");              /* Play / Pause */
	case DIK_MEDIASTOP:		return _T("MediaStop");         /* Media Stop */
	case DIK_VOLUMEDOWN:	return _T("Volume-");           /* Volume - */
	case DIK_VOLUMEUP:		return _T("Volume+");			/* Volume + */
	case DIK_WEBHOME:		return _T("WebHome");           /* Web home */
	case DIK_NUMPADCOMMA:	return _T("NumPadComma");       /* , on numeric keypad (NEC PC98) */
	case DIK_DIVIDE:		return _T("NumPad/");           /* / on numeric keypad */
	case DIK_SYSRQ:			return _T("PrintScreen");           
	case DIK_RMENU:			return _T("a.");             /* right Alt */
	case DIK_PAUSE:			return _T("Pause");             /* Pause */
	case DIK_HOME:			return _T("Home");              /* Home on arrow keypad */
	case DIK_UP:			return _T("Up");                /* UpArrow on arrow keypad */
	case DIK_PRIOR:			return _T("PageUp");            /* PgUp on arrow keypad */
	case DIK_LEFT:			return _T("Left");              /* LeftArrow on arrow keypad */
	case DIK_RIGHT:			return _T("Right");             /* RightArrow on arrow keypad */
	case DIK_END:			return _T("End");               /* End on arrow keypad */
	case DIK_DOWN:			return _T("Down");              /* DownArrow on arrow keypad */
	case DIK_NEXT:			return _T("PageDown");          /* PgDn on arrow keypad */
	case DIK_INSERT:		return _T("Insert");            /* Insert on arrow keypad */
	case DIK_DELETE:		return _T("Delete");            /* Delete on arrow keypad */
	case DIK_LWIN:			return _T("L-Windows");         /* Left Windows key */
	case DIK_RWIN:			return _T("R-Windows");         /* Right Windows key */
	case DIK_APPS:			return _T("AppMenu");           /* AppMenu key */
	case DIK_POWER:			return _T("Power");             /* System Power */
	case DIK_SLEEP:			return _T("Sleep");             /* System Sleep */
	case DIK_WAKE:			return _T("Wake");              /* System Wake */
	case DIK_WEBSEARCH:		return _T("WebSearch");         /* Web Search */
	case DIK_WEBFAVORITES:	return _T("WebFavorites");      /* Web Favorites */
	case DIK_WEBREFRESH:	return _T("WebRefresh");        /* Web Refresh */
	case DIK_WEBSTOP:		return _T("WebStop");           /* Web Stop */
	case DIK_WEBFORWARD:	return _T("WebForward");        /* Web Forward */
	case DIK_WEBBACK:		return _T("WebBack");           /* Web Back */
	case DIK_MYCOMPUTER:	return _T("MyComputer");        /* My Computer */
	case DIK_MAIL:			return _T("Mail");              /* Mail */
	case DIK_MEDIASELECT:	return _T("MediaSelect");       /* Media Select */
	default:				return _T("Unknow");
	}
}