#include "stdafx.h"
#include "RemoteRoleStateFrame.h"
#include "TargetCharacterFrame.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "ItemButton.h"
#include "PlayerViewWindow.h"
#include "Item.h"
#include "ItemMgr.h"
#include "Role.h"
#include "Player.h"
#include "RoleMgr.h"
#include "..\WorldDefine\role_att.h"

RemoteRoleStateFrame::RemoteRoleStateFrame( void ):m_Trunk(this)
{

	m_pWnd_Father = NULL;
	m_pWnd_State = NULL;		
	m_pStc_PlayerName = NULL;		
	m_pStc_PlayerGuild = NULL;
	m_pStc_PlayerLevel = NULL;
	m_pStc_PlayerClass = NULL;
	m_pFm_SuitOpt = NULL;
	m_pStc_SuitOpt = NULL;
	m_pPbn_DisplayEquip = NULL;
	m_pPbn_DisplayFashion = NULL;
	m_pWnd_AttEx = NULL;
	m_pWnd_AttIn = NULL;
	m_pWnd_Resist = NULL;
	m_pWnd_EquipView = NULL;
	for (INT i=0; i<EEP_MaxEquip; ++i)
	{
		m_pStc_EquipBack[EEP_MaxEquip] = NULL;
		m_pBtn_Equip[EEP_MaxEquip] = NULL;
	}
	m_bIsFashionDisplay = FALSE;
	m_dwTime = GT_INVALID;
}

RemoteRoleStateFrame::~RemoteRoleStateFrame( void )
{

}

BOOL RemoteRoleStateFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	//注册网络消息
	m_pCmdMgr->Register("NS_GetRemoteRoleEquipInfo",			(NETMSGPROC)m_Trunk.sfp2(&RemoteRoleStateFrame::OnNS_GetRemoteRoleEquipInfo),    _T("NS_GetRemoteRoleEquipInfo"));

	//注册游戏事件
	m_pMgr->RegisterEventHandle(_T("DeleRemoteRoleEquipInfo"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&RemoteRoleStateFrame::OnDeleRemoteRoleEquipInfo));

	return bRet;
}

BOOL RemoteRoleStateFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	//注销网络消息
	m_pCmdMgr->UnRegister("NS_GetRemoteRoleEquipInfo",			(NETMSGPROC)m_Trunk.sfp2(&RemoteRoleStateFrame::OnNS_GetRemoteRoleEquipInfo));

	//注销游戏事件
	m_pMgr->UnRegisterEventHandler(_T("DeleRemoteRoleEquipInfo"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&RemoteRoleStateFrame::OnDeleRemoteRoleEquipInfo));

	return bRet;
}

