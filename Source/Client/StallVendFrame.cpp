#include "StdAfx.h"
#include "StallVendFrame.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "IconStatic.h"
#include "ItemButton.h"
#include "Container.h"
#include "StallMgr.h"
#include "ItemMgr.h"
#include "CurrencyMgr.h"
#include "..\WorldDefine\shop_define.h"
#include "..\WorldDefine\ItemDefine.h"
#include "..\WorldDefine\msg_stall.h"

StallVendFrame::StallVendFrame(void) : m_Trunk(this)
{
	m_pWnd			= NULL;
	m_pWndCaption	= NULL;
	m_pBtnBuy		= NULL;
	m_pBtnCancel	= NULL;
	m_pBtnPageup	= NULL;
	m_pBtnPagedown	= NULL;
	m_pBtnExit		= NULL;
	m_pStcTitle		= NULL;
	//m_pStcSignboard	= NULL;
	m_pStcPage		= NULL;

	m_pStcStallSel.clear();
	m_pStcStallGoods.clear();

	m_pContainerStall = NULL;
	m_listStallItem.Clear();

	m_dwStallRoleID	= GT_INVALID;
	m_nSelGoods		= GT_INVALID;
	m_bSelLock		= false;

	ZeroMemory(m_pIstStar, sizeof(m_pIstStar));
}

StallVendFrame::~StallVendFrame(void)
{
}

BOOL StallVendFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	m_pContainerStall = new TemporaryContainer(STALL_MAX_DISPLAY);

	m_pCmdMgr->Register("NS_StallGet",			(NETMSGPROC)m_Trunk.sfp2(&StallVendFrame::OnNetStallGet),			_T("NS_StallGet"));
	m_pCmdMgr->Register("NS_StallBuy",			(NETMSGPROC)m_Trunk.sfp2(&StallVendFrame::OnNetStallBuy),			_T("NS_StallBuy"));
	m_pCmdMgr->Register("NS_StallBuyRefresh",	(NETMSGPROC)m_Trunk.sfp2(&StallVendFrame::OnNetStallBuyRefresh),	_T("NS_StallBuyRefresh"));
	m_pCmdMgr->Register("NS_VIPStallBuy",		(NETMSGPROC)m_Trunk.sfp2(&StallVendFrame::OnNS_VIPStallBuy),		_T("NS_VIPStallBuy"));


	m_pMgr->RegisterEventHandle(_T("MsgBox_StallBuyNumber"), (FRAMEEVENTPROC)m_Trunk.sfp1(&StallVendFrame::OnEventStallBuyNumber));
	m_pMgr->RegisterEventHandle(_T("StallStar"), (FRAMEEVENTPROC)m_Trunk.sfp1(&StallVendFrame::OnEventShowStar));
	

	return bRet;
}

BOOL StallVendFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	m_pCmdMgr->UnRegister("NS_StallGet",		(NETMSGPROC)m_Trunk.sfp2(&StallVendFrame::OnNetStallGet));
	m_pCmdMgr->UnRegister("NS_StallBuy",		(NETMSGPROC)m_Trunk.sfp2(&StallVendFrame::OnNetStallBuy));
	m_pCmdMgr->UnRegister("NS_StallBuyRefresh",	(NETMSGPROC)m_Trunk.sfp2(&StallVendFrame::OnNetStallBuyRefresh));
	m_pCmdMgr->UnRegister("NS_VIPStallBuy",		(NETMSGPROC)m_Trunk.sfp2(&StallVendFrame::OnNS_VIPStallBuy));


	m_pMgr->UnRegisterEventHandler(_T("MsgBox_StallBuyNumber"), (FRAMEEVENTPROC)m_Trunk.sfp1(&StallVendFrame::OnEventStallBuyNumber));
	m_pMgr->UnRegisterEventHandler(_T("StallStar"), (FRAMEEVENTPROC)m_Trunk.sfp1(&StallVendFrame::OnEventShowStar));

	if (P_VALID(m_pWnd))
	{
		HideTip(m_pWnd);
		m_pGUI->AddToDestroyList(m_pWnd);
	}
	
	if (P_VALID(m_pContainerStall))
	{
		m_pContainerStall->Destroy();
		SAFE_DEL(m_pContainerStall);
	}

	Item* pItem = NULL;
	m_listStallItem.ResetIterator();
	while (m_listStallItem.PeekNext(pItem))
	{
		SAFE_DEL(pItem);
	}
	m_listStallItem.Clear();

	DestroyMsgBox();

	return bRet;
}

