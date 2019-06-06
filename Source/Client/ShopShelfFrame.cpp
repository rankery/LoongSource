#include "stdafx.h"
#include "ShopShelfFrame.h"
#include "ItemMgr.h"
#include "Container.h"
#include "ItemButton.h"
#include "QuestMgr.h"
#include "CreatureData.h"
#include "ShopProtoData.h"
#include "ItemProtoData.h"
#include "NetSession.h"
#include "DragBtn.h"
#include "ItemEvent.h"
#include "Item.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "CombatSysUtil.h"
#include "RoleMgr.h"
#include "CurrencyMgr.h"
#include "GuildMgr.h"
#include "ReputeMgr.h"
#include "..\WorldDefine\msg_shop.h"
#include "TreasureMgr.h"


#include "AudioSys.h"
ShopShelfFrame::ShopShelfFrame():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndCaption = NULL;
	m_pBtnExit = NULL;
	m_pBtnClose = NULL;
	
	m_nCurMenu = 0;
	for (INT i = 0; i < MAX_SHOP_SHELF_NUM; i++)
	{
		m_pPbnMenu[i] = NULL;
		m_nCurPage[i] = 0;
		m_nCurMaxPage[i] = 0;
	}
	
	for (INT i = 0; i < MAX_SHOP_ONE_SHELF; i++)
	{
		m_pUnitItem[i] = NULL;
	}
	
	m_pBtnPageUp = NULL;
	m_pBtnPageDown = NULL;
	m_pStcPage = NULL;
	m_pStcTitle = NULL;
	m_pSellItem = NULL;
	m_dwShopNpcID = GT_INVALID;
	m_nClickKey = GT_INVALID;
	m_nDragPos = GT_INVALID;
	m_nFirstBlank = GT_INVALID;
}

ShopShelfFrame::~ShopShelfFrame(void)
{

}

BOOL ShopShelfFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam );

	m_pMgr->RegisterEventHandle(_T("ItemEvent_Move2Shop"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ShopShelfFrame::OnEventItemPosChange));
	m_pMgr->RegisterEventHandle(_T("MsgBox_SellCheck"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ShopShelfFrame::OnEventCheckSellItem));
	m_pMgr->RegisterEventHandle(_T("MsgBox_GoodsNumber"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ShopShelfFrame::OnEventClickGoodsInput));
	m_pMgr->RegisterEventHandle(_T("MsgBox_ShopDragNumber"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ShopShelfFrame::OnEventDragGoodsInput));
	m_pMgr->RegisterEventHandle(_T("ItemEvent_Buy"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ShopShelfFrame::OnEventBuyGoods));

	m_pCmdMgr->Register("NS_FeedbackFromShop",	(NETMSGPROC)m_Trunk.sfp2(&ShopShelfFrame::OnNetFeedbackFromShop),	_T("NS_FeedbackFromShop"));
	m_pCmdMgr->Register("NS_GetShopItems",		(NETMSGPROC)m_Trunk.sfp2(&ShopShelfFrame::OnNetGetShopItems),		_T("NS_GetShopItems"));
	m_pCmdMgr->Register("NS_GetShopEquips",		(NETMSGPROC)m_Trunk.sfp2(&ShopShelfFrame::OnNetGetShopEquips),		_T("NS_GetShopEquips"));
	m_pCmdMgr->Register("NS_BuyShopItem",		(NETMSGPROC)m_Trunk.sfp2(&ShopShelfFrame::OnNetBuyShopItem),		_T("NS_BuyShopItem"));
	m_pCmdMgr->Register("NS_BuyShopEquip",		(NETMSGPROC)m_Trunk.sfp2(&ShopShelfFrame::OnNetBuyShopEquip),		_T("NS_BuyShopEquip"));


	return bRet;
}

BOOL ShopShelfFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();
	
	m_pMgr->UnRegisterEventHandler(_T("ItemEvent_Move2Shop"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ShopShelfFrame::OnEventItemPosChange));
	m_pMgr->UnRegisterEventHandler(_T("MsgBox_SellCheck"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ShopShelfFrame::OnEventCheckSellItem));
	m_pMgr->UnRegisterEventHandler(_T("MsgBox_GoodsNumber"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ShopShelfFrame::OnEventClickGoodsInput));
	m_pMgr->UnRegisterEventHandler(_T("MsgBox_ShopDragNumber"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ShopShelfFrame::OnEventDragGoodsInput));
	m_pMgr->UnRegisterEventHandler(_T("ItemEvent_Buy"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&ShopShelfFrame::OnEventBuyGoods));

	m_pCmdMgr->UnRegister("NS_FeedbackFromShop",	(NETMSGPROC)m_Trunk.sfp2(&ShopShelfFrame::OnNetFeedbackFromShop));
	m_pCmdMgr->UnRegister("NS_GetShopItems",		(NETMSGPROC)m_Trunk.sfp2(&ShopShelfFrame::OnNetGetShopItems));
	m_pCmdMgr->UnRegister("NS_GetShopEquips",		(NETMSGPROC)m_Trunk.sfp2(&ShopShelfFrame::OnNetGetShopEquips));
	m_pCmdMgr->UnRegister("NS_BuyShopItem",			(NETMSGPROC)m_Trunk.sfp2(&ShopShelfFrame::OnNetBuyShopItem));
	m_pCmdMgr->UnRegister("NS_BuyShopEquip",		(NETMSGPROC)m_Trunk.sfp2(&ShopShelfFrame::OnNetBuyShopEquip));

	if(P_VALID(m_pWnd))
	{
		HideTip(m_pWnd);

		m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&ShopShelfFrame::EventHandler));
		m_pGUI->DestroyWnd(m_pWnd);
		m_pWnd = NULL;
	}

	DestroyMsgBox();
	
	FreeMap();

	return bRet;
}

