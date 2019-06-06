#include "stdafx.h"
#include "PlayerTradeFrame.h"
#include "Container.h"
#include "ItemButton.h"
#include "GameClientDefine.h"
#include "CurrencyMgr.h"
#include "..\WorldDefine\msg_exchange.h"
#include "PlayerNameTab.h"
#include "..\WorldDefine\currency_define.h"
#include "ItemMgr.h"
#include "Item.h"
#include "ToolTipFrame.h"
#include "ToolTipCreator.h"
#include "ItemEvent.h"
#include "LockStatic.h"
#include "CombatSysUtil.h"
#include "RoleMgr.h"
#include "TradeFrame.h"

PlayerTradeFrame::PlayerTradeFrame():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndCaption = NULL;
	m_pStcBackSrc = NULL;
	m_pStcGoldSrc = NULL;
	m_pStcSilverSrc = NULL;
	m_pStcPlayerNameSrc = NULL;
	m_pStcLockSrc = NULL;	
	m_pStcBackDst = NULL;
	m_pStcGoldDst = NULL;
	m_pStcSilverDst = NULL;
	m_pStcPlayerNameDst = NULL;
	m_pStcLockDst = NULL;
	for (INT i=0; i<MAX_EXCHANGE_ITEM; ++i)
	{
		m_pItemSrc[i] = NULL;
		m_pItemDst[i] = NULL;
	}
	m_pBtnClose = NULL;
	m_pBtnExchange = NULL;
	m_pBtnOK = NULL;
	m_pBtnCancel = NULL;

	m_dwTargetID = GT_INVALID;
}

PlayerTradeFrame::~PlayerTradeFrame()
{
	
}

BOOL PlayerTradeFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
	
	m_pContainerSrc = new TemporaryContainer(MAX_EXCHANGE_ITEM);
	m_pContainerDst = new TemporaryContainer(MAX_EXCHANGE_ITEM);

	m_pCmdMgr->Register("NS_ExchangeAdd_2Src",	(NETMSGPROC)m_Trunk.sfp2(&PlayerTradeFrame::OnNetExchangeAdd2Src),	_T("NS_ExchangeAdd_2Src"));
	m_pCmdMgr->Register("NS_ExchangeAdd_2Dst",	(NETMSGPROC)m_Trunk.sfp2(&PlayerTradeFrame::OnNetExchangeAdd2Dst),	_T("NS_ExchangeAdd_2Dst"));
	m_pCmdMgr->Register("NS_ExchangeMoney",		(NETMSGPROC)m_Trunk.sfp2(&PlayerTradeFrame::OnNetExchangeMoney),	_T("NS_ExchangeMoney"));
	m_pCmdMgr->Register("NS_ExchangeDec",		(NETMSGPROC)m_Trunk.sfp2(&PlayerTradeFrame::OnNetExchangeDec),		_T("NS_ExchangeDec"));
	m_pCmdMgr->Register("NS_ExchangeLock",		(NETMSGPROC)m_Trunk.sfp2(&PlayerTradeFrame::OnNetExchangeLock),		_T("NS_ExchangeLock"));

	m_pMgr->RegisterEventHandle(_T("ItemEvent_Move2Trade"), (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerTradeFrame::OnEventItemPosChange));
	m_pMgr->RegisterEventHandle(_T("MsgBox_ExchangeMoney"), (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerTradeFrame::OnEventExchangeMoney));

	return bRet;
}

