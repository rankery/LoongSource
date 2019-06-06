#include "StdAfx.h"
#include "StallShelfFrame.h"
#include "ItemButton.h"
#include "IconStatic.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "ItemMgr.h"
#include "StallMgr.h"
#include "Container.h"
#include "CombatSysUtil.h"
#include "ItemEvent.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "..\WorldDefine\shop_define.h"
#include "..\WorldDefine\msg_stall.h"
#include "FilterData.h"

StallShelfFrame::StallShelfFrame(void) : m_Trunk(this)
{
	m_pWnd			= NULL;
	m_pWndCaption	= NULL;
	m_pBtnShangjia	= NULL;
	m_pBtnXiajia	= NULL;
	//m_pBtnAd		= NULL;
	m_pBtnStall		= NULL;
	m_pBtnPageup	= NULL;
	m_pBtnPagedown	= NULL;
	m_pBtnExit		= NULL;
	m_pStcTitle		= NULL;
	//m_pStcSignboard	= NULL;
	m_pStcPage		= NULL;
	//m_pPbnAdFlag	= NULL;

	m_pStcStallSel.clear();
	m_pStcStallGoods.clear();

	m_nSelGoods = GT_INVALID;
	m_bSelLock	= false;

	ZeroMemory(m_pIstStar, sizeof(m_pIstStar));
}

StallShelfFrame::~StallShelfFrame(void)
{
}

BOOL StallShelfFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
	
	m_pCmdMgr->Register("NS_StallSetGoods",		(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallSetGoods),		_T("NS_StallSetGoods"));
	m_pCmdMgr->Register("NS_StallUnsetGoods",	(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallUnsetGoods),	_T("NS_StallUnsetGoods"));
	m_pCmdMgr->Register("NS_StallSetTitle",		(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallSetTitle),		_T("NS_StallSetTitle"));
	m_pCmdMgr->Register("NS_StallSetAdText",	(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallSetAdText),	_T("NS_StallSetAdText"));
	m_pCmdMgr->Register("NS_StallSetAdFlag",	(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallSetAdFlag),	_T("NS_StallSetAdFlag"));
	m_pCmdMgr->Register("NS_StallSetFinish",	(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallSetFinish),	_T("NS_StallSetFinish"));

	
	m_pMgr->RegisterEventHandle(_T("MsgBox_StallPrice"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&StallShelfFrame::OnEventStallPriceInput));
	m_pMgr->RegisterEventHandle(_T("MsgBox_StallName"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallShelfFrame::OnEventStallNameInput));
	m_pMgr->RegisterEventHandle(_T("MsgBox_StallAd"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallShelfFrame::OnEventStallAdInput));
	m_pMgr->RegisterEventHandle(_T("ItemEvent_Move2Stall"), (FRAMEEVENTPROC)m_Trunk.sfp1(&StallShelfFrame::OnEventStallAddItem));
	m_pMgr->RegisterEventHandle(_T("StallStar"), (FRAMEEVENTPROC)m_Trunk.sfp1(&StallShelfFrame::OnEventShowStar));
	
	for (int i = 0; i < 10; ++i)
	{
		m_pIstStar[i]->SetInvisible(TRUE);
	}

	int nStallLevel = RoleMgr::Inst()->GetLocalPlayer()->GetPlayerStallLevel();
	for (int i = 0; i < nStallLevel; ++i)
	{
		m_pIstStar[i]->SetPic(_T("data\\ui\\Bag\\l_baitan-lv1.dds"));
		m_pIstStar[i]->SetInvisible(FALSE);
	}

	return bRet;
}

