#include "StdAfx.h"
#include "StorageFrame.h"
#include "ItemFrame.h"
#include "DragBtn.h"
#include "ItemMgr.h"
#include "Item.h"
#include "Container.h"
#include "ItemButton.h"
#include "ItemEvent.h"
#include "CurrencyMgr.h"
#include "..\WorldDefine\ItemDefine.h"
#include "..\WorldDefine\currency_define.h"
#include "..\WorldDefine\msg_item.h"
#include "..\WorldDefine\msg_func_npc.h"
#include "NetSession.h"
#include "QuestMgr.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "RoleMgr.h"
#include "ItemMenuButton.h"

const INT MAX_COL = 6;
const INT MAX_ROW = 4;
const SIZE UINT_SIZE = {36,36};
const SIZE SPACE_SIZE = {9,7};

StorageFrame::StorageFrame():m_Trunk(this), m_bWalkWare(false)
{
	m_pWnd = NULL;
	m_pWndCaption = NULL;
	m_pBtnSaving = NULL;
	m_pBtnSettle = NULL;
	m_pBtnLoading = NULL;
	m_pBtnExit = NULL;

	for(INT i = 0; i < MAX_BAG_NUM; ++i)
	{
		m_pPbnStorageIndex[i] = NULL;
	}

	/*m_pStcTreasure = NULL;*/
	m_pStcGold = NULL;
	m_pStcSilver = NULL;
	m_dwNPCID = GT_INVALID;

	m_n16CurPage = 0;
}

StorageFrame::~StorageFrame()
{

}

BOOL StorageFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_UpdateItem"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventUpdateLPItem));
	m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_Move2Ware"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventItemPosChange));
	m_pFrameMgr->RegisterEventHandle(_T("Ware_Silver"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventRoleWareSilverChange));
	/*m_pFrameMgr->RegisterEventHandle(_T("Ware_Yuanbao"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventRoleWareYuanbaoChange));*/
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_SaveMoney"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventSaveMoneyToWare));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_LoadMoney"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventLoadMoneyFromWare));
	m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_WareExtend"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventWareExtend));
	
    m_bWalkWare = false;
	
	return bRet;
}

BOOL StorageFrame::Destroy()
{
    m_bWalkWare = false;

	BOOL bRet = GameFrame::Destroy();

	if (P_VALID(m_pWnd))
	{
		HideTip(m_pWnd);

		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
	
	m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_UpdateItem"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventUpdateLPItem));
	m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_Move2Ware"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventItemPosChange));
	m_pFrameMgr->UnRegisterEventHandler(_T("Ware_Silver"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventRoleWareSilverChange));
	/*m_pFrameMgr->UnRegisterEventHandler(_T("Ware_Yuanbao"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventRoleWareYuanbaoChange));*/
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_SaveMoney"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventSaveMoneyToWare));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_LoadMoney"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventLoadMoneyFromWare));
	m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_WareExtend"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StorageFrame::OnEventWareExtend));
	
	DestroyMsgBox();

	return bRet;
}