BOOL StallVendFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\store2.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	for(INT i=0; i<STALL_MAX_DISPLAY; ++i)
	{
		CHAR szTmp[X_LONG_NAME] = {0};
		_snprintf(szTmp, sizeof(szTmp), "storewin2\\storepic2\\storeback\\store%d", i+1);
		m_pGUI->ChangeXml(&element, szTmp, "ClassName", "IconStatic");

		_snprintf(szTmp, sizeof(szTmp), "storewin2\\storepic2\\storeback\\store%d\\item%d", i+1, i+1);
		m_pGUI->ChangeXml(&element, szTmp, "ClassName", "ItemButton");
	}
	
	for (int i = 0; i < 10; ++i)
	{
		CHAR szTmp[X_LONG_NAME] = {0};
		_snprintf(szTmp, sizeof(szTmp), "storewin2\\storepic2\\img%d", i + 1);
		m_pGUI->ChangeXml(&element, szTmp, "ClassName", "IconStatic");
	}


	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), (VUIEVENTHANDLE)m_Trunk.sfp1(&StallVendFrame::EventHandler));

	m_pWndCaption	= m_pWnd->GetChild(_T("storepic2"));
	m_pBtnCancel	= (GUIButton*)m_pWndCaption->GetChild(_T("close"));
	m_pBtnBuy		= (GUIButton*)m_pWndCaption->GetChild(_T("advbutt"));
	m_pBtnExit		= (GUIButton*)m_pWndCaption->GetChild(_T("closebutt"));
	m_pBtnPageup	= (GUIButton*)m_pWndCaption->GetChild(_T("back"));
	m_pBtnPagedown	= (GUIButton*)m_pWndCaption->GetChild(_T("next"));
	m_pStcPage		= (GUIStatic*)m_pWndCaption->GetChild(_T("word3"));
	m_pStcTitle		= (GUIStatic*)m_pWndCaption->GetChild(_T("storename"));
	//m_pStcSignboard = (GUIStaticEx*)m_pWndCaption->GetChild(_T("gradepic"));

	for(INT i=0; i<STALL_MAX_DISPLAY; ++i)
	{
		TCHAR szTemp[X_SHORT_NAME] = {0};

		_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("storeback\\store%d"), i+1);
		IconStatic* pIconStatic = (IconStatic*)m_pWndCaption->GetChild(szTemp);
		m_pStcStallSel.push_back(pIconStatic);

		_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), _T("item%d"), i+1);
		ItemButton* pItemButton = (ItemButton*)pIconStatic->GetChild(szTemp);
		m_pStcStallGoods.push_back(pItemButton);
		pItemButton->RefreshItem();
	}

	for (int i = 0; i < 10; ++i)
	{
		TCHAR szTmp[256] = {0};
		_sntprintf(szTmp, 256, _T("storepic2\\img%d"), i + 1);
		m_pIstStar[i] = (IconStatic*)m_pWnd->GetChild(szTmp);
		m_pIstStar[i]->SetInvisible(TRUE);
	}
	


	m_pBtnPageup->SetEnable(FALSE);
	m_pBtnPagedown->SetEnable(FALSE);

	return TRUE;
}