BOOL ShopShelfFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\shop.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	for (INT i=0; i<MAX_SHOP_ONE_SHELF; ++i)
	{
		CHAR szFullName[X_LONG_NAME] = {0};
		_snprintf(szFullName, sizeof(szFullName), "shopwin\\shopbackpic\\itemback\\itempic%d\\item%d", i+1, i+1);
		m_pGUI->ChangeXml(&ele, szFullName, "ClassName", "ItemButton");
		m_pGUI->ChangeXml(&ele, szFullName, "TextRect_right", "36");
		m_pGUI->ChangeXml(&ele, szFullName, "TextRect_bottom", "36");
		m_pGUI->ChangeXml(&ele, szFullName, "TextAlign", "RightBottom");
	}

	// 创建
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&ShopShelfFrame::EventHandler));

	m_pWndCaption = m_pWnd->GetChild(_T("shopbackpic"));

	m_pBtnExit = (GUIButton*)m_pWndCaption->GetChild(_T("closebutt1"));
	m_pBtnClose = (GUIButton*)m_pWndCaption->GetChild(_T("closebutt2"));
	m_pStcTitle = (GUIStatic*)m_pWndCaption->GetChild(_T("shopname"));

	for(INT i = 0; i < MAX_SHOP_SHELF_NUM; ++i)
	{
		TCHAR szTmp[X_SHORT_NAME] = {0};
		_sntprintf(szTmp,sizeof(szTmp)/sizeof(TCHAR), _T("shopfra\\shopbutt%d") ,i+1);
		m_pPbnMenu[i] = (GUIPushButton*)m_pWndCaption->GetChild(szTmp);
	}

	
	// 创建单元格
	for (INT i = 0; i < MAX_SHOP_ONE_SHELF; ++i)
	{
		TCHAR szFullName[X_LONG_NAME] = {0};
		_sntprintf(szFullName, sizeof(szFullName)/sizeof(TCHAR), _T("itemback\\itempic%d\\item%d"), i+1, i+1);
		m_pUnitItem[i] = (ItemButton*)m_pWndCaption->GetChild(szFullName);
	}
	
	m_pBtnPageUp = (GUIButton*)m_pWndCaption->GetChild(_T("before"));
	m_pBtnPageDown = (GUIButton*)m_pWndCaption->GetChild(_T("next"));
	m_pStcPage = (GUIStatic*)m_pWndCaption->GetChild(_T("word1\\new"));
	
	// 开启商店
	if( !StartShopShelf() )
	{
		// 开启失败则关闭
		tagGameEvent event(_T("Close_Shop"), NULL);
		m_pFrameMgr->SendEvent(&event);
	}

	// 翻到当前页
	m_pPbnMenu[m_nCurMenu]->SetState(EGUIBS_PushDown, FALSE);

	return TRUE;
}