BOOL StallShelfFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	m_pCmdMgr->UnRegister("NS_StallSetGoods",	(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallSetGoods));
	m_pCmdMgr->UnRegister("NS_StallUnsetGoods",	(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallUnsetGoods));
	m_pCmdMgr->UnRegister("NS_StallSetTitle",	(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallSetTitle));
	m_pCmdMgr->UnRegister("NS_StallSetAdText",	(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallSetAdText));
	m_pCmdMgr->UnRegister("NS_StallSetAdFlag",	(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallSetAdFlag));
	m_pCmdMgr->UnRegister("NS_StallSetFinish",	(NETMSGPROC)m_Trunk.sfp2(&StallShelfFrame::OnNetStallSetFinish));

	m_pMgr->UnRegisterEventHandler(_T("MsgBox_StallPrice"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallShelfFrame::OnEventStallPriceInput));
	m_pMgr->UnRegisterEventHandler(_T("MsgBox_StallName"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallShelfFrame::OnEventStallNameInput));
	m_pMgr->UnRegisterEventHandler(_T("MsgBox_StallAd"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallShelfFrame::OnEventStallAdInput));
	m_pMgr->UnRegisterEventHandler(_T("ItemEvent_Move2Stall"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&StallShelfFrame::OnEventStallAddItem));
	
	m_pMgr->UnRegisterEventHandler(_T("StallStar"), (FRAMEEVENTPROC)m_Trunk.sfp1(&StallShelfFrame::OnEventShowStar));

	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
	}
	
	DestroyMsgBox();

	return bRet;
}

BOOL StallShelfFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\store.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;
	
	for(INT i=0; i<STALL_MAX_DISPLAY; ++i)
	{
		CHAR szTmp[X_LONG_NAME] = {0};
		_snprintf(szTmp, sizeof(szTmp), "storewin\\storepic\\storeback\\store%d", i+1);
		m_pGUI->ChangeXml(&element, szTmp, "ClassName", "IconStatic");
		
		_snprintf(szTmp, sizeof(szTmp), "storewin\\storepic\\storeback\\store%d\\item%d", i+1, i+1);
		m_pGUI->ChangeXml(&element, szTmp, "ClassName", "GoodsButton");
	}

	for (int i = 0; i < 10; ++i)
	{
		CHAR szTmp[X_LONG_NAME] = {0};
		_snprintf(szTmp, sizeof(szTmp), "storewin\\storepic\\img%d", i + 1);
		m_pGUI->ChangeXml(&element, szTmp, "ClassName", "IconStatic");
	}
	
	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), (VUIEVENTHANDLE)m_Trunk.sfp1(&StallShelfFrame::EventHandler));

	m_pWndCaption	= m_pWnd->GetChild(_T("storepic"));
	m_pBtnShangjia	= (GUIButton*)m_pWndCaption->GetChild(_T("sale"));
	m_pBtnXiajia	= (GUIButton*)m_pWndCaption->GetChild(_T("del"));
	m_pBtnStall		= (GUIButton*)m_pWndCaption->GetChild(_T("close"));
	//m_pBtnAd		= (GUIButton*)m_pWndCaption->GetChild(_T("advbutt"));
	m_pBtnExit		= (GUIButton*)m_pWndCaption->GetChild(_T("closebutt"));
	m_pBtnPageup	= (GUIButton*)m_pWndCaption->GetChild(_T("back"));
	m_pBtnPagedown	= (GUIButton*)m_pWndCaption->GetChild(_T("next"));
	m_pStcPage		= (GUIStatic*)m_pWndCaption->GetChild(_T("word3"));
	m_pStcTitle		= (GUIStatic*)m_pWndCaption->GetChild(_T("storename"));
	//m_pStcSignboard = (GUIStaticEx*)m_pWndCaption->GetChild(_T("gradepic"));
	//m_pPbnAdFlag	= (GUIPushButton*)m_pWndCaption->GetChild(_T("word2\\new"));

	m_pBtnPageup->SetEnable(FALSE);
	m_pBtnPagedown->SetEnable(FALSE);
	
	for(INT i=0; i<STALL_MAX_DISPLAY; ++i)
	{
		TCHAR szTemp[X_SHORT_NAME] = {0};

		_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("storeback\\store%d"), i+1);
		IconStatic* pIconStatic = (IconStatic*)m_pWndCaption->GetChild(szTemp);
		m_pStcStallSel.push_back(pIconStatic);

		_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("item%d"), i+1);
		GoodsButton* pItemButton = (GoodsButton*)pIconStatic->GetChild(szTemp);
		m_pStcStallGoods.push_back(pItemButton);
		ReflashShelf(i);
	}

	for (int i = 0; i < 10; ++i)
	{
		TCHAR szTmp[256] = {0};
		_sntprintf(szTmp, 256, _T("storepic\\img%d"), i + 1);
		m_pIstStar[i] = (IconStatic*)m_pWnd->GetChild(szTmp);
		m_pIstStar[i]->SetInvisible(TRUE);
	}

	if (StallMgr::Inst()->IsStallState())
	{
		m_pBtnStall->SetText(g_StrTable[_T("StallButton_HideStall")]);
	}
	else
	{
		m_pBtnStall->SetText(g_StrTable[_T("StallButton_ShowStall")]);
	}

	m_pStcTitle->SetText(StallMgr::Inst()->GetStallName(RoleMgr::Inst()->GetLocalPlayerID()));

	//m_pPbnAdFlag->SetState(StallMgr::Inst()->GetStallAdFlag() ? EGUIBS_PushDown : EGUIBS_PopUp, FALSE);

	return TRUE;
}

