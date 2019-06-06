#include "StdAfx.h"
#include "PocketFrame.h"
#include "ItemFrame.h"
#include "DragBtn.h"
#include "ItemMgr.h"
#include "Item.h"
#include "Container.h"
#include "ItemButton.h"
#include "ItemMenuButton.h"
#include "ToolTipCreator.h"
#include "ItemEvent.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "CursorMgr.h"
#include "CurrencyMgr.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "QuestMgr.h"
#include "..\WorldDefine\ItemDefine.h"
#include "..\WorldDefine\currency_define.h"
#include "..\WorldDefine\msg_item.h"
#include "..\WorldDefine\shop_define.h"
#include "StallMgr.h"
#include "CombatSysUtil.h"
#include "PetDef.h"
#include "PetManager.h"
#include "StorageFrame.h"

const INT MAX_COL = 5;
const INT MAX_ROW = 4;
const SIZE UINT_SIZE = {36,36};
const SIZE SPACE_SIZE = {9,7};

PocketFrame::PocketFrame():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndCaption = NULL;
	m_pBtnThorne = NULL;
	m_pBtnSettle = NULL;
	m_pBtnEncrypt = NULL;
	m_pBtnExit = NULL;
	m_pBtnStall = NULL;

	for(INT i = 0; i < MAX_BAG_NUM; ++i)
	{
		m_pPbnPocketIndex[i] = NULL;
	}

	m_pStcTreasure = NULL;
	m_pStcGold = NULL;
	m_pStcSilver = NULL;

	m_pSliptItem = NULL;
	
	m_n16CurPage = 0;

	m_dwNewPsdCrc	= GT_INVALID;
	m_dwOldPsdCrc	= GT_INVALID;
}

PocketFrame::~PocketFrame()
{

}

BOOL PocketFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_UpdateItem"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnUpdateLPItem));
	m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_Move2Bag"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnItemPosChangeEvent));
	m_pFrameMgr->RegisterEventHandle(_T("Bag_Silver"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnBagSilverChange));
	m_pFrameMgr->RegisterEventHandle(_T("Bag_Yuanbao"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnBagYuanbaoChange));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_ItemNumber"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnInputItemNumber));
	m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_EquipChange"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnUpdateEQDataChange));
	m_pFrameMgr->RegisterEventHandle(_T("tagUpdateItemCDEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnUpdateItemCD));
	m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_BagExtend"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventBagExtend));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_SetPsdFirst"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventSetPsdFirst));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_SetPsdSecond"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventSetPsdSecond));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_ModPsdFirst"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventModPsdFirst));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_ModPsdSecond"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventModPsdSecond));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_ModPsdThird"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventModPsdThird));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_DelPsdFirst"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventDelPsdFirst));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_OpenBagPsd"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventOpenBagPsd));
	m_pFrameMgr->RegisterEventHandle(_T("Open_Encrypt"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnOpen_Encrypt));


	return bRet;
}

BOOL PocketFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}

	m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_UpdateItem"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnUpdateLPItem));
	m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_Move2Bag"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnItemPosChangeEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("Bag_Silver"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnBagSilverChange));
	m_pFrameMgr->UnRegisterEventHandler(_T("Bag_Yuanbao"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnBagYuanbaoChange));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_ItemNumber"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnInputItemNumber));
	m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_EquipChange"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnUpdateEQDataChange));
	m_pFrameMgr->UnRegisterEventHandler(_T("tagUpdateItemCDEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnUpdateItemCD));
	m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_BagExtend"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventBagExtend));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_SetPsdFirst"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventSetPsdFirst));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_SetPsdSecond"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventSetPsdSecond));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_ModPsdFirst"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventModPsdFirst));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_ModPsdSecond"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventModPsdSecond));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_ModPsdThird"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventModPsdThird));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_DelPsdFirst"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventDelPsdFirst));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_OpenBagPsd"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnEventOpenBagPsd));
	m_pFrameMgr->UnRegisterEventHandler(_T("Open_Encrypt"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&PocketFrame::OnOpen_Encrypt));

	DestroyMsgBox();

	return bRet;
}