DWORD ShopShelfFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnExit || pWnd == m_pBtnClose)//退出商店
			{
				tagGameEvent event(_T("Close_Shop"), this);
				m_pFrameMgr->SendEvent(&event);
			}
			else if (pWnd == m_pBtnPageUp)
			{
				PageDown(FALSE);
			}
			else if (pWnd == m_pBtnPageDown)
			{
				PageDown();
			}
			else if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)//点击货架中的物品
			{
				for (INT i = 0; i < MAX_SHOP_ONE_SHELF; ++i)
				{
					if (pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						TryToClickItem(i, (GetKeyState(VK_SHIFT)&0x80)!=0);
						break;
					}
				}
			}
		}
		break;
	case EGUIE_Drag:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)//拖动货架中的物品
			{
				for (INT i = 0; i < MAX_SHOP_ONE_SHELF; ++i)
				{
					if (pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						TryToDragItem(i, (GetKeyState(VK_SHIFT)&0x80)!=0);
						break;
					}
				}
			}
		}
		break;
	case EGUIE_PushButtonDown:
		{
			if (pEvent->dwParam1 != 1)//不是按下不处理
				return 0;

			if(_tcsnccmp(pWnd->GetName().c_str(), _T("shopbutt"), 8) == 0)
			{
				for (INT i=0; i<MAX_SHOP_SHELF_NUM; ++i)
				{
					if (pWnd == m_pPbnMenu[i])
					{
						ShowCurMenu(i);
						return 0;
					}
				}
			}
		}
		break;
	case EGUISCB_Drop:
		{
			if(EDT_Item == pEvent->dwParam1)//如果是背包中的物品则
			{
				OnSellItemToShop((INT16)LOWORD(pEvent->dwParam2), pEvent->dwParam3);
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)//拖动货架中的物品
			{
				for (INT i=0; i<MAX_SHOP_ONE_SHELF; ++i)
				{
					if (pWnd == m_pUnitItem[i])
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

VOID ShopShelfFrame::ShowCurMenu(INT nMenu)
{
	m_nCurMenu = nMenu;
	m_nCurPage[nMenu] = 0;
	
	// 发送特殊商品列表请求
	SendShelfSpecList((BYTE)nMenu);
	
	// 计算当前货架最大页数
	if (P_VALID(m_pShop))
	{
		if (m_pShop->bEquip)
		{
			m_nCurMaxPage[nMenu] = ( m_mapNormalGoods[nMenu].Size() + m_mapSpecEquip[nMenu].Size() ) / MAX_SHOP_ONE_SHELF;
		}
		else
		{
			m_nCurMaxPage[nMenu] = ( m_mapNormalGoods[nMenu].Size() + m_mapSpecItem[nMenu].Size() ) / MAX_SHOP_ONE_SHELF;
		}
	}
	else
	{
		m_nCurMaxPage[nMenu] = m_mapNormalGoods[nMenu].Size() / MAX_SHOP_ONE_SHELF;
	}
	
	ShowCurPage(0);
}

VOID ShopShelfFrame::ShowCurPage(INT nPage)
{
	m_nFirstBlank = 0;
	m_nCurPage[m_nCurMenu] = nPage;
	// 先翻到当前页，从普通商品开始
	TMap<INT, const tagShopItemProto*>::TMapIterator iter;
	iter = m_mapNormalGoods[m_nCurMenu].Begin();
	INT i = 0;
	for(; i<m_nCurPage[m_nCurMenu]*MAX_SHOP_ONE_SHELF; ++i)
	{
		++iter;

		if (i > m_mapNormalGoods[m_nCurMenu].Size())
			break;
	}
	// 普通商店翻完，翻动特殊商店
	TMap<INT, tagShopItemClient*>::TMapIterator iterItem = m_mapSpecItem[m_nCurMenu].Begin();
	TMap<INT, tagShopEquipClient*>::TMapIterator iterEquip = m_mapSpecEquip[m_nCurMenu].Begin();
	if (m_pShop->bEquip)// 装备店翻动装备特殊表
	{
		for(; i<m_nCurPage[m_nCurMenu]*MAX_SHOP_ONE_SHELF; ++i)
		{
			++iterEquip;

			if (i > m_mapSpecEquip[m_nCurMenu].Size())
				break;
		}
	}
	else// 道具店翻动道具特殊表
	{
		for(; i<m_nCurPage[m_nCurMenu]*MAX_SHOP_ONE_SHELF; ++i)
		{
			++iterItem;

			if (i > m_mapSpecItem[m_nCurMenu].Size())
				break;
		}
	}
	
	// 翻到当前页，先显示普通商品
	i = 0;
	const tagShopItemProto* pProto = NULL;
	INT pos = 0;
	while (m_mapNormalGoods[m_nCurMenu].PeekNext(iter, pos, pProto))
	{
		if(P_VALID(pProto))
		{
			const tagItemProto* pItem = ItemProtoData::Inst()->FindItemProto(pProto->dwTypeID);
			if(P_VALID(pItem))
			{
				m_pUnitItem[i]->RefreshItem(pProto->dwTypeID, 1, pItem->byQuality);
				// 记录在整体货架中的位置
				m_pUnitItem[i]->SetBtnData(ESBT_Item, 1, pProto->dwTypeID, pos);
			}
			else
			{
				m_pUnitItem[i]->RefreshItem();
				m_pUnitItem[i]->SetBtnData(ESBT_Null, GT_INVALID, GT_INVALID, GT_INVALID);
			}
		}
		else
		{
			m_pUnitItem[i]->RefreshItem();
			m_pUnitItem[i]->SetBtnData(ESBT_Null, GT_INVALID, GT_INVALID, GT_INVALID);
		}
		++i;
	}
	// 显示特殊商品
	pos = 0;
	if (m_pShop->bEquip)// 装备店显示特殊装备
	{
		tagShopEquipClient* pEquip = NULL;
		while (m_mapSpecEquip[m_nCurMenu].PeekNext(iterEquip, pos, pEquip))
		{
			if (P_VALID(pEquip))
			{
				m_pUnitItem[i]->RefreshItem(pEquip->pEquip->GetItemTypeID(), 1, pEquip->pEquip->GetItemQuality());
				// 记录在整体货架中的位置
				m_pUnitItem[i]->SetBtnData(ESBT_Item, 1, pEquip->pEquip->GetItemTypeID(), pos);
			}
			else
			{
				m_pUnitItem[i]->RefreshItem();
				m_pUnitItem[i]->SetBtnData(ESBT_Null, GT_INVALID, GT_INVALID, GT_INVALID);
			}
		}
	}
	else// 道具店显示特殊道具
	{
		tagShopItemClient* pItem = NULL;
		while (m_mapSpecItem[m_nCurMenu].PeekNext(iterItem, pos, pItem))
		{
			if (P_VALID(pItem))
			{
				m_pUnitItem[i]->RefreshItem(pItem->dwTypeID, pItem->n16RemainNum, pItem->pItemProto->byQuality);
				// 记录在整体货架中的位置
				m_pUnitItem[i]->SetBtnData(ESBT_Item, pItem->n16RemainNum, pItem->dwTypeID, pos);
			}
			else
			{
				m_pUnitItem[i]->RefreshItem();
				m_pUnitItem[i]->SetBtnData(ESBT_Null, GT_INVALID, GT_INVALID, GT_INVALID);
			}
		}	
	}
	
	// 设置当前位置为第一空位
	m_nFirstBlank = i;
	// 清空所有
	for (; i<MAX_SHOP_ONE_SHELF; ++i)
	{
		m_pUnitItem[i]->RefreshItem();
		m_pUnitItem[i]->SetBtnData(ESBT_Null, GT_INVALID, GT_INVALID, GT_INVALID);
	}
			
	SetCurPage();
}


VOID ShopShelfFrame::PageDown(bool bFlag /* = TRUE */)
{
	if (bFlag)
	{
		++m_nCurPage[m_nCurMenu];
		if (m_nCurPage[m_nCurMenu] >= m_nCurMaxPage[m_nCurMenu])
		{
			--m_nCurPage[m_nCurMenu];
		}
	}
	else
	{
		--m_nCurPage[m_nCurMenu];
		if (m_nCurPage[m_nCurMenu] < 0)
		{
			++m_nCurPage[m_nCurMenu];
		}
	}
	ShowCurPage(m_nCurPage[m_nCurMenu]);
}

VOID ShopShelfFrame::SetSignboard(LPCTSTR szSignboard)
{
	m_pStcTitle->SetText(szSignboard);
	m_pStcTitle->SetRefresh();
}

VOID ShopShelfFrame::SetCurPage()
{
	TCHAR szPage[X_SHORT_NAME] = {0};
	_sntprintf(szPage, sizeof(szPage)/sizeof(TCHAR), _T("%d"), m_nCurPage[m_nCurMenu] + 1);
	m_pStcPage->SetText(szPage);
	m_pStcPage->FlipToTop();

	m_pBtnPageUp->SetEnable(0 < m_nCurPage[m_nCurMenu]);
	m_pBtnPageDown->SetEnable(m_nCurMaxPage[m_nCurMenu] - 1 > m_nCurPage[m_nCurMenu]);

}

BOOL ShopShelfFrame::StartShopShelf()
{
	m_dwShopNpcID = QuestMgr::Inst()->GetTalkNPCGlobalID();
	const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(QuestMgr::Inst()->GetTalkNPCTypeID());
	if( P_VALID(pCreature) && EFNPCT_Shop == pCreature->eFunctionType )
	{
		const tagShopProtoClient* pShop = ShopProtoData::Inst()->FindShopProto(pCreature->uFunctionID.dwShopID);
		if( P_VALID(pShop) )
		{
			m_pShop = pShop;
			//设置商店表头
			SetSignboard(pShop->szName);
			//设置货架个数
			for(INT i=0; i<MAX_SHOP_SHELF_NUM; ++i)
			{
				SetShelfName(i, pShop->szShelf[i]);
			}
			//设置普通商品列表
			SetShelfNormalList(pShop);
			
			// 如果是装备商店，隐藏第二条提示“shift可购买多个物品”
			if(pShop->bEquip)
			{
				GUIWnd* pWnd = m_pWndCaption->GetChild(_T("register2"));
				if(P_VALID(pWnd))
				{
					pWnd->SetInvisible(true);
				}
			}


			return TRUE;
		}
	}
	return FALSE;
}

VOID ShopShelfFrame::SetShelfName( INT nIndex, LPCTSTR szShelfName )
{
	if( _tcsclen(szShelfName) != 0 )
	{
		m_pPbnMenu[nIndex]->SetText(szShelfName);
		m_pPbnMenu[nIndex]->SetRefresh();
        m_pPbnMenu[nIndex]->SetInvisible(FALSE);
	}
	else
	{
		m_pPbnMenu[nIndex]->SetInvisible(TRUE);
	}
}

VOID ShopShelfFrame::SetShelfNormalList(const tagShopProtoClient* pShop)
{
	//清除以前的list
	for ( INT i=0; i<MAX_SHOP_SHELF_NUM; ++i)
	{
		m_mapNormalGoods[i].Clear();
	}
	//加入list
	for ( INT i=0; i<MAX_SHOP_COMMON_ITEM; ++i)
	{
		//如果dwTypeID有效
		if( P_VALID(pShop->Item[i].dwTypeID) )
		{
			// 氏族宝库需判断该商品是否被激活
			if(pShop->bClanTreasury)
			{
				// 未激活则不存入商店
				if(!TreasureMgr::Inst()->IsTreaActivedbyItemID(pShop->Item[i].dwTypeID, MTRANS_ECCT2ECLT(pShop->eCostType)))
					continue;
			}
			m_mapNormalGoods[pShop->Item[i].byShelf - 1].Add( i, &pShop->Item[i] );
		}
		else//发现有无效ID则直接结束
			break;
	}

}

VOID ShopShelfFrame::TryToClickItem( INT nPos, bool bShift/*=FALSE*/ )
{
	INT nKey = (INT)m_pUnitItem[nPos]->GetBtnData().GetData3();
	// 如果nKey是默认值0，则要看是不是第一页的第一个，不是则直接返回，因为只有第一页的第一个才有可能是0
	if (nKey == 0 && (nPos != 0 || m_nCurPage[m_nCurMenu] != 0))
		return;
	INT nMaxCanBuy = 0;
	DWORD dwTypeID = GetGoodsTypeID(nKey, nMaxCanBuy);
	if(nMaxCanBuy <= 0)// 如果一个都买不了，直接返回
		return;

	if(GT_VALID(dwTypeID))
	{	
		INT16 n16Num = 1;
		//按下shift键则为物品拆分
		if(bShift)
		{
			// 装备不能批量购买
			if( !MIsEquipment(dwTypeID) )
			{
				if(P_VALID(GetObj("ShelfNumber")))
					KillObj("ShelfNumber");

				CreateObj("ShelfNumber", "MsgInputBox");
				TObjRef<MsgInputBox>("ShelfNumber")->Init(
					_T(""), g_StrTable[_T("MsgBoxText_InputBuyNumber")], _T("MsgBox_GoodsNumber"), nMaxCanBuy );
				
				m_nClickKey = nKey;
				return;
			}
		}

		OnBuyShopItem(nKey, dwTypeID, n16Num);
	}
}

VOID ShopShelfFrame::TryToDragItem( INT nPos, bool bShift/*=FALSE*/ )
{
	INT nKey = (INT)m_pUnitItem[nPos]->GetBtnData().GetData3();
	// 如果nKey是默认值0，则要看是不是第一页的第一个，不是则直接返回，因为只有第一页的第一个才有可能是0
	if (nKey == 0 && (nPos != 0 || m_nCurPage[m_nCurMenu] != 0))
		return;
	
	INT nMaxCanBuy = 0;
	DWORD dwTypeID = GetGoodsTypeID(nKey, nMaxCanBuy);
	if(nMaxCanBuy <= 0)// 如果一个都买不了，直接返回
		return;
		
	if(GT_VALID(dwTypeID))
	{	
		INT16 n16Num = 1;
		//按下shift键则为物品拆分
		if(bShift)
		{
			// 装备不能批量购买
			if( !MIsEquipment(dwTypeID) )
			{
				if(P_VALID(GetObj("ShopDragNumber")))
					KillObj("ShopDragNumber");

				CreateObj("ShopDragNumber", "MsgInputBox");
				TObjRef<MsgInputBox>("ShopDragNumber")->Init(
					_T(""), g_StrTable[_T("MsgBoxText_InputBuyNumber")], _T("MsgBox_ShopDragNumber"), nMaxCanBuy );

				m_nDragPos = nPos;
				return;
			}
		}

		OnDragItem(nPos, nKey, dwTypeID, n16Num);
	}
}

VOID ShopShelfFrame::OnDragItem( INT nPos, INT nKey, DWORD dwTypeID, INT16 n16Num )
{
	DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
	if (P_VALID(pDrag))
	{
		//发送游戏事件
		tagGUIEvent event(m_pUnitItem[nPos], EGUIE_Drag, NULL);
		POINT pt;
		::GetCursorPos(&pt);
		event.dwParam1 = EDT_ItemShop;
		event.dwParam2 = MAKELONG(nKey, n16Num);
		event.dwParam3 = dwTypeID;
		event.dwParam4 = MAKELONG(pt.x, pt.y);
		
		pDrag->OnEvent(&event);
	}
}

VOID ShopShelfFrame::OnBuyShopItem( INT nKey, DWORD dwTypeID, INT16 n16Num )
{
	INT64 n64Serial = GT_INVALID;

	// 判断是否可买
	if (!IsGoodsCanBuy(nKey, n16Num, n64Serial))
		return;
	
	// 买装备
	if(MIsEquipment(dwTypeID))
	{
		tagNC_BuyShopEquip m;
		m.byShelf = (BYTE)m_nCurMenu;
		m.dwNPCID = m_dwShopNpcID;
		m.n64Serial = n64Serial;
		m.dwTypeID = dwTypeID;
		m_pSession->Send(&m);		
	}
	else
	{
		tagNC_BuyShopItem m;
		m.byShelf = (BYTE)m_nCurMenu;
		m.dwNPCID = m_dwShopNpcID;
		m.n16ItemNum = n16Num;
		m.dwTypeID = dwTypeID;
		m_pSession->Send(&m);
	}
}



VOID ShopShelfFrame::OnSellItemToShop( INT16 n16Pos, DWORD dwTypeID )
{
	Item* pItem = ItemMgr::Inst()->GetPocketItem(n16Pos);
	if(P_VALID(pItem) && pItem->GetItemTypeID() == dwTypeID)
	{
		// 判断是否可卖
		if(pItem->IsVendible())
		{
			//品级不是白色则需要提示
			if(pItem->GetItemQuality() > 0)
			{
				if(P_VALID(GetObj("SellCheck")))
					KillObj("SellCheck");

				TCHAR szTmp[X_LONG_NAME] = {0};
				_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("RoleSellItemToShop_Check")], pItem->GetDisplay()->szName, pItem->GetItemQuantity());
				CreateObj("SellCheck", "MsgBox");
				TObjRef<MsgBox>("SellCheck")->Init(_T(""), szTmp, _T("MsgBox_SellCheck"));
				m_pSellItem = pItem;
			}
			else
			{
				SendSellToShop(pItem->GetItemId());
			}
		}
		else
		{
			// 说明不可售出理由
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("CantVend")]);
		}
		
	}
}