DWORD StallShelfFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_DragStart:
		{
			if (pWnd == m_pStcTitle)
			{
				if (StallMgr::Inst()->IsStallState())
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("StallError_InStallForbid")]);
					return 0;
				}

				if(P_VALID(GetObj("StallName")))
					KillObj("StallName");

				CreateObj("StallName", "MsgInputBox");
				TObjRef<MsgInputBox>("StallName")->Init(
					_T(""), g_StrTable[_T("StallMsg_NameInputText")], 
					_T("MsgBox_StallName"), GT_INVALID, MBIT_Name);
			}
		}
		break;
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnExit)//退出摊位
			{
				tagGameEvent event(_T("StallEvent_CloseWindow"), this);
				m_pMgr->SendEvent(&event);
			}
			else if (pWnd == m_pBtnPageup)
			{
				ASSERT(1);
				//PageUp();
			}
			else if (pWnd == m_pBtnPagedown)
			{
				ASSERT(1);
				//PageDown();
			}
			else if (pWnd == m_pBtnShangjia)
			{
				SetShangjia(true);
			}
			else if (pWnd == m_pBtnXiajia)
			{
				SetShangjia(false);
			}
			//else if (pWnd == m_pBtnAd)
			//{
			//	if (StallMgr::Inst()->IsStallState())
			//	{
			//		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("StallError_InStallForbid")]);
			//		return 0;
			//	}

			//	if(P_VALID(GetObj("StallAd")))
			//		KillObj("StallAd");

			//	CreateObj("StallAd", "MsgInputBox");
			//	TObjRef<MsgInputBox>("StallAd")->Init(
			//		_T(""), g_StrTable[_T("StallMsg_AdInputText")], 
			//		_T("MsgBox_StallAd"), GT_INVALID, MBIT_Text);
			//	//SetAd();
			//}
			else if (pWnd == m_pBtnStall)
			{
				StartStall();
			}
			else if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)//右键点击货架中的物品
			{
				for (INT16 i = 0; i < STALL_MAX_DISPLAY; ++i)
				{
					if (pWnd == m_pStcStallGoods[i])
					{
						DelGoods(i);
						ShowTip(NULL, NULL);
						break;
					}
				}
			}

		}
		break;
	case EGUIE_Drag:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)//左键点击货架中的物品
			{
				for (INT16 i = 0; i < STALL_MAX_DISPLAY; ++i)
				{
					if (pWnd == m_pStcStallGoods[i])
					{
						SelGoods(i);
						break;
					}
				}
			}
		}
		break;
	case EGUISCB_Drop:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)//拖入货架中的物品
			{
				for (INT16 i = 0; i < STALL_MAX_DISPLAY; ++i)
				{
					if (pWnd == m_pStcStallGoods[i])
					{
						if( EDT_Item == pEvent->dwParam1 )
						{
							OnStallAddItem((INT16)LOWORD(pEvent->dwParam2), i);
						}
						break;
					}
				}
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)//拖动货架中的物品
			{
				for (INT16 i=0; i<STALL_MAX_DISPLAY; ++i)
				{
					if (pWnd == m_pStcStallGoods[i])
					{
						ShowGoodsTip(i);
						break;
					}
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(NULL, _T(""));
		}
		break;
	case EGUIE_PushButtonDown:
		{
			/*if (pWnd == m_pPbnAdFlag)
			{
				tagNC_StallSetAdFlag m;
				m.bFlag = (pEvent->dwParam1 == 1);
				m_pSession->Send(&m);
			}*/
		}
		break;
	default :
		break;
	}
	return 0;
}