BOOL PocketFrame::ReloadUI()
{
	// 创建
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\bag.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	
	INT i,j;
	for(i = 0; i < MAX_BAG_NUM + 1; ++i)
	{
		CHAR szTmp[X_LONG_NAME] = {0};
		_snprintf(szTmp, sizeof(szTmp), "bagback\\bagpic\\bagchoose\\bagchoose%d" ,i+1);
		m_pGUI->ChangeXml(&ele, szTmp, "ClassName", "ItemMenuButton");
	}

	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&PocketFrame::EventHandler));

	m_pWndCaption = m_pWnd->GetChild(_T("bagpic"));
	
	m_pBtnThorne = (GUIButton*)m_pWndCaption->GetChild(_T("boxbutt"));
	m_pBtnSettle = (GUIButton*)m_pWndCaption->GetChild(_T("settlebutt"));
	m_pBtnEncrypt = (GUIButton*)m_pWndCaption->GetChild(_T("encbutt"));
	m_pBtnExit = (GUIButton*)m_pWndCaption->GetChild(_T("new"));
	m_pBtnStall = (GUIButton*)m_pWndCaption->GetChild(_T("button"));
	
	for(i = 0; i < MAX_BAG_NUM + 1; ++i)
	{
		TCHAR szTmp[X_SHORT_NAME] = {0};
		_sntprintf(szTmp,sizeof(szTmp)/sizeof(TCHAR), _T("bagchoose\\bagchoose%d") ,i+1);
		m_pPbnPocketIndex[i] = (ItemMenuButton*)m_pWndCaption->GetChild(szTmp);
	}
	
	m_pStcTreasure = (GUIButton*)m_pWndCaption->GetChild(_T("yuanum"));
	m_pStcGold = (GUIButton*)m_pWndCaption->GetChild(_T("monum2"));
	m_pStcSilver = (GUIButton*)m_pWndCaption->GetChild(_T("monum1"));

	m_pStcPocket = (GUIStatic*)m_pWndCaption->GetChild(_T("bagpic1\\new"));
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
			m_pUnitItem[MAX_COL * j + i] = (ItemButton*)m_pGUI->CreateWnd(m_pStcPocket->GetFullName().c_str(), &ele);
		}
	}
	// 设置最大页数
	//UpdateMaxPage();
	// 翻到当前页
	ShowCurPage(m_n16CurPage);
	m_pPbnPocketIndex[m_n16CurPage]->SetState(EGUIBS_PushDown, FALSE);
	/*m_pBtnEncrypt->SetEnable(false);
	m_pBtnSettle->SetEnable(false);*/
	
	m_pWnd->SetInvisible(TRUE);

	return TRUE;
}