VOID ShopShelfFrame::SendSellToShop( INT64 nItemSerial )
{
	tagNC_SellToShop m;
	m.dwNPCID  = m_dwShopNpcID;
	m.n64Serial = nItemSerial;
	m_pSession->Send(&m);
}

DWORD ShopShelfFrame::OnEventClickGoodsInput( tagMsgInputBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult && MBIT_Number == pGameEvent->eInputType)
	{
		INT nKey = m_nClickKey;
		DWORD dwTypeID = GetGoodsTypeID(nKey);
		if(GT_VALID(dwTypeID))
		{
			OnBuyShopItem(nKey, dwTypeID, pGameEvent->nInputNum);
		}
	}
	m_nClickKey = GT_INVALID;
	return 0;
}

DWORD ShopShelfFrame::OnEventDragGoodsInput( tagMsgInputBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult && MBIT_Number == pGameEvent->eInputType)
	{
		ASSERT(m_nDragPos >= 0 && m_nDragPos < MAX_SHOP_ONE_SHELF);
		INT nPos = m_nDragPos;
		INT nKey = (INT)m_pUnitItem[nPos]->GetBtnData().GetData3();
		DWORD dwTypeID = GetGoodsTypeID(nKey);
		if(GT_VALID(dwTypeID))
		{
			OnDragItem(nPos, nKey, dwTypeID, pGameEvent->nInputNum);
		}
	}
	m_nDragPos = GT_INVALID;
	return 0;
}