VOID StallShelfFrame::SelGoods( INT16 nIndex )
{
	if (m_bSelLock)
		return;

	if (nIndex >= 0 && nIndex < STALL_MAX_DISPLAY)
	{
		if (nIndex != m_nSelGoods)
		{
			tagTemporaryItem* pTemp = StallMgr::Inst()->GetItem(nIndex);
			if (!P_VALID(pTemp))
				return;

			if (m_nSelGoods >= 0 && m_nSelGoods < STALL_MAX_DISPLAY)
			{
				m_pStcStallSel[m_nSelGoods]->SetPic(_T("data\\ui\\Common\\L_icon.bmp"));
			}
			m_pStcStallSel[nIndex]->SetPic(_T("data\\ui\\Common\\L_icon-l.bmp"));
			m_nSelGoods = nIndex;
		}
		else
		{
			m_pStcStallSel[nIndex]->SetPic(_T("data\\ui\\Common\\L_icon.bmp"));
			m_nSelGoods = GT_INVALID;
		}
	}
}

VOID StallShelfFrame::ShowGoodsTip( INT16 nIndex )
{
	tagTemporaryItem* pTemporary = StallMgr::Inst()->GetItem(nIndex);
	if (P_VALID(pTemporary) && P_VALID(pTemporary->pItem))
	{
		tstring strTip = ToolTipCreator::Inst()->GetItemTips(pTemporary->pItem);
		tstring strTipEx = _T("");
		if (pTemporary->bShelf)
		{
			tagItemCost cost;
			cost.n64CostSilver = pTemporary->n64Price;
			strTipEx= ToolTipCreator::Inst()->GetStallPriceTips(&cost, pTemporary->pItem->GetItemQuantity());
		}
		ShowTip(m_pStcStallGoods[nIndex], strTip.c_str(), strTipEx.c_str());
	}
}

VOID StallShelfFrame::SetShangjia( bool bShang )
{
	if (m_nSelGoods >= 0 && m_nSelGoods < STALL_MAX_DISPLAY)
	{	
		//上下架时锁定选中，等待消息返回
		m_bSelLock = true;

		tagTemporaryItem* pTemp = StallMgr::Inst()->GetItem(m_nSelGoods);
		if (P_VALID(pTemp) && P_VALID(pTemp->pItem) && bShang != pTemp->bShelf)
		{
			if (bShang)
			{
				// 上架需要输入单价
				SendPriceInput(pTemp->n64Price);
			}
			else
			{
				// 下架直接发送
				tagNC_StallUnsetGoods m;
				m.byIndex	= (BYTE)m_nSelGoods;
				m_pSession->Send(&m);
			}
		}
		else
		{
			// 货架上没有货物则取消锁定
			m_bSelLock = false;
		}
	}
}


VOID StallShelfFrame::AddGoods( Item* pItem, INT16 nIndex )
{
	BOOL bRet = StallMgr::Inst()->AddItem(pItem, nIndex);
	if (bRet)
	{
		ReflashShelf(nIndex);
		// 拖入后直接执行上架操作
		if (!m_bSelLock)// 没有锁定选中则可以上架
		{
			SelGoods(nIndex);
			SetShangjia(true);
		}
	}
}