DWORD PocketFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnExit)//退出
			{
				m_pMgr->SendEvent(&tagGameEvent(_T("Close_Package_UI"), this));
			} 
			else if (pWnd == m_pBtnStall)//摆摊
			{
				m_pMgr->SendEvent(&tagGameEvent(_T("StallEvent_OpenWindow"), this));
			}
			else if (pWnd == m_pBtnEncrypt)//加密
			{
				GameFrame* pFrame = m_pFrameMgr->GetFrame(_T("PocketEncrypt"));
				if(!P_VALID(pFrame))
				{
					pFrame = m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("PocketEncrypt"), _T("PocketEncryptFrame"), 0);
				}
				else
				{
					m_pFrameMgr->AddToDestroyList(pFrame);
				}
			}
			else if (pWnd == m_pBtnThorne)//开启百宝箱
			{
				GameFrame* pFrame = m_pFrameMgr->GetFrame(_T("Throne"));
				if(!P_VALID(pFrame))
				{
					pFrame = m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("Throne"), _T("ThroneFrame"), 0);
				}
				else
				{
					m_pFrameMgr->AddToDestroyList(pFrame);
				}
				
			}
			else if (pWnd == m_pBtnSettle)//整理背包
			{
				ItemContainer* pItemCon = ItemMgr::Inst()->GetPocket();
				if(P_VALID(pItemCon))
				{
					pItemCon->AutoSettle();
				}
			}
			else if (_tcsnccmp(pWnd->GetName().c_str(), _T("unititem"), 8) == 0)//使用背包中的物品
			{
				for (INT i = 0; i < SPACE_ONE_BAG; ++i)
				{
					if (pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						TryToUseItem((INT16)(m_n16CurPage*SPACE_ONE_BAG + i));
						ShowTip(NULL, NULL);
						break;
					}
				}
			}
		}
		break;
	case EGUIE_Drag:
		{
			if (MAX_BAG_NUM == m_n16CurPage)//任务栏不可拖动
				return 0;

			if (_tcsnccmp(pWnd->GetName().c_str(), _T("unititem"), 8) == 0)//拖动背包中的物品
			{
				for (INT i = 0; i < SPACE_ONE_BAG; ++i)
				{
					if (pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						TryToDragItem(m_n16CurPage*SPACE_ONE_BAG + i, (GetKeyState(VK_SHIFT)&0x80)!=0);
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
			
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("bagchoose"), 9) == 0)//换背包
			{
				for (INT i = 0; i < MAX_BAG_NUM + 1; ++i)
				{
					if(pWnd == m_pPbnPocketIndex[i])
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
			if(_tcsnccmp(pWnd->GetName().c_str(), _T("unititem"), 8) == 0)
			{
				for(INT i = 0; i < SPACE_ONE_BAG; ++i)
				{
					if(pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						// 先判断该位置上是否有物品，如果有则要看物品是否可以被操作
						// 如果目标位置上的物品不能操作，则不能拖入物品
						Item* pItem = GetItem(i);
						if (P_VALID(pItem) && !pItem->IsOperable())
							return 0;

						if( EDT_Item == pEvent->dwParam1 )
						{
							OnItemChange((INT16)LOWORD(pEvent->dwParam2), (INT16)(m_n16CurPage*SPACE_ONE_BAG + i));
						}
						else if( EDT_ItemEquip == pEvent->dwParam1 )
						{
							OnUnEquipment((EEquipPos)LOWORD(pEvent->dwParam2), (INT16)(m_n16CurPage*SPACE_ONE_BAG + i));
						}
						else if( EDT_ItemWare == pEvent->dwParam1)
						{
							OnItemChangeEx(EICT_RoleWare, (INT16)LOWORD(pEvent->dwParam2), (INT16)(m_n16CurPage*SPACE_ONE_BAG + i));
						}
						else if( EDT_ItemSplit == pEvent->dwParam1)
						{
							OnItemChange((INT16)LOWORD(pEvent->dwParam2), (INT16)(m_n16CurPage*SPACE_ONE_BAG + i), (INT16)HIWORD(pEvent->dwParam2));
							//处理完才取消拖动
							DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
							ASSERT(P_VALID(pDrag));
							pDrag->EndDrag();
						}
						else if( EDT_ItemShop == pEvent->dwParam1)
						{
							OnBuyItem((INT)LOWORD(pEvent->dwParam2), pEvent->dwParam3, (INT16)HIWORD(pEvent->dwParam2));
						}
                        else if (EDT_PetEquip == pEvent->dwParam1)
                        {
                            OnPetUnEquipment(pEvent->dwParam2, (INT8)pEvent->dwParam3, (INT16)(m_n16CurPage*SPACE_ONE_BAG + i));
                        }
                        else if( EDT_GuildWare == pEvent->dwParam1)
                        {
                            OnItemChangeEx(EICT_GuildWare, (INT16)LOWORD(pEvent->dwParam2), (INT16)(m_n16CurPage*SPACE_ONE_BAG + i));
                        }
						break;
					}
				}
			}
			else if(_tcsnccmp(pWnd->GetName().c_str(), _T("bagchoose"), 9) == 0)
			{
				for (INT i = 0; i < MAX_BAG_NUM; ++i)
				{
					if(pWnd == m_pPbnPocketIndex[i])
					{
						INT16 n16PosDst = ItemMgr::Inst()->GetConBlank(EICT_Bag, i);
						if(!GT_VALID(n16PosDst))
							break;

						if( EDT_Item == pEvent->dwParam1 )
						{
							OnItemChange((INT16)LOWORD(pEvent->dwParam2), n16PosDst);
						}
						else if( EDT_ItemEquip == pEvent->dwParam1 )
						{
							OnUnEquipment((EEquipPos)LOWORD(pEvent->dwParam2), n16PosDst);
						}
						else if( EDT_ItemWare == pEvent->dwParam1)
						{
							OnItemChangeEx(EICT_RoleWare, (INT16)LOWORD(pEvent->dwParam2), n16PosDst);
						}
						else if( EDT_ItemSplit == pEvent->dwParam1)
						{
							OnItemChange((INT16)LOWORD(pEvent->dwParam2), n16PosDst, (INT16)HIWORD(pEvent->dwParam2));
						}
						break;
					}
				}
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if(_tcsnccmp(pWnd->GetName().c_str(), _T("unititem"), 8) == 0)
			{
				for(INT i = 0; i < SPACE_ONE_BAG; ++i)
				{
					if(pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						ShowItemTip(m_pUnitItem[i], (INT16)(m_n16CurPage*SPACE_ONE_BAG + i));
						break;
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
	case EGUISCB_UseSpecFunc:
		{
			if(_tcsnccmp(pWnd->GetName().c_str(), _T("unititem"), 8) == 0)
			{
				for(INT i = 0; i < SPACE_ONE_BAG; ++i)
				{
					if(pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						Item* pItemDst = GetItem((INT16)(m_n16CurPage*SPACE_ONE_BAG + i));
						if(P_VALID(pItemDst))
						{
							// 判断是否发出动作源对象是否是物品
							if(pEvent->dwParam1 > ECAN_ItemStart)
							{
								// 处理物品特殊操作
								Item* pItemSrc = ItemMgr::Inst()->GetPocketItem((INT16)pEvent->dwParam2);
								if(P_VALID(pItemSrc))
								{
									tagOnTheRoleOfItemEvent event(_T("ItemEvent_TargetItem"), this);
									event.eTypeSrc = pItemSrc->GetConType();
									event.n16IndexSrc = pItemSrc->GetPos();
									event.dwTypeIDSrc = pItemSrc->GetItemTypeID();
									event.eSpecFuncSrc = pItemSrc->GetItemSpecFunc();
									event.eTypeDst = pItemDst->GetConType();
									event.n16IndexDst = pItemDst->GetPos();
									event.dwTypeIDDst = pItemDst->GetItemTypeID();
									m_pFrameMgr->SendEvent(&event);
								}
							}
							else
							{
								// 处理其他特殊操作
							}
						}
						break;
					}
				}
			}
		}
	default :
		break;
	}
	return 0;
}

VOID PocketFrame::SetMoneyValue(INT64 n64Value)
{
	TCHAR szTmp[X_SHORT_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), n64Value/GOLD2SILVER);
	m_pStcGold->SetText(szTmp);
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), n64Value%GOLD2SILVER);
	m_pStcSilver->SetText(szTmp);

	m_pStcGold->SetRefresh(TRUE);
	m_pStcSilver->SetRefresh(TRUE);
}

VOID PocketFrame::SetTreasureValue(INT nValue)
{
	TCHAR szTmp[X_SHORT_NAME] = {0};
	_sntprintf(szTmp, sizeof(szTmp), _T("%d"), nValue);
	m_pStcTreasure->SetText(szTmp);

	m_pStcTreasure->SetRefresh(TRUE);
}

VOID PocketFrame::ShowCurPage(INT16 page)
{
	m_n16CurPage = page;
	for (INT i=0; i<SPACE_ONE_BAG; ++i)
	{
		RefreshItem(page * SPACE_ONE_BAG + i);
	}
}

Item* PocketFrame::GetItem(INT16 index)
{
	if (m_n16CurPage == MAX_BAG_NUM)
	{
		return ItemMgr::Inst()->GetQuest()->GetValue(index%SPACE_QUEST_BAG);
	}
	return ItemMgr::Inst()->GetPocket()->GetValue(index);
}

VOID PocketFrame::UpdateItem(INT16 index)
{
	if (IsCurPage(index))//如果为当前页则更新，不是当前页则不更新
	{
		RefreshItem(index);
	}
}

VOID PocketFrame::RefreshItem(INT16 index)
{
	DWORD dwTypeID = GT_INVALID;
	INT16 n16Quantity = 0;
	BYTE byQuality = 0;
	

	INT nMaxSize = ItemMgr::Inst()->GetPocket()->MaxSize();
	if(index >= nMaxSize && index < SPACE_ALL_BAG)
	{
		dwTypeID = 0;
	}
	else
	{
		Item* pItem = GetItem(index);
		if (P_VALID(pItem))
		{
			//获取物品TypeID
			dwTypeID = pItem->GetItemTypeID();
			//获取物品数量
			n16Quantity = pItem->GetItemQuantity();
			//获取物品品级
			byQuality = pItem->GetItemQuality();
		}
	}
	
	INT i = index%SPACE_ONE_BAG;
	m_pUnitItem[i]->RefreshItem(dwTypeID, n16Quantity, byQuality);
	if(m_pGUI->GetMousePoint() == m_pUnitItem[i])
	{
		ShowItemTip(m_pUnitItem[i], index);
	}
}

VOID PocketFrame::UpdateMaxPage()
{
	// 根据最大背包大小和单个背包大小算出最大背包个数
	INT nMaxSize = ItemMgr::Inst()->GetPocket()->MaxSize();
	INT nMaxPage = nMaxSize/SPACE_ONE_BAG;
	
	// 重新设置背包切换状态
	for(INT i=0; i<MAX_BAG_NUM; ++i)
	{
		m_pPbnPocketIndex[i]->SetEnable(i < nMaxPage);
	}
}

VOID PocketFrame::UpdateMoney()
{
	
}

VOID PocketFrame::OnItemChange(INT16 n16PosSrc, INT16 n16PosDst /* = GT_INVALID */, INT16 n16NumDst /* = 0 */)
{
	if(n16PosSrc == n16PosDst)
		return;

	Item* pItem = GetItem(n16PosSrc);
	if(P_VALID(pItem))
	{
		tagNC_ItemPosChange m;
		m.eConType = EICT_Bag;
		m.n16Num = n16NumDst;
		m.n16PosDst = n16PosDst;
		m.n64Serial = pItem->GetItemId();
		m_pSession->Send(&m);
	}
}

VOID PocketFrame::OnItemChangeEx( EItemConType ePosSrc, INT16 n16PosSrc, INT16 n16PosDst /*= GT_INVALID*/ )
{
	Item* pItem = ItemMgr::Inst()->GetConItem(ePosSrc, n16PosSrc);
	if(P_VALID(pItem))
	{
		tagNC_ItemPosChangeEx m;
		m.eConTypeDst = EICT_Bag;
		m.eConTypeSrc = ePosSrc;
		m.n16PosDst = n16PosDst;
		m.n64Serial1 = pItem->GetItemId();
		
		StorageFrame* pFrame = (StorageFrame*)(m_pFrameMgr->GetFrame( _T("Storage") ));
		if( P_VALID(pFrame) && pFrame->GetWalkWareMode() )
		{
			m.dwNPCID = GT_INVALID;
		}
		else if (ePosSrc == EICT_RoleWare || ePosSrc == EICT_GuildWare)
		{
			m.dwNPCID = QuestMgr::Inst()->GetTalkNPCGlobalID();
		}
		m_pSession->Send(&m);
	}
}


VOID PocketFrame::OnBuyItem( INT nShelfPos, DWORD dwTypeID, INT16 n16Num /*= 1*/ )
{
	tagBuyItemEvent event(_T("ItemEvent_Buy"), this);
	event.n16Pos = (INT16)nShelfPos;
	event.n16Num = n16Num;
	event.dwTypeID = dwTypeID;
	m_pFrameMgr->SendEvent(&event);
}

VOID PocketFrame::OnUnEquipment(EEquipPos ePos, INT16 n16PosDst /* = GT_INVALID */)
{
	LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
	if( !P_VALID(pLp) )
		return;

	//如果本地玩家在备战状态或使用技能不容许换装
	if( pLp->IsSkillState() )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ItemMsgError_CombatEquip")]);
		return;
	}
	else if( pLp->IsReadyState() )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ItemMsgError_ReadyEquip")]);
		return;
	}

	Equipment* pEquip = ItemMgr::Inst()->GetCurEquip(ePos);
	if(P_VALID(pEquip))
	{
		tagNC_Unequip m;
		m.n16PosDst = n16PosDst;
		m.n64Serial = pEquip->GetItemId();
		m_pSession->Send(&m);
	}
}

DWORD PocketFrame::OnUpdateLPItem( tagUpdateLPItemEvent* pGameEvent )
{
	if(EICT_Bag == pGameEvent->eType)
	{
		//如果是当前页则更新
		if(IsCurPage(pGameEvent->n16Index))
		{
			RefreshItem(pGameEvent->n16Index);
		}
	}
	else if(EICT_Quest == pGameEvent->eType)
	{
		if(m_n16CurPage == MAX_BAG_NUM)
		{
			RefreshItem(pGameEvent->n16Index);
		}
	}
	return 0;
}

DWORD PocketFrame::OnItemPosChangeEvent( tagItemPosChangeExEvent* pGameEvent )
{
	if(EICT_Bag == pGameEvent->eTypeDst || EICT_Quest == pGameEvent->eTypeDst)
	{
		if(EICT_Equip == pGameEvent->eTypeSrc)
		{
			OnUnEquipment((EEquipPos)pGameEvent->n16IndexSrc);
		}
		else 
		{
			OnItemChangeEx(pGameEvent->eTypeSrc, pGameEvent->n16IndexSrc);
		}
	}
	return 0;
}


DWORD PocketFrame::OnInputItemNumber( tagMsgInputBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult)
	{
		Item* pItem = m_pSliptItem;
		if(P_VALID(pItem)&& MBIT_Number == pGameEvent->eInputType)
		{
			INT16 n16Num = (INT16)pGameEvent->nInputNum;
			//数量不合法则填整堆数量
			if(n16Num < 0 || n16Num > pItem->GetItemQuantity())
			{
				n16Num = pItem->GetItemQuantity();
			}
			OnItemDrag(pItem, n16Num, true);
		}
	}
	m_pSliptItem = NULL;
	return 0;
}