DWORD ShopShelfFrame::OnEventBuyGoods( tagBuyItemEvent* pGameEvent )
{
	// 直接发送买商品
	OnBuyShopItem( pGameEvent->n16Pos, pGameEvent->dwTypeID, pGameEvent->n16Num);

	return 0;
}

DWORD ShopShelfFrame::OnEventItemPosChange( tagItemPosChangeExEvent* pGameEvent )
{
	if(EICT_Bag == pGameEvent->eTypeSrc && EICT_Shop == pGameEvent->eTypeDst)
	{
		OnSellItemToShop(pGameEvent->n16IndexSrc, pGameEvent->dwTypeIDSrc);
	}
	return 0;
}

DWORD ShopShelfFrame::OnEventCheckSellItem( tagMsgBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult)
	{
		ASSERT(P_VALID(m_pSellItem));
		if(P_VALID(m_pSellItem))
		{
			SendSellToShop(m_pSellItem->GetItemId());
			m_pSellItem = NULL;
		}
	}
	return 0;
}

VOID ShopShelfFrame::ShowGoodsTip( INT nPos )
{
	INT nKey = (INT)m_pUnitItem[nPos]->GetBtnData().GetData3();
	// 如果nKey是默认值0，则要看是不是第一页的第一个，不是则直接返回，因为只有第一页的第一个才有可能是0
	if (nKey == 0 && (nPos != 0 || m_nCurPage[m_nCurMenu] != 0))
		return;
	
	const tagShopItemProto* pItem = m_mapNormalGoods[m_nCurMenu].Peek(nKey);
	if(P_VALID(pItem) && P_VALID(m_pShop))
	{
		// 氏族宝库需要显示激活玩家名称
		tstring strTip;
		if(m_pShop->bClanTreasury)
		{
			strTip = ToolTipCreator::Inst()->GetItemTips(pItem->dwTypeID, 0, TreasureMgr::Inst()->GetTreaIDbyItemID(pItem->dwTypeID, MTRANS_ECCT2ECLT(m_pShop->eCostType)));
		}
		else
		{
			strTip = ToolTipCreator::Inst()->GetItemTips(pItem->dwTypeID);
		}
		
		tagItemCost cost(pItem->nSilver, m_pShop->eCostType, pItem->nCostNum, (INT)pItem->byRepLevel, m_pShop->dwItemTypeID);
		tstring strTipEx = ToolTipCreator::Inst()->GetPriceTips(&cost, TRUE);		
		
		ShowTip(m_pUnitItem[nPos], strTip.c_str(), strTipEx.c_str());
		// 显示装备比较信息
		if(MIsEquipment(pItem->dwTypeID))
		{
			ToolTipCreator::Inst()->ShowEquipCompare(pItem->dwTypeID);
		}
		return;
	}
	if (m_pShop->bEquip)
	{
		tagShopEquipClient* pRare = m_mapSpecEquip[m_nCurMenu].Peek(nKey);
		if(P_VALID(pRare) && P_VALID(m_pShop))
		{
			/*tstring strTip = ToolTipCreator::Inst()->GetItemTips(pRare->pEquip);*/
			// 氏族宝库需要显示激活玩家名称
			tstring strTip;
			if(m_pShop->bClanTreasury)
			{
				strTip = ToolTipCreator::Inst()->GetItemTips(pRare->pRareProto->dwTypeID, pRare->pRareProto->byQuality, 
					TreasureMgr::Inst()->GetTreaIDbyItemID(pRare->pRareProto->dwTypeID, MTRANS_ECCT2ECLT(m_pShop->eCostType)));
			}
			else
			{
				strTip = ToolTipCreator::Inst()->GetItemTips(pRare->pRareProto->dwTypeID, pRare->pRareProto->byQuality);
			}

			tagItemCost cost(pRare->pRareProto->nSilver, m_pShop->eCostType, pRare->pRareProto->nCostNum, (INT)pRare->pRareProto->byRepLevel, m_pShop->dwItemTypeID);
			tstring strTipEx = ToolTipCreator::Inst()->GetPriceTips(&cost, TRUE);		

			ShowTip(m_pUnitItem[nPos], strTip.c_str(), strTipEx.c_str());
			// 显示装备比较信息
			ToolTipCreator::Inst()->ShowEquipCompare(pRare->pRareProto->dwTypeID);
		}
	}
	else
	{
		tagShopItemClient* pRare = m_mapSpecItem[m_nCurMenu].Peek(nKey);
		if(P_VALID(pRare) && P_VALID(m_pShop))
		{
			// 氏族宝库需要显示激活玩家名称
			tstring strTip;
			if(m_pShop->bClanTreasury)
			{
				strTip = ToolTipCreator::Inst()->GetItemTips(pRare->dwTypeID, 0, TreasureMgr::Inst()->GetTreaIDbyItemID(pRare->dwTypeID, MTRANS_ECCT2ECLT(m_pShop->eCostType)));
			}
			else
			{
				strTip = ToolTipCreator::Inst()->GetItemTips(pRare->dwTypeID);
			}

			tagItemCost cost(pRare->pRareProto->nSilver, m_pShop->eCostType, pRare->pRareProto->nCostNum, (INT)pRare->pRareProto->byRepLevel, m_pShop->dwItemTypeID);
			tstring strTipEx = ToolTipCreator::Inst()->GetPriceTips(&cost, TRUE);		

			ShowTip(m_pUnitItem[nPos], strTip.c_str(), strTipEx.c_str());
		}
	}
}

DWORD ShopShelfFrame::OnNetFeedbackFromShop( tagNS_FeedbackFromShop* pNetCmd, DWORD )
{
	if(E_Success != pNetCmd->dwErrorCode)
	{
		ShowShopErrorMsg(pNetCmd->dwErrorCode);
	}
	else
	{
		// 交易成功，播放音效
		TObjRef<AudioSys>()->Play2DSound(_T("trade"), SOUND_NORMAL);
	}
	return 0;
}