VOID StallShelfFrame::DelGoods( INT16 nIndex )
{
	if (GT_INVALID != nIndex)
	{
		BOOL bRet = StallMgr::Inst()->DelItem(nIndex);
		if (bRet)
		{
			ReflashShelf(nIndex);
			// 如果选中则取消
			if (m_nSelGoods == nIndex)
			{
				SelGoods(nIndex);
			}
		}
	}
}

VOID StallShelfFrame::ReflashShelf( INT16 nIndex )
{
	tagTemporaryItem* pTemporary = StallMgr::Inst()->GetItem(nIndex);
	if (P_VALID(pTemporary) && P_VALID(pTemporary->pItem) && pTemporary->pItem->IsItem())
	{
		m_pStcStallGoods[nIndex]->RefreshItem(pTemporary->pItem->GetItemTypeID(), pTemporary->pItem->GetItemQuantity(), pTemporary->pItem->GetItemQuality());
		m_pStcStallGoods[nIndex]->SetShangjia(pTemporary->bShelf);
	}
	else
	{
		m_pStcStallGoods[nIndex]->RefreshItem();
	}
}

VOID StallShelfFrame::StartStall()
{
	// 如果正在摆摊则收摊，否则摆摊
	if (StallMgr::Inst()->IsStallState())
	{
		tagNC_StallClose m;
		m_pSession->Send(&m);
	}
	else 
	{

		if (!RoleMgr::Inst()->GetLocalPlayer()->GetRoleState(ERS_StallArea))
		{
			StallMgr::Inst()->ShowStallErrorMsg(E_Stall_Area_NotAllowed);
			return;
		}

		if (!RoleMgr::Inst()->GetLocalPlayer()->IsIdleState())
		{
			StallMgr::Inst()->ShowStallErrorMsg(E_Stall_RoleState_NotPermit);
			return;
		}

		if (RoleMgr::Inst()->GetLocalPlayer()->GetRoleState(ERS_Combat)
			||RoleMgr::Inst()->GetLocalPlayer()->GetRoleState(ERS_PK) || RoleMgr::Inst()->GetLocalPlayer()->GetRoleState(ERS_PKEX) )
		{
			StallMgr::Inst()->ShowStallErrorMsg(E_Stall_RoleState_NotPermit);
			return;
		}

		if (!StallMgr::Inst()->CanStall(1))
		{
			StallMgr::Inst()->ShowStallErrorMsg(E_Stall_Space_NotEnough);
			return;
		}

		tagNC_StallSetFinish m;
		m_pSession->Send(&m);
	}
}


DWORD StallShelfFrame::OnNetStallSetFinish( tagNS_StallSetFinish* pNetCmd, DWORD )
{
	if (pNetCmd->dwErrorCode == E_Success)
	{
		m_pBtnStall->SetText(g_StrTable[_T("StallButton_HideStall")]);
	}
	else
	{
		StallMgr::Inst()->ShowStallErrorMsg(pNetCmd->dwErrorCode);
	}
	return 0;
}

DWORD StallShelfFrame::OnNetStallSetGoods( tagNS_StallSetGoods* pNetCmd, DWORD )
{
	if (E_Success == pNetCmd->dwErrorCode)
	{
		m_pStcStallGoods[m_nSelGoods]->SetShangjia(true);

		tagTemporaryItem* pTemp = StallMgr::Inst()->GetItem(m_nSelGoods);
		if (P_VALID(pTemp))
		{
			pTemp->bShelf = true;
		}
	}
	else
	{
		StallMgr::Inst()->ShowStallErrorMsg(pNetCmd->dwErrorCode);
	}
	m_bSelLock = false;
	return 0;
}