BOOL StorageFrame::ReloadUI()
{
	// 创建
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\warehouse.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());

	INT i,j;
	for(i = 0; i < MAX_WARE_NUM + 1; ++i)
	{
		CHAR szTmp[X_LONG_NAME] = {0};
		_snprintf(szTmp, sizeof(szTmp), "wh_win\\wh_backpic\\wh_fram\\warehouse%d" ,i+1);
		m_pGUI->ChangeXml(&ele, szTmp, "ClassName", "ItemMenuButton");
	}

	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&StorageFrame::EventHandler));

	m_pWndCaption = m_pWnd->GetChild(_T("wh_backpic"));

	m_pBtnSaving = (GUIButton*)m_pWndCaption->GetChild(_T("savemoney"));
	m_pBtnSettle = (GUIButton*)m_pWndCaption->GetChild(_T("arrangebutt"));
	m_pBtnLoading = (GUIButton*)m_pWndCaption->GetChild(_T("drawmoney"));
	m_pBtnExit = (GUIButton*)m_pWndCaption->GetChild(_T("closebutt"));

	for(i = 0; i < MAX_WARE_NUM; ++i)
	{
		TCHAR szTmp[X_SHORT_NAME] = {0};
		_sntprintf(szTmp,sizeof(szTmp)/sizeof(TCHAR), _T("wh_fram\\warehouse%d") ,i+1);
		m_pPbnStorageIndex[i] = (ItemMenuButton*)m_pWndCaption->GetChild(szTmp);
	}

	/*m_pStcTreasure = (GUIButton*)m_pWndCaption->GetChild(_T("money1"));*/
	m_pStcGold = (GUIButton*)m_pWndCaption->GetChild(_T("money2"));
	m_pStcSilver = (GUIButton*)m_pWndCaption->GetChild(_T("money3"));

	m_pStcStorage = (GUIStatic*)m_pWndCaption->GetChild(_T("wh_itemback\\wh_item"));
	// 创建单元格
	for(i = 0; i < MAX_COL; ++i)
	{
		for (j = 0; j < MAX_ROW; ++j)
		{
			XmlElement ele;
			CHAR szTemp[X_SHORT_NAME]={0};
			_snprintf(szTemp, sizeof(szTemp), "unititem%d", MAX_COL * j + i);
			ele.SetAttribute("Name", szTemp);
			ele.SetAttribute("ClassName", "ItemButton");
			ele.SetAttribute("Size_x", UINT_SIZE.cx);
			ele.SetAttribute("Size_y", UINT_SIZE.cy);
			ele.SetAttribute("LogicPos_x", 4 + SPACE_SIZE.cx*i + UINT_SIZE.cx*i);
			ele.SetAttribute("LogicPos_y", 4 + SPACE_SIZE.cy*j + UINT_SIZE.cy*j);
			ele.SetAttribute("Font", m_pUtil->UnicodeToUnicode8(g_StrTable[_T("Font_MSYH")]));
			/*ele.SetAttribute("FontWidth", 8);
			ele.SetAttribute("FontHeight", 16);*/
			ele.SetAttribute("TextRect_left", 0);
			ele.SetAttribute("TextRect_top", 0);
			ele.SetAttribute("TextRect_right", UINT_SIZE.cx);
			ele.SetAttribute("TextRect_bottom", UINT_SIZE.cy);
			ele.SetAttribute("TextColor", 0xFFFFFFFF);
			ele.SetAttribute("TextAlign", "RightBottom");
			m_pUnitItem[MAX_COL * j + i] = (ItemButton*)m_pGUI->CreateWnd(m_pStcStorage->GetFullName().c_str(), &ele);
		}
	}
	// 隐藏仓库栏
	//m_pWnd->SetInvisible(TRUE);

	// 翻到当前页
	ShowCurPage(m_n16CurPage);
	m_pPbnStorageIndex[m_n16CurPage]->SetState(EGUIBS_PushDown, FALSE);

	// 设置NPC
	m_dwNPCID = QuestMgr::Inst()->GetTalkNPCGlobalID();

	// 设置金钱数
	SetMoneyValue(CurrencyMgr::Inst()->GetWareSilver());
	/*SetTreasureValue(CurrencyMgr::Inst()->GetWareYuanbao());*/

	return TRUE;
}