DWORD ShopShelfFrame::OnNetGetShopItems( tagNS_GetShopItems* pNetCmd, DWORD )
{
	if (P_VALID(m_pShop) && !m_pShop->bEquip)
	{
		// 清空数据
		FreeMap();
		// 重新设置当前货架的第一个空位
		m_nFirstBlank = m_mapNormalGoods[m_nCurMenu].Size();

		INT nMenu = pNetCmd->byShelf;
		ASSERT(nMenu>=0 && nMenu<MAX_SHOP_SHELF_NUM);

		for (INT i=0,pos=0; i<pNetCmd->n16RareItemNum; ++i)
		{
			tagMsgShopItem m;
			memcpy(&m, pNetCmd->byData+pos, sizeof(tagMsgShopItem));
			pos += sizeof(tagMsgShopItem);

			//如果数量为零则不处理
			if(m.n16RemainNum == 0)
				continue;
			
			tagShopItemClient* pRare = new tagShopItemClient;
			
			ASSERT(m.byProtoIndex>=0 && m.byProtoIndex<MAX_SHOP_RARE_ITEM);
			BOOL bRet = TRUE;
		
			pRare->pRareProto = &m_pShop->RareItem[m.byProtoIndex];
			if (!P_VALID(pRare->pRareProto) || pRare->pRareProto->dwTypeID != m.dwTypeID)
				bRet = FALSE;
			
			const tagItemProto* pProto = ItemProtoData::Inst()->FindItemProto(m.dwTypeID);
			if (!P_VALID(pProto))
				bRet = FALSE;
			
			if (bRet)
			{
				// 赋值并存储当前刷新商品
				pRare->n16Index = m.n16Index;
				pRare->n16RemainNum = m.n16RemainNum;
				pRare->dwTypeID = m.dwTypeID;
				pRare->pItemProto = pProto;
				m_mapSpecItem[nMenu].Add(m.n16Index, pRare);

				// 如果该货架是当前货架，且该货架第一页还有空位，则显示出来
				if (m_nCurMenu == nMenu && m_nFirstBlank >= 0 && m_nFirstBlank < MAX_SHOP_ONE_SHELF)
				{
					m_pUnitItem[m_nFirstBlank]->RefreshItem(pRare->dwTypeID, pRare->n16RemainNum, pProto->byQuality);
					// 记录在整体货架中的位置
					m_pUnitItem[m_nFirstBlank]->SetBtnData(ESBT_Item, pRare->n16RemainNum, pRare->dwTypeID, pRare->n16Index);
				
						++m_nFirstBlank;
				}
			}
			else
			{
				SAFE_DEL(pRare);
			}
		}
	}
	return 0;
}

DWORD ShopShelfFrame::OnNetGetShopEquips( tagNS_GetShopEquips* pNetCmd, DWORD )
{
	if (P_VALID(m_pShop) && m_pShop->bEquip)
	{
		// 清空数据
		FreeMap();
		// 重新设置当前货架的第一个空位
		m_nFirstBlank = m_mapNormalGoods[m_nCurMenu].Size();
		
		INT nMenu = pNetCmd->byShelf;
		ASSERT(nMenu>=0 && nMenu<MAX_SHOP_SHELF_NUM);

		for (INT i=0,pos=0; i<pNetCmd->n16EquipNum; ++i)
		{
			tagMsgShopEquip m;
			memcpy(&m, pNetCmd->byData+pos, sizeof(tagMsgShopEquip));
			pos += sizeof(tagMsgShopEquip);

			BOOL bRet = TRUE;
			tagShopEquipClient* pRare = new tagShopEquipClient;
			ASSERT(m.byProtoIndex>=0 && m.byProtoIndex<MAX_SHOP_RARE_ITEM);
			pRare->pRareProto = &m_pShop->RareItem[m.byProtoIndex];
			if (!P_VALID(pRare->pRareProto))
				bRet = FALSE;

			Equipment *pEquip = new Equipment(m.Equip);
			if ( !P_VALID(pEquip) || !pEquip->IsItem() )
				bRet = FALSE;
			else
				pRare->pEquip = pEquip;

			if (bRet)
			{
				// 赋值并存储当前刷新商品
				pRare->n16Index = m.n16Index;
				pRare->n16RemainNum = m.n16RemainNum;
				m_mapSpecEquip[nMenu].Add(m.n16Index, pRare);
				// 计算当前货架最大页
				m_nCurMaxPage[nMenu] = ( m_mapNormalGoods[nMenu].Size() + m_mapSpecEquip[nMenu].Size() ) / MAX_SHOP_ONE_SHELF;
				
				// 如果该货架是当前货架，且该货架第一页还有空位，则显示出来
				if (m_nCurMenu == nMenu && m_nFirstBlank >= 0 && m_nFirstBlank < MAX_SHOP_ONE_SHELF)
				{
					m_pUnitItem[m_nFirstBlank]->RefreshItem(pRare->pEquip->GetItemTypeID(), 1, pRare->pEquip->GetItemQuality());
					// 记录在整体货架中的位置
					m_pUnitItem[m_nFirstBlank]->SetBtnData(ESBT_Item, 1, pRare->pEquip->GetItemTypeID(), pRare->n16Index);

					++m_nFirstBlank;
				}
				
			}
			else
			{
				SAFE_DEL(pEquip);
				SAFE_DEL(pRare);
			}
		}

	}	
	
	return 0;
}

DWORD ShopShelfFrame::OnNetBuyShopItem( tagNS_BuyShopItem* pNetCmd, DWORD )
{
	if (pNetCmd->dwNPCID == m_dwShopNpcID)
	{
		tagShopItemClient* pItem = m_mapSpecItem[pNetCmd->byShelf].Peek(pNetCmd->byIndex);
		if (P_VALID(pItem) && pItem->dwTypeID == pNetCmd->dwTypeID)
		{
			pItem->n16RemainNum = pNetCmd->n16RemainNum;
			for (INT i=0; i<MAX_SHOP_ONE_SHELF; ++i)
			{
				if(m_pUnitItem[i]->GetBtnData().GetData3() == pNetCmd->byIndex)
				{
					if (pNetCmd->n16RemainNum>0)
					{
						m_pUnitItem[i]->RefreshItem(pItem->dwTypeID, pItem->n16RemainNum, pItem->pItemProto->byQuality);
						// 记录在整体货架中的位置
						m_pUnitItem[i]->SetBtnData(ESBT_Item, pItem->n16RemainNum, pItem->dwTypeID, pItem->n16Index);
					}
					else
					{
						m_pUnitItem[i]->RefreshItem();
						m_pUnitItem[i]->SetBtnData(ESBT_Null, GT_INVALID, GT_INVALID, GT_INVALID);
					}
					break;
				}
			}
		}

	}
	return 0;
}