DWORD PocketFrame::OnUpdateEQDataChange( tagUpdateLPItemEvent* pGameEvent )
{
	if(EICT_Bag == pGameEvent->eType && IsCurPage(pGameEvent->n16Index))
	{
		//如果是当前页中的物品，则更新物品Tips
		Item* pItem = ItemMgr::Inst()->GetPocketItem(pGameEvent->n16Index);
		if(P_VALID(pItem))
		{
			INT i = pGameEvent->n16Index%SPACE_ONE_BAG;
			ReflashItemTips(pItem, m_pUnitItem[i]);
			m_pUnitItem[i]->SetQuality(pItem->GetItemQuality());
		}
	}
	return 0;
}

VOID PocketFrame::ShowWnd( BOOL bShow )
{
	m_pWnd->SetInvisible(!bShow);
	if(bShow)
	{
		m_pWnd->SetRefresh();
		m_pWnd->FlipToTop();
	}
	else
	{
		DestroyChild();
		DestroyMsgBox();
	}
}

VOID PocketFrame::TryToUseItem( INT16 index )
{
	Item* pItem = GetItem(index);
	if(P_VALID(pItem))
	{
		if(!pItem->IsOperable())
			return;

		tagClickItemEvent event(_T("tagClickItemEvent"),this);
		event.eType = pItem->GetConType();
		event.n16Index = pItem->GetPos();
		event.dwTypeID = pItem->GetItemTypeID();
		m_pFrameMgr->SendEvent(&event);
	}
	
}