DWORD StorageFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnExit)//退出
			{
				tagGameEvent event(_T("Close_Ware"), this);
				m_pMgr->SendEvent(&event);
			} 
			else if (pWnd == m_pBtnLoading)//取款
			{
				if(GetObj("LoadMoneyBox"))
					KillObj("LoadMoneyBox");

				CreateObj("LoadMoneyBox", "MsgMoneyInputBox");
				TObjRef<MsgMoneyInputBox>("LoadMoneyBox")->Init(
					_T(""), _T(""), _T("MsgBox_LoadMoney"), 
					CurrencyMgr::Inst()->GetWareSilver(),
					0, 0, 0, MBMT_Silver);
			}
			else if (pWnd == m_pBtnSaving)//存款
			{
				if(GetObj("SaveMoneyBox"))
					KillObj("SaveMoneyBox");

				CreateObj("SaveMoneyBox", "MsgMoneyInputBox");
				TObjRef<MsgMoneyInputBox>("SaveMoneyBox")->Init(
					_T(""), _T(""), _T("MsgBox_SaveMoney"), 
					CurrencyMgr::Inst()->GetBagSilver(),
					0, 0, 0, MBMT_Silver);
			}
			else if (pWnd == m_pBtnSettle)//整理仓库
			{
				ItemContainer* pItemCon = ItemMgr::Inst()->GetStorge();
				if(P_VALID(pItemCon))
				{
					DWORD dwNPCID = m_bWalkWare ? GT_INVALID : m_dwNPCID;
					pItemCon->AutoSettle( dwNPCID );
				}
			}
			else if (_tcsnccmp(pWnd->GetName().c_str(), _T("unititem"), 8) == 0)//使用仓库中的物品
			{
				for (INT i = 0; i < SPACE_ONE_WARE; ++i)
				{
					if (pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						OnItemChangeEx(EICT_RoleWare, EICT_Bag, (INT16)(m_n16CurPage*SPACE_ONE_WARE + i));			
						ShowTip(NULL, NULL);
						break;
					}
				}
			}
		}
		break;
	case EGUIE_Drag:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("unititem"), 8) == 0)//拖动仓库中的物品
			{
				for (INT i = 0; i < SPACE_ONE_WARE; ++i)
				{
					if (pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						Item* pItem = GetItem((INT16)(m_n16CurPage*SPACE_ONE_WARE + i));
						if(P_VALID(pItem))
						{
							DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
							if(P_VALID(pDrag))
							{
								pEvent->dwParam1 = EDT_ItemWare;
								pEvent->dwParam2 = MAKELONG(pItem->GetPos(), pItem->GetItemQuantity());
								pEvent->dwParam3 = pItem->GetItemTypeID();
								pDrag->OnEvent(pEvent);
							}
						}
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

			if (_tcsnccmp(pWnd->GetName().c_str(), _T("warehouse"), 9) == 0)//换仓库
			{
				for (INT i = 0; i < MAX_WARE_NUM; ++i)
				{
					if(pWnd == m_pPbnStorageIndex[i])
					{
						ShowCurPage(i);
						return 0;
					}
				}
			}
		}
		break;
	case EGUISCB_Drop:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("unititem"), 8) == 0)//接收物品拖动
			{
				for (INT i = 0; i < SPACE_ONE_WARE; ++i)
				{
					if (pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						if(EDT_ItemWare == (EDragType)pEvent->dwParam1)
						{
							OnItemChange((INT16)LOWORD(pEvent->dwParam2), (INT16)(m_n16CurPage*SPACE_ONE_WARE + i));
						}
						else if(EDT_Item == (EDragType)pEvent->dwParam1)
						{
							OnItemChangeEx(EICT_Bag, EICT_RoleWare, (INT16)LOWORD(pEvent->dwParam2), (INT16)(m_n16CurPage*SPACE_ONE_WARE + i));
						}
						break;
					}
				}
			}
			else if(_tcsnccmp(pWnd->GetName().c_str(), _T("warehouse"), 9) == 0)
			{
				for (INT i = 0; i < MAX_WARE_NUM; ++i)
				{
					if(pWnd == m_pPbnStorageIndex[i])
					{
						INT16 n16PosDst = ItemMgr::Inst()->GetConBlank(EICT_RoleWare, i);
						if(!GT_VALID(n16PosDst))
							break;

						if(EDT_ItemWare == (EDragType)pEvent->dwParam1)
						{
							OnItemChange((INT16)LOWORD(pEvent->dwParam2), n16PosDst);
						}
						else if(EDT_Item == (EDragType)pEvent->dwParam1)
						{
							OnItemChangeEx(EICT_Bag, EICT_RoleWare, (INT16)LOWORD(pEvent->dwParam2), n16PosDst);
						}
						break;
					}
				}
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("unititem"), 8) == 0)//移动到仓库中的物品
			{
				for (INT i = 0; i < SPACE_ONE_WARE; ++i)
				{
					if (pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						Item* pItem = GetItem((INT16)(m_n16CurPage*SPACE_ONE_WARE + i));
						if(P_VALID(pItem))
						{
							tstring strTip = ToolTipCreator::Inst()->GetItemTips(pItem);
							ShowTip(pWnd, strTip.c_str());
						}
						break;
					}
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(NULL, NULL);
		}
		break;
	default :
		break;
	}
	return 0;
}