DWORD ShopShelfFrame::OnNetBuyShopEquip( tagNS_BuyShopEquip* pNetCmd, DWORD )
{
	if (pNetCmd->dwNPCID == m_dwShopNpcID)
	{
		tagShopEquipClient* pItem = m_mapSpecEquip[pNetCmd->byShelf].Peek(pNetCmd->byIndex);
		if (P_VALID(pItem))
		{
			pItem->n16RemainNum = pNetCmd->n16RemainNum;
			if (pNetCmd->n16RemainNum>0)
			{
				tagEquip equip;
				memcpy(&equip, pNetCmd->byData, sizeof(tagEquip));
				Equipment* pEquip = new Equipment(equip);
				if (pEquip->IsItem())
				{
					SAFE_DEL(pItem->pEquip);
					pItem->pEquip = pEquip;
				}
				else
				{
					SAFE_DEL(pEquip);
				}
			}
			
			for (INT i=0; i<MAX_SHOP_ONE_SHELF; ++i)
			{
				if(m_pUnitItem[i]->GetBtnData().GetData3() == pNetCmd->byIndex)
				{
					if (pNetCmd->n16RemainNum>0)
					{
						m_pUnitItem[i]->RefreshItem(pItem->pEquip->GetItemTypeID(), 1, pItem->pEquip->GetItemQuality());
						// 记录在整体货架中的位置
						m_pUnitItem[i]->SetBtnData(ESBT_Item, 1, pItem->pEquip->GetItemTypeID(), pItem->n16Index);
					}
					else
					{
						m_pUnitItem[i]->RefreshItem();
						m_pUnitItem[i]->SetBtnData(ESBT_Null, GT_INVALID, GT_INVALID, GT_INVALID);
					}
				}
			}
		}
	}
	return 0;
}

VOID ShopShelfFrame::ShowShopErrorMsg( DWORD dwErrorCode )
{
	// 取正值
	if(dwErrorCode > 0x0fffffff)
		dwErrorCode = 0 - dwErrorCode + 10000;//取正值，避免冲突加上10000

	TCHAR szTmp[X_LONG_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("ShowShopErrorMsg_ErrorCode%d"), dwErrorCode);
	CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[szTmp]);
}

VOID ShopShelfFrame::Update()
{
	if (RoleMgr::Inst()->IsOutValidDist(m_dwShopNpcID))
	{
		tagGameEvent event(_T("Close_Shop"), this);
		m_pFrameMgr->SendEvent(&event);
	}

	GameFrame::Update();
}

VOID ShopShelfFrame::SendShelfSpecList(BYTE byMemu)
{
	if (m_pShop->bEquip)
	{
		tagNC_GetShopEquips m;
		m.dwNPCID = m_dwShopNpcID;
		m.byShelf = byMemu;
		m_pSession->Send(&m);
	}
	else
	{
		tagNC_GetShopItems m;
		m.dwNPCID = m_dwShopNpcID;
		m.byShelf = byMemu;
		m_pSession->Send(&m);
	}
}

DWORD ShopShelfFrame::GetGoodsTypeID( INT nKey, INT& nMaxCanBuy )
{
	ASSERT(P_VALID(m_pShop));
	nMaxCanBuy = 0;

	const tagShopItemProto* pProto = m_mapNormalGoods[m_nCurMenu].Peek(nKey);
	if(P_VALID(pProto))
	{
		nMaxCanBuy = GetGoodsMaxCanBuy(pProto->nSilver, pProto->nCostNum, pProto->byRepLevel);
		const tagItemProto* pItemProto = ItemProtoData::Inst()->FindItemProto(pProto->dwTypeID);
		if(P_VALID(pItemProto))
		{
			nMaxCanBuy = min(nMaxCanBuy, pItemProto->n16MaxLapNum);
		}
		return pProto->dwTypeID;
	}

	if (m_pShop->bEquip)
	{
		tagShopEquipClient* pEquip = m_mapSpecEquip[m_nCurMenu].Peek(nKey);
		if (P_VALID(pEquip))
		{
			// 限量装备最多只能买一个
			nMaxCanBuy = GetGoodsMaxCanBuy(pEquip->pRareProto->nSilver, pEquip->pRareProto->nCostNum, pEquip->pRareProto->byRepLevel);
			nMaxCanBuy = min(nMaxCanBuy, 1);
			return pEquip->pEquip->GetItemTypeID();
		}
	}
	else
	{
		tagShopItemClient* pItem = m_mapSpecItem[m_nCurMenu].Peek(nKey);
		if (P_VALID(pItem))
		{
			// 限量物品不能大于当前最大数量
			nMaxCanBuy = GetGoodsMaxCanBuy(pItem->pRareProto->nSilver, pItem->pRareProto->nCostNum, pItem->pRareProto->byRepLevel);
			nMaxCanBuy = min(nMaxCanBuy, pItem->n16RemainNum);
			nMaxCanBuy = min(nMaxCanBuy, pItem->pItemProto->n16MaxLapNum);
			return pItem->dwTypeID;
		}
	}
	
	return GT_INVALID;
}

DWORD ShopShelfFrame::GetGoodsTypeID( INT nKey )
{
	ASSERT(P_VALID(m_pShop));
	
	const tagShopItemProto* pProto = m_mapNormalGoods[m_nCurMenu].Peek(nKey);
	if(P_VALID(pProto))
	{
		return pProto->dwTypeID;
	}

	if (m_pShop->bEquip)
	{
		tagShopEquipClient* pEquip = m_mapSpecEquip[m_nCurMenu].Peek(nKey);
		if (P_VALID(pEquip))
		{
			return pEquip->pEquip->GetItemTypeID();
		}
	}
	else
	{
		tagShopItemClient* pItem = m_mapSpecItem[m_nCurMenu].Peek(nKey);
		if (P_VALID(pItem))
		{
			return pItem->dwTypeID;
		}
	}

	return GT_INVALID;
}
BOOL ShopShelfFrame::IsGoodsCanBuy( INT nKey, INT16 n16Num, INT64 &n64Serial )
{
	ASSERT(P_VALID(m_pShop));
	n64Serial = GT_INVALID;

	const tagShopItemProto* pProto = m_mapNormalGoods[m_nCurMenu].Peek(nKey);
	if(P_VALID(pProto))
	{
		// 判断是否可以买

		return TRUE;
	}
	
	if (m_pShop->bEquip)
	{
		tagShopEquipClient* pEquip = m_mapSpecEquip[m_nCurMenu].Peek(nKey);
		if (P_VALID(pEquip))
		{
			// 判断是否可以买

			n64Serial = pEquip->pEquip->GetItemId();
			return TRUE;
		}

	}
	else
	{
		tagShopItemClient* pItem = m_mapSpecItem[m_nCurMenu].Peek(nKey);
		if (P_VALID(pItem))
		{	
			// 判断是否可以买

			return TRUE;
		}
	}

	return FALSE;
}