BOOL PlayerTradeFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	m_pCmdMgr->UnRegister("NS_ExchangeAdd_2Src",	(NETMSGPROC)m_Trunk.sfp2(&PlayerTradeFrame::OnNetExchangeAdd2Src));
	m_pCmdMgr->UnRegister("NS_ExchangeAdd_2Dst",	(NETMSGPROC)m_Trunk.sfp2(&PlayerTradeFrame::OnNetExchangeAdd2Dst));
	m_pCmdMgr->UnRegister("NS_ExchangeMoney",		(NETMSGPROC)m_Trunk.sfp2(&PlayerTradeFrame::OnNetExchangeMoney));
	m_pCmdMgr->UnRegister("NS_ExchangeDec",			(NETMSGPROC)m_Trunk.sfp2(&PlayerTradeFrame::OnNetExchangeDec));
	m_pCmdMgr->UnRegister("NS_ExchangeLock",		(NETMSGPROC)m_Trunk.sfp2(&PlayerTradeFrame::OnNetExchangeLock));

	m_pMgr->UnRegisterEventHandler(_T("ItemEvent_Move2Trade"), (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerTradeFrame::OnEventItemPosChange));
	m_pMgr->UnRegisterEventHandler(_T("MsgBox_ExchangeMoney"), (FRAMEEVENTPROC)m_Trunk.sfp1(&PlayerTradeFrame::OnEventExchangeMoney));

	for(INT16 i=0; i<MAX_EXCHANGE_ITEM; ++i)
	{
		tagTemporaryItem* pTemp = m_pContainerSrc->GetValue(i);
		if (P_VALID(pTemp) && P_VALID(pTemp->pItem))
		{
			pTemp->pItem->SetOperable(true);
		}
	}
	if(P_VALID(m_pContainerSrc))
	{
		m_pContainerSrc->Destroy();
		SAFE_DEL(m_pContainerSrc);
	}
	if(P_VALID(m_pContainerDst))
	{
		m_pContainerDst->Destroy();
		SAFE_DEL(m_pContainerDst);
	}
	
	Item* pItem = NULL;
	m_listItemDst.ResetIterator();
	while(m_listItemDst.PeekNext(pItem))
	{
		if(P_VALID(pItem))
		{
			SAFE_DEL(pItem);
		}
	}
	m_listItemDst.Clear();

	if(P_VALID(m_pWnd))
	{
		HideTip(m_pWnd);
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}

	DestroyMsgBox();

	return bRet;
}

BOOL PlayerTradeFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\exchange.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	for (INT i=0; i<MAX_EXCHANGE_ITEM; ++i)
	{
		CHAR szFullName[X_LONG_NAME] = {0};
		_snprintf(szFullName, sizeof(szFullName), "ec_win\\ec_backpic\\itembackpic1\\itempic%d\\item%d", i+1, i+1);
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "ItemButton");
		_snprintf(szFullName, sizeof(szFullName), "ec_win\\ec_backpic\\itembackpic2\\itempic%d\\item%d", i+1, i+1);
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "ItemButton");
	}
	m_pGUI->ChangeXml(&ele, "ec_win\\ec_backpic\\itembackpic1\\lock1", "ClassName", "LockStatic");
	m_pGUI->ChangeXml(&ele, "ec_win\\ec_backpic\\itembackpic2\\lock2", "ClassName", "LockStatic");

	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&PlayerTradeFrame::EventHandler));

	m_pWndCaption = (GUIStatic*)m_pWnd->GetChild(_T("ec_backpic"));

	// 创建按钮
	m_pBtnClose = (GUIButton*)m_pWndCaption->GetChild(_T("new"));
	m_pBtnExchange = (GUIButton*)m_pWndCaption->GetChild(_T("ready"));
	m_pBtnOK = (GUIButton*)m_pWndCaption->GetChild(_T("sure"));
	m_pBtnCancel = (GUIButton*)m_pWndCaption->GetChild(_T("ancle"));

	
	// 创建单元格
	m_pStcBackSrc = (GUIStatic*)m_pWndCaption->GetChild(_T("itembackpic2"));
	m_pStcBackDst = (GUIStatic*)m_pWndCaption->GetChild(_T("itembackpic1"));
	for (INT i = 0; i < MAX_EXCHANGE_ITEM; ++i)
	{
		TCHAR szFullName[X_LONG_NAME] = {0};
		_sntprintf(szFullName, sizeof(szFullName)/sizeof(TCHAR), _T("itempic%d\\item%d"), i+1, i+1);
		m_pItemSrc[i] = (ItemButton*)m_pStcBackSrc->GetChild(szFullName);
		m_pItemDst[i] = (ItemButton*)m_pStcBackDst->GetChild(szFullName);
		if (P_VALID(m_pItemSrc[i]))
			m_pItemSrc[i]->RefreshItem();
		if (P_VALID(m_pItemDst[i]))
			m_pItemDst[i]->RefreshItem();
	}
	
	// 创建金钱显示框
	m_pStcGoldDst = (GUIStatic*)m_pStcBackDst->GetChild(_T("num1"));
	m_pStcSilverDst = (GUIStatic*)m_pStcBackDst->GetChild(_T("num2"));
	m_pStcGoldSrc = (GUIStatic*)m_pStcBackSrc->GetChild(_T("putpic1"));
	m_pStcSilverSrc = (GUIStatic*)m_pStcBackSrc->GetChild(_T("putpic2"));

	// 创建锁定框
	m_pStcLockDst = (LockStatic*)m_pStcBackDst->GetChild(_T("lock1"));
	m_pStcLockSrc = (LockStatic*)m_pStcBackSrc->GetChild(_T("lock2"));

	// 创建玩家交易提示
	m_pStcPlayerNameDst = (GUIStatic*)m_pWndCaption->GetChild(_T("playername1"));
	m_pStcPlayerNameSrc = (GUIStatic*)m_pWndCaption->GetChild(_T("playername2"));
	
	LockDstControl(FALSE);
	LockSrcControl(FALSE);
	
	return TRUE;
}