DWORD StallVendFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnExit)//退出摊位
			{
				m_pMgr->SendEvent(&tagGameEvent(_T("StallEvent_CloseVend"), this));
				
			}
			else if (pWnd == m_pBtnPageup)
			{
				//PageUp();
			}
			else if (pWnd == m_pBtnPagedown)
			{
				//PageDown();
			}
			else if (pWnd == m_pBtnBuy)
			{
				BuySelGoods((GetKeyState(VK_SHIFT)&0x80)!=0);
			}
			else if (pWnd == m_pBtnCancel)//退出摊位
			{
				m_pMgr->SendEvent(&tagGameEvent(_T("StallEvent_CloseVend"), this));
			}
			else if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)//右键点击货架中的物品
			{
				for (INT16 i = 0; i < STALL_MAX_DISPLAY; ++i)
				{
					if (pWnd == m_pStcStallGoods[i])
					{
						SelGoods(i, TRUE);
						BuySelGoods((GetKeyState(VK_SHIFT)&0x80)!=0);
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
	default :
		break;
	}
	return 0;
}

VOID StallVendFrame::SelGoods( INT16 nIndex, BOOL bForce )
{
	if (m_bSelLock)
		return;

	if (nIndex >= 0 && nIndex < STALL_MAX_DISPLAY)
	{
		if (bForce || nIndex != m_nSelGoods)
		{
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

VOID StallVendFrame::ShowGoodsTip( INT16 nIndex )
{
	tagTemporaryItem* pTemporary = m_pContainerStall->GetValue(nIndex);
	if (P_VALID(pTemporary) && P_VALID(pTemporary->pItem))
	{
		tstring strTip = ToolTipCreator::Inst()->GetItemTips(pTemporary->pItem);
		tagItemCost cost;
		cost.n64CostSilver = pTemporary->n64Price;
		tstring strTipEx= ToolTipCreator::Inst()->GetStallPriceTips(&cost, pTemporary->pItem->GetItemQuantity());
		ShowTip(m_pStcStallGoods[nIndex], strTip.c_str(), strTipEx.c_str());
	}
}

DWORD StallVendFrame::OnNetStallGet( tagNS_StallGet* pNetCmd, DWORD )
{
	if (pNetCmd->dwErrorCode == E_Success)
	{
		m_pContainerStall->Destroy();

		Item* pItem = NULL;
		m_listStallItem.ResetIterator();
		while (m_listStallItem.PeekNext(pItem))
		{
			SAFE_DEL(pItem);
		}
		m_listStallItem.Clear();

		m_dwStallRoleID = pNetCmd->dwStallRoleID;
		m_pStcTitle->SetText(StallMgr::Inst()->GetStallName(m_dwStallRoleID));
		// 获取售出的商品
		for (INT i=0,pos=0; i<pNetCmd->byNum; ++i)
		{
			tagMsgStallGoods goods;
			memcpy(&goods, pNetCmd->byData+pos, sizeof(tagMsgStallGoods));
			pos += sizeof(tagMsgStallGoods)-1;
			if(goods.byItem == 0)
			{
				tagEquip equip;
				memcpy(&equip, pNetCmd->byData+pos, sizeof(tagEquip));
				pos += sizeof(tagEquip);
				Equipment* pEquip = new Equipment(equip);
				AddItem(pEquip, goods.n64UnitPrice, goods.byIndex);
			}
			else
			{
				tagItem item;
				memcpy(&item, pNetCmd->byData+pos, sizeof(tagItem));
				pos += sizeof(tagItem);
				Item* pItem = new Item(item);
				AddItem(pItem, goods.n64UnitPrice, goods.byIndex);
			}
		}
	}
	else
	{
		StallMgr::Inst()->ShowStallErrorMsg(pNetCmd->dwErrorCode);
	}
	return 0;
}

DWORD StallVendFrame::OnNetStallBuy( tagNS_StallBuy* pNetCmd, DWORD )
{
	if (pNetCmd->dwErrorCode == E_Success)
	{

	}
	else
	{
		StallMgr::Inst()->ShowStallErrorMsg(pNetCmd->dwErrorCode);
	}
	m_bSelLock = false;
	return 0;
}

DWORD StallVendFrame::OnNetStallBuyRefresh( tagNS_StallBuyRefresh* pNetCmd, DWORD )
{
	if (pNetCmd->dwStallRoleID == m_dwStallRoleID)
	{
		tagTemporaryItem* pTemp = m_pContainerStall->GetValue(pNetCmd->byIndex);
		if (P_VALID(pTemp) && P_VALID(pTemp->pItem))
		{
			if (pNetCmd->n16Num > 0)
			{
				pTemp->pItem->SetItemQuantity(pNetCmd->n16Num);
				UpdateItem(pTemp->pItem, pTemp->n64Price, (INT16)pNetCmd->byIndex);
			}
			else
			{
				DelItem((INT16)pNetCmd->byIndex);
			}
			
		}
	}
	return 0;
}

VOID StallVendFrame::AddItem( Item* pItem, INT64 n64Price, INT16 nIndex )
{
	if (P_VALID(pItem))
	{
		tagTemporaryItem* pTemp = new tagTemporaryItem(pItem, nIndex);
		if (!m_pContainerStall->Add(pTemp))
		{
			SAFE_DEL(pItem);
			SAFE_DEL(pTemp);
			return;
		}
		pTemp->n64Price = n64Price;
		pTemp->bShelf	= true;
		m_listStallItem.PushBack(pItem);
		m_pStcStallGoods[nIndex]->RefreshItem(pItem->GetItemTypeID(), pItem->GetItemQuantity(), pItem->GetItemQuality());
	}
}


VOID StallVendFrame::DelItem( INT16 nIndex )
{
	tagTemporaryItem* pTemp = m_pContainerStall->Remove(nIndex);
	if (P_VALID(pTemp) && P_VALID(pTemp->pItem))
	{
		if (m_listStallItem.Erase(pTemp->pItem))
		{
			SAFE_DEL(pTemp->pItem);
		}
		SAFE_DEL(pTemp);

		m_pStcStallGoods[nIndex]->RefreshItem();
		// 如果选中则取消
		if (m_nSelGoods == nIndex)
		{
			SelGoods(nIndex);
		}
	}
}

VOID StallVendFrame::UpdateItem( Item* pItem, INT64 n64Price, INT16 nIndex )
{
	tagTemporaryItem* pTemp = m_pContainerStall->GetValue(nIndex);
	if (P_VALID(pTemp) && P_VALID(pItem))
	{
		pTemp->pItem		= pItem;
		pTemp->n64Price		= n64Price;
		pTemp->n64ItemId	= pItem->GetItemId();
		m_pStcStallGoods[nIndex]->RefreshItem(pItem->GetItemTypeID(), pItem->GetItemQuantity(), pItem->GetItemQuality());
	}
}

VOID StallVendFrame::BuySelGoods( bool bShift )
{
	tagTemporaryItem* pTemp = m_pContainerStall->GetValue(m_nSelGoods);
	if (P_VALID(pTemp) && P_VALID(pTemp->pItem) && pTemp->pItem->IsItem())
	{
		// 锁住选择
		m_bSelLock = true;

		INT nMaxCanBuy = (INT)(CurrencyMgr::Inst()->GetBagSilver() / pTemp->n64Price);
		nMaxCanBuy = min(nMaxCanBuy, pTemp->pItem->GetItemQuantity());
		if (nMaxCanBuy <= 0)// 如果一个也买不起则直接返回
		{
			StallMgr::Inst()->ShowStallErrorMsg(E_Stall_CustomerMoney_NotEnough);
			m_bSelLock = false;
			return;
		}

		// 如果物品对于一个且按下shift，则可购买多个
		if (bShift && pTemp->pItem->GetItemQuantity() > 1)
		{
			if(P_VALID(GetObj("StallBuyNumber")))
				KillObj("StallBuyNumber");

			CreateObj("StallBuyNumber", "MsgInputBox");
			TObjRef<MsgInputBox>("StallBuyNumber")->Init(
				_T(""), g_StrTable[_T("MsgBoxText_InputBuyNumber")], _T("MsgBox_StallBuyNumber"), nMaxCanBuy );
		}
		else
		{
			BuyGoods(pTemp);
		}
	}
}

VOID StallVendFrame::BuyGoods( tagTemporaryItem* pTemporary, INT16 n16Quantity /*= 1*/ )
{
	if(P_VALID(pTemporary))
	{
		if( StallMgr::Inst()->GetOperateStallType() == StallMgr::EOperateStallType_Normal )
		{
			tagNC_StallBuy m;
			m.byIndex		= (BYTE)pTemporary->n16Pos;
			m.dwStallRoleID	= m_dwStallRoleID;
			m.n16Num		= n16Quantity;
			m.n64Serial		= pTemporary->n64ItemId;
			m.n64UnitPrice	= pTemporary->n64Price;
			m_pSession->Send(&m);
			m_bSelLock = FALSE;
		}
		else if( StallMgr::Inst()->GetOperateStallType() == StallMgr::EOperateStallType_Vip )
		{
			tagNC_VIPStallBuy msg;
			msg.byIndex			= (BYTE)pTemporary->n16Pos;
			msg.dwStallRoleID	= m_dwStallRoleID;
			msg.n16Num			= n16Quantity;
			msg.n64Serial		= pTemporary->n64ItemId;
			msg.n64UnitPrice	= pTemporary->n64Price;
			m_pSession->Send(&msg);
			m_bSelLock = FALSE;
		}
		
	}
}

DWORD StallVendFrame::OnEventStallBuyNumber( tagMsgInputBoxEvent* pGameEvent )
{
	if (pGameEvent->eResult == MBF_OK && pGameEvent->eInputType == MBIT_Number)
	{
		tagTemporaryItem* pTemp = m_pContainerStall->GetValue(m_nSelGoods);
		BuyGoods(pTemp, pGameEvent->nInputNum);
	}
	else
	{
		m_bSelLock = false;
	}
	return 0;
}

VOID StallVendFrame::Update()
{
	if(StallMgr::Inst()->IsOutValidDist())
		m_pMgr->SendEvent(&tagGameEvent(_T("StallEvent_CloseVend"), this));
	
	GameFrame::Update();
}

BOOL StallVendFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

	m_pMgr->SendEvent(&tagGameEvent(_T("StallEvent_CloseVend"), this));
	return TRUE;
}

VOID StallVendFrame::DestroyMsgBox()
{
	if(P_VALID(GetObj("StallBuyNumber")))
		KillObj("StallBuyNumber");
}

DWORD StallVendFrame::OnNS_VIPStallBuy( tagNS_VIPStallBuy* pMsg, DWORD )
{
	if (pMsg->dwErrorCode == E_Success)
	{

	}
	else
	{
		StallMgr::Inst()->ShowStallErrorMsg(pMsg->dwErrorCode);
	}
	m_bSelLock = false;
	return 0;
}

DWORD StallVendFrame::OnEventShowStar(tagStallStarEvent *pEvent)
{
	//TCHAR szBuffer[128];
	//_sntprintf(szBuffer, 128, _T("StallStar%d"), pEvent->nLevel);
	//m_pStcSignboard->SetText(g_StrTable[szBuffer]);
	//m_pStcSignboard->SetRefresh(TRUE);
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