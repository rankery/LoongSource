#include "stdafx.h"
#include "CharacterFrame.h"
#include "ReputeFrame.h"
#include "RoleTitleFrame.h"
#include "InformationFrame.h"
#include "StateFrame.h"
#include "EquipmentFrame.h"
#include "..\WorldDefine\reputation.h"

CharacterFrame::CharacterFrame(void):m_Trunk(this)
{
	m_pState = NULL;
	m_pEquipment = NULL;
	m_pWnd = NULL;
	m_pWndCaption = NULL;
	m_pPbnState = NULL;
	m_pPbnCareer = NULL;
	m_pPbnRepute = NULL;
	m_pPbnRoleTitle = NULL;
	m_pBtnClose = NULL;
	m_pWndState	= NULL;
	m_pWndRepute = NULL;
	m_pWndCareer = NULL;
	m_pWndRoleTitle = NULL;
}

CharacterFrame::~CharacterFrame(void)
{
}

BOOL CharacterFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	m_pMgr->RegisterEventHandle(_T("Open_Role"),    (FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnOpenCharacter));
	m_pMgr->RegisterEventHandle(_T("Close_Role"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnCloseCharacter));

    m_pMgr->RegisterEventHandle(_T("OpenEquipState"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnOpenEquipState));
    m_pMgr->RegisterEventHandle(_T("OpenInfomation"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnOpenInfomation));
    m_pMgr->RegisterEventHandle(_T("OpenReputation"),   (FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnOpenReputation));
    m_pMgr->RegisterEventHandle(_T("OpenTitle"),	    (FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnOpenTitle));

	return bRet;
}

BOOL CharacterFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();
	
	if( P_VALID(m_pWnd) )
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
	
	m_pState = NULL;
	m_pEquipment = NULL;
	m_pRoleTitle = NULL;
	m_pRepute = NULL;
	m_pInformation = NULL;

	m_pMgr->UnRegisterEventHandler(_T("Open_Role"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnOpenCharacter));
	m_pMgr->UnRegisterEventHandler(_T("Close_Role"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnCloseCharacter));

    m_pMgr->UnRegisterEventHandler(_T("OpenEquipState"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnOpenEquipState));
    m_pMgr->UnRegisterEventHandler(_T("OpenInfomation"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnOpenInfomation));
    m_pMgr->UnRegisterEventHandler(_T("OpenReputation"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnOpenReputation));
    m_pMgr->UnRegisterEventHandler(_T("OpenTitle"),	        (FRAMEEVENTPROC)m_Trunk.sfp1(&CharacterFrame::OnOpenTitle));

	return bRet;
}

BOOL CharacterFrame::ReloadUI()
{
	// 创建
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\rolestate.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\role", "ClassName", "PlayerViewWindow");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\armet\\armetpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\mantle\\mantlepic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\armor\\armorpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\sash\\sashpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\cuff\\cuffpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\mainwp\\mainwppic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\minorwp\\minorwppic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\ring1\\ring1pic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\ring2\\ring2pic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\boot\\bootpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\pants\\pantspic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\necklace\\nlpic", "ClassName", "ItemButton");
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\rstatepic\\equipback\\mask\\maskpic", "ClassName", "ItemButton");

	for (int i = 0; i < ECLT_NUM; ++i)
	{
		char cbuff[128];
		_snprintf(cbuff, sizeof(cbuff)/sizeof(char),
			"ra_win\\ra_pic\\repute\\reputebackpic\\propic%d\\pro%d",
			i + 1, i + 1);
		m_pGUI->ChangeXml(&ele, cbuff, "ClassName", "ProgressEx");
	}
	
	m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\roletitle\\titlelistpic\\titlelist", "ClassName", "TreeEx");
    m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\informationpic\\information\\headpicback\\headpic", "ClassName", "HttpPicEx");
    m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\informationpic\\information\\cityname\\cityname_input", "MaxTextCount", "6");
    m_pGUI->ChangeXml(&ele, "ra_win\\ra_pic\\informationpic\\information\\presentationpic\\presentation", "MaxTextCount", "52");
	
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&CharacterFrame::EventHandler));

	m_pWndCaption = m_pWnd->GetChild(_T("ra_pic"));
	m_pWndState = m_pWndCaption->GetChild(_T("rstatepic"));
	m_pWndCareer = m_pWndCaption->GetChild(_T("informationpic"));
	m_pWndRepute = m_pWndCaption->GetChild(_T("repute"));
	m_pWndRoleTitle = m_pWndCaption->GetChild(_T("roletitle"));

	m_pPbnState = (GUIPushButton*)m_pWndCaption->GetChild(_T("winopt\\optbutt1"));
	m_pPbnCareer = (GUIPushButton*)m_pWndCaption->GetChild(_T("winopt\\optbutt2"));
	m_pPbnRepute = (GUIPushButton*)m_pWndCaption->GetChild(_T("winopt\\optbutt3"));
	m_pPbnRoleTitle = (GUIPushButton*)m_pWndCaption->GetChild(_T("winopt\\optbutt30"));

	m_pPbnState->SetEnable(true);
	m_pPbnCareer->SetEnable(true);
	m_pPbnRepute->SetEnable(true);
	m_pPbnRoleTitle->SetEnable(true);

	m_pPbnState->SetState(EGUIBS_PushDown, FALSE);

	m_pBtnClose = (GUIButton*)m_pWndCaption->GetChild(_T("new"));

	m_pState = (StateFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("State"), _T("StateFrame"), 0);

	m_pEquipment = (EquipmentFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("Equip"), _T("EquipmentFrame"), 0);

	m_pRepute = (ReputeFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("Repute"), _T("ReputeFrame"), 0);
	if(!P_VALID(m_pRepute))
		IMSG(_T("Create ReputeFrame failed"));

	m_pRoleTitle = (RoleTitleFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("RoleTitle"), _T("RoleTitleFrame"), 0);
	if(!P_VALID(m_pRoleTitle))
		IMSG(_T("Create RoleTitleFrame failed"));

	m_pInformation = (InformationFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("Information"), _T("InformationFrame"), 0);
	if(!P_VALID(m_pInformation))
		IMSG(_T("Create InformationFrame failed"));

	m_pWnd->SetInvisible(TRUE);

	return TRUE;
}