VOID PocketFrame::TryToDragItem( INT16 index, bool bShift )
{
	Item* pItem = GetItem(index);
	if(P_VALID(pItem))
	{	
		if(!pItem->IsOperable())
			return;

		INT16 n16Num = pItem->GetItemQuantity();
		//按下shift键则为物品拆分
		if(bShift)
		{
			// 只有
			if(pItem->IsStackable(n16Num))
			{
				if(P_VALID(GetObj("ItemNumber")))
					KillObj("ItemNumber");

				CreateObj("ItemNumber", "MsgInputBox");
				TObjRef<MsgInputBox>("ItemNumber")->Init(
					_T(""), g_StrTable[_T("MsgBoxText_InputSplitNumber")], _T("MsgBox_ItemNumber"), pItem->GetItemQuantity() );
				
				m_pSliptItem = pItem;
				return;
			}
		}

		OnItemDrag(pItem, n16Num);
	}
}

DWORD PocketFrame::OnBagSilverChange( tagRoleSilverChange* pGameEvent )
{
	SetMoneyValue(pGameEvent->n64Silver);
	return 0;
}

DWORD PocketFrame::OnBagYuanbaoChange( tagRoleYuanbaoChange* pGameEvent )
{
	SetTreasureValue(pGameEvent->nYuanbao);
	return 0;
}