VOID PlayerTradeFrame::Update()
{
	//more
	if (RoleMgr::Inst()->IsOutValidDist(m_dwTargetID))
		SendCancelExchange();

	GameFrame::Update();
}

BOOL PlayerTradeFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if(!P_VALID(pWnd))
		return FALSE;
	
	switch (pEvent->eEvent)
	{
	case EGUIE_DragStart:
		{
			if (pWnd == m_pStcSilverSrc || pWnd == m_pStcGoldSrc)
			{
				if(GetObj("ExchangeMoneyBox"))
					KillObj("ExchangeMoneyBox");

				CreateObj("ExchangeMoneyBox", "MsgMoneyInputBox");
				TObjRef<MsgMoneyInputBox>("ExchangeMoneyBox")->Init(
					_T(""), _T(""), _T("MsgBox_ExchangeMoney"), 
					CurrencyMgr::Inst()->GetBagSilver(),
					GT_INVALID, 0, 0, MBMT_Silver);
			}
		}
		break;
	case EGUIE_Click:
		{
			if(pWnd == m_pBtnClose || pWnd == m_pBtnCancel)
			{
				SendCancelExchange();	
			}
			else if(pWnd == m_pBtnOK)
			{
				SendLockExchange();
			}
			else if(pWnd == m_pBtnExchange)
			{
				SendVerifyExchange();
			}
			else
			{
				// 如果本地交易框已锁定，不能再操作物品
				if (!m_pStcLockSrc->IsInvisible())
					break;

				for(INT i=0; i<MAX_EXCHANGE_ITEM; ++i)
				{
					if(pWnd == m_pItemSrc[i])
					{
						//移出交易框
						OnItemDec((INT16)i);
						break;
					}
				}
			}
			
		}
		break;
	case EGUIE_EditBoxEnter:
		{
			/*if(pWnd == m_pEdtGoldSrc || pWnd == m_pEdtSilverSrc)
			{
				SendMoneyExchange();
			}*/
		}
		break;
	case EGUISCB_Drop:
		{
			// 如果本地交易框已锁定，不能再拖入物品
			if (!m_pStcLockSrc->IsInvisible())
				break;

			for(INT i=0; i<MAX_EXCHANGE_ITEM; ++i)
			{
				if(pWnd == m_pItemSrc[i])
				{
					//拖放到交易框
					OnItemAdd((INT16)LOWORD(pEvent->dwParam2), pEvent->dwParam3);
					break;
				}
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			for(INT i=0; i<MAX_EXCHANGE_ITEM; ++i)
			{
				if(pWnd == m_pItemSrc[i])
				{
					tagTemporaryItem* pItem = m_pContainerSrc->GetValue(i);
					if (P_VALID(pItem) && P_VALID(pItem->pItem))
					{
						tstring strTip = ToolTipCreator::Inst()->GetItemTips(pItem->pItem);
						ShowTip(pWnd, strTip.c_str());
					}
					break;
				}
				if(pWnd == m_pItemDst[i])
				{
					tagTemporaryItem* pItem = m_pContainerDst->GetValue(i);
					if (P_VALID(pItem) && P_VALID(pItem->pItem))
					{
						tstring strTip = ToolTipCreator::Inst()->GetItemTips(pItem->pItem);
						ShowTip(pWnd, strTip.c_str());
					}
					break;
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(NULL, NULL);
		}
		break;
	}
	return TRUE;
}

VOID PlayerTradeFrame::LockSrcControl(BOOL bLock)
{
	m_pStcLockSrc->SetInvisible(!bLock);
	m_pBtnOK->SetEnable(bLock ? false : true);
	m_pBtnOK->SetRefresh(true);
	
	SetExchangeEnable();
}

VOID PlayerTradeFrame::LockDstControl(BOOL bLock)
{
	m_pStcLockDst->SetInvisible(!bLock);

	SetExchangeEnable();
}

VOID PlayerTradeFrame::SetTargetPlayerID( DWORD dwTargetID )
{
	m_dwTargetID = dwTargetID;
	// 设置对方玩家姓名
	LPCTSTR szName = PlayerNameTab::Inst()->FindNameByID(dwTargetID);
	m_pStcPlayerNameDst->SetText(szName);
	m_pStcPlayerNameDst->SetRefresh(true);
	// 设置自己姓名
	LPCTSTR szMine = PlayerNameTab::Inst()->FindNameByID(RoleMgr::Inst()->GetLocalPlayerID());
	m_pStcPlayerNameSrc->SetText(szMine);
	m_pStcPlayerNameSrc->SetRefresh(true);
}

VOID PlayerTradeFrame::SetTitle()
{
	TCHAR szTmp[X_LONG_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("")], PlayerNameTab::Inst()->FindNameByID(m_dwTargetID));
	m_pWndCaption->SetText(szTmp);
	m_pWndCaption->SetRefresh();
}

VOID PlayerTradeFrame::SetMoneySrc( INT64 n64Silver )
{
	TCHAR szTmp[X_SHORT_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), MSilver2DBGold(n64Silver));
	m_pStcGoldSrc->SetText(szTmp);
	m_pStcGoldSrc->SetRefresh(TRUE);
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), MSilver2DBSilver(n64Silver));
	m_pStcSilverSrc->SetText(szTmp);
	m_pStcSilverSrc->SetRefresh(TRUE);

	if(!m_pStcLockDst->IsInvisible())
		LockDstControl(FALSE);
}