VOID StorageFrame::SetMoneyValue(INT64 n64Value)
{
	TCHAR szTmp[X_SHORT_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), n64Value/GOLD2SILVER);
	m_pStcGold->SetText(szTmp);
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), n64Value%GOLD2SILVER);
	m_pStcSilver->SetText(szTmp);

	m_pStcGold->SetRefresh(TRUE);
	m_pStcSilver->SetRefresh(TRUE);
}

//VOID StorageFrame::SetTreasureValue(INT nValue)
//{
//	TCHAR szTmp[X_SHORT_NAME] = {0};
//	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), nValue);
//	m_pStcTreasure->SetText(szTmp);
//
//	m_pStcTreasure->SetRefresh(TRUE);
//}

VOID StorageFrame::ShowCurPage(INT16 page)
{
	m_n16CurPage = page;
	for (INT i = 0; i < SPACE_ONE_WARE; ++i)
	{
		RefreshItem(page * SPACE_ONE_WARE + i);
	}
}

Item* StorageFrame::GetItem(INT16 index)
{
	return ItemMgr::Inst()->GetStorge()->GetValue(index);
}

VOID StorageFrame::UpdateItem(INT16 index)
{
	if (IsCurPage(index))//如果为当前页则更新，不是当前页则不更新
	{
		RefreshItem(index);
	}
}

VOID StorageFrame::RefreshItem(INT16 index)
{
	DWORD dwTypeID = GT_INVALID;
	INT16 n16Quantity = 0;
	BYTE byQuality = 0;
	
	INT nMaxSize = ItemMgr::Inst()->GetStorge()->MaxSize();
	if(index >= nMaxSize && index < SPACE_ALL_WARE)
	{
		dwTypeID = 0;
	}
	else
	{
		Item* pItem = GetItem(index);
		if (P_VALID(pItem))
		{
			dwTypeID = pItem->GetItemTypeID();
			n16Quantity = pItem->GetItemQuantity();
			byQuality = pItem->GetItemQuality();
		}
	}

	m_pUnitItem[index%SPACE_ONE_WARE]->RefreshItem(dwTypeID, n16Quantity, byQuality);

}

VOID StorageFrame::OnItemChange( INT16 n16PosSrc, INT16 n16PosDst /*= GT_INVALID*/ )
{
	if(n16PosSrc == n16PosDst)
		return;

	Item* pItem = GetItem(n16PosSrc);
	if(P_VALID(pItem))
	{
		tagNC_ItemPosChange m;
		m.eConType = EICT_RoleWare;
		m.n16Num = 0;
		m.n16PosDst = n16PosDst;
		m.n64Serial = pItem->GetItemId();
		m.dwNPCID = m_bWalkWare ? GT_INVALID : m_dwNPCID;
		m_pSession->Send(&m);
	}
}

VOID StorageFrame::OnItemChangeEx( EItemConType ePosSrc, EItemConType ePosDst, INT16 n16PosSrc, INT16 n16PosDst /*= GT_INVALID*/ )
{
	Item* pItem = ItemMgr::Inst()->GetConItem(ePosSrc, n16PosSrc);
	if(P_VALID(pItem))
	{
		tagNC_ItemPosChangeEx m;
		m.eConTypeDst = ePosDst;
		m.eConTypeSrc = ePosSrc;
		m.n16PosDst = n16PosDst;
		m.n64Serial1 = pItem->GetItemId();
		m.dwNPCID = m_bWalkWare ? GT_INVALID : m_dwNPCID;
		m_pSession->Send(&m);
	}
}