DWORD CharacterFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnClose)
			{
				m_pMgr->SendEvent( &tagGameEvent( _T("Close_Role_UI"), this ) );
			}
		}
		break;
	case EGUIE_PushButtonDown:
		{
			if (pEvent->dwParam1 != 1)//不是按下不处理
				return 0;

			if (pWnd == m_pPbnState)//角色
			{
				m_pWndState->SetInvisible(FALSE);
				m_pWndCareer->SetInvisible(TRUE);
				m_pWndRepute->SetInvisible(TRUE);
				m_pWndRoleTitle->SetInvisible(TRUE);
			}
			else if (pWnd == m_pPbnCareer)//名帖
			{
				m_pWndState->SetInvisible(TRUE);
				m_pWndCareer->SetInvisible(FALSE);
				m_pWndRepute->SetInvisible(TRUE);
				m_pWndRoleTitle->SetInvisible(TRUE);
			}
			else if (pWnd == m_pPbnRepute)//声望
			{
				m_pWndState->SetInvisible(TRUE);
				m_pWndCareer->SetInvisible(TRUE);
				m_pWndRepute->SetInvisible(FALSE);
				m_pWndRoleTitle->SetInvisible(TRUE);
			}
			else if (pWnd == m_pPbnRoleTitle)//称号
			{
				m_pWndState->SetInvisible(TRUE);
				m_pWndCareer->SetInvisible(TRUE);
				m_pWndRepute->SetInvisible(TRUE);
				m_pWndRoleTitle->SetInvisible(FALSE);
			}
		}
		break;
	}
	return 0;
}

VOID CharacterFrame::ShowWnd( BOOL bVal )
{
	m_pWnd->SetInvisible(!bVal);
	if(bVal)
	{
		m_pEquipment->UpdateEquipAvatar();
		m_pState->UpdatePalyerAtt();
		m_pRoleTitle->UpdateRoleTitle();
		m_pInformation->UpdateInformation();
        m_pRepute->UpdateRepute();

		m_pWnd->FlipToTop();
		m_pWnd->SetRefresh();
	}
	else
	{
		/*! 关闭名人堂 */
		m_pFrameMgr->SendEvent(&tagGameEvent(_T("SetFamousClose"), this));
	}
}


DWORD CharacterFrame::OnOpenCharacter( tagGameEvent* pGameEvent )
{
	ShowWnd(TRUE);

	return 0;
}

DWORD CharacterFrame::OnCloseCharacter( tagGameEvent* pGameEvent )
{
	ShowWnd(FALSE);

	return 0;
}

BOOL CharacterFrame::EscCancel()
{
	// 界面隐藏则返回FALSE
	if(m_pWnd->IsInvisible())
		return FALSE;
	// 否则发送关闭事件，返回TRUE
	m_pMgr->SendEvent( &tagGameEvent( _T("Close_Role_UI"), this ) );
	return TRUE;
}

DWORD CharacterFrame::OnOpenEquipState( tagGameEvent* pEvent )
{
    if( EGUIBS_PushDown != m_pPbnState->GetState() )
        m_pPbnState->SetState( EGUIBS_PushDown );

	return 0;
}

DWORD CharacterFrame::OnOpenInfomation( tagGameEvent* pEvent )
{
    if( EGUIBS_PushDown != m_pPbnCareer->GetState() )
        m_pPbnCareer->SetState( EGUIBS_PushDown );
	
	return 0;
}

DWORD CharacterFrame::OnOpenReputation( tagGameEvent* pEvent )
{
    if( EGUIBS_PushDown != m_pPbnRepute->GetState() )
        m_pPbnRepute->SetState( EGUIBS_PushDown );

	return 0;
}

DWORD CharacterFrame::OnOpenTitle( tagGameEvent* pEvent )
{
    if( EGUIBS_PushDown != m_pPbnRoleTitle->GetState() )
        m_pPbnRoleTitle->SetState( EGUIBS_PushDown );

	return 0;
}