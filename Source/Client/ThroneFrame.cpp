#include "StdAfx.h"
#include "ThroneFrame.h"
#include "ItemFrame.h"
#include "DragBtn.h"
#include "ItemMgr.h"
#include "Item.h"
#include "Container.h"
#include "ItemButton.h"
#include "ItemEvent.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "CurrencyMgr.h"
#include "PlayerNameTab.h"
#include "ItemProtoData.h"
#include "..\WorldDefine\ItemDefine.h"
#include "..\WorldDefine\currency_define.h"
#include "..\WorldDefine\msg_item.h"
#include "..\WorldDefine\msg_currency.h"
#include "..\WorldDefine\msg_func_npc.h"

const INT MAX_COL = 5;
const INT MAX_ROW = 4;
const SIZE UINT_SIZE = {36,36};
const SIZE SPACE_SIZE = {9,7};

ThroneFrame::ThroneFrame():m_Trunk(this)
{
	m_pWnd			= NULL;
	m_pWndCaption	= NULL;
	m_pBtnExit		= NULL;
	m_pBtnSwitch	= NULL;
	m_pStcList		= NULL;
	m_pWndList		= NULL;

	for(INT i=0; i<SPACE_BAIBAO_BAG; ++i)
	{
		m_pUnitItem[i] = NULL;
	}
}

ThroneFrame::~ThroneFrame()
{
	
}

BOOL ThroneFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
	
	m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_UpdateItem"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ThroneFrame::OnEventUpdateLPItem));
	m_pFrameMgr->RegisterEventHandle(_T("Baibao_Yuanbao"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ThroneFrame::OnEventYuanbaoChange));
	m_pFrameMgr->RegisterEventHandle(_T("MsgBox_LoadMoney"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ThroneFrame::OnEventLoadYuanbao));
	m_pFrameMgr->RegisterEventHandle(_T("ItemEvent_BaiBaoRecord"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ThroneFrame::OnEventBaiBaoRecord));
	m_pFrameMgr->RegisterEventHandle(_T("tagRoleGetNameEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ThroneFrame::OnEventRoleGetName));

	return bRet;
}

BOOL ThroneFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}

	m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_UpdateItem"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ThroneFrame::OnEventUpdateLPItem));
	m_pFrameMgr->UnRegisterEventHandler(_T("Baibao_Yuanbao"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ThroneFrame::OnEventYuanbaoChange));
	m_pFrameMgr->UnRegisterEventHandler(_T("MsgBox_LoadMoney"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ThroneFrame::OnEventLoadYuanbao));
	m_pFrameMgr->UnRegisterEventHandler(_T("ItemEvent_BaiBaoRecord"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ThroneFrame::OnEventBaiBaoRecord));
	m_pFrameMgr->UnRegisterEventHandler(_T("tagRoleGetNameEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ThroneFrame::OnEventRoleGetName));

	
	DestroyMsgBox();

	return bRet;
}

BOOL ThroneFrame::ReloadUI()
{
	// 创建
	tstring strPath = g_strLocalPath + _T("\\ui\\giftbag.xml");
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&ThroneFrame::EventHandler));

	m_pWndCaption = m_pWnd->GetChild(_T("gb_backpic"));

	m_pBtnSwitch = (GUIButton*)m_pWndCaption->GetChild(_T("closebutt2"));
	m_pBtnExit = (GUIButton*)m_pWndCaption->GetChild(_T("closebutt1"));
	m_pWndList = m_pWndCaption->GetChild(_T("gb_pic\\giftlistwin"));
	m_pStcList = (GUIStaticEx*)m_pWndList->GetChild(_T("giftlist"));
	m_pWndList->SetInvisible(TRUE);
	m_pStcYuanbao = (GUIStatic*)m_pWndCaption->GetChild(_T("money2"));
	m_pBtnLoading = (GUIButton*)m_pWndCaption->GetChild(_T("drawmoney"));
	INT i,j;
	m_pStcThrone = (GUIStatic*)m_pWndCaption->GetChild(_T("gb_pic\\gb_item"));
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
			m_pUnitItem[MAX_COL * j + i] = (ItemButton*)m_pGUI->CreateWnd(m_pStcThrone->GetFullName().c_str(), &ele);
		}
	}

	ReflashThrone();
	ReflashRecord();
	m_pBtnSwitch->SetText(g_StrTable[_T("BaiBaoText_Info")]);

	return TRUE;
}