VOID PocketFrame::OnItemDrag( Item* pItem, INT16 n16Num, bool bSplit )
{
	DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
	if (P_VALID(pDrag))
	{
		//发送游戏事件
		tagGUIEvent event(m_pUnitItem[pItem->GetPos()%SPACE_ONE_BAG], EGUIE_Drag, NULL);
		POINT pt;
		::GetCursorPos(&pt);
		event.dwParam1 = EDT_Item;
		event.dwParam2 = MAKELONG(pItem->GetPos(), n16Num);
		event.dwParam3 = pItem->GetItemTypeID();
		event.dwParam4 = MAKELONG(pt.x, pt.y);
		if(bSplit)
		{
			event.dwParam1 = EDT_ItemSplit;
		}
		pDrag->OnEvent(&event);
	}
}

VOID PocketFrame::Update()
{
	float deltaTime =  Cool3D::Kernel::Inst()->GetDeltaTime();

	GameFrame::Update();
}

VOID PocketFrame::ReflashItemTips(Item* pItem, GUIWnd* pWnd)
{
	//如果鼠标在天资技能按钮上，则重新显示Tips
	if( P_VALID(pItem) && pWnd == m_pGUI->GetMousePoint())
	{
		tstring strTips = ToolTipCreator::Inst()->GetItemTips(pItem);
		ShowTip(pWnd, strTips.c_str());
		if(MIsEquipment(pItem->GetItemTypeID()))
		{
			ToolTipCreator::Inst()->ShowEquipCompare(pItem->GetItemTypeID());
		}
	}
}

DWORD PocketFrame::OnUpdateItemCD( tagGameEvent* pGameEvent )
{
	ShowCurPage(m_n16CurPage);
	return 0;
}

DWORD PocketFrame::OnEventBagExtend( tagGameEvent* pGameEvent )
{
	ShowCurPage(m_n16CurPage);
	return 0;
}