DWORD StallShelfFrame::OnNetStallUnsetGoods( tagNS_StallUnsetGoods* pNetCmd, DWORD )
{
	if (E_Success == pNetCmd->dwErrorCode)
	{
		m_pStcStallGoods[m_nSelGoods]->SetShangjia(false);
		
		tagTemporaryItem* pTemp = StallMgr::Inst()->GetItem(m_nSelGoods);
		if (P_VALID(pTemp))
		{
			pTemp->bShelf	= false;
		}
	}
	else
	{
		StallMgr::Inst()->ShowStallErrorMsg(pNetCmd->dwErrorCode);
	}
	m_bSelLock = false;
	return 0;
}

DWORD StallShelfFrame::OnNetStallSetTitle( tagNS_StallSetTitle* pNetCmd, DWORD )
{
	if (E_Success == pNetCmd->dwErrorCode)
	{
		StallMgr::Inst()->SetStallName(RoleMgr::Inst()->GetLocalPlayerID(), m_pStcTitle->GetText().c_str(),true);
	}
	else
	{
		m_pStcTitle->SetText(StallMgr::Inst()->GetStallName(RoleMgr::Inst()->GetLocalPlayerID()));
		m_pStcTitle->SetRefresh(TRUE);
		/*StallMgr::Inst()->ShowStallErrorMsg(pNetCmd->dwErrorCode);*/
	}
	return 0;
}

DWORD StallShelfFrame::OnNetStallSetAdText( tagNS_StallSetAdText* pNetCmd, DWORD )
{
	if (E_Success == pNetCmd->dwErrorCode)
	{
		
	}
	else
	{
		/*StallMgr::Inst()->ShowStallErrorMsg(pNetCmd->dwErrorCode);*/
	}
	return 0;
}

DWORD StallShelfFrame::OnNetStallSetAdFlag( tagNS_StallSetAdFlag* pNetCmd, DWORD )
{
	/*if (E_Success == pNetCmd->dwErrorCode)
	{
		StallMgr::Inst()->SetStallAdFlag(m_pPbnAdFlag->GetState() == EGUIBS_PushDown);
	}
	else
	{
		StallMgr::Inst()->ShowStallErrorMsg(pNetCmd->dwErrorCode);
	}*/
	return 0;
}


DWORD StallShelfFrame::OnEventStallPriceInput( tagMsgMoneyBoxEvent* pGameEvent )
{
	if (pGameEvent->eResult == MBF_OK && pGameEvent->eMonetType == MBMT_Silver)
	{
		// 不能不输入价格
		if (pGameEvent->n64Silver < STALL_MIN_SOLD_SLIVER)
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("StallError_NoPriceForbid")]);
			return 0;	
		}
		
		// 发送游戏事件
		tagTemporaryItem* pTemp = StallMgr::Inst()->GetItem(m_nSelGoods);
		if (P_VALID(pTemp) && P_VALID(pTemp->pItem) && pTemp->n64ItemId == pTemp->pItem->GetItemId())
		{
			tagNC_StallSetGoods m;
			m.byIndex		= (BYTE)pTemp->n16Pos;
			m.n64Serial		= pTemp->n64ItemId;
			m.n64UnitPrice	= pGameEvent->n64Silver;
			m_pSession->Send(&m);

			pTemp->n64Price = pGameEvent->n64Silver;
		}
		
	}
	else
	{
		m_bSelLock = false;
	}
	return 0;
}

VOID StallShelfFrame::SendPriceInput( INT64 n64Default )
{
	if(GetObj("StallMoneyBox"))
		KillObj("StallMoneyBox");

	CreateObj("StallMoneyBox", "MsgMoneyInputBox");
	TObjRef<MsgMoneyInputBox>("StallMoneyBox")->Init(
		_T(""), g_StrTable[_T("StallMsg_PriceInputText")], _T("MsgBox_StallPrice"), 
		STALL_MAX_SOLD_SLIVER, GT_INVALID, n64Default, 0, MBMT_Silver);
}