DWORD ThroneFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnExit)//退出
			{
				m_pFrameMgr->AddToDestroyList(this);
			}
			else if (pWnd == m_pBtnSwitch)//物品/明细
			{
				BOOL bFlag = m_pStcThrone->IsInvisible();
				m_pStcThrone->SetInvisible(!bFlag);
				m_pWndList->SetInvisible(bFlag);
				if(bFlag)
					m_pBtnSwitch->SetText(g_StrTable[_T("BaiBaoText_Info")]);
				else
					m_pBtnSwitch->SetText(g_StrTable[_T("BaiBaoText_Item")]);
			}
			else if (pWnd == m_pBtnLoading)//取款
			{
				if(GetObj("LoadMoneyBox"))
					KillObj("LoadMoneyBox");

				CreateObj("LoadMoneyBox", "MsgMoneyInputBox");
				TObjRef<MsgMoneyInputBox>("LoadMoneyBox")->Init(
					_T(""), _T(""), _T("MsgBox_LoadMoney"), 
					0, CurrencyMgr::Inst()->GetBaibaoYuanbao(), 
					0, 0, MBMT_Yuanbao);
			}
			else if (_tcsnccmp(pWnd->GetName().c_str(), _T("unititem"), 8) == 0)//使用百宝袋中的物品
			{
				for (INT i = 0; i < SPACE_BAIBAO_BAG; ++i)
				{
					if (pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						OnItemPosChangeEx((INT16)i);
						ShowTip(NULL, NULL);
						break;
					}
				}
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("unititem"), 8) == 0)//使用百宝袋中的物品
			{
				for (INT i = 0; i < SPACE_BAIBAO_BAG; ++i)
				{
					if (pWnd == (GUIWnd*)m_pUnitItem[i])
					{
						Item* pItem = GetItem(i);
						if(P_VALID(pItem))
						{
							ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str());
						}
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
	default :
		break;
	}
	return 0;
}

VOID ThroneFrame::ReflashThrone()
{
	for (INT i=0; i<SPACE_BAIBAO_BAG; ++i)
	{
		UpdateItem(i);
	}

	UpdateYuanbao(CurrencyMgr::Inst()->GetBaibaoYuanbao());
	// 发送游戏事件让onlinetips刷新
	m_pFrameMgr->SendEvent(&tagGameEvent( _T("Refresh_Onlinetips"), NULL ));
}

Item* ThroneFrame::GetItem(INT16 index)
{
	return ItemMgr::Inst()->GetThrone()->GetValue(index);
}

VOID ThroneFrame::UpdateItem(INT16 index)
{
	DWORD dwTypeID = GT_INVALID;
	INT16 n16Quantity = 0;
	BYTE byQuality = 0;

	Item* pItem = GetItem(index);

	if (P_VALID(pItem))
	{
		dwTypeID = pItem->GetItemTypeID();
		n16Quantity = pItem->GetItemQuantity();
		byQuality = pItem->GetItemQuality();
	}
	
	m_pUnitItem[index]->RefreshItem(dwTypeID, n16Quantity, byQuality);
}

VOID ThroneFrame::OnItemPosChangeEx( INT16 n16Pos, EItemConType eDstCon /*= EICT_Bag*/, INT16 n16DstPos /*= GT_INVALID*/ )
{
	Item* pItem = GetItem(n16Pos);
	if(P_VALID(pItem))
	{
		tagNC_ItemPosChangeEx m;
		m.eConTypeSrc = EICT_Baibao;
		m.eConTypeDst = eDstCon;
		m.n64Serial1 = pItem->GetItemId();
		m.n16PosDst = n16DstPos;
		m_pSession->Send(&m);
	}
}

DWORD ThroneFrame::OnEventUpdateLPItem( tagUpdateLPItemEvent* pGameEvent )
{
	if(EICT_Baibao == pGameEvent->eType)
	{
		UpdateItem(pGameEvent->n16Index);
		// 发送游戏事件让onlinetips刷新
		m_pFrameMgr->SendEvent(&tagGameEvent( _T("Refresh_Onlinetips"), NULL ));
	}
	return 0;
}

VOID ThroneFrame::UpdateYuanbao( INT nYuanbao )
{
	TCHAR szTmp[X_SHORT_NAME];
	_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("%d"), nYuanbao);
	m_pStcYuanbao->SetText(szTmp);

	m_pStcYuanbao->SetRefresh(true);
}

DWORD ThroneFrame::OnEventYuanbaoChange( tagRoleYuanbaoChange* pGameEvent )
{
	UpdateYuanbao(pGameEvent->nYuanbao);
	return 0;
}

DWORD ThroneFrame::OnEventLoadYuanbao( tagMsgMoneyBoxEvent* pGameEvent )
{
	if (MBF_OK == pGameEvent->eResult)
	{
		if(pGameEvent->nYuanbao > 0)
		{
			tagNC_GetYuanBao m;
			m.nYuanBaoGet = pGameEvent->nYuanbao;
			m_pSession->Send(&m);
		}
	}
	return 0;
}