DWORD PocketFrame::OnOpen_Encrypt(tagGameEvent* pGameEvent)
{
	GameFrame* pFrame = m_pFrameMgr->GetFrame(_T("PocketEncrypt"));
	if(!P_VALID(pFrame))
	{
		pFrame = m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("PocketEncrypt"), _T("PocketEncryptFrame"), 0);
	}
	return 0;
}

DWORD PocketFrame::OnEventSetPsdFirst( tagMsgInputBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult && MBIT_Password == pGameEvent->eInputType)
	{
		m_dwNewPsdCrc = m_pUtil->Crc32(pGameEvent->strInputName.c_str());

		if(P_VALID(GetObj("SetPsdSecond")))
			KillObj("SetPsdSecond");

		CreateObj("SetPsdSecond", "MsgInputBox");
		TObjRef<MsgInputBox>("SetPsdSecond")->Init(
			g_StrTable[_T("PocketEncrypt_CaptionSetup")],g_StrTable[_T("PocketEncrypt_TextNewPasswordAgain")],
			_T("MsgBox_SetPsdSecond"),GT_INVALID, MBIT_Password);
	}
	return 0;
}

DWORD PocketFrame::OnEventSetPsdSecond( tagMsgInputBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult && MBIT_Password == pGameEvent->eInputType)
	{
		if(m_dwNewPsdCrc == m_pUtil->Crc32(pGameEvent->strInputName.c_str()))
		{
			tagNC_SetBagPsd cmd;
			cmd.dwBagPsdCrc = m_dwNewPsdCrc;
			m_pSession->Send(&cmd);
			m_dwNewPsdCrc = GT_INVALID;
			// 发送完毕设置状态
			ItemMgr::Inst()->SetPocketEncryptState(ItemMgr::EPES_SetupPassword);
		}
		else
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ItemMsgError_DoublePsdDiff")]);

			if(P_VALID(GetObj("SetPsdFirst")))
				KillObj("SetPsdFirst");

			CreateObj("SetPsdFirst", "MsgInputBox");
			TObjRef<MsgInputBox>("SetPsdFirst")->Init(
				g_StrTable[_T("PocketEncrypt_CaptionSetup")],g_StrTable[_T("PocketEncrypt_TextNewPassword")],
				_T("MsgBox_SetPsdFirst"),GT_INVALID, MBIT_Password);
		}
	}
	return 0;
}

DWORD PocketFrame::OnEventModPsdFirst( tagMsgInputBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult && MBIT_Password == pGameEvent->eInputType)
	{
		m_dwOldPsdCrc = m_pUtil->Crc32(pGameEvent->strInputName.c_str());

		tagNC_OldBagPsd cmd;
		cmd.dwOldBagPsdCrc = m_dwOldPsdCrc;
		m_pSession->Send(&cmd);

		/*if(P_VALID(GetObj("ModPsdSecond")))
			KillObj("ModPsdSecond");

		CreateObj("ModPsdSecond", "MsgInputBox");
		TObjRef<MsgInputBox>("ModPsdSecond")->Init(
			g_StrTable[_T("PocketEncrypt_CaptionModify")],g_StrTable[_T("PocketEncrypt_TextNewPassword")],
			_T("MsgBox_ModPsdSecond"),GT_INVALID, MBIT_Password);*/
	}
	return 0;
}

DWORD PocketFrame::OnEventModPsdSecond( tagMsgInputBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult && MBIT_Password == pGameEvent->eInputType)
	{
		m_dwNewPsdCrc = m_pUtil->Crc32(pGameEvent->strInputName.c_str());

		if(P_VALID(GetObj("ModPsdThird")))
			KillObj("ModPsdThird");

		CreateObj("ModPsdThird", "MsgInputBox");
		TObjRef<MsgInputBox>("ModPsdThird")->Init(
			g_StrTable[_T("PocketEncrypt_CaptionModify")],g_StrTable[_T("PocketEncrypt_TextNewPasswordAgain")],
			_T("MsgBox_ModPsdThird"),GT_INVALID, MBIT_Password);
	}
	return 0;
}