VOID PlayerTradeFrame::SetMoneyDst( INT64 n64Silver )
{
	TCHAR szTmp[X_SHORT_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), MSilver2DBGold(n64Silver));
	m_pStcGoldDst->SetText(szTmp);
	m_pStcGoldDst->SetRefresh(TRUE);
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), MSilver2DBSilver(n64Silver));
	m_pStcSilverDst->SetText(szTmp);
	m_pStcSilverDst->SetRefresh(TRUE);

	if(!m_pStcLockSrc->IsInvisible())
		LockSrcControl(FALSE);
}

VOID PlayerTradeFrame::SendCancelExchange()
{	
	tagNC_ExchangeCancel m;
	m_pSession->Send(&m);
}

VOID PlayerTradeFrame::SendVerifyExchange()
{
	tagNC_ExchangeVerify m;
	m_pSession->Send(&m);

	m_pBtnExchange->SetEnable(false);
	m_pBtnExchange->SetRefresh(true);
}

VOID PlayerTradeFrame::SendLockExchange()
{
	tagNC_ExchangeLock m;
	m_pSession->Send(&m);
}

VOID PlayerTradeFrame::SendAddItemExchange(Item* pItem)
{
	tagNC_ExchangeAdd m;
	m.n64Serial = pItem->GetItemId();
	m_pSession->Send(&m);
}