BOOL ThroneFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;
	
	m_pFrameMgr->AddToDestroyList(this);
	return FALSE;
}

void ThroneFrame::DestroyMsgBox()
{
	if(GetObj("LoadMoneyBox"))
		KillObj("LoadMoneyBox");
}

DWORD ThroneFrame::OnEventBaiBaoRecord( tagGameEvent* pGameEvent )
{
	ReflashRecord();
	return 0;
}

void ThroneFrame::ReflashRecord()
{
	// 获取记录列表
	TList<tagBaiBaoRecordC> listRecord;
	ItemMgr::Inst()->GetBaiBaoList(listRecord);

	if(listRecord.Size() <= 0)
		return;

	tstring strList = _T("");
	tagBaiBaoRecordC sRecord;
	// 从头写列表
	listRecord.ResetIterator();
	while(listRecord.PeekNext(sRecord))
	{
		// 物品名字
		tstring strItem = _T("");
		if(P_VALID(sRecord.dwTypeID))
		{
			const tagItemDisplayInfo* pItem = ItemProtoData::Inst()->FindItemDisplay(sRecord.dwTypeID);
			if(P_VALID(pItem))
			{
				strItem = pItem->szName;
			}
		}

		// 玩家名字
		tstring strRole = _T("");
		if(P_VALID(sRecord.dwRoleID))
		{
			strRole = PlayerNameTab::Inst()->FindNameByID(sRecord.dwRoleID);
			if(strRole.size() <= 0)
			{
				m_setRecordRoleNameReq.insert(sRecord.dwRoleID);
			}
		}

		// 操作时间
		TCHAR szTime[X_SHORT_NAME];
		_sntprintf(szTime, sizeof(szTime)/sizeof(TCHAR), g_StrTable[_T("BaiBaoRecord_Time")], 
			sRecord.sTime.year, sRecord.sTime.month, sRecord.sTime.day, sRecord.sTime.hour, sRecord.sTime.min);
		// 根据类型填写描述语言
		TCHAR szBuff[X_LONG_NAME];
		switch(sRecord.eType)
		{
		case EBBRT_System:
			{
				_sntprintf(szBuff, sizeof(szBuff)/sizeof(TCHAR), g_StrTable[_T("BaiBaoRecord_Text0")], strItem.c_str(), szTime);
			}
			break;
		case EBBRT_Mall:
			{
				_sntprintf(szBuff, sizeof(szBuff)/sizeof(TCHAR), g_StrTable[_T("BaiBaoRecord_Text1")], strItem.c_str(), szTime);
			}
			break;
		case EBBRT_Friend:
			{
				_sntprintf(szBuff, sizeof(szBuff)/sizeof(TCHAR), g_StrTable[_T("BaiBaoRecord_Text2")], strItem.c_str(), strRole.c_str(), szTime);
			}
			break;
		case EBBRT_Myself:
			{
				_sntprintf(szBuff, sizeof(szBuff)/sizeof(TCHAR), g_StrTable[_T("BaiBaoRecord_Text3")], strItem.c_str(), strRole.c_str(), szTime);
			}
			break;
		case EBBRT_GroupPurchase:
			{
				_sntprintf(szBuff, sizeof(szBuff)/sizeof(TCHAR), g_StrTable[_T("BaiBaoRecord_Text4")], strItem.c_str(), szTime);
			}
			break;
		}
		// 写入列表
		strList += szBuff;
		
		// 如果有赠言，则显示赠言
		if(sRecord.strWords.size() > 0)
		{
			_sntprintf(szBuff, sizeof(szBuff)/sizeof(TCHAR), g_StrTable[_T("BaiBaoRecord_Words")], sRecord.strWords.c_str());
			strList += szBuff;
		}
	}
	// 显示文字
	m_pStcList->SetText(strList.c_str());
	tagPoint size = m_pStcList->GetSize();
	if( m_pStcList->GetTextTotalHeight() > size.y )
	{
		size.y = m_pStcList->GetTextTotalHeight();
		m_pStcList->SetResizable(TRUE);
		m_pStcList->Resize( size );
		m_pStcList->SetResizable(FALSE);
	}
	
}

DWORD ThroneFrame::OnEventRoleGetName( tagRoleGetNameEvent* pGameEvent )
{
	// 删除相应缓存表中的内容
	m_setRecordRoleNameReq.erase(pGameEvent->dwRoleID);
	
	// 如果缓存列表为空，则刷新日志信息
	if(m_setRecordRoleNameReq.size() <= 0)
	{
		ReflashRecord();
	}
	return 0;
}