VOID ShopShelfFrame::FreeMap()
{
	// 清空数据
	for(INT i=0; i<MAX_SHOP_SHELF_NUM; ++i)
	{
		tagShopItemClient* pItem = NULL;
		m_mapSpecItem[i].ResetIterator();
		while (m_mapSpecItem[i].PeekNext(pItem))
		{
			if (P_VALID(pItem))
			{
				SAFE_DEL(pItem);
			}
		}
		m_mapSpecItem[i].Clear();
	}

	for(INT i=0; i<MAX_SHOP_SHELF_NUM; ++i)
	{
		tagShopEquipClient* pEquip = NULL;
		m_mapSpecEquip[i].ResetIterator();
		while (m_mapSpecEquip[i].PeekNext(pEquip))
		{
			if (P_VALID(pEquip))
			{
				SAFE_DEL(pEquip->pEquip);
				SAFE_DEL(pEquip);
			}
		}
		m_mapSpecEquip[i].Clear();
	}
}

INT ShopShelfFrame::GetGoodsMaxCanBuy( INT32 nSilver, INT32 nCostNum, BYTE byLevel )
{
	INT nRet = 0;
	if(byLevel > 0)
	{
		// 先判断声望等级是否达到
		if(m_pShop->eCostType >= ECCT_ClanConXuanYuan && m_pShop->eCostType <= ECCT_ClanConGongGong )
		{
			const tagReputeLevel* pLevel = ReputeMgr::Inst()->GetReputeLevel(MTRANS_ECCT2ECLT(m_pShop->eCostType));
			// 如果当前相应声望等级没有达到，则直接返回0
			if(P_VALID(pLevel) && pLevel->eLevel < byLevel)
			{
				ShowShopErrorMsg(E_Shop_RepLevel_Low);
				return nRet;
			}
		}
	}
	if(nSilver > 0)
	{
		// 银两是否足够
		INT64 n64Silver = CurrencyMgr::Inst()->GetBagSilver();
		nRet = (INT)(n64Silver / nSilver);
		if(nRet <= 0)
		{
			ShowShopErrorMsg(E_Shop_NotEnough_SilverInBag);
		}
	}
	if(nCostNum > 0)
	{
		INT nQuat;
		// 处理特殊消耗
		switch(m_pShop->eCostType)
		{
		case ECCT_BagSilver:// 背包银两
			{
				INT64 n64Silver = CurrencyMgr::Inst()->GetBagSilver();
				nQuat = (INT)(n64Silver / nCostNum);
			}
			break;
		case ECCT_BagYuanBao:// 背包元宝
			{
				INT nYanbao = CurrencyMgr::Inst()->GetBagYuanbao();
				nQuat = (INT)(nYanbao / nCostNum);
			}
			break;
		case ECCT_WareSilver:// 仓库银两
			{
				INT64 n64Silver = CurrencyMgr::Inst()->GetWareSilver();
				nQuat = (INT)(n64Silver / nCostNum);
			}
			break;
		case ECCT_BaiBaoYuanBao:// 仓库元宝
			{
				INT nYanbao = CurrencyMgr::Inst()->GetBaibaoYuanbao();
				nQuat = (INT)(nYanbao / nCostNum);				
			}
			break;
		case ECCT_GuildContribe:// 帮派贡献
			{
				INT nCont = GuildMgr::Inst()->GetLpInfo()->nContribution;
				nQuat =(INT)(nCont / nCostNum);
			}
			break;
		case ECCT_GuildExploit:// 帮派功勋
			{
				INT nExp = GuildMgr::Inst()->GetLpInfo()->nExploit;
				nQuat = (INT)(nExp / nCostNum);
			}
			break;
		case ECCT_ItemExchange:// 物品兑换
			{
				ItemContainer* pPocket = ItemMgr::Inst()->GetPocket();
				INT nNum = pPocket->GetItemQuantity(m_pShop->dwItemTypeID);
				nQuat = (INT)(nNum / nCostNum);
			}
			break;
		case ECCT_ClanConXuanYuan:// 轩辕贡献
		case ECCT_ClanConShenNong:// 神农贡献
		case ECCT_ClanConFuXi:// 伏羲贡献
		case ECCT_ClanConSanMiao:// 三苗贡献
		case ECCT_ClanConJiuLi:// 九黎贡献
		case ECCT_ClanConYueZhi:// 月氏贡献
		case ECCT_ClanConNvWa:// 女娲贡献
		case ECCT_ClanConGongGong:// 共工贡献
			{
				INT nCon = ReputeMgr::Inst()->GetReputeValue(MTRANS_ECCT2ECLT(m_pShop->eCostType));
				nQuat = (INT)(nCon / nCostNum);
			}
			break;
		}
		if(nQuat <= 0)
		{
			if(m_pShop->eCostType == ECCT_ItemExchange)
				ShowShopErrorMsg(E_Item_NotEnough);
			else
				ShowShopErrorMsg(E_Shop_NotEnough_SpecCost);
		}
		

		if(nRet > 0)
			nRet = min(nRet,nQuat);
		else
			nRet = nQuat;
	}
	return nRet;
}

BOOL ShopShelfFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

	tagGameEvent event(_T("Close_Shop"), this);
	m_pFrameMgr->SendEvent(&event);
	return TRUE;
}

VOID ShopShelfFrame::DestroyMsgBox()
{
	if(P_VALID(GetObj("ShelfNumber")))
		KillObj("ShelfNumber");

	if(P_VALID(GetObj("ShopDragNumber")))
		KillObj("ShopDragNumber");

	if(P_VALID(GetObj("SellCheck")))
		KillObj("SellCheck");
}

void ShopShelfFrame::ShowWnd( BOOL bVisable )
{
    if (bVisable)
    {
        // 开启商店
        if( !StartShopShelf() )
        {
            // 开启失败则关闭
            ShowWnd(FALSE);
            return;
        }

        // 翻到当前页
        m_nCurMenu = 0;
        m_pPbnMenu[m_nCurMenu]->SetState(EGUIBS_PushDown, TRUE);

        // 打开窗口
        if (P_VALID(m_pWnd))
        {
            m_pWnd->SetInvisible(FALSE);
            m_pWnd->SetRefresh(TRUE);
        }
    }
    else
    {
        // 关闭窗体
        if (P_VALID(m_pWnd))
        {
            m_pWnd->SetInvisible(TRUE);
            HideTip(m_pWnd);
        }

        DestroyMsgBox();

        FreeMap();
    }
}

bool ShopShelfFrame::IsOpen()
{
    if (P_VALID(m_pWnd))
        return !m_pWnd->IsInvisible();

    return false;
}