DWORD PocketFrame::OnEventModPsdThird( tagMsgInputBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult && MBIT_Password == pGameEvent->eInputType)
	{
		if(m_dwNewPsdCrc == m_pUtil->Crc32(pGameEvent->strInputName.c_str()))
		{
			tagNC_ResetBagPsd cmd;
			cmd.dwNewBagPsdCrc = m_dwNewPsdCrc;
			cmd.dwOldBagPsdCrc = m_dwOldPsdCrc;
			m_pSession->Send(&cmd);

			m_dwNewPsdCrc = GT_INVALID;
			m_dwOldPsdCrc = GT_INVALID;
			// 发送完毕设置状态
			ItemMgr::Inst()->SetPocketEncryptState(ItemMgr::EPES_ModifyPassword);
		}
		else
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ItemMsgError_DoublePsdDiff")]);

			if(P_VALID(GetObj("ModPsdSecond")))
				KillObj("ModPsdSecond");

			CreateObj("ModPsdSecond", "MsgInputBox");
			TObjRef<MsgInputBox>("ModPsdSecond")->Init(
				g_StrTable[_T("PocketEncrypt_CaptionModify")],g_StrTable[_T("PocketEncrypt_TextNewPassword")],
				_T("MsgBox_ModPsdSecond"),GT_INVALID, MBIT_Password);
		}
	}
	return 0;
}

DWORD PocketFrame::OnEventDelPsdFirst( tagMsgInputBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult && MBIT_Password == pGameEvent->eInputType)
	{
		tagNC_UnsetBagPsd cmd;
		cmd.dwSafeCodeCrc = m_pUtil->Crc32(pGameEvent->strInputName.c_str());
		m_pSession->Send(&cmd);

		// 发送完毕设置状态
		ItemMgr::Inst()->SetPocketEncryptState(ItemMgr::EPES_CancelPassword);
	}
	return 0;
}

DWORD PocketFrame::OnEventOpenBagPsd( tagMsgInputBoxEvent* pGameEvent )
{
	if(MBF_OK == pGameEvent->eResult && MBIT_Password == pGameEvent->eInputType)
	{
		tagNC_OpenBagPsd cmd;
		cmd.dwBagPsdCrc = m_pUtil->Crc32(pGameEvent->strInputName.c_str());
		m_pSession->Send(&cmd);

		// 发送完毕设置状态
		ItemMgr::Inst()->SetPocketEncryptState(ItemMgr::EPES_EnterPassword);
	}
	return 0;
}

BOOL PocketFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

	m_pMgr->SendEvent(&tagGameEvent(_T("Close_Package_UI"), this));
	return TRUE;
}

VOID PocketFrame::DestroyChild()
{
	// 销毁所有子系统
	m_listChild.ResetIterator();
	GameFrame* pFrame = NULL;
	while(m_listChild.PeekNext(pFrame))
	{
		m_pMgr->AddToDestroyList(pFrame);
	}
}

VOID PocketFrame::DestroyMsgBox()
{
	if(P_VALID(GetObj("ItemNumber")))
		KillObj("ItemNumber");

	if(P_VALID(GetObj("SetPsdFirst")))
		KillObj("SetPsdFirst");

	if(P_VALID(GetObj("SetPsdSecond")))
		KillObj("SetPsdSecond");

	if(P_VALID(GetObj("ModPsdFirst")))
		KillObj("ModPsdFirst");

	if(P_VALID(GetObj("ModPsdSecond")))
		KillObj("ModPsdSecond");
	
	if(P_VALID(GetObj("ModPsdThird")))
		KillObj("ModPsdThird");

	if(P_VALID(GetObj("DelPsdFirst")))
		KillObj("DelPsdFirst");
}


VOID PocketFrame::OnPetUnEquipment( DWORD dwPetID, INT8 n8PebPos, INT16 n16PosDst /*= GT_INVALID*/ )
{
    const tagPetEquip* petEquip = PetManager::Inst()->FindPetEquip(dwPetID, n8PebPos);
    if (P_VALID(petEquip))
    {
        PetManager::Inst()->BeginNC_PetUnequip(dwPetID, petEquip->n64ItemID, n16PosDst);
    }
}

VOID PocketFrame::ShowItemTip( GUIWnd* pWnd, INT16 n16Index )
{
	Item* pItem = GetItem(n16Index);
	if(P_VALID(pItem))
	{
		tstring strPrice = _T("");
		if(EIST_ItemShopVend == ((ItemFrame*)m_pFather)->GetCurStrategy())
		{
			tagItemCost cost(pItem->IsVendible() ? pItem->GetItemPrice() : GT_INVALID, ECCT_Null, 0, 0, GT_INVALID);
			strPrice = ToolTipCreator::Inst()->GetPriceTips(&cost);
		}
		ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str(), strPrice.c_str());

		// 如果是装备显示装备比较
		if(MIsEquipment(pItem->GetItemTypeID()))
		{
			ToolTipCreator::Inst()->ShowEquipCompare(pItem->GetItemTypeID());
		}
	}
}