VOID PlayerTradeFrame::SendDecItemExchange(Item* pItem)
{
	tagNC_ExchangeDec m;
	m.n64Serial = pItem->GetItemId();
	m_pSession->Send(&m);
}


DWORD PlayerTradeFrame::OnNetExchangeAdd2Src( tagNS_ExchangeAdd_2Src* pNetCmd, DWORD )
{
	if( pNetCmd->dwErrorCode == E_Success )
	{
		Item* pItem = ItemMgr::Inst()->GetPocketItemByID(pNetCmd->n64Serial);
		if (P_VALID(pItem))
		{
			tagTemporaryItem* pTemp = new tagTemporaryItem(pItem, (INT16)pNetCmd->nIndex);
			m_pContainerSrc->Add(pTemp);
			ReflashItemSrc(pTemp->n16Pos);

			pItem->SetOperable(false);

			if(!m_pStcLockDst->IsInvisible())
				LockDstControl(FALSE);
		}
	}
	else
	{
		ShowExchangeErrorMsg(pNetCmd->dwErrorCode);
	}
	return 0;
}

DWORD PlayerTradeFrame::OnNetExchangeAdd2Dst( tagNS_ExchangeAdd_2Dst* pNetCmd, DWORD )
{
	tagItem item;
	memcpy(&item, pNetCmd->byData, sizeof(tagItem));
	if(MIsEquipment(item.dwTypeID))
	{
		tagEquip equip;
		memcpy(&equip, pNetCmd->byData, sizeof(tagEquip));
		Equipment* pEquip = new Equipment(equip);
		tagTemporaryItem* pTemp = new tagTemporaryItem(pEquip, (INT16)pNetCmd->nIndex);
		m_pContainerDst->Add(pTemp);
		m_listItemDst.PushBack(pEquip);
	}
	else
	{
		Item* pItem = new Item(item);
		tagTemporaryItem* pTemp = new tagTemporaryItem(pItem, (INT16)pNetCmd->nIndex);
		m_pContainerDst->Add(pTemp);
		m_listItemDst.PushBack(pItem);
	}
	ReflashItemDst((INT16)pNetCmd->nIndex);

	if(!m_pStcLockSrc->IsInvisible())
		LockSrcControl(FALSE);
	
	return 0;
}

DWORD PlayerTradeFrame::OnNetExchangeMoney( tagNS_ExchangeMoney* pNetCmd, DWORD )
{
	if (pNetCmd->dwErrorCode == E_Success)
	{
		if(pNetCmd->dwSrcID == m_dwTargetID)
		{
			SetMoneyDst(pNetCmd->n64Money);
		}
		else
		{
			SetMoneySrc(pNetCmd->n64Money);
		}
	}
	else
	{
		ShowExchangeErrorMsg(pNetCmd->dwErrorCode);
	}
	return 0;
}

DWORD PlayerTradeFrame::OnNetExchangeLock( tagNS_ExchangeLock* pNetCmd, DWORD )
{
	if (pNetCmd->dwErrorCode == E_Success)
	{
		if (pNetCmd->dwSrcID == m_dwTargetID)
		{
			LockDstControl(true);
		}
		else
		{
			LockSrcControl(true);
		}
	}
	else
	{
		ShowExchangeErrorMsg(pNetCmd->dwErrorCode);
	}
	return 0;
}

DWORD PlayerTradeFrame::OnNetExchangeDec( tagNS_ExchangeDec* pNetCmd, DWORD )
{
	if (pNetCmd->dwErrorCode == E_Success)
	{
		if (pNetCmd->dwSrcID == m_dwTargetID)
		{
			tagTemporaryItem* pItem = m_pContainerDst->RemoveById(pNetCmd->n64Serial);
			if(P_VALID(pItem))
			{
				ReflashItemDst(pItem->n16Pos);
				SAFE_DEL(pItem);
			}

			if(!m_pStcLockSrc->IsInvisible())
				LockSrcControl(FALSE);

		}
		else if (pNetCmd->dwDstID == m_dwTargetID)
		{
			tagTemporaryItem* pItem = m_pContainerSrc->RemoveById(pNetCmd->n64Serial);
			if(P_VALID(pItem) && P_VALID(pItem->pItem))
			{
				pItem->pItem->SetOperable(true);
				ReflashItemSrc(pItem->n16Pos);
				SAFE_DEL(pItem);
			}

			if(!m_pStcLockDst->IsInvisible())
				LockDstControl(FALSE);
		}
	}
	else
	{
		ShowExchangeErrorMsg(pNetCmd->dwErrorCode);
	}
	return 0;
}