DWORD StorageFrame::OnEventUpdateLPItem( tagUpdateLPItemEvent* pGameEvent )
{
	if(EICT_RoleWare == pGameEvent->eType)
	{
		//如果是当前页则更新
		if(IsCurPage(pGameEvent->n16Index))
		{
			RefreshItem(pGameEvent->n16Index);
		}
	}
	return 0;
}

DWORD StorageFrame::OnEventItemPosChange( tagItemPosChangeExEvent* pGameEvent )
{
	if(EICT_RoleWare == pGameEvent->eTypeDst)
	{
		OnItemChangeEx(pGameEvent->eTypeSrc, EICT_RoleWare, pGameEvent->n16IndexSrc);
	}
	return 0;
}

DWORD StorageFrame::OnEventRoleWareSilverChange( tagRoleSilverChange* pGameEvent )
{
	SetMoneyValue(pGameEvent->n64Silver);
	return 0;
}

//VOID StorageFrame::OnEventRoleWareYuanbaoChange( tagRoleYuanbaoChange* pGameEvent )
//{
//	SetTreasureValue(pGameEvent->nYuanbao);
//}

DWORD StorageFrame::OnEventSaveMoneyToWare( tagMsgMoneyBoxEvent* pGameEvent )
{
	if (MBF_OK == pGameEvent->eResult)
	{
		if(pGameEvent->n64Silver > 0)
		{
			tagNC_SaveSilver m1;
			m1.dwNPCID = m_bWalkWare ? GT_INVALID : m_dwNPCID;
			m1.n64SilverSave = pGameEvent->n64Silver;
			m_pSession->Send(&m1);
		}

		/*if(pGameEvent->nYuanbao > 0)
		{
			tagNC_SaveYuanBao m2;
			m2.dwNPCID = m_dwNPCID;
			m2.nYuanBaoSave = pGameEvent->nYuanbao;
			m_pSession->Send(&m2);
		}*/
	}
	return 0;
}

DWORD StorageFrame::OnEventLoadMoneyFromWare( tagMsgMoneyBoxEvent* pGameEvent )
{
	if (MBF_OK == pGameEvent->eResult)
	{
		if(pGameEvent->n64Silver > 0)
		{
			tagNC_GetSilver m1;
			m1.dwNPCID = m_bWalkWare ? GT_INVALID : m_dwNPCID;
			m1.n64SilverGet = pGameEvent->n64Silver;
			m_pSession->Send(&m1);
		}

		/*if(pGameEvent->nYuanbao > 0)
		{
			tagNC_GetYuanBao m2;
			m2.dwNPCID = m_dwNPCID;
			m2.nYuanBaoGet = pGameEvent->nYuanbao;
			m_pSession->Send(&m2);
		}*/
	}
	return 0;
}

VOID StorageFrame::Update()
{
    if (m_bWalkWare)
        return;

	if (RoleMgr::Inst()->IsOutValidDist(m_dwNPCID))
	{
		tagGameEvent event(_T("Close_Ware"), this);
		m_pFrameMgr->SendEvent(&event);
	}
}

DWORD StorageFrame::OnEventWareExtend( tagGameEvent* pGameEvent )
{
	ShowCurPage(m_n16CurPage);

	return 0;
}

VOID StorageFrame::DestroyMsgBox()
{
	if(GetObj("LoadMoneyBox"))
		KillObj("LoadMoneyBox");

	if(GetObj("SaveMoneyBox"))
		KillObj("SaveMoneyBox");
}

BOOL StorageFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

    m_bWalkWare = false;
	tagGameEvent event(_T("Close_Ware"), this);
	m_pMgr->SendEvent(&event);
	return TRUE;
}