VOID StallShelfFrame::OnStallAddItem( INT16 n16PosSrc, INT16 n16PosDst )
{
	// 如果该位置已经有物品，则不能替换
	if ( GT_VALID(n16PosDst) && P_VALID(m_pStcStallGoods[n16PosDst]->GetBtnData().GetData1()) )
		return;

	Item* pItem = ItemMgr::Inst()->GetPocketItem(n16PosSrc);
	if (P_VALID(pItem))
	{
        // 如果物品已绑定，不允许添加
        if (pItem->IsItemBinded())
        {
            CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildWareBinding")]);
            return;
        }

		AddGoods(pItem, n16PosDst);
	}
}

DWORD StallShelfFrame::OnEventStallAddItem( tagItemPosChangeExEvent* pGameEvent )
{
	if (pGameEvent->eTypeSrc == EICT_Bag)
	{
		OnStallAddItem(pGameEvent->n16IndexSrc);
	}
	return 0;
}

DWORD StallShelfFrame::OnEventStallNameInput( tagMsgInputBoxEvent* pGameEvent )
{
	if (MBF_OK == pGameEvent->eResult && MBIT_Name == pGameEvent->eInputType)
	{
		INT nLen = _tcslen(pGameEvent->strInputName.c_str());
		
		if (nLen >= STALL_MAX_TITLE_NUM )
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("StallInput_Name2Long")]);
			return 0;
		}
		// 检查摊位名是否合法
		if(E_Success != FilterData::Inst()->IsValidName(pGameEvent->strInputName.c_str(), STALL_MAX_TITLE_NUM))
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("StallInput_NameBad")]);
			return 0;
		}

		if (nLen > 0)
		{
			tagNC_StallSetTitle m;
			memcpy(m.szData, pGameEvent->strInputName.c_str(), nLen * sizeof(TCHAR));
			m.szData[nLen] = _T('\0');
			m_pSession->Send(&m);

			m_pStcTitle->SetText(pGameEvent->strInputName.c_str());
			m_pStcTitle->SetRefresh(TRUE);
		}
	}
	return 0;
}

DWORD StallShelfFrame::OnEventStallAdInput( tagMsgInputBoxEvent* pGameEvent )
{
	if (MBF_OK == pGameEvent->eResult && MBIT_Text == pGameEvent->eInputType)
	{
		INT nLen = _tcslen(pGameEvent->strInputName.c_str());
		
		if (nLen >= STALL_MAX_ADTEXT_NUM )
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("StallInput_Ad2Long")]);
			return 0;
		}
		// 屏蔽非法字符
		tstring strTemp = pGameEvent->strInputName;
		FilterData::Inst()->WordsFilteror(strTemp);

		if (nLen > 0)
		{
			tagNC_StallSetAdText m;
			memcpy(m.szData, strTemp.c_str(), nLen * sizeof(TCHAR));
			m.szData[nLen] = _T('\0');
			// 消息长度修正
			m.dwSize -= (STALL_MAX_ADTEXT_NUM - nLen - 1) * sizeof(TCHAR); 
			m_pSession->Send(&m);
		}
	}
	return 0;
}

BOOL StallShelfFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

	tagGameEvent event(_T("StallEvent_CloseWindow"), this);
	m_pMgr->SendEvent(&event);
	return TRUE;
}

VOID StallShelfFrame::DestroyMsgBox()
{
	if(P_VALID(GetObj("StallName")))
		KillObj("StallName");

	if(P_VALID(GetObj("StallAd")))
		KillObj("StallAd");

	if(GetObj("StallMoneyBox"))
		KillObj("StallMoneyBox");
}

DWORD StallShelfFrame::OnEventShowStar(tagStallStarEvent *pEvent)
{
	for (int i = 0; i < 10; ++i)
	{
		m_pIstStar[i]->SetInvisible(TRUE);
	}
	for (int i = 0; i < pEvent->nLevel; ++i)
	{
		m_pIstStar[i]->SetPic(_T("data\\ui\\Bag\\l_baitan-lv1.dds"));
		m_pIstStar[i]->SetInvisible(FALSE);
	}
	return 0;
}