VOID PlayerTradeFrame::ReflashItemSrc( INT16 n16Pos )
{
	tagTemporaryItem* pTemp = m_pContainerSrc->GetValue(n16Pos);
	if (P_VALID(pTemp))
	{
		m_pItemSrc[n16Pos]->RefreshItem(pTemp->pItem->GetItemTypeID(),pTemp->pItem->GetItemQuantity(), pTemp->pItem->GetItemQuality());
	}
	else
	{
		m_pItemSrc[n16Pos]->RefreshItem();
	}
	
}

VOID PlayerTradeFrame::ReflashItemDst( INT16 n16Pos )
{
	tagTemporaryItem* pTemp = m_pContainerDst->GetValue(n16Pos);
	if (P_VALID(pTemp))
	{
		m_pItemDst[n16Pos]->RefreshItem(pTemp->pItem->GetItemTypeID(),pTemp->pItem->GetItemQuantity(), pTemp->pItem->GetItemQuality());
	}
	else
	{
		m_pItemDst[n16Pos]->RefreshItem();
	}
}

DWORD PlayerTradeFrame::OnEventItemPosChange( tagItemPosChangeExEvent* pGameEvent )
{
	if(pGameEvent->eTypeSrc == EICT_Bag)
	{
		OnItemAdd(pGameEvent->n16IndexSrc, pGameEvent->dwTypeIDSrc);
	}
	return 0;
}


VOID PlayerTradeFrame::OnItemAdd( INT16 n16Pos, DWORD dwTypeID )
{
	Item* pItem = ItemMgr::Inst()->GetPocketItem(n16Pos);
	if (P_VALID(pItem) && pItem->GetItemTypeID() == dwTypeID)
	{
		if(pItem->IsTradeable())
		{
			SendAddItemExchange(pItem);
		}
		else
		{
			// 发送错误信息
			ShowExchangeErrorMsg(E_Exchange_ItemCanNot_Exchange);
		}

	}
}

VOID PlayerTradeFrame::OnItemDec( INT16 n16Pos )
{
	tagTemporaryItem* pItem = m_pContainerSrc->GetValue(n16Pos);
	if (P_VALID(pItem) && P_VALID(pItem->pItem))
	{
		SendDecItemExchange(pItem->pItem);
	}
}

DWORD PlayerTradeFrame::OnEventExchangeMoney( tagMsgMoneyBoxEvent* pGameEvent )
{
	if (MBF_OK == pGameEvent->eResult)
	{
		if (pGameEvent->n64Silver > 0)
		{
			tagNC_ExchangeMoney m;
			m.n64Money = pGameEvent->n64Silver;
			m_pSession->Send(&m);
		}
	}
	return 0;
}

VOID PlayerTradeFrame::SetExchangeEnable()
{
	bool bEnable = !(m_pStcLockSrc->IsInvisible() || m_pStcLockDst->IsInvisible());
	m_pBtnExchange->SetEnable(bEnable);
	m_pBtnExchange->SetRefresh(true);
}

VOID PlayerTradeFrame::ShowExchangeErrorMsg( DWORD dwErrorCode )
{
	TradeFrame* pTrade = (TradeFrame*)m_pFather;
	pTrade->ShowExchangeErrorMsg(dwErrorCode);
}

BOOL PlayerTradeFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;
	// 发送交易取消
	SendCancelExchange();
	return TRUE;
}

VOID PlayerTradeFrame::DestroyMsgBox()
{
	if(GetObj("ExchangeMoneyBox"))
		KillObj("ExchangeMoneyBox");
}