BOOL RemoteRoleStateFrame::ReloadUI()
{
	m_pWnd_Father = m_pGUI->GetWnd(((TargetCharacterFrame*)m_pFather)->GetGUIFullName().c_str());
	m_pWnd_State = m_pWnd_Father->GetChild(_T("ra_pic\\rstatepic"));

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd_State->GetFullName().c_str(), m_Trunk.sfp1(&RemoteRoleStateFrame::EventHandler));

	TCHAR szTmp[X_SHORT_NAME] = {0};
	//人物名称,帮派,等级
	{
		m_pStc_PlayerName = (GUIStatic*)m_pWnd_State->GetChild(_T("rname"));
		m_pStc_PlayerLevel = (GUIStatic*)m_pWnd_State->GetChild(_T("rgrad1"));
		m_pStc_PlayerClass = (GUIStatic*)m_pWnd_State->GetChild(_T("rcareer"));
		m_pStc_PlayerGuild = (GUIStatic*)m_pWnd_State->GetChild(_T("rpost1"));
		m_pStc_PlayerLevel->SetText(_T(""));
	}

	//二级属性和固有属性
	{
		m_pWnd_AttIn = m_pWnd_State->GetChild(_T("rs_pic1"));
		m_pWnd_AttIn->SetInvisible(TRUE);
		m_pWnd_AttEx = m_pWnd_State->GetChild(_T("rs_pic2"));
		m_pWnd_AttEx->SetInvisible(TRUE);
	}

	//抗性
	{
		m_pWnd_Resist = m_pWnd_State->GetChild(_T("rs_pic3"));
		m_pWnd_Resist->SetInvisible(TRUE);
	}
	//时装/装备显示
	{
		m_pStc_SuitOpt = (GUIStatic*)m_pWnd_State->GetChild(_T("word1"));
		m_pFm_SuitOpt = (GUIFrameMutex*)m_pWnd_State->GetChild(_T("word1\\ra_opt"));
		m_pPbn_DisplayEquip = (GUIPushButton*)m_pWnd_State->GetChild(_T("word1\\ra_opt\\optbutt2"));
		m_pPbn_DisplayFashion = (GUIPushButton*)m_pWnd_State->GetChild(_T("word1\\ra_opt\\optbutt3"));
		m_pStc_SuitOpt->SetInvisible(TRUE);
	}

	// 获取控件
	// 装备预览
	m_pWnd_EquipView = (PlayerViewWindow*)m_pWnd_State->GetChild(_T("equipback\\role"));
	// 头部
	m_pStc_EquipBack[EEP_Head] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\armet"));
	m_pBtn_Equip[EEP_Head] = (ItemButton*)m_pStc_EquipBack[EEP_Head]->GetChild(_T("armetpic"));
	// 面部
	m_pStc_EquipBack[EEP_Face] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\mask"));
	m_pBtn_Equip[EEP_Face] = (ItemButton*)m_pStc_EquipBack[EEP_Face]->GetChild(_T("maskpic"));
	// 上身
	m_pStc_EquipBack[EEP_Body] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\armor"));
	m_pBtn_Equip[EEP_Body] = (ItemButton*)m_pStc_EquipBack[EEP_Body]->GetChild(_T("armorpic"));
	// 下身
	m_pStc_EquipBack[EEP_Legs] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\pants"));
	m_pBtn_Equip[EEP_Legs] = (ItemButton*)m_pStc_EquipBack[EEP_Legs]->GetChild(_T("pantspic"));
	// 背部
	m_pStc_EquipBack[EEP_Back] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\mantle"));
	m_pBtn_Equip[EEP_Back] = (ItemButton*)m_pStc_EquipBack[EEP_Back]->GetChild(_T("mantlepic"));
	// 腕部
	m_pStc_EquipBack[EEP_Wrist] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\cuff"));
	m_pBtn_Equip[EEP_Wrist] = (ItemButton*)m_pStc_EquipBack[EEP_Wrist]->GetChild(_T("cuffpic"));
	// 脚部
	m_pStc_EquipBack[EEP_Feet] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\boot"));
	m_pBtn_Equip[EEP_Feet] = (ItemButton*)m_pStc_EquipBack[EEP_Feet]->GetChild(_T("bootpic"));
	// 戒指
	m_pStc_EquipBack[EEP_Finger1] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\ring1"));
	m_pBtn_Equip[EEP_Finger1] = (ItemButton*)m_pStc_EquipBack[EEP_Finger1]->GetChild(_T("ring1pic"));
	// 戒指
	m_pStc_EquipBack[EEP_Finger2] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\ring2"));
	m_pBtn_Equip[EEP_Finger2] = (ItemButton*)m_pStc_EquipBack[EEP_Finger2]->GetChild(_T("ring2pic"));
	// 腰饰
	m_pStc_EquipBack[EEP_Waist] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\sash"));
	m_pBtn_Equip[EEP_Waist] = (ItemButton*)m_pStc_EquipBack[EEP_Waist]->GetChild(_T("sashpic"));
	// 项链
	m_pStc_EquipBack[EEP_Neck] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\necklace"));
	m_pBtn_Equip[EEP_Neck] = (ItemButton*)m_pStc_EquipBack[EEP_Neck]->GetChild(_T("nlpic"));
	// 主手武器
	m_pStc_EquipBack[EEP_RightHand] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\mainwp"));
	m_pBtn_Equip[EEP_RightHand] = (ItemButton*)m_pStc_EquipBack[EEP_RightHand]->GetChild(_T("mainwppic"));
	// 副手武器
	m_pStc_EquipBack[EEP_LeftHand] = (GUIStatic*)m_pWnd_State->GetChild(_T("equipback\\minorwp"));
	m_pBtn_Equip[EEP_LeftHand] = (ItemButton*)m_pStc_EquipBack[EEP_LeftHand]->GetChild(_T("minorwppic"));


	return TRUE;
}

VOID RemoteRoleStateFrame::Render3D()
{
	if( P_VALID( m_pWnd_EquipView ) )
		m_pWnd_EquipView->Render3D();
	GameFrame::Render3D();
}

DWORD RemoteRoleStateFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch (pEvent->eEvent)
	{
	case EGUIE_MoveIntoCtrl:				// 鼠标移入该控件	
		{
			for(INT i = 0; i < EEP_MaxEquip; ++i)
			{
				if(pWnd == m_pBtn_Equip[i])
				{
					Equipment* pEquip = NULL;
					/*if (!m_bIsFashionDisplay || i <= EEP_Feet || i == EEP_RightHand || i == EEP_LeftHand)*/
					pEquip = GetCurRemoteRoleEquip(GetEquipPos(i));

					if(P_VALID(pEquip))
					{
						ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pEquip).c_str());
					}
					else
					{
						// 没有装备则显示装备部位
						TCHAR szTmp[X_SHORT_NAME];
						_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("EquipPos%d"), GetEquipPos(i)+1);
						tstring strTmp = g_StrTable[_T("TipsDefaultFont")];
						strTmp += g_StrTable[szTmp];
						ShowTip(pWnd, strTmp.c_str());
					}
					break;
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:				// 鼠标移出该控件
		{
			ShowTip(pWnd, NULL);
		}
		break;
	default:
		return 0;
		break;
	}

	return 0;
}

DWORD RemoteRoleStateFrame::OnDeleRemoteRoleEquipInfo(tagGameEvent* pGameEvent)
{
	m_mapRemoteRoleEquip.clear();
	
	return 0;
}

DWORD RemoteRoleStateFrame::OnNS_GetRemoteRoleEquipInfo(tagNS_GetRemoteRoleEquipInfo* pMsg, DWORD dwParam)
{
	if( pMsg->dwRoleID == m_dwRemoteRoleID )
	{
		tagEquip* equip = reinterpret_cast<tagEquip*>(pMsg->byEquip);
		for( int i=0; i<pMsg->nEquipNum; ++i )
		{
			Equipment equipex(equip[i]);
			if( equipex.GetConType() == EICT_Equip )
				m_mapRemoteRoleEquip.insert(make_pair( equipex.GetPos(), equipex ));
		}

		UpdateAllEquip();
	}
	return 0;
}

EEquipPos RemoteRoleStateFrame::GetEquipPos( INT nIndex )
{
	EEquipPos eRet = EEP_End;
	if(m_bIsFashionDisplay)
	{
		if(nIndex < EEP_End - EEP_MaxEquip)
		{
			eRet = (EEquipPos)(nIndex + EEP_MaxEquip);
		}
		else
		{
			eRet = (EEquipPos)nIndex;
		}
	}
	else
	{
		eRet = (EEquipPos)nIndex;
	}
	return eRet;
}

Equipment* RemoteRoleStateFrame::GetCurRemoteRoleEquip( EEquipPos pos/*=EEP_Null */ )
{
	map<INT16, Equipment>::iterator it = m_mapRemoteRoleEquip.find(pos);

	if( it!=m_mapRemoteRoleEquip.end() )
		return &it->second;

	return NULL;
}

void RemoteRoleStateFrame::UpdateAllEquip()
{
	for (INT i=0; i<EEP_MaxEquip; ++i)
	{
		UpdateEquipment((EEquipPos)i);
	}

	map<INT16, Equipment>::iterator it = m_mapRemoteRoleEquip.begin();
	for( ; it!=m_mapRemoteRoleEquip.end(); ++it )
	{
		INT16 n16Pos = it->first;
		if( n16Pos >= EEP_MaxEquip  )
			continue;
		BYTE byQuality = it->second.GetItemQuality();
		DWORD dwTypeID = it->second.GetItemTypeID();
		m_pBtn_Equip[n16Pos]->RefreshItem( dwTypeID, 0, byQuality );
	}
}

void RemoteRoleStateFrame::UpdateEquipment( EEquipPos ePos )
{
	DWORD dwTypeID = GT_INVALID;
	BYTE byQuality = 0;
	m_pBtn_Equip[ePos]->RefreshItem( dwTypeID, 0, byQuality );
}

void RemoteRoleStateFrame::UpdateEquipAvatar()
{
	Role* pRole = RoleMgr::Inst()->FindRole( m_dwRemoteRoleID );
	if( P_VALID( pRole ) && P_VALID( m_pWnd_EquipView ))
	{
		Player* pPlayer = dynamic_cast<Player*>(pRole);
		if( P_VALID(pPlayer) )
			m_pWnd_EquipView->UpdataAvatar( pPlayer